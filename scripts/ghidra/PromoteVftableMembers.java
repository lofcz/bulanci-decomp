// PromoteVftableMembers - reparent virtual methods into their RTTI class.
//
// Ghidra's RTTIAnalyzer recovers class hierarchy + vftables for an MSVC
// binary, but the functions whose addresses appear inside a vftable stay
// in the Global namespace. As a result `GenerateMapping.java` records
// almost everything as a bare `FUN_004xxxxx` and `seed_units_listing.py`
// has nothing to split on - all 4000 functions end up in the catch-all
// `_Globals` unit and decomp.dev sees just one giant TU.
//
// This script walks every `GhidraClass` namespace, looks for symbols
// whose name starts with "vftable" (Ghidra's convention for the
// class virtual function table), reads each function pointer out of the
// table, and sets the pointed-to Function's parent namespace to the
// class. Functions that have already been moved out of the Global
// namespace (manually renamed in Ghidra, multiple-inheritance shared
// thunks, etc.) are left untouched.
//
// Run as a headless analyzer post-script after RTTI analysis. The
// project may be opened read-only - the script will refuse to write
// in that case. For our `configure.py` pipeline we run it against the
// repo-root `bulanci.gpr` in writeable mode, then `GenerateMapping.java`
// re-exports `mapping.csv` with the new namespace prefixes.
//
//@category bulanci

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.data.Array;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.Pointer;
import ghidra.program.model.data.Structure;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.GhidraClass;
import ghidra.program.model.listing.Listing;
import ghidra.program.model.symbol.Namespace;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolIterator;
import ghidra.program.model.symbol.SymbolTable;

import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;

public class PromoteVftableMembers extends GhidraScript {

    @Override
    public void run() throws Exception {
        SymbolTable st = currentProgram.getSymbolTable();
        Listing listing = currentProgram.getListing();

        int classesVisited = 0;
        int vftablesScanned = 0;
        int reparented = 0;
        int alreadyOwned = 0;
        Set<Address> seenFunctionEntries = new HashSet<>();

        // Iterate every class namespace declared in the program. Classes
        // discovered by RTTI analysis appear here automatically.
        Iterator<GhidraClass> classIt = st.getClassNamespaces();
        while (classIt.hasNext()) {
            GhidraClass cls = classIt.next();
            classesVisited++;

            for (Symbol candidate : findVftableSymbols(st, cls)) {
                Data data = listing.getDefinedDataAt(candidate.getAddress());
                if (data == null) {
                    // Vftable symbol exists but Ghidra hasn't laid down a
                    // structured data definition there - cannot walk the
                    // slots. Skip.
                    continue;
                }
                vftablesScanned++;
                int components = countSlots(data);
                for (int i = 0; i < components; i++) {
                    Data field = data.getComponent(i);
                    if (field == null) {
                        continue;
                    }
                    Address target = extractFunctionAddress(field);
                    if (target == null) {
                        continue;
                    }
                    Function fn = listing.getFunctionAt(target);
                    if (fn == null || fn.isThunk()) {
                        continue;
                    }
                    if (!seenFunctionEntries.add(fn.getEntryPoint())) {
                        // Already processed via another vftable in this run.
                        continue;
                    }
                    Namespace parent = fn.getParentNamespace();
                    if (parent != null && !parent.isGlobal()) {
                        // Already in some non-global namespace - either
                        // a base class, a shared thunk, or a manual rename.
                        // Respect the existing assignment.
                        alreadyOwned++;
                        continue;
                    }
                    try {
                        fn.setParentNamespace(cls);
                        reparented++;
                    } catch (Exception e) {
                        printf("[%s] could not reparent %s -> %s: %s\n",
                               cls.getName(true), fn.getName(),
                               cls.getName(true), e.getMessage());
                    }
                }
            }
        }
        printf("PromoteVftableMembers: visited %d class(es), scanned %d vftable(s), reparented %d function(s), %d already owned by another namespace.\n",
               classesVisited, vftablesScanned, reparented, alreadyOwned);
    }

    /**
     * Symbols inside a class namespace whose name is or starts with
     * `vftable` (case-insensitive). Multiple-inheritance classes may
     * carry `vftable_0`, `vftable_1`, ...
     */
    private Iterable<Symbol> findVftableSymbols(SymbolTable st, GhidraClass cls) {
        java.util.List<Symbol> out = new java.util.ArrayList<>();
        SymbolIterator it = st.getSymbols(cls);
        while (it.hasNext()) {
            Symbol s = it.next();
            String n = s.getName().toLowerCase();
            if (n.startsWith("vftable") || n.startsWith("vtable")) {
                out.add(s);
            }
        }
        return out;
    }

    /**
     * Return the number of pointer-sized slots in `data`. Handles both
     * fixed-size arrays (`Array<Pointer>`) and ad-hoc structures.
     */
    private int countSlots(Data data) {
        DataType dt = data.getDataType();
        if (dt instanceof Array) {
            return ((Array) dt).getNumElements();
        }
        if (dt instanceof Structure) {
            return data.getNumComponents();
        }
        // Some vftables are laid down as a sequence of individual
        // pointer Data items without an enclosing array. getNumComponents
        // on a non-composite Data returns 0; that's fine, we'll just
        // skip and rely on neighbouring components being picked up by
        // future passes once Ghidra coalesces them.
        return data.getNumComponents();
    }

    /**
     * Extract the function entry address `field` points to, or null if
     * `field` is not a pointer-valued slot.
     */
    private Address extractFunctionAddress(Data field) {
        DataType dt = field.getDataType();
        if (!(dt instanceof Pointer)) {
            return null;
        }
        Object value = field.getValue();
        if (value instanceof Address) {
            return (Address) value;
        }
        return null;
    }
}
