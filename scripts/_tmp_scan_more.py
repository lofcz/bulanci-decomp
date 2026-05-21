#!/usr/bin/env python3
"""Find more simple 2-3 insn patterns."""
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

func_sizes: dict[str, int] = {}
for unit in report["units"]:
    for f in unit.get("functions", []):
        func_sizes[f["name"]] = int(f.get("size", "0"))

PATTERNS = {
    "mov_ecx_member_jmp": re.compile(
        r"mov\s+ecx,\s*(?:dword ptr )?\[ecx\+([0-9A-Fa-fh]+)\]\s*\|\|\s*jmp\s+(\S+)"),
}

_INSN_RE = re.compile(r"\b(mov|jmp|call|ret|push|pop|xor|sub|add|lea|test|cmp|and|or|shl|shr|sar|neg|inc|dec|nop|fld|fmul|fadd|fsub|fdiv|fstp)\b\s*(.*)$")
def insns(line: str) -> str | None:
    m = _INSN_RE.search(line)
    if m:
        op = m.group(1)
        args = m.group(2).strip()
        return f"{op} {args}".strip()
    return None

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
                    inst = insns(lines[j])
                    if inst:
                        body.append(inst)
                j += 1
            if name in stub_funcs.get(unit_name, set()) and 2 <= len(body) <= 4:
                results.append((name, body))
            i = max(j, i + 1)
        else:
            i += 1
    return results

# Group by pattern
patterns = {}
total_scanned = 0
total_hits = 0
for unit_name in sorted(stub_funcs):
    obj_path = ORIG / (unit_name.split("/")[-1] + ".obj")
    res = scan(obj_path, unit_name)
    total_scanned += 1
    total_hits += len(res)
    for name, body in res:
        # categorize by mnemonics
        key = "|".join(b.split()[0] for b in body)
        patterns.setdefault(key, []).append((unit_name, name, body, func_sizes.get(name, 0)))

import sys
print(f"DEBUG: scanned {total_scanned} units, {total_hits} hits", file=sys.stderr)
print(f"DEBUG: stub_funcs size = {sum(len(v) for v in stub_funcs.values())}", file=sys.stderr)

for key, items in sorted(patterns.items(), key=lambda kv: -len(kv[1])):
    print(f"=== Pattern: {key} ({len(items)} hits, total {sum(s[3] for s in items)} bytes) ===")
    for unit_name, name, body, sz in items:
        print(f"  {unit_name}\t{name}\t({sz}B)")
        for b in body:
            print(f"    {b}")
