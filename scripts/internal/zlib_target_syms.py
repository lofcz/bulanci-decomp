"""Show all symbols matching a pattern in the target .obj."""
import sys
sys.path.insert(0, "scripts/internal")
from near_miss import coff_function_bytes
from pathlib import Path

t = coff_function_bytes(Path("build/orig/bulanci/_Globals.obj"))

print(f"Total functions in target: {len(t)}")
print("Sample first 20:")
for k in sorted(t.keys())[:20]:
    print(f"  {k}")
print("...")
print("Anything containing 'inflate' or 'deflate' or 'zlib' or '46e' or '46f' or '470':")
for k in sorted(t.keys()):
    s = k.lower()
    if any(x in s for x in ("inflate", "deflate", "zlib", "46ee", "46ef", "46f3", "46f6", "4702", "4704", "434e")):
        print(f"  {k}  len={len(t[k])}")
