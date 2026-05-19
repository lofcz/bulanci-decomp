// PromoteByConstantThis - reparent by call-site arg0 type aggregation.
//
// AssignByThisPointerType reads each function's STORED first-parameter
// type and reparents if it's a `CClass*`. That leaves on the table every
// function whose stored param_0 is still `undefined4 *` because Ghidra
// never propagated the type into the signature - even though every
// caller's HighFunction can see at the call site that arg0 is a
// `CClass*`. That's the gap this pass fills.
//
// Algorithm:
//   1. Decompile every non-thunk, non-external function once.
//   2. For every CALL pcode in every HighFunction, inspect the first
//      *argument* varnode (op.getInput(1)) and ask Ghidra for its
//      HighVariable's data type at the call site. Strip Pointer +
//      TypeDef wrappers to get the bare composite name.
//   3. Aggregate per-callee: callee -> { class -> count, total }.
//   4. If a callee is currently in the Global namespace, is named
//      `FUN_xxxx`, and >= MIN_DOMINANCE of its call sites agree on
//      one CClass*, reparent to that class.
//
// Conservative defaults: MIN_CALL_SITES = 2 (need at least two
// independent call sites to vote) and MIN_DOMINANCE = 1.0 (every call
// site must agree). Both are tunable per-run via Ghidra script args.
//
//@category bulanci

import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.decompiler.DecompileResults;
import ghidra.program.model.address.Address;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.Pointer;
import ghidra.program.model.data.Structure;
import ghidra.program.model.data.TypeDef;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.GhidraClass;
import ghidra.program.model.pcode.HighFunction;
import ghidra.program.model.pcode.HighVariable;
import ghidra.program.model.pcode.PcodeOp;
import ghidra.program.model.pcode.PcodeOpAST;
import ghidra.program.model.pcode.Varnode;
import ghidra.program.model.symbol.Namespace;
import ghidra.program.model.symbol.SymbolTable;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

public class PromoteByConstantThis extends GhidraScript {

    private static final int DECOMP_TIMEOUT_SEC = 20;

    @Override
    public void run() throws Exception {
        // ---- Args: min_call_sites, min_dominance ----------------------
        int minCallSites = 2;
        double minDominance = 1.0;
        try {
            String[] args = getScriptArgs();
            if (args.length > 0 && !args[0].isEmpty()) {
                minCallSites = Integer.parseInt(args[0]);
            }
            if (args.length > 1 && !args[1].isEmpty()) {
                minDominance = Double.parseDouble(args[1]);
            }
        } catch (Exception ex) {
            printerr("PromoteByConstantThis: bad args, using defaults ("
                    + ex.getMessage() + ")");
        }
        printf("PromoteByConstantThis: min_call_sites=%d min_dominance=%.2f%n",
               minCallSites, minDominance);

        // ---- Index class namespaces by bare leaf name. ----------------
        SymbolTable st = currentProgram.getSymbolTable();
        Map<String, GhidraClass> classByName = new HashMap<>();
        java.util.Iterator<GhidraClass> classIt = st.getClassNamespaces();
        while (classIt.hasNext()) {
            GhidraClass cls = classIt.next();
            String key = cls.getName();
            if (classByName.containsKey(key)) {
                // Ambiguous: two classes share a leaf name.  Mark unusable.
                classByName.put(key, null);
            } else {
                classByName.put(key, cls);
            }
        }

        // ---- Set up decompiler. ---------------------------------------
        DecompInterface decomp = new DecompInterface();
        DecompileOptions opts = new DecompileOptions();
        decomp.setOptions(opts);
        decomp.toggleCCode(false);
        decomp.toggleSyntaxTree(true);
        decomp.setSimplificationStyle("decompile");
        if (!decomp.openProgram(currentProgram)) {
            printerr("PromoteByConstantThis: cannot open program: "
                    + decomp.getLastMessage());
            return;
        }

        // ---- Pass 1: decompile every function, aggregate call sites. --
        // callee -> (className -> count)
        Map<Function, Map<String, Integer>> votes = new HashMap<>();
        Map<Function, Integer> totalSites = new HashMap<>();

        int functionsScanned = 0, callsiteSeen = 0;
        try {
            FunctionIterator fi = currentProgram.getListing().getFunctions(true);
            while (fi.hasNext()) {
                if (monitor.isCancelled()) break;
                Function caller = fi.next();
                if (caller.isThunk() || caller.isExternal()) continue;
                functionsScanned++;
                if (functionsScanned % 200 == 0) {
                    monitor.setMessage("PromoteByConstantThis: scanned "
                            + functionsScanned + " function(s)");
                }
                DecompileResults res = decomp.decompileFunction(caller,
                        DECOMP_TIMEOUT_SEC, monitor);
                if (res == null || !res.decompileCompleted()) continue;
                HighFunction hf = res.getHighFunction();
                if (hf == null) continue;

                Iterator<PcodeOpAST> ops = hf.getPcodeOps();
                while (ops.hasNext()) {
                    PcodeOpAST op = ops.next();
                    if (op.getOpcode() != PcodeOp.CALL) continue;
                    if (op.getNumInputs() < 2) continue;

                    // input[0] is the call target address.
                    Varnode targetVn = op.getInput(0);
                    if (targetVn == null || !targetVn.isAddress()) continue;
                    Function callee = currentProgram.getFunctionManager()
                            .getFunctionAt(targetVn.getAddress());
                    if (callee == null || callee.isThunk() || callee.isExternal()) continue;

                    Namespace ns = callee.getParentNamespace();
                    if (ns != null && !ns.isGlobal()) continue;
                    if (!callee.getName().startsWith("FUN_")) continue;

                    // input[1] is arg0.
                    Varnode arg0 = op.getInput(1);
                    if (arg0 == null) continue;
                    HighVariable hv = arg0.getHigh();
                    if (hv == null) continue;
                    DataType dt = hv.getDataType();
                    String className = classFromType(dt, classByName);

                    callsiteSeen++;
                    totalSites.merge(callee, 1, Integer::sum);
                    if (className != null) {
                        votes.computeIfAbsent(callee, k -> new HashMap<>())
                                .merge(className, 1, Integer::sum);
                    }
                }
            }
        } finally {
            decomp.dispose();
        }

        printf("PromoteByConstantThis: scanned %d function(s), inspected "
               + "%d call site(s), votes for %d distinct callee(s).%n",
               functionsScanned, callsiteSeen, votes.size());

        // ---- Pass 2: commit reparenting decisions. --------------------
        // Voting policy:
        //   * Need at least MIN_CALL_SITES *typed* votes for ONE class.
        //   * Need that class's votes / total-typed-votes >= MIN_DOMINANCE,
        //     i.e. no contradicting class wins among the typed sites.
        //   * Untyped call sites (arg0 isn't a class pointer at all) are
        //     ignored - they don't count for or against.
        int reparented = 0, tooFewSites = 0, ambiguous = 0;
        for (Map.Entry<Function, Map<String, Integer>> e : votes.entrySet()) {
            if (monitor.isCancelled()) break;
            Function callee = e.getKey();

            String bestName = null;
            int bestCount = 0;
            int totalNamedVotes = 0;
            for (Map.Entry<String, Integer> v : e.getValue().entrySet()) {
                totalNamedVotes += v.getValue();
                if (v.getValue() > bestCount) {
                    bestCount = v.getValue();
                    bestName = v.getKey();
                }
            }
            if (bestCount < minCallSites) {
                tooFewSites++;
                continue;
            }
            double dominance = (double) bestCount / (double) totalNamedVotes;
            if (dominance < minDominance) {
                ambiguous++;
                continue;
            }
            GhidraClass cls = classByName.get(bestName);
            if (cls == null) {
                ambiguous++;
                continue;
            }
            try {
                callee.setParentNamespace(cls);
                reparented++;
            } catch (Exception ex) {
                // refused - skip
            }
        }
        printf("PromoteByConstantThis: reparented=%d too_few_typed_sites=%d "
               + "ambiguous=%d%n",
               reparented, tooFewSites, ambiguous);
    }

    /**
     * Resolve a Pointer-to-Composite (possibly wrapped in TypeDefs) to
     * the bare class leaf name. Returns null if `dt` doesn't denote a
     * pointer to a known composite.
     */
    private String classFromType(DataType dt, Map<String, GhidraClass> classByName) {
        if (dt == null) return null;
        // Walk through typedefs at the outer level.
        for (int i = 0; i < 5 && dt instanceof TypeDef; i++) {
            dt = ((TypeDef) dt).getBaseDataType();
        }
        if (!(dt instanceof Pointer)) return null;
        DataType target = ((Pointer) dt).getDataType();
        for (int i = 0; i < 5 && target instanceof TypeDef; i++) {
            target = ((TypeDef) target).getBaseDataType();
        }
        if (!(target instanceof Structure)) return null;
        String name = target.getName();
        if (name == null || name.isEmpty()) return null;
        return classByName.containsKey(name) ? name : null;
    }
}
