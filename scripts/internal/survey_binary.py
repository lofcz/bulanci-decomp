"""Quick survey of bulanci.exe: imports, RTTI classes, strings, resources.

Run from repo root:

    python scripts/internal/survey_binary.py

Pure-Python, uses only pefile + the committed mapping.csv. No Ghidra
required. Prints a one-screen overview to help decide which agent
tasks are worth investing in.
"""
from __future__ import annotations

import csv
import re
from collections import Counter, defaultdict
from pathlib import Path

import pefile

REPO_ROOT = Path(__file__).resolve().parents[2]
EXE = REPO_ROOT / "orig" / "bulanci.exe"
MAPPING = REPO_ROOT / "config" / "bulanci" / "mapping.csv"


def load_mapping() -> list[tuple[str, int]]:
    rows: list[tuple[str, int]] = []
    with MAPPING.open(newline="", encoding="utf-8") as f:
        for row in csv.reader(f, delimiter=";"):
            if not row or len(row) < 4:
                continue
            try:
                size = int(row[3], 0)
            except ValueError:
                continue
            rows.append((row[1], size))
    return rows


def main() -> int:
    if not EXE.exists():
        print(f"missing {EXE}")
        return 1
    pe = pefile.PE(str(EXE), fast_load=False)

    # 1. import table
    by_dll: dict[str, list[str]] = defaultdict(list)
    for entry in pe.DIRECTORY_ENTRY_IMPORT:
        dll = entry.dll.decode("ascii", "replace").lower()
        for imp in entry.imports:
            name = imp.name.decode("ascii", "replace") if imp.name else f"ord_{imp.ordinal}"
            by_dll[dll].append(name)
    print("== IMPORTS ==")
    for dll, names in sorted(by_dll.items(), key=lambda kv: -len(kv[1])):
        print(f"  {dll:<20s} {len(names):>3d} symbols")
    print()

    # 2. resources
    res_types = Counter()
    if hasattr(pe, "DIRECTORY_ENTRY_RESOURCE"):
        for top in pe.DIRECTORY_ENTRY_RESOURCE.entries:
            tname = pefile.RESOURCE_TYPE.get(top.id, str(top.id)) if top.id is not None else str(top.name)
            count = len(top.directory.entries) if hasattr(top, "directory") else 1
            res_types[tname] += count
    print("== RESOURCES ==")
    for t, c in res_types.most_common():
        print(f"  {t:<22s} {c}")
    print()

    # 3. rough RTTI / function snapshot
    rows = load_mapping()
    by_ns: dict[str, list[tuple[str, int]]] = defaultdict(list)
    unnamed = 0
    total_code = 0
    for name, size in rows:
        total_code += size
        if "::" in name:
            ns = name.rsplit("::", 1)[0]
        else:
            ns = "_Globals"
        leaf = name.rsplit("::", 1)[-1]
        if leaf.startswith("FUN_"):
            unnamed += 1
        by_ns[ns].append((leaf, size))
    print("== FUNCTIONS ==")
    print(f"  total:           {len(rows)} ({total_code:,} B)")
    print(f"  still FUN_xxx:   {unnamed} ({100*unnamed/len(rows):.1f}%)")
    print(f"  distinct nses:   {len(by_ns)}")
    print()

    print("== LARGEST CLASSES (by total code mass) ==")
    ranked = sorted(by_ns.items(), key=lambda kv: -sum(s for _, s in kv[1]))[:15]
    for ns, fns in ranked:
        bytes_ = sum(s for _, s in fns)
        named = sum(1 for n, _ in fns if not n.startswith("FUN_"))
        print(f"  {ns:<28s} {bytes_:>7,} B  {len(fns):>4d} fns  ({named} named)")
    print()

    # 4. czech / english strings already in mapping (heuristic: scan exe)
    data = EXE.read_bytes()
    strings = [s.decode("ascii", "replace") for s in re.findall(rb"[\x20-\x7e]{6,}", data)]
    interesting = [
        s for s in strings
        if re.search(r"\b(CDS|game|level|score|player|server|client|sound|video|stream|mpx|eap|wav|jpeg|gzip)\b", s, re.I)
    ]
    print(f"== ENGINE-FLAVOUR STRINGS ({len(interesting)} hits, showing 25) ==")
    for s in interesting[:25]:
        print(f"  {s[:90]}")


if __name__ == "__main__":
    main()
