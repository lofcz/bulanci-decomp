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

## `menu_hover_audio_trace.js` — CSwitch hover SFX

Runtime trace for the original main-menu button hover audio path. It
hooks the function ENTRY points of:

- `CSwitch::OnMouseEnter`      (`0x00424cf0`)
- `CSwitch::OnMouseLeave`      (`0x00424d10`)
- `CSwitch::PlayIdleTrack`     (`0x00423ef0`)
- `CSwitch::PlayHoverTrack`    (`0x00423f20`)
- `TriggerBankSample`          (`0x00422430`, slot-filtered to menu cues)
- `CDSAudioPlayer::Init`       (`0x0043a760`, filtered to menu trigger chain)

It deliberately does NOT hook the hot DirectSound mixer paths
(`CDSAudioPlayer_Play/Stop/ApplyEffectiveVolume`); volume / category
state are read once per Init, after the call has populated the player.

> **Address sanity:** every address above is a function entry. The
> previous iteration of this script hooked `CSwitch_PlayHoverTrack`
> at `0x00423f50`, which is `0x30` bytes INTO the function. Frida's
> 5-byte inline trampoline overwrote real instructions, so the
> first hover call landed in junk and the process died. If you ever
> retarget this script to a new address, double-check via
> `ghidra-mcp.get_function_by_address` first.

### Running

You will want a draggable window so you can actually mouse-hover the
menu buttons; chain `orig\patch_window.js` for that (see its header
for the full rationale).

```powershell
frida -l orig\patch_window.js ^
      -l scripts\frida\menu_hover_audio_trace.js ^
      -f orig\bulanci.exe ^
      -o hover_audio.jsonl
```

The Frida CLI does not accept `--no-pause` / `--resume` flags in
recent versions — type `%resume` at the prompt instead. Use `-o
hover_audio.jsonl` (rather than shell `>` redirection) so the prompt
stays on stdout and only the JSONL trace goes to the file.

### Capture these gestures

1. Move from empty menu space onto Start, then out.
2. Move quickly Start -> History -> Quit -> Start.
3. Hover the currently selected button (after clicking Start to
   navigate into Start-Game).
4. Click a non-selected button, then hover it again while selected.

### Important fields

- `CSwitch_OnMouseEnter.sw.state_0xc4`: selected buttons have state
  `1` and must NOT reach `CSwitch_PlayHoverTrack` (retail filters in
  `OnMouseEnter` itself).
- `TriggerBankSample.slot`: hover voice cue is global AudioBank slot
  `0`; click cues are `0x18`/`0x19`/`0x1a`/`0x1b`/`0x1c`.
- `CDSAudioPlayer_Init.volumePercent_0x54`: per-player volume percent
  after Init populates the struct. Default `100`.
- `CDSAudioPlayer_Init.category_0x4c`: category pointer used for the
  later effective-volume calculation (Music vs SFX vs Voice).
- `EXCEPTION`: emitted by `Process.setExceptionHandler` if anything
  crashes. Includes register dump and the bytes around `EIP` so a
  future regression in this script is diagnosable rather than just a
  bare "Process terminated".

## `menu_audio_mixer_trace.js` — menu music volume

Runtime trace for the original `CDSAudioPlayer` mixer path. Use this
when comparing OpenBulanci vs retail loudness; do not infer a linear
gain from `SetVolumePercent` without this trace.

It hooks:

- `CMenu::LoadBackgroundMusic` (`0x004252a0`)
- `CMenu::SetDayNightBg` (`0x004252f0`)
- `CMenu::EnableBackgroundState` (`0x00424010`)
- `CMenu::OnMusicFadeTick` (`0x00424080`)
- `CDSAudioPlayer::CreateFromResource` (`0x00422550`)
- `CDSAudioPlayer::Init` (`0x0043a760`)
- `CDSAudioPlayer::SetVolumePercent` (`0x0043a0d0`)
- `CDSAudioPlayer::ApplyEffectiveVolume` (`0x0043a060`)
- `CDSAudioPlayer::Play` / `PlayAndRelease` / `Stop`
- `TriggerBankSample` for the menu SFX slots
- the live `IDirectSoundBuffer::SetVolume` target, discovered from each
  player buffer's vtable at runtime.

### Running

```powershell
frida -l orig\patch_window.js ^
      -l scripts\frida\menu_audio_mixer_trace.js ^
      -f orig\bulanci.exe ^
      -o menu_audio_mixer.jsonl
```

Type `%resume` in the Frida prompt if needed.

### Important fields

- `volumePercent_0x54`: the engine's stored percent argument.
- `category_0x4c`: the mixer bus selector used with `DAT_004b8370`.
- `expectedEffectiveDb`: the DirectSound attenuation computed from
  Ghidra's `CDSAudioPlayer_ApplyEffectiveVolume` formula.
- `DirectSound_SetVolume.attenuationDb100`: the value retail actually
  passed to `IDirectSoundBuffer::SetVolume`, in hundredths of a dB.
- `DirectSound_SetVolume.linearGain`: `10^(attenuationDb100 / 2000)`,
  useful when comparing against `rodio::Sink::set_volume`.

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
