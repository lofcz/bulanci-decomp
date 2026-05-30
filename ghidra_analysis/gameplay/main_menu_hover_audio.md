# Main-menu hover SFX: the scheduler-slot-0 gate

The retail main-menu buttons (`CSwitch` at `0x00424bc0`) play their hover
voice cue (`AudioBank` slot `0`, ~360 ms mono 16-bit @ 22050 Hz) through
a per-button gate buried inside the TrackManager's `Scheduler`. This
section maps the gate from Ghidra and from a live Frida trace
(`scripts/frida/menu_hover_audio_trace.js`).

## 1. Function chain

```
CSwitch_OnMouseEnter      0x00424cf0
   └─ if (state_0xc4 == 0)
      └─ CSwitch_PlayHoverTrack       0x00423f20
            ├─ slot = Scheduler_GetEventSlot(this+0x80, 0)
            ├─ if ((slot[+8] & 1) != 0):              ← THE GATE
            │     SetCurrentTrack(this+0x7c, 0, 1)    ← track 0 = glow-up
            │     TM_Play(this+0x7c, 0)
            │     └─ TM_PauseAndStampClock(this+0x7c) 0x00439840
            │     │     └─ if !(slot[+8] & 1):
            │     │           slot[0] = g_dwElapsedMs
            │     │           Scheduler_ArmSlot(this+0x80, 0)     ← sets bit 0
            │     └─ Scheduler_AckSlot(this+0x80, 0, -1)         ← clears bit 0
            │     TriggerBankSample(1, 0, 0, 0, 0, 1)
            │     CDSAudioPlayer_PlayAndRelease(player, 0)
            │     ↑ runs only when the gate passes
            └─ (no audio when slot[+8] & 1 == 0)
```

`CSwitch_PlayIdleTrack` (`0x00423ef0`) is the dim-down counterpart and
uses the same `slot[+8] & 1` gate.

`Scheduler_ArmSlot` (`FUN_0042f300` @ `0x0042f300`):

```c
slot[+8] |= 1;        // bit 0 = "armed for manual trigger"
slot[+12] += 1;       // pending trigger count
```

`Scheduler_AckSlot` (`FUN_0042f330` @ `0x0042f330`, called from `TM_Play`):

```c
if (slot[+8] & 1) {
    slot[+12] -= 1;
    if (slot[+12] == 0) {
        slot[+8] &= ~1;                   // disarm
        if (slot[+8] & 2)                 // "stamp lastFire" flag
            slot[0] = g_dwElapsedMs;
    }
}
```

`Scheduler_DispatchDueEvents` (`0x0042eb30`) only fires the slot's
callback when `(slot[+8] & 1) == 0` and `now >= slot[0] + slot[1]`. So
bit 0 has **dual meaning**:

| `bit 0` | Dispatcher | `PlayHoverTrack` gate |
|---|---|---|
| 0 (cleared) | fires periodic callback | **does NOT** play audio |
| 1 (set)     | skips slot              | **plays** audio          |

The slot is "manually armed, periodic dispatch suppressed" while bit 0
is set; "periodic, audio inhibited" while bit 0 is clear.

## 2. Initial state (per CSwitch instance)

`ConstructTrackManager` (`0x00439cec`), called from `CSwitch_ctor`:

```c
Scheduler_RegisterEventSlot(trackMgr+4, /*idx=*/0, /*intervalMs=*/0, /*flags=*/7);
```

`flags = 0x7` = `bit0 | bit1 | bit2`:

- `bit 0` → start in **armed** state (audio eligible)
- `bit 1` → `slot[0] = g_dwElapsedMs` (stamp lastFire to now)
- `bit 2` → repeating slot (kept across dispatches; not freed after fire)

`Scheduler_RegisterEventSlot` also seeds `slot[+12] = 1` (pending = 1)
when bit 0 of the flags arg is set. So every fresh `CSwitch` enters the
main menu with **one pending audio trigger** ready to consume.

## 3. The arming cycle

A first hover on a never-touched button consumes the boot-seeded
pending trigger immediately, then enters the brightness cycle:

```
T=0    ctor                            pending=1, bit0=1   (armed)
T=t1   OnMouseEnter (state==0)
        └─ PlayHoverTrack: gate=1 → PROCEED
           SetCurrentTrack(0, 1)       (TM_PauseAndStampClock = no-op,
                                        slot already armed)
           TM_Play(0):
             TM_PauseAndStampClock     no-op
             Scheduler_AckSlot         pending=0, bit0=0   (DISARMED)
           TriggerBankSample(slot=0)   → hover SFX plays once
T=t1..   glow-up track 0 dispatches frames (8 × 71 ms = 568 ms)
```

While the gate is disarmed the dispatcher (`Scheduler_DispatchDueEvents`
@ `0x0042eb30`) fires the per-frame callback. Any
`OnMouseEnter`/`OnMouseLeave` arriving during the run still calls
`PlayHoverTrack`/`PlayIdleTrack`, but those bail at the same gate
because `slot[+8] & 1 == 0` — the animation cannot be interrupted.

When the active track's frame index wraps from 7 → 0,
`FUN_00439b90` (the CDSVideoPlayer per-frame callback) calls
`TM_PauseAndStampClock`, which re-arms the slot. `CSwitch_OnAnimEnd`
(`0x00424e10`) then fires synchronously with `frameIdx == -1` and
inspects the **live** mouse-over / state values to pick the next
direction:

```
if (aux0_0xa8 == 0) {           // glow-up just ended
    if (!mouseOver && state==0)  PlayIdleTrack();      // → dim-down
    // else: stay parked at fully bright
} else {                         // dim-down just ended
    if (mouseOver || state!=0)   PlayHoverTrack();     // → glow-up + SFX
    // else: stay parked at fully dim
}
```

The crucial consequence: the hover voice cue can fire on **two**
distinct events:

1. **Immediate** — `OnMouseEnter` while the gate is already armed
   (boot-seeded, or armed after a previous animation parked).
2. **Delayed** — `OnAnimEnd` of an in-flight **dim-down** with the
   cursor still over the button. The cue waits for the dim-down to
   complete, then plays at that instant only if `mouseOver` is still
   true. If the cursor has already left, no cue fires for that hover
   gesture.

Empirically (`hover_audio.jsonl`, n=30 → n=35):

```
t=3700   OnMouseEnter QUIT       aux0=1 aux1=7   ← user enters mid-dim-down (frame 7)
t=3700   PlayHoverTrack QUIT     gate=0          ← bails, no audio yet
   ── 43 ms while dim-down runs to its wrap ──
t=3743   Scheduler_ArmSlot QUIT  (FUN_00439b90 → TM_PauseAndStampClock)
t=3743   PlayHoverTrack QUIT     (from OnAnimEnd, gate=1 now → PROCEED)
t=3743   Scheduler_AckSlot QUIT  (TM_Play disarms again)
t=3743   TriggerBankSample(0)    ← hover SFX plays here, 43 ms late
```

Net effect: a single hover may produce **no audio at all**, **immediate
audio**, or **delayed audio** depending on the brightness phase the
button was in when the cursor entered.

## 4. Cross-button independence

The gate lives in *each* CSwitch's own scheduler (`this+0x80`), so
hovering button A and then B back-to-back fires the SFX twice even
within the same frame — exactly what the trace records at e.g.
`t=8001` (QUIT) → `t=8066` (HISTORY), 65 ms apart, both playing.

## 5. Empirical verification (Frida trace)

`scripts/frida/menu_hover_audio_trace.js` captured 159 events under
the gestures listed in `scripts/frida/README.md`. Identifying buttons
by `(x, cmd_0xc6)`:

| `ptr`         | `(x, y)`  | `cmd_0xc6` (dec) | Role     |
|---------------|-----------|------------------|----------|
| `0x961fbb0`   | (35, 37)  | 201 (`0xc9`)     | START    |
| `0x961fc80`   | (35, 121) | 202 (`0xca`)     | HISTORY  |
| `0x961fd50`   | (35, 205) | 203 (`0xcb`)     | QUIT     |

### 5.1 Selected button silences

Every event on the START button after `t=2443` has `state_0xc4 == 1`
(START auto-selects on menu enter and stays selected for the whole
trace). For those entries `CSwitch_OnMouseEnter` fires but
**`PlayHoverTrack` does not** — the `state==0` filter in `OnMouseEnter`
short-circuits before the gate is even consulted. No selected-button
hover SFX, ever.

### 5.2 Per-button cooldown

Successful `TriggerBankSample(slot=0)` events on each non-selected
button:

| Button   | Times of successful hover (ms)                       | Δ from previous (ms)              |
|----------|------------------------------------------------------|-----------------------------------|
| HISTORY  | 4445, 6210, 8066, 9257, 10425, 11595                 | —, 1765, 1856, **1191, 1168, 1170** |
| QUIT     | 6795, 8001, 9337, 10395, 11577                       | —, 1206, **1336, 1058, 1182**       |

Every `PlayHoverTrack` invocation that did **not** fire a
`TriggerBankSample` (i.e. the gate was 0) occurred strictly inside one
of those Δ windows on the same button.

Stable Δ ≈ 1100–1300 ms matches the full brightness cycle:

```
glow-up    (8 frames × 71 ms) = 568 ms
dim-down   (8 frames × 71 ms) = 568 ms
                       total ≈ 1136 ms
```

The empirically larger values (up to 1336 ms) are user-driven dwell
between the cycle completing and the next mouse-enter; the empirical
lower bound matches the cycle length to within 1–2 frame ticks.

### 5.3 Sample parameters

Every `CDSAudioPlayer_Init` snapshot for `slot=0` shows the same hover
sample at the same player initial settings:

```
sample.byteLen        15904       (mono, 16-bit, 22050 Hz → 360 ms)
volumePercent_0x54    100
cachedEffectiveVolume_0x50  0     (untouched until ApplyEffectiveVolume)
category_0x4c         0x1         (SFX bus)
flags                 0x42 (bit 1 | bit 6)
```

Players are short-lived: each hover allocates a fresh `CDSAudioPlayer`
(`CDSAudioPlayer_Create` → `Init` → `Play` → `Release`). There is **no
global "is the previous hover still playing?" mute**; the only
suppression is the per-button slot-0 gate analysed above.

## 6. Reimplementation in Rust (`open_bulanci/client`)

The retail behaviour is a per-button state machine over
`(track, frame, hover_armed)`, where the brightness animation is
**uninterruptible** once started. See
`open_bulanci/client/src/menu.rs::update_menu_input` for the live
implementation; the shape is:

```text
state[B] = (track, frame, hover_armed)
boot:      (1,     7.0,   true)        // parked at fully dim

per tick:
  want_glow = is_active_for_subscreen(B) || hovered_btn == Some(B)

  if hover_armed[B]:
      # gate open, animation parked at frame 7 of `track`
      match (track, want_glow):
          (1, true):  track=0, frame=0, armed=false
                      if !is_active_for_subscreen: play_hover_sfx()
          (0, false): track=1, frame=0, armed=false
          _:          # stable parked state, no-op
  else:
      # gate closed, animation dispatching
      frame = min(frame + dt / 0.071, 7.0)
      if frame >= 7.0:
          armed = true
          match (track, want_glow):
              (0, false): track=1, frame=0, armed=false
              (1, true):  track=0, frame=0, armed=false
                          if !is_active_for_subscreen: play_hover_sfx()
              _:          # stay parked at frame 7 of current track
```

Mapping to the retail mechanisms:

| Step                                  | Retail counterpart                                                                                |
|---------------------------------------|---------------------------------------------------------------------------------------------------|
| `boot (1, 7.0, true)`                 | `Scheduler_RegisterEventSlot(.., 0, 0, /*flags=*/7)` + `SetCurrentTrack(0,1)` parked at frame 0   |
| `(1, true)` while armed → glow-up + SFX | `OnMouseEnter` → `PlayHoverTrack` succeeds through `slot[+8] & 1 == 1` gate, `TM_Play(0)` AckSlots |
| `(0, false)` while armed → dim-down   | `OnMouseLeave` → `PlayIdleTrack` succeeds through gate, `TM_Play(1)` AckSlots                     |
| `frame >= 7.0` → `armed = true`       | `FUN_00439b90` → `TM_PauseAndStampClock` → `Scheduler_ArmSlot`                                    |
| Same-tick OnAnimEnd transition        | `CSwitch_OnAnimEnd` invoked synchronously, picks next track from live `mouseOver`/`state`         |
| Mid-animation `OnMouseEnter` ignored  | `PlayHoverTrack` bails at `slot[+8] & 1 == 0` gate while animation is dispatching                 |
| Delayed SFX after dim-down completes  | OnAnimEnd's `(track 1 ended) + mouseOver` branch re-enters `PlayHoverTrack` with the gate armed   |

Cross-button independence is automatic — each button has its own
`(track, frame, hover_armed)` triple. The selected sub-screen button
participates in the animation cycle but is filtered out of the SFX
firing (`if !is_active_for_subscreen`), mirroring `OnMouseEnter`'s
`if (state_0xc4 == 0)` short-circuit at `0x00424cf0`.

## 7. Function rename ledger

| Old name        | New name              | Reason |
|-----------------|------------------------|--------|
| `FUN_0042f300`  | `Scheduler_ArmSlot`    | Sets `slot[+8] |= 1` and increments pending count. |
| `FUN_0042f330`  | `Scheduler_AckSlot`    | Decrements pending count; clears `slot[+8] & 1` when pending hits 0. |

## 8. The "high-pitched intro" — rodio's linear-interp resampler

A direct A/B against retail revealed a clearly audible high-frequency
artifact in the first ~100 ms of every menu SFX in our build that retail
does not have, **even though the PCM bytes we play are byte-identical
to retail's `sample.ptr` data** (verified by inspecting
`unpacked/overlay/res_0000065874_43_AudioBank.wav` at offset `44` for
`byteLen = 15904`).  The user independently confirmed the asset is
fine by playing it through other Windows media players (Windows Media
Player, Groove) — only our build exhibits the artifact.

This section documents both the wrong path I went down (an "attack-
envelope" theory that led to a 5 → 25 ms `fade_in`) and the actual
root cause and fix.

### 8.1 Failed hypothesis: DirectSound pre-roll vs sharp PCM onset

`sfx_hover.wav` is the only menu SFX whose PCM **starts mid-oscillation**
rather than at digital silence (`open_bulanci/asset_pipeline/check_sfx_onsets.py`):

```
sample                 sr   ch  first 10 samples (mono)                                peak50  step5
sfx_hover.wav          22050 1   [-27, -206, -381, -428, -359, -266, -128, 44, 247, 474]  1095   179
sfx_radio_click.wav    22050 1   [-43, -62, -42, -37, -35, -32, -32, -26, -30, -24]       1577   20
sfx_history.wav        22050 1   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]                              1   0
sfx_quit.wav           22050 1   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]                              1   0
sfx_start.wav          22050 1   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]                              1   0
sfx_force_exit.wav     22050 1   [-301, -385, -271, -230, -154, -103, -43, -24, -13, -96] 4727   114
sfx_alt_exit.wav       22050 1   [-303, -387, -274, -232, -155, -104, -43, -25, -12, -97] 4764   113
```

The hover sample alone has a `step5 = 179` between samples 0 and 1 — a
~1.7 kHz transient at the very start.  The initial theory was that
DirectSound's secondary-buffer pre-roll (20–40 ms on XP-era hardware)
smoothed this transient, and that adding a `Source::fade_in(25 ms)`
in our `play_sfx_with_duration` would reproduce that softness.

Aligned 10 ms envelopes of the WASAPI loopback captures backed the
theory:

```
                          us (rodio, pre-fix)              retail (DirectSound)
  t after onset (ms)    peak rel. to file peak             peak rel. to file peak
  0                       0.025                            0.033
  10                      0.144   <- abrupt jump            0.125
  20                      0.172                             0.147
  30                      0.182                             0.179
  40                      0.168                             0.138
```

`SFX_FADE_IN_MS = 25` reduced the perceived "click" but the user
reported that an explicit fade made things *worse* once they listened
for it directly: "no, 25 is clearly a hack. Something ELSE is going on.
it's this distinct high pitched intro of the hover effect.  It's
something added to the first ~100 ms of every sample and we can't solve
it by fading."  That feedback prompted spectral analysis instead of
envelope analysis.

### 8.2 Root cause: rodio's `SampleRateConverter` is unfiltered linear interp

`scripts/fft_intro_compare.py` compares the FFT of the first 100 ms of
two WAVs band-by-band.  Running it against rodio's offline output of
`sfx_hover.wav` (decoder + `UniformSourceIterator` to 48 kHz stereo,
no fade, no sink, no cpal) vs the same source pre-resampled to 48 kHz
with scipy's polyphase Kaiser-window filter:

```
band (Hz)         linear-interp     pre-rendered      ratio
17500-18000           0.0285           0.0000      1443.5x
18000-18500           0.2395           0.0000     12356.3x
18500-19000           0.4427           0.0001      6756.0x
19000-19500           0.4361           0.0001      6188.5x
19500-20000           0.6124           0.0002      3430.3x
20000-20500           0.1966           0.0002       799.9x
```

That is the **smoking gun**.  Rodio 0.17.3's
`SampleRateConverter` (`src/conversions/sample_rate.rs`, lines 127-200)
implements **unfiltered linear interpolation between integer-sample
pairs**:

```rust
let sample = Sample::lerp(*cur, *next, numerator, self.to);
```

Linear interpolation has the frequency response of a triangular
window, which is **not** band-limited.  Every percussive onset in the
source produces large amounts of aliasing in the upper half of the
output spectrum.  For a 22050 → 48000 Hz hover with a ~1.7 kHz initial
transient, the aliased lobes land at 17-20 kHz, with reflections that
beat down into the audible 4-9 kHz band when mixed and converted back
through Windows' shared-mode mixer — which the listener perceives as
the "high-pitched intro".

Retail does not exhibit this because DirectSound hands the 22050 Hz
buffer directly to the Windows shared-mode mixer, which uses a high-
order polyphase / sinc resampler.  Windows Media Player and Groove
take the same path, which is why the user heard a clean sample in those
players.

### 8.3 The fix: pre-render SFX to 48 kHz at build time

Rodio's converter short-circuits when `from == to`
(`src/conversions/sample_rate.rs:70-72` and `:129-132`):

```rust
let (first_samples, next_samples) = if from == to {
    // if `from` == `to` == 1, then we just pass through
    debug_assert_eq!(from, gcd);
    (Vec::new(), Vec::new())
} else { ... };
```

The shipping asset path therefore needs to match the runtime's output
rate.  `open_bulanci/asset_pipeline/build_assets.py` now calls
`open_bulanci/asset_pipeline/resample_sfx.py` (a thin wrapper around scipy's
`signal.resample_poly` with the default Kaiser window — ≈100 dB
stopband attenuation) to convert each retail-extracted 22050 Hz mono
WAV to 48 kHz mono before copying it into
`open_bulanci/assets/audio/`.  The byte-identical 22050 Hz extraction
is preserved untouched in
`unpacked/.../extracted_all_samples/sample_*.wav` so that bank parity
with retail is still verifiable from source.

Runtime change in `open_bulanci/client/src/audio.rs::play_sfx_with_duration`
is minimal — the `fade_in` band-aid is removed and the source goes
straight to the sink:

```rust
let source = Decoder::new(cursor).ok()?;
let duration_ms = source.total_duration().map(...);
if let Ok(sink) = Sink::try_new(&self._stream_handle) {
    sink.append(source);
    sink.detach();
}
```

Because the source now reports `sample_rate() == 48000`, rodio's
internal pipeline (`Sink::append → UniformSourceIterator → SampleRateConverter`)
detects `from == to == 48000` and returns each input sample verbatim —
the only transformation that still happens is mono → stereo channel
doubling.  The FFT diff against retail in the affected bands collapses
from "1000-12000x more energy" to "1.4x at most" (full table in §8.4).

A startup diagnostic in `AudioManager::new` prints the cpal default
output's `sample_rate / channels / sample_format` so that future
non-48-kHz devices are obvious from the log; if such a device ever
appears, the right fix is to teach `build_assets.py` to ship a
per-rate variant, not to re-introduce the broken linear-interp path.

### 8.4 Reproducing the analysis

| Script                                | Purpose                                                                                  |
|---------------------------------------|------------------------------------------------------------------------------------------|
| `open_bulanci/asset_pipeline/check_sfx_onsets.py` | Dump first-10-samples + peak/step for every menu SFX (raw asset side).                   |
| `scripts/dump_full_envelope.py`       | 10 ms peak envelope of two WAVs, full duration, side by side.                            |
| `scripts/compare_hover_recordings.py` | Onset-aligned 5 ms windowed envelope + first 32 normalized samples.                      |
| `scripts/fft_hover_compare.py`        | Top FFT peaks + spectral centroid of the loudest 256 ms slice.                           |
| `scripts/fft_intro_compare.py`        | Band-by-band FFT comparison of the first 100 ms after onset.  This is what found the aliasing. |
| `open_bulanci/asset_pipeline/resample_sfx.py` | scipy polyphase resampler used by `build_assets.py` to produce the 48 kHz assets.   |

Two regression tests in `open_bulanci/client/src/audio.rs::tests`
keep the playback path honest:

- `shipped_sfx_assets_are_pre_resampled_to_48k` — every shipped menu
  SFX must decode at 48 kHz mono; if `build_assets.py` regresses
  and ships a 22050 Hz asset, this fails before the binary is built.
- `hover_pcm_dominant_frequency_matches_retail` — the dominant
  per-channel frequency of the first 10 ms of resampled hover stays
  under 3.5 kHz.  Spikes above that would mean rodio's resampler is
  actually engaging (which it shouldn't, given §8.3) and we're back to
  aliasing the source up.
