#!/usr/bin/env python3
"""Find simple 2-3 instruction unmatched functions across all units."""
import json
import re
import subprocess
from pathlib import Path

ORIG = Path("build/orig/bulanci")

with open("report.json", "r") as f:
    report = json.load(f)

stub_funcs: dict[str, set[str]] = {}
for unit in report["units"]:
    name = unit["name"]
    stub_funcs[name] = {f["name"] for f in unit.get("functions", [])
                       if f.get("fuzzy_match_percent", 0) < 100.0}


def scan(obj_path: Path, unit_name: str):
    if not obj_path.exists():
        return []
    out = subprocess.run(
        ["tools/msvc8/Bin/dumpbin.exe", "/disasm", str(obj_path)],
        capture_output=True, text=True, encoding="utf-8")
    lines = out.stdout.splitlines()
    results = []
    i = 0
    while i < len(lines):
        line = lines[i]
        m = re.match(r"^([A-Za-z_][\w:]*):", line)
        if m:
            name = m.group(1)
            body = []
            j = i + 1
            while j < len(lines) and (lines[j].startswith("  ") or lines[j].strip() == ""):
                if lines[j].startswith("  "):
                    body.append(lines[j].strip())
                j += 1
            if name in stub_funcs.get(unit_name, set()) and 2 <= len(body) <= 3:
                results.append((name, body))
            i = max(j, i + 1)
        else:
            i += 1
    return results


count = 0
for unit_name in sorted(stub_funcs):
    obj_path = ORIG / (unit_name.split("/")[-1] + ".obj")
    res = scan(obj_path, unit_name)
    for name, body in res:
        # strip the byte-prefix and offset
        clean = []
        for b in body:
            m = re.match(r"^[0-9A-F]+:\s+[0-9A-F ]+\s+(.+)$", b)
            clean.append(m.group(1) if m else b)
        print(f"{unit_name}\t{name}")
        for c in clean:
            print(f"  {c}")
        count += 1
print(f"\nTOTAL: {count}")
