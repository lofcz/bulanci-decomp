// ApplyDxGdt - import the DirectX `.gdt` produced by build_dx_gdt.py
// into the current program's data type manager.
//
// Usage (headless):
//   analyzeHeadless . bulanci -process bulanci.exe -noanalysis \
//       -scriptPath scripts/ghidra \
//       -postScript ApplyDxGdt.java <path-to-directx_feb2007.gdt>
//
// If no path is supplied the script defaults to
//   tools/dxsdk_feb2007/gdt/directx_feb2007.gdt
// resolved against the Ghidra project's parent directory (i.e. the
// repo root).
//
// The .gdt is opened read-only, every top-level data type is resolved
// into the program's DTM, and a one-line summary is printed.
//
//@category bulanci

import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.DataTypeConflictHandler;
import ghidra.program.model.data.DataTypeManager;
import ghidra.program.model.data.FileDataTypeManager;
import ghidra.program.model.data.SourceArchive;

import java.io.File;
import java.util.Iterator;

public class ApplyDxGdt extends GhidraScript {

    private static final String DEFAULT_REL_PATH =
            "tools/dxsdk_feb2007/gdt/directx_feb2007.gdt";

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        File gdtFile;
        if (args.length >= 1) {
            gdtFile = new File(args[0]);
        } else {
            gdtFile = new File(getProjectRoot(), DEFAULT_REL_PATH);
        }
        if (!gdtFile.exists()) {
            printerr("ApplyDxGdt: .gdt not found: " + gdtFile.getAbsolutePath());
            return;
        }

        FileDataTypeManager src = FileDataTypeManager.openFileArchive(gdtFile, false);
        DataTypeManager dst = currentProgram.getDataTypeManager();

        int txId = dst.startTransaction("Apply " + gdtFile.getName());
        int imported = 0;
        try {
            Iterator<DataType> it = src.getAllDataTypes();
            while (it.hasNext()) {
                if (monitor.isCancelled()) {
                    break;
                }
                DataType srcDt = it.next();
                // KEEP_HANDLER preserves any existing type the user
                // already added; ALIGN won't conflict because all
                // archived types were parsed under the same compile
                // spec as the program.
                dst.resolve(srcDt, DataTypeConflictHandler.KEEP_HANDLER);
                imported++;
            }

            // `resolve()` records a back-reference to the *source*
            // FileDataTypeManager (`SourceArchive`) on every imported
            // type, so Ghidra knows where they came from for diff /
            // sync workflows.  We don't want that link: the project
            // would otherwise pop an "Archive file not found" dialog
            // on every GUI open if the .gdt path on disk ever moves
            // (different machine, different worktree, etc.).
            //
            // `removeSourceArchive()` strips the SourceArchive record
            // from the program's DTM but keeps every imported type in
            // place.  We match both by UniversalID (the just-imported
            // one) and by name-stem (any stale entries left over from
            // previous runs whose UUID has since changed - this makes
            // the script idempotent in the face of a .gdt rebuild).
            String gdtStem = gdtFile.getName();
            int dot = gdtStem.lastIndexOf('.');
            if (dot > 0) gdtStem = gdtStem.substring(0, dot);

            int dropped = 0;
            SourceArchive byId = dst.getSourceArchive(src.getUniversalID());
            if (byId != null) {
                dst.removeSourceArchive(byId);
                dropped++;
            }
            // Iterate over a snapshot - removeSourceArchive mutates
            // the live list.
            for (SourceArchive sa : new java.util.ArrayList<>(dst.getSourceArchives())) {
                if (sa == byId) continue;
                String name = sa.getName();
                if (name != null && (name.equals(gdtStem)
                        || name.equals(gdtFile.getName())
                        || name.startsWith(gdtStem + "."))) {
                    dst.removeSourceArchive(sa);
                    dropped++;
                }
            }
            if (dropped > 0) {
                printf("ApplyDxGdt: dissociated %d source-archive link(s) "
                       + "for '%s' so the project won't try to reopen it.%n",
                       dropped, gdtFile.getName());
            }
        } finally {
            dst.endTransaction(txId, true);
            src.close();
        }
        printf("ApplyDxGdt: resolved %d type(s) from %s into program DTM.%n",
               imported, gdtFile.getAbsolutePath());
    }

    /** Best-effort: the project root is the parent of the .gpr file. */
    private File getProjectRoot() {
        try {
            String path = state.getProject().getProjectLocator()
                    .getProjectDir().getAbsolutePath();
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
