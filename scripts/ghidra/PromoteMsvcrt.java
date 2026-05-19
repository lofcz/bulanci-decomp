// Reparent MSVC8 C runtime helpers into a `Runtime::MSVCRT` namespace.
//
// Why this exists: the 200+ underscore-prefixed helpers Ghidra
// auto-names from the PE Rich Header (e.g. `_malloc`, `_free`,
// `__SEH_prolog4`, `__chkstk`, `___security_init_cookie`,
// `_initterm`, `__report_gsfailure`, ...) all land in the Global
// namespace. They have nothing to do with the game's logic - they're
// boilerplate compiled in from MSVCRT - so we segregate them into
// their own unit. This shrinks `_Globals` and lets us ignore the
// MSVCRT bucket entirely during matching work.
//
// Matching strategy: a deliberately narrow allow-list of *exact
// names* and *prefixes* known to be MSVC8 CRT entry points. We do
// NOT match "any function whose name starts with `_`" - that would
// catch the project's own underscore-named globals.
//
// All matches are reparented into `Runtime::MSVCRT` (created on
// first use). Idempotent: skips anything already living under the
// target namespace.
//
//@category bulanci

import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.symbol.Namespace;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.SymbolTable;

import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

public class PromoteMsvcrt extends GhidraScript {

    /**
     * In this binary, every function whose Ghidra-assigned name starts
     * with `_` (one, two or three underscores) and lives in the Global
     * namespace turns out to be MSVCRT scaffolding: memory ops
     * (`_malloc`/`_free`), heap internals (`___sbh_*`), EH glue
     * (`__CxxThrowException@8`, `_CallCatchBlock2`), startup
     * (`__cinit`, `___tmainCRTStartup`), threading (`__beginthreadex`),
     * long-math (`__alldiv`, `__aulldiv`), locale (`__crtLCMap*`),
     * stack probes (`__alloca_probe_*`), stdio (`__flsbuf`,
     * `__fputwc_nolock`), and many more. The project's own functions
     * use class-qualified names (`CBulanci::FUN_xxx`) so they never
     * collide. We therefore use a *broad* match (one to three leading
     * underscores -> CRT) and a small carve-out list for known
     * uppercase-CamelCase MSVCRT helpers that don't start with `_`.
     */
    private static final java.util.regex.Pattern UNDERSCORE_NAME =
        java.util.regex.Pattern.compile("^_{1,3}[A-Za-z][A-Za-z0-9_@]*$");

    /** Exact non-underscored CRT entry-point names. */
    private static final Set<String> EXACT = new HashSet<>(Arrays.asList(
        // Memory
        "_malloc", "_free", "_calloc", "_realloc", "_recalloc", "_msize",
        "_expand", "__nh_malloc", "_callnewh", "_set_new_handler",
        "_set_new_mode", "_query_new_handler", "_query_new_mode",
        "_aligned_malloc", "_aligned_free", "_aligned_realloc",
        // SEH / EH glue
        "__SEH_prolog4", "__SEH_epilog4", "__SEH_prolog4_GS", "__SEH_epilog4_GS",
        "__except_handler3", "__except_handler4", "__except_handler4_common",
        "_setjmp3", "_local_unwind4", "_global_unwind2",
        "_CxxThrowException", "__CxxFrameHandler", "__CxxFrameHandler3",
        "_purecall", "_unhandled_exception_filter",
        // Startup / shutdown
        "_initterm", "_initterm_e", "__initstdio", "__endstdio",
        "_amsg_exit", "_exit", "__exit", "__cexit", "__c_exit",
        "_atexit", "_onexit", "_get_invalid_parameter_handler",
        "_set_invalid_parameter_handler", "_invalid_parameter",
        "_invalid_parameter_noinfo", "_invoke_watson",
        "__crtExitProcess", "__crtTerminateProcess",
        "___security_init_cookie", "___security_check_cookie",
        "__report_gsfailure", "__GSHandlerCheck",
        // Locale / multibyte
        "__getmainargs", "__wgetmainargs", "__set_app_type",
        "__p__commode", "__p__fmode", "__p___initenv",
        "__p__environ", "__p__wenviron",
        "_lock", "_unlock",
        // CRT linkage helpers
        "__alloca_probe", "__alloca_probe_16", "__chkstk", "_chkesp",
        "__crtUnhandledException", "__crtTerminateProcessHelper",
        "___tmainCRTStartup", "__CRT_INIT", "__DllMainCRTStartup",
        "__mainCRTStartup", "__wmainCRTStartup",
        "__matherr", "__set_error_mode",
        // RTTI infra
        "__RTDynamicCast", "__RTtypeid", "___RTDynamicCast", "___RTtypeid",
        // tls
        "__tls_used", "__tls_index"
    ));

    /** Prefixes whose entire family is CRT. Used in addition to EXACT. */
    private static final String[] PREFIXES = new String[] {
        "__sbh_",        // small-block heap internals
        "__crtRand",
        "__crtGet",      // __crtGetEnvironmentStringsA, ...
        "__crtSet",
        "__crtMessageBoxA", "__crtMessageBoxW",
        "__crtCorExitProcess",
        "__crt_debugger_hook",
        "__crt_waiting_on_thread_lock",
        "__floor_pentium4",
        "__fast_error_exit",
        "__SEH_prolog",  // also covers _GS, _GS_3, variants
        "__SEH_epilog",
        "___locale_changed",
        "___setlc_active",
        "___unguarded_readlc_active",
        "___initmbctable",
        "___initmonetary",
        "___initnumeric",
        "___initctype",
        "___initmbctable",
        "___mbcsenum",
        "___mb_cur_max",
        "__lconv_intl_refcount",
        "___lc_handle",
        "___lc_codepage",
        "___pInvalidArgHandler",
    };

    @Override
    public void run() throws Exception {
        FunctionManager fm = currentProgram.getFunctionManager();
        SymbolTable st = currentProgram.getSymbolTable();

        Namespace runtime = getOrCreate(st, currentProgram.getGlobalNamespace(), "Runtime");
        Namespace msvcrt = getOrCreate(st, runtime, "MSVCRT");

        int scanned = 0;
        int matched = 0;
        int reparented = 0;
        int alreadyThere = 0;
        int alreadyClassed = 0;

        FunctionIterator fi = fm.getFunctions(true);
        while (fi.hasNext()) {
            if (monitor.isCancelled()) break;
            Function fn = fi.next();
            scanned++;

            String name = fn.getName();
            if (!isCrtName(name)) continue;
            matched++;

            Namespace ns = fn.getParentNamespace();
            if (ns != null) {
                if (ns == msvcrt || msvcrt.getName(true).equals(ns.getName(true))) {
                    alreadyThere++;
                    continue;
                }
                if (!ns.isGlobal()) {
                    // Don't overwrite a more specific attribution
                    // (e.g. someone already mapped __SEH_prolog4 to
                    // its caller via the EH funclet pass).
                    alreadyClassed++;
                    continue;
                }
            }
            fn.setParentNamespace(msvcrt);
            reparented++;
        }

        printf("PromoteMsvcrt: scanned=%d matched=%d reparented=%d "
               + "already_in_msvcrt=%d already_classed=%d%n",
               scanned, matched, reparented, alreadyThere, alreadyClassed);
    }

    private static boolean isCrtName(String name) {
        if (name == null) return false;
        // Carve out synthetic Ghidra-generated names that happen to
        // start with an uppercase letter (handled by other passes).
        if (name.startsWith("FUN_") || name.startsWith("thunk_FUN_")
                || name.startsWith("Catch@") || name.startsWith("Unwind@")
                || name.startsWith("switchD_")) {
            return false;
        }
        if (EXACT.contains(name)) return true;
        if (UNDERSCORE_NAME.matcher(name).matches()) {
            // One-to-three leading underscores plus a letter-led tail:
            // MSVC's naming convention for C runtime helpers.
            return true;
        }
        for (String p : PREFIXES) {
            if (name.startsWith(p)) return true;
        }
        return false;
    }

    private static Namespace getOrCreate(SymbolTable st, Namespace parent,
                                         String name) throws Exception {
        Namespace existing = st.getNamespace(name, parent);
        if (existing != null) return existing;
        return st.createNameSpace(parent, name, SourceType.ANALYSIS);
    }
}
