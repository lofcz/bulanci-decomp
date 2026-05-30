# Round 3 — Task 25 report

## Task

| Field | Value |
|-------|-------|
| **id** | 25 |
| **title** | Retype CDSAudioVideoPlayer videoTrackManager byte[72] to nested CDSVideoPlayer (0x48) |
| **one_liner** | `CDSAudioVideoPlayer.videoTrackManager` @ `+0x08` is embedded `CDSVideoPlayer` (72 B); movie path via `CMovieView::StartPlayback`. |
| **acceptance** | Ghidra struct field typed `CDSVideoPlayer`; ctor/dtor/`SetupTrack`/`StartPlayback` decompile with named embed; docs updated |
| **structs** | `CDSAudioVideoPlayer`, `CDSVideoPlayer`, `CMovieView` |
| **addresses** | `0x0043bca0`, `0x0043bba0`, `0x0043bc10`, `0x00422d50` |

## Status

**DONE**

## Evidence

| Claim | func@addr | Notes |
|-------|-----------|-------|
| Parent size **0x50** | `CDSAudioVideoPlayer_Constructor@0x0043bca0`, `CMovieView::StartPlayback@0x00422d50` | `OperatorNewWithBadAlloc(0x50)` |
| Embed init at `+0x08` | `CDSAudioVideoPlayer_Constructor@0x0043bca0` | `CDSObject::ConstructTrackManager(&this->videoTrackManager, param_2)` |
| Embed teardown (no `_free` on subobject) | `CDSAudioVideoPlayer_dtor@0x0043bc10` | `CDSVideoPlayer::CDSVideoPlayer_TM_Destructor` on embed base (`&(videoTrackManager).pVftable_IDSChained` ≡ `this+8`) |
| `SetupTrack` wires audio ↔ video | `CDSAudioVideoPlayer_SetupTrack@0x0043bba0` | `this_00 = &videoTrackManager`; `AddTrackSource`; `(videoTrackManager).bPaused = 1`; `pAudioPlayer->pTrackSync = this_00` |
| `bPaused` parent alias | `CDSAudioVideoPlayer_SetupTrack@0x0043bba0` | Embed `+0x35` → parent `+0x3d` (`0x08 + 0x35`) |
| Movie consumer | `CMovieView::StartPlayback@0x00422d50` | Alloc `0x50` → ctor → `m_pPlayer`; `SetupTrack`; `ODSImage__SetImage(..., pPlayer->videoTrackManager.pRenderTarget)` |
| Shared TM layout **0x48** | `ConstructTrackManager@0x00439c70` | Same helper as 12 other embedded parents + heap factory `CreateTrackManagerHeap@0x00439f50` |
| Heap TM path (contrast) | `CDSVideoPlayer_ScalarDeletingDtor@0x00439fc0` | `TM_Destructor` + `_free` when standalone `OperatorNew(0x48)` |

### Ghidra struct (verified)

```
get_struct_layout CDSAudioVideoPlayer → Size: 80
  +0x00 vftable_IDSEventHandler
  +0x04 pAudioPlayer (CDSAudioPlayer *)
  +0x08 videoTrackManager (CDSVideoPlayer, 72 B)

get_struct_layout CDSVideoPlayer → Size: 72
  +0x00 pVftable_IDSChained … +0x44 nFrameDelayOverrideMs
```

### Decompiler samples (post-retype)

- **Constructor:** `ConstructTrackManager(&this->videoTrackManager, …)`; heap `CDSAudioPlayer` after embed init.
- **SetupTrack:** `(this->videoTrackManager).bPaused = 1`; `pCVar1->pTrackSync = this_00` with `this_00 = &videoTrackManager`.
- **Dtor:** `CDSVideoPlayer_TM_Destructor` on embedded subobject; audio player released separately.
- **StartPlayback:** `this->pPlayer = this_00`; `(this->pPlayer->videoTrackManager).pRenderTarget` passed to `ODSImage__SetImage`.

## Ghidra deltas

- `modify_struct_field` `CDSAudioVideoPlayer.videoTrackManager` → `CDSVideoPlayer` (was legacy `byte[72]` / stub).
- `set_function_prototype` on `CDSAudioVideoPlayer_Constructor@0x0043bca0`, `CDSAudioVideoPlayer_dtor@0x0043bc10`, `CDSAudioVideoPlayer_SetupTrack@0x0043bba0`, `CDSAudioVideoPlayer_Stop@0x0043bc00` → `CDSAudioVideoPlayer *` / `void` as appropriate.
- `set_decompiler_comment` @ `0x0043bca0`, `0x0043bba0`, `0x00422d50` (R3 todo 25).
- `save_program bulanci.exe` — yes.

## Struct doc updates

- [CDSAudioVideoPlayer.md](./CDSAudioVideoPlayer.md) — layout, embed vs heap table, Ghidra apply block (agent todo 25 R3).
- [CDSVideoPlayer.md](./CDSVideoPlayer.md) — embedded-in-AV-player cross-ref (unchanged size proof).
- Prior embedded-vs-heap narrative: [round3_task_09_report.md](./round3_task_09_report.md).

## Remaining UNK

- Runtime consumer of heap `CDSVideoPlayer` via class id `0x31` (`CDSVideoPlayer_StaticClassRegister@0x0047d750`; DATA xref only).
- `CDSAudioVideoPlayer_Stop@0x0043bc00` may still route through `_Globals::` wrapper naming despite typed `this->pAudioPlayer`.
