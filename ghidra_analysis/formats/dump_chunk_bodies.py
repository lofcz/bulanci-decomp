#!/usr/bin/env python3
"""Dump per-tag chunk bodies from a single BitmapSprite for codec study.

Reads `unpacked/overlay/_manifest.json`, finds the requested sprite ID,
re-reads its `res_*_52_BitmapSprite.bin` payload, and prints:

* the 0x2c-byte file header decoded into named fields,
* per-chunk: index, offset, chunkSize, tag, body-as-hex.

Useful for staring at tag-1/tag-2/tag-3 bodies side-by-side. Run e.g.

    python tools/bulanci_unpack/ghidra_analysis/dump_chunk_bodies.py 65716
"""
from __future__ import annotations

import json
import struct
import sys
from pathlib import Path

OUT_DIR = Path("unpacked/overlay")


def _hex(b: bytes, group: int = 16, limit: int = 256) -> str:
    if len(b) > limit:
        body = b[:limit]
        tail = f" ... (+{len(b) - limit} bytes truncated)"
    else:
        body = b
        tail = ""
    lines = []
    for i in range(0, len(body), group):
        chunk = body[i : i + group]
        hexpart = " ".join(f"{x:02x}" for x in chunk)
        ascpart = "".join(chr(x) if 32 <= x < 127 else "." for x in chunk)
        lines.append(f"      +{i:04x}  {hexpart:<{group*3-1}}  {ascpart}")
    if tail:
        lines.append(f"      {tail}")
    return "\n".join(lines)


def main(sprite_id: int) -> int:
    mf = json.loads((OUT_DIR / "_manifest.json").read_text(encoding="utf-8"))
    rec = next(
        (r for r in mf["resources"] if r["classId"] == 52 and r["id"] == sprite_id),
        None,
    )
    if rec is None:
        print(f"no BitmapSprite with id={sprite_id}")
        return 1

    raw = (OUT_DIR / rec["rawFile"]).read_bytes()
    fields = struct.unpack_from("<11I", raw, 0)
    s = rec["sprite"]
    print(f"sprite {sprite_id}: {rec['rawFile']}")
    print(
        f"  totalSize={fields[0]} encA={fields[1]} encB={fields[2]} "
        f"w={fields[3]} h={fields[4]} frames={fields[5]} ch={fields[6]} "
        f"inMem={fields[7]} flags={fields[8]} encC={fields[9]} "
        f"packed=0x{fields[10]:08x}"
    )
    print(f"  chunkCount={s['chunkCount']} chunkTags={s['chunkTags']}")
    print()
    for idx, ch in enumerate(s["chunks"]):
        off, cs, tag, ds = ch["offset"], ch["chunkSize"], ch["tag"], ch["dataSize"]
        body = raw[off + 5 : off + cs]
        print(
            f"chunk #{idx:2d}  offset=0x{off:04x}  chunkSize={cs:>6}  "
            f"tag={tag}  dataSize={ds}"
        )
        if ds:
            print(_hex(body))
        else:
            print("      (empty)")
        print()
    return 0


if __name__ == "__main__":
    sid = int(sys.argv[1]) if len(sys.argv) > 1 else 65716
    sys.exit(main(sid))
