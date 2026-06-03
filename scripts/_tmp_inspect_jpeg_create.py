"""Objdiff + COFF byte check for jpeg_CreateDecompress @ 0x0045e6a0."""
import json
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, "scripts/internal")
from near_miss import coff_function_bytes

UNIT = "bulanci/_Globals"
SYMS = (
    "_Globals::jpeg_CreateDecompress",
    "_Globals::FUN_0045e6a0",
)


def main() -> None:
    out = Path("build/__jpeg_create_diff.json")
    subprocess.run(
        [
            r"tools/objdiff-cli.exe",
            "diff",
            "--project",
            ".",
            "--unit",
            UNIT,
            "--format",
            "json",
            "--output",
            str(out),
        ],
        check=True,
    )
    doc = json.loads(out.read_text(encoding="utf-8"))
    left = {s["name"]: s for s in doc.get("left", {}).get("symbols", [])}
    right = {s["name"]: s for s in doc.get("right", {}).get("symbols", [])}

    print(f"  {'symbol':40s} {'size':>5s}  match%")
    print(f"  {'-' * 40} {'-' * 5}  ------")
    for name in SYMS:
        o = left.get(name)
        m = right.get(name)
        size = (o or m or {}).get("size", "?")
        mp = (o or {}).get("match_percent")
        mp_str = f"{mp:.1f}%" if mp is not None else "  -  "
        print(f"  {name:40s} {size:>5}  {mp_str}")

    orig = Path("build/orig/bulanci/_Globals.obj")
    src = Path("build/Src/bulanci/_Globals.obj")
    ofns = coff_function_bytes(orig)
    bfns = coff_function_bytes(src)
    for name in SYMS:
        if name not in ofns and name == "_Globals::jpeg_CreateDecompress":
            name = "_Globals::FUN_0045e6a0"
        tb = ofns.get(name)
        bb = bfns.get(name)
        if not tb:
            print(f"\nCOFF: missing target {name}")
            continue
        print(f"\nCOFF {name}: target={len(tb)} base={len(bb) if bb else 0}")
        if bb:
            if tb == bb:
                print("  -> EXACT byte match")
            else:
                diffs = sum(1 for x, y in zip(tb, bb) if x != y)
                print(f"  -> {diffs} differing bytes (same len={len(tb)==len(bb)})")


if __name__ == "__main__":
    main()
