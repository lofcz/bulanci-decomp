# CMina

## Status

**PARTIAL** — heap size `0x118` verified; embedded **`CAnim`** `0x00–0xEF` fully offset-mapped (round 3 task 47). Tail `0x108–0x114` proven. Ghidra: `CAnim animBase` @ 0, `CDSUpdatedItem` @ `0xf0`.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CMina) == 0x118` | `0x004212b0` | `CWeapon::Fire` → `OperatorNewWithBadAlloc(0x118)` → `CMina::CMina_Ctor` |
| Same allocation elsewhere | `0x004212b0` area / `bulanci.ghidra.exe.c` | Additional mine spawns use `OperatorNewWithBadAlloc(0x118)` |
| Frees same object | `0x0041c530` | `CMina_ScalarDeletingDtor` → `_free(this)` when `param_1 & 1` |
| Last used byte at `+0x114` | `0x0041cb70` | ctor writes `bArmed = 0`; object padded to `0x118` |

## Layout (CMina-owned tail)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0xF0 | 0x18 | `CDSUpdatedItem` | `updatedItem` | `CDSUpdatedItem_ctor(this+0xf0)` @ `CMina_Ctor`; dtor @ `CMina_dtor@0x0041c010` |
| 0x108 | 4 | `CBulanek *` | `pOwnerBulanek` | `CMina_Ctor@0x0041cb70`; read `param_1[0x42]` in `_Globals__ExplodeMine@0x0041e070` |
| 0x10C | 4 | `void *` | `pAux` | zeroed ctor; `FUN_0041b4a0` @ `CMina_dtor`; `FUN_0041c0d0` stores danger-zone node |
| 0x110 | 4 | `int` | `nDeployOrOwnerCtx` | `InitMine@0x0041cce0`; `*(this+0x110)=*(param_2+0x84)` in `CMina_Ctor` |
| 0x114 | 1 | `byte` | `bArmed` | ctor; `(char)param_1[0x45]` in `_Globals__ExplodeMine` |
| 0x115 | 3 | — | *(pad)* | allocation size `0x118` − last field end |

## Embedded `CAnim` (`animBase` @ `+0x00`, size `0xF0`)

Full byte map matches Ghidra type **`CAnim`** (`get_struct_layout` → 240 B). Global field proofs: [CAnim.md](./CAnim.md). **CMina** column: code that uses `this` as `CMina *` and touches the subobject (or CMina-specific vtables).

| Offset | Size | Type | Name | Evidence (func@addr) | CMina |
|--------|------|------|------|----------------------|-------|
| 0x00 | 4 | `pointer` | `vftable_primary` | `CAnim_SubobjectCtor@0x00419870` → `g_pCAnim_*`; overwritten | **`g_pCMina_vftable_primary`** @ `CMina_Ctor@0x0041cb70`, `InitMine@0x0041cce0`, `CMina_dtor@0x0041c010`, `CMina_DefaultCtor@0x0041a990` |
| 0x04 | 4 | `pointer` | `vftable_IDSChained` | same | **`g_pCMina_vftable_IDSChained`** (same ctors/dtor) |
| 0x08 | 4 | `uint` | `dwChainField_08` | `CDSChained` layout mirror | *(inherits chain header; no CMina-only write)* |
| 0x0C | 4 | `uint` | `dwChainField_0c` | `CDSChained` layout mirror | **read** `!= 0` gate @ `FUN_00419fd0@0x00419fd0` |
| 0x10 | 4 | `pointer` | `vftable_IDSReferenced` | `CAnim_SubobjectCtor@0x00419870` | vtable write (same ctors) |
| 0x14 | 2 | `uint16` | `wViewFlags` | `CBulAnim` / views: `\|= 0x200` pattern | **`\|= 0x200`** @ `CMina_Ctor`, `InitMine`; `FUN_0041b420` when `param_4 != 0` |
| 0x16 | 2 | `uint16` | `wPad_16` | `CDSChained` mirror | — |
| 0x18 | 4 | `pointer` | `vftable_IDSEventHandler` | `CAnim_SubobjectCtor` | vtable write (CMina ctors) |
| 0x1C | 4 | `uint` | `dwField_1c` | `CDSChained` mirror | — |
| 0x20 | 4 | `int` | `nOrigin_x` | `CAnim_ctor@0x00419940`; `CDSView_SetRect@0x0042c480` | **weapon:** owner view + team offset @ `CMina_Ctor`; **deploy:** `InitMine`; **explode:** `param+8` @ `_Globals__ExplodeMine@0x0041e070`; **danger zone:** `FUN_0041c0d0` (−10) |
| 0x24 | 4 | `int` | `nOrigin_y` | same | same sources |
| 0x28 | 4 | `int` | `nSpatial_bucket_x` | spatial / danger-zone aux | `CMina_RegisterDangerZone@0x0041c0d0` (this=`updatedItem`) |
| 0x2C | 4 | `int` | `nSpatial_bucket_y` | spatial bucket summand | `CGameView_GetSpatialBucketKey@0x00416f50` |
| 0x30 | 4 | `int` | `nBounds_left` | `CDSView_SetRect@0x0042c480` | — |
| 0x34 | 4 | `int` | `nBounds_top` | same | — |
| 0x38 | 4 | `int` | `nBounds_right` | same | — |
| 0x3C | 4 | `int` | `nBounds_bottom` | same | — |
| 0x40 | 4 | `uint` | `dwView_pad_40` | *(pad)* | `CMina_RegisterDangerZone` (updatedItem−0xd0) |
| 0x44 | 4 | `uint` | `view_flags` | `CDSView_SetRect` (`& 0x40`) | — |
| 0x48 | 12 | `byte[12]` | `pChain_pad_48` | before `child_chain` | — |
| 0x54 | 4 | `pointer` | `child_chain` | `CDSChained_GetFirstChildView(this+0x54)` | — |
| 0x58 | 16 | `byte[16]` | `pHeader_tail_58` | view header tail | — |
| 0x68 | 1 | `byte` | `bView_state_68` | `FUN_00416590@0x00416590` | via `CAnim_SubobjectCtor` → `FUN_00419070` |
| 0x69 | 1 | `byte` | `bView_state_69` | same | same |
| 0x6A | 1 | `byte` | `bView_state_6a` | same | same |
| 0x6B | 5 | `byte[5]` | `pPad_6b` | — | — |
| 0x70 | 1 | `byte` | `bGaming_slot_id` | `FUN_00416590` (`0xff`) | — |
| 0x71 | 3 | `byte[3]` | `pPad_71` | — | — |
| 0x74 | 4 | `int` | `nField_74` | `FUN_00419070@0x00419070` `=0` | via subobject ctor |
| 0x78 | 4 | `int` | `nField_78` | same | same |
| 0x7C | 4 | `int` | `nField_7c` | same | same |
| 0x80 | 4 | `int` | `nField_80` | same | same |
| 0x84 | 4 | `CGaming *` | `gaming_host` | `FUN_00416590` `=0`; `CGameView_Update@0x00419010` | **read** byte @ `CGaming+0xd8` (`game+0xd4`) @ `CMina_UpdateTraceAreas@0x00419fd0` |
| 0x88 | 4 | `pointer` | `vftable_IDSUpdated` | `CAnim_SubobjectCtor` | **CMina vtable** @ `CMina_Ctor` / `InitMine` / dtor |
| 0x8C | 4 | `pointer` | `vftable_IDSAnim` | same | **CMina vtable** (same) |
| 0x90 | 8 | `byte[8]` | `pPad_90` | between IDSAnim and anim_sub | dtor thunk @ `+0x90` (`CMina_ScalarDeletingDtor_thunk_Sub88`) |
| 0x98 | 4 | `pointer` | `vftable_anim_sub` | `AnimInner_Init@0x00439270`; `TM_*` | **CMina vtable**; **`TM_BindSequence` / `TM_SetTrack`** @ ctors; **`TM_RenderFrame`** @ `CMina_RenderAnimFrame@0x00416790` (`&animBase+0x98`) |
| 0x9C | 4 | `int` | `nAnim_inner_field_9c` | `CAnim_SubobjectCtor` `=0` | via subobject ctor |
| 0xA0 | 4 | `int` | `nAnim_inner_field_a0` | same | same |
| 0xA4 | 4 | `int` | `nAnim_inner_field_a4` | same | same |
| 0xA8 | 72 | `byte[72]` | `pTrack_manager` | `ConstructTrackManager@0x00439c70`; `CAnim_dtor` `TM_Destructor` | **`CAnim::FUN_0041b190`** from `CMina_OnSchedulerHook`; inner layout not split |

### CMina vtable cluster (embedded faces)

| Vtable symbol | RVA | Face | Install offset |
|---------------|-----|------|----------------|
| `g_pCMina_vftable_primary` | `0x0048257c` | primary (31) | +0x00 |
| `g_pCMina_vftable_IDSChained` | `0x004824dc` | IDSChained (5) | +0x04 |
| *(shared anim faces)* | `0x00482508` etc. | IDSAnim / anim_sub | +0x88, +0x8c, +0x98 |
| `g_pCMina_vftable_IDSUpdated` | — | CDSUpdatedItem | +0xF0 (`updatedItem`), not inside `0xEF` |

See `master_vtable_catalog.csv` rows `CMina,*`.

## Deploy paths (`+0x108` / `+0x110`)

Same offsets, different semantics by entry point (decompile 2026-05-30):

| Field | `CMina_Ctor@0x0041cb70` (weapon via `CWeapon::Fire`) | `InitMine@0x0041cce0` (map deploy) |
|-------|------------------------------------------------------|-------------------------------------|
| `pOwnerBulanek` | `param_2` (`CBulanek *`) | `NULL` |
| `nDeployOrOwnerCtx` | `param_2->pGamingHost` | `deployContext` (`int`) |

## Ghidra apply

Slice **14** (2026-05-30): `get_struct_layout CMina` → **280 B (`0x118`)** — `CAnim animBase` @ 0, `CDSUpdatedItem updatedItem` @ `0xf0`, tail `pOwnerBulanek` / `pAux` / `nDeployOrOwnerCtx` / `bArmed`. Verified `CWeapon::Fire@0x004212b0` → `OperatorNewWithBadAlloc(0x118)` + `CMina_Ctor`. `save_program bulanci.exe`.

**Agent todo 15** (2026-05-30): `CAnim.gaming_host` @ `+0x84` → **`CGaming *`** (`pGaming_host` in Ghidra); `CBitmap.gaming_host` matched for `CGameView_Update@0x00419010`. Renamed `FUN_00419fd0` → **`CMina_UpdateTraceAreas`**; prototype `void __fastcall CMina_UpdateTraceAreas(CMina *this)`. Decompile: `(animBase.pGaming_host)->game` byte @ `+0xd4` (linear `CGaming+0xd8`). `save_program bulanci.exe`.

Agent todo **20** r2 (worker 20, 2026-05-30): **DONE** — `CMina.animBase` = **`CAnim`** (240 B). Mapped header band `+0x28..+0x67`: `nSpatial_bucket_x/y`, bounds, `dwView_pad_40`, `view_flags`, `pChain_pad_48`, `child_chain`, `pHeader_tail_58`, `pLinked_bulanek`. `FUN_0041c0d0` → **`CMina_RegisterDangerZone`**. `save_program bulanci.exe`.

## UNK

- `dwChainField_0c` meaning when non-zero (trace-area loop gate only).
- `dwView_pad_40`, `pChain_pad_48`, `pHeader_tail_58` — only `CMina_RegisterDangerZone` / generic CDSView paths.
- `pTrack_manager` (+0xa8..+0xef) interior — opaque `ConstructTrackManager` blob.
- `pAux` — danger-zone node via `CMina_RegisterDangerZone@0x0041c0d0`; release `FUN_0041b4a0` @ dtor (type TBD).
- `nDeployOrOwnerCtx` — weapon path stores owner `pGamingHost`; deploy path stores opaque deploy `int` (not a pointer).

## Follow-up

- Split `pTrack_manager` using `CDSObject::ConstructTrackManager@0x00439c70` when batch scopes `CDSObject` scheduler sub-struct.
