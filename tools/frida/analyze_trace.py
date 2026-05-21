"""Quick offline analyser for cursor_trace.js output.

Reads `trace.jsonl` (path passed on CLI, defaults to ./trace.jsonl) and
prints a frame-aligned report so we can see the dot's law of motion at a
glance:

    [tick   ][dt   ] mouse        target       dotOff      dotBase     |
    [16682373][0   ] (480,   0)   (735, 206)   ( 2,  3)   ( 2,  3)    qsize=29
    [16682395][22  ] (480,   0)   (735, 206)   ( 2,  3)   ( 2,  3)    qsize=29
    ...

It also flags every line where any of (target, dotOff, dotBase) changed
relative to the previous Draw — those are the only data points that
actually need explaining.
"""
from __future__ import annotations

import json
import sys
from pathlib import Path

DEFAULT_TRACE = Path("trace.jsonl")


def load_events(path: Path):
    events = []
    with path.open("r", encoding="utf-8", errors="replace") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            # The frida REPL prefix and banner lines aren't JSON; skip
            # anything that doesn't begin with `{`.
            try:
                start = line.index("{")
            except ValueError:
                continue
            try:
                obj = json.loads(line[start:])
            except json.JSONDecodeError:
                continue
            events.append(obj)
    return events


def summarise(events):
    by_ev = {}
    for e in events:
        by_ev.setdefault(e.get("ev", "?"), 0)
        by_ev[e["ev"]] += 1
    print(f"# events by type: {by_ev}")


def report_draws(events):
    print()
    print(
        f"{'t':>10} {'dt':>5}  {'mouse':>12} {'tgt':>12} {'dotOff':>10}"
        f" {'dotBase':>10} {'q.size':>6} {'q.head':>6}  changed"
    )
    prev_after = None
    prev_t = None
    for e in events:
        if e.get("ev") != "Draw":
            continue
        a = e["after"]
        m = tuple(a["mouse"])
        tg = tuple(a["target"])
        d = tuple(a["dotOff"])
        b = tuple(a["dotBase"])
        q = a["q"]
        changed = []
        if prev_after is not None:
            if tuple(prev_after["target"]) != tg:
                changed.append("target")
            if tuple(prev_after["dotOff"]) != d:
                changed.append("dotOff")
            if tuple(prev_after["dotBase"]) != b:
                changed.append("dotBase")
        dt = (e["t"] - prev_t) if prev_t is not None else 0
        prev_t = e["t"]
        prev_after = a
        print(
            f"{e['t']:>10} {dt:>5}  "
            f"({m[0]:>5},{m[1]:>4}) ({tg[0]:>5},{tg[1]:>4}) "
            f"({d[0]:>3},{d[1]:>3}) ({b[0]:>3},{b[1]:>3}) "
            f"{q['size']:>6} {q['head']:>6}   "
            + ",".join(changed)
        )


def report_dotbase_changes(events):
    """Just the lines where dot_base changes (= OnMouseMove fired between
    this and the prior Draw). Lets us derive the cadence and the
    (dx, dy) increments at a glance."""
    print()
    print("# dot_base evolution (only changes shown)")
    print(
        f"{'t':>10} {'dt_since_prev':>14} {'mouse_now':>12}"
        f" {'dotBase_prev':>14} {'dotBase_now':>14} {'delta':>10}  notes"
    )
    prev = None
    prev_t = None
    for e in events:
        if e.get("ev") not in ("Draw", "Erase"):
            continue
        a = e["after"]
        b = tuple(a["dotBase"])
        if prev is not None and b != prev:
            d = (b[0] - prev[0], b[1] - prev[1])
            dt = e["t"] - prev_t
            m = tuple(a["mouse"])
            print(
                f"{e['t']:>10} {dt:>14} ({m[0]:>5},{m[1]:>4})"
                f" ({prev[0]:>5},{prev[1]:>5}) ({b[0]:>5},{b[1]:>5})"
                f" ({d[0]:>3},{d[1]:>3})  ev={e['ev']}"
            )
        prev = b
        prev_t = e["t"]


def report_dotoff_during_motion(events):
    """During mouse motion, look at how dot_offset (= dot_base snapshot
    at Erase time) compares to the mouse position."""
    print()
    print("# per-frame Draw — derived dot screen position vs live mouse")
    print(
        f"{'t':>10} {'mouse':>12} {'target':>12} {'dotOff':>10}"
        f" {'dot_screen':>14}  {'lag':>8}"
    )
    sw, sh = 14, 13  # cursor_dot.png frame is 14x13 per CGunMouse_Draw
    for e in events:
        if e.get("ev") != "Draw":
            continue
        a = e["after"]
        m = tuple(a["mouse"])
        tg = tuple(a["target"])
        d = tuple(a["dotOff"])
        dx = tg[0] + d[0] - sw // 2
        dy = tg[1] + d[1] - sh // 2
        lagx = m[0] - dx
        lagy = m[1] - dy
        print(
            f"{e['t']:>10}  ({m[0]:>5},{m[1]:>4}) ({tg[0]:>5},{tg[1]:>4})"
            f" ({d[0]:>3},{d[1]:>3}) ({dx:>5},{dy:>5})"
            f"  ({lagx:>3},{lagy:>3})"
        )


def report_onmousemove_origins(events):
    print()
    print("# OnMouseMove firings + backtrace")
    for e in events:
        if e.get("ev") != "OnMouseMove":
            continue
        print(f"  n={e['n']} t={e['t']} deltas={e['deltas']}"
              f" param1={e.get('param1')}")
        for bt in e.get("backtrace", []):
            print(f"      {bt}")


def report_motion_path(events):
    """Concatenate every OnMouseMove `deltas` field in time order and
    segment the stream into closed loops (contiguous slices whose
    running sum returns to 0).

    OnAnimTick re-rolls the active track at random intervals, so we
    expect ONE broadcaster (param1) emitting a sequence of distinct
    loops as the track index changes.
    """
    print()
    print("# OnMouseMove deltas — closed-loop segmentation")
    by_src = {}
    by_src_t = {}
    for e in events:
        if e.get("ev") != "OnMouseMove":
            continue
        src = e.get("param1", "?")
        by_src.setdefault(src, []).append(tuple(e["deltas"]))
        by_src_t.setdefault(src, []).append(e["t"])
    for src, deltas in by_src.items():
        ts = by_src_t[src]
        print(f"\n  ## broadcaster {src} — {len(deltas)} deltas")
        # Greedy segmentation: walk the deltas, accumulate, and start a
        # new segment every time the running sum is (0, 0) again.
        loops = []
        seg_start = 0
        sx = sy = 0
        for i, (dx, dy) in enumerate(deltas):
            sx += dx
            sy += dy
            if sx == 0 and sy == 0 and i > seg_start:
                loops.append((seg_start, i + 1, tuple(deltas[seg_start:i + 1])))
                seg_start = i + 1
                sx = sy = 0
        # Trailing (still-running) segment.
        if seg_start < len(deltas):
            loops.append((seg_start, len(deltas), tuple(deltas[seg_start:])))

        # Group identical loops to count repeats.
        by_shape = {}
        for (a, b, loop) in loops:
            by_shape.setdefault(loop, []).append((a, b))

        print(f"  → segments: {len(loops)}   distinct shapes: {len(by_shape)}")
        for shape, occurrences in sorted(
            by_shape.items(), key=lambda kv: -len(kv[1])
        ):
            first_a, first_b = occurrences[0]
            dur_ms = ts[first_b - 1] - ts[first_a] if first_b > first_a else 0
            x = y = 0
            absxy = [(0, 0)]
            for (dx, dy) in shape:
                x += dx
                y += dy
                absxy.append((x, y))
            bbox = (
                min(p[0] for p in absxy), min(p[1] for p in absxy),
                max(p[0] for p in absxy), max(p[1] for p in absxy),
            )
            closed = (x, y) == (0, 0)
            print(
                f"    shape len={len(shape):>3}  count={len(occurrences):>3}"
                f"  closed={closed}  bbox=({bbox[0]:>3},{bbox[1]:>3})"
                f"..({bbox[2]:>3},{bbox[3]:>3})"
                f"  first_dur~{dur_ms}ms"
            )
            if len(shape) <= 40:
                print(f"      deltas: {list(shape)}")
                print(f"      absolute: {absxy}")


def report_target_vs_mouse(events):
    """For each Draw, plot the time-aligned (mouse, target) pair and
    measure the *actual* lag (in number of pushes) by finding which
    earlier `mouse` value the current `target` matches."""
    print()
    print("# target vs. mouse lag (every 100th frame)")
    print(f"{'n':>6} {'t':>10}  {'mouse':>12} {'target':>12} {'lag_pushes':>10}")
    history = []  # list of (n, t, mouse)
    for e in events:
        if e.get("ev") != "Draw":
            continue
        a = e["after"]
        m = tuple(a["mouse"])
        tg = tuple(a["target"])
        history.append((e["n"], e["t"], m))
        # Find the most recent index whose mouse == target.
        lag = None
        for k in range(len(history) - 1, -1, -1):
            if history[k][2] == tg:
                lag = (len(history) - 1) - k
                break
        if e["n"] % 100 == 0:
            print(f"{e['n']:>6} {e['t']:>10}  ({m[0]:>5},{m[1]:>4})"
                  f" ({tg[0]:>5},{tg[1]:>4}) {str(lag):>10}")


def report_dot_screen(events):
    """Compute the dot's actual screen-space position each Draw
    (= target + dot_offset - sprite_half) and print every line where
    that position changes meaningfully."""
    print()
    print("# dot screen position (= target + dotOff − sprite_half)")
    print(f"{'n':>6} {'t':>10}  {'mouse':>12} {'dot_screen':>12}"
          f"  {'mouse - dot':>12}")
    sw, sh = 14, 13
    prev = None
    for e in events:
        if e.get("ev") != "Draw":
            continue
        a = e["after"]
        if not a.get("dotReady"):
            continue
        m = a["mouse"]
        tg = a["target"]
        d = a["dotOff"]
        if None in m or None in tg or None in d:
            continue
        dx = tg[0] + d[0] - sw // 2
        dy = tg[1] + d[1] - sh // 2
        if prev != (dx, dy) and e["n"] % 50 == 0:
            print(f"{e['n']:>6} {e['t']:>10}  ({m[0]:>5},{m[1]:>4})"
                  f" ({dx:>5},{dy:>4})  ({m[0]-dx:>5},{m[1]-dy:>4})")
            prev = (dx, dy)


def main():
    path = Path(sys.argv[1]) if len(sys.argv) > 1 else DEFAULT_TRACE
    events = load_events(path)
    summarise(events)
    if "--paths" in sys.argv:
        report_motion_path(events)
    if "--lag" in sys.argv:
        report_target_vs_mouse(events)
    if "--screen" in sys.argv:
        report_dot_screen(events)
    if "--om" in sys.argv:
        report_onmousemove_origins(events)
    if "--base" in sys.argv:
        report_dotbase_changes(events)
    if "--frames" in sys.argv:
        report_draws(events)


if __name__ == "__main__":
    main()
