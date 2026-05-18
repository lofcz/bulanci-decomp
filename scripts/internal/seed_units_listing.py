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


def _all_namespaces(qualified_name: str) -> str:
    """Return the qualified parent namespace, e.g.

        std::exception::exception  -> std::exception
        ATL::_AtlGetThreadACPFake  -> ATL
        _Globals::FUN_00401000     -> _Globals
    """
    parts = qualified_name.split("::")
    if len(parts) <= 1:
        return ""
    return "::".join(parts[:-1])


def seed(mapping_path: Path, units_path: Path, completed_path: Path) -> dict:
    if not mapping_path.exists():
        raise SystemExit(f"mapping.csv not found at {mapping_path} - run export_mapping_via_mcp.py first")

    with mapping_path.open("r", encoding="utf-8") as f:
        lines = [ln.rstrip("\n") for ln in f if ln.strip()]

    rewritten: list[str] = []
    # top-level -> count of functions
    namespace_counts: dict[str, int] = {}
    # top-level -> set of full namespace paths (e.g. {"std", "std::exception", "std::bad_alloc"})
    namespaces_by_unit: dict[str, set[str]] = {}
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
            namespace_counts[GLOBALS_UNIT] = namespace_counts.get(GLOBALS_UNIT, 0) + 1
            namespaces_by_unit.setdefault(GLOBALS_UNIT, set()).add(GLOBALS_UNIT)
        else:
            top = _top_namespace(qualified)
            namespace_counts[top] = namespace_counts.get(top, 0) + 1
            parent_ns = _all_namespaces(qualified)
            namespaces_by_unit.setdefault(top, set()).add(parent_ns)
        rewritten.append(";".join(parts))

    with mapping_path.open("w", encoding="utf-8", newline="\n") as f:
        for line in rewritten:
            f.write(line + "\n")

    units_path.parent.mkdir(parents=True, exist_ok=True)
    with units_path.open("w", encoding="utf-8", newline="\n") as f:
        for unit in sorted(namespaces_by_unit):
            # Sort nested namespaces shortest first (so the top-level appears
            # before its descendants) and keep all of them in the row -
            # helpers.has_functions() does exact-string membership checks.
            nested = sorted(namespaces_by_unit[unit], key=lambda s: (s.count("::"), s))
            f.write(unit + "," + ",".join(nested) + "\n")

    if not completed_path.exists():
        completed_path.parent.mkdir(parents=True, exist_ok=True)
        completed_path.touch()

    return {
        "globals_promoted": globals_promoted,
        "namespaces": namespace_counts,
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
