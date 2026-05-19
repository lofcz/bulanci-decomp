// DumpFunctionsCsv - export every function with size, namespace, and convention.
//
// Sister script to DumpVftablesCsv.java. Together their CSVs feed
// `scripts/internal/build_module_map.py`, which produces the
// `docs/MODULES.md` overview and `config/bulanci/modules.csv`.
//
// Output rows:
//   address;size;namespace;name;is_thunk;has_custom_name;calling_convention
//
//   * `address` and `size` are lowercase hex without the `0x` prefix.
//   * `namespace` is the function's fully qualified parent (`std::exception`
//     etc.) or empty for global scope.
//   * `name` is the bare leaf (`FUN_0040a000`, `_malloc`, ...).
//   * `is_thunk` is `1` when Function.isThunk() agrees.
//   * `has_custom_name` is `1` when the leaf is not one of Ghidra's
//     synthetic patterns (`FUN_`, `thunk_FUN_`, `Catch@`, `Unwind@`,
//     `switchD_`).
//   * `calling_convention` is the value from `getCallingConventionName()`
//     when available; empty string otherwise.
//
// Usage (headless):
//   analyzeHeadless ... -postscript DumpFunctionsCsv.java \
//       "config/bulanci/ghidra_functions_dump.csv"
//
// If the path argument is omitted it defaults to the same location.
//
//@category bulanci

import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.FunctionManager;

import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;

public class DumpFunctionsCsv extends GhidraScript {

    private static final String DEFAULT_OUT =
            "config/bulanci/ghidra_functions_dump.csv";

    @Override
    public void run() throws Exception {
        String outArg = (getScriptArgs().length >= 1)
                ? getScriptArgs()[0]
                : DEFAULT_OUT;
        File outFile = resolveOutput(outArg);
        outFile.getParentFile().mkdirs();

        FunctionManager fm = currentProgram.getFunctionManager();

        int totalFuncs = 0;
        long totalSize = 0;
        int globalCount = 0;
        long globalSize = 0;
        int thunkCount = 0;

        try (PrintWriter pw = new PrintWriter(new FileWriter(outFile))) {
            pw.println("address;size;namespace;name;is_thunk;has_custom_name;calling_convention");
            FunctionIterator fi = fm.getFunctions(true);
            while (fi.hasNext()) {
                if (monitor.isCancelled()) {
                    break;
                }
                Function f = fi.next();
                long addr = f.getEntryPoint().getOffset();
                long size = f.getBody().getNumAddresses();

                // Empty namespace string is the canonical encoding for
                // "Global"; `build_module_map.py` keys off that.
                String ns = f.getParentNamespace().getName(true);
                if ("Global".equals(ns)) {
                    ns = "";
                }
                String name = f.getName();
                boolean isThunk = f.isThunk();
                boolean customName = !(name.startsWith("FUN_")
                        || name.startsWith("thunk_FUN_")
                        || name.startsWith("Catch@")
                        || name.startsWith("Unwind@")
                        || name.startsWith("switchD_"));

                String cc;
                try {
                    cc = f.getCallingConventionName();
                } catch (Throwable t) {
                    cc = "";
                }

                pw.printf("%08x;%x;%s;%s;%d;%d;%s%n",
                          addr, size, ns, name,
                          isThunk ? 1 : 0,
                          customName ? 1 : 0,
                          cc == null ? "" : cc);

                totalFuncs++;
                totalSize += size;
                if (isThunk) {
                    thunkCount++;
                }
                if (ns.isEmpty()) {
                    globalCount++;
                    globalSize += size;
                }
            }
        }
        printf("DumpFunctionsCsv: %d function(s), %d total bytes; "
                       + "%d global-scope (%d bytes), %d thunk(s). Wrote %s.\n",
               totalFuncs, totalSize,
               globalCount, globalSize, thunkCount,
               outFile.getAbsolutePath());
    }

    /** Resolve a relative path against the project root. Absolute paths pass through. */
    private File resolveOutput(String pathArg) {
        File f = new File(pathArg);
        if (f.isAbsolute()) {
            return f;
        }
        return new File(getProjectRoot(), pathArg);
    }

    /** Best-effort: the project root is the parent of the .gpr file. */
    private File getProjectRoot() {
        try {
            String path = state.getProject().getProjectLocator().getProjectDir().getAbsolutePath();
            File p = new File(path).getParentFile();
            if (p != null) {
                return p;
            }
        } catch (Throwable ignored) {
            // headless writeable projects may not surface the locator
        }
        return new File(System.getProperty("user.dir"));
    }
}
