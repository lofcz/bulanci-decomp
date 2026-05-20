#!/usr/bin/env python3
"""Print the full 44-byte header of a single BitmapSprite resource."""
import struct
import sys
from pathlib import Path


def main(path: str) -> int:
    raw = Path(path).read_bytes()
    head = raw[:44]
    hex_str = " ".join(f"{b:02X}" for b in head)
    fields = struct.unpack_from("<11I", raw, 0)
    print(f"{path}  ({len(raw)} bytes)")
    print(f"  hex   : {hex_str}")
    for i, v in enumerate(fields):
        print(f"  f[{i:2d}] : {v} (0x{v:08X})")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1]))
