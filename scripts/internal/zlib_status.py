"""Per-function byte-size & match status for the GZip/zlib functions.

After the zlib helpers were renamed under the `zlib::` namespace (Ghidra
+ mapping.csv) and migrated to `build/orig/bulanci/zlib.obj`, this
script compares both the engine-level CDSGZipStream methods that stayed
in `_Globals.obj` and the zlib API helpers that now live in `zlib.obj`.

Trailing underscores in zlib's API names (`inflateInit_`, `deflateInit2_`,
...) are stripped on both sides by `_sanitize_symbol_name()` /
`sanitizeSymbolNames()`, so we look them up under their stripped form.
"""
import sys
sys.path.insert(0, "scripts/internal")
from near_miss import coff_function_bytes
from pathlib import Path

# (orig_obj, src_obj, [list of mangled names])
TARGETS = [
    ("build/orig/bulanci/_Globals.obj", "build/Src/bulanci/_Globals.obj", [
        # CDSGZipStream::Decompress / Compress engine methods.
        "_Globals::FUN_00434e30",
        "_Globals::FUN_00434ee0",
    ]),
    ("build/orig/bulanci/zlib.obj", "build/Src/bulanci/zlib.obj", [
        # zlib API helpers (trailing _ stripped by sanitiser).
        "zlib::inflateInit",      # 0x46EF60 (inflateInit_)  EXACT 26
        "zlib::deflateInit",      # 0x4704C0 (deflateInit_)  EXACT 37
        "zlib::zcalloc",          # 0x471260                  EXACT 19
        "zlib::inflate_codes_free",  # 0x4733D0               EXACT 22
        # Opacity-barrier stubs (sized to original; bodies not real).
        "zlib::inflateEnd",       # 0x46EE10
        "zlib::deflateEnd",       # 0x46F660
        "zlib::inflateInit2",     # 0x46EE60 (inflateInit2_)
        "zlib::deflateInit2",     # 0x4702C0 (deflateInit2_)
        "zlib::inflate",          # 0x46EF80
        "zlib::deflate",          # 0x46F3E0
        # Real bodies still TODO.
        "zlib::inflateReset",        # 0x46EDC0
        "zlib::inflate_blocks_reset",  # 0x4704F0
        "zlib::inflate_blocks_new",    # 0x470570
        "zlib::inflate_blocks",        # 0x470620
        "zlib::inflate_blocks_free",   # 0x4710E0
        "zlib::init_block",            # 0x471290
    ]),
]

def status(target_bytes, base_bytes):
    if target_bytes is None or base_bytes is None:
        return "MISSING", None
    if target_bytes == base_bytes:
        return "EXACT", 0
    if len(target_bytes) == len(base_bytes):
        d = sum(1 for x, y in zip(target_bytes, base_bytes) if x != y)
        return f"diff={d}", d
    return f"len-diff={len(target_bytes) - len(base_bytes):+d}", None


for orig_obj, src_obj, names in TARGETS:
    if not Path(orig_obj).exists() or not Path(src_obj).exists():
        print(f"# Skipping {orig_obj} / {src_obj} (not built yet)")
        continue
    t = coff_function_bytes(Path(orig_obj))
    b = coff_function_bytes(Path(src_obj))
    print(f"# {orig_obj}  vs  {src_obj}")
    for n in names:
        tb = t.get(n)
        bb = b.get(n)
        ts = len(tb) if tb is not None else None
        bs = len(bb) if bb is not None else None
        st, _ = status(tb, bb)
        print(f"{n:32s}  target={ts!s:>5}  base={bs!s:>5}  {st}")
    print()
