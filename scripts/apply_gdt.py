"""Apply one or more Ghidra `.gdt` data-type archives to bulanci.gpr.

By default, applies every committed .gdt that ships under
`tools/*/gdt/*.gdt`.  Use `--gdt` to apply a specific archive.

Modes:
    --mode tempdir   (default) copy the .gpr into a temp dir so the
                     script works even when the GUI is open. Useful
                     for diagnostics; does not mutate the user's
                     working copy.
    --mode existing  apply directly to ./bulanci.gpr.  Caller must
                     guarantee Ghidra is closed.
"""
from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path
from typing import List

REPO_ROOT = Path(__file__).resolve().parents[1]
SCRIPT_DIR = REPO_ROOT / "scripts" / "ghidra"


def _discover_gdts() -> List[Path]:
    """Find every committed .gdt across tools/<sdk>/gdt/."""
    found: List[Path] = []
    for d in (REPO_ROOT / "tools").iterdir():
        gdt_dir = d / "gdt"
        if gdt_dir.is_dir():
            found.extend(sorted(gdt_dir.glob("*.gdt")))
    return found


def _ghidra_apply(project_dir: Path, project_name: str, gdts: List[Path],
                  read_only: bool) -> int:
    ghidra_home = os.environ.get("GHIDRA_HOME") or "C:\\tools\\ghidra"
    analyze = Path(ghidra_home) / "support" / "analyzeHeadless.bat"
    if not analyze.exists():
        sys.exit(f"analyzeHeadless.bat missing: {analyze}")
    cmd = [
        str(analyze),
        str(project_dir), project_name,
        "-process", "bulanci.exe",
        "-noanalysis",
        "-scriptPath", str(SCRIPT_DIR),
    ]
    # Chain one -postScript per .gdt so each gets resolved into the
    # DTM in sequence under its own transaction.
    for g in gdts:
        cmd += ["-postScript", "ApplyDxGdt.java", str(g)]
    if read_only:
        cmd.append("-readOnly")
    print("    " + " ".join(cmd))
    proc = subprocess.run(cmd, text=True, capture_output=True, check=False)
    if proc.stdout:
        sys.stdout.write(proc.stdout)
    if proc.stderr:
        sys.stderr.write(proc.stderr)
    return proc.returncode


def _apply_tempdir(gdts: List[Path]) -> int:
    src_gpr = REPO_ROOT / "bulanci.gpr"
    src_rep = REPO_ROOT / "bulanci.rep"
    if not src_gpr.exists() or not src_rep.exists():
        sys.exit("bulanci.gpr / bulanci.rep not found - "
                 "run scripts/configure.py once first")
    with tempfile.TemporaryDirectory(prefix="apply_gdt_") as tmp:
        tmp_path = Path(tmp)
        shutil.copy2(src_gpr, tmp_path / "bulanci.gpr")
        shutil.copytree(src_rep, tmp_path / "bulanci.rep")
        return _ghidra_apply(tmp_path, "bulanci", gdts, read_only=True)


def _apply_existing(gdts: List[Path]) -> int:
    if not (REPO_ROOT / "bulanci.gpr").exists():
        sys.exit("bulanci.gpr not found - run scripts/configure.py once first")
    return _ghidra_apply(REPO_ROOT, "bulanci", gdts, read_only=False)


def main(argv: List[str]) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--mode", choices=("tempdir", "existing"),
                        default="tempdir")
    parser.add_argument(
        "--gdt", type=Path, action="append",
        help="apply a specific .gdt; can be repeated. "
             "Default: apply every tools/*/gdt/*.gdt.",
    )
    args = parser.parse_args(argv)

    gdts = args.gdt if args.gdt else _discover_gdts()
    if not gdts:
        sys.exit("no .gdt archives found - run scripts/build_gdt.py first")
    for g in gdts:
        if not g.exists():
            sys.exit(f"{g} not found")
    print(f"applying {len(gdts)} archive(s):")
    for g in gdts:
        print(f"  - {g.relative_to(REPO_ROOT) if g.is_relative_to(REPO_ROOT) else g}")

    if args.mode == "tempdir":
        return _apply_tempdir(gdts)
    return _apply_existing(gdts)


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
