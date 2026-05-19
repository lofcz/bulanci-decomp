// Apply FunctionDefinition data types from the SDK GDT categories to
// every matching external import symbol.
//
// Why this exists: `ParseHeadersToGdt.java` + `ApplyDxGdt.java`
// imported every struct, typedef *and* function prototype from the
// vendored DirectX and Win32 headers into the program's DTM. The
// function prototypes live under categories like
// `/directx_feb2007.i/functions/DirectDrawCreate` as
// `FunctionDefinition` data types. They were *not* attached to the
// program's external import symbols, so Ghidra still treats
// `DirectDrawCreate(...)` as a zero-arg, void-returning thunk - and
// the decompiler has no callee signature to propagate types from.
//
// This script wraps Ghidra's `ApplyFunctionDataTypesCmd`, pointing it
// at the SDK function-definition categories. The Cmd walks every
// FunctionDefinition in the category, looks up matching symbol names
// in the program, and stamps the signature onto the function with
// `SourceType.IMPORTED`. After this runs, the decompiler sees the
// real signatures for `DirectDrawCreate`, `RegOpenKeyExW`, etc. and
// CommitDecompilerSignatures can then propagate the typed args
// backward into the calling functions' parameter slots.
//
// Categories to scan are passed as `getScriptArgs()`. With no args
// the script applies every known SDK category (DirectX + Win32) on
// the assumption that the user just finished an `apply_gdt.py` run.
//
//@category bulanci

import ghidra.app.cmd.function.ApplyFunctionDataTypesCmd;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.Category;
import ghidra.program.model.data.CategoryPath;
import ghidra.program.model.data.DataTypeManager;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.symbol.SourceType;

import java.util.ArrayList;
import java.util.List;

public class ApplyImportSignatures extends GhidraScript {

    private static final String[] DEFAULT_CATEGORIES = new String[] {
        "/directx_feb2007.i/functions",
        "/dx_main.clean.i/functions",
        "/dx_main.i/functions",
        "/win32_msvc8.i/functions",
        "/win32_main.clean.i/functions",
        "/win32_main.i/functions",
    };

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        List<String> categories;
        if (args == null || args.length == 0) {
            categories = new ArrayList<>();
            for (String p : DEFAULT_CATEGORIES) categories.add(p);
        } else {
            categories = new ArrayList<>();
            for (String a : args) categories.add(a);
        }

        DataTypeManager dtm = currentProgram.getDataTypeManager();
        FunctionManager fnMgr = currentProgram.getFunctionManager();

        long beforeNamedParams = countNamedParams(fnMgr);
        long appliedCategories = 0;

        for (String catPath : categories) {
            Category cat = dtm.getCategory(new CategoryPath(catPath));
            if (cat == null) {
                println("  [skip] category not found: " + catPath);
                continue;
            }
            int fnDefs = cat.getDataTypes().length;
            if (fnDefs == 0) {
                println("  [skip] empty category: " + catPath);
                continue;
            }
            println("  [apply] " + catPath + " (" + fnDefs + " types)");

            ApplyFunctionDataTypesCmd cmd = new ApplyFunctionDataTypesCmd(
                cat,
                /*addresses=*/ null,
                SourceType.IMPORTED,
                /*alwaysReplace=*/ true,
                /*createBookmarksEnabled=*/ false);
            boolean ok = cmd.applyTo(currentProgram, monitor);
            if (!ok) {
                printerr("  [warn] ApplyFunctionDataTypesCmd returned false for "
                         + catPath + ": " + cmd.getStatusMsg());
            }
            appliedCategories++;
        }

        long afterNamedParams = countNamedParams(fnMgr);
        printf("ApplyImportSignatures: categories_applied=%d "
               + "named_params_before=%d named_params_after=%d delta=%d%n",
               appliedCategories, beforeNamedParams, afterNamedParams,
               afterNamedParams - beforeNamedParams);
    }

    /** Count how many *external* function parameters carry a name that
     *  isn't the Ghidra default (`paramN` / null). Used as a rough
     *  before/after measure of how many import signatures got real
     *  arguments. */
    private long countNamedParams(FunctionManager fm) {
        long n = 0;
        for (Function f : fm.getExternalFunctions()) {
            for (ghidra.program.model.listing.Parameter p : f.getParameters()) {
                String name = p.getName();
                if (name != null && !name.isEmpty()
                        && !name.startsWith("param_")
                        && !name.startsWith("arg")) {
                    n++;
                }
            }
        }
        return n;
    }
}
