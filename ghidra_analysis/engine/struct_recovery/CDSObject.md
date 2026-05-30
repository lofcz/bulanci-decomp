# CDSObject

## Status

**PARTIAL** — two heap sizes: **`0x48`** track-manager (`ConstructTrackManager` only) and **`0x60`** image-bearing (`CDSObject_CtorWithImage` + embedded `CDSImage` at `+0x04`). Ghidra struct **96 B** (superset). Track-manager fields through `+0x44` from `ConstructTrackManager@0x00439c70`; image MI tail `+0x48..+0x5c` from `CDSObject_CtorWithImage@0x0042563a`.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Track-manager `sizeof == 0x48` | `0x00439f9a` | `_Globals::CreateObject`: `OperatorNewWithBadAlloc(0x48)` → `ConstructTrackManager` |
| Image object `sizeof == 0x60` | `0x00432c0e` | `CDSFlxFile::CreateBoundClone`: `OperatorNewWithBadAlloc(0x60)` → `CDSObject_CtorWithImage` |
| Same `0x60` alloc | `0x00439af3` | `CDSDsmFile::InitializeChildObject`: `OperatorNewWithBadAlloc(0x60)` → `CDSObject_CtorWithImage` |
| Dtor uses through `+0x58` | `0x004256d8` | `CDSObject_dtor`: `FUN_00434250(this+0x58)`; `CDSImage_dtor(this+4)` |

## Layout table (track-manager path)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `void *` | `pVftable_IDSReferenced` | `ConstructTrackManager@0x00439c86` `= g_pCDSObject_vftable_IDSReferenced` |
| `+0x04` | 24 | `CDSUpdatedItem` | `scheduler` | `CDSUpdatedItem_ctor(this+4)` `@0x00439c92`; facet `0x487770` @ `+0x04` |
| `+0x1c` | 4 | `void *` | `pTracks` | `ConstructTrackManager@0x00439cad` `= 0` |
| `+0x20` | 4 | `uint` | `dwTracksAllocated` | `ConstructTrackManager@0x00439cb3` `= 0` |
| `+0x24` | 4 | `uint` | `dwTracks` | `ConstructTrackManager@0x00439cb9` `= 0` |
| `+0x28` | 4 | `uint` | `dwTrackCapacity` | `ConstructTrackManager@0x00439cbf` `= 8` |
| `+0x2c` | 4 | `int` | `nCurrentTrackIdx` | `ConstructTrackManager@0x00439cd4` `= -1` |
| `+0x30` | 4 | `void *` | `pRenderTarget` | `ConstructTrackManager@0x00439cc8` `= 0`; `TM_AdvanceFrame@0x004399b0`; `AnimInner_Init` via `inner+0x40` on hosts |
| `+0x34` | 1 | `byte` | `bPlayFlags` | `TM_Play@0x00439940` write; `CDSVideoPlayer_AdvanceFrameAndPauseIfDone@0x00439b90` read (via `this+4` base) |
| `+0x35` | 1 | `byte` | `bPaused` | `ConstructTrackManager@0x00439cda` `= 0`; `TM_Play` / `TM_PauseAndStampClock@0x00439850` |
| `+0x38` | 4 | `uint` | `dwCurrentFrameIdx` | `TM_AdvanceFrame@0x004399b0` increment / wrap |
| `+0x3c` | 4 | `void *` | `pNotifyCookie` | `ConstructTrackManager@0x00439ce0` `= 0` |
| `+0x40` | 4 | `uint` | `dwSchedulerCookie` | `ConstructTrackManager@0x00439ce6` `= 0xffffffff`; `CGaming_PauseClockIfActive@0x00439990` |
| `+0x44` | 4 | `int` | `nFrameDelayOverrideMs` | `ConstructTrackManager@0x00439cec` `= 0xffffffff`; `TM_AdvanceFrame@0x004399b0` |

## Variant overlay — image path (`CDSObject_CtorWithImage`)

`CDSImage_ctor(&this->scheduler, …)` treats **`this+4`** as a full `CDSImage` base (`CDSImage.md`). The outer `CDSObject::pVftable` is replaced with `CDSImage::vftable`; `scheduler`’s first dword becomes the image primary vtable (`CDSObject_CtorWithImage@0x0042563a`).

| `CDSObject` abs | Track-manager meaning | Image embed (`CDSImage` @ `this+4`) |
|-----------------|----------------------|-------------------------------------|
| `+0x04..+0x1b` | `CDSUpdatedItem` | `CDSImage` header / bitmap fields (facet overwritten) |
| `+0x34` | `bPlayFlags` (byte) | `m_copyHeight` (dword); `CDSImage__Allocate@0x00436f40` when `this=&scheduler` |
| `+0x38` | `dwCurrentFrame` | `m_paletteEntries` (dword); same allocate |
| `+0x48` | (padding on `0x48` alloc) | `nEmbeddedImage_field_44` — `CDSImage::field_44` (`CDSImage_ctor@0x00425460` `+0x44 = 8`) |
| `+0x4c` | — | `pEmbeddedImage_vf_primary` — `CDSImage` MI slot `+0x48`; `CDSObject_CtorWithImage@0x0042568a` stores `CDSImage::vftable`; `CDSImage_ReleaseRefcount@0x004322f0` dispatches `(**(code**)(*(this+0x4c)+4))()` |
| `+0x50` | 4 | `uint` | `dwImageField_50` | `CDSObject_CtorWithImage@0x00425666` `MOV [this+0x50], 1` (dword tag; MI `vf_IDSChained` at `+0x54`) |
| `+0x54` | — | `*pPad_54 = g_pCDSImage_vftable_IDSChained` |
| `+0x58` | — | `*(pPad_54+4) = CDSImage::vftable`; `FUN_00434250` in dtor |
| `+0x5c` | — | `dwImageField_5c = 0` (embed tail; standalone `CDSImage` uses `m_chain` here) |

Palette / plane helpers (`GetPaletteBuffer@0x004360d0`, `GetColorPlane@0x004360f0`) take a **`CDSImage*`** and use **`CDSImage+0x34`** (`m_paletteEntries`), which is **`CDSObject+0x38`** when embedded.

## Ghidra apply

```
delete_data_type CDSObject
create_struct CDSObject [ /* 96 B — see get_struct_layout after round3 task 24 */ ]
get_struct_layout CDSObject → Size: 96
```

Round 3 task 24: `bPlayFlags` @ `+0x34`, `dwCurrentFrame` @ `+0x38`, image tail `+0x50..+0x5c` (`image_vf_IDSChained` … `image_tail_5c`).

**Slice 00 (2026-05-30):** `modify_struct_field` `dwRenderTarget` → `pRenderTarget` (`void *` @ `+0x30`). `get_struct_layout` → **96** bytes unchanged.

**Agent todo 9 (2026-05-30):** `nImage_field_44` → `nEmbeddedImage_field_44` (`int` @ `+0x48`); `nImage_field_48` → `pEmbeddedImage_vf_primary` (`void *` @ `+0x4c`). Evidence: `CDSImage_ctor@0x00425460`, `CDSObject_CtorWithImage@0x00425620`, alloc sites `CDSFlxFile::CreateBoundClone@0x00432c0e`, `CDSDsmFile::InitializeChildObject@0x00439af3`. `save_program bulanci.exe`.

**Agent todo 9 round-2 (2026-05-30):** `CDSObject_schedulerOverlay_u` @ `+0x04` (56 B): `as_track_manager` (`CDSObject_trackOverlay56`) vs `as_embedded_image` (`CDSImage_embedAt04`). Decompiler comments @ `CDSObject_CtorWithImage@0x0042563a`, `ConstructTrackManager` scheduler init @ `0x00439c92`, `GetPaletteBuffer@0x004360d0`. `save_program bulanci.exe`.

**Agent todo 24 (2026-05-30):** Track-manager prefix renamed to match `CDSVideoPlayer` (`pTracks`, `dwTracksAllocated`, `dwTracks`, `dwTrackCapacity`, `dwCurrentFrameIdx`). `CDSAnim.track_manager` @ `+0x88` is embedded `CDSVideoPlayer` (72 B); removed duplicate `pCached_sequence`. `create_struct CDSTrackEntry` (8 B). `save_program bulanci.exe`.

**Agent todo 1 r3 (2026-05-30):** `dwImageField_50` `uint` @ `+0x50` (`MOV [this+0x50],1` @ `CDSObject_CtorWithImage@0x00425670`); `set_function_this_type` on `CDSObject_CtorWithImage` + `CDSImage_ctor@0x00425460`; decompile shows `nDefaultBppTag` on embed path → abs `+0x48` `nEmbeddedImage_field_44`. Comments @ `0x00425670`, `0x004254af`; `save_program bulanci.exe`.

## Follow-up

- Scheduler/`CDSImage` overlay, `bPlayFlags`, `+0x48..+0x5c` MI naming — see agent todo 1 r3 / `CDSImage.md` (R3 `agent_todos_50_r3` id **24** is `CDSAudioBankSample`, not `CDSObject`).

## UNK

- ~~`+0x50` naming~~ **done** (agent todo 1 r3): Ghidra `dwImageField_50` (`uint`); `pImage_vf_IDSChained` moved to `+0x54`.
- MI thunk wiring for `image_vf_streamHost` / `image_tail_5c` (defer to `CDSImage.md`, `bmp_decoder.md`).
