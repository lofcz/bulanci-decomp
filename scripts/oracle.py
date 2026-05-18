"""decomp-goal-harness oracle adapter for bulanci-decomp.

The harness's generic adapter calls three commands per unit:

* `--build`           regenerate `report.json` via `tools/objdiff-cli.exe`.
* `--score --unit U`  read `report.json`, locate unit `U` (matches against
                      both `bulanci/U` and `U`), and print the JSON shape
                      the harness expects:
                          {matched, score, exact_bytes, total_bytes,
                           matching_prefix_bytes, matching_prefix_percent,
                           first_mismatch_offset}
* `--diff --unit U`   try `objdiff-cli diff ...`; if that's not available
                      yet (e.g. base objs missing), fall back to a small
                      human-readable summary of the unit's measures.

For our first decomp.dev report there are no base `.obj` files, so
`matched`/`score` collapse to 0/0 and the harness records us at 0%.
That's the legitimate starting point.
"""
from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DEFAULT_REPORT = ROOT / "report.json"
DEFAULT_OBJDIFF = ROOT / "tools" / "objdiff-cli.exe"
DEFAULT_OBJDIFF_JSON = ROOT / "objdiff.json"


def _find_objdiff() -> Path:
    env = os.environ.get("OBJDIFF_CLI")
    if env:
        return Path(env)
    return DEFAULT_OBJDIFF


def _report_generate(objdiff_cli: Path, output: Path) -> None:
    if not objdiff_cli.exists():
        raise SystemExit(
            f"objdiff-cli not found at {objdiff_cli}. Download from "
            "https://github.com/encounter/objdiff/releases and drop the "
            "binary into tools/ (or set OBJDIFF_CLI)."
        )
    subprocess.run(
        [str(objdiff_cli), "report", "generate", "-o", str(output), "-f", "json-pretty"],
        check=True,
        cwd=str(ROOT),
    )


def _load_report(report_path: Path) -> dict:
    if not report_path.exists():
        raise SystemExit(f"{report_path} does not exist - run `oracle.py --build` first.")
    with report_path.open("r", encoding="utf-8") as f:
        return json.load(f)


def _find_unit(report: dict, unit: str) -> dict | None:
    units = report.get("units", [])
    candidates = {unit, f"bulanci/{unit}"}
    for u in units:
        name = u.get("name", "")
        if name in candidates:
            return u
        # also match suffixes so `--unit _Globals` works against `bulanci/_Globals`
        if name.endswith("/" + unit) or name == unit:
            return u
    return None


def _score_from_unit(unit_record: dict) -> dict:
    measures = unit_record.get("measures", {}) or {}
    total = int(measures.get("total_code", 0))
    matched_code = int(measures.get("matched_code", 0))
    fuzzy_pct = float(measures.get("fuzzy_match_percent", 0.0))
    matched = bool(measures.get("complete_code_percent", 0.0) >= 100.0) if total else False
    score = fuzzy_pct / 100.0 if fuzzy_pct else 0.0
    # objdiff reports prefix info per function, not per unit. Sum the
    # per-function prefix bytes when present, otherwise leave 0 / None.
    prefix_bytes = 0
    first_mismatch: int | None = None
    for fn in unit_record.get("functions", []) or []:
        m = fn.get("measures", {}) or {}
        prefix_bytes += int(m.get("matching_prefix_bytes", 0) or 0)
        fmo = m.get("first_mismatch_offset")
        if fmo is not None and first_mismatch is None:
            first_mismatch = int(fmo)
    prefix_pct = (prefix_bytes / total) if total else 0.0
    return {
        "matched": matched,
        "score": round(score, 6),
        "exact_bytes": matched_code,
        "total_bytes": total,
        "matching_prefix_bytes": prefix_bytes,
        "matching_prefix_percent": round(prefix_pct, 6),
        "first_mismatch_offset": first_mismatch,
        "unit": unit_record.get("name"),
    }


def cmd_build(args: argparse.Namespace) -> int:
    objdiff_cli = _find_objdiff()
    _report_generate(objdiff_cli, Path(args.report))
    return 0


def cmd_score(args: argparse.Namespace) -> int:
    report = _load_report(Path(args.report))
    unit_record = _find_unit(report, args.unit)
    if unit_record is None:
        print(json.dumps({
            "matched": False,
            "score": 0.0,
            "exact_bytes": 0,
            "total_bytes": 0,
            "matching_prefix_bytes": 0,
            "matching_prefix_percent": 0.0,
            "first_mismatch_offset": None,
            "unit": args.unit,
            "error": "unit not found in report.json",
        }, indent=2))
        return 1
    print(json.dumps(_score_from_unit(unit_record), indent=2))
    return 0


def cmd_diff(args: argparse.Namespace) -> int:
    objdiff_cli = _find_objdiff()
    if objdiff_cli.exists():
        try:
            unit_name = args.unit
            if "/" not in unit_name:
                unit_name = f"bulanci/{unit_name}"
            subprocess.run(
                [str(objdiff_cli), "diff", "--config", str(DEFAULT_OBJDIFF_JSON), "--unit", unit_name],
                check=False,
                cwd=str(ROOT),
            )
            return 0
        except FileNotFoundError:
            pass
    # Fallback: summarise from report.json so the harness still gets *something*.
    report = _load_report(Path(args.report))
    unit_record = _find_unit(report, args.unit)
    if unit_record is None:
        print(f"unit {args.unit} not found in {args.report}")
        return 1
    measures = unit_record.get("measures", {}) or {}
    print(f"unit: {unit_record.get('name')}")
    for key in ("total_code", "matched_code", "matched_code_percent",
                "fuzzy_match_percent", "complete_code", "complete_code_percent"):
        if key in measures:
            print(f"  {key}: {measures[key]}")
    return 0


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--report", default=str(DEFAULT_REPORT))
    parser.add_argument("--unit", default="_Globals")
    parser.add_argument("--build", action="store_true")
    parser.add_argument("--score", action="store_true")
    parser.add_argument("--diff", action="store_true")
    args = parser.parse_args(argv)

    if args.build:
        return cmd_build(args)
    if args.score:
        return cmd_score(args)
    if args.diff:
        return cmd_diff(args)

    parser.print_help()
    return 2


if __name__ == "__main__":
    sys.exit(main())
