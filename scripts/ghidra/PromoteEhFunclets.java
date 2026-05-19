// Reparent every MSVC `Catch@xxxx` / `Unwind@xxxx` EH funclet into
// the namespace of its owning function.
//
// Why this exists: MSVC's /EHsc generates a small helper function for
// every `catch` block and unwind action and names them `Catch@<addr>`
// / `Unwind@<addr>`. Ghidra's auto-analysis recognises them as
// real functions but leaves them in the Global namespace, so they
// dominate the `_Globals` super-unit even though they semantically
// belong to whichever function originally declared the try / catch.
//
// Ideally we'd walk the `_FuncInfo` / `_UnwindMapEntry` /
// `_HandlerType` tables in `.data` and follow back to the parent's
// prolog (the canonical mapping). In practice Ghidra hasn't typed
// those tables as proper structures on this binary, so the
// cross-reference chain is broken. We fall back to MSVC8's well-
// documented code layout: EH funclets are emitted into the same
// `.text` section *immediately after* (or within a few bytes of) the
// parent function's `ret`.
//
// Algorithm:
//   1. Snapshot all functions sorted by entry address.
//   2. For each Catch@/Unwind@ at address X, walk *backward* through
//      the sorted array skipping over other Catch@/Unwind@/external
//      stubs.  The first real function we land on becomes the parent.
//   3. Require parent.end+MAX_GAP >= X. EH funclets sit immediately
//      after the parent's last instruction (possibly with 0-15 bytes
//      of alignment padding); a much larger gap means we've crossed
//      into a different function's territory.
//   4. Attribute the funclet to parent's namespace. If parent itself
//      is in Global the funclet stays in Global - that's fine, the
//      goal is to bind the funclet to its anchor, not to invent a
//      namespace.
//
// Reset semantics: if the funclet is *already* under a non-Global
// namespace this script reattributes it whenever the spatial heuristic
// disagrees. That makes the pass idempotent in the face of a
// previous-version mis-attribution (e.g. an over-greedy heuristic
// that latched onto a far-away classed function).
//
// Outputs an audit CSV at `config/bulanci/eh_funclet_parents.csv`.
//
//@category bulanci

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.symbol.Namespace;

import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;

public class PromoteEhFunclets extends GhidraScript {

    private static final String DEFAULT_CSV = "config/bulanci/eh_funclet_parents.csv";

    /** Max gap (in bytes) tolerated between parent's end and funclet's
     *  entry. MSVC8 pads functions to 16-byte boundaries and may insert
     *  a small thunk between siblings - 64 bytes is comfortably above
     *  that and well below "we crossed into another function". */
    private static final long MAX_GAP = 0x40;

    @Override
    public void run() throws Exception {
        FunctionManager fm = currentProgram.getFunctionManager();

        List<Function> all = new ArrayList<>();
        FunctionIterator fi = fm.getFunctions(true);
        while (fi.hasNext()) all.add(fi.next());
        all.sort(Comparator.comparing(Function::getEntryPoint));

        File outFile = new File(getProjectRoot(), DEFAULT_CSV);
        outFile.getParentFile().mkdirs();
        PrintWriter csv = new PrintWriter(new FileWriter(outFile));
        csv.println("funclet_addr,funclet_name,parent_addr,parent_namespace,parent_name,gap_bytes,action");

        int candidates = 0;
        int reparentedToClass = 0;   // funclet moved to a class namespace
        int leftInGlobal = 0;        // parent itself is in Global, nothing to do
        int reset = 0;               // wrong prior attribution undone
        int noParent = 0;            // no preceding non-funclet function within MAX_GAP

        Namespace globalNs = currentProgram.getGlobalNamespace();

        for (int i = 0; i < all.size(); i++) {
            if (monitor.isCancelled()) break;
            Function fn = all.get(i);
            String name = fn.getName();
            if (!name.startsWith("Catch@") && !name.startsWith("Unwind@")) continue;
            candidates++;

            // Step 1: always reset to Global first - makes the pass
            // fully idempotent in the face of prior (possibly wrong)
            // attributions. We'll re-attribute below if the spatial
            // heuristic finds a confident match.
            Namespace currentNs = fn.getParentNamespace();
            boolean wasClassed = currentNs != null && !currentNs.isGlobal();
            if (wasClassed) {
                try {
                    fn.setParentNamespace(globalNs);
                    reset++;
                } catch (Exception ex) {
                    printerr("PromoteEhFunclets: cannot reset "
                            + fn.getName() + " (" + ex.getMessage() + ")");
                }
            }

            // Step 2: find nearest preceding non-funclet, non-external
            // function.
            Function parent = null;
            for (int j = i - 1; j >= 0; j--) {
                Function f = all.get(j);
                if (f.isExternal()) continue;
                String fname = f.getName();
                if (fname.startsWith("Catch@") || fname.startsWith("Unwind@")) continue;
                parent = f;
                break;
            }
            if (parent == null) {
                noParent++;
                continue;
            }

            long pEnd = parent.getBody().getMaxAddress().getOffset();
            long fEntry = fn.getEntryPoint().getOffset();
            long gap = fEntry - pEnd;
            if (gap < 0 || gap > MAX_GAP) {
                noParent++;
                continue;
            }

            Namespace targetNs = parent.getParentNamespace();
            if (targetNs == null || targetNs.isGlobal()) {
                leftInGlobal++;
                csv.printf("%08x,%s,%08x,%s,%s,%d,%s%n",
                        fEntry, name, parent.getEntryPoint().getOffset(),
                        "<global>", parent.getName(), gap, "anchor-global");
                continue;
            }

            try {
                fn.setParentNamespace(targetNs);
                reparentedToClass++;
                String action = wasClassed ? "reattribute" : "attribute";
                csv.printf("%08x,%s,%08x,%s,%s,%d,%s%n",
                        fEntry, name, parent.getEntryPoint().getOffset(),
                        targetNs.getName(true), parent.getName(), gap, action);
            } catch (Exception ex) {
                noParent++;
                printerr("PromoteEhFunclets: skip "
                        + fn.getName() + " -> " + targetNs.getName(true)
                        + " (" + ex.getClass().getSimpleName() + ": "
                        + ex.getMessage() + ")");
            }

            if ((reparentedToClass + leftInGlobal) % 200 == 0) {
                monitor.setMessage("PromoteEhFunclets: "
                        + reparentedToClass + " to class, "
                        + leftInGlobal + " anchored in global");
            }
        }
        csv.close();

        printf("PromoteEhFunclets: candidates=%d reparented_to_class=%d "
               + "anchored_in_global=%d reset=%d no_parent=%d%n",
               candidates, reparentedToClass, leftInGlobal, reset, noParent);
        printf("PromoteEhFunclets: audit -> %s%n", outFile.getAbsolutePath());
    }

    private File getProjectRoot() {
        String prop = System.getProperty("bulanci.repo");
        if (prop != null) {
            File f = new File(prop);
            if (f.isDirectory()) return f;
        }
        try {
            File scriptFile = getSourceFile().getFile(true);
            File d = scriptFile.getParentFile();
            for (int i = 0; i < 6 && d != null; i++, d = d.getParentFile()) {
                if (new File(d, "config/bulanci").isDirectory()) return d;
            }
        } catch (Throwable ignored) {}
        File cwd = new File(System.getProperty("user.dir"));
        for (int i = 0; i < 6 && cwd != null; i++, cwd = cwd.getParentFile()) {
            if (new File(cwd, "config/bulanci").isDirectory()) return cwd;
        }
        return new File(System.getProperty("user.dir"));
    }
}
