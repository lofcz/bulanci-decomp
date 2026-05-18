import os
from pathlib import Path
import tempfile
from project import DecompUnit

import ghidra_helpers

SCRIPT_PATH = Path(os.path.realpath(__file__)).parent

def exportObjs(unit: DecompUnit):
    os.makedirs(str(unit.buildOrig), exist_ok=True)
    with tempfile.TemporaryDirectory() as tempdir:
        ghidra_helpers.runAnalyze(
            str(tempdir),
            "ProjectTempName",
            import_file=str(unit.filePath),
            analysis=True,
            post_scripts=[
                ["ImportFromCsv.java", str(unit.configPath/"mapping.csv")],
                [
                    "ExportDelinker.java",
                    str(unit.configPath/"units_listing.csv"),
                    str(unit.buildOrig),
                ],
            ],
        )

if __name__ == "__main__":
    workspace = SCRIPT_PATH.parent
    os.chdir(workspace)
    unit = DecompUnit("Game code", "bulanci", "bulanci.exe")
    exportObjs(unit)
