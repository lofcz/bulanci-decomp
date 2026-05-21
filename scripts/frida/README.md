# Frida instrumentation for the original Bulanci game

Scripts in this folder attach to a running `Bulanci.exe` and stream
runtime data out as line-delimited JSON. They are *only* used for
reverse engineering; nothing the reimplementation ships with depends
on them.

## Prerequisites

```powershell
python -m pip install --user frida-tools
```

Frida ships its own injection runtime, so nothing else is needed —
no PDB, no debugger, no admin (Bulanci.exe is 32-bit and runs
unelevated).

## `cursor_trace.js` — the CGunMouse pipeline

End-to-end trace of every input that touches the menu's red software
cursor. Hooks:

| Address      | Symbol                          | Why it's hooked |
| ------------ | ------------------------------- | --------------- |
| `0x0042a5c0` | `CDSApp::MouseQueue`            | Win32 WM_MOUSEMOVE enters the engine here |
| `0x00429db0` | `CDSApp::DispatchInputEvent`    | Writes the live mouse to `g_pApp[+0xe0]/[+0xe4]` |
| `0x00423b50` | `CGunMouse::Activate`           | Seed `(2, 3)` for dot offset |
| `0x004249b0` | `CGunMouse::Erase`              | Promotes `dot_base` → `dot_offset` each frame |
| `0x00424610` | `CGunMouse::Draw`               | Push (10 ms gate) / pop (size==30) / render |
| `0x00423900` | `CGunMouse::OnMouseMove`        | Mystery observer callback that writes `[+0x19c]` and `[+0x1a0]` |

For every `Draw` call (= once per visible frame) the script logs the
full struct slice (`+0x190…+0x20c`) **before and after** the push/pop
plus the entire 30-slot ring buffer in chronological order. That is
enough to reconstruct what the engine actually fed into the dot's
final blit position, frame by frame, on top of every OS mouse motion.

### Running

Launch the game under Frida:

```powershell
frida -l tools\frida\cursor_trace.js -f path\to\Bulanci.exe --no-pause > trace.jsonl
```

Or attach to a running instance:

```powershell
frida -l tools\frida\cursor_trace.js Bulanci.exe > trace.jsonl
```

Each line of `trace.jsonl` is a self-contained JSON record:

```json
{"n":42,"t":12345,"ev":"Draw","gm":"0x09a40000",
 "before":{"mouse":[404,302],"target":[400,300],"dotOff":[2,3],
           "dotBase":[2,3],"lastTick":12340,"dotReady":1,
           "q":{"cap":30,"size":30,"head":7,"data":[[400,300],...]}},
 "after":{"mouse":[404,302],"target":[400,300],"dotOff":[2,3], ...}}
```

The `t` field is `g_dwElapsedMs` (DWORD at `0x004b3bd8`), so events
across hooks share a single, monotonic clock you can sort/diff on.

### Suggested gestures

To isolate the *exact* policy that produces the catch-up the user
keeps describing, capture short runs (≤ 5 s) of:

1. Idle (mouse parked) — to see the steady-state OnMouseMove pattern
   and confirm the 24-frame sprite cadence drives the per-frame dot
   wobble.
2. Slow constant-velocity drag — both axes — to read off the lag in
   pixels between `mouse` and the dot's actual screen position.
3. Fast diagonal flick — to verify whether the dot can ever be
   visually centred under the crosshair while moving.
4. Stop-after-flick — to see how long the dot keeps moving after
   the mouse stops (= how full the queue is of stale samples).

For each gesture, replay the trace through a small Python notebook
(plot `mouse[0]` vs `after.target[0]` vs `mouse[0] - after.target[0] -
sw/2 + after.dotOff[0]`) and the dot's law-of-motion should fall out
without any more guessing.

## Companion analysis scripts

The Python helpers next to `cursor_trace.js` consume the `trace.jsonl`
above:

| Script                          | What it answers |
| ------------------------------- | --------------- |
| `analyze_trace.py`              | High-level breakdown of every emitted event kind plus a per-`BitmapSprite` motion-path segmentation (matches the three idle drift loops 65801/65802/65803). |
| `check_sprite_match.py`         | Cross-checks the runtime motion paths against the unpacker's `motionPath.perFrameAbsolutePosition` block in the matching `*.atlas.json`. |
| `analyze_follow_lag.py`         | Quantifies push/pop cadence and follow-lag distribution. Distinguishes "300 ms gate-capped" (fps ≥ 100) from "frame-rate-limited" (lag = `30 × frame_interval`) regimes. |
| `extract_motion_segment.py`     | Tabulates a contiguous window of `Draw` events around the first mouse motion (`t, mx, my, tx, ty, dx, dy, mx-tx, my-ty`) — the canonical view for eyeballing whether `target_*` ever catches up to a fast flick. |

The trace we captured for the cursor-follow investigation runs at
~200 Hz with `lastTick` 8–49 s behind `g_dwElapsedMs`, so the push
gate fires every Draw. `analyze_follow_lag.py` reads:

```
draw-to-draw    p50= 5.0  p95= 10  max= 119
push-to-push    p50= 5.0  p95= 10  max= 119
follow lag      p50= 143  p95=289  max= 299  mean=196
```

i.e. queue spans `30 × 5 ms ≈ 150 ms` during the trace, capped at
the gate's `30 × 10 ms = 300 ms` ceiling. That envelope is what
`GunMouse::tick_queue` in `open_bulanci/client/src/cursor.rs`
reproduces, by running the same `if (last_push_ms + 10 <= now_ms)`
guard once per render frame.
