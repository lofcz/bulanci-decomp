"""Promote functions into class namespaces by chaining ten heuristics.

Each pass is a Ghidra headless script that runs in WRITEABLE mode
against the repo-root project (``bulanci.gpr``). Earlier passes anchor
the call graph and the later passes propagate ownership downstream:

  1. ``ApplyFidDb.java``
       If FunctionID databases are attached to the project, apply
       single-match results to remaining ``FUN_xxxx`` functions. Names
       CRT / STL / ATL / MFC entry points that Ghidra's RTTI analysis
       did not cover. No-op if no databases are attached.

  2. ``AssignEntryPoints.java``
       Tag the PE entry as ``_mainCRTStartup`` and probe its
       first-hop callees + ``RegisterClass*`` / ``DialogBox*`` argument
       slots to anchor ``WinMain`` / ``WndProc`` / ``DialogProc``.

  3. ``PromoteMsvcrt.java``
       Carve every underscore-prefixed CRT helper (``_malloc``,
       ``__SEH_prolog4``, ``___tmainCRTStartup``, ...) into a
       ``Runtime::MSVCRT`` namespace. Removes ~230 functions from
       _Globals before the class-based passes start.

  4. ``PromoteVftableMembers.java``
       Walk every RTTI-discovered class's vftable and reparent each
       virtual method into the class namespace.

  5. ``PromoteByVtableWrite.java``
       Find every ``FUN_xxxx`` that stores a known vftable pointer into
       ``*this`` (or whose only vftable write is to one class) - that's
       a constructor or destructor for whichever class owns the
       vftable. Catches functions that aren't in any vftable (so the
       previous pass missed them).

  6. ``AssignByStringRefs.java``
       Match defined strings against the class name table; if a
       function references exactly one class's name string, reparent it.

  7. ``AssignByThisPointerType.java``
       For each file-scope function whose first parameter has a
       pointer-to-known-class type (in the *stored* signature),
       reparent it. Catches non-virtual methods Ghidra typed but
       never assigned.

  8. ``PromoteByConstantThis.java``
       Decompile every function once and aggregate, for each callee,
       the data type that callers pass as arg0 *at the call site*.
       If at least N typed call sites all agree on the same
       ``CClass*``, reparent the callee.  Catches functions whose
       stored signature is still ``undefined4 *`` even though every
       caller's HighFunction proves arg0 is a class pointer.

  9. ``PropagateCallerNamespaces.java``
       Iterate to fixed point: if a function is only called by methods
       of a single class, attribute it to that class.

 10. ``PromoteEhFunclets.java``
       Reparent every ``Catch@xxxx`` / ``Unwind@xxxx`` MSVC EH funclet
       into the namespace of its spatially adjacent parent function.
       Runs *last* so all the earlier class assignments inform the
       parent-search.

After the chain runs we regenerate ``config/bulanci/mapping.csv`` (via
``GenerateMapping.java``) so the new namespace prefixes flow into
the build configuration.

Close the Ghidra GUI before running - the chain mutates the project."""

from __future__ import annotations

import argparse
import os
import sys
from pathlib import Path

import ghidra_helpers

SCRIPT_PATH = Path(os.path.realpath(__file__)).parent
REPO_ROOT = SCRIPT_PATH.parent


def _refuse_if_locked(project: str) -> None:
    lock = REPO_ROOT / f"{project}.lock"
    locktilde = REPO_ROOT / f"{project}.lock~"
    if lock.exists() or locktilde.exists():
        raise SystemExit(
            f"refusing to run: {project}.lock or {project}.lock~ is present.\n"
            "Close the Ghidra GUI first; this script needs write access to "
            f"{project}.gpr and the project must be unlocked."
        )


def promoteNamespaces(
    project: str,
    program: str,
    mapping_out: str,
    skip_fiddb: bool = False,
    skip_entry_points: bool = False,
    skip_msvcrt: bool = False,
    skip_vftable_write: bool = False,
    skip_strings: bool = False,
    skip_thisptr: bool = False,
    skip_constant_this: bool = False,
    skip_callgraph: bool = False,
    skip_eh_funclets: bool = False,
    callgraph_ambiguity: int = 0,
    constant_this_min_sites: int = 1,
    constant_this_dominance: float = 1.0,
) -> None:
    _refuse_if_locked(project)
    scripts: list[list[str]] = []
    if not skip_fiddb:
        scripts.append(["ApplyFidDb.java"])
    if not skip_entry_points:
        scripts.append(["AssignEntryPoints.java"])
    if not skip_msvcrt:
        scripts.append(["PromoteMsvcrt.java"])
    scripts.append(["PromoteVftableMembers.java"])
    if not skip_vftable_write:
        scripts.append(["PromoteByVtableWrite.java"])
    if not skip_strings:
        scripts.append(["AssignByStringRefs.java"])
    if not skip_thisptr:
        scripts.append(["AssignByThisPointerType.java"])
    if not skip_constant_this:
        scripts.append([
            "PromoteByConstantThis.java",
            str(constant_this_min_sites),
            str(constant_this_dominance),
        ])
    if not skip_callgraph:
        scripts.append(["PropagateCallerNamespaces.java", str(callgraph_ambiguity)])
    # EH funclets run AFTER class-based passes so the spatial parent
    # search can find class-anchored neighbours.
    if not skip_eh_funclets:
        scripts.append(["PromoteEhFunclets.java"])
    # Refresh mapping.csv + the module-map inputs so docs/MODULES.md can
    # be regenerated by build_module_map.py without going back to MCP.
    scripts.append(["GenerateMapping.java", mapping_out])
    scripts.append(["DumpFunctionsCsv.java",
                    str(REPO_ROOT / "config" / "bulanci" / "ghidra_functions_dump.csv")])
    scripts.append(["DumpVftablesCsv.java",
                    str(REPO_ROOT / "config" / "bulanci" / "vftable_methods.csv")])

    print(
        f"[ghidra] running {len(scripts)} promotion pass(es) against "
        f"{project}.gpr (writeable). This persists renames to the project."
    )
    for s in scripts:
        print(f"  - {s[0]}")
    ghidra_helpers.runAnalyze(
        str(REPO_ROOT),
        project,
        process=program,
        analysis=False,
        read_only=False,
        post_scripts=scripts,
    )


def main() -> int:
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("--project", default="bulanci")
    parser.add_argument("--program", default="bulanci.exe")
    parser.add_argument(
        "--mapping",
        default=str(REPO_ROOT / "config" / "bulanci" / "mapping.csv"),
        help="Output path for the regenerated mapping.csv",
    )
    parser.add_argument(
        "--skip-fiddb",
        action="store_true",
        help="Skip ApplyFidDb (no FunctionID database lookup).",
    )
    parser.add_argument(
        "--skip-entry-points",
        action="store_true",
        help="Skip AssignEntryPoints (no WinMain / WndProc / DialogProc tagging).",
    )
    parser.add_argument(
        "--skip-msvcrt",
        action="store_true",
        help="Skip PromoteMsvcrt (no Runtime::MSVCRT carve-out for CRT helpers).",
    )
    parser.add_argument(
        "--skip-vftable-write",
        action="store_true",
        help="Skip PromoteByVtableWrite (no constructor detection via vftable stores).",
    )
    parser.add_argument(
        "--skip-strings",
        action="store_true",
        help="Skip AssignByStringRefs (no string-driven ownership).",
    )
    parser.add_argument(
        "--skip-thisptr",
        action="store_true",
        help="Skip AssignByThisPointerType (no first-param-class reparenting).",
    )
    parser.add_argument(
        "--skip-constant-this",
        action="store_true",
        help="Skip PromoteByConstantThis (no call-site arg0 type aggregation).",
    )
    parser.add_argument(
        "--constant-this-min-sites",
        type=int,
        default=1,
        help=(
            "Minimum number of typed call sites that must agree on a class "
            "before PromoteByConstantThis reparents a callee. Default: 1."
        ),
    )
    parser.add_argument(
        "--constant-this-dominance",
        type=float,
        default=1.0,
        help=(
            "Fraction of *typed* call sites that must agree (untyped sites "
            "are ignored). Default: 1.0 (every typed site must agree)."
        ),
    )
    parser.add_argument(
        "--skip-callgraph",
        action="store_true",
        help="Skip PropagateCallerNamespaces (no caller-dominated reparenting).",
    )
    parser.add_argument(
        "--skip-eh-funclets",
        action="store_true",
        help="Skip PromoteEhFunclets (no Catch@/Unwind@ adjacent-parent reparenting).",
    )
    parser.add_argument(
        "--callgraph-ambiguity",
        type=int,
        default=0,
        help=(
            "Maximum number of strangers tolerated in the caller set before "
            "PropagateCallerNamespaces refuses to assign a function. Higher "
            "values promote more aggressively but risk false attributions. "
            "Default: 0 (every caller must be in the same class)."
        ),
    )
    args = parser.parse_args()

    os.chdir(REPO_ROOT)
    promoteNamespaces(
        project=args.project,
        program=args.program,
        mapping_out=args.mapping,
        skip_fiddb=args.skip_fiddb,
        skip_entry_points=args.skip_entry_points,
        skip_msvcrt=args.skip_msvcrt,
        skip_vftable_write=args.skip_vftable_write,
        skip_strings=args.skip_strings,
        skip_thisptr=args.skip_thisptr,
        skip_constant_this=args.skip_constant_this,
        skip_callgraph=args.skip_callgraph,
        skip_eh_funclets=args.skip_eh_funclets,
        callgraph_ambiguity=args.callgraph_ambiguity,
        constant_this_min_sites=args.constant_this_min_sites,
        constant_this_dominance=args.constant_this_dominance,
    )
    print(
        "\nDone. Next steps:\n"
        "  python scripts/internal/build_module_map.py     "
        "# refresh docs/MODULES.md from the new dumps\n"
        "  python scripts/internal/seed_units_listing.py   "
        "# splits _Globals along the new namespaces\n"
        "  python scripts/configure.py --ghidra-mode existing  "
        "# regenerates stubs, COFFs, objdiff.json, build.ninja\n"
        "  (then recompile and run progress_update.bat for a fresh report)"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
