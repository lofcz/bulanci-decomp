"""Run Ghidra's ExportDelinker headlessly for each DecompUnit.

Two execution modes are supported:

* "existing" - reuse the repo-root Ghidra project (``bulanci.gpr`` +
  ``bulanci.rep``). Skips auto-analysis since the project is already
  analysed, so a re-export takes a handful of seconds. Requires the
  Ghidra GUI to be CLOSED (the project lock must be free).
* "tempdir" - import the binary into a throwaway Ghidra project under a
  temporary directory and run full auto-analysis. Always works (no lock
  needed) but takes 10-30 minutes for the full 22 MB ``bulanci.exe``.

The mode is auto-detected: if ``<unit>.gpr`` exists and no lock is held,
"existing" is preferred. Set ``BULANCI_GHIDRA_MODE=tempdir`` to force the
slow path (e.g. for CI).
"""

from __future__ import annotations

import os
import sys
import tempfile
from pathlib import Path

import ghidra_helpers
from project import DecompUnit

SCRIPT_PATH = Path(os.path.realpath(__file__)).parent


def _post_scripts(unit: DecompUnit) -> list[list[str]]:
    return [
        ["ImportFromCsv.java", str(unit.configPath / "mapping.csv")],
        [
            "ExportDelinker.java",
            str(unit.configPath / "units_listing.csv"),
            str(unit.buildOrig),
        ],
    ]


def _project_name(unit: DecompUnit) -> str:
    """Convention: <unit.directory_name>.gpr at repo root, e.g. bulanci.gpr."""
    return unit.directory_name


def _existing_project_available(repo_root: Path, unit: DecompUnit) -> tuple[bool, str]:
    """Return (ok, reason). If ok=False, reason explains the fallback."""
    project_name = _project_name(unit)
    gpr = repo_root / f"{project_name}.gpr"
    rep = repo_root / f"{project_name}.rep"
    if not gpr.exists() or not rep.exists():
        return False, f"no {project_name}.gpr / {project_name}.rep at {repo_root}"

    for lock_name in (f"{project_name}.lock", f"{project_name}.lock~"):
        if (repo_root / lock_name).exists():
            return False, (
                f"{lock_name} present - close the Ghidra GUI to use the fast path"
            )

    return True, ""


def _export_via_existing(repo_root: Path, unit: DecompUnit) -> None:
    print(
        f"[ghidra] reusing existing project {_project_name(unit)}.gpr "
        f"(skipping import + analysis)"
    )
    ghidra_helpers.runAnalyze(
        str(repo_root),
        _project_name(unit),
        process=Path(unit.filePath).name,
        analysis=False,
        post_scripts=_post_scripts(unit),
    )


def _export_via_tempdir(unit: DecompUnit) -> None:
    print(
        f"[ghidra] importing {unit.filePath} into a throwaway project "
        f"(this triggers full auto-analysis; may take 10-30 minutes)"
    )
    with tempfile.TemporaryDirectory() as tempdir:
        ghidra_helpers.runAnalyze(
            str(tempdir),
            "ProjectTempName",
            import_file=str(unit.filePath),
            analysis=True,
            post_scripts=_post_scripts(unit),
        )


def exportObjs(unit: DecompUnit, mode: str | None = None) -> None:
    os.makedirs(str(unit.buildOrig), exist_ok=True)

    repo_root = Path.cwd()
    forced = mode or os.environ.get("BULANCI_GHIDRA_MODE")
    if forced not in (None, "auto", "existing", "tempdir"):
        raise ValueError(
            f"invalid mode {forced!r}; expected one of: auto, existing, tempdir"
        )

    if forced == "tempdir":
        _export_via_tempdir(unit)
        return

    if forced == "existing":
        ok, reason = _existing_project_available(repo_root, unit)
        if not ok:
            raise RuntimeError(f"cannot use existing project: {reason}")
        _export_via_existing(repo_root, unit)
        return

    ok, reason = _existing_project_available(repo_root, unit)
    if ok:
        _export_via_existing(repo_root, unit)
    else:
        print(f"[ghidra] {reason}; falling back to tempdir mode", file=sys.stderr)
        _export_via_tempdir(unit)


if __name__ == "__main__":
    workspace = SCRIPT_PATH.parent
    os.chdir(workspace)
    unit = DecompUnit("Game code", "bulanci", "bulanci.exe")
    exportObjs(unit)
