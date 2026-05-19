#!/usr/bin/env python3
"""Master-pack regression test for the CDSFlxFile pixel decoders.

The actual implementations now live in :mod:`bulanci_unpack` (functions
``_flxrle_decode``, ``_flxdelta_apply``, ``_flxpalette_apply``) — this
script is a thin sanity check that re-runs them across every
BitmapSprite (ClassID 52) entry in ``unpacked/overlay/_manifest.json``
and confirms:

  * every keyframe RLE chunk (inner opcode 0x00) decodes to exactly
    ``bitmapWidth * bitmapHeight = fields[4] * fields[5]`` bytes;
  * every delta chunk (inner opcode 0x04) consumes its body cleanly;
  * every palette chunk (inner opcode 0x09) writes 1..256 BGR triplets
    without overrunning the 256-entry palette.

A clean run prints "OK: 130/130 sprites" and exits 0. Any failure
includes the sprite id, frame index, and per-opcode error.

Run from the repo root after running ``bulanci_unpack.py``:

    python tools/bulanci_unpack/ghidra_analysis/proto_decoder.py
"""
from __future__ import annotations

import json
import sys
from pathlib import Path

# Re-use the production decoders so this file can't drift from the
# real unpacker.
sys.path.insert(0, str(Path(__file__).parent.parent))
from bulanci_unpack import (  # noqa: E402
    _flxrle_decode,
    _flxdelta_apply,
    _flxpalette_apply,
    _walk_bitmap_sprite_frames,
)

OUT_DIR = Path("unpacked/overlay")


def main() -> int:
    mf = json.loads((OUT_DIR / "_manifest.json").read_text(encoding="utf-8"))
    sprites = [r for r in mf["resources"] if r.get("classId") == 52]
    if not sprites:
        print("no ClassID 52 sprites in manifest; did you run bulanci_unpack.py?")
        return 1

    fail = 0
    rle_chunks = delta_chunks = palette_chunks = 0
    for r in sprites:
        s = r["sprite"]
        raw = (OUT_DIR / r["rawFile"]).read_bytes()
        # The bitmap dimensions empirically use the field at +0x14 as
        # *bitmap height* and the field at +0x10 as *bitmap width*. See
        # `sprite_container.md` for the orientation verification.
        bw, bh = s["height"], s["frameCount"]  # NB: legacy field names
        frames, walk_err = _walk_bitmap_sprite_frames(raw)
        if walk_err:
            print(f"  sprite {r['id']}: frame walk: {walk_err}")
            fail += 1
            continue
        pixels = bytearray(bw * bh)
        palette = bytearray([0xFF] * 1024)
        for fr_idx, fr in enumerate(frames):
            for ic in fr["innerChunks"]:
                op = ic["opcode"]
                body = raw[ic["offset"] + 5 : ic["offset"] + ic["size"]]
                try:
                    if op == 0x00:
                        rle_chunks += 1
                        out = _flxrle_decode(body)
                        if len(out) != bw * bh:
                            raise ValueError(
                                f"rle output {len(out)} != bw*bh {bw*bh}"
                            )
                        pixels[:] = out
                    elif op == 0x04:
                        delta_chunks += 1
                        _flxdelta_apply(body, pixels)
                    elif op == 0x09:
                        palette_chunks += 1
                        _flxpalette_apply(body, palette)
                    elif op == 0x0E:
                        # Alt buffer is the same shape; redirect to a scratch
                        # bytearray so the main bitmap stays intact.
                        alt = bytearray(bw * bh)
                        out = _flxrle_decode(body)
                        if len(out) != bw * bh:
                            raise ValueError(
                                f"alt rle output {len(out)} != bw*bh {bw*bh}"
                            )
                        alt[:] = out
                    elif op == 0x0F:
                        _flxdelta_apply(body, bytearray(bw * bh))
                    # Metadata opcodes (0x08, 0x0a, 0x0b, 0x0c, 0x0d) need no check
                except Exception as exc:
                    print(
                        f"  sprite {r['id']} frame {fr_idx} inner op 0x{op:02x}: {exc}"
                    )
                    fail += 1
                    break

    if fail == 0:
        print(
            f"OK: {len(sprites)}/{len(sprites)} sprites  "
            f"({rle_chunks} rle, {delta_chunks} delta, {palette_chunks} palette chunks)"
        )
        return 0
    print(f"FAILURES: {fail}")
    return 1


if __name__ == "__main__":
    sys.exit(main())
