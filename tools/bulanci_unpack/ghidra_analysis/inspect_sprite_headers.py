#!/usr/bin/env python3
"""Quick survey of BitmapSprite (ClassID 52) headers.

Dumps the first 44 bytes of every res_*_52_BitmapSprite.bin file as 11
little-endian u32s so we can look for patterns across resources.

Usage:
    python tools/bulanci_unpack/ghidra_analysis/inspect_sprite_headers.py \
        unpacked/overlay
"""
from __future__ import annotations

import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path


def main(out_dir: str) -> int:
    p = Path(out_dir)
    files = sorted(p.glob("res_*_52_BitmapSprite.bin"))
    if not files:
        print(f"No BitmapSprite files found in {p}")
        return 1

    print(f"{'id':<10} {'fileSize':>9} | {'totalSize':>9} f1 f2 f3 f4 f5 f6 f7 f8 f9 f10")
    print("-" * 110)

    sample_counts = Counter()
    width_counter = Counter()
    height_counter = Counter()
    frame_counter = Counter()
    field_index_to_values: dict[int, Counter[int]] = defaultdict(Counter)

    for fp in files:
        raw = fp.read_bytes()
        if len(raw) < 44:
            print(f"{fp.name}: TOO SHORT ({len(raw)} bytes)")
            continue
        f = struct.unpack_from("<11I", raw, 0)
        sample_counts["count"] += 1
        # Track per-field histograms.
        for i, v in enumerate(f):
            field_index_to_values[i][v] += 1
        # Pretty print: id from filename.
        rid = fp.stem.split("_")[1]
        print(
            f"{rid:<10} {len(raw):>9} | "
            f"{f[0]:>9} "
            f"{f[1]:>5} {f[2]:>5} {f[3]:>5} {f[4]:>5} {f[5]:>5} "
            f"{f[6]:>5} {f[7]:>5} {f[8]:>5} {f[9]:>5} {f[10]:>5}"
        )

    print()
    print("== Per-field unique-value summary (top 8 of each) ==")
    for i in sorted(field_index_to_values):
        ctr = field_index_to_values[i]
        top = ctr.most_common(8)
        unique = len(ctr)
        print(f"  field[{i:2d}] unique={unique:5d}  top={top}")

    print()
    print(f"Files inspected: {sample_counts['count']}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1] if len(sys.argv) > 1 else "unpacked/overlay"))
