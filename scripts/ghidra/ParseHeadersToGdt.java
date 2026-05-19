// ParseHeadersToGdt - feed a preprocessed C/C++ header (.i / .h) to
// Ghidra's bundled C parser and save the resulting type universe as a
// `.gdt` File-Backed Data Type Archive.
//
// Args:
//   ${args[0]}  output .gdt path (will be (re)created)
//   ${args[1]}  first header / preprocessed-file to parse
//   ${args[2..N]} optional additional header paths or include dirs
//
// Designed to be called from `scripts/build_dx_gdt.py` after that
// driver has invoked `cl.exe /E` on a wrapper .c that pulls in the
// DirectX SDK headers.
//
// We deliberately route through `CParserUtils.parseHeaderFiles`
// instead of the lower-level `CParser` because the utility wires up:
//   * include-path resolution for any paths in `filenames` that
//     resolve to directories,
//   * RootDataTypeManager scaffolding for the output .gdt,
//   * a sane progress message channel via the supplied TaskMonitor.
//
// Note: the input file should already be flattened by `cl /E`; we
// pass an empty preprocessor-args array, so anything you `#include`
// would have to live next to the input file.
//
//@category bulanci

import ghidra.app.script.GhidraScript;
import ghidra.app.util.cparser.C.CParserUtils;
import ghidra.program.model.data.DataTypeManager;
import ghidra.program.model.data.FileDataTypeManager;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class ParseHeadersToGdt extends GhidraScript {

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) {
            printerr("Usage: ParseHeadersToGdt <output.gdt> <input.i> [more headers/dirs]");
            return;
        }

        File outFile = new File(args[0]);
        outFile.getParentFile().mkdirs();
        // FileDataTypeManager refuses to clobber an existing archive
        // through parseHeaderFiles; delete first so reruns are idempotent.
        if (outFile.exists()) {
            outFile.delete();
        }

        List<String> filenames = new ArrayList<>();
        for (int i = 1; i < args.length; i++) {
            filenames.add(args[i]);
        }

        printf("ParseHeadersToGdt: parsing %d input(s) -> %s%n",
               filenames.size(), outFile.getAbsolutePath());
        for (String fn : filenames) {
            printf("  - %s%n", fn);
        }

        FileDataTypeManager dtm = null;
        try {
            dtm = CParserUtils.parseHeaderFiles(
                    new DataTypeManager[0],
                    filenames.toArray(new String[0]),
                    new String[0],
                    outFile.getAbsolutePath(),
                    monitor);
        } catch (Throwable t) {
            printerr("ParseHeadersToGdt: parse threw " + t.getClass().getName()
                     + ": " + t.getMessage());
            String diag = CParserUtils.handleParseProblem(t,
                    filenames.isEmpty() ? "<input>" : filenames.get(0));
            if (diag != null) {
                printerr(diag);
            }
            throw t;
        }

        if (dtm == null) {
            printerr("ParseHeadersToGdt: parseHeaderFiles returned null");
            return;
        }

        int composites = 0;
        int enums = 0;
        int typedefs = 0;
        java.util.Iterator<ghidra.program.model.data.DataType> it =
                dtm.getAllDataTypes();
        while (it.hasNext()) {
            ghidra.program.model.data.DataType dt = it.next();
            String cls = dt.getClass().getSimpleName();
            if (cls.contains("Composite") || cls.contains("Structure")
                    || cls.contains("Union")) {
                composites++;
            } else if (cls.contains("Enum")) {
                enums++;
            } else if (cls.contains("Typedef")) {
                typedefs++;
            }
        }
        printf("ParseHeadersToGdt: parsed OK. composites=%d enums=%d typedefs=%d%n",
               composites, enums, typedefs);
        dtm.close();
    }
}
