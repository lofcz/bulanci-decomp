# CDSVideoPlayer

## Status

`VERIFIED` (size `0x48`; Ghidra struct matches `ConstructTrackManager` / track-manager helpers)

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof == 0x48` (72 bytes) | `0x00439c70` | `ConstructTrackManager` initializes fields through `+0x44`; Ghidra `get_struct_layout` reports 72 |
| Embedded in `CDSAudioVideoPlayer` | `movie_cinema_views.md` | Track manager at parent `+0x08` (80-byte AV player) |
| Scalar delete path | `0x00439fc0` | `CDSVideoPlayer_ScalarDeletingDtor` → `_free(this)` when `freeFlag&1` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `void*` | `pVftable_IDSChained` | `ConstructTrackManager@0x00439c70` (`0x487788`) |
| `+0x04` | 24 | `CDSUpdatedItem` | `scheduler` | `CDSUpdatedItem_ctor@0x0042f060` on `this+4`; `Scheduler_RegisterEventSlot(this+4,...)` |
| `+0x1c` | 16 | `CDSTrackVector` | `trackVector` | `ConstructTrackManager@0x00439c70` (`trackVector` zeroed, `cTrackCapacity=8`); helpers take `&trackVector` / inner `this` |
| `+0x2c` | 4 | `int32` | `nCurrentTrackIdx` | `ConstructTrackManager@0x00439c70` (`param_1[0xb]=-1`); `TM_AdvanceFrame@0x00439940` |
| `+0x30` | 4 | `void*` | `pRenderTarget` | `ConstructTrackManager@0x00439c70`; `CDSVideoPlayer_TM_Destructor@0x00439d30` (`param_1[0xc]`) |
| `+0x34` | 1 | `byte` | `bPlayFlags` | `TM_Play@0x00439940` |
| `+0x35` | 1 | `byte` | `bPaused` | `ConstructTrackManager@0x00439c70`; `TM_Play` / `AdvanceFrameAndPauseIfDone@0x00439b90` |
| `+0x36` | 2 | — | `pad_0x36` | — |
| `+0x38` | 4 | `uint32` | `dwCurrentFrameIdx` | `TM_AdvanceFrame@0x00439940`; `AdvanceFrameAndPauseIfDone@0x00439b90` |
| `+0x3c` | 4 | `void*` | `pNotifyCookie` | `ConstructTrackManager@0x00439c70` (`param_1[0xf]=0`) |
| `+0x40` | 4 | `uint32` | `dwSchedulerCookie` | `ConstructTrackManager@0x00439c70` (`=0xffffffff`); `TM_PauseAndStampClock@0x00439850` stores `g_dwElapsedMs` when pausing |
| `+0x44` | 4 | `int32` | `nFrameDelayOverrideMs` | `ConstructTrackManager@0x00439c70` (`param_1[0x11]=0xffffffff`); `TM_AdvanceFrame@0x00439940` |

**Track vector subobject** — nested `CDSTrackVector trackVector` @ `+0x1c` (16 bytes). See [CDSTrackVector.md](./CDSTrackVector.md). `InsertOrFindTrack`, `TM_LookupTrackIndex@0x00439730`, `TM_InsertTrackAt@0x00439a70`, `TM_ClearTracks`, `CDSVideoPlayer_EnsureCapacity` use `CDSTrackVector*` (`&this->trackVector`).

**Track vector element** (heap, 8 bytes): sorted `{key, IDSStream*}` pairs — `InsertOrFindTrack@0x00439bd0`, `TM_InsertTrackAt@0x00439a70`.

## Allocation paths (same 0x48 layout)

| Path | Entry | Alloc | Init | Teardown |
|------|-------|-------|------|----------|
| **Embedded subobject** | Parent ctors (e.g. `CDSAudioVideoPlayer::Constructor@0x0043bd00`) | Inside parent blob | `ConstructTrackManager(base)` | `CDSVideoPlayer_TM_Destructor(base)` only |
| **Standalone heap** | `CDSVideoPlayer::CreateTrackManagerHeap@0x00439f50` | `OperatorNewWithBadAlloc(0x48)` @ `0x00439f9a` | `ConstructTrackManager(heap)` | `CDSVideoPlayer_ScalarDeletingDtor@0x00439fc0` → `TM_Destructor` + `_free` |

`ConstructTrackManager@0x00439c70` is shared by **12** embedded parents (`CAnim`, `CGunMouse`, `CSwitch`, `CDSAudioVideoPlayer`, …) and the heap factory.

**Static registration:** `CDSVideoPlayer_StaticClassRegister@0x0047d750` pushes factory `0x00439f50`, class id **`0x31`**, `HandleClassRegister@0x0042e910`. `get_xrefs_to(0x00439f50)` → DATA only (no in-game `CALL`).

Primary movie path uses **embedded** TM inside `CDSAudioVideoPlayer` (`CMovieView::StartPlayback@0x00422d50`), not the heap factory.

## Ghidra apply

```
get_struct_layout CDSVideoPlayer  → Size: 72 bytes; CDSUpdatedItem @ +4; CDSTrackVector trackVector @ +0x1c (agent todo 44, 2026-05-30)
get_struct_layout CDSTrackVector  → Size: 16 bytes; pTracks, cTracks* (Ghidra may still list dwTracks*)
```

**Prototypes:** `ConstructTrackManager@0x00439c70` → `CDSVideoPlayer * __fastcall ConstructTrackManager(CDSVideoPlayer *this, int schedulerParam)`; `TM_PauseAndStampClock@0x00439850` / `TM_AdvanceFrame@0x004399b0` / `TM_Play@0x00439940` → `CDSVideoPlayer *this` (agent todo 44); `TM_LookupTrackIndex@0x00439730` / `TM_InsertTrackAt@0x00439a70` → `CDSTrackVector *this` (Ghidra ECX may still decompile as `void*`).

**Prototype (todo 42):** `BeginCurrentTrackPlayback@0x00439b40` → `void __fastcall BeginCurrentTrackPlayback(CDSVideoPlayer *this)` — callers include `CBulanekCtor` (`this+0xa8`) and `CBulanek_ApplyAction` (`&videoTrackManager`).

**Callers:** **12** embedded parents + `CDSVideoPlayer_CreateTrackManagerHeap@0x00439f50` (heap `OperatorNew(0x48)`).

**Critical path:** `CBulanek` embeds `scheduler` @ `+0x88` and `videoTrackManager` (`CDSVideoPlayer`) @ `+0xA8` — Ghidra `CBulanek` fields applied slice 41.

## Follow-up (round 3 task 09)

- Documented heap `CreateTrackManagerHeap` vs embed subobject; renamed factory @ `0x00439f50`.

## UNK

- `TM_LookupTrackIndex@0x00439730` with `searchCount==-1` reads `cTracksAllocated` at inner `+4`; live path passes `cTracks` from inner `+8` via `InsertOrFindTrack`.
- Decompiler `this` on track-vector helpers may remain `void*` despite `CDSTrackVector *` prototypes (__thiscall ECX limitation).
- Runtime consumer of class id `0x31` heap factory (registration only in static init).
