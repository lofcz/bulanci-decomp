#!/usr/bin/env python3
"""Quick post-extract sanity check for BitmapSprite (ClassID 52) entries.

Reads `_manifest.json` and prints:

* total count of BitmapSprite entries,
* any entry whose `sprite.sanityWarnings` list is non-empty,
* a histogram of the rare "encodedSize != encodedSize2" multi-stream
  variant (informational only).

Run after `bulanci_unpack.py overlay ... -o <dir>` to make sure the new
header parser stays clean across the master pack.
"""
from __future__ import annotations

import json
import sys
from collections import Counter
from pathlib import Path


def main(out_dir: str) -> int:
    p = Path(out_dir) / "_manifest.json"
    if not p.exists():
        print(f"No manifest at {p}")
        return 1

    mf = json.loads(p.read_text(encoding="utf-8"))
    sprites = [r for r in mf["resources"] if r.get("className") == "BitmapSprite"]
    print(f"BitmapSprite entries : {len(sprites)}")

    warnings = [r for r in sprites if r.get("sprite", {}).get("sanityWarnings")]
    print(f"  with sanityWarnings: {len(warnings)}")
    for r in warnings:
        print(f"    id={r['id']}  warns={r['sprite']['sanityWarnings']}")

    multi_stream = [
        r for r in sprites
        if r.get("sprite", {}).get("encodedSize") != r.get("sprite", {}).get("encodedSize2")
    ]
    print(f"  multi-stream (encA != encB): {len(multi_stream)}")
    for r in multi_stream:
        s = r["sprite"]
        print(
            f"    id={r['id']}  encA={s['encodedSize']} encB={s['encodedSize2']} "
            f"width={s['width']} height={s['height']} frames={s['frameCount']}"
        )

    # Verify the "marker" invariants the parser relies on hold across all
    # samples. (Should be empty lists — these would trip new warnings if
    # ever violated.)
    bad_channels = [r for r in sprites if r["sprite"]["channels"] != 3]
    bad_total = [
        r for r in sprites
        if r["sprite"]["totalSize"] != r["size"]
    ]
    print(f"  channels != 3        : {len(bad_channels)}")
    print(f"  totalSize != size    : {len(bad_total)}")

    # Histograms for quick eyeballing.
    flag_hist = Counter(r["sprite"]["flags"] for r in sprites)
    width_hist = Counter(r["sprite"]["width"] for r in sprites)
    print("  flags top-8          :", flag_hist.most_common(8))
    print("  width top-8          :", width_hist.most_common(8))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1] if len(sys.argv) > 1 else "unpacked/overlay"))
