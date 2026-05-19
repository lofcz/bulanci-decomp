// DumpVftablesCsv - export every recovered C++ vftable as CSV.
//
// Walks every symbol named "vftable" inside a non-Global namespace,
// reads consecutive 32-bit pointer slots from .rdata, and resolves
// each slot to a function. Stops at the first explicit (non-default)
// label, signalling the next vftable / data structure.
//
// Output rows:
//   class_name;vftable_addr;slot;method_addr;method_name;is_pure_virtual
//
// is_pure_virtual is `1` when the slot points at a `_purecall` thunk
// or any function name containing "purecall", `0` otherwise.
//
// Designed to be re-run after every namespace-promotion pass:
//
//   analyzeHeadless ... -postscript DumpVftablesCsv.java \
//       "config/bulanci/vftable_methods.csv"
//
// If the path argument is omitted it defaults to the same location.
//
// Consumed by `scripts/internal/build_module_map.py` together with
// `DumpFunctionsCsv.java`'s output.
//
//@category bulanci

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressFactory;
import ghidra.program.model.address.AddressSpace;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryAccessException;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolIterator;
import ghidra.program.model.symbol.SymbolTable;

import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;
import java.util.TreeMap;

public class DumpVftablesCsv extends GhidraScript {

    private static final String DEFAULT_OUT =
            "config/bulanci/vftable_methods.csv";

    /** Window of code addresses we treat as plausible function pointers. */
    private static final long TEXT_LOW  = 0x00401000L;
    private static final long TEXT_HIGH = 0x0047f000L;

    /** Defensive cap; no real C++ vftable in this program has > 256 slots. */
    private static final int MAX_SLOTS = 256;

    @Override
    public void run() throws Exception {
        String outArg = (getScriptArgs().length >= 1)
                ? getScriptArgs()[0]
                : DEFAULT_OUT;
        File outFile = resolveOutput(outArg);
        outFile.getParentFile().mkdirs();

        SymbolTable st = currentProgram.getSymbolTable();
        Memory mem = currentProgram.getMemory();
        FunctionManager fm = currentProgram.getFunctionManager();
        AddressFactory af = currentProgram.getAddressFactory();
        AddressSpace defSpace = af.getDefaultAddressSpace();

        // Collect every `vftable` symbol that belongs to a class
        // namespace. Global-scope vftables are not interesting because
        // they cannot be attributed to a single C++ class.
        List<Symbol> vftables = new ArrayList<>();
        SymbolIterator si = st.getAllSymbols(true);
        while (si.hasNext()) {
            Symbol s = si.next();
            if (!"vftable".equals(s.getName())) {
                continue;
            }
            if (s.getParentNamespace() == null
                    || s.getParentNamespace().isGlobal()) {
                continue;
            }
            vftables.add(s);
        }
        printf("DumpVftablesCsv: processing %d vftable(s).\n", vftables.size());

        int totalSlots = 0;
        int uniqueMethods = 0;
        java.util.Set<Long> seenMethods = new java.util.HashSet<>();
        TreeMap<String, Integer> classCounts = new TreeMap<>();

        try (PrintWriter pw = new PrintWriter(new FileWriter(outFile))) {
            pw.println("class_name;vftable_addr;slot;method_addr;method_name;is_pure_virtual");

            for (Symbol vfSym : vftables) {
                if (monitor.isCancelled()) {
                    break;
                }
                String className = vfSym.getParentNamespace().getName(true);
                Address vfAddr = vfSym.getAddress();

                for (int slot = 0; slot < MAX_SLOTS; slot++) {
                    Address slotAddr = vfAddr.add(slot * 4L);

                    // Stop at the next named label (likely the next
                    // vftable or RTTI structure). Slot 0 itself is
                    // labelled `vftable`, so skip the test there.
                    if (slot > 0) {
                        boolean foreign = false;
                        for (Symbol lbl : st.getSymbols(slotAddr)) {
                            if (lbl.getSource() != SourceType.DEFAULT) {
                                foreign = true;
                                break;
                            }
                        }
                        if (foreign) {
                            break;
                        }
                    }

                    int rawPtr;
                    try {
                        rawPtr = mem.getInt(slotAddr);
                    } catch (MemoryAccessException e) {
                        break;
                    }
                    long ptr = ((long) rawPtr) & 0xFFFFFFFFL;
                    if (ptr < TEXT_LOW || ptr >= TEXT_HIGH) {
                        break;
                    }

                    Address fnAddr = defSpace.getAddress(ptr);
                    Function fn = fm.getFunctionAt(fnAddr);
                    boolean pure = false;
                    String fnName;
                    if (fn == null) {
                        fnName = "__no_func__";
                        Symbol[] lbl = st.getSymbols(fnAddr);
                        if (lbl.length > 0) {
                            fnName = lbl[0].getName();
                        }
                        if (fnName.contains("purecall")) {
                            pure = true;
                        }
                    } else {
                        fnName = fn.getName(true);
                        if (fnName.contains("_purecall")) {
                            pure = true;
                        }
                    }
                    pw.printf("%s;%08x;%d;%08x;%s;%s%n",
                              className, vfAddr.getOffset(), slot, ptr,
                              fnName, pure ? "1" : "0");
                    totalSlots++;
                    if (seenMethods.add(ptr)) {
                        uniqueMethods++;
                    }
                    classCounts.merge(className, 1, Integer::sum);
                }
            }
        }
        printf("DumpVftablesCsv: %d slot(s), %d unique method(s), %d class(es). " +
               "Wrote %s.\n",
               totalSlots, uniqueMethods, classCounts.size(),
               outFile.getAbsolutePath());
    }

    /** Resolve a relative path against the project root. Absolute paths pass through. */
    private File resolveOutput(String pathArg) {
        File f = new File(pathArg);
        if (f.isAbsolute()) {
            return f;
        }
        return new File(getProjectRoot(), pathArg);
    }

    /** Best-effort: the project root is the parent of the .gpr file. */
    private File getProjectRoot() {
        try {
            String path = state.getProject().getProjectLocator().getProjectDir().getAbsolutePath();
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
