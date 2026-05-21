#!/usr/bin/env python3
"""Extract a contiguous slice of Draw events where the mouse is moving
so we can plot exactly how `target_*` follows the live mouse.

Usage:
    python tools/frida/extract_motion_segment.py trace.jsonl [N]

Prints up to N (default 200) consecutive Draws starting at the first
one whose mouse position differs from its predecessor's, as a tidy
table: `t  mouse(x,y)  target(x,y)  dx_dot  dy_dot  Δ(mouse-target)`.
"""

from __future__ import annotations

import json
import sys
from pathlib import Path


def main() -> int:
    path = Path(sys.argv[1] if len(sys.argv) > 1 else "trace.jsonl")
    want = int(sys.argv[2]) if len(sys.argv) > 2 else 200

    rows = []
    prev_mouse = None
    found_motion = False
    with path.open("r", encoding="utf-8") as fh:
        for line in fh:
            line = line.strip()
            brace = line.find("{")
            if brace < 0:
                continue
            try:
                ev = json.loads(line[brace:])
            except json.JSONDecodeError:
                continue
            if ev.get("ev") != "Draw":
                continue
            a = ev.get("after", {})
            m = a.get("mouse")
            tg = a.get("target")
            do = a.get("dotOff")
            if not (isinstance(m, list) and len(m) == 2):
                continue
            if not found_motion and prev_mouse is not None and m != prev_mouse:
                found_motion = True
            if found_motion:
                rows.append(
                    (
                        ev.get("t"),
                        m[0], m[1],
                        tg[0] if tg else None,
                        tg[1] if tg else None,
                        do[0] if do else None,
                        do[1] if do else None,
                    )
                )
                if len(rows) >= want:
                    break
            prev_mouse = m

    print(f"{'t':>8}  {'mx':>5} {'my':>5}  {'tx':>5} {'ty':>5}  "
          f"{'dx':>4} {'dy':>4}  {'mx-tx':>6} {'my-ty':>6}")
    for r in rows:
        t, mx, my, tx, ty, dx, dy = r
        dmx = (mx - tx) if (mx is not None and tx is not None) else None
        dmy = (my - ty) if (my is not None and ty is not None) else None
        print(
            f"{t:>8}  {mx:>5} {my:>5}  "
            f"{(tx if tx is not None else 0):>5} {(ty if ty is not None else 0):>5}  "
            f"{(dx if dx is not None else 0):>4} {(dy if dy is not None else 0):>4}  "
            f"{(dmx if dmx is not None else 0):>6} {(dmy if dmy is not None else 0):>6}"
        )


if __name__ == "__main__":
    raise SystemExit(main())
