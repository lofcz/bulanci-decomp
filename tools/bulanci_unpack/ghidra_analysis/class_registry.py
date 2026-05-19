"""Dump every CDS class registered by `HandleClassRegister` (0x0042e910).

`bulanci.exe`'s static initialiser block at `0x0047aff0..0x0047d800`
calls `HandleClassRegister(this, classId, parent_meta_ptr, factory)`
once per CDS class. Each call uses the canonical 4-instruction
prologue::

    PUSH imm32      ; factory function
    PUSH imm32      ; parent-class meta pointer-slot (e.g. 0x4b7bfc for &CDSObject)
    PUSH imm8|imm32 ; classId (PUSH imm8 takes 2 bytes; imm32 takes 5)
    MOV  ECX, imm32 ; class-meta this pointer (e.g. 0x4b7c60 for CDSStaticTexts)
    CALL FUN_0042e910

This script scans the `.text` section for every `CALL` site of
`FUN_0042e910` and reconstructs the (classId, factory, meta, parent)
tuple — that's enough to identify *every* deserialisable class in
the binary along with its factory function.

Output is sorted by classId. Useful for:

* Mapping resource-pack class IDs (2000-2078) to the C++ class that
  consumes their bytes.
* Auditing the small interface-tier IDs (3..94) that the engine
  uses internally for its type system.
* Following factories back to a vftable RTTI walk when you want a
  class's name (see `static_texts.md` for the recipe).

Run::

    python tools/bulanci_unpack/ghidra_analysis/class_registry.py
"""

from __future__ import annotations

import struct
from pathlib import Path

import pefile

EXE = Path(__file__).resolve().parents[3] / "orig" / "bulanci.exe"
TARGET = 0x0042E910  # HandleClassRegister


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

    # Find every CALL imm32 site whose target is HandleClassRegister.
    # x86 near-CALL is `E8 rel32` with rel32 = target - (call_site + 5).
    call_target_rel = TARGET - text_lo  # relative to .text base for easier scan
    found_sites: list[int] = []
    region = raw[text_lo - image_base : text_hi - image_base]
    for i in range(len(region) - 5):
        if region[i] != 0xE8:
            continue
        rel = struct.unpack_from("<i", region, i + 1)[0]
        target = (text_lo + i + 5 + rel) & 0xFFFFFFFF
        if target == TARGET:
            found_sites.append(text_lo + i)
    print(f"# Found {len(found_sites)} call sites to HandleClassRegister at 0x{TARGET:08x}")

    rows: list[tuple[int, int, int, int, int]] = []  # (call_site, factory, name_ptr, classId, meta)
    for site in found_sites:
        # Walk backwards looking for: PUSH imm32 (factory), PUSH imm32 (name),
        # PUSH imm8 or imm32 (classId), MOV ECX, imm32 (meta).
        # The CALL is at site; expect MOV ECX, imm32 right before it, then 3 PUSHes.
        off = site - image_base
        try:
            # MOV ECX, imm32 = B9 xx xx xx xx (5 bytes)
            mov_at = off - 5
            if raw[mov_at] != 0xB9:
                continue
            meta = struct.unpack_from("<I", raw, mov_at + 1)[0]

            # PUSH imm32 (classId): 68 xx xx xx xx (5 bytes) OR PUSH imm8: 6A xx (2 bytes)
            if raw[mov_at - 5] == 0x68:
                cid = struct.unpack_from("<I", raw, mov_at - 4)[0]
                p3_start = mov_at - 5
            elif raw[mov_at - 2] == 0x6A:
                cid = raw[mov_at - 1]
                p3_start = mov_at - 2
            else:
                continue

            # PUSH imm32 (name_ptr): 68 xx xx xx xx (5 bytes)
            p2_at = p3_start - 5
            if raw[p2_at] != 0x68:
                continue
            name_ptr = struct.unpack_from("<I", raw, p2_at + 1)[0]

            # PUSH imm32 (factory): 68 xx xx xx xx (5 bytes)
            p1_at = p2_at - 5
            if raw[p1_at] != 0x68:
                continue
            factory = struct.unpack_from("<I", raw, p1_at + 1)[0]

            rows.append((site, factory, name_ptr, cid, meta))
        except Exception:
            continue

    # Print sorted by classId.
    print()
    print(f"{'call_site':>10}  {'meta':>10}  {'factory':>10}  {'name_ptr':>10}  {'classId':>10}")
    rows.sort(key=lambda r: r[3])
    for site, factory, name_ptr, cid, meta in rows:
        print(
            f"  0x{site:08x}  0x{meta:08x}  0x{factory:08x}  0x{name_ptr:08x}"
            f"  {cid:10d} (0x{cid:x})"
        )

    print()
    print(f"# Unique classIds: {sorted(set(r[3] for r in rows))}")
    print(f"# Total registrations parsed: {len(rows)} / {len(found_sites)}")


if __name__ == "__main__":
    main()
