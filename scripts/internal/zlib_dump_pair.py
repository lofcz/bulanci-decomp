"""Side-by-side hex dump of two COFF function bodies, highlighting diffs.

Usage: python scripts/internal/zlib_dump_pair.py <symbol> [src.obj] [tgt.obj]
"""
import sys
sys.path.insert(0, "scripts/internal")
from near_miss import coff_function_bytes
from pathlib import Path

if len(sys.argv) < 2:
    print(__doc__)
    sys.exit(1)

symbol = sys.argv[1]
src_obj = Path(sys.argv[2]) if len(sys.argv) >= 3 else Path("build/Src/bulanci/_Globals.obj")
tgt_obj = Path(sys.argv[3]) if len(sys.argv) >= 4 else Path("build/orig/bulanci/_Globals.obj")

tb = coff_function_bytes(tgt_obj).get(symbol)
bb = coff_function_bytes(src_obj).get(symbol)
if tb is None: print(f"missing in {tgt_obj}: {symbol}"); sys.exit(1)
if bb is None: print(f"missing in {src_obj}: {symbol}"); sys.exit(1)

print(f"# {symbol}: target={len(tb)}  base={len(bb)}")
print(f"#   {tgt_obj}  (target)")
print(f"#   {src_obj}  (base)")
print()

for i in range(0, max(len(tb), len(bb)), 16):
    tg = tb[i:i+16]
    bs = bb[i:i+16]
    th = " ".join(f"{b:02x}" for b in tg).ljust(48)
    bh = " ".join(f"{b:02x}" for b in bs).ljust(48)
    if tg == bs:
        marker = "  "
    else:
        marker = "!!"
    print(f"{i:04x}  {marker}  {th}  |  {bh}")
