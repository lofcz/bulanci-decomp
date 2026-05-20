"""Print per-function match% for the CDSJpegImage + _Globals targets we care about."""
import json
import subprocess

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


def load_unit(unit):
    res = subprocess.run(
        [r"tools/objdiff-cli.exe", "diff", "--project", ".",
         "--unit", unit, "--format", "json", "--output", "build/__diff.json"],
        capture_output=True, check=True,
    )
    return json.load(open("build/__diff.json", encoding="utf-8"))


cds = load_unit(UNIT)
glb = load_unit("bulanci/_Globals")

per = {s["name"]: s for s in cds.get("left", {}).get("symbols", [])}
per.update({s["name"]: s for s in glb.get("left", {}).get("symbols", [])})

print(f"  {'name':45s} {'size':>5s}  match%")
print(f"  {'-'*45} {'-'*5}  ------")
for name in TARGETS:
    sym = per.get(name)
    if not sym:
        print(f"  {name:45s}    -    not found")
        continue
    mp = sym.get("match_percent")
    size = sym.get("size", "?")
    mp_str = f"{mp:.1f}%" if mp is not None else "  -  "
    print(f"  {name:45s} {size:>5}  {mp_str}")

sec = next((s for s in cds.get("left", {}).get("sections", []) if s["name"] == ".text"), {})
print()
print(f"  CDSJpegImage .text section: {sec.get('match_percent', 0):.2f}%  ({sec.get('size')} B)")
