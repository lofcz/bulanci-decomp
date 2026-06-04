#!/usr/bin/env python3
import struct
import sys

path = sys.argv[1] if len(sys.argv) > 1 else r"orig/bulanci_insturmented.exe"
with open(path, "rb") as f:
    dos = f.read(64)
    e_lfanew = struct.unpack_from("<I", dos, 0x3C)[0]
    f.seek(e_lfanew + 6)
    num_sections = struct.unpack("<H", f.read(2))[0]
    f.seek(e_lfanew + 20)
    opt_size = struct.unpack("<H", f.read(2))[0]
    f.seek(e_lfanew + 24 + opt_size)
    sections = []
    for _ in range(num_sections):
        name = f.read(8)
        vsz, va, rawsz, rawptr = struct.unpack("<IIII", f.read(16))
        f.read(16)
        sections.append((va, rawsz, rawptr, name.rstrip(b"\x00").decode()))


def read_dword(rva: int) -> int:
    for va, rawsz, rawptr, _ in sections:
        if va <= rva < va + max(rawsz, 1):
            with open(path, "rb") as f:
                f.seek(rawptr + (rva - va))
                return struct.unpack("<I", f.read(4))[0]
    raise SystemExit(f"RVA 0x{rva:x} not in any section")


print("Sections:")
for va, rawsz, rawptr, name in sections:
    print(f"  {name}: VA=0x{va:08X} size=0x{rawsz:X} raw=0x{rawptr:X}")

base = 0x4B09C8
if not any(va <= base < va + max(rawsz, 1) for va, rawsz, _, _ in sections):
    print("BlitTable_DestKey RVA not mapped in this PE")
    sys.exit(0)

print("BlitTable_DestKey src=1 row (slots 8..15):")
for i in range(8, 16):
    ptr = read_dword(base + i * 4)
    print(f"  slot {i:2d} dst={i % 8} src={i // 8}: 0x{ptr:08X}")
print(f"slot 13 (expected 0x0043E740): 0x{read_dword(base + 13 * 4):08X}")
