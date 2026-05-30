# CDSAudioVideoPlayer

## Status

**VERIFIED** — size `0x50` (80); Ghidra struct `CDSAudioVideoPlayer` @ `/CDSAudioVideoPlayer` with nested `CDSVideoPlayer videoTrackManager` @ `+0x08` (slice 24, 2026-05-30).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Alloc size **0x50 (80)** | `0x0043bca0` (`CDSAudioVideoPlayer_Constructor`) | `OperatorNewWithBadAlloc(0x50)` then ctor; entry **0x0043bca0** (not mid-body `0x0043bd00`) |
| Gameplay alloc | `0x00422d50` (`CMovieView::StartPlayback`) | Same `OperatorNew(0x50)` → `CDSAudioVideoPlayer_Constructor` → `CMovieView+0x80` (`m_pPlayer`) |
| Heap free on delete | `0x0043bd40` | `CDSAudioVideoPlayer_ScalarDeletingDtor` → `_free(this)` when `param_1 & 1` |
| Embedded TM ends at parent **0x50** | `0x00439c70` (`ConstructTrackManager`) | `+0x08` embed + `0x44` last field = `+0x4c`; object boundary at `0x50` |
| `SetupTrack` byte | `0x0043bba0` (`CDSAudioVideoPlayer_SetupTrack`) | `(this->videoTrackManager).bPaused = 1` (parent `+0x3d`) |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `pointer` | `vftable_IDSEventHandler` | `CDSAudioVideoPlayer_Constructor` @ `0x0043bca0`; dtor restore @ `0x0043bc10` |
| 0x04 | 4 | `CDSAudioPlayer *` | `pAudioPlayer` | ctor `pAudioPlayer=0` then heap `CDSAudioPlayer` (0x58) @ `0x0043bca0`; dtor release @ `0x0043bc10`; `Stop`/`Play`/`SetupTrack` use `this->pAudioPlayer` |
| 0x08 | 0x48 | `CDSVideoPlayer` | `videoTrackManager` | `ConstructTrackManager(&videoTrackManager)` @ `0x0043bca0`; `CDSVideoPlayer_TM_Destructor` @ `0x0043bc10`; see [CDSVideoPlayer.md](./CDSVideoPlayer.md) |

**Note:** `videoTrackManager.bPaused` (`+0x35` in embed) aliases parent byte at **`+0x3d`** (`SetupTrack@0x0043bba0`).

## Embedded vs standalone `CDSVideoPlayer` (0x48)

Same **`CDSObject::ConstructTrackManager`** / **`CDSVideoPlayer_TM_Destructor`** helpers; different ownership:

| Path | How obtained | Teardown |
|------|----------------|----------|
| **Embedded** (this class) | Subobject at `+0x08` inside `OperatorNew(0x50)` parent | `TM_Destructor` only in `CDSAudioVideoPlayer_dtor` — no `_free` on embed |
| **Standalone heap** | `CDSVideoPlayer::CreateTrackManagerHeap@0x00439f50`: `OperatorNew(0x48)` + `ConstructTrackManager` | `CDSVideoPlayer_ScalarDeletingDtor@0x00439fc0`: `TM_Destructor` + `_free` |

Standalone factory: sole xref **DATA** @ `CDSVideoPlayer_StaticClassRegister@0x0047d750` (class id `0x31`, `HandleClassRegister`) — no direct gameplay caller found.

`SetupTrack` wires sync: `CDSAudioPlayer_Init`, `AddTrackSource(&videoTrackManager)`, `*(pAudioPlayer+0x10) = &videoTrackManager`, `SetCurrentTrack` / `TM_AdvanceFrame`.

## Leaf functions (slice 24)

| Address | Symbol | Role |
|---------|--------|------|
| `0x0043bca0` | `CDSAudioVideoPlayer_Constructor` | `OperatorNew(0x50)` path; embed TM + heap audio |
| `0x0043bba0` | `CDSAudioVideoPlayer_SetupTrack` | Wire `pAudioPlayer`, `videoTrackManager`, `bPaused` |
| `0x0043bbf0` | `CDSAudioVideoPlayer_Play` | `CDSAudioPlayer_Play(pAudioPlayer, …)` |
| `0x0043bc00` | `CDSAudioVideoPlayer_Stop` | `CDSAudioPlayer_Stop(pAudioPlayer, 1)` |
| `0x0043bc10` | `CDSAudioVideoPlayer_dtor` | TM destroy + audio release |
| `0x0043bd40` | `CDSAudioVideoPlayer_ScalarDeletingDtor` | dtor + optional `_free` |
| `0x0043bc90` | `CDSAudioVideoPlayer_GetTypeInfo` | RTTI |
| `0x00422d50` | `CMovieView::StartPlayback` | Primary consumer (alloc + `SetupTrack` + `Play`) |

## Ghidra apply

```
get_struct_layout CDSAudioVideoPlayer → Size: 80
  +0x00 vftable_IDSEventHandler
  +0x04 pAudioPlayer
  +0x08 videoTrackManager (CDSVideoPlayer, 72 B / 0x48 embed)
modify_struct_field videoTrackManager → CDSVideoPlayer (agent todo 25 R3, 2026-05-30)
set_function_prototype:
  CDSAudioVideoPlayer_Constructor@0x0043bca0 → CDSAudioVideoPlayer * __fastcall (CDSAudioVideoPlayer *this, int)
  CDSAudioVideoPlayer_dtor@0x0043bc10 → void __fastcall (CDSAudioVideoPlayer *this)
  CDSAudioVideoPlayer_ScalarDeletingDtor@0x0043bd40 → CDSAudioVideoPlayer * __thiscall (…, uchar)
  CDSAudioVideoPlayer_SetupTrack@0x0043bba0 / Play@0x0043bbf0 / Stop@0x0043bc00
set_decompiler_comment @0x0043bca0 @0x0043bba0 @0x00422d50 (R3 todo 25)
save_program bulanci.exe (agent todo 25 R3)
```

**Agent todo 25 R3 (2026-05-30):** `videoTrackManager` retyped from legacy `byte[72]` to nested **`CDSVideoPlayer`** (72 B @ `+0x08`). Decompile: `ConstructTrackManager(&this->videoTrackManager, …)`, `(this->videoTrackManager).bPaused = 1`, `CDSVideoPlayer_TM_Destructor` on embed. `CMovieView::StartPlayback@0x00422d50` allocates `OperatorNew(0x50)` and stores in `m_pPlayer`.

**Agent todo 23 (2026-05-30):** Stale nested **1 B** type removed (`delete_data_type` on category `cdsaudiovideoplayer`). `CDSAudioVideoPlayer *` resolves for `__fastcall` ctor/dtor.

## RTTI / vtable

| Vtable | Address | Role |
|--------|---------|------|
| `g_pCDSAudioVideoPlayer_vftable_IDSEventHandler` | `0x0048948c` | Primary (`IDSEventHandler`, 4 slots) |
| Type info | `CDSAudioVideoPlayer_GetTypeInfo` @ `0x0043bc90` → `DAT_004b8468` | |

## Follow-up (round 3 task 09)

- Resolved embedded `0x48` vs heap factory; rebuilt Ghidra struct with `videoTrackManager`.
- Proven `+0x3d` === `videoTrackManager.bPaused`.

## UNK

- Runtime use of heap `CDSVideoPlayer` via class id `0x31` registration (if any).
- `CDSAudioVideoPlayer_Stop@0x0043bc00` may still decompile with opaque `this` (MCP __thiscall ECX limit); prototype set to `CDSAudioVideoPlayer *`.
