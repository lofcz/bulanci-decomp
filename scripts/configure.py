import argparse
import os
from pathlib import Path

from generate_sources import generateSources
from generate_ninja import generateNinja
from generate_objdiff import generateObjdiffConfig
from project import DecompUnit


def configureProject(base_dir: Path, skip_ghidra: bool = False):
    os.chdir(base_dir)
    gameUnit = DecompUnit("Game code", "bulanci", "bulanci.exe")
    configure([gameUnit], skip_ghidra=skip_ghidra)


def configure(decompUnits: list[DecompUnit], skip_ghidra: bool = False):
    if not skip_ghidra:
        # Importing here so a missing Ghidra installation does not break
        # the source/objdiff/ninja generation in first-run mode.
        from export_ghidra_objs import exportObjs
        for unit in decompUnits:
            exportObjs(unit)

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
    args = parser.parse_args()
    configureProject(Path(__file__).parent.parent, skip_ghidra=args.skip_ghidra)
