"""Promote RTTI-discovered virtual methods into their class namespace.

Ghidra's RTTIAnalyzer recovers `<Class>::vftable` symbols but leaves the
functions inside in the Global namespace, which means `mapping.csv` ends
up with nearly everything bucketed under `_Globals`. This wrapper drives
``PromoteVftableMembers.java`` against the repo-root project, then
re-runs ``GenerateMapping.java`` so the new namespace prefixes flow into
``config/bulanci/mapping.csv``. Run it once any time you re-import the
binary or after a fresh round of Ghidra RTTI analysis.

Unlike ``update_mapping.py``, this script opens the project **writable**
so the renames persist to ``bulanci.rep``. Close the Ghidra GUI before
running it; the lock check below will refuse to clobber an open project.

Usage:
    python scripts/promote_vftables.py                 \
        --ghidra-home C:\\tools\\ghidra                  \
        --project   bulanci                            \
        --program   bulanci.exe                        \
        --mapping   config/bulanci/mapping.csv
"""

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


def promoteVftables(project: str, program: str, mapping_out: str) -> None:
    _refuse_if_locked(project)
    print(
        f"[ghidra] reparenting RTTI virtual methods in {project}.gpr "
        "and refreshing mapping.csv (this saves changes back to the project)"
    )
    # One headless run: first promote namespaces (writeable), then
    # regenerate mapping.csv as a postscript so callers do not need to
    # invoke `update_mapping.py` separately.
    ghidra_helpers.runAnalyze(
        str(REPO_ROOT),
        project,
        process=program,
        analysis=False,
        read_only=False,
        post_scripts=[
            ["PromoteVftableMembers.java"],
            ["GenerateMapping.java", mapping_out],
        ],
    )


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--project",
        default="bulanci",
        help="Ghidra project name (without .gpr extension); defaults to 'bulanci'",
    )
    parser.add_argument(
        "--program",
        default="bulanci.exe",
        help="Binary file name as imported into the project",
    )
    parser.add_argument(
        "--mapping",
        default=str(REPO_ROOT / "config" / "bulanci" / "mapping.csv"),
        help="Output path for the regenerated mapping.csv",
    )
    args = parser.parse_args()

    os.chdir(REPO_ROOT)
    promoteVftables(args.project, args.program, args.mapping)
    print(
        "\nDone. Next steps:\n"
        "  python scripts/internal/seed_units_listing.py   "
        "# splits _Globals along the new namespaces\n"
        "  python scripts/configure.py --skip-ghidra        "
        "# regenerates stubs, objdiff.json, build.ninja\n"
        "  (rebuild & run progress_update.bat for a fresh report)"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
