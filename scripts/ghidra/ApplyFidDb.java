// ApplyFidDb - apply attached FunctionID databases to default-named functions.
//
// FidDB lookup pairs each function in the program against a content-
// addressed hash of its instruction stream. For MSVC binaries this is
// excellent at naming CRT / STL / ATL / MFC entry points which would
// otherwise sit in `_Globals` as anonymous `FUN_xxxx`.
//
// FidDb files must already be registered with the user's Ghidra
// install. Use `scripts/setup_fiddb.py` (or `AttachFidDb.java`) once
// to do that. Without any active FidDb files this script prints a
// summary and exits.
//
// Conservative: only renames functions that are still default-named
// (FUN_/LAB_/entry-style) AND whose match set agrees on a single name
// (MatchNameAnalysis.getMostOptimisticCount() == 1). Ambiguous and
// already-named functions are left untouched.
//
//@category bulanci

import ghidra.app.script.GhidraScript;
import ghidra.feature.fid.db.FidFile;
import ghidra.feature.fid.db.FidFileManager;
import ghidra.feature.fid.db.FidQueryService;
import ghidra.feature.fid.service.FidMatch;
import ghidra.feature.fid.service.FidSearchResult;
import ghidra.feature.fid.service.FidService;
import ghidra.feature.fid.service.MatchNameAnalysis;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;

import java.util.List;

public class ApplyFidDb extends GhidraScript {

    @Override
    public void run() throws Exception {
        FidService service = new FidService();
        FidFileManager mgr = FidFileManager.getInstance();
        List<FidFile> available = mgr.getFidFiles();
        long active = 0;
        for (FidFile f : available) {
            if (f.isActive()) active++;
        }
        printf("ApplyFidDb: %d FidDb file(s) registered, %d active.\n",
               available.size(), active);
        if (available.isEmpty() || active == 0) {
            printf("  No active FidDb files. Run scripts/setup_fiddb.py first.\n");
            return;
        }
        if (!service.canProcess(currentProgram.getLanguage())) {
            printf("ApplyFidDb: no FidDb covers language %s. Skipping.\n",
                   currentProgram.getLanguage().getLanguageID());
            return;
        }

        FidQueryService query;
        try {
            query = mgr.openFidQueryService(currentProgram.getLanguage(), false);
        } catch (Exception e) {
            printf("ApplyFidDb: could not open FidQueryService: %s\n", e.getMessage());
            return;
        }

        int considered = 0;
        int applied = 0;
        int skippedAmbiguous = 0;
        int skippedNonDefault = 0;
        try {
            List<FidSearchResult> results = service.processProgram(
                    currentProgram, query,
                    service.getDefaultScoreThreshold(),
                    monitor);
            if (results == null) {
                printf("ApplyFidDb: processProgram returned null.\n");
                return;
            }
            for (FidSearchResult r : results) {
                if (monitor.isCancelled()) break;
                considered++;
                Function fn = r.function;
                if (fn == null || fn.isThunk()) {
                    continue;
                }
                if (!isDefaultName(fn)) {
                    skippedNonDefault++;
                    continue;
                }
                List<FidMatch> matches = r.matches;
                if (matches == null || matches.isEmpty()) {
                    continue;
                }
                MatchNameAnalysis mna = new MatchNameAnalysis();
                mna.analyzeNames(matches, currentProgram, monitor);
                if (mna.getMostOptimisticCount() != 1) {
                    skippedAmbiguous++;
                    continue;
                }
                String pick = mna.getMostOptimisticName();
                if (pick == null || pick.isEmpty()) {
                    continue;
                }
                try {
                    fn.setName(pick, SourceType.ANALYSIS);
                    applied++;
                    if (applied <= 20) {
                        printf("  %s -> %s\n", fn.getEntryPoint(), pick);
                    }
                } catch (Exception e) {
                    printf("  rename failed for %s -> %s: %s\n",
                           fn.getEntryPoint(), pick, e.getMessage());
                }
            }
        } finally {
            try { query.close(); } catch (Exception ignored) {}
        }
        printf("ApplyFidDb: %d candidate result(s), %d applied, %d skipped " +
               "(already-named), %d skipped (ambiguous matches).\n",
               considered, applied, skippedNonDefault, skippedAmbiguous);
    }

    private boolean isDefaultName(Function fn) {
        String n = fn.getName();
        if (n == null) return true;
        return n.startsWith("FUN_") || n.startsWith("LAB_") || n.startsWith("entry");
    }
}
