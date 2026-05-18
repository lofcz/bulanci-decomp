/*
 * LICENSE
 */
// Description
//@author renzo904
//@category exports
//@keybinding
//@menupath Skeleton
//@toolbar Skeleton
import static java.util.Map.entry;

import ghidra.app.analyzers.RelocationTableSynthesizerAnalyzer;
import ghidra.app.script.GhidraScript;
import ghidra.app.services.Analyzer;
import ghidra.app.util.DomainObjectService;
import ghidra.app.util.Option;
import ghidra.app.util.exporter.CoffRelocatableObjectExporter;
import ghidra.app.util.importer.MessageLog;
import ghidra.framework.model.DomainFile;
import ghidra.framework.model.DomainObject;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.listing.GhidraClass;
import ghidra.program.model.address.AddressRange;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.symbol.Namespace;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolTable;
import java.io.File;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

public class ExportDelinker extends GhidraScript
{
    @Override protected void run() throws Exception
    {
        // First run the Relocation Table Synthesizer, to pickup any potentially
        // new globals in the reloc table.
        Analyzer analyzer = new RelocationTableSynthesizerAnalyzer();
        analyzer.added(currentProgram, currentProgram.getMemory(), monitor, new MessageLog());

        // Then, export the COFFs.
        CoffRelocatableObjectExporter exporter = new CoffRelocatableObjectExporter();

        List<Option> exporterOptions = exporter.getOptions(new DomainObjectService() {
            @Override public DomainObject getDomainObject()
            {
                return currentProgram;
            }
        });

        // bulanci.exe is RTTI-rich C++: Ghidra's auto-analysis assigns the same
        // bare name (::vftable, ::vbtable, ::CreateObject, ...) to multiple
        // distinct symbols inside the same class namespace (one per inheritance
        // path / overload). CoffRelocatableObjectExporter aborts the export
        // when its EXTERNAL-undefined symbol table contains two entries with
        // identical names. The "static-visibility regex" option does NOT help
        // here - external/undefined symbols are always EXTERNAL class regardless
        // of visibility predicates (see computeExternalSymbols).
        //
        // Workaround: walk the in-memory symbol table once and suffix every
        // duplicate's name with its address. The project is opened -readOnly,
        // so these renames stay in memory and never touch the .gpr on disk.
        //
        // We also sanitise Ghidra's synthetic symbol names that are not
        // valid C++ identifiers (e.g. `Catch@<addr>`, backtick-quoted
        // `` `scalar_deleting_destructor' ``) so the COFF symbols here
        // match what `scripts/generate_sources.py` writes into the stub
        // sources via `_sanitize_symbol_name()`. Without this, objdiff
        // would see the target and stub functions under different names
        // and never pair them.
        sanitizeSymbolNames();
        deduplicateSymbolNames();

        exporter.setOptions(exporterOptions);

        File inFile = askFile("Config File", "Select");
        File outDir = askDirectory("Output Folder", "Select");

        String configFile = Files.readString(inFile.toPath(), StandardCharsets.UTF_8);
        Iterable<String> iterable = () -> configFile.lines().iterator();
        for (String objDataStr : iterable)
        {
            List<String> objData = new ArrayList<>(Arrays.asList(objDataStr.split(",")));

            String objClass = objData.remove(0);

            File outFile = new File(outDir, objClass + ".obj");

            AddressSet set = new AddressSet();
            for (String ghidraClassName : objData)
            {
                printf("Handling %s.obj - class %s\n", objClass, ghidraClassName);

                List<String> ghidraClassNameParts = new ArrayList<>(Arrays.asList(ghidraClassName.split("::")));
                String finalPart = ghidraClassNameParts.removeLast();

                Namespace curNs = null;
                for (String nsPart : ghidraClassNameParts)
                {
                    curNs = this.getNamespace(curNs, nsPart);
                }

                Symbol sym;
                if ((sym = this.getSymbol(finalPart, curNs)) == null)
                {
                    printf("Cannot find namespace or function %s, skipping.\n", ghidraClassName);
                    continue;
                }
                if (!(sym.getObject() instanceof Namespace))
                {
                    printf("Namespace %s is not a namespace or a function, skipping.\n", ghidraClassName);
                    continue;
                }

                Namespace ns = (Namespace)sym.getObject();
                set = set.union(ns.getBody());
            }

            if (set.isEmpty())
            {
                printf("No namespaces found for %s.obj, skipping.\n", objClass);
                continue;
            }

            boolean success = exporter.export(outFile, currentProgram, set, monitor);
            if (!success) {
                printf("Failed exporting %s.obj to %s file.\n", objClass, outFile);
                printf("Export log: %s.\n", exporter.getMessageLog().toString());
            }
        }
    }

    /**
     * Suffix every symbol whose qualified name collides with at least one
     * other symbol so each surviving name is unique. Address suffix is in
     * lower-case hex (e.g. "vftable_0042a000"), matching MSVC mangling
     * conventions reasonably well. Idempotent: already-suffixed names are
     * untouched unless they still collide.
     *
     * Operates in-memory; requires either -readOnly or a writeable project.
     */
    /**
     * Rewrite any symbol whose simple name contains characters that are
     * illegal in a C++ identifier (commonly `@`, backtick, single-quote)
     * into a sanitised form built only from `[A-Za-z0-9_]` plus a leading
     * `~` for destructors. Mirrors `_sanitize_symbol_name()` in
     * `scripts/generate_sources.py` so the stub COFF symbols and the
     * target COFF symbols agree.
     *
     * Operator overloads (`operator==`, `operator new[]`, ...) are left
     * alone - those are legal C++ method names and MSVC will mangle them
     * the same way on both sides of the diff.
     *
     * Operates in-memory; requires either -readOnly or a writeable project.
     */
    private void sanitizeSymbolNames() throws Exception {
        SymbolTable st = currentProgram.getSymbolTable();
        int renamed = 0;
        for (Symbol sym : st.getAllSymbols(true)) {
            String original = sym.getName();
            String sanitised = sanitiseName(original);
            if (sanitised.equals(original)) {
                continue;
            }
            try {
                sym.setName(sanitised, SourceType.USER_DEFINED);
                renamed++;
            } catch (Exception e) {
                // Some symbols reject renames (function entry points
                // bound to type info, etc.); skip them rather than abort.
            }
        }
        if (renamed > 0) {
            printf("Sanitised %d symbol name(s) for COFF export.\n", renamed);
        }
    }

    /** Pure-function version of `_sanitize_symbol_name()`. */
    private static String sanitiseName(String name) {
        if (name == null || name.isEmpty()) {
            return name;
        }
        if (name.startsWith("operator") && name.length() > "operator".length()) {
            return name;
        }
        StringBuilder out = new StringBuilder(name.length());
        for (int i = 0; i < name.length(); i++) {
            char c = name.charAt(i);
            if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
                    || (c >= '0' && c <= '9') || c == '_') {
                out.append(c);
            } else if (c == '~' && i == 0) {
                out.append(c);
            } else {
                out.append('_');
            }
        }
        // Collapse runs of underscores introduced by the substitution.
        String collapsed = out.toString().replaceAll("_+", "_");
        // Strip outer underscores, but reinstate a leading `_` if the
        // original started with one (legal C++ identifier start).
        String stripped = collapsed.replaceAll("^_+|_+$", "");
        if (name.charAt(0) == '_' && !stripped.startsWith("_")) {
            stripped = "_" + stripped;
        }
        return stripped.isEmpty() ? "_anon" : stripped;
    }

    private void deduplicateSymbolNames() throws Exception {
        SymbolTable st = currentProgram.getSymbolTable();
        Map<String, List<Symbol>> byQualifiedName = new HashMap<>();
        for (Symbol sym : st.getAllSymbols(true)) {
            String qn = sym.getName(true);
            byQualifiedName.computeIfAbsent(qn, k -> new ArrayList<>()).add(sym);
        }

        int renamed = 0;
        for (Map.Entry<String, List<Symbol>> entry : byQualifiedName.entrySet()) {
            List<Symbol> dups = entry.getValue();
            if (dups.size() <= 1) {
                continue;
            }
            // Keep one as-is, suffix all others.
            dups.sort((a, b) -> a.getAddress().compareTo(b.getAddress()));
            for (int i = 1; i < dups.size(); i++) {
                Symbol s = dups.get(i);
                String suffix = "_" + s.getAddress().toString().toLowerCase();
                try {
                    s.setName(s.getName() + suffix, SourceType.USER_DEFINED);
                    renamed++;
                } catch (Exception e) {
                    // Some symbols (e.g. function entry points) may reject
                    // renames; skip them silently rather than aborting the
                    // entire export run.
                }
            }
        }
        if (renamed > 0) {
            printf("Deduplicated %d colliding symbol name(s) by suffixing with address.\n", renamed);
        }
    }
}
