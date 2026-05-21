# Effective Speed: Why Bulanci's "10 ms" Gates Behave Like 5 ms On Era Hardware

A handful of subsystems in Bulanci use the same gate idiom:

```c
if (m_lastFireTick + N <= g_dwElapsedMs) {
    m_lastFireTick += N;
    do_the_thing();
}
```

The naive reading — "this fires once every N ms" — is **wrong on any
slow-rendering host**. The single `if` (not `while`) combined with the
in-place `+= N` advance is what makes it effectively *fps-coupled*
below a critical frame rate. This document pins down the regime
boundaries and lists the call sites we already know are affected.

---

## 1. The Two Regimes

Let `f` be the rendering frame rate, `T = 1000/f` the inter-frame
interval in milliseconds, and `N` the gate period (10 ms in the
known cases). The gate runs once per Draw / per host call. Two
regimes emerge:

### 1.A `T < N` — fast host, gate brakes the rate

`g_dwElapsedMs` advances by `T` per Draw, `m_lastFireTick` by `N` per
successful fire. After the first few frames `m_lastFireTick` *catches
up to* `g_dwElapsedMs`, then the gate begins to fail every
`(N - T) / T` frames. Long-run fire rate stabilises at exactly `1/N`
fires per ms, i.e. the gate behaves as a 100 Hz cap (for `N = 10`).

This is the textbook reading. **It only applies above the critical
fps `f_crit = 1000/N`** — 100 fps for `N = 10`.

### 1.B `T >= N` — slow host, gate fires every Draw

`g_dwElapsedMs` advances by `T` per Draw, `m_lastFireTick` by `N`.
The gap `g_dwElapsedMs - m_lastFireTick` grows by `T - N` per
frame and never shrinks. After a second or two the gap is large
enough that the gate condition is *always* satisfied; the `if`
fires on every Draw. Long-run fire rate equals the **frame rate**,
not `1/N`.

The gate has become a no-op brake. The only thing it still does
in this regime is throttle the *very first few* Draws while
`m_lastFireTick` catches up — once it falls behind it stays behind.

---

## 2. The Trace's Operating Point

`tools/frida/cursor_trace.js` ran with the original `Bulanci.exe`
under Frida instrumentation on a modern host. Empirically:

| Metric                                      | Value          |
| ------------------------------------------- | -------------- |
| Median Draw-to-Draw gap                     | **5 ms** (~200 fps) |
| `g_dwElapsedMs - m_lastFireTick` (whole trace) | 8 000–49 000 ms |
| Push-to-push interval (median)              | **5 ms**       |
| `CGunMouse` 30-slot ring follow-lag         | **134–140 ms** |
| Predicted lag at 200 fps in regime 1.B      | `30 × 5 = 150 ms` ✓ |

The trace runs entirely in regime **1.B**. The gate is doing
nothing — push rate equals frame rate, and the 30-slot ring spans
`30 × T = 30 × 5 ms ≈ 150 ms` of mouse history.

**This is almost certainly the rate the original was designed
against.** A 2007-era machine running the game uncapped would also
land in this regime; the gate exists to *cap* spikes when the
host suddenly runs *faster* than ~200 fps, not as the steady-state
governor.

---

## 3. Why The Port Cannot Just Replicate The Gate Literally

A faithful per-`Draw` reproduction of the `if (last + N <= elapsed)`
gate, run at macroquad's vsync-bound 60 fps:

```text
T = 16.67 ms,  N = 10 ms,  T > N    →  regime 1.B
push rate = 60 Hz
30-slot ring follow-lag = 30 × 16.67 ms ≈ 500 ms
```

That half-second lag is what the user immediately spotted as "boxy
follow / huge delay". The port has to emulate **the original's
trace-empirical 150 ms feel**, not its literal byte-level gate, to
look right at any fps.

### Implementation pattern (used by `CGunMouse`)

```rust
// state.rs
pub const CURSOR_TICK_MS: u64 = 5;       // the trace's effective beat
pub const SLOT_CURSOR_TICK: u32 = 0x01;

// app.rs
scheduler.register(SLOT_CURSOR_TICK, CURSOR_TICK_MS, true, now_ms);
// ...
SLOT_CURSOR_TICK => self.gun_mouse.on_cursor_tick(),

// cursor.rs — push+pop only, no internal gate (cadence is the gate)
pub fn on_cursor_tick(&mut self) { /* push, then pop if size == 30 */ }
```

The catch-up dispatcher (`bulanci_core::scheduler`) guarantees the
beat runs once per 5 ms of engine time regardless of render fps.
This is the cleanest expression of "the original's effective
cadence on era hardware" in Rust.

---

## 4. Known Call Sites Using The Gate Idiom

These are the functions we've identified that follow the
`if (last + N <= elapsed)` pattern. For each, regime 1.B matters
only if the port runs noticeably slower than the original was
designed for — *which is always the case at vsync-bound 60 fps* —
so each one is a candidate for the same scheduler-driven
remediation.

| Site                                              | Gate (ms) | Effect at 60 fps in port  | Notes |
|---------------------------------------------------|-----------|---------------------------|-------|
| `CGunMouse::Draw @ 0x00424610` push gate          | 10        | lag 500 ms vs trace 150 ms | Fixed via `SLOT_CURSOR_TICK`, 5 ms. |
| `CRuch::OnScheduledTick @ 0x00423e60` (TBD)       | ?         | ?                         | Already uses a scheduler slot — fine. |
| `CPoemScroller::OnScheduledTick @ FUN_00425df0`   | 120       | Scheduler-driven           | Already correct — 1 px / 120 ms, no `<=` gate. |
| `CDSApp_PulseTasks` slot pulse loop               | per-slot  | Catch-up loop             | Engine-level dispatcher itself; *enables* the fix. |

The list isn't exhaustive — any time you see a struct field at
offset `0x1f8` or similar holding a `m_lastFireTick`-style ms
timestamp, run the gate-vs-frame-interval analysis before
porting. If `T > gate_ms` on the user's machine and the gate is a
single `if` (not `while`), behaviour is fps-dependent and you
probably want a scheduler beat at the trace-empirical interval.

---

## 5. How To Triage A New "Possibly Affected" Function

Decision flow:

```text
1. Decompile the function. Look for:
       if (lastTick + N <= g_dwElapsedMs) { lastTick += N; ... }
   (Single `if`, in-place +=, no `while`.)
2. If absent: not affected by this issue. Stop.
3. If present, capture a Frida trace of the original game and
   measure the actual fire rate. Compare to N:
       fire_rate ≈ 1/N        → regime 1.A (gate acts as brake).
       fire_rate ≈ frame_rate → regime 1.B (gate is fps-coupled).
4. If regime 1.B:
       effective_period = 1000 / observed_fps_on_trace
       In the port, drive the underlying action from a scheduler
       slot with `delay_ms = effective_period`.
   If regime 1.A: a literal per-frame `if` in the port is fine —
       the gate genuinely caps the rate at 1/N regardless of fps.
```

---

## 6. Pinning The Frame Rate Itself

We have **not** found an explicit `Sleep(...)`-based fps cap in the
engine; the message pump is the only natural throttle. So the
original's `f` is determined by:

1. Vsync settings of the host DirectDraw / DirectInput config.
2. CPU throughput on the available work per frame.

On a 2007-era P4/Athlon-XP with DirectDraw vsync enabled at a
typical 85 Hz CRT or 60 Hz LCD, `f` would have been 60-85.
Uncapped, modern hardware easily exceeds 200 fps. The Frida
trace landed at 200 fps because the host happened to be running
the game windowed with vsync off; the design target was probably
in the 100-300 fps band, which is exactly where the gate is
effectively transparent (regime 1.B) but the absolute fire rate
stays close to 100-300 Hz.

For port purposes the takeaway is: **assume "original effective
rate" ≈ 200 Hz unless we have specific evidence otherwise**, and
size scheduler slots accordingly when porting a gated subsystem.

---

## 7. Cross-References

- `ghidra_analysis/engine/tick_system.md` — `g_dwElapsedMs`, the
  scheduler pulse loop, and the catch-up semantics.
- `ghidra_analysis/gameplay/red_menu_cursor.md` — the worked
  example for `CGunMouse` (this document's motivating case).
- `tools/frida/cursor_trace.js` + `tools/frida/analyze_follow_lag.py`
  — how the 200 fps / 5 ms beat / 150 ms lag numbers were measured
  on the original binary.
- `open_bulanci/client/src/cursor.rs` (module docs) + `state.rs`
  (`SLOT_CURSOR_TICK`) — the in-tree port pattern.
