# CShot

## Status

**PARTIAL** — `sizeof == 0xb0` (176 B) proven via heap alloc; Ghidra layout applied R5 worker 27 (`+0x40..+0x9F` gaming band + `CDSUpdatedItem` embed). R10 worker 10 re-verified **scheduler tick** (`CShot_SchedulerTick@0x0041b1d0`) and **collision tail** (`+0x20` rect trace, `+0xA4..+0xAC`) with live disasm + IDA correlation. Projectile behavior cross-ref: `combat_projectiles.md`.

**Note:** There is no separate `CBullet` class in the binary; “bullet” = **`CShot`** (`OperatorNew(0xB0)`, entity type **`0x0F`**).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CShot) == 0xb0` | `CGaming_SpawnBulletAndPlaySound@0x0041f230` | `OperatorNewWithBadAlloc(0xB0)` + `CShot_Ctor` |
| ctor | `0x0041edf0` | `CShot_Ctor` — `CDSChained_ctor` → `CDSUpdatedItem_ctor(&updatedItem)` → CShot vtables |
| Subobject ctor | `0x00417030` | `CShot_SubobjectCtor` (shell helper) |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `pointer` | `vftable_primary` | `CShot_Ctor@0x0041edf0` → `0x00481cbc` |
| `+0x04` | 4 | `pointer` | `vftable_IDSChained` | `0x00481ca0` |
| `+0x10` | 4 | `pointer` | `vftable_IDSReferenced` | `0x00481c88` |
| `+0x18` | 4 | `pointer` | `vftable_IDSEventHandler` | `0x00481c74` |
| `+0x20` | 16 | `int[4]` | `pWorldRect` | `CShot_Ctor`; `TraceCollision` / `ResolveHit` / `Update` |
| `+0x30` | 16 | `int[4]` | `pScreenRect` | `CShot_Draw@0x00417bd0` (single-pellet blit); shares bytes with `CGameView::nBounds_*` when `CGameView_Update` / `CDSView_SetRect` run |
| `+0x40` | 4 | `uint` | `dwField_40` | `CGameView` mirror — no CShot-specific consumer |
| `+0x44` | 4 | `uint` | `dwView_flags` | `CGameView` mirror (`TM_SetFrameDelayOverrideMs`, collision bit 0) |
| `+0x48` | 12 | `byte[12]` | `pChain_pad_48` | before `child_chain` |
| `+0x54` | 4 | `pointer` | `child_chain` | `CGameEntity_SetEntityType@0x00418fe0` tests `!= 0` before `CGaming_InsertEntityByDepth` |
| `+0x58` | 16 | `byte[16]` | `pHeader_tail_58` | opaque (`CGameView` parity) |
| `+0x68` | 1 | `byte` | `bActiveInWorld` | `CGameEntity_SetEntityType` `=1`; `CGameView_InitGamingFields` `=0` |
| `+0x69` | 1 | `byte` | `bSkipSpatialWhenNonZero` | `InitGamingFields` |
| `+0x6A` | 1 | `byte` | `bView_state_6a` | `InitGamingFields` |
| `+0x6B` | 1 | `byte` | `bView_flag_6b` | UNK |
| `+0x6C` | 4 | `uint` | `dwView_aux_6c` | **`CGameEntity_SetEntityType` stores entity type (`0x0F`)**; on plain `CGameView` same offset is bucket aux (`GetSpatialBucketKey`) |
| `+0x70` | 1 | `byte` | `bPlayerSlot` | `InitGamingFields` `=0xFF` (view shell; not shooter id) |
| `+0x74` | 16 | `int[4]` | `nCollisionLeft`…`nCollisionBottom` | `CShot_Ctor@0x0041ee3f` `MOV [ESI+0x74..0x80],0`; **not read** by `Update`/`TraceCollision` — shot trace uses `pWorldRect@+0x20` |
| `+0x84` | 4 | `CGaming *` | `pGaming_host` | `CBulanek::AddEntity@0x0041a390`; `TraceCollision` → `SpatialQuery` |
| `+0x88` | 24 | `CDSUpdatedItem` | `updatedItem` | `CDSUpdatedItem_ctor`; `pVftable_IDSUpdated` @ `0x0041ee95` → `0x481c5c`; `Scheduler_RegisterEventSlot(...,0x32,6)` |
| `+0xA0` | 4 | `pointer` | `pBulletFrames` | `CGameView::FUN_00417f40` in ctor |
| `+0xA4` | 1 | `byte` | `bDirection` | ctor |
| `+0xA5` | 1 | `byte` | `bOwnerSlotId` | ctor; shooter slot (`0xFF` neutral) |
| `+0xA6` | 1 | `byte` | `bWeaponStrength` | `CShot::Update@0x0041df60` path select |
| `+0xA7` | 1 | `byte` | `bPelletMask` | init `0x1F`; multi-pellet loop |
| `+0xA8` | 1 | `byte` | `bExpired` | `CGaming_CleanupInactiveBullets` @ `CGaming+0x2C8` list |
| `+0xAC` | 4 | `int` | `nScatterJitter` | ctor when `bWeaponStrength > 2` |

## Key methods

| Symbol | Address |
|--------|---------|
| `CShot_Ctor` | `0x0041edf0` |
| `CShot_SchedulerTick` | `0x0041b1d0` | Primary vtable slot 30 (`0x00481cbc`+0x78 → xref `0x00481d34`); `LEA ESI,[ECX+0x88]`; `Scheduler_GetEventSlot(updatedItem,0)`; if `slot+8` bit0 clear → `Scheduler_EnqueueEvent(param_1,1,slot)` |
| `CShot::Update` | `0x0041df60` |
| `CShot_Draw` | `0x00417bd0` |
| `CShot::TraceCollision` | `0x0041de60` |
| `CShot_ResolveHit` | `0x0041dd70` |
| `CGaming_SpawnBulletAndPlaySound` | `0x0041f230` |

## Ghidra apply

- `recreate_struct CShot` size **176** (`0xb0`); `+0x40..+0x87` mirrors `CGameView` gaming header; `updatedItem` = nested `CDSUpdatedItem` @ `+0x88`.
- `set_function_this_type` **`CShot *`** on `Update`, `TraceCollision`, `ResolveHit`, `ComputePelletRect`; `CShot_Draw` prototype `__thiscall`.
- Primary vtable slot **11** = `Update` (not generic `CGameView_Update`).

## UNK

- `+0x40`, `+0x44`, `pHeader_tail_58` — no CShot-only consumers (inherit `CGameView` names for offset parity).
- `bView_flag_6b` @ `+0x6B` — ctor-cleared only.
- `dwView_aux_6c` @ `+0x6C` — stores **`entityTypeId`** for shots; rename deferred (shared offset semantics on `CGameView` vs combat entities).
- `bPlayerSlot` @ `+0x70` vs shooter `bOwnerSlotId` @ `+0xA5` — distinct roles; net spawn uses `+0xA5`.

## Upward links

- Spawn from **`CWeapon::Fire`** / `CGaming_SpawnBulletAndPlaySound` (`combat_projectiles.md`)
- Owner slot **`+0xA5`** aligns with **`CGameView+0x70`** `bPlayerSlot` family (different fields on `CShot`)
- **`CGaming`** bullet list @ `+0x2C8`

## Scheduler tick (R10 verified)

`CShot_SchedulerTick` @ `0x0041b1d0` — `__thiscall`, `CShot *this`, `void *schedulerList` (stack arg after `RET 4`).

| Step | Disasm | Meaning |
|------|--------|---------|
| 1 | `LEA ESI,[ECX+0x88]` | `CDSUpdatedItem updatedItem` embed offset |
| 2 | `CALL 0x0042f1e0` (`Scheduler_GetEventSlot`, slot 0) | Returns event-slot descriptor |
| 3 | `TEST byte ptr [EAX+0x8],0x1` | Skip enqueue when armed / bit0 set |
| 4 | `CALL 0x0041ad80` (`Scheduler_EnqueueEvent`) | `EnqueueEvent(schedulerList, kind=1, slotRef)` |

Ctor arms slot 0 on spawn-blocked shots: `Scheduler_ArmSlot(updatedItem,0)` @ `0x0041ef68` after `bExpired=1` + `CDSView_Hide`.

## Collision path (R10 verified)

| Field | Offset | Consumer | Evidence |
|-------|--------|----------|----------|
| `pWorldRect` | `+0x20` | `Update`, `TraceCollision`, `ResolveHit`, ctor spawn test | `MOV from [ESI+0x20..0x2c]` @ `Update@0x0041df6b` |
| `pGaming_host` | `+0x84` | `TraceCollision` → `SpatialQuery` | `MOV ECX,[EDI+0x84]` @ `0x0041dee4` |
| `bWeaponStrength` | `+0xA6` | `Update` path select | `0` or `≥3` → single trace; `1` or `2` → 5-pellet loop (`CMP AL,2; JNC` @ `0x0041df9d`) |
| `bPelletMask` | `+0xA7` | multi-pellet loop | `TEST [ESI+0xA7],BL` @ `0x0041dfd0`; cleared per-hit |
| `bExpired` | `+0xA8` | hit / cleanup | `MOV [ESI+0xA8],1` @ `0x0041e031` / `0x0041e056` |
| `bDirection` | `+0xA4` | `ResolveHit` → net placement | `this+0xA4` in `CGaming_OnSlotPlacementEvent` |
| `bOwnerSlotId` | `+0xA5` | `ResolveHit` shooter slot | hit entity `+0x70` = victim slot |
| `nScatterJitter` | `+0xAC` | ctor only when `bWeaponStrength > 2` | `MOV [ESI+0xAC],EAX` @ `0x0041ef8d` |

`TraceCollision` — 4-step lerp `i=1..4`, `SpatialQuery(pGaming_host, rect, …, param_4=1, param_5=1)`; in-bounds when `left/top ≥ 0`, `right ≤ 799`, `bottom ≤ 0x203`.

## Follow-up

- R5 layout: [round5_worker_27_report.md](./round5_worker_27_report.md)
- R10 deep: [r10_deep_task_10_report.md](../deep_recovery/r10_deep_task_10_report.md)
