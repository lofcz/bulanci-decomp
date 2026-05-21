#!/usr/bin/env python3
"""Find STUB functions whose target body is `mov eax, offset DAT_xxx; ret`.

These are 6-byte global-pointer getters and are trivially matched.
"""
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
            if name in stub_funcs.get(unit_name, set()) and len(body) == 2:
                # mov eax, offset DAT/PTR_xxx; ret
                m1 = re.search(r"mov\s+eax,\s*offset\s+(\S+)\s*$", body[0])
                if m1 and re.search(r"ret\s*$", body[1]):
                    results.append((name, m1.group(1)))
            i = max(j, i + 1)
        else:
            i += 1
    return results


total = 0
for unit_name in sorted(stub_funcs):
    obj_path = ORIG / (unit_name.split("/")[-1] + ".obj")
    res = scan(obj_path, unit_name)
    for name, sym in res:
        print(f"{unit_name}\t{name}\t&{sym}")
        total += 1
print(f"\nTOTAL: {total} candidates")
