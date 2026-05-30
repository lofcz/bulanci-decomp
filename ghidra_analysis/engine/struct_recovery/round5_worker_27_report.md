# Round 5 — Worker 27 report (CShot / bullet struct UNKs)

## Task

| Field | Value |
|-------|-------|
| **worker** | 27 / 50 |
| **round** | 5 (WRITE) |
| **focus** | Resolve `CShot` struct UNKs from existing docs (`CShot.md`, `CGameView.md`, `CDSUpdatedItem.md`, `combat_projectiles.md`) |
| **types** | `CShot` (no `CBullet` type in binary) |
| **manifest note** | `agent_todos_50_r5.json` id 27 is CDSChain handoff — **parent scope override:** projectile `CShot` layout only |
| **prior** | [CShot.md](./CShot.md), [round4_task_31_report.md](./round4_task_31_report.md) (spawn from `CWeapon::Fire`) |

## Status

**DONE** — Replaced `pPad_40` / `pPad_7c` / `byte[24] updatedItem` blobs with `CGameView` gaming-band fields (`+0x40..+0x87`), nested `CDSUpdatedItem`, and typed tail; decompiler shows named fields in `CShot_Ctor` / `Update` / `TraceCollision`.

## Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| No `CBullet` struct | `search_data_types` | Pattern `CBullet` — zero hits |
| `sizeof == 0xB0` | `CGaming_SpawnBulletAndPlaySound@0x0041f230` | `OperatorNew(0xB0)` + `CShot_Ctor` |
| Collision AABB zeroed | `CShot_Ctor@0x0041ee3f` | `MOV [ESI+0x74..0x80], 0` before `InitGamingFields` |
| Gaming bytes via shared helper | `CGameView_InitGamingFields@0x00416590` | Cast `(CGameView *)this`; writes `bActiveInWorld`, `bPlayerSlot=0xFF`, `pGaming_host=0` |
| Entity type + active flag | `CGameEntity_SetEntityType@0x00418fe0` | `*(this+0x6c)=param_1` (`0x0F`); `*(this+0x68)=1`; uses `pGaming_host` @ `+0x84`, `child_chain` @ `+0x4c` |
| `CDSUpdatedItem` embed | `CShot_Ctor@0x0041ee61` | `CDSUpdatedItem_ctor(&updatedItem)`; `pVftable_IDSUpdated = 0x481c5c` |
| Scheduler 50 ms tick | `Scheduler_RegisterEventSlot@0x0042f210` | `RegisterEventSlot(updatedItem, 0, 0x32, 6)` from ctor |
| World / screen rects | `CShot_Ctor` / `CShot_Draw` / `CShot_Update` | `pWorldRect` spawn sizing from `pBulletFrames+4/+8`; draw uses `pScreenRect` when `bWeaponStrength` is `0` or `2` |
| Shooter slot | `CShot_Ctor@0x0041eeae` | `bOwnerSlotId = param_4` @ `+0xA5` (distinct from `bPlayerSlot` @ `+0x70`) |
| Spatial query host | `TraceCollision@0x0041de60` | `_Globals__SpatialQuery(this->pGaming_host, …)` |

### Offset parity (`+0x40`..`+0x87`)

`CShot` does **not** use `CGameView` `nOrigin_x` / spatial buckets at `+0x28..+0x3F` — those 32 bytes are **`pWorldRect` + `pScreenRect`**. From **`+0x40` (hex)** through **`+0x87`**, field names match [CGameView.md](./CGameView.md) (R4 pass) so `InitGamingFields`, `CGameView_Update`, and `CGameEntity_SetEntityType` decompile with consistent offsets.

## Ghidra deltas (R5 worker 27)

| Action | Target | Result |
|--------|--------|--------|
| `recreate_struct` | `CShot` | 176 B; vtables `+0x00..+0x18`; rects `+0x20`/`+0x30`; gaming band `+0x40..+0x87`; `CDSUpdatedItem updatedItem` @ `+0x88`; tail `+0xA0..+0xAC` |
| `set_function_this_type` | `CShot_Update`, `TraceCollision`, `CShot_ResolveHit`, `CShot_ComputePelletRect` | `CShot *` |
| `set_function_prototype` | `CShot_Draw@0x00417bd0` | `void __thiscall CShot_Draw(CShot *this)` |
| `set_decompiler_comment` | `CGameEntity_SetEntityType@0x00418fe0` | entity type @ `+0x6c`, active @ `+0x68` |
| `force_decompile` | `CShot_Ctor`, `CShot_Update`, `TraceCollision` | Named `updatedItem`, `pGaming_host`, collision fields |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CShot.md](./CShot.md) — full layout table; `CBullet` note; UNK trimmed; R5 follow-up link

## Remaining UNK

- `dwField_40`, `dwView_flags`, `pHeader_tail_58`, `bView_flag_6b` — no shot-only xref (kept as `CGameView` parity names).
- `dwView_aux_6c` @ `+0x6C` — **stores `entityTypeId` (`0x0F`) on shots** but shares offset with `CGameView_GetSpatialBucketKey` bucket aux on other view types; per-class rename not applied.
- `bPlayerSlot` @ `+0x70` vs `bOwnerSlotId` @ `+0xA5` — both proven; runtime fill of `bPlayerSlot` on bullets not observed.
