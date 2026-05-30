# Round 5 — Worker 38 report

## Task

| Field | Value |
|-------|-------|
| **worker** | 38 / 50 |
| **mode** | WRITE |
| **scope** | Close `CDSAudioPlayer` / `CDSAudioBank` UNKs from struct_recovery docs |
| **types** | `CDSAudioPlayer`, `CDSAudioBank`, `CDSWavStream`, `CDSVideoPlayer`, `CDSAudioVideoPlayer` |
| **addresses** | `0x0043bba0`, `0x0043a1f0`, `0x00439990`, `0x00429470`, `0x00429480`, `0x0043ba30`, `0x0047c4b0` |

## Status

**DONE** — Ghidra saved (`bulanci.exe`).

## Findings

### CDSAudioPlayer — `pTrackSync` @ +0x10

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Typed **`CDSVideoPlayer *`** | struct `CDSAudioPlayer` | `modify_struct_field` @ +0x10 |
| Sole **writer** | `CDSAudioVideoPlayer_SetupTrack@0x0043bba0` | `pCVar1->pTrackSync = &this->videoTrackManager` after `AddTrackSource` |
| Ctor clears | `CDSAudioPlayer_ctor@0x0043a4f0` | `pTrackSync = 0` |
| **Init** does not touch | `CDSAudioPlayer_Init@0x0043a760` | no store @ +0x10 |
| Reader | `CDSAudioPlayer_OnPlaybackTick@0x0043a1f0` | if non-null: `CDSVideoPlayer_GetActiveTrackBytesPerFrame(pTrackSync)` then `TM_SeekToFrame(pTrackSync, frame)` from playback byte position |

**Conclusion:** `pTrackSync` is always an embedded or heap **`CDSVideoPlayer`** track-manager used for **movie / audio-video** sync (R3 todo 25 path). Bank SFX players created via `TriggerBankSample` leave it **null** — no other writers in binary.

### CDSVideoPlayer — sync scale helper

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Renamed | `0x00439990` | was `TM_GetCurrentTrackSeqFrameCount` → **`CDSVideoPlayer_GetActiveTrackBytesPerFrame`** |
| Logic | same | if `nCurrentTrackIdx >= 0`: return `trackVector[idx].field_0x14` (bytes-per-frame for active track) |
| Sole caller | `CDSAudioPlayer_OnPlaybackTick` | scales `dwPlaybackBytePos + dwDsSubOffset` against `pSource->dwSampleByteSize` before `TM_SeekToFrame` |

### CDSAudioBank — ctor vs factory vs wav tail

| Object | Alloc | Entry | Role |
|--------|-------|-------|------|
| **Class 67** index | `OperatorNew(0x20)` | `CDSAudioBank_Factory@0x00429470` | Registry @ `CDSAudioBank_StaticClassRegister@0x0047c4b0` (`PUSH 0x43`); compact header; `slotVector` zeroed |
| **0x40 bank facet** | (embedded / resource) | `CDSAudioBank_Ctor@0x00429480` | **No CALL xrefs** — vtables `0x486exx`, `dwInitFlag=1`, slotVector self-pointers; used when `+0x14` deserialize vtable present |
| **Class 43 PCM** | `OperatorNew(0x40)` | `CDSWavStream_Factory@0x0043bb00` | Wav vtables; `+0x14` bank deserialize **not** installed |

**`+0x24..+0x2f` (shared 0x40 tail):** renamed on `CDSAudioBank` to match `CDSWavStream`:

| Offset | Name | Consumer |
|--------|------|----------|
| +0x24 | `dwPcmBindLo` | `CDSWav_BindPcmMemStream@0x0043ba30` |
| +0x28 | `dwPcmBindHi` | same (cleared on bind) |
| +0x2C | `dwStreamTellHi` | wav stream slice / tell companion |

Bank ctor does not initialize these; only wav `face_8slots` path touches them.

## Ghidra deltas

| Action | Target |
|--------|--------|
| `modify_struct_field` | `CDSAudioPlayer.pTrackSync` → `CDSVideoPlayer *` |
| `modify_struct_field` / `add_struct_field` | `CDSAudioBank`: `dwPcmBindLo`, `dwPcmBindHi`, `dwStreamTellHi` @ +0x24..+0x2C (was `pGapWavPcmHelpers`) |
| `rename_function_by_address` | `0x00439990` → `CDSVideoPlayer_GetActiveTrackBytesPerFrame` |
| `set_function_prototype` | `uint __fastcall CDSVideoPlayer_GetActiveTrackBytesPerFrame(CDSVideoPlayer *)` |
| `set_decompiler_comment` | `0x00429470`, `0x00429480`, `0x00439990`, `0x0043bba0` |
| `save_program` | `bulanci.exe` |

## Struct doc updates

- [CDSAudioPlayer.md](./CDSAudioPlayer.md) — `pTrackSync` row; UNK cleared
- [CDSAudioBank.md](./CDSAudioBank.md) — factory vs ctor table; wav tail fields; UNK trimmed

## Remaining UNK

| Item | Notes |
|------|-------|
| C++ MI declaration order | Source not in repo |
| Class-67 `0x20` vs `0x40` | Whether compact bank headers are ever promoted to full deserialize footprint at runtime |
