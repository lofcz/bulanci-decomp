import argparse
import os
from pathlib import Path

from generate_sources import generateSources
from generate_ninja import generateNinja
from generate_objdiff import generateObjdiffConfig
from project import DecompUnit


def configureProject(
    base_dir: Path,
    skip_ghidra: bool = False,
    ghidra_mode: str | None = None,
):
    os.chdir(base_dir)
    gameUnit = DecompUnit("Game code", "bulanci", "bulanci.exe")
    configure([gameUnit], skip_ghidra=skip_ghidra, ghidra_mode=ghidra_mode)


def configure(
    decompUnits: list[DecompUnit],
    skip_ghidra: bool = False,
    ghidra_mode: str | None = None,
):
    if not skip_ghidra:
        # Importing here so a missing Ghidra installation does not break
        # the source/objdiff/ninja generation in first-run mode.
        from export_ghidra_objs import exportObjs
        for unit in decompUnits:
            exportObjs(unit, mode=ghidra_mode)

    for unit in decompUnits:
        generateSources(unit)

    generateNinja(decompUnits, Path("."))
    generateObjdiffConfig(decompUnits, Path("./build/orig"), Path("./build/Src"), Path("."))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Configure the bulanci decomp pipeline.")
    parser.add_argument(
        "--skip-ghidra",
        action="store_true",
        help="Skip the Ghidra headless ExportDelinker step (use for first-run / when target objs already exist).",
    )
    parser.add_argument(
        "--ghidra-mode",
        choices=["auto", "existing", "tempdir"],
        default=None,
        help=(
            "How to run Ghidra: 'existing' (reuse repo-root project; requires Ghidra GUI closed), "
            "'tempdir' (slow re-import + auto-analysis; always works), or 'auto' (default, prefers existing)."
        ),
    )
    args = parser.parse_args()
    configureProject(
        Path(__file__).parent.parent,
        skip_ghidra=args.skip_ghidra,
        ghidra_mode=args.ghidra_mode,
    )
