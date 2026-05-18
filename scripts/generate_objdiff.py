from pathlib import Path
import json
import os
from helpers import has_functions
from project import DecompUnit

WORKSPACE_PATH = Path(__file__).parent.parent


def generateObjdiffConfig(
    decompUnits: list[DecompUnit],
    master_target_dir: Path,
    master_base_dir: Path,
    output_dir: Path,
):
    # Auto-detect whether the maintainer has dropped a real cl.exe in
    # tools/msvc8/. When present, let objdiff invoke ninja to rebuild base
    # objects; otherwise stay in first-run mode (base_path null + build_base
    # false), which keeps the report at an honest 0 % and avoids broken
    # objdiff sessions complaining about missing compilers.
    cl_path = Path(os.environ.get("BULANCI_CL", "tools/msvc8/Bin/cl.exe"))
    base_build_available = cl_path.exists()

    config = {
        "$schema": "https://raw.githubusercontent.com/encounter/objdiff/main/config.schema.json",
        "custom_make": "ninja",
        "target_dir": str(master_target_dir),
        "base_dir": str(master_base_dir),
        # build_target stays false: target objs come from Ghidra ExportDelinker,
        # not from a ninja edge - objdiff has no way to invoke that itself.
        "build_target": False,
        "build_base": base_build_available,
        "watch_patterns": ["*.c", "*.cpp", "*.h", "*.hpp"],
    }

    progress_categories = []
    seen_categories = set()
    for decompUnit in decompUnits:
        if decompUnit.progress_category in seen_categories:
            continue
        progress_categories.append({
            "id": decompUnit.progress_category,
            "name": decompUnit.progress_category,
        })
        seen_categories.add(decompUnit.progress_category)
    config["progress_categories"] = progress_categories

    conf_units = []
    for decompUnit in decompUnits:
        for unit, namespaces in decompUnit.units.items():
            if not has_functions(namespaces, decompUnit.mappings):
                continue
            target_path = decompUnit.buildOrig / (unit + ".obj")
            base_path = decompUnit.buildSrc / (unit + ".obj")

            conf_unit = {
                "name": decompUnit.directory_name + "/" + unit,
                "target_path": str(target_path),
                # base_path is null while the source-side build is not wired
                # up yet; objdiff treats this as 'no base, 0% progress'.
                "base_path": str(base_path) if base_path.exists() else None,
                "reverse_fn_order": False,
            }

            metadata = {"progress_categories": [decompUnit.progress_category]}
            if unit in decompUnit.completed:
                metadata["complete"] = True
            conf_unit["metadata"] = metadata

            conf_units.append(conf_unit)

    config["units"] = conf_units

    with (output_dir / "objdiff.json").open("w") as file:
        json.dump(config, file, indent=4)


if __name__ == "__main__":
    os.chdir(WORKSPACE_PATH)
    unit = DecompUnit("Game code", "bulanci", "bulanci.exe")
    generateObjdiffConfig(
        [unit],
        WORKSPACE_PATH / "build" / "orig",
        WORKSPACE_PATH / "build" / "Src",
        WORKSPACE_PATH,
    )
