#!/usr/bin/env python3
import json
import subprocess
import sys

unit, symbol = sys.argv[1], sys.argv[2]
target = f"build/orig/{unit}.obj"
base = f"build/Src/{unit}.obj"
out = subprocess.check_output(
    ["tools/objdiff-cli.exe", "diff",
     "-1", target, "-2", base, "--format", "json", "-o", "-", symbol],
    text=True, encoding="utf-8")
data = json.loads(out)

for side_name in ["left", "right"]:
    for s in data[side_name].get("symbols", []):
        if symbol in s.get("name", ""):
            print(side_name, s["name"], "match%=", s.get("match_percent"))
            for ib in s.get("instructions", []):
                ins = ib.get("instruction", {})
                dk = ib.get("diff_kind", "")
                addr = ins.get("address", "?")
                fmt = ins.get("formatted", "?")
                tag = f"[{dk[:12]}]" if dk else "             "
                print(f"  {tag:14} {addr!s:>5}  {fmt}")
            print()
