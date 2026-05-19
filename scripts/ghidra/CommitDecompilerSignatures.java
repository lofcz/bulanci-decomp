// Commit the decompiler's inferred parameters and return type onto
// every non-thunk function's stored signature.
//
// Why this exists: Ghidra's decompiler infers function parameter and
// return types from data-flow analysis every time you decompile a
// function (the "High Function" view). Those inferences are *not*
// persisted to the function's stored parameter list - so a fresh
// run of GenerateMapping.java will still see the bare
// `void __thiscall FUN_xxx(void *this, uint param_1)` signature that
// was created at function-creation time, even though the decompiler
// view shows fully typed parameters.
//
// This script walks every non-thunk function, runs the decompiler,
// and calls `HighFunctionDBUtil.commitParamsToDatabase()` to push the
// inferred parameters + return type onto the stored signature with
// SourceType.ANALYSIS. Subsequent runs of GenerateMapping.java then
// see the typed signature, which flows into mapping.csv and the
// generated stubs.
//
// Safe to re-run: skips functions whose signature SourceType is
// already USER_DEFINED or IMPORTED, so manual edits (renamed params,
// custom signatures) are preserved.
//
// Optional arg: a single integer limit that caps the number of
// functions processed (useful for smoke tests). Default = process all.
//
//@category bulanci

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.pcode.HighFunction;
import ghidra.program.model.pcode.HighFunctionDBUtil;
import ghidra.program.model.pcode.HighFunctionDBUtil.ReturnCommitOption;
import ghidra.program.model.symbol.SourceType;

public class CommitDecompilerSignatures extends GhidraScript {

    private static final int DECOMPILE_TIMEOUT_SEC = 30;

    @Override
    public void run() throws Exception {
        long limit = Long.MAX_VALUE;
        String[] args = getScriptArgs();
        if (args != null && args.length > 0 && !args[0].isBlank()) {
            try {
                limit = Long.parseLong(args[0]);
            } catch (NumberFormatException nfe) {
                printerr("CommitDecompilerSignatures: ignoring non-numeric limit '"
                         + args[0] + "'.");
            }
        }

        DecompInterface decomp = new DecompInterface();
        DecompileOptions opts = new DecompileOptions();
        decomp.setOptions(opts);
        // Match the default decompiler view: keep parameter ID on so the
        // High function carries the same types the user sees in the GUI.
        decomp.toggleCCode(false);
        decomp.toggleSyntaxTree(true);
        decomp.toggleParamMeasures(true);
        decomp.setSimplificationStyle("decompile");

        boolean opened = decomp.openProgram(currentProgram);
        if (!opened) {
            throw new RuntimeException("DecompInterface failed to open program: "
                                       + decomp.getLastMessage());
        }

        long total = 0, attempted = 0, committed = 0, skipped = 0, failed = 0;

        try {
            FunctionIterator fi = currentProgram.getListing().getFunctions(true);
            while (fi.hasNext()) {
                if (monitor.isCancelled()) break;
                if (committed >= limit) break;
                Function fn = fi.next();
                total++;

                if (fn.isThunk() || fn.isExternal()) {
                    skipped++;
                    continue;
                }
                SourceType sig = fn.getSignatureSource();
                if (sig == SourceType.USER_DEFINED || sig == SourceType.IMPORTED) {
                    // Preserve manually-edited / imported signatures.
                    skipped++;
                    continue;
                }

                attempted++;
                if (attempted % 200 == 0) {
                    monitor.setMessage(String.format(
                        "CommitDecompilerSignatures: %d attempted, %d committed, %d failed",
                        attempted, committed, failed));
                }

                DecompileResults res;
                try {
                    res = decomp.decompileFunction(fn, DECOMPILE_TIMEOUT_SEC, monitor);
                } catch (Throwable t) {
                    failed++;
                    continue;
                }
                if (res == null || !res.decompileCompleted()) {
                    failed++;
                    continue;
                }
                HighFunction hf = res.getHighFunction();
                if (hf == null) {
                    failed++;
                    continue;
                }

                try {
                    HighFunctionDBUtil.commitParamsToDatabase(
                        hf,
                        /*useDataTypes=*/ true,
                        ReturnCommitOption.COMMIT_NO_VOID,
                        SourceType.ANALYSIS);
                    committed++;
                } catch (Throwable t) {
                    failed++;
                }
            }
        } finally {
            decomp.dispose();
        }

        printf("CommitDecompilerSignatures: scanned=%d attempted=%d "
               + "committed=%d skipped=%d failed=%d%n",
               total, attempted, committed, skipped, failed);
    }
}
