#!/usr/bin/env python3
"""Pretty-print the objdiff for a single function symbol.

Usage:
    python scripts/_tmp_inspect_func_diff.py <unit> <symbol>

Example:
    python scripts/_tmp_inspect_func_diff.py bulanci/CDSMpx CDSMpx::mad_bit_nextbyte
"""
import json
import subprocess
import sys

if len(sys.argv) < 3:
    print(__doc__)
    sys.exit(1)

unit, symbol = sys.argv[1], sys.argv[2]
out = subprocess.check_output(
    ["tools/objdiff-cli.exe", "diff",
     "-p", ".", "-u", unit, "--format", "json", "-o", "-", symbol],
    text=True, encoding="utf-8")
data = json.loads(out)


def find_symbol(side: dict, name: str):
    for s in side["symbols"]:
        if s.get("name") == name:
            return s
    return None


def fmt(instr_block):
    insn = instr_block.get("instruction") or {}
    addr = insn.get("address", "0")
    sz = insn.get("size", "?")
    text = insn.get("formatted", "?")
    return f"  {int(addr) if isinstance(addr, str) else addr:>4}  {text} ({sz}B)"


left = find_symbol(data["left"], symbol)
right = find_symbol(data["right"], symbol)

print(f"=== {symbol} ===")
if left is None and right is None:
    print(f"  not found in either side")
    sys.exit(2)

print(f"  match_percent (left/built):    {left and left.get('match_percent', 0):.2f}%")
print(f"  match_percent (right/target):  {right and right.get('match_percent', 0):.2f}%")
print()

print(f"--- BUILT (our output) ---")
for ib in (left.get("instructions") or []) if left else []:
    dk = ib.get("diff_kind", "")
    marker = {"DIFF_REPLACE": "~", "DIFF_INSERT": "+", "DIFF_DELETE": "-"}.get(dk, " ")
    print(f"  {marker}{fmt(ib)[1:]}{'  <-- ' + dk if dk else ''}")

print()
print(f"--- TARGET (original bulanci.exe) ---")
for ib in (right.get("instructions") or []) if right else []:
    dk = ib.get("diff_kind", "")
    marker = {"DIFF_REPLACE": "~", "DIFF_INSERT": "+", "DIFF_DELETE": "-"}.get(dk, " ")
    print(f"  {marker}{fmt(ib)[1:]}{'  <-- ' + dk if dk else ''}")
