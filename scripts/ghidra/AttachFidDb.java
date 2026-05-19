// AttachFidDb - register a FidDb file with the user's Ghidra
// installation so subsequent runs of ApplyFidDb.java can use it.
//
// One-shot setup script. Reads the database path from the script
// arguments, calls FidFileManager.addUserFidFile, and prints whether
// the file is now registered. Idempotent.
//
// Usage (headless):
//   analyzeHeadless ... -preScript AttachFidDb.java "<path-to.fidb>"
//
//@category bulanci

import ghidra.app.script.GhidraScript;
import ghidra.feature.fid.db.FidFile;
import ghidra.feature.fid.db.FidFileManager;

import java.io.File;
import java.util.List;

public class AttachFidDb extends GhidraScript {

    @Override
    protected void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 1) {
            printerr("AttachFidDb: pass the absolute path to the .fidb as the " +
                     "first argument.");
            return;
        }
        File db = new File(args[0]);
        if (!db.isFile()) {
            printerr("AttachFidDb: file does not exist: " + db.getAbsolutePath());
            return;
        }

        FidFileManager mgr = FidFileManager.getInstance();
        List<FidFile> before = mgr.getFidFiles();
        boolean already = false;
        for (FidFile f : before) {
            if (db.getAbsolutePath().equals(f.getPath())) {
                already = true;
                break;
            }
        }
        if (already) {
            printf("AttachFidDb: %s already registered.\n", db.getAbsolutePath());
        } else {
            mgr.addUserFidFile(db);
            printf("AttachFidDb: registered %s.\n", db.getAbsolutePath());
        }
        printf("Active FidDb files now:\n");
        for (FidFile f : mgr.getFidFiles()) {
            printf("  %s%s\n", f.getPath(), f.isActive() ? "  (active)" : "  (inactive)");
        }
    }
}
