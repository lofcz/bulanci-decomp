"""Extract the static CDSStaticTexts string pool from `bulanci.exe`.

The shipped Czech build of `bulanci.exe` has exactly one populated
`CDSStaticTexts` instance: a compile-time singleton at .data
`0x004afbb4` whose data slot points at a `wchar_t*[127]` table at
`0x004af9b8`.

Layout of the singleton (16 bytes; matches `CDSStaticTexts::CDSStaticTexts`
at `0x0042eeb0` and the constructor's `[EAX]=0x00486cc4`,
`[EAX+4]=0x00486cb0` writes):

    +0x00  void*  primary_vftable      = 0x00486cc4
    +0x04  void*  ids_texts_vftable    = 0x00486cb0   (IDSTexts subobject)
    +0x08  wchar_t** pool               = 0x004af9b8
    +0x0c  int32  count                 = 127 (0x7f)

The pool's entries are UTF-16LE C-strings scattered across `.rdata`.
Many of them are empty / scratch slots (a single nul wchar followed
by an unused gap) -- the engine only needs 127 fixed-position slots
to be addressable, not to be all filled.

Run::

    python tools/bulanci_unpack/ghidra_analysis/static_texts.py

It writes `static_texts.json` next to this script with::

    [
      {"index": 0, "ptr": "0x0047f668", "value": ""},
      ...
      {"index": 50, "ptr": "0x004862d4", "value": "Typ hry:"},
      ...
    ]
"""

from __future__ import annotations

import json
import struct
from pathlib import Path

import pefile

EXE = Path(__file__).resolve().parents[3] / "orig" / "bulanci.exe"
OUT = Path(__file__).resolve().parent / "static_texts.json"

SINGLETON_VA = 0x004AFBB4
POOL_VA = 0x004AF9B8
POOL_COUNT = 127

PRIMARY_VFTABLE = 0x00486CC4
IDS_TEXTS_VFTABLE = 0x00486CB0


def main() -> None:
    pe = pefile.PE(str(EXE), fast_load=True)
    image_base = pe.OPTIONAL_HEADER.ImageBase
    raw = pe.get_memory_mapped_image()

    def rd_u32(va: int) -> int:
        return struct.unpack_from("<I", raw, va - image_base)[0]

    def rd_utf16z(va: int, max_chars: int = 512) -> str:
        off = va - image_base
        chars: list[str] = []
        for i in range(max_chars):
            cu = struct.unpack_from("<H", raw, off + i * 2)[0]
            if cu == 0:
                break
            chars.append(chr(cu))
        return "".join(chars)

    # Sanity: confirm the singleton has the layout we expect.
    primary = rd_u32(SINGLETON_VA + 0)
    secondary = rd_u32(SINGLETON_VA + 4)
    pool_ptr = rd_u32(SINGLETON_VA + 8)
    count = rd_u32(SINGLETON_VA + 12)
    assert primary == PRIMARY_VFTABLE, f"primary vftable 0x{primary:08x} != expected"
    assert secondary == IDS_TEXTS_VFTABLE, f"IDSTexts vftable 0x{secondary:08x} != expected"
    assert pool_ptr == POOL_VA, f"pool ptr 0x{pool_ptr:08x} != expected"
    assert count == POOL_COUNT, f"count {count} != {POOL_COUNT}"

    entries: list[dict] = []
    for i in range(POOL_COUNT):
        slot_va = POOL_VA + i * 4
        ptr = rd_u32(slot_va)
        if ptr == 0:
            entries.append({"index": i, "ptr": "0x00000000", "value": None})
            continue
        try:
            value = rd_utf16z(ptr)
        except Exception as exc:  # pragma: no cover - defensive
            value = f"<unreadable: {exc}>"
        entries.append({"index": i, "ptr": f"0x{ptr:08x}", "value": value})

    OUT.write_text(json.dumps(entries, ensure_ascii=False, indent=2), encoding="utf-8")
    non_empty = sum(1 for e in entries if e["value"])
    print(f"Wrote {OUT.name}: {non_empty}/{POOL_COUNT} entries non-empty")

    def fmt(e: dict) -> str:
        v = e["value"]
        if not v:
            return repr(v)
        return v.encode("ascii", errors="backslashreplace").decode("ascii")

    for e in entries[:8]:
        print(f"  [{e['index']:3d}] {e['ptr']}  {fmt(e)}")
    print(f"  ... [50] {entries[50]['ptr']}  {fmt(entries[50])}")
    print(f"  ... [59] {entries[59]['ptr']}  {fmt(entries[59])}")
    print(f"  ... [{POOL_COUNT - 1}] {entries[-1]['ptr']}  {fmt(entries[-1])}")


if __name__ == "__main__":
    main()
