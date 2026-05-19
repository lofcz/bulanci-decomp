// AssignEntryPoints - name well-known PE entry points and roots.
//
// Every MSVC Win32 executable has a fixed set of entry points the
// CRT/PE conventions can anchor blindly:
//
//   * The PE OptionalHeader.AddressOfEntryPoint is some flavour of
//     `mainCRTStartup` (or `WinMainCRTStartup`, `wWinMainCRTStartup`,
//     ...). It is essentially always the CRT bootstrap.
//
//   * From that CRT entry, the user program is invoked via a small
//     known relay. For a /SUBSYSTEM:WINDOWS exe the relay calls one of
//        WinMain   (HINSTANCE,HINSTANCE,LPSTR ,int)
//        wWinMain  (HINSTANCE,HINSTANCE,LPWSTR,int)
//     For /SUBSYSTEM:CONSOLE it's `main` / `wmain`.
//
//   * `WndProc` / `DialogProc` show up as direct arguments to the
//     RegisterClass{A,W,Ex{A,W}} and DialogBox / CreateDialog APIs.
//     They have unambiguous signatures and call-graph positions.
//
// This script anchors those well-known names. Anchoring the top of the
// call graph dramatically improves what `PropagateCallerNamespaces.java`
// can spread downwards in subsequent runs.
//
// Conservative: never overwrites an existing non-default name and never
// rewrites a function already in a non-Global namespace. Multiple
// `RegisterClass*` callsites are allowed - we will tag the same
// function `WndProc` if it shows up several times, otherwise name them
// `WndProc_<addr>` to keep them distinct.
//
//@category bulanci

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressIterator;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.Listing;
import ghidra.program.model.scalar.Scalar;
import ghidra.program.model.symbol.RefType;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolTable;

import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class AssignEntryPoints extends GhidraScript {

    /** APIs whose first or sole code argument is a window procedure. */
    private static final List<String> WNDPROC_APIS = Arrays.asList(
            "RegisterClassA", "RegisterClassW",
            "RegisterClassExA", "RegisterClassExW");

    /**
     * APIs whose first code argument is a dialog procedure. The actual
     * `lpDialogFunc` slot varies by call (DialogBoxParamA has it at
     * position 4, DialogBoxA at 3, ...) but we treat any code-typed
     * argument that survives constant propagation as a candidate.
     */
    private static final List<String> DIALOGPROC_APIS = Arrays.asList(
            "DialogBoxA", "DialogBoxW",
            "DialogBoxParamA", "DialogBoxParamW",
            "DialogBoxIndirectParamA", "DialogBoxIndirectParamW",
            "CreateDialogA", "CreateDialogW",
            "CreateDialogParamA", "CreateDialogParamW");

    @Override
    public void run() throws Exception {
        renameCrtEntry();
        renameWinMainFromCrt();
        nameProcsFromImports(WNDPROC_APIS, "WndProc");
        nameProcsFromImports(DIALOGPROC_APIS, "DialogProc");
    }

    /**
     * Find the PE entry point and tag it `_mainCRTStartup` if it's
     * still default-named.
     */
    private void renameCrtEntry() throws Exception {
        SymbolTable st = currentProgram.getSymbolTable();
        AddressIterator allEntries = st.getExternalEntryPointIterator();
        while (allEntries.hasNext()) {
            Address ep = allEntries.next();
            Function fn = currentProgram.getListing().getFunctionAt(ep);
            if (fn == null || !isDefaultName(fn)) {
                continue;
            }
            try {
                fn.setName("_mainCRTStartup", SourceType.USER_DEFINED);
                printf("entry point at %s -> _mainCRTStartup\n", ep);
                return;
            } catch (Exception e) {
                printf("could not rename entry point at %s: %s\n", ep, e.getMessage());
            }
        }
    }

    /**
     * If `_mainCRTStartup` exists, look at the functions it calls and
     * pick the one with a WinMain-ish signature shape (4 stack args /
     * stdcall). For unstripped MSVC binaries this is usually the most
     * heavily-called function at one level of indirection from the
     * entry.
     */
    private void renameWinMainFromCrt() throws Exception {
        SymbolTable st = currentProgram.getSymbolTable();
        Symbol crtSym = findFirstSymbol(st, "_mainCRTStartup");
        if (crtSym == null) {
            return;
        }
        Function crt = currentProgram.getListing().getFunctionAt(crtSym.getAddress());
        if (crt == null) {
            return;
        }
        Function best = null;
        int bestArity = 0;
        for (Function callee : crt.getCalledFunctions(monitor)) {
            if (callee == null || callee.isThunk() || callee.isExternal()) {
                continue;
            }
            if (!isDefaultName(callee)) {
                continue;
            }
            int arity = callee.getParameterCount();
            if (arity > bestArity) {
                bestArity = arity;
                best = callee;
            }
        }
        if (best != null && bestArity >= 3) {
            try {
                best.setName("WinMain", SourceType.USER_DEFINED);
                printf("WinMain candidate: %s (%d params)\n",
                       best.getEntryPoint(), bestArity);
            } catch (Exception e) {
                printf("could not rename WinMain candidate: %s\n", e.getMessage());
            }
        }
    }

    /**
     * For every call site of an API in `imports`, look back through
     * the instruction stream for the most recent `PUSH <const>` that
     * targets a function start, and rename that function.
     */
    private void nameProcsFromImports(List<String> imports, String baseName) throws Exception {
        Set<Function> tagged = new HashSet<>();
        Listing listing = currentProgram.getListing();
        SymbolTable st = currentProgram.getSymbolTable();

        for (String apiName : imports) {
            for (Symbol api : findExternalSymbols(st, apiName)) {
                ReferenceIterator refs = currentProgram.getReferenceManager()
                        .getReferencesTo(api.getAddress());
                while (refs.hasNext()) {
                    Reference ref = refs.next();
                    if (!ref.getReferenceType().isCall() &&
                        !ref.getReferenceType().isJump() &&
                        !ref.getReferenceType().isIndirect()) {
                        continue;
                    }
                    Function caller = listing.getFunctionContaining(ref.getFromAddress());
                    if (caller == null) {
                        continue;
                    }
                    // Walk backwards from the call site looking at the
                    // most recent PUSH <imm>; if the imm is a function
                    // entry, we have our candidate procedure.
                    Function candidate = findPushedFunction(caller, ref.getFromAddress());
                    if (candidate == null || candidate.isThunk()) {
                        continue;
                    }
                    if (!isDefaultName(candidate)) {
                        continue;
                    }
                    if (!tagged.add(candidate)) {
                        continue;  // already tagged this run
                    }
                    String name = baseName + "_" +
                            Long.toHexString(candidate.getEntryPoint().getOffset());
                    try {
                        candidate.setName(name, SourceType.USER_DEFINED);
                        printf("%s call at %s -> %s = %s\n",
                               apiName, ref.getFromAddress(), candidate.getEntryPoint(), name);
                    } catch (Exception e) {
                        printf("rename failed for %s candidate at %s: %s\n",
                               baseName, candidate.getEntryPoint(), e.getMessage());
                    }
                }
            }
        }
    }

    /**
     * Scan up to ~32 instructions backwards from `callSite` within
     * `caller`. Return the first function whose entry address equals
     * a PUSH-immediate operand. This handles `mov reg,Func ; push reg`
     * via reference following: any data reference from a backward
     * instruction to a function entry is taken as the answer.
     */
    private Function findPushedFunction(Function caller, Address callSite) {
        Listing listing = currentProgram.getListing();
        Instruction cur = listing.getInstructionAt(callSite);
        if (cur == null) {
            return null;
        }
        for (int hops = 0; hops < 32; hops++) {
            cur = cur.getPrevious();
            if (cur == null) {
                return null;
            }
            // Stop walking if we've left the function we started in
            // (different caller).
            if (!caller.getBody().contains(cur.getAddress())) {
                return null;
            }
            for (Reference r : cur.getReferencesFrom()) {
                if (r.getReferenceType() == RefType.DATA ||
                    r.getReferenceType().isFlow() ||
                    r.getReferenceType().isJump()) {
                    Function f = listing.getFunctionAt(r.getToAddress());
                    if (f != null) {
                        return f;
                    }
                }
            }
            String mnem = cur.getMnemonicString();
            if (mnem == null) continue;
            if (mnem.equalsIgnoreCase("PUSH")) {
                Object[] ops = cur.getOpObjects(0);
                if (ops.length > 0 && ops[0] instanceof Scalar) {
                    long v = ((Scalar) ops[0]).getUnsignedValue();
                    Address a = currentProgram.getAddressFactory()
                            .getDefaultAddressSpace().getAddress(v);
                    Function f = listing.getFunctionAt(a);
                    if (f != null) {
                        return f;
                    }
                }
            }
        }
        return null;
    }

    private boolean isDefaultName(Function fn) {
        String n = fn.getName();
        if (n == null) {
            return true;
        }
        return n.startsWith("FUN_") || n.startsWith("LAB_") || n.startsWith("entry");
    }

    private Symbol findFirstSymbol(SymbolTable st, String name) {
        List<Symbol> all = st.getGlobalSymbols(name);
        return all.isEmpty() ? null : all.get(0);
    }

    private Iterable<Symbol> findExternalSymbols(SymbolTable st, String name) {
        return st.getGlobalSymbols(name);
    }
}
