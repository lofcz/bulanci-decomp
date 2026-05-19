// AssignByStringRefs - reparent functions based on debug/log strings.
//
// Many MSVC C++ codebases sprinkle the source with `__FILE__`-style or
// hand-rolled debug strings that name the owning class - assertions,
// trace logs, error-reporting helpers, RTTI fallback messages. If a
// function `FUN_004xxxxx` references the string "CBulanci" (or
// "CBulanci.cpp", "CBulanci::Init", "[CBulanci]", ...) and references
// at most one such class string, that function almost certainly belongs
// to CBulanci. This script reparents those functions into the matching
// class namespace.
//
// Conservative by design:
//   * A function is only moved if EVERY class-named string it references
//     points to the same class. Functions that touch strings for several
//     different classes (logging helpers, dispatchers, ...) are left
//     alone.
//   * Functions that are already in some non-Global namespace are
//     never touched - vftable-recovered methods, manually-named
//     functions, and STL members stay put.
//   * Thunks are skipped entirely.
//
// Usage:
//   analyzeHeadless ... -postscript AssignByStringRefs.java
//
//@category bulanci

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.data.StringDataInstance;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.DataIterator;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.GhidraClass;
import ghidra.program.model.listing.Listing;
import ghidra.program.model.symbol.Namespace;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.SymbolTable;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class AssignByStringRefs extends GhidraScript {

    /**
     * Plain identifier in surrounding text - any class-shaped token of
     * at least 4 chars. We deliberately do NOT use `\b` here: MSVC
     * RTTI strings look like `.?AVCBulanci@@`, so the class name has
     * a word-character (`V`) immediately to its left and `\b` would
     * fail to fire there.
     */
    private static final Pattern CLASS_IDENT =
            Pattern.compile("([A-Z][A-Za-z0-9_]{3,})");

    /**
     * MSVC RTTI type descriptor names. Group 1 is the demangled class
     * identifier. `.?AV` is the type-of-class prefix; `.?AU` denotes
     * a struct (which Ghidra often promotes to GhidraClass too).
     */
    private static final Pattern RTTI_NAME =
            Pattern.compile("\\.\\?A[VU]([A-Za-z_][A-Za-z0-9_]*)@@");

    /**
     * Hungarian-style prefixes the project strips when authoring
     * debug / log strings: `CBulanci` is logged as `Bulanci`,
     * `CDSWav` as `Wav`, ... We expand the alias table so a stripped
     * form still points back at the right class.
     */
    private static final String[] HUNGARIAN_PREFIXES = {"CDS", "C"};

    @Override
    public void run() throws Exception {
        SymbolTable st = currentProgram.getSymbolTable();
        Listing listing = currentProgram.getListing();

        // Build a set of all class namespaces known to Ghidra. This
        // catches both RTTI-recovered classes (CBulanci, CGaming, ...)
        // and STL classes (std::exception). We key by the leaf class
        // name AND any Hungarian-prefix-stripped alias, so a debug
        // string of "Bulanci" or "QueueStream" still maps back to
        // CBulanci / CDSQueueStream. Aliases that collide between
        // multiple classes are dropped to avoid false attribution.
        Map<String, GhidraClass> classByName = new HashMap<>();
        Set<String> ambiguousAliases = new HashSet<>();
        Iterator<GhidraClass> classIt = st.getClassNamespaces();
        int rawClasses = 0;
        while (classIt.hasNext()) {
            GhidraClass cls = classIt.next();
            rawClasses++;
            for (String alias : aliasesFor(cls.getName())) {
                GhidraClass prior = classByName.get(alias);
                if (prior == null) {
                    classByName.put(alias, cls);
                } else if (!prior.equals(cls)) {
                    ambiguousAliases.add(alias);
                }
            }
        }
        for (String a : ambiguousAliases) {
            classByName.remove(a);
        }
        printf("AssignByStringRefs: %d class namespace(s) loaded (%d aliases, " +
               "%d dropped as ambiguous)\n",
               rawClasses, classByName.size(), ambiguousAliases.size());

        // For each function, accumulate the set of classes whose name
        // it textually references via any defined string in the program.
        Map<Function, Set<GhidraClass>> classRefsByFunc = new HashMap<>();
        int stringsScanned = 0;
        int stringsMatched = 0;

        DataIterator strs = listing.getDefinedData(true);
        while (strs.hasNext()) {
            if (monitor.isCancelled()) {
                break;
            }
            Data data = strs.next();
            String text = stringValueOf(data);
            if (text == null || text.isEmpty()) {
                continue;
            }
            stringsScanned++;

            // Collect every class name appearing in this string. Many
            // log messages name two classes (sender + receiver); we
            // need them all so the per-function aggregation can spot
            // ambiguity later.
            Set<GhidraClass> matched = new HashSet<>();

            // Tier 1: RTTI mangled-name form .?AVClass@@ - exact, no
            // false positives.
            Matcher rttiMatch = RTTI_NAME.matcher(text);
            while (rttiMatch.find()) {
                GhidraClass cls = classByName.get(rttiMatch.group(1));
                if (cls != null) {
                    matched.add(cls);
                }
            }

            // Tier 2: free-text identifier scan, e.g. "Bulanci" inside
            // "Software\SleepTeam\Bulanci". Matches both the canonical
            // class name and any Hungarian-stripped alias.
            Matcher textMatch = CLASS_IDENT.matcher(text);
            while (textMatch.find()) {
                GhidraClass cls = classByName.get(textMatch.group(1));
                if (cls != null) {
                    matched.add(cls);
                }
            }

            if (matched.isEmpty()) {
                continue;
            }
            stringsMatched++;

            // For each function that refers to this string, accumulate
            // the matched classes.
            ReferenceIterator refIt =
                    currentProgram.getReferenceManager().getReferencesTo(data.getAddress());
            while (refIt.hasNext()) {
                Reference ref = refIt.next();
                Function fn = listing.getFunctionContaining(ref.getFromAddress());
                if (fn == null || fn.isThunk()) {
                    continue;
                }
                Namespace ns = fn.getParentNamespace();
                if (ns != null && !ns.isGlobal()) {
                    // Already attributed - don't second-guess vftable
                    // recovery or manual renames.
                    continue;
                }
                classRefsByFunc.computeIfAbsent(fn, k -> new HashSet<>())
                        .addAll(matched);
            }
        }

        // Reparent any function whose accumulated set is a single class.
        int reparented = 0;
        int skippedAmbiguous = 0;
        for (Map.Entry<Function, Set<GhidraClass>> e : classRefsByFunc.entrySet()) {
            Set<GhidraClass> classes = e.getValue();
            if (classes.size() != 1) {
                skippedAmbiguous++;
                continue;
            }
            GhidraClass cls = classes.iterator().next();
            Function fn = e.getKey();
            try {
                fn.setParentNamespace(cls);
                reparented++;
            } catch (Exception ex) {
                printf("could not reparent %s -> %s: %s\n",
                       fn.getName(), cls.getName(true), ex.getMessage());
            }
        }
        printf("AssignByStringRefs: scanned %d string(s), %d matched a class name, " +
               "reparented %d function(s), %d ambiguous (multi-class refs).\n",
               stringsScanned, stringsMatched, reparented, skippedAmbiguous);
    }

    /**
     * Expand a class name into the search aliases used during string
     * scanning. We keep the original, plus every Hungarian-stripped
     * form long enough to survive the {3,} length filter in
     * CLASS_IDENT. Examples:
     *   CBulanci       -> ["CBulanci", "Bulanci"]
     *   CDSWav         -> ["CDSWav",   "Wav"]               (Wav is 3 chars, kept)
     *   CDSQueueStream -> ["CDSQueueStream", "QueueStream"]
     */
    private static List<String> aliasesFor(String className) {
        List<String> out = new ArrayList<>(3);
        if (className == null || className.isEmpty()) {
            return out;
        }
        out.add(className);
        for (String prefix : HUNGARIAN_PREFIXES) {
            if (className.startsWith(prefix)
                    && className.length() > prefix.length() + 3) {
                String stripped = className.substring(prefix.length());
                // Only keep alphabetic-leading aliases - dropping a
                // prefix could expose a digit start, e.g. C2Var ->
                // 2Var, which would never match the regex anyway.
                if (Character.isUpperCase(stripped.charAt(0))) {
                    out.add(stripped);
                }
            }
        }
        return out;
    }

    /**
     * Best-effort string extraction for a defined Data item. Returns
     * null if the item is not a string we can read.
     */
    private static String stringValueOf(Data data) {
        if (!data.hasStringValue()) {
            return null;
        }
        try {
            StringDataInstance sdi = StringDataInstance.getStringDataInstance(data);
            return sdi == null ? null : sdi.getStringValue();
        } catch (Throwable t) {
            return null;
        }
    }
}
