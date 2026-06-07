# Round 10 — Deep task 10 report (CShot projectile)

## Task

| Field | Value |
|-------|-------|
| **id** | 10 / 20 |
| **round** | 10 (deep_recovery) |
| **struct** | `CShot` |
| **seed** | `0x0041b1d0` (`CShot_SchedulerTick`) |
| **addresses** | `0x0041b1d0`, `0x0041aa30`, `0x0041aa40` |
| **acceptance** | Scheduler tick + collision fields with disasm proof; update `CShot.md` |

## Status

**DONE** — Live Ghidra decompile/disasm confirms scheduler hook on `updatedItem@+0x88` and full collision field usage in `Update`/`TraceCollision`/`ResolveHit`. IDA `sub_41DF60` branch predicate corrected via disasm.

## Functions / Struct

| Symbol | Address | Role | Evidence |
|--------|---------|------|----------|
| `CShot_SchedulerTick` | `0x0041b1d0` | Primary vtable slot **30** | `LEA ESI,[ECX+0x88]`; `Scheduler_GetEventSlot(0)`; `TEST [EAX+8],1`; `Scheduler_EnqueueEvent(list,1,slot)`; xref from `0x00481d34` (vtable `0x00481cbc`) |
| `CShot_Update` | `0x0041df60` | Per-tick move + collision | Reads `pWorldRect@+0x20`; branches on `bWeaponStrength@+0xA6`; sets `bExpired@+0xA8` |
| `CShot_TraceCollision` | `0x0041de60` | Sub-frame spatial trace | `pGaming_host@+0x84` → `SpatialQuery@0x00418300`; 4 lerp steps; bounds `800×0x204` |
| `CShot_ResolveHit` | `0x0041dd70` | Hit dispatch | `bWeaponStrength==2` → `CExplosion`; else `bOwnerSlotId@+0xA5`, `bDirection@+0xA4`, victim `entity+0x70` |
| `CShot_Ctor` | `0x0041edf0` | Init + spawn overlap | Zeros `+0x74..+0x80`; fills tail `+0xA0..+0xAC`; `Scheduler_RegisterEventSlot(updatedItem,0,0x32,6)` |
| `CMina_GetClassTable` | `0x0041aa30` | *(manifest neighbor)* | Returns `DAT_004b384c` — **not** `CShot`-related |
| `CDSChain_ReleaseChild` | `0x0041aa40` | *(manifest neighbor)* | Chain dtor helper — **not** `CShot`-related |

### Collision fields (proven offsets)

| Offset | Field | Used by |
|--------|-------|---------|
| `+0x20` | `pWorldRect[4]` | `Update` (old rect), `TraceCollision`, ctor spawn query, `ResolveHit` explosion rect |
| `+0x84` | `pGaming_host` | `TraceCollision` → `SpatialQuery` |
| `+0xA4` | `bDirection` | `ResolveHit` → `CGaming_OnSlotPlacementEvent` |
| `+0xA5` | `bOwnerSlotId` | `ResolveHit` shooter slot |
| `+0xA6` | `bWeaponStrength` | `Update` single vs multi-pellet (`1` **or** `2` = scatter) |
| `+0xA7` | `bPelletMask` | Multi-pellet loop; bit cleared per hit |
| `+0xA8` | `bExpired` | Set on terminal hit; `CGaming_CleanupInactiveBullets` |
| `+0xAC` | `nScatterJitter` | Ctor when `bWeaponStrength > 2` |
| `+0x74..+0x80` | `nCollisionLeft…Bottom` | Ctor zero only — **no** `Update`/`TraceCollision` read |

### Scheduler embed

| Offset | Field | Evidence |
|--------|-------|----------|
| `+0x88` | `CDSUpdatedItem updatedItem` | `CShot_SchedulerTick`: `this+0x88`; ctor `CDSUpdatedItem_ctor` + `RegisterEventSlot(0,0x32,6)` @ `0x0041ef1f` |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `CShot_Update@0x0041df60` | Documents multi-pellet gate for `bWeaponStrength` 1 **or** 2 (IDA mismatch) |
| `save_program` | `bulanci.exe` | saved |

No struct recreation needed — `get_struct_layout CShot` already 176 B with named tail fields.

## Decomp corrections

| Location | IDA (`bulanci.ida.exe.c`) | Ghidra / disasm (correct) |
|----------|---------------------------|---------------------------|
| `sub_41DF60` @ `0x0041df60` | `if (*(this+166) == 1)` → multi-pellet only | Disasm: `TEST AL; JZ` single when `0`; `CMP AL,2; JNC` single when `≥3`; **multi-pellet for `1` and `2`** |
| `sub_41B1D0` @ `0x0041b1d0` | `this+34` (= `+0x88`) | Matches Ghidra `updatedItem` — IDA dword index notation |

## Frida

Not required — static disasm + decompile sufficient.

## Remaining UNK

- `nCollisionLeft…@+0x74` — inherited `CGameView` band; zeroed on shot ctor but unused in projectile trace (uses `pWorldRect` directly).
- `dwField_40`, `dwView_flags`, `pHeader_tail_58`, `bView_flag_6b` — no shot-only consumers (R5 parity names retained).
- `dwView_aux_6c` — stores `entityTypeId` (`0x0F`); shared offset semantics on plain `CGameView`.

## Struct doc

- Updated [CShot.md](../struct_recovery/CShot.md) — R10 scheduler + collision sections.
