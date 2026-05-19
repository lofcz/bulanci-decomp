// PropagateCallerNamespaces - reparent file-scope helpers by callgraph.
//
// After RTTI vftable recovery + string-driven assignment, the remaining
// file-scope functions tend to fall into two categories:
//
//   1. True free helpers - called from many classes (logging, math,
//      memory allocation, string formatting). These should stay
//      file-scope (`_Globals`).
//
//   2. Private members of a class that just happened not to be virtual.
//      These are usually called from one and only one class. If we
//      observe `FUN_x` being called only from `CBulanci::*`, the most
//      likely interpretation is that `FUN_x` is a private CBulanci
//      method that the compiler inlined into the class's source file.
//
// This script handles case 2 by iterating to a fixed point:
//
//   while changed:
//       for each function fn in Global namespace, not a thunk:
//           let owners = { caller.parentNamespace
//                          for caller in callers(fn)
//                          if caller.parentNamespace is a class }
//           if len(owners) == 1 and |unowned_callers| <= ambiguityBudget:
//               fn.setParentNamespace(the single owner)
//
// `ambiguityBudget` defaults to 0 (callers MUST all live in the same
// class) but can be relaxed via the script argument:
//
//   -postscript PropagateCallerNamespaces.java 0       # strict
//   -postscript PropagateCallerNamespaces.java 1       # tolerate 1 stranger
//
// The fixed-point loop matters: once `CBulanci::FUN_x` is attributed,
// any helper called only by `FUN_x` becomes a CBulanci function in the
// next iteration, propagating ownership down the call chain.
//
//@category bulanci

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.Listing;
import ghidra.program.model.symbol.Namespace;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.SymbolType;

import java.util.HashSet;
import java.util.Set;

public class PropagateCallerNamespaces extends GhidraScript {

    private int ambiguityBudget = 0;
    private int maxIterations = 16;

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length >= 1) {
            try {
                ambiguityBudget = Integer.parseInt(args[0]);
            } catch (NumberFormatException nfe) {
                printf("ignoring non-integer ambiguityBudget arg %s\n", args[0]);
            }
        }
        if (args.length >= 2) {
            try {
                maxIterations = Integer.parseInt(args[1]);
            } catch (NumberFormatException nfe) {
                printf("ignoring non-integer maxIterations arg %s\n", args[1]);
            }
        }

        Listing listing = currentProgram.getListing();
        int totalReparented = 0;

        for (int iter = 1; iter <= maxIterations; iter++) {
            if (monitor.isCancelled()) {
                break;
            }
            int reparentedThisRound = 0;

            FunctionIterator fnIt = listing.getFunctions(true);
            while (fnIt.hasNext()) {
                if (monitor.isCancelled()) {
                    break;
                }
                Function fn = fnIt.next();
                if (fn.isThunk()) {
                    continue;
                }
                Namespace ns = fn.getParentNamespace();
                if (ns == null || !ns.isGlobal()) {
                    continue;
                }

                Namespace winner = tryDeriveOwner(fn);
                if (winner == null) {
                    continue;
                }
                try {
                    fn.setParentNamespace(winner);
                    reparentedThisRound++;
                } catch (Exception e) {
                    // Some setters reject moves into class types for
                    // C-linkage functions; ignore.
                }
            }
            totalReparented += reparentedThisRound;
            printf("Propagation iter %d: reparented %d function(s).\n",
                   iter, reparentedThisRound);
            if (reparentedThisRound == 0) {
                break;
            }
        }
        printf("PropagateCallerNamespaces: %d function(s) reparented (budget=%d).\n",
               totalReparented, ambiguityBudget);
    }

    /**
     * Inspect every caller of `fn`. Returns the single class namespace
     * that dominates the call sites, or null if no clear winner.
     *
     *  * Owners = { caller.parentNamespace | caller is a class member }
     *  * If owners has exactly one element AND the number of callers
     *    NOT in any class is <= ambiguityBudget, return that owner.
     *  * Otherwise return null (don't promote).
     */
    private Namespace tryDeriveOwner(Function fn) {
        Set<Namespace> owners = new HashSet<>();
        int unattributedCallers = 0;
        int totalCallers = 0;

        ReferenceIterator refs = currentProgram.getReferenceManager()
                .getReferencesTo(fn.getEntryPoint());
        while (refs.hasNext()) {
            Reference ref = refs.next();
            // Only count code references (calls / jumps), not data
            // references like a function-pointer-in-vftable slot - that
            // is already handled by PromoteVftableMembers.
            if (!ref.getReferenceType().isCall() &&
                !ref.getReferenceType().isJump()) {
                continue;
            }
            Function caller = currentProgram.getListing()
                    .getFunctionContaining(ref.getFromAddress());
            if (caller == null || caller.equals(fn)) {
                continue;
            }
            totalCallers++;
            Namespace cns = caller.getParentNamespace();
            if (cns != null && !cns.isGlobal() && isClassLike(cns)) {
                owners.add(cns);
                if (owners.size() > 1) {
                    return null;  // multiple classes share - not a single owner
                }
            } else {
                unattributedCallers++;
            }
        }
        if (totalCallers == 0) {
            // Roots of the call graph (entry points etc.) - leave alone.
            return null;
        }
        if (owners.size() != 1) {
            return null;
        }
        if (unattributedCallers > ambiguityBudget) {
            return null;
        }
        return owners.iterator().next();
    }

    /**
     * True iff `ns` is a class-shaped namespace (GhidraClass) suitable
     * as an ownership target. We deliberately exclude plain `Namespace`
     * objects (raw `namespace`-keyword scopes) because we want each
     * promoted function to land inside a CLASS so it shows up as a
     * method in the generated stub headers.
     */
    private boolean isClassLike(Namespace ns) {
        return ns.getSymbol().getSymbolType() == SymbolType.CLASS;
    }
}
