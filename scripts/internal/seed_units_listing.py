"""Seed an initial config/bulanci/units_listing.csv from mapping.csv.

Strategy: one TU per top-level namespace in mapping.csv, plus a single
`_Globals` TU collecting every function that lives at file scope (no
namespace).

For the global-scope entries we ALSO rewrite mapping.csv in place,
promoting `;FUN_xxx;...` rows to `;_Globals::FUN_xxx;...`. Without that
rename, `helpers.has_functions` would have no namespace to key the
`_Globals` unit on, and `generate_sources.py` would try to emit an
anonymous C++ class. Doing it here keeps Ghidra-derived names untouched
on disk until somebody explicitly re-runs the seeder.

Re-running this script after Ghidra learns more real namespaces is safe:
already-namespaced rows are left alone, the `_Globals` rename is
idempotent, and `units_listing.csv` is regenerated each time.
"""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

DEFAULT_MAPPING = Path(__file__).parent.parent.parent / "config" / "bulanci" / "mapping.csv"
DEFAULT_UNITS = Path(__file__).parent.parent.parent / "config" / "bulanci" / "units_listing.csv"
DEFAULT_COMPLETED = Path(__file__).parent.parent.parent / "config" / "bulanci" / "completed.csv"

GLOBALS_UNIT = "_Globals"


def _top_namespace(qualified_name: str) -> str:
    parts = qualified_name.split("::")
    if len(parts) <= 1:
        return ""
    return parts[0]


def seed(mapping_path: Path, units_path: Path, completed_path: Path) -> dict:
    if not mapping_path.exists():
        raise SystemExit(f"mapping.csv not found at {mapping_path} - run export_mapping_via_mcp.py first")

    with mapping_path.open("r", encoding="utf-8") as f:
        lines = [ln.rstrip("\n") for ln in f if ln.strip()]

    rewritten: list[str] = []
    namespaces: dict[str, int] = {}
    globals_promoted = 0

    for line in lines:
        parts = line.split(";")
        if len(parts) < 7:
            rewritten.append(line)
            continue
        qualified = parts[1]
        if "::" not in qualified:
            new_qualified = f"{GLOBALS_UNIT}::{qualified}" if qualified else f"{GLOBALS_UNIT}::"
            parts[1] = new_qualified
            globals_promoted += 1
            namespaces[GLOBALS_UNIT] = namespaces.get(GLOBALS_UNIT, 0) + 1
        else:
            top = _top_namespace(qualified)
            namespaces[top] = namespaces.get(top, 0) + 1
        rewritten.append(";".join(parts))

    with mapping_path.open("w", encoding="utf-8", newline="\n") as f:
        for line in rewritten:
            f.write(line + "\n")

    units_path.parent.mkdir(parents=True, exist_ok=True)
    with units_path.open("w", encoding="utf-8", newline="\n") as f:
        for unit in sorted(namespaces):
            f.write(f"{unit},{unit}\n")

    if not completed_path.exists():
        completed_path.parent.mkdir(parents=True, exist_ok=True)
        completed_path.touch()

    return {
        "globals_promoted": globals_promoted,
        "namespaces": namespaces,
    }


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--mapping", default=str(DEFAULT_MAPPING))
    parser.add_argument("--units", default=str(DEFAULT_UNITS))
    parser.add_argument("--completed", default=str(DEFAULT_COMPLETED))
    args = parser.parse_args(argv)

    info = seed(Path(args.mapping), Path(args.units), Path(args.completed))
    print(f"namespaces: {len(info['namespaces'])}")
    for ns, n in sorted(info["namespaces"].items()):
        print(f"  {ns}: {n}")
    print(f"_Globals promotions: {info['globals_promoted']}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
