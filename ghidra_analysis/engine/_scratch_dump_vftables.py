"""Dump candidate vftables found around RTTI for CDSStaticTexts / CStaticText / IDSTexts.

Walks each .rdata pointer slot of a candidate vftable until it stops looking
like a code pointer (i.e. address outside .text section or a non-zero
non-text value that is in fact .rdata data, which signals the next COL ptr).
"""

from __future__ import annotations

import struct
from pathlib import Path

import pefile

EXE = Path(r"C:\Users\mstagl-dev\Documents\GitHub\bulanci\orig\bulanci.exe")


def main() -> None:
    pe = pefile.PE(str(EXE), fast_load=True)
    image_base = pe.OPTIONAL_HEADER.ImageBase
    raw = pe.get_memory_mapped_image()

    def sec(name: bytes) -> tuple[int, int]:
        for s in pe.sections:
            if s.Name.rstrip(b"\x00") == name:
                return image_base + s.VirtualAddress, image_base + s.VirtualAddress + s.Misc_VirtualSize
        raise KeyError(name)

    text_lo, text_hi = sec(b".text")
    rdata_lo, rdata_hi = sec(b".rdata")

    def in_text(p: int) -> bool:
        return text_lo <= p < text_hi

    def in_rdata(p: int) -> bool:
        return rdata_lo <= p < rdata_hi

    def rd_u32(va: int) -> int:
        return struct.unpack_from("<I", raw, va - image_base)[0]

    def walk_vftable(vt: int, max_len: int = 64) -> list[int]:
        out: list[int] = []
        for i in range(max_len):
            p = rd_u32(vt + i * 4)
            if not in_text(p):
                break
            out.append(p)
        return out

    vftables = {
        # CDSStaticTexts: primary then IDSTexts secondary
        "CDSStaticTexts (primary, off=0)": 0x00486CC4,
        "CDSStaticTexts (IDSTexts sub, off=4)": 0x00486CB0,
        # CStaticText: 4 vftables found
        "CStaticText (primary, off=0)": 0x0047FC9C,
        "CStaticText (off=4)": 0x0047FC7C,
        "CStaticText (off=16)": 0x0047FC64,
        "CStaticText (off=24)": 0x0047FC50,
    }

    for name, vt in vftables.items():
        slots = walk_vftable(vt)
        print(f"--- {name} @ 0x{vt:08x} ({len(slots)} slots) ---")
        for i, fn in enumerate(slots):
            print(f"  [{i:2d}] 0x{fn:08x}")
        print()


if __name__ == "__main__":
    main()
