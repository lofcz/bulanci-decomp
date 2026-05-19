"""Attach the NSA-published FidDb databases to the local Ghidra install.

FidFile registration is a user-level preference - once attached, every
subsequent headless run (and the GUI) can use the database without
re-registering. Idempotent: running this twice is harmless.

This script runs Ghidra headless against a THROWAWAY project under
``%TEMP%``, so it works while the real ``bulanci.gpr`` is open in the
GUI.

Required environment:
  * GHIDRA_HOME  - root of the Ghidra installation
  * GHIDRA_DATA  - root of the NSA `ghidra-data` checkout (optional;
                   defaults to ``../ghidra-data``). Must contain a
                   ``FunctionID/`` subdirectory with the .fidb files.

For ``bulanci.exe`` (x86, MSVC 8 RTM) the relevant FidDb is
``vsOlder_x86.fidb`` which covers VS6 - VS2010. The script attaches
that one by default and prints a summary of every active database.
"""

from __future__ import annotations

import argparse
import os
import shlex
import subprocess
import sys
import tempfile
from pathlib import Path

import ghidra_helpers

SCRIPT_PATH = Path(os.path.realpath(__file__)).parent
REPO_ROOT = SCRIPT_PATH.parent

DEFAULT_FIDBS = [
    "vsOlder_x86.fidb",    # VS6 - VS2010 x86 (covers VS2005 RTM / bulanci.exe)
]


def _discover_data_root(explicit: str | None) -> Path:
    if explicit:
        return Path(explicit).expanduser().resolve()
    env = os.environ.get("GHIDRA_DATA")
    if env:
        return Path(env).expanduser().resolve()
    sibling = REPO_ROOT.parent / "ghidra-data"
    if sibling.is_dir():
        return sibling.resolve()
    raise SystemExit(
        "Could not locate ghidra-data. Pass --data-root, set $GHIDRA_DATA, "
        "or clone https://github.com/NationalSecurityAgency/ghidra-data "
        "next to the bulanci repo."
    )


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument(
        "--data-root",
        default=None,
        help="Path to the ghidra-data checkout (overrides $GHIDRA_DATA).",
    )
    parser.add_argument(
        "--fidb",
        action="append",
        default=None,
        help="Specific .fidb filename to attach (may be repeated). "
             "Defaults to: " + ", ".join(DEFAULT_FIDBS),
    )
    args = parser.parse_args()

    data_root = _discover_data_root(args.data_root)
    fid_root = data_root / "FunctionID"
    if not fid_root.is_dir():
        raise SystemExit(f"No FunctionID/ directory under {data_root}")

    targets = args.fidb or DEFAULT_FIDBS
    resolved: list[Path] = []
    for t in targets:
        p = fid_root / t
        if not p.is_file():
            raise SystemExit(f"{p} not found - check --data-root / --fidb.")
        resolved.append(p)

    headless = ghidra_helpers.findAnalyzeHeadless()
    with tempfile.TemporaryDirectory(prefix="bulanci_fid_setup_") as tmp:
        tmp_path = Path(tmp)
        for fidb in resolved:
            cmd = [
                headless,
                str(tmp_path),
                "fid_setup",
                "-scriptPath", str(SCRIPT_PATH / "ghidra"),
                "-noanalysis",
                "-preScript", "AttachFidDb.java", str(fidb),
            ]
            print(f"[ghidra] attaching {fidb.name}")
            print("Running " + " ".join(shlex.quote(x) for x in cmd))
            subprocess.run(cmd, check=True)

    print(
        "\nDone. Next: run `python scripts/promote_namespaces.py` and the\n"
        "ApplyFidDb pass will start picking up matches."
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
