"""Find where zlib helpers ended up in the target tree."""
import sys
sys.path.insert(0, "scripts/internal")
from near_miss import coff_function_bytes
from pathlib import Path

addrs = ["434e30", "434ee0", "435050", "435140", "435670",
         "4354a0", "4356e0", "46ef60", "46ee60", "46ef80",
         "46ee10", "46f3e0", "46f660", "4702c0", "4704c0"]

import sys
print(f"Scanning {len(list(Path('build/orig/bulanci').glob('*.obj')))} .obj files", file=sys.stderr)
for objp in sorted(Path("build/orig/bulanci").glob("*.obj")):
    try:
        fns = coff_function_bytes(objp)
    except Exception as e:
        print(f"  ERR {objp.name}: {e}", file=sys.stderr)
        continue
    for fnname in fns.keys():
        s = fnname.lower()
        if any(a in s for a in addrs) or "inflate" in s or "deflate" in s or "zlib" in s:
            print(f"  {objp.name:35s} {fnname}  len={len(fns[fnname])}")
