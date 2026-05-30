// Apply CDS framework vtable global names + method renames from TSV.
//@category bulanci

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Listing;
import ghidra.program.model.symbol.SourceType;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

public class ApplyCdsVtableRenames extends GhidraScript {

    private static final class GlobalSpec {
        final String addr;
        final String name;
        final String typeName;
        final int arrayLen;
        final String plate;

        GlobalSpec(String addr, String name, String typeName, int arrayLen, String plate) {
            this.addr = addr;
            this.name = name;
            this.typeName = typeName;
            this.arrayLen = arrayLen;
            this.plate = plate;
        }
    }

    @Override
    public void run() throws Exception {
        Path repo = locateRepoRoot();
        Path tsv = repo.resolve("ghidra_analysis/engine/_cds_vtable_renames.tsv");
        if (!tsv.toFile().isFile()) {
            printerr("Missing TSV: " + tsv);
            return;
        }

        int fnOk = 0;
        int fnSkip = 0;
        int fnFail = 0;
        List<String> fnErrors = new ArrayList<>();

        Map<String, String> renames = loadRenames(tsv.toFile());
        Listing listing = currentProgram.getListing();

        for (Map.Entry<String, String> e : renames.entrySet()) {
            Address addr = toAddr(e.getKey());
            String newName = e.getValue();
            Function fn = listing.getFunctionAt(addr);
            if (fn == null) {
                fnFail++;
                fnErrors.add(addr + " no function @ " + newName);
                continue;
            }
            String old = fn.getName();
            if (old.equals(newName)) {
                fnSkip++;
                continue;
            }
            try {
                fn.setName(newName, SourceType.USER_DEFINED);
                fnOk++;
            } catch (Exception ex) {
                fnFail++;
                fnErrors.add(addr + " " + old + " -> " + newName + ": " + ex.getMessage());
            }
        }

        int gOk = 0;
        int gSkip = 0;
        int gFail = 0;
        List<String> gErrors = new ArrayList<>();

        for (GlobalSpec g : globalSpecs()) {
            Address addr = toAddr(g.addr);
            try {
                createLabel(addr, g.name, true, SourceType.USER_DEFINED);
                gOk++;
            } catch (Exception ex) {
                gFail++;
                gErrors.add(g.addr + " " + g.name + ": " + ex.getMessage());
            }
            // plate comment via existing symbol if possible
            try {
                setPlateComment(addr, g.plate);
            } catch (Exception ignored) {
            }
        }

        println("=== ApplyCdsVtableRenames ===");
        println("functions: applied=" + fnOk + " skipped=" + fnSkip + " failed=" + fnFail);
        for (String err : fnErrors) {
            println("  FN_ERR: " + err);
        }
        println("globals: applied=" + gOk + " skipped=" + gSkip + " failed=" + gFail);
        for (String err : gErrors) {
            println("  GL_ERR: " + err);
        }
    }

    private Map<String, String> loadRenames(File f) throws Exception {
        Map<String, String> out = new LinkedHashMap<>();
        try (BufferedReader br = new BufferedReader(new FileReader(f))) {
            String line = br.readLine(); // header
            String row;
            while ((row = br.readLine()) != null) {
                if (row.isBlank()) continue;
                String[] parts = row.split("\t");
                if (parts.length < 2) continue;
                out.put(parts[0].trim().toLowerCase(), parts[1].trim());
            }
        }
        return out;
    }

    private List<GlobalSpec> globalSpecs() {
        List<GlobalSpec> specs = new ArrayList<>();
        specs.add(new GlobalSpec("0x0048700c", "g_pCDSApp_vftable", "pointer[34]", 34,
            "CDSApp primary vftable (34 slots); COL@0x00487008"));
        specs.add(new GlobalSpec("0x0047f954", "g_pCDSView_vftable_primary", "CDSView_vftable_t *", 28,
            "CDSView primary vftable (28 slots)"));
        specs.add(new GlobalSpec("0x0047f834", "g_pCBulanci_vftable_primary", "pointer[34]", 34,
            "CBulanci app-shell primary vftable (34 slots)"));
        specs.add(new GlobalSpec("0x0047f6a8", "g_pCDSObject_vftable_IDSReferenced", "pointer[3]", 3,
            "CDSObject IDSReferenced MI face"));
        specs.add(new GlobalSpec("0x0047f6b8", "g_pCDSChain_vftable_IDSChained", "pointer[6]", 6,
            "CDSChain IDSChained MI face @+0x14"));
        specs.add(new GlobalSpec("0x0047f6d4", "g_pCDSChain_vftable_IDSReferenced", "pointer[3]", 3,
            "CDSChain IDSReferenced MI face @+0x18"));
        specs.add(new GlobalSpec("0x00489560", "g_pCDSWorkingThread_vftable_IDSReferenced", "pointer[3]", 3,
            "CDSWorkingThread IDSReferenced MI"));
        specs.add(new GlobalSpec("0x00489570", "g_pCDSWorkingThread_vftable_IDSChained", "pointer[5]", 5,
            "CDSWorkingThread IDSChained MI"));
        specs.add(new GlobalSpec("0x00486f9c", "g_pCDSBackBuffer_vftable_IDSEventHandler", "pointer[4]", 4,
            "CDSBackBuffer IDSEventHandler @+0x7c"));
        specs.add(new GlobalSpec("0x00486fb0", "g_pCDSBackBuffer_vftable_IDSReferenced", "pointer[3]", 3,
            "CDSBackBuffer IDSReferenced @+0x80"));
        specs.add(new GlobalSpec("0x0048725c", "g_pCDSFlxFile_vftable_resource", "pointer[5]", 5,
            "CDSFlxFile resource-name face"));
        specs.add(new GlobalSpec("0x00487274", "g_pCDSFlxFile_vftable_chain", "pointer[6]", 6,
            "CDSFlxFile IDSChained face"));
        specs.add(new GlobalSpec("0x00487290", "g_pCDSFlxFile_vftable_event", "pointer[4]", 4,
            "CDSFlxFile IDSEventHandler face"));
        specs.add(new GlobalSpec("0x004872a4", "g_pCDSFlxFile_vftable_meta", "pointer[10]", 10,
            "CDSFlxFile 10-slot meta face"));
        specs.add(new GlobalSpec("0x004872d0", "g_pCDSFlxFile_vftable_IDSReferenced", "pointer[3]", 3,
            "CDSFlxFile IDSReferenced face"));
        return specs;
    }

    private Path locateRepoRoot() {
        File here = sourceFile != null ? sourceFile.getFile(false) : null;
        if (here != null) {
            File parent = here.getParentFile();
            // <repo>/scripts/ghidra/ApplyCdsVtableRenames.java
            if (parent != null && "ghidra".equals(parent.getName())) {
                File scripts = parent.getParentFile();
                if (scripts != null && "scripts".equals(scripts.getName())) {
                    return scripts.getParentFile().toPath();
                }
            }
        }
        return Paths.get("C:/Users/mstagl-dev/Documents/GitHub/bulanci");
    }
}
