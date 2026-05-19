import json, sys, subprocess

UNIT = "bulanci/CDSJpegImage"
TARGETS = [
    "CDSJpegImage::FUN_00431510",
    "CDSJpegImage::FUN_0045f1f0",
    "CDSJpegImage::FUN_0045f230",
    "CDSJpegImage::FUN_0045f1b0",
    "CDSJpegImage::FUN_0045f660",
    "CDSJpegImage::FUN_0045f370",
    "CDSJpegImage::FUN_0045ed80",
    "CDSJpegImage::FUN_0045efe0",
    "CDSJpegImage::FUN_0045f6e0",
    "CDSJpegImage::FUN_00432090",
    "_Globals::FUN_0045f7e0",
]

# Single call to get the unit-level diff with all per-symbol match%.
result = subprocess.run(
    [r"tools/objdiff-cli.exe", "diff", "--project", ".",
     "--unit", UNIT, "--format", "json", "--output", "-"],
    capture_output=True, text=False, check=True,
)
left = json.loads(result.stdout).get("left", {})
per_symbol = {s["name"]: s for s in left.get("symbols", [])}

# Same for _Globals (the jdiv_round_up entry lives there).
result2 = subprocess.run(
    [r"tools/objdiff-cli.exe", "diff", "--project", ".",
     "--unit", "bulanci/_Globals", "--format", "json", "--output", "-"],
    capture_output=True, text=False, check=True,
)
per_symbol_g = {s["name"]: s for s in json.loads(result2.stdout).get("left", {}).get("symbols", [])}

print(f"  {'name':45s} {'size':>5s}  match%")
print(f"  {'-'*45} {'-'*5}  ------")
for name in TARGETS:
    sym = per_symbol.get(name) or per_symbol_g.get(name)
    if not sym:
        print(f"  {name:45s}    -    not found")
        continue
    mp = sym.get("match_percent")
    size = sym.get("size", "?")
    mp_str = f"{mp:.1f}%" if mp is not None else "  -  "
    print(f"  {name:45s} {size:>5}  {mp_str}")

# Section-level summary
sec = next((s for s in left.get("sections", []) if s["name"] == ".text"), {})
print()
print(f"  CDSJpegImage .text section: {sec.get('match_percent', 0):.2f}%  ({sec.get('size')} B)")
