"""Pick the next-best unit for a matching agent to attempt.

The decomp-goal-harness built-in `pick` only knows about DTK-style
projects with `Object(NonMatching, ...)` entries in configure.py. We
use the generic adapter, so it falls back to whatever's in
``decomp-goal.toml`` as ``default_unit``.

This script consumes ``report.json`` (produced by ``objdiff-cli``) and
ranks the unmatched units by a banteg-style cost/value heuristic:

  cost  = log(unit total_code_bytes + 1)            # bigger units = harder
  value = -unit_complete_code_percent                # less complete = better target
  bonus = isolation_factor                          # fewer callers = leaf

Output is one of:

  * a human-readable table (default),
  * a single bare unit name (with ``--print-unit``),
  * a ready-to-run ``decomp-goal goal`` command (with ``--command``).

Use it as the front end to a matching-agent loop:

    for unit in $(python scripts/pick_unit.py --top 5 --print-unit); do
        decomp-goal goal --repo . --unit "$unit"
    done

Filtering knobs:

  --exclude-globals    Skip _Globals (always huge, never the right
                       first target).
  --min-functions N    Skip units with fewer than N functions.
  --max-bytes N        Skip units larger than N total_code bytes.
  --pattern S          Restrict to units whose name contains S.
"""

from __future__ import annotations

import argparse
import json
import math
import os
import sys
from pathlib import Path
from typing import Any

REPO_ROOT = Path(os.path.realpath(__file__)).resolve().parents[1]
REPORT_FILE = REPO_ROOT / "report.json"


def _coerce_int(v: Any) -> int:
    try:
        return int(v)
    except (TypeError, ValueError):
        return 0


def _coerce_float(v: Any) -> float:
    try:
        return float(v)
    except (TypeError, ValueError):
        return 0.0


def _score(unit: dict[str, Any]) -> tuple[float, dict[str, Any]]:
    """Return (score, summary). Higher score = better next target."""
    m = unit.get("measures", {}) or {}
    total = _coerce_int(m.get("total_code"))
    fns = _coerce_int(m.get("total_functions"))
    matched_pct = _coerce_float(m.get("matched_code_percent"))
    complete_pct = _coerce_float(m.get("complete_code_percent"))
    fuzzy_pct = _coerce_float(m.get("fuzzy_match_percent"))

    if total == 0 or fns == 0:
        return -1.0, {"reason": "empty unit"}

    # Headroom: how much room remains to improve.
    headroom = max(0.0, 100.0 - matched_pct)

    # Size cost: log so 30 KB doesn't dominate 300 B too much, but
    # smaller units still rank higher when other factors tie.
    size_cost = math.log10(total + 1)

    # Density bonus: fewer functions per byte means each match is
    # higher-impact. Avoids wasting agents on units that are bags of
    # tiny stubs.
    avg_fn_size = total / fns

    score = (
        headroom * 1.0
        - size_cost * 5.0
        + min(20.0, avg_fn_size / 50.0)
        + fuzzy_pct * 0.1       # almost-matched units climb to the top
    )
    return score, {
        "total_code": total,
        "total_functions": fns,
        "matched_code_percent": round(matched_pct, 2),
        "complete_code_percent": round(complete_pct, 2),
        "fuzzy_match_percent": round(fuzzy_pct, 2),
        "avg_fn_size_bytes": round(avg_fn_size, 1),
        "headroom_percent": round(headroom, 2),
    }


def _unit_name(u: dict[str, Any]) -> str:
    """Strip the ``bulanci/`` prefix that objdiff prepends."""
    name = str(u.get("name") or "")
    if "/" in name:
        return name.split("/", 1)[1]
    return name


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--report", default=str(REPORT_FILE),
                        help="Path to report.json (default: ./report.json)")
    parser.add_argument("--top", type=int, default=10,
                        help="How many units to print (default 10).")
    parser.add_argument("--exclude-globals", action="store_true",
                        help="Hide the _Globals super-unit.")
    parser.add_argument("--include-complete", action="store_true",
                        help="Include units already 100%% matched.")
    parser.add_argument("--min-functions", type=int, default=1)
    parser.add_argument("--max-bytes", type=int, default=0,
                        help="0 means no cap.")
    parser.add_argument("--pattern", default=None,
                        help="Substring filter on the unit name.")
    parser.add_argument("--print-unit", action="store_true",
                        help="Just print unit names, one per line.")
    parser.add_argument("--command", action="store_true",
                        help="Print a ready-to-run `decomp-goal goal` command per unit.")
    parser.add_argument("--codex", action="store_true",
                        help="Print a `decomp-goal codex` command instead.")
    parser.add_argument("--reasoning-effort", default="high",
                        help="Used with --codex (default 'high'; use 'xhigh' for last-mile).")
    args = parser.parse_args()

    report_path = Path(args.report)
    if not report_path.exists():
        print(f"missing {report_path}; run `progress_update.bat` first",
              file=sys.stderr)
        return 1
    report = json.loads(report_path.read_text(encoding="utf-8"))

    scored: list[tuple[float, str, dict[str, Any]]] = []
    for u in report.get("units", []):
        name = _unit_name(u)
        if args.exclude_globals and name == "_Globals":
            continue
        if args.pattern and args.pattern not in name:
            continue
        score, summary = _score(u)
        if score < 0:
            continue
        if summary["total_functions"] < args.min_functions:
            continue
        if args.max_bytes and summary["total_code"] > args.max_bytes:
            continue
        if not args.include_complete and summary["complete_code_percent"] >= 100.0:
            continue
        scored.append((score, name, summary))

    scored.sort(key=lambda t: -t[0])
    scored = scored[: args.top]

    if not scored:
        print("(no candidate units after filters)", file=sys.stderr)
        return 1

    if args.print_unit:
        for _, name, _ in scored:
            print(name)
        return 0

    if args.command or args.codex:
        for _, name, _ in scored:
            if args.codex:
                print(
                    f'decomp-goal codex --repo "{REPO_ROOT}" --unit {name} '
                    f'--name "{name}" --reasoning-effort {args.reasoning_effort}'
                )
            else:
                print(f'decomp-goal goal --repo "{REPO_ROOT}" --unit {name} --name "{name}"')
        return 0

    print(f"# {len(scored)} candidate unit(s), ranked by next-best-target score")
    print(f"# {'unit':<25s} {'score':>7s}  {'bytes':>7s}  {'fns':>4s}  "
          f"{'avg':>5s}  {'matched%':>8s}  {'fuzzy%':>7s}")
    for score, name, s in scored:
        print(
            f"  {name:<25s} {score:>7.1f}  "
            f"{s['total_code']:>7d}  {s['total_functions']:>4d}  "
            f"{s['avg_fn_size_bytes']:>5.0f}  "
            f"{s['matched_code_percent']:>8.2f}  "
            f"{s['fuzzy_match_percent']:>7.2f}"
        )
    return 0


if __name__ == "__main__":
    sys.exit(main())
