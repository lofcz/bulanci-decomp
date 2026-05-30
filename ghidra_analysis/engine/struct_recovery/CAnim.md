# CAnim

## Status

**PARTIAL** — object size `0xf0` and Ghidra type **`CAnim` (240 B)** applied. Layout table below matches `get_struct_layout CAnim` (round 3 task 47 sync; task 29: `pOds_owner` / `pOds_drawable` @ `+0x90..+0x97`). Vtable anchors, ctor coordinates, CDSView `bounds` / `view_flags` / `child_chain`, view-state bytes, `ODSImage` mixin tail, anim-inner / track-manager bases are instruction-proven. CMina embed @ `0x00`: [CMina.md](./CMina.md).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CAnim) == 0xf0` | `0x0041cfb0` | `CBulanci_AllocAnimFromSprite`: `OperatorNewWithBadAlloc(0xf0)` then `CAnim_ctor` |
| `sizeof(CAnim) == 0xf0` | `0x0041d6e0` | `CLevelScriptOpExt_CreateAnim`: `OperatorNewWithBadAlloc(0xf0)` then `CAnim_ctor` |
| `sizeof(CAnim) == 0xf0` | `0x00411010` | `CScoreCtor` trophy: `OperatorNewWithBadAlloc(0xf0)` then `CAnim_ctor(…,0x78,0x32,…)` |
| `sizeof(CAnim) == 0xf0` | `0x00411c0e` region | `CExitDlg_ctor` (see `main_menu.md` / asset naming): two `CAnim` panels |
| Embedded in `CMina` | `CMina_Ctor@0x0041cb70` | `CAnim_SubobjectCtor(this)`; parent size `0x118` |

## Layout table (`0x00`–`0xEF`)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `pointer` | `vftable_primary` | `CAnim_SubobjectCtor@0x00419870` → `g_pCAnim_vftable_primary`; `CAnim_ctor@0x00419940` |
| `+0x04` | 4 | `pointer` | `vftable_IDSChained` | `CAnim_SubobjectCtor@0x00419870` |
| `+0x08` | 4 | `uint` | `dwChainField_08` | `CDSChained` header mirror (`struct CDSChained`) |
| `+0x0C` | 4 | `uint` | `bTraceAreasActive` | `CDSChained` mirror; **read** `CMina_UpdateTraceAreas@0x00419fd0` when embedded on `CMina` |
| `+0x10` | 4 | `pointer` | `vftable_IDSReferenced` | `CAnim_SubobjectCtor` / `CAnim_ctor@0x00419940` |
| `+0x14` | 2 | `uint16` | `wViewFlags` | `\|= 0x200` on `CBulAnim` / `CMina_Ctor@0x0041cb70` (`ushort` @ +0x14) |
| `+0x16` | 2 | `uint16` | `wPad_16` | `CDSChained` mirror |
| `+0x18` | 4 | `pointer` | `vftable_IDSEventHandler` | `CAnim_SubobjectCtor` / `CAnim_ctor` |
| `+0x1C` | 4 | `uint` | `dwField_1c` | `CDSChained` mirror |
| `+0x20` | 4 | `int` | `nOrigin_x` | `CAnim_ctor@0x00419940` `=param_1`; `CMina_Ctor` / `InitMine` |
| `+0x24` | 4 | `int` | `nOrigin_y` | `CAnim_ctor@0x00419940` `=param_2`; `CMina_Ctor` / `InitMine` |
| `+0x28` | 4 | `int` | `nSpatial_bucket_x` | **Extent/aux X:** `CMina_RegisterDangerZone@0x0041c0d0` (±10 with `nOrigin`); `CTeleportPoint_Ctor` writes from partner bounds; no dedicated `CAnim_ctor` write |
| `+0x2C` | 4 | `int` | `nSpatial_bucket_y` | **Extent/aux Y:** `CGameView_GetSpatialBucketKey@0x00416f50` `+ dwView_aux_6c`; same danger-zone / teleport consumers as `+0x28` |
| `+0x30` | 4 | `int` | `nBounds_left` | `CDSView_SetRect@0x0042c480` |
| `+0x34` | 4 | `int` | `nBounds_top` | same |
| `+0x38` | 4 | `int` | `nBounds_right` | same |
| `+0x3C` | 4 | `int` | `nBounds_bottom` | same |
| `+0x40` | 4 | `uint` | `dwChainRoot` | `CDSChained_ResetChainCounters@0x0042beb0` band mirror (was `dwView_pad_40`); **R5 w36:** not a pad — same offset as `CDSChained.dwChainRoot` |
| `+0x44` | 4 | `uint` | `view_flags` | `CDSView_SetRect@0x0042c480` `(field & 0x40)` |
| `+0x48` | 12 | `byte[12]` | `pChain_pad_48` | before `child_chain` |
| `+0x54` | 4 | `pointer` | `child_chain` | `CDSChained_GetFirstChildView(this+0x54)` @ `CDSView_SetRect` |
| `+0x58` | 12 | `byte[12]` | `pHeader_tail_58` | gap before `pLinked_bulanek` (Ghidra field @ `+0x58`) |
| `+0x64` | 4 | `CBulanek *` | `pOverlap_entity` | Same slot as `CDSChained::dwField_64` (hex offset). **Writers:** `CDSChained_ctor` zero @ `0x00403366`; `CDSChained_InitWithRect` @ `0x0040b611`. **Not** `CBulanek_UpdateStateFromParams` (that `MOV [ECX+0x64]` is CBulanek `pHeader_tail_58+0xc` — R4 todo 22). **Consumer:** `CTeleportPoint_OnEvent` `MOV ECX,[ESI+0x64]` @ `0x0041feff`. Owner/host: `pGaming_host` @ `+0x84` via `AddEntity` @ `0x0041a3a6` only |
| `+0x68` | 1 | `byte` | `bView_state_68` | `FUN_00416590@0x00416590` (via `FUN_00419070` in ctor chain) |
| `+0x69` | 1 | `byte` | `bView_state_69` | same |
| `+0x6A` | 1 | `byte` | `bView_state_6a` | same |
| `+0x6B` | 1 | `byte` | `bView_flag_6b` | `CRadio_OnMouseDown@0x00402ff0`; Ghidra renamed (task 48) |
| `+0x6C` | 4 | `uint` | `dwView_aux_6c` | `CRadio_Render@0x00403ba3` (task 48) |
| `+0x70` | 1 | `byte` | `bGaming_slot_id` | `FUN_00416590@0x00416590` stores `0xff` (follows `dwView_aux_6c`; no separate `+0x6d` pad in Ghidra layout) |
| `+0x71` | 3 | `byte[3]` | `pPad_71` | — |
| `+0x74` | 4 | `int` | `nDest_x` | `CTeleportPoint_Ctor@0x0041c9a0`; `FUN_00419070@0x00419070` `=0` |
| `+0x78` | 4 | `int` | `nDest_y` | same |
| `+0x7C` | 4 | `int` | `nSrc_x` | same |
| `+0x80` | 4 | `int` | `nSrc_y` | same |
| `+0x84` | 4 | `CGaming *` | `gaming_host` | `FUN_00416590` `=0`; `CGameView_Update@0x00419010`; `CMina_UpdateTraceAreas@0x00419fd0` |
| `+0x88` | 4 | `pointer` | `vftable_IDSUpdated` | `CAnim_SubobjectCtor` / `CAnim_ctor` (subclass e.g. **CMina** replaces) |
| `+0x8C` | 4 | `pointer` | `vftable_IDSAnim` | same — `ODSImage::pVf_odsimage` (embedded `ODSImage` @ `+0x88`) |
| `+0x90` | 4 | `CBulanci *` | `pOds_owner` | `ODSImage::SetOwner@0x00419070` via `FUN_00419070` (`(ODSImage *)(this+0x88)`); cleared `AnimInner_Teardown@0x004392e0` / `CDSView_DtorTeardownAnimInner@0x00417100` (`*(base+0x8)`) |
| `+0x94` | 4 | `CDSImage *` | `pOds_drawable` | `ODSImage::pDrawable`; `TM_SetCurrentSequence@0x00439100` / `ODSImage__SetImage` on `this+0x88` (`[base+0xc]`); CDSAnim analog `+0x74` |
| `+0x98` | 4 | `pointer` | `vftable_anim_sub` | `AnimInner_Init@0x00439270`; `TM_RenderFrame` @ `CAnim_RenderAnimFrame@0x004164f0` |
| `+0x9C` | 4 | `int` | `nAnim_inner_field_9c` | `CAnim_SubobjectCtor@0x00419870` `=0` |
| `+0xA0` | 4 | `int` | `nAnim_inner_field_a0` | same |
| `+0xA4` | 4 | `int` | `nFx_coord` | `TriggerTeleportFX@0x0041fca0`; `CAnim_SubobjectCtor@0x00419870` `=0` |
| `+0xA8` | 72 | `CDSVideoPlayer` | `track_manager` | `ConstructTrackManager@0x00439c70`; `CAnim_dtor@0x004171b0` `CDSVideoPlayer_TM_Destructor`; scheduler embed `CDSUpdatedItem` @ `track_manager+0x4` (`ConstructTrackManager@0x00439cab`) |

## Ghidra apply

**Slice 03 (2026-05-30):** Deleted truncated `CAnim` (188 B) and recreated **`CAnim` (240 B)** with `wViewFlags@+0x14`, `gaming_host@+0x84`, `pOds_owner` / `pOds_drawable` @ `+0x90..+0x97`, `pTrack_manager` @ `+0xa8` (72 B). **Slice 21:** `overlap_entity` (was `pLinked_bulanek`) @ `+0x64`; `nDest_x/y`, `nSrc_x/y` @ `+0x74..+0x80`; `nFx_coord` @ `+0xa4`; `CTeleportPoint.canim_base` embeds `CAnim`. `save_program bulanci.exe`.

**Agent todo 22 (2026-05-30):** Renamed `pLinked_bulanek` → `pOverlap_entity` (`CBulanek *`); documented ctor-chain zero @ `CDSChained_ctor+0x64` and `CBulanek_AddEntity` owner @ `+0x84` (`0x0041a3a6`). R3: [round3_task_22_report.md](./round3_task_22_report.md). **R4 (2026-05-30):** Fixed `-BAD-` type; corrected `UpdateStateFromParams` false writer link; [round4_task_22_report.md](./round4_task_22_report.md). `save_program bulanci.exe`.

**Agent todo 08 (2026-05-30):** `pTrack_manager` `undefined1[72]` → **`CDSVideoPlayer track_manager` @ `+0xa8`** — nested `scheduler` (`CDSUpdatedItem` 24 B @ `+0xac`) and track-vector tail per [CDSVideoPlayer.md](./CDSVideoPlayer.md). `ConstructTrackManager@0x00439c70` decompile shows `CDSUpdatedItem_ctor` on `this->scheduler` and host `pVftable_IDSUpdated` store @ `0x00439cb6`. `save_program bulanci.exe`.

**Agent todo 15 (2026-05-30):** `gaming_host` retyped **`CGaming *`** (Ghidra name `pGaming_host`); `CMina_UpdateTraceAreas@0x00419fd0` renamed + prototyped. `CBitmap.gaming_host` aligned for view update path.

**Agent todo 5 (2026-05-30):** Named CDSView header band `+0x28..+0x67`: `pHeader_tail_58` @ `+0x58` (12 B gap); `view_flags` @ `+0x44`; `pOds_owner` @ `+0x90`. `get_struct_layout CAnim` → 240 B unchanged. `save_program bulanci.exe`.

**Agent todo 20 r2 (2026-05-30):** Split `pPad_28` → `nSpatial_bucket_x` / `nSpatial_bucket_y`; `dwField_40` → `dwView_pad_40`; `dwView_flags` → `view_flags`; `pOds_owner` → `CBulanci *`. Renamed `FUN_0041c0d0` → `CMina_RegisterDangerZone`. Comments @ `0x0041c0d0`, `0x0041cb70`, `0x00416f50`, `0x00419fd0`. `save_program bulanci.exe`.

**Agent todo 5 r2 (2026-05-30):** Verified `track_manager` (`CDSVideoPlayer` 72 B @ `+0xa8`) nests `trackVector` (`CDSTrackVector` 16 B @ host `+0xc4` = `track_manager+0x1c`). `CAnim_SubobjectCtor@0x00419870` → `ConstructTrackManager(&track_manager)`; decompile shows `(trackVector).pTracks` / capacity init. Prototypes: `ConstructTrackManager@0x00439c70`, `TM_LookupTrackIndex@0x00439730`, `TM_InsertTrackAt@0x00439a70`. `pOds_owner` retyped `CBulanci *`. `save_program bulanci.exe`.

**Agent todo 4 r3 (2026-05-30):** `set_function_this_type` → **`CAnim *`** @ `CAnim_ctor@0x00419940`, `CAnim_RenderAnimFrame@0x004164f0`; `force_decompile` shows `this->nOrigin_x/y`, `&this->vftable_anim_sub` (`+0x98`). Shared `CGameView_*` sites @ `0x00416f50`, `0x0041acf0`, `0x00416590` plated **`CGameView *`** (see [CGameView.md](./CGameView.md)). `save_program bulanci.exe`.

**R5 worker 36 (2026-05-30):** Ghidra `dwView_pad_40` → **`dwChainRoot` @ `+0x40`**; `pOds_owner` → **`CBulanci *`**; decompiler comments @ `GetSpatialBucketKey`, `CMina_RegisterDangerZone`, `CMina_UpdateTraceAreas`. [round5_worker_36_report.md](./round5_worker_36_report.md). `save_program bulanci.exe`.

## UNK

- `dwChainField_08` — `CDSChained_ctor` zero only; no consumer.
- ~~`dwChainField_0c`~~ — **renamed `bTraceAreasActive` (R5-44):** when non-zero on `CMina`, enables gaming-host entity-slot trace sweep ([CMina.md](./CMina.md)); no anim-specific writer found.
- `pChain_pad_48` @ `+0x48` — Ghidra 12 B blob; overlaps `CDSChained` `wChainInit44` / chain-flag band (`+0x44..+0x4f`); script bridge sites use bogus `this[5].pChain_pad_48` decompiler indexing (real slot read `view+0x70` in `CLevelScript_FireOnBitmapEvt_FromView`).
- `nSpatial_bucket_*` name vs role — used as extent/partner-bounds aux and sort key, not only hash buckets; rename deferred.
- `CDSTrackVector` Ghidra display names may remain `dwTracks*` despite logical `cTracks*` ([CDSTrackVector.md](./CDSTrackVector.md)).
- RTTI / COL records (`0x004a20b0` etc.) — see `anim_runtime.md`.

## Notes

- **Not** `CDSAnim` subclass: separate `0xf0` widget-framed player; `IDSAnim` @ `+0x8c`, inner @ `+0x98` (vs `CDSAnim` inner @ `+0x78`, size `0xd0`).
- **`ODSImage` @ `+0x88`:** vftables `+0x88/+0x8c`, owner `+0x90`, drawable `+0x94` — same mixin as `CDSAnim+0x68..+0x77`, shifted `+0x20` (task 29).
- `CAnim_RenderAnimFrame@0x004164f0` → `TM_RenderFrame(&vftable_anim_sub,…)`.
- `CreateAnim@0x0041d6e0` binds sequences with `TM_BindSequence` / `TM_SetTrack` on `+0x98`.
