# Round 10 — Deep Task 13 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 13 |
| **title** | Struct deep: CDSAudioPlayer DirectSound path |
| **kind** | struct |
| **struct_name** | `CDSAudioPlayer` |
| **seed_address** | `0x0043a030` |
| **addresses** | `0x0043a030`, `0x0043a100`, `0x0043a200` |
| **prior R5 block** | `round5_worker_14_results.jsonl` — `"0x0043a030:CDSAudioPlayer DirectSound"` |

## Status

**DONE** — R5 “blocked” was **scope-only** (worker 14 = DirectPlay slice; DirectSound is out of scope). Live Ghidra MCP proves `CDSAudioPlayer` DirectSound field layout and `IDirectSoundBuffer` vtable usage across all three seed functions. Corrected misleading R5 worker-10 rename (`SetDirectSoundBufferPosition` → spatial-atten path).

## R5 block resolution

| Claim | Evidence |
|-------|----------|
| Worker 14 marked `0x0043a030` BLOCKED | `round5_worker_14_report.md` §Proof table: “**BLOCKED (out of DirectPlay)** — Direct**Sound**, not DirectPlay” |
| Not a technical unknown | Worker 10 (same R5 band) already renamed @ `0x0043a030`; worker 14 deferred because DirectPlay worker cannot own audio symbols |
| Acceptance met | `pDirectSoundBuffer` @ `+0x14` and ring/playback fields proven by disasm on all three seeds; vtable slot map built from Play/Init/Refill/ApplyEffectiveVolume cross-check |

## Functions / Struct

### `CDSAudioPlayer` DirectSound fields (disasm proof)

| Offset | Name | Size | Seed evidence |
|--------|------|------|---------------|
| `+0x14` | `pDirectSoundBuffer` | 4 | All three seeds: `MOV EAX,[ESI+0x14]` / null-test before COM dispatch |
| `+0x30` | `dwDsBufferBytes` | 4 | OnPlaybackTick @ `0x0043a299` ring wrap; Refill no-DS path @ `0x0043a1d5` |
| `+0x34` | `dwDsPeriodBytes` | 4 | OnPlaybackTick @ `0x0043a285` refill window math |
| `+0x38` | `dwDsRingPos` | 4 | OnPlaybackTick @ `0x0043a28f` vs `dwDsPeriodBytes` |
| `+0x3C` | `dwDsSubOffset` | 4 | Refill @ `0x0043a12c`/`0x0043a15d`; OnPlaybackTick @ `0x0043a288` |
| `+0x40` | `dwPlaybackBytePos` | 4 | Refill @ `0x0043a137`/`0x0043a159`; OnPlaybackTick @ `0x0043a314` AV sync |

Struct size **0x58** unchanged ([CDSAudioPlayer.md](../struct_recovery/CDSAudioPlayer.md) VERIFIED).

### Seed functions

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x0043a030` | `CDSAudioPlayer_ApplySpatialAttenToDirectSound` | Post-`Init` spatial attenuation on DS buffer | **Disasm:** `CMP [ECX+0x14],0` → `CALL [vt+0x40](buffer, arg)` @ `0x0043a030`–`0x0043a044`. **Caller (1):** `CDSAudioPlayer_Create@0x00422399` when `preDelayOrFlags!=0`. **Arg chain:** `TriggerBankSample(..., attenDb, ...)` → Create 3rd arg → `ComputeSpatialAttenuationDb100` at spatial SFX sites (returns dB100, e.g. `(dist*3000)/800 - 0x5dc`). **Not** sample position — no byte/ms conversion. **IDA:** `sub_43A030(this+5, a2)` @ `bulanci.ida.exe.c:119356` — `this+5` = `+0x14`. |
| `0x0043a100` | `CDSAudioPlayer_RefillDirectSoundBuffer` | Non-streaming byte-position sync | **Disasm:** DS path `GetCurrentPosition` `[vt+0x10]` @ `0x0043a109`; end-of-sample `Stop` `[vt+0x48]` @ `0x0043a14a`; stores `dwDsSubOffset`/`dwPlaybackBytePos`. No-DS path: elapsed-ms × rate/ch/bps → bytes @ `0x0043a168`+. |
| `0x0043a200` | `CDSAudioPlayer_OnPlaybackTick` | Per-tick status, streaming refill arm, AV lip-sync | **Disasm:** `GetStatus` `[vt+0x24]` @ `0x0043a21f`; tests `DSBSTATUS_PLAYING/LOOPING/BUFFERLOST` bits; completion `CDSView__PostMessage(...,0x200,1,this,0)` @ `0x0043a22e`; streaming refill via `RefillDirectSoundBuffer` + `bRefillArmed` @ `+0x23`; `TM_SeekToFrame` when `pTrackSync` @ `+0x10` set @ `0x0043a305`+. |

### `IDirectSoundBuffer` vtable slots (binary-observed)

Mapped from disasm across `CDSAudioPlayer_*` cluster (signature → slot):

| Vtable + | Args (after `this`) | Inferred method | Proof site |
|----------|---------------------|-----------------|------------|
| `+0x10` | 2× out DWORD* | GetCurrentPosition | Refill @ `0x0043a109` |
| `+0x24` | 1× out BYTE* | GetStatus | OnPlaybackTick @ `0x0043a21f` |
| `+0x2C` | Lock (7 stack) | Lock | Init, FillDirectSoundBuffer |
| `+0x30` | 0, 0, flags | Play | Play @ `0x0043aa37` |
| `+0x34` | 0 | SetPan(0) | Play @ `0x0043a9ef` |
| `+0x3C` | db100 LONG | SetVolume | ApplyEffectiveVolume @ `0x0043a0ab` |
| `+0x40` | attenDb100 LONG | **UNK** (1-arg; spatial atten from Create) | ApplySpatialAtten @ `0x0043a041` |
| `+0x48` | none | Stop | Refill @ `0x0043a14a` |
| `+0x4C` | Unlock (4 stack) | Unlock | Init, FillDirectSoundBuffer |

Slot `+0x40` receives **dB100 spatial attenuation** from `TriggerBankSample` — disproves prior name `SetDirectSoundBufferPosition`. Exact COM export name for `+0x40` remains UNK (same 1-arg LONG signature as SetVolume/SetPan family; only one consumer).

### Create → DirectSound call chain

```
TriggerBankSample(category, bank, slot, attenDb, eventTarget, loopArm)
  └─ CDSAudioPlayer_Create(category, pSource, attenDb, eventTarget, loopArm)
       ├─ CDSAudioPlayer_Init(...)     // creates/binds pDirectSoundBuffer @ +0x14
       └─ if (attenDb != 0)
            CDSAudioPlayer_ApplySpatialAttenToDirectSound(this, attenDb)
              └─ pDirectSoundBuffer->vtable[+0x40](attenDb)
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0043a030` → `CDSAudioPlayer_ApplySpatialAttenToDirectSound` | Replaces misleading `CDSAudioPlayer_SetDirectSoundBufferPosition` |
| `set_function_prototype` | `void __thiscall CDSAudioPlayer_ApplySpatialAttenToDirectSound(int attenDb100)` | Success; `this` stays `void*` (API limit) |
| `set_decompiler_comment` | `0x0043a030`, `0x0043a0f0`, `0x0043a1f0` | Plate comments with vtable/offset proof |
| `save_program` | `bulanci.exe` | Saved |

## Decomp corrections (IDA vs Ghidra)

| Issue | Stale / R5 worker 10 | IDA / live disasm | Post-R10 |
|-------|----------------------|-------------------|----------|
| Function name @ `0x0043a030` | `CDSAudioPlayer_SetDirectSoundBufferPosition` | `sub_43A030` (unnamed) | `CDSAudioPlayer_ApplySpatialAttenToDirectSound` |
| 2nd parameter semantics | “position” | `a2` from Create = `a3` = attenDb when non-zero | `attenDb100` typed; TriggerBankSample chain proven |
| COM slot `+0x40` | “SetCurrentPosition” | 1-arg `__stdcall`; values are dB100 from spatial helper | Documented as UNK 1-arg; **not** SetCurrentPosition |
| Refill decomp locals | `unaff_EDI` for write cursor | `[ESP+0xc]` after GetCurrentPosition | Decompiler artifact; disasm @ `0x0043a128`/`0x0043a15d` shows store to `+0x3C` |

`bulanci.ghidra.exe.c` export still references old `SetDirectSoundBufferPosition` name — stale; use live MCP decompile.

## Frida

**none** — static xref + disasm + IDA correlation sufficient (`TriggerBankSample` chain already Frida-verified per `bulanci.ghidra.exe.c` @ `0x00422430` comment block).

## Remaining UNK

- **`IDirectSoundBuffer` vtable `+0x40`** exact export name (receives spatial `attenDb100`; 1-arg LONG — not sample position).
- **`CDSAudioPlayer_ApplySpatialAttenToDirectSound` `this` type** — Ghidra API cannot retype ECX to `CDSAudioPlayer *`; offsets in body are correct.
- **`config/bulanci/mapping.csv` / `_Globals.h`** still list old `SetDirectSoundBufferPosition` symbol — IDA re-export pass deferred (outside Ghidra mutation scope).
