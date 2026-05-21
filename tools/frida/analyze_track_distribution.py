#!/usr/bin/env python3
"""Count how often each idle-twitch track was played in the trace.

A "play" is one full closed-loop cycle of a single broadcaster
(param1 in OnMouseMove records — the engine's per-`BitmapSprite`
event source pointer). We segment the OnMouseMove stream into
contiguous runs of the same broadcaster, and within a run the
cumulative `(Δx, Δy)` returns to (0, 0) at the end of each full
loop. Counting those zero-crossings gives the loop count per
broadcaster.

Output:
    <broadcaster_ptr>  loop_count  frames_per_loop  total_ticks
plus the normalised probability (per loop, per tick), which is what
the original engine's RNG-driven SetCurrentTrack roll has to match.
"""

from __future__ import annotations

import json
import sys
from collections import defaultdict, Counter
from pathlib import Path


def iter_events(path: Path):
    with path.open("r", encoding="utf-8") as fh:
        for line in fh:
            brace = line.find("{")
            if brace < 0:
                continue
            try:
                yield json.loads(line[brace:])
            except json.JSONDecodeError:
                continue


def main() -> int:
    path = Path(sys.argv[1] if len(sys.argv) > 1 else "trace.jsonl")

    # broadcaster -> list of full loops (each loop is a list of (dx, dy))
    loops_by_bc: dict[str, list[list[tuple[int, int]]]] = defaultdict(list)
    ticks_by_bc: Counter = Counter()

    # Running per-broadcaster accumulator. Whenever cumulative returns
    # to (0, 0) AND a non-trivial loop has elapsed, snapshot it.
    cur_bc: str | None = None
    cur_buf: list[tuple[int, int]] = []
    cur_sx = 0
    cur_sy = 0

    for ev in iter_events(path):
        if ev.get("ev") != "OnMouseMove":
            continue
        bc = ev.get("param1")
        deltas = ev.get("deltas")
        if not isinstance(bc, str) or not isinstance(deltas, list) or len(deltas) != 2:
            continue
        dx, dy = int(deltas[0]), int(deltas[1])
        ticks_by_bc[bc] += 1

        if cur_bc is None or cur_bc != bc:
            # Broadcaster switched. If the previous partial buffer was
            # mid-loop it's incomplete data; drop it.
            cur_bc = bc
            cur_buf = []
            cur_sx = 0
            cur_sy = 0

        cur_buf.append((dx, dy))
        cur_sx += dx
        cur_sy += dy
        if cur_sx == 0 and cur_sy == 0 and len(cur_buf) > 1:
            loops_by_bc[bc].append(list(cur_buf))
            cur_buf = []

    print(f"OnMouseMove events:       {sum(ticks_by_bc.values())}")
    print(f"distinct broadcasters:    {len(ticks_by_bc)}")
    print()
    # NOTE: with only 33 plays in this trace this is a small-sample
    # window — see the chi-squared report below for whether the
    # observed counts are statistically distinguishable from uniform.
    # The track-manager broadcaster is the SAME pointer across the
    # three sprites — every NotifyMove fans through the consumer
    # callback list registered at `CGunMouse + 0x6c`. Differentiating
    # the active sprite therefore has to come from the *loop length*,
    # which is unique per sprite (9 / 22 / 24).
    SPRITE_BY_LEN = {9: 65803, 22: 65802, 24: 65801}

    # Flatten all loops across all broadcasters and bucket by length.
    all_loops = [loop for runs in loops_by_bc.values() for loop in runs]
    by_len: Counter = Counter(len(l) for l in all_loops)
    total_loops = sum(by_len.values())

    print(f"loops detected:           {total_loops}")
    print()
    print(f"{'sprite':>6}  {'len':>4}  {'plays':>6}  "
          f"{'p(loop)':>8}  {'expected (uniform)':>20}")
    expected_uniform = 1.0 / 3.0
    for ln in sorted(by_len.keys()):
        sprite = SPRITE_BY_LEN.get(ln, "?")
        n = by_len[ln]
        p = n / total_loops
        deviation = (p - expected_uniform) * 100
        print(
            f"{str(sprite):>6}  {ln:>4}  {n:>6}  "
            f"{p:>8.4f}  {expected_uniform:>10.4f}    "
            f"({deviation:+.1f}% off uniform)"
        )

    print()
    print("Loop-length histogram (any unexpected lengths = bug or split run):")
    for ln in sorted(by_len.keys()):
        bar = "#" * by_len[ln]
        print(f"  len={ln:>3}: {by_len[ln]:>3}  {bar}")

    # Empirical weight expressed as integers: useful for matching a
    # bucket-table or a `rand() % N` over a small repeated table.
    print()
    print("Empirical bucket weights (counts):")
    counts = sorted(by_len.items(), key=lambda x: -x[1])
    g = counts[0][1]
    import math
    for ln, c in counts:
        g = math.gcd(g, c)
    print(f"  GCD of counts: {g}")
    print("  reduced ratio:")
    for ln, c in counts:
        sprite = SPRITE_BY_LEN.get(ln, "?")
        print(f"    sprite {sprite}  weight = {c // g}  (raw count = {c})")

    # Chi-squared GoF test against the uniform null. With n samples
    # split into 3 categories the expected count per cell is n/3; the
    # decision threshold for df=2 at α=0.05 is 5.991.
    print()
    n = total_loops
    exp = n / 3.0
    chi2 = sum(((c - exp) ** 2) / exp for c in by_len.values())
    print(
        f"Chi-squared test against uniform (n={n}, df=2):\n"
        f"  chi² = {chi2:.3f}\n"
        f"  α=0.05 critical = 5.991  (reject uniform if chi² > 5.991)\n"
        f"  α=0.01 critical = 9.210"
    )
    if chi2 <= 5.991:
        print(
            "  -> observed counts are CONSISTENT with a uniform 1/3/1/3/1/3 "
            "selector.\n  This matches the byte-level decomp of "
            "CGunMouse_OnAnimTick @ 0x00423bd0:\n      idx = (MSVCRT::rand() * 48) >> 19\n"
            "  which is a zero-bias uniform 3-way roll over rand()'s 0..32767 range."
        )
    else:
        print(
            "  -> observed counts REJECT the uniform null at this α level —\n"
            "     the engine is using a weighted table somewhere we haven't yet pinned."
        )


if __name__ == "__main__":
    raise SystemExit(main())
