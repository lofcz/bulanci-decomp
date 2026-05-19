// AssignByThisPointerType - reparent functions by their `this` argument.
//
// MSVC __thiscall member functions take `this` as their first
// parameter. After RTTI recovery + decompiler-driven type
// propagation, many of those functions have a parameter 0 with a
// recovered type of `CClass*` even though Ghidra never assigned the
// function itself to `CClass`.
//
// This pass picks them up: for every file-scope function whose
// parameter 0 is a pointer to some known class namespace, reparent
// the function to that class. This catches:
//
//   * Non-virtual methods that weren't in any vftable.
//   * Static-like helpers that take a `CClass*` first parameter as a
//     plain pointer (they would also belong on the class even if
//     declared `static`).
//   * Member operators that Ghidra named but didn't reparent.
//
// Conservative: only touches functions in the global namespace; never
// overrides an existing class membership.
//
//@category bulanci

import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.Pointer;
import ghidra.program.model.data.Structure;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.GhidraClass;
import ghidra.program.model.listing.Parameter;
import ghidra.program.model.symbol.Namespace;
import ghidra.program.model.symbol.SymbolTable;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

public class AssignByThisPointerType extends GhidraScript {

    @Override
    public void run() throws Exception {
        SymbolTable st = currentProgram.getSymbolTable();

        // Index classes by their bare leaf name. Two classes can
        // share a leaf name across different parents (e.g. `_Internal`
        // inside two different scopes), so disambiguate with a `null`
        // sentinel for clashes.
        Map<String, GhidraClass> classByName = new HashMap<>();
        Iterator<GhidraClass> classIt = st.getClassNamespaces();
        while (classIt.hasNext()) {
            GhidraClass cls = classIt.next();
            String key = cls.getName();
            if (classByName.containsKey(key)) {
                classByName.put(key, null);  // ambiguous
            } else {
                classByName.put(key, cls);
            }
        }
        printf("AssignByThisPointerType: %d class namespace(s) registered.\n",
               classByName.size());

        int considered = 0;
        int reparented = 0;
        int skippedNoMatch = 0;
        int skippedAmbiguous = 0;

        FunctionIterator fnIt = currentProgram.getListing().getFunctions(true);
        while (fnIt.hasNext()) {
            if (monitor.isCancelled()) {
                break;
            }
            Function fn = fnIt.next();
            if (fn.isThunk()) {
                continue;
            }
            Namespace ns = fn.getParentNamespace();
            if (ns == null || !ns.isGlobal()) {
                continue;
            }
            considered++;

            Parameter p0 = fn.getParameter(0);
            if (p0 == null) {
                continue;
            }
            // Walk through the parameter type looking for the
            // pointed-at composite. We accept either:
            //   CClass*               -> Pointer<Structure CClass>
            //   CClass               (rare, e.g. value-type self)
            DataType dt = unwrapPointer(p0.getDataType());
            if (dt == null) {
                continue;
            }
            String dtName = dt.getName();
            if (dtName == null || dtName.isEmpty()) {
                continue;
            }
            GhidraClass cls = classByName.get(dtName);
            if (cls == null) {
                if (classByName.containsKey(dtName)) {
                    skippedAmbiguous++;
                } else {
                    skippedNoMatch++;
                }
                continue;
            }
            try {
                fn.setParentNamespace(cls);
                reparented++;
            } catch (Exception e) {
                // setParentNamespace can refuse for C-linkage / extern
                // functions; ignore.
            }
        }
        printf("AssignByThisPointerType: %d global function(s) inspected, %d reparented, " +
               "%d had a non-class first parameter, %d skipped (ambiguous class name).\n",
               considered, reparented, skippedNoMatch, skippedAmbiguous);
    }

    /**
     * If `dt` is a Pointer, return its target stripped of further
     * pointer wrappers. Otherwise return `dt` itself if it looks like
     * a composite (Structure), or null. We deliberately do NOT follow
     * typedefs blindly - Ghidra's auto-renames sometimes wrap a class
     * in a `typedef <Class> XYZ_t` and that's not informative.
     */
    private DataType unwrapPointer(DataType dt) {
        if (dt == null) {
            return null;
        }
        if (dt instanceof Pointer) {
            DataType inner = ((Pointer) dt).getDataType();
            return inner;
        }
        if (dt instanceof Structure) {
            return dt;
        }
        return null;
    }
}
