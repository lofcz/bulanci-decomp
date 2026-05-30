# CShot

## Status

**PARTIAL** — `sizeof == 0xb0` (176 B) proven via heap alloc; header through `+0xAC` documented in `combat_projectiles.md` with ctor/update/draw xrefs. Extends **`CGameView`** shell + **`CDSUpdatedItem`** @ `+0x88`.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CShot) == 0xb0` | `CGaming_SpawnBulletAndPlaySound` | `OperatorNewWithBadAlloc(0xB0)` + `CShot_Ctor` |
| ctor | `0x0041edf0` | `CShot_Ctor` — `CDSChained_ctor` → `CDSUpdatedItem_ctor(this+0x88)` → CShot vtables |
| Subobject ctor | `0x00417030` | `CShot_SubobjectCtor` (shell helper) |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `pointer` | `vftable_primary` | `CShot_Ctor@0x0041edf0` → `0x00481cbc` |
| `+0x04` | 4 | `pointer` | `vftable_IDSChained` | `0x00481ca0` |
| `+0x10` | 4 | `pointer` | `vftable_IDSReferenced` | `0x00481c88` |
| `+0x18` | 4 | `pointer` | `vftable_IDSEventHandler` | `0x00481c74` |
| `+0x20` | 16 | `int[4]` | `worldRect` | `CShot_Ctor`; `TraceCollision` / `ResolveHit` |
| `+0x30` | 16 | `int[4]` | `screenRect` | `CShot_Draw@0x00417bd0` |
| `+0x68` | 1 | `byte` | `movementEnabled` | `CGameEntity_SetEntityType@0x00418fe0` |
| `+0x6C` | 4 | `int` | `entityTypeId` | `CShot_Ctor` sets **`0x0F`** |
| `+0x84` | 4 | `CGaming *` | `pCGaming` | `CBulanek::AddEntity@0x0041a390` |
| `+0x88` | — | `CDSUpdatedItem` | `scheduler` | `Scheduler_RegisterEventSlot(this+0x88,0,0x32,6)` |
| `+0xA0` | 4 | `pointer` | `pBulletFrames` | `FUN_00417f40` in ctor |
| `+0xA4` | 1 | `byte` | `direction` | ctor |
| `+0xA5` | 1 | `byte` | `ownerSlotId` | ctor; `0xFF` neutral |
| `+0xA6` | 1 | `byte` | `weaponStrength` | `CShot::Update@0x0041df60` path select |
| `+0xA7` | 1 | `byte` | `pelletMask` | init `0x1F`; multi-pellet loop |
| `+0xA8` | 1 | `byte` | `expired` | cleanup @ `CGaming+0x2C8` list |
| `+0xAC` | 4 | `int` | `scatterJitter` | ctor when `weaponStrength > 2` |

## Key methods

| Symbol | Address |
|--------|---------|
| `CShot_Ctor` | `0x0041edf0` |
| `CShot::Update` | `0x0041df60` |
| `CShot_Draw` | `0x00417bd0` |
| `CShot::TraceCollision` | `0x0041de60` |
| `CShot_ResolveHit` | `0x0041dd70` |
| `CGaming_SpawnBulletAndPlaySound` | `0x0041f230` |

## Ghidra apply

- `create_struct CShot` size **176** (`0xb0`); prefix mirrors `CGameView` through `+0x87`.
- Primary vtable slot **11** = `Update` (not generic `CGameView_Update`).

## UNK

- `+0x44..+0x67` CDSView band (partial in `widgets.md`).
- Exact `CDSUpdatedItem` size/fields inside `+0x88..+0x9F`.

## Upward links

- Spawn from **`CWeapon_FirePistol`** / machine gun (`combat_projectiles.md`)
- Owner slot convention **`+0xA5`** aligns with **`CGameView+0x70`** `gaming_slot_id` family
- **`CGaming`** bullet list @ `+0x2C8`
