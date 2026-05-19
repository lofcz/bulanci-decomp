// DumpGdtTypes - print a summary of every top-level type stored in a
// Ghidra `.gdt` archive.  Used by build_dx_gdt.py's verification
// phase.
//
// Usage:
//   analyzeHeadless ... -preScript DumpGdtTypes.java <path.gdt>
//        [name-substring [name-substring ...]]
//
// If one or more `name-substring` arguments are given, the script
// reports the matching-type count for each and lists every match
// (helpful for verifying canary symbols).  Otherwise it just emits
// a one-line per-kind summary so the output stays tractable.
//
//@category bulanci

import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.FileDataTypeManager;

import java.io.File;
import java.util.Iterator;
import java.util.TreeMap;

public class DumpGdtTypes extends GhidraScript {

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 1) {
            printerr("Usage: DumpGdtTypes <path.gdt> [name-substring ...]");
            return;
        }
        File gdt = new File(args[0]);
        if (!gdt.exists()) {
            printerr("DumpGdtTypes: not found: " + gdt.getAbsolutePath());
            return;
        }
        java.util.List<String> filters = new java.util.ArrayList<>();
        for (int i = 1; i < args.length; i++) {
            filters.add(args[i]);
        }

        FileDataTypeManager dtm = FileDataTypeManager.openFileArchive(gdt, false);
        try {
            int total = 0;
            TreeMap<String, Integer> byKind = new TreeMap<>();
            TreeMap<String, Integer> filterCounts = new TreeMap<>();
            java.util.List<String> filterHits = new java.util.ArrayList<>();
            Iterator<DataType> it = dtm.getAllDataTypes();
            while (it.hasNext()) {
                DataType dt = it.next();
                total++;
                byKind.merge(dt.getClass().getSimpleName(), 1, Integer::sum);
                String path = dt.getPathName();
                for (String f : filters) {
                    if (path.contains(f)) {
                        filterCounts.merge(f, 1, Integer::sum);
                        filterHits.add(String.format("  %-40s  %s",
                                dt.getClass().getSimpleName(), path));
                    }
                }
            }
            printf("DumpGdtTypes: %s (%d types)%n",
                   gdt.getAbsolutePath(), total);
            for (java.util.Map.Entry<String, Integer> e : byKind.entrySet()) {
                printf("    %-30s  %d%n", e.getKey(), e.getValue());
            }
            for (String f : filters) {
                int n = filterCounts.getOrDefault(f, 0);
                printf("    filter[%s] -> %d match(es)%n", f, n);
            }
            for (String h : filterHits) {
                println(h);
            }
        } finally {
            dtm.close();
        }
    }
}
