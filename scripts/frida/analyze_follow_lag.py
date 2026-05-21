#!/usr/bin/env python3
"""Quantify exactly how the original `CGunMouse` dot follows the cursor.

Two things we want to nail down from the trace:

1. **Push cadence** — how often does the engine actually push a sample
   into the 30-slot ring? In our Rust port we run a strict 10 ms
   scheduler slot, which gives a 300 ms steady-state lag. The original
   gates the push on `if (lastTick + 10 <= elapsed)` *once per Draw*,
   so the real cadence is bounded by the frame rate.

2. **Effective lag in wall-clock ms** — for each `Draw` event where
   the mouse is moving, how many ms-old is the position that ended up
   in `target_*`? Answered by walking back through the trace to find
   the last `Erase` (which writes `mouse` from
   `g_pApp+0xf0/+0xf4`) whose `mouse` equals this Draw's new
   `target`, and reporting the wall-clock delta `t_Draw - t_Erase`.

Usage:
    python tools/frida/analyze_follow_lag.py trace.jsonl
"""

from __future__ import annotations

import json
import statistics
import sys
from collections import deque
from pathlib import Path


def iter_events(path: Path):
    """Yield every JSON event in `trace.jsonl`, skipping the two
    human-readable header lines and any pure status messages."""
    with path.open("r", encoding="utf-8") as fh:
        for line in fh:
            line = line.strip()
            if not line:
                continue
            if line.startswith("[+]") or line.startswith("["):
                # Frida prefixes like "[Local::Bulanci.exe ]-> "
                # appear *before* the JSON payload — strip everything
                # up to the first '{'.
                brace = line.find("{")
                if brace < 0:
                    continue
                line = line[brace:]
            try:
                yield json.loads(line)
            except json.JSONDecodeError:
                continue


def analyze(path: Path) -> None:
    push_gaps_ms: list[int] = []
    pop_gaps_ms: list[int] = []
    follow_lag_ms: list[int] = []
    follow_lag_pos: list[tuple[int, int, int, int, int]] = []
    last_push_t: int | None = None
    last_pop_t: int | None = None
    last_pop_target: tuple[int, int] | None = None

    # Bound the per-event mouse window so the script doesn't run out of
    # memory on a 25 k-line trace. 256 entries covers ~4 s at the
    # observed event cadence; that's far more than the worst-case
    # 30-slot ring would ever need.
    mouse_window: deque[tuple[int, int, int]] = deque(maxlen=256)

    n_events = 0
    n_draw = 0
    last_draw_t: int | None = None
    draw_gap_ms: list[int] = []
    no_push_draws = 0
    for ev in iter_events(path):
        n_events += 1
        t = ev.get("t")
        kind = ev.get("ev")
        if t is None or kind is None:
            continue
        if kind == "Draw":
            if last_draw_t is not None:
                draw_gap_ms.append(t - last_draw_t)
            last_draw_t = t

        # Track the *live* mouse position every event carries
        # (`before.mouse`). This is what the engine reads from
        # `g_pApp+0xf0/+0xf4` at the top of Draw.
        before = ev.get("before", {})
        m = before.get("mouse")
        if isinstance(m, list) and len(m) == 2:
            mouse_window.append((t, int(m[0]), int(m[1])))

        if kind != "Draw":
            continue
        n_draw += 1
        after = ev.get("after", {})
        b_q = before.get("q", {}) or {}
        a_q = after.get("q", {}) or {}
        if not b_q or not a_q:
            continue

        # PUSH detection: queue grew, OR `head` stayed put while `size`
        # would have grown. Easier signal: lastTick moved by 10.
        b_lt = before.get("lastTick")
        a_lt = after.get("lastTick")
        if (
            isinstance(b_lt, int)
            and isinstance(a_lt, int)
            and a_lt == b_lt + 10
        ):
            if last_push_t is not None:
                push_gaps_ms.append(t - last_push_t)
            last_push_t = t
        else:
            no_push_draws += 1

        # POP detection: head advanced by exactly 1 OR target changed.
        b_head = b_q.get("head")
        a_head = a_q.get("head")
        cap = a_q.get("cap") or 30
        popped = (
            isinstance(b_head, int)
            and isinstance(a_head, int)
            and ((a_head - b_head) % cap) == 1
        )
        if popped:
            if last_pop_t is not None:
                pop_gaps_ms.append(t - last_pop_t)
            last_pop_t = t

            # The target written by Draw equals the slot at b_head
            # (which the pop just consumed). Verify directly via the
            # after-state.
            new_target = after.get("target")
            old_target = before.get("target")
            if (
                isinstance(new_target, list)
                and len(new_target) == 2
                and new_target != old_target
            ):
                tx, ty = int(new_target[0]), int(new_target[1])
                # Walk the mouse window back to find when (tx, ty)
                # was the live mouse pos. That timestamp tells us
                # how stale `target` actually is.
                for (mt, mx, my) in reversed(mouse_window):
                    if mx == tx and my == ty:
                        lag = t - mt
                        follow_lag_ms.append(lag)
                        follow_lag_pos.append((t, tx, ty, mouse_window[-1][1], mouse_window[-1][2]))
                        break

    def show(label: str, data: list[int]) -> None:
        if not data:
            print(f"  {label:<22} (no samples)")
            return
        data = sorted(data)
        print(
            f"  {label:<22} n={len(data):>4}  "
            f"min={data[0]:>5}  p50={statistics.median(data):>5}  "
            f"p95={data[int(0.95 * (len(data) - 1))]:>5}  "
            f"p99={data[int(0.99 * (len(data) - 1))]:>5}  "
            f"max={data[-1]:>5}  mean={statistics.mean(data):>6.1f}"
        )

    print(f"events parsed:    {n_events}")
    print(f"Draw events:      {n_draw}")
    print(f"Draws that pushed:{n_draw - no_push_draws}  ({100.0 * (n_draw - no_push_draws) / max(1, n_draw):.1f}%)")
    print()
    print("Gap distributions (engine ms — `g_dwElapsedMs` from inside the game):")
    show("draw-to-draw", draw_gap_ms)
    show("push-to-push", push_gaps_ms)
    show("pop-to-pop",   pop_gaps_ms)
    show("follow lag",   follow_lag_ms)
    print()
    if follow_lag_pos:
        print("First 5 follow-lag samples (t_draw, target, live_mouse):")
        for (t, tx, ty, lx, ly) in follow_lag_pos[:5]:
            print(f"  t={t:>7}  target=({tx:>4},{ty:>4})  live=({lx:>4},{ly:>4})")


if __name__ == "__main__":
    p = Path(sys.argv[1] if len(sys.argv) > 1 else "trace.jsonl")
    analyze(p)
