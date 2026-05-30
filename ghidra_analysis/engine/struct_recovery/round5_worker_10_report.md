# Round 5 — Worker 10/50 report

## Task

| Field | Value |
|-------|--------|
| **worker** | 10 / 50 |
| **mode** | WRITE |
| **scope** | `FUN_*` → evidence rename for **CDSVideoPlayer / track-manager (TM) / FLX-adjacent AV sync** in `0x00439000..0x0043D000` |
| **program** | `bulanci.exe` via user-ghidra-mcp |

## Status

**PARTIAL** — TM/anim cluster was largely pre-named from R3–R4; **6** remaining `FUN_*` in scope renamed with disasm + caller proof; **1** TM helper deferred (field-offset conflict).

## Scope inventory (`0x00439000..0x0043D000`)

| Band | Already named (sample) | Still `FUN_*` at start |
|------|------------------------|-------------------------|
| TM core `0x439010..0x439eb0` | `TM_TickBlit`, `TM_AdvanceFrame`, `ConstructTrackManager`, `SetCurrentTrack`, `InsertOrFindTrack`, … | `FUN_00439710` only |
| CDSVideoPlayer vtable | `CDSVideoPlayer_ScalarDeletingDtor`, `CDSVideoPlayer_AdvanceFrameAndPauseIfDone`, `CDSVideoPlayer_GetTypeInfo`, … | none |
| CDSAudioPlayer / AV | `CDSAudioPlayer_OnPlaybackTick`, `ComputeDurationMs`, `CDSAudioVideoPlayer_*` dtors | was `FUN_0043a030` → renamed |
| FLX decode | lives `0x00432xxx` (outside band); TM reads `seq+0x14` populated by `CDSFlxFile_BindStream` | — |
| DirectPlay / DirectSound / MemQueue `0x43b000..0x43cd000` | many `FUN_*` | **skipped** (not TM/FLX) |

## Evidence — renames applied

| Old name | New name | Address | Proof |
|----------|----------|---------|-------|
| `FUN_00438fd0` | `TM_SetAnimFrameFromAnimSub` | `0x00438fd0` | Sole caller `CLevelScriptOpExt_SetAnimFrame@0x00416bd0` → `view+0x98`; body `CALL TM_…(this+0x10, frame)` |
| `FUN_004397e0` | `TM_ShiftTrackEntries` | `0x004397e0` | Disasm: 8-byte `{key,seq*}` memmove; sole caller `TM_InsertTrackAt@0x00439a70` insert path |
| `CGaming_PreMatchModal_ArmSchedulerSlot0` | `CGaming_ArmTrackMgrSchedulerIfUnpaused` | `0x00439880` | Caller `CGaming_RunPreMatchModal@0x0041c290`; if `!bPaused` arms scheduler slot 0, stamps `+0x40` with `g_dwElapsedMs` |
| `CDSVideoPlayer_GetActiveTrackBytesPerFrame` | `TM_GetCurrentTrackSeqFrameCount` | `0x00439990` | Reads `currentTrack.seq+0x14` (`nSeqFrameCountMinusOne` / FLX meta); caller `CDSAudioPlayer_OnPlaybackTick@0x0043a1f0` → `TM_SeekToFrame` AV lip-sync |
| `FUN_00439bc0` | `TM_ResetTrackListHead` | `0x00439bc0` | `*(this+8)=0`; tail `TM_ClearTracks(this,0)`; unwind helpers @ `0x0047a763` / `0x0047a7cf` |
| `FUN_0043a030` | `CDSAudioPlayer_SetDirectSoundBufferPosition` | `0x0043a030` | Sole caller `CDSAudioPlayer_Create@0x00422310`; `CALL [pDirectSoundBuffer+0x40](position)` with HRESULT throw |

## Deferred (no rename)

| Address | Current name | Reason |
|---------|--------------|--------|
| `0x00439710` | `FUN_00439710` | Disasm `MOV [ECX+0x3c], EAX` — `ConstructTrackManager@0x00439c70` initializes **same offset** as `pNotifyCookie` (`MOV [ESI+0x3c], EBX`). Callers: `TM_SetAnimFrameFromAnimSub`, `CGunMouse_ctor` (stores literal `1`). **`TM_AdvanceFrame@0x004399b0` uses `dwCurrentFrameIdx` @ `+0x38`**, not `+0x3c`. Script table claims “frame index” but store target ≠ proven frame field — needs layout re-audit before rename. |

## Out of scope (left `FUN_*`)

| Address | Notes |
|---------|-------|
| `0x0043b120..0x0043cd000` | DirectPlay / DirectSound / DirectXException — worker 14/15 territory |
| `0x0043c9b0` | `CBulanci` gaming audio COM teardown; caller `CDSApp_OnDestroy` — not TM/FLX |
| `0x0043ad55` | No function entry in Ghidra |

## Ghidra deltas

- `rename_function_by_address` × 6 (see table).
- `set_function_prototype` on all six; `set_decompiler_comment` @ `0x00438fd0`, `0x004397e0`, `0x00439990`.
- `save_program bulanci.exe`.

## Struct doc updates

None — function-only pass; existing [CDSVideoPlayer.md](./CDSVideoPlayer.md) / [CDSFlxFile.md](./CDSFlxFile.md) / [anim_runtime.md](../anim_runtime.md) already cover TM layout.

## Remaining UNK

- `FUN_00439710` field semantics (`+0x3c` vs `dwCurrentFrameIdx @ +0x38`).
- DirectPlay/DirectSound `FUN_*` in band (other R5 workers).
