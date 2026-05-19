"""Per-function byte-size & match status for the GZip/zlib functions."""
import sys
sys.path.insert(0, "scripts/internal")
from near_miss import coff_function_bytes
from pathlib import Path
import binascii

t = coff_function_bytes(Path("build/orig/bulanci/_Globals.obj"))
b = coff_function_bytes(Path("build/Src/bulanci/_Globals.obj"))

names = [
    "_Globals::FUN_0046ef60",
    "_Globals::FUN_004704c0",
    "_Globals::FUN_00434e30",
    "_Globals::FUN_00434ee0",
    "_Globals::FUN_0046ee10",
    "_Globals::FUN_0046f660",
    "_Globals::FUN_004702c0",
    "_Globals::FUN_0046ee60",
    "_Globals::FUN_0046f3e0",
    "_Globals::FUN_0046ef80",
]
for n in names:
    tb = t.get(n)
    bb = b.get(n)
    ts = len(tb) if tb is not None else None
    bs = len(bb) if bb is not None else None
    if tb is None or bb is None:
        status = "MISSING"
    elif tb == bb:
        status = "EXACT"
    elif len(tb) == len(bb):
        d = sum(1 for x, y in zip(tb, bb) if x != y)
        status = f"diff={d}"
    else:
        status = f"len-diff={len(tb) - len(bb):+d}"
    print(f"{n:30s}  target={ts!s:>5}  base={bs!s:>5}  {status}")
