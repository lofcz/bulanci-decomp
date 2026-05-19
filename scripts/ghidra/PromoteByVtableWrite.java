// PromoteByVtableWrite - reparent constructors by their vftable store.
//
// In an MSVC __thiscall constructor, the canonical first action is to
// install the class's virtual function table into the object:
//
//     mov dword ptr [ecx], offset CClass::`vftable'
//
// `PromoteVftableMembers.java` walks each vftable and reparents the
// virtual METHODS listed inside it - but the constructor itself is not
// in any vftable, so it stays as `FUN_xxxx` in `_Globals`.
//
// This pass closes that gap. We index every known vftable address to
// its owning class, then decompile every globally-scoped `FUN_xxxx` and
// scan its high-pcode for the following pattern:
//
//     STORE [param_0], <constant equal to a known vftable address>
//
// where `param_0` is the function's first input (the `this` pointer).
// When we find such a store, the function is a constructor/destructor
// for whichever class owns the vftable and we reparent it.
//
// Notes:
//  * MSVC's destructor also writes a (possibly parent) vftable, so this
//    pass attributes constructors AND destructors to the right class.
//  * Multiple-inheritance classes write secondary vftables to
//    `*(this+N)`. We accept those by walking through PTRSUB / PTRADD
//    intermediates - the dominant primary vftable still anchors the
//    class.
//  * Only functions in the Global namespace named `FUN_xxxx` are
//    considered. Anything already hand-named or already in a class is
//    left untouched.
//
//@category bulanci

import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.GhidraClass;
import ghidra.program.model.pcode.HighFunction;
import ghidra.program.model.pcode.HighSymbol;
import ghidra.program.model.pcode.HighVariable;
import ghidra.program.model.pcode.LocalSymbolMap;
import ghidra.program.model.pcode.PcodeOp;
import ghidra.program.model.pcode.PcodeOpAST;
import ghidra.program.model.pcode.Varnode;
import ghidra.program.model.symbol.Namespace;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolIterator;
import ghidra.program.model.symbol.SymbolTable;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

public class PromoteByVtableWrite extends GhidraScript {

    private static final int DECOMP_TIMEOUT_SEC = 30;

    @Override
    public void run() throws Exception {
        SymbolTable st = currentProgram.getSymbolTable();

        // ----- 1. Index every vftable -> its owning class. -------------
        Map<Long, GhidraClass> vftableByAddr = new HashMap<>();
        Iterator<GhidraClass> classIt = st.getClassNamespaces();
        while (classIt.hasNext()) {
            GhidraClass cls = classIt.next();
            SymbolIterator si = st.getSymbols(cls);
            while (si.hasNext()) {
                Symbol s = si.next();
                String n = s.getName().toLowerCase();
                if (n.startsWith("vftable") || n.startsWith("vtable")) {
                    vftableByAddr.put(s.getAddress().getOffset(), cls);
                }
            }
        }
        printf("PromoteByVtableWrite: indexed %d vftable address(es).%n",
               vftableByAddr.size());

        // ----- 2. Set up the decompiler interface. ---------------------
        DecompInterface decomp = new DecompInterface();
        DecompileOptions opts = new DecompileOptions();
        decomp.setOptions(opts);
        decomp.toggleCCode(false);          // we only need pcode
        decomp.toggleSyntaxTree(true);
        decomp.toggleJumpLoads(false);
        decomp.setSimplificationStyle("decompile");
        if (!decomp.openProgram(currentProgram)) {
            printerr("PromoteByVtableWrite: cannot open program for decompilation: "
                    + decomp.getLastMessage());
            return;
        }

        // ----- 3. Walk every candidate FUN_xxxx and scan its pcode. ----
        int candidates = 0, reparented = 0, decompFailed = 0, noMatch = 0;
        // DEBUG: dump the pcode of the first N candidates so we can see
        // what the actual store/value chain looks like and refine the
        // matcher.
        int sniffHits = 0;
        try {
            FunctionIterator fi = currentProgram.getListing().getFunctions(true);
            while (fi.hasNext()) {
                if (monitor.isCancelled()) break;
                Function fn = fi.next();
                if (fn.isThunk() || fn.isExternal()) continue;
                // Eligibility: only consider Ghidra-default-named functions
                // still living in the global namespace.  Anything classified
                // elsewhere has been (intentionally) assigned by a previous
                // pass or a human; do not overwrite it.
                Namespace ns0 = fn.getParentNamespace();
                if (ns0 != null && !ns0.isGlobal()) continue;
                if (!fn.getName().startsWith("FUN_")) continue;
                candidates++;

                DecompileResults res = decomp.decompileFunction(fn, DECOMP_TIMEOUT_SEC, monitor);
                if (res == null || !res.decompileCompleted()) {
                    decompFailed++;
                    continue;
                }
                HighFunction hf = res.getHighFunction();
                if (hf == null) {
                    decompFailed++;
                    continue;
                }

                // Cheap pre-filter: does the function STORE a known
                // vftable anywhere?  If not we skip the (more expensive)
                // detailed match.
                boolean hasVftableRef = false;
                Iterator<PcodeOpAST> sniff = hf.getPcodeOps();
                while (sniff.hasNext()) {
                    PcodeOpAST op = sniff.next();
                    if (op.getOpcode() != PcodeOp.STORE) continue;
                    if (op.getNumInputs() < 3) continue;
                    Varnode v = unwrap(op.getInput(2));
                    if (v != null && v.isConstant() && vftableByAddr.containsKey(v.getOffset())) {
                        hasVftableRef = true;
                        sniffHits++;
                        break;
                    }
                }
                if (!hasVftableRef) {
                    noMatch++;
                    continue;
                }

                GhidraClass owner = findVtableWriteOwner(hf, vftableByAddr);
                if (owner == null) {
                    noMatch++;
                    continue;
                }
                try {
                    fn.setParentNamespace(owner);
                    reparented++;
                    if (reparented % 50 == 0) {
                        monitor.setMessage("PromoteByVtableWrite: "
                                + reparented + " reparented");
                    }
                } catch (Exception ex) {
                    printerr("PromoteByVtableWrite: cannot reparent "
                            + fn.getName() + " -> " + owner.getName(true)
                            + ": " + ex.getMessage());
                }
            }
        } finally {
            decomp.dispose();
        }

        printf("PromoteByVtableWrite: candidates=%d sniff_hits=%d reparented=%d "
               + "decomp_failed=%d no_match=%d%n",
               candidates, sniffHits, reparented, decompFailed, noMatch);
    }

    /**
     * Scan the high-pcode for STORE ops whose value is a known vftable
     * address.  Strategy in priority order:
     *
     *  1. If a STORE writes a vftable through a pointer that resolves
     *     to param_0 (this), return that vftable's owner immediately.
     *     This is the canonical constructor/destructor pattern.
     *  2. Otherwise, if the function writes ONE and only one distinct
     *     class's vftable(s) anywhere, attribute it to that class.
     *     Catches static factories, free-standing initialisers, and
     *     constructors where Ghidra failed to recover `this` as
     *     param_0.  Multi-class writers are rejected to avoid
     *     mis-attributing helpers that compose two objects.
     */
    private GhidraClass findVtableWriteOwner(HighFunction hf,
                                             Map<Long, GhidraClass> vftableByAddr) {
        LocalSymbolMap lsm = hf.getLocalSymbolMap();
        HighSymbol p0sym = lsm.getNumParams() > 0 ? lsm.getParamSymbol(0) : null;
        HighVariable p0hv = p0sym != null ? p0sym.getHighVariable() : null;

        GhidraClass singleClass = null;
        boolean multipleClasses = false;

        Iterator<PcodeOpAST> ops = hf.getPcodeOps();
        while (ops.hasNext()) {
            PcodeOpAST op = ops.next();
            if (op.getOpcode() != PcodeOp.STORE) continue;
            if (op.getNumInputs() < 3) continue;

            Varnode value = unwrap(op.getInput(2));
            if (value == null || !value.isConstant()) continue;
            GhidraClass owner = vftableByAddr.get(value.getOffset());
            if (owner == null) continue;

            if (p0hv != null && resolvesToParam0(op.getInput(1), p0hv)) {
                // Canonical match - constructors/destructors take this
                // path and the first hit is authoritative.
                return owner;
            }

            // Otherwise, remember the owner for the single-class
            // fallback at the end.
            if (singleClass == null) {
                singleClass = owner;
            } else if (singleClass != owner) {
                multipleClasses = true;
            }
        }

        if (singleClass != null && !multipleClasses) {
            return singleClass;
        }
        return null;
    }

    /**
     * Walk through trivial dataflow wrappers to expose the underlying
     * varnode that carries the real value:
     *
     *   * CAST / COPY: pass through to input[0].
     *   * PTRSUB(0, X) / PTRADD(0, X, _): Ghidra's typed representation
     *     of a constant pointer X; pass through to input[1].  This is
     *     the canonical encoding of "address-typed immediate" in high
     *     pcode and is what an MSVC `mov [ecx], offset vftable` ends
     *     up as.
     */
    private Varnode unwrap(Varnode v) {
        for (int hops = 0; hops < 8; hops++) {  // bounded to avoid cycles
            if (v == null || v.getDef() == null) return v;
            PcodeOp def = v.getDef();
            int code = def.getOpcode();
            if (code == PcodeOp.CAST || code == PcodeOp.COPY) {
                v = def.getInput(0);
                continue;
            }
            if (code == PcodeOp.PTRSUB || code == PcodeOp.PTRADD) {
                Varnode base = def.getInput(0);
                Varnode off = def.getInput(1);
                if (base != null && base.isConstant() && base.getOffset() == 0
                        && off != null && off.isConstant()) {
                    // PTRSUB(0, X) -> address-typed constant X.
                    return off;
                }
            }
            return v;
        }
        return v;
    }

    /**
     * Return true if `ptr` either directly is `param_0`'s HighVariable
     * or is computed from it via PTRSUB / PTRADD / INT_ADD with a
     * constant offset (which is how MSVC reaches secondary vftables in
     * multiple-inheritance layouts: `*(this + N)`).
     */
    private boolean resolvesToParam0(Varnode ptr, HighVariable p0hv) {
        if (ptr == null) return false;
        // Same HighVariable as param_0.
        HighVariable hv = ptr.getHigh();
        if (hv == p0hv) return true;

        // PTRSUB(this, 0) / PTRADD(this, 0, k) etc.
        PcodeOp def = ptr.getDef();
        if (def == null) return false;
        int code = def.getOpcode();
        if (code == PcodeOp.PTRSUB || code == PcodeOp.PTRADD
                || code == PcodeOp.INT_ADD || code == PcodeOp.CAST
                || code == PcodeOp.COPY) {
            // Recurse on the base operand.
            return resolvesToParam0(def.getInput(0), p0hv);
        }
        return false;
    }
}
