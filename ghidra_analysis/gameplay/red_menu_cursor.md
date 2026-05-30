# Red Menu Cursor (CGunMouse)

The main menu of Bulánci features a highly distinct, custom software-rendered
mouse cursor: a red sniper-scope reticle (`CGunMouse`) with a thin horizontal
and vertical crosshair stretching to the screen edges, and an **inner red
dot** that lags ~150 ms behind the cursor (measured on era hardware) and
plays one of three pre-authored idle-twitch sprite tracks driven by the
engine's track manager.

This document is the byte-level reverse-engineered specification of
`CGunMouse`. Every coordinate equation, predicate, and side-effect in here
is sourced from the decompiled functions listed below (Ghidra MCP).

> **Behavioural correction (May 2026 RE pass).**
> An earlier version of this document described the dot's motion as the
> output of a **spring/damped physics simulator** driven by the track
> manager. That description was **wrong** — the dot's positional anchor
> comes from a strict 30-slot FIFO queue (§4) plus the active
> `BitmapSprite`'s motion-path deltas (§5 / §6). There is no spring
> constant, no damping, no exponential decay, and no per-frame physics
> step.
>
> **Follow-lag correction (May 2026 trace).** An intermediate version
> of this document quoted the follow-lag as **300 ms** (= 30 × 10 ms).
> That number is the *upper bound* the in-Draw push gate enforces, not
> the value the original game actually exhibited. The Frida trace
> measures the lag at **134–140 ms** end-to-end — the gate operates in
> the "transient" regime on era-typical hardware (~200 fps), pushing
> at the render cadence rather than at the gate's 100 Hz cap. The
> rationale is captured separately in
> [`engine/effective_speed.md`](../engine/effective_speed.md) because
> the same pattern affects every gated subsystem in the engine.

---

## 1. Subsystem Architecture & Memory Layout

`CGunMouse` is a software-rendered cursor of size **`0x218` (536 bytes)**.
It extends `CDSImageMouse`, which manages saving the background behind the
cursor sprite footprint so that the SW-blitted cursor doesn't leave trails
on the screen backbuffer.

### Memory Layout Table (`CGunMouse`, size `0x218` bytes)

| Byte Offset | Member | Type | Description |
| :--- | :--- | :--- | :--- |
| `+0x00` | `m_vtable1` | `void*` | Primary vtable. |
| `+0x04` | `m_vtable2` | `void*` | Secondary input-interface vtable. |
| `+0x08` | `m_dwActiveState` | `uint` | 1 = initialised / active. |
| `+0x0c` | `m_vtable3` | `void*` | Coordinate / mouse-observer callback vtable (`0x00483770`). |
| `+0x10` | `m_bgLineX` | `CDSImage` | Saved BG behind the horizontal crosshair lines (size `0x60`). |
| `+0x70` | `m_bgLineY` | `CDSImage` | Saved BG behind the vertical crosshair lines. |
| `+0xd0` | `m_bgReticle` | `CDSImage` | Saved BG behind the reticle sprite. |
| `+0x130` | `m_bgDot` | `CDSImage` | Saved BG behind the inner dot sprite. |
| `+0x190` | `m_mouseX` | `int` | Current hardware-mouse X (latched at draw time from `g_pApp`). |
| `+0x194` | `m_mouseY` | `int` | Current hardware-mouse Y. |
| `+0x198` | `m_targetX` | `int` | Dot anchor X — value popped from the ring buffer once it saturates. |
| `+0x19c` | `m_targetY` | `int` | Dot anchor Y. |
| `+0x1a0` | `m_dotOffsetX` | `int` | "Frame-stale" copy of `m_dotBaseOffsetX`. Read by `Draw`. |
| `+0x1a4` | `m_dotOffsetY` | `int` | "Frame-stale" copy of `m_dotBaseOffsetY`. |
| `+0x1a8` | `m_dotBaseOffsetX` | `int` | Accumulator of the active BitmapSprite track's `NotifyMove(dx)` deltas — see §5 / §6. |
| `+0x1ac` | `m_dotBaseOffsetY` | `int` | Accumulator of the active BitmapSprite track's `NotifyMove(dy)` deltas. |
| `+0x1b0` | `m_trackManager` | `CDSTrackManager` | Sprite-frame animator (size `0x48`). Drives both the dot-frame index *and* the per-tick `(dx, dy)` broadcast to `OnMouseMove`. |
| `+0x1f8` | `m_lastTickMs` | `uint` | Last engine-clock value at which a coordinate was queued (10 ms gate). |
| `+0x1fc` | `m_pDotSprite` | `void*` | Pointer to the currently-active dot **BitmapSprite** (one of three — see §6). |
| `+0x200` | `m_pReticleSprite` | `void*` | Pointer to the reticle sprite resource (`0x100ad`). |
| `+0x204` | `m_bDotNeedsUpdate` | `char` | "An `OnMouseMove` fired this frame" latch. Cleared in `Erase`. |
| `+0x205` | `m_bActive` | `char` | 1 = cursor running, 0 = paused/hidden. |
| `+0x206` | `m_bDotReady` | `char` | 1 = the ring buffer is full → dot is renderable. |
| `+0x208` | `m_coordQueue` | `CoordQueue` | 30-element FIFO ring buffer of recent mouse coordinates. |

### Coordinate Queue (`CoordQueue`, size `0x10`)

The nested ring buffer at `CGunMouse + 0x208` is a fixed-capacity FIFO:

- `+0x00`: pointer to a heap-allocated array of `int[2]` POINTs.
- `+0x04`: capacity, **hard-coded to 30**.
- `+0x08`: current size (0..30).
- `+0x0c`: head read index.

The 10 ms enqueue gate in `CGunMouse::Draw` (§4.A) is a **minimum** delay
between pushes, not a fixed cadence: on era hardware (~200 fps) the gate
fires every render frame and the ring spans 30 × frame_interval ≈ 150 ms.
See [`engine/effective_speed.md`](../engine/effective_speed.md) for the
full regime analysis. Once the buffer saturates, every subsequent push
pops the oldest element into `(m_targetX, m_targetY)`.

---

## 2. Software Double-Buffering & Flicker Avoidance

Because Bulánci composites the cursor on top of a software-rendered
backbuffer (DDraw 8 bpp), the cursor must explicitly save and restore the
backbuffer pixels it overwrites — otherwise the page-flip would leave
streaks behind the cursor.

### A. `CGunMouse::Erase()` (`0x004249b0`)

1. Restores the four saved regions (`m_bgLineX`, `m_bgLineY`,
   `m_bgReticle`, `m_bgDot`) back onto the backbuffer (`g_pApp + 0x80`).
2. Registers their footprints with `_Globals::AddDirtyRect` so the
   subsequent flip submits them to the primary surface.
3. **Promotes the accumulated `m_dotBaseOffset` into `m_dotOffset`** and
   clears the per-frame "moved" latch:
   ```
   m_dotOffsetX     = m_dotBaseOffsetX;
   m_dotOffsetY     = m_dotBaseOffsetY;
   m_bDotNeedsUpdate = 0;
   ```
   `m_dotBaseOffset*` itself is **not** reset here. It carries forward
   into the next frame unchanged — see §5.B for why this matters.

### B. `CGunMouse::Draw()` (`0x00424610`)

1. Latches the live hardware-mouse position from the application loop:
   ```
   m_mouseX = g_pApp->m_mouseX;
   m_mouseY = g_pApp->m_mouseY;
   ```
2. Possibly pushes `(m_mouseX, m_mouseY)` into `m_coordQueue` (§4.A).
3. Saves the four backbuffer regions about to be painted into
   `m_bgLineX`, `m_bgLineY`, `m_bgReticle`, `m_bgDot`.
4. Renders, in this order (which is significant — see §3 and §7):
   1. Inner dot sprite **(only if `m_bDotReady != 0`)** at
      `(m_targetX + m_dotOffsetX - W_dot/2,
        m_targetY + m_dotOffsetY - H_dot/2)`.
   2. The four crosshair line segments at 50 % alpha red.
   3. The reticle sprite **on top of everything else**, opaque-blit,
      centered at `(m_mouseX, m_mouseY)`.
5. Registers all painted rects with `AddDirtyRect`.

---

## 3. Crosshair Line Drawing

The crosshair is four line segments extending from the screen edges to
the outer bounding box of the reticle sprite. The segments stop **at**
the reticle bounds so the reticle's interior is never crossed by them.

Let

- $M_x, M_y$ = current `(m_mouseX, m_mouseY)`,
- $W_r, H_r$ = reticle sprite width / height,
- $W_s, H_s$ = screen width / height (800×600).

then

$$
\begin{aligned}
R_\text{left}   &= M_x - W_r/2 & R_\text{right}  &= R_\text{left} + W_r \\
R_\text{top}    &= M_y - H_r/2 & R_\text{bottom} &= R_\text{top}  + H_r
\end{aligned}
$$

| Segment | From | To | Orientation |
| :--- | :--- | :--- | :--- |
| Left   | $(0,           M_y)$              | $(R_\text{left},   M_y)$ | horizontal |
| Right  | $(R_\text{right}, M_y)$           | $(W_s,             M_y)$ | horizontal |
| Top    | $(M_x, 0)$                        | $(M_x, R_\text{top})$    | vertical   |
| Bottom | $(M_x, R_\text{bottom})$          | $(M_x, H_s)$             | vertical   |

### Line colour and blending

Each segment is **1 pixel thick** and rendered with the
`CPoemScroller::FUN_00445da0` line kernel (the same engine helper the
poem scroller uses for its underlines). That kernel reads `ALPHA = 0x80`
from the per-call argument and blends source-red against the destination
backbuffer pixel **50/50**:

$$
\text{dst}.\text{rgb} \leftarrow \tfrac{1}{2}(\text{dst}.\text{rgb}) + \tfrac{1}{2}(\text{src}.\text{rgb})
$$

The previous version of this document claimed the lines were drawn
"in pure Red `0xff0000` with a shadow/blending key of `0x808080`". That is
**incorrect**: there is no separate shadow pass, and the `0x80` byte is
the blend alpha, not a shadow key. The visible result of the 50 % blend
against a dark menu is a slightly dimmed red that reads as anti-aliased,
even though the line is itself axis-aligned and 1 px wide. Over the dot
sprite (which is painted *before* the lines — see §2.B step 4.i) the
50 % blend lifts the dot's red to a near-pure bright red; the reticle is
painted last (step 4.iii) so it covers any blend artefact under itself.

---

## 4. Coordinate Ring Buffer & ~150 ms Lag

The inner dot does **not** follow the mouse instantaneously. Its target
coordinate is the mouse position captured 30 push-events ago — and on
era-typical hardware (~200 fps) those pushes happen at the render
cadence, so the effective lag is **30 × 5 ms ≈ 150 ms** (measured at
134–140 ms in the Frida trace).

### A. Tick queueing (`Draw`-time, gated on the engine clock)

Every `Draw` call increments `m_lastTickMs` by 10 ms if (and only if)
the gate passes:

```c
if (m_lastTickMs + 10 <= g_dwElapsedMs) {          // single `if`, NOT a while
    m_lastTickMs += 10;
    CGunMouse_CoordRing_Push(&m_coordQueue, &m_mouseX);  // sub @ 0x00424370
}
if (m_coordQueue.size == 30) {
    CGunMouse_CoordRing_Pop(&m_coordQueue, &m_targetX);  // sub @ 0x004243b0
    m_bDotReady = 1;
}
```

Three things are critical here:

1. The gate is a **single `if`**, not a `while`. So even after a long
   stall (e.g. 100 ms between Draws), exactly one sample is enqueued
   that frame — not 10.
2. `m_lastTickMs` advances by `+10`, not to `g_dwElapsedMs`. So if
   `g_dwElapsedMs - m_lastTickMs` is large, it stays large (the gate
   doesn't "catch up" the lost time). After the first second of play
   the gap is always > 10 ms and the gate fires on every Draw.
3. The pop step runs **unconditionally on every Draw**, gated only by
   `size == 30`. Once the ring is saturated `target_*` updates exactly
   once per Draw — i.e. the dot's apparent fps equals the render fps.

> **Effective rate.** Combined with point (2), the long-run push rate
> equals the frame rate whenever `frame_interval >= 10 ms`. At the
> trace's ~200 fps that puts the ring's time span at ~150 ms. At
> 60 fps it would be ~500 ms — which is why our port drives the queue
> from a fixed 5 ms scheduler beat instead (`SLOT_CURSOR_TICK` in
> `state.rs`) to keep the perceptual lag fps-independent. See
> [`engine/effective_speed.md`](../engine/effective_speed.md) for the
> full regime analysis and the same-shaped fix template for any other
> gated subsystem we need to port.

### B. Lag pop & saturation transition

The queue is empty at `Activate`, fills over the first ~30 pushes
(~150 ms on era hardware), and from then on operates in a steady state
where every push triggers a pop. While the queue is partially full
(`m_bDotReady == 0`) the dot is **not drawn at all** — only the
crosshair and the reticle are painted. The first frame on which
`m_bDotReady` flips to 1 is the frame on which the oldest of the first
30 samples (= the mouse position from "30 pushes ago") becomes
`m_target`.

---

## 5. Inner Dot Offset Accumulator (sprite-track deltas)

`m_dotBaseOffset*` and `m_dotOffset*` are **not** mouse-driven. They
are the per-frame snapshot/accumulator pair fed by the active
BitmapSprite's `NotifyMove` opcodes — i.e. the cursor's own
hand-authored idle-twitch motion path, completely independent of the
OS mouse. The full track catalogue is in §6.

### A. Fields and their per-frame relationship

- `m_dotBaseOffset*` is an integer accumulator. Every `NotifyMove`
  event from the active track adds its `(dx, dy)` to it. Because every
  track is a closed loop (cumulative sum = 0 over the full sequence)
  the accumulator returns to its starting value once per loop — there
  is no unbounded drift even though we never "reset" it.
- `m_dotOffset*` is a **one-frame-stale snapshot** of
  `m_dotBaseOffset*`, captured by `Erase` at the start of each frame.
- `m_bDotNeedsUpdate` is a per-frame latch: cleared in `Erase`, set in
  `OnMouseMove`. Used as a "first-event-this-frame" flag.

### B. `OnMouseMove(Δx, Δy)` (`0x00423900`)

Called for every `NotifyMove` opcode the active BitmapSprite emits
this frame. Despite the name, **the OS mouse never invokes this
callback directly** — the observer pointer is registered with the
track manager (`AddTrackSource`), not the input system. The `(dx, dy)`
arguments come from FLX opcode `0x0A` inside the sprite's frame stream
(see `engine/anim_runtime.md`).

```cpp
if (m_bDotNeedsUpdate == 0) {
    // First NotifyMove since the last Erase.
    // Re-seed m_dotBase from the (still-stale) m_dotOffset.
    m_dotBaseOffsetX = m_dotOffsetX + dx;
    m_dotBaseOffsetY = m_dotOffsetY + dy;
    m_bDotNeedsUpdate = 1;
} else {
    // Subsequent intra-frame NotifyMove events: just add the delta.
    m_dotBaseOffsetX += dx;
    m_dotBaseOffsetY += dy;
}
```

The "first event re-seeds from `m_dotOffset`" branch ensures the
accumulator base and the just-promoted snapshot stay coherent across
the frame boundary: at start-of-frame they're equal (by virtue of
`Erase`'s promotion), the first `NotifyMove` makes them equal again
(`base = offset + delta`, where `offset` is just the prior `base`),
and any subsequent `NotifyMove` simply adds its delta to `base`. Net:
at end-of-frame `base = offset_prev + Σ(deltas this frame)`.

### C. `Activate` (`0x00423b50`)

`Activate` is called when the cursor first appears and after every
`Deactivate`/`Activate` pair (e.g. when the menu re-takes mouse focus
after a sub-screen closes). It writes:

```
m_dotBaseOffsetX = 2;
m_dotBaseOffsetY = 3;
m_dotOffsetX     = 2;
m_dotOffsetY     = 3;
m_bDotNeedsUpdate = 0;
CGunMouse_CoordRing_Clear(&m_coordQueue);
m_bDotReady      = 0;
m_lastTickMs     = g_dwElapsedMs;
```

The `(2, 3)` seed is the dot's resting offset relative to the
reticle's centre — purely a literal in the binary.

### D. Per-frame state diagram (one tick of the menu loop)

1. `Erase`: `m_dotOffset ← m_dotBaseOffset`; `m_bDotNeedsUpdate ← 0`.
2. Zero-or-one `OnMouseMove(Δx, Δy)` callbacks for the active sprite's
   current frame (one per `NotifyMove` opcode; the master-pack tracks
   carry exactly one `0x0A` per frame).
3. `Draw`:
   - Latches the live OS mouse to `m_mouse*` (from `g_pApp+0xf0/0xf4`).
   - Possibly enqueues a 10 ms sample (§4.A).
   - If `m_bDotReady`, paints the dot at
     `(m_target* + m_dotOffset* − sprite/2)`.

The dot's screen position therefore reads as **(OS mouse, delayed by
~150 ms via the ring) + (sprite-track wobble, accumulated by
NotifyMove)**. When the OS mouse holds still for ≥30 push intervals,
the ring eventually feeds the still-frozen position into `m_target*`
and the dot catches up; the sprite-track wobble keeps running on its
own ~85 ms cadence regardless, which is why a perfectly idle cursor
still visibly drifts within a small bounding box (see §6).

### E. Apparent "elastic catch-up" during sustained motion

There is **no physics-style elastic term** — the only thing that
varies during sustained motion is `m_target*`. With the ring spanning
30 pushes (~150 ms on era hardware), a uniform 1000 px/s drag produces:

- `m_target*` ≈ mouse position 150 ms ago, i.e. ~150 px behind the
  live cursor in the direction of motion.
- `m_dotOffset*` is the sprite-track wobble, bounded by the active
  track's bbox (≤ 30 × 24 px for sprite 65801).
- Visible dot position = `m_target* + m_dotOffset*` ≈ mouse − 150 px,
  plus a small wobble. No spring, no damper.

---

## 6. Idle Sprite Tracks (visual frame + positional wobble combined)

The dot's "twitch when idle" is the combined output of two things the
same `BitmapSprite` resource carries:

1. **A 14×13 sprite-frame strip** — one frame per tick, blitted at
   `(m_target + m_dotOffset − sprite/2)`.
2. **A `NotifyMove(dx, dy)` opcode (0x0A) per frame** — broadcast to
   `CGunMouse::OnMouseMove` (§5.B), accumulated into
   `m_dotBaseOffset*`.

Both indices come from the same `sprite_frame` counter inside the
track manager. So the visual the player sees and the offset the dot
drifts to are **always in lock-step** — when frame 5 of a track is
on screen, the dot has accumulated exactly `Σ deltas[0..=4]` of
positional wobble. Breaking that sync (e.g. by playing the strip from
sprite A while applying deltas from sprite B) shows up immediately as
visible "skating".

### A. `OnAnimTick` track selection (`0x00423bd0`)

`OnAnimTick` is the cursor's observer-vtable callback (entry in
`m_vtable3` at `+0x0c`). It's invoked by the engine's track-manager
broadcast (`AddTrackSource → BroadcastFrameTimeHint`) when the active
track wraps (`param_2 == -1`):

```c
iVar1 = MSVCRT::rand();   // [0, 32767]
SetCurrentTrack(&m_trackManager,
    (iVar1 * 0x30 + (iVar1 * 0x30 >> 0x1f & 0x7fff)) >> 0x13,
    1);
```

The arithmetic simplifies (for non-negative `rand()`) to
`(rand() * 48) >> 19`, which partitions `[0, 32767]` into the three
buckets `[0, 10923]`, `[10923, 21846]`, `[21846, 32767]` —
10923 / 10923 / 10922 outcomes each, i.e. a **zero-bias 3-way uniform
roll**. Verified against the Frida trace in
`tools/frida/analyze_track_distribution.py` (chi² = 2.36, df = 2,
p ≈ 0.31). Anything cleverer (e.g. `rand() % 3`) would re-introduce
modulo bias on the 32767-period rand and silently desync.

### B. The three pre-registered tracks

`CGunMouse_ctor @ 0x00426060` walks `g_dwCGunMouseTrackSources` (at
`0x004af900`), calling `AddTrackSource` with each entry in turn, then
selects index 0 to play first via `SetCurrentTrack(0, 1)`. The
in-memory order is **`{65803, 65802, 65801}`** — note that this is
the *reverse* of the resource ID ordering you might naively expect.

| Track index | Resource ID | Decimal | Unpacked file | Frames | Frame size |
| :--- | :--- | :--- | :--- | :--- | :--- |
| 0 | `0x1010b` | 65803 | `res_0000065803_52_BitmapSprite.atlas.png` |  **9** | 14×13 |
| 1 | `0x1010a` | 65802 | `res_0000065802_52_BitmapSprite.atlas.png` | **22** | 14×13 |
| 2 | `0x10109` | 65801 | `res_0000065801_52_BitmapSprite.atlas.png` | **24** | 14×13 |

> Every track's per-frame `NotifyMove(dx, dy)` sum is **(0, 0)** over
> the full loop — verified both by static analysis of the unpacked
> sidecar (`motionPath.perFrameAbsolutePosition` block) and by the
> in-tree test `full_track_loop_returns_dot_base_to_starting_offset`
> in `cursor_tests.rs`. This closed-loop invariant is what lets the
> port re-roll the active track at end-of-loop with no visible jump.

Track cadence is the standard `CDSTrackManager`
`sequenceDefaultCadenceMs` (clip-duration ÷ frame-count;
`CGunMouse` does not override via the `speed != 100` formula); the
Frida trace measures **84–85 ms per tick** on all three tracks, which
is the value the port hard-codes on `SLOT_CURSOR_DOT_FRAME`.

### C. The client-side mirror

`open_bulanci/assets/cursor/manifest.json` lists the same three
tracks in the same order, alongside their PNG strip filenames and
their per-tick `(dx, dy)` arrays. `open_bulanci/asset_pipeline/build_assets.py`
bakes that manifest from the unpacker's `*.atlas.json` sidecars and
copies the matching `*.atlas.png` strips into `assets/cursor/`. At
runtime, `client/src/cursor_atlas.rs` loads the lot via the VFS into
a `CursorAtlasSet` that the cursor module then indexes with the same
single counter (`sprite_frame`), guaranteeing visual ↔ positional
sync.

### D. Frame timing

Every BitmapSprite in this engine derives its per-frame delay from one
of two formulas (see `engine/anim_runtime.md`):

- `speed != 100`: `delay_ms = round(4725 / speed)` per frame (uniform).
- `speed == 100`: per-frame `delay = ((f+1)·T)/N - (f·T)/N` ms, where
  `T` is the clip's total declared duration and `N` is the frame count.

For the cursor dot tracks, the spawner uses `speed == 100`, so each
track plays back at the as-authored cadence. The unpacker records the
exact resolved value in `transparencySource` /
`sequenceDefaultCadenceMs` in the atlas sidecar JSON.

### E. Compiler quirk: the `+0x1a4` ↔ `+0x1b0` shift

`OnAnimTick` is a virtual member of the observer interface whose vtable
lives at `CGunMouse + 0x0c`. The `this` pointer arrives shifted by
+0x0c so its `+0x1a4` accesses are physically at `+0x1b0` from the
start of the object — i.e. `m_trackManager`. This is purely a vtable-
interface offset trick; it does not imply that the track manager
"writes" `m_dotOffsetY` (which is also at `+0x1a4`). The byte-code at
`0x00423bd0` only ever **calls** `SetCurrentTrack` / `TM_Play` through
this pointer; it doesn't store to it.

---

## 7. Asset Pipeline Note: the dot-track chroma-key fix

All three dot atlases (resources 65801 / 65802 / 65803) ship in the
original binary with an **opcode-0x0D explicit transparent index of
100**. Palette slot 100 in these sprites is never written by an
opcode-0x09 palette write, so it stays at the default
`(255, 255, 255)` that `tools/bulanci_unpack/bulanci_unpack.py`
initialises the buffer with at `palette = bytearray([0xFF] * 1024)`.
The sprite's actual background is encoded at **palette index 0**
(which the inline palette does set to `(0, 0, 0)`), so a strict RGB
sibling lookup against `palette[100]` returns a sibling set that no
pixel in the sprite ever references — the chroma-key rewrite becomes
a no-op and the BG would render opaque black.

The unpacker's `_resolve_bg_key_indices` now validates the explicit
sibling set against the actual snapshot pixels and falls back to the
**perimeter-vote heuristic** (which correctly identifies palette[0] =
BLACK as the bitmap-edge majority) when the explicit set hits zero
pixels. The regenerated `*.atlas.png` strips correctly carry
`alpha = 0` over their background pixels, allowing the Rust client to
blit each frame directly via `draw_texture_ex` with no runtime
chroma-keying step. The pipeline that copies them into the runtime
VFS lives in `open_bulanci/asset_pipeline/build_assets.py` (see §6.C).

The engine itself does NOT need this fix; the live game runs on a 8 bpp
DDraw destination-side chroma key set on the back buffer surface that
filters out black at flip time anyway. The fix is purely for our
RGBA-pipeline atlas consumer.

---

## 8. Function Reference

| Function | Address | Role |
| :--- | :--- | :--- |
| `CGunMouse::ctor` | `0x00426060` | Allocates the four background buffers, walks `g_dwCGunMouseTrackSources` to load reticle + 3 dot tracks, calls `CGunMouse_CoordRing_Reserve(30)`, selects track 0. |
| `CGunMouse::Activate` | `0x00423b50` | Seeds `m_dotBase*` / `m_dotOffset*` to `(2, 3)`, calls `CGunMouse_CoordRing_Clear`, resets `m_bDotReady`. |
| `CGunMouse::Erase` | `0x004249b0` | Restores saved BG; `m_dotOffset ← m_dotBaseOffset`; `m_bDotNeedsUpdate ← 0`. |
| `CGunMouse::OnMouseMove` | `0x00423900` | Updates `m_dotBaseOffset*` per the two-branch rule in §5.B (called by the active BitmapSprite, not by OS mouse). |
| `CGunMouse::Draw` | `0x00424610` | Latches OS mouse, runs the (single-`if`) 10 ms push gate, pops when full, blits dot / lines / reticle. |
| `CGunMouse::OnAnimTick` | `0x00423bd0` | Re-rolls the dot's sprite track (0/1/2) via `(rand() * 48) >> 19` and restarts playback. |
| `CGunMouse_CoordRing_Push` | `0x00424370` | 30-slot FIFO push (no-op when full). Called only by `CGunMouse::Draw`. |
| `CGunMouse_CoordRing_Pop` | `0x004243b0` | 30-slot FIFO pop. Called only by `CGunMouse::Draw` (guarded by `size == 30`). |
| `CGunMouse_CoordRing_Clear` | `0x00423ac0` | Resets `size = 0`. Called only by `CGunMouse::Activate`. |
| `CGunMouse_CoordRing_Reserve` | `0x004253d0` | Resizes the backing array. Called once from `CGunMouse::ctor` with `capacity = 30`. |
| `CPoemScroller::FUN_00445da0` | `0x00445da0` | The shared 1-px line kernel used by both `Draw`'s crosshair and the poem scroller. ALPHA = 0x80 (50 % blend). |

