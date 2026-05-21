#!/usr/bin/env python3
"""Scan all target .obj files for trivially-implementable function bodies.

Looks for three patterns from the target disassembly:
  P1: `mov al, K; ret`            (3 bytes)  -> return K
  P2: `mov eax, K32; ret`         (6 bytes)  -> return K
  P3: `mov eax, offset DAT_X; ret` (6 bytes)  -> return reinterpret_cast<...>(&DAT_X)
  P4: `xor al, al; ret`           (3 bytes)  -> return 0
  P5: `xor eax, eax; ret`         (3 bytes)  -> return 0
"""
import json
import re
import subprocess
from pathlib import Path

ORIG = Path("build/orig/bulanci")
SRC = Path("src/bulanci")

with open("report.json", "r") as f:
    report = json.load(f)

unmatched: dict[str, set[str]] = {}
for unit in report["units"]:
    name = unit["name"]
    unmatched[name] = set()
    for f in unit.get("functions", []):
        if f.get("fuzzy_match_percent", 0) < 100.0:
            unmatched[name].add(f["name"])


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
            j = i + 1
            body = []
            while j < len(lines) and (lines[j].startswith("  ") or lines[j].strip() == ""):
                if lines[j].startswith("  "):
                    body.append(lines[j].strip())
                j += 1
            symname = f"{unit_name.split('/')[-1] if '::' not in name else name}"
            qualified = name
            if qualified in unmatched.get(unit_name, set()):
                # Try to match a simple pattern
                if len(body) == 2:
                    a, b = body[0], body[1]
                    if "ret" in b:
                        # Extract constant or symbol from line a
                        ma = re.search(r"^([0-9A-F]+):\s+([0-9A-F ]+)\s+([a-z]+)\s+(.*)", a)
                        if ma:
                            mnem = ma.group(3)
                            args = ma.group(4)
                            # ret arg
                            mret = re.search(r"^([0-9A-F]+):\s+([0-9A-F ]+)\s+ret\s*(\d*)", b)
                            ret_n = mret.group(3) if mret else ""
                            results.append((qualified, mnem, args, ret_n, len(body)))
        else:
            j = i + 1
        i = max(j, i + 1)
    return results


for unit_name, unit_data in sorted(unmatched.items()):
    if not unit_data:
        continue
    obj_path = ORIG / (unit_name.split("/")[-1] + ".obj")
    res = scan(obj_path, unit_name)
    for name, mnem, args, ret_n, body_len in res:
        print(f"{unit_name}\t{name}\t{mnem} {args}\tret {ret_n}\t({body_len} insns)")
