# Round 4 — Task 08 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 8 |
| **round** | 4 (MCP `set_function_this_type` pass) |
| **title** | Prototype CExplosion blast helpers with CGaming* arg |
| **one_liner** | `CollectEntitiesInBlastRect` / `CollectLandminesInBlastRect` ECX is `CGaming*` (`explosion->pGaming` @ `+0x84`), not `CExplosion*`. |
| **prior** | [round3_task_08_report.md](./round3_task_08_report.md) |
| **structs** | CExplosion, CGaming, CAnim |

## Status

**DONE** — R4 re-verified disasm + decompiler; `set_function_this_type` confirmed for all four blast helpers.

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| Collect helper body uses `CGaming+0x31c` | `0x004183d0` | Entry: `MOV ECX,[ECX+0x31c]`; `ADD ECX,0x54` → view-chain head (not `CExplosion` layout) |
| Decompile `CGaming *this` on collect | `0x004183d0`, `0x0041a2f0` | `CGaming::CExplosion_CollectEntitiesInBlastRect(CGaming *this,…)`; reads `this->field_0x31c` |
| Caller passes `explosion->pGaming` | `0x0041e175` | `ApplyAreaDamage`: `MOV ECX,[ESI+0x84]` then `CALL 0x004183d0` |
| Landmine collect same ECX | `0x0041e288` | `MOV ECX,[ESP+0x40]` (`local_1010` = `pGaming`) → `CALL 0x0041a2f0` |
| `DamageAtPoint` ECX is `CExplosion*` | `0x0041e1e6` | `MOV ECX,ESI` before `CALL 0x0041b250`; uses `this->pGaming`, `this->bOwnerSlot` |
| `pGaming` on explosion | struct | `get_struct_layout CExplosion` → `pGaming` `CGaming *` @ `+0x84` (132), 244 B |
| Secondary caller | `0x004193da`–`0x004193e1` | `MOV ECX,[ESI+0x84]` → `CALL CollectEntitiesInBlastRect` (`CBulanek_CheckSlotCollision`) |
| Mine filter class id | `0x0041a2f0` | Decompile: meta `*(iVar1+8)==0x816` (CMina landmine) |

### Prototype split (verified)

| Function | ECX (`this`) | Stack args |
|----------|--------------|------------|
| `CExplosion_CollectEntitiesInBlastRect` | `CGaming *` | `int *blastRect`, `int *entityBuf`, `char includeHidden` |
| `CExplosion_CollectLandminesInBlastRect` | `CGaming *` | `int *blastRect`, `int *mineBuf` |
| `CExplosion_ApplyAreaDamage` | `CExplosion *` | — |
| `CExplosion_DamageAtPoint` | `CExplosion *` | `tagRECT *`, entity buf/count, ray/octant flags |

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `set_function_this_type` | `0x004183d0` | Confirmed in class `CGaming`; `this` = `CGaming *` |
| `set_function_this_type` | `0x0041a2f0` | Confirmed in class `CGaming`; `this` = `CGaming *` |
| `set_function_this_type` | `0x0041e140` | Confirmed in class `CExplosion`; `this` = `CExplosion *` |
| `set_function_this_type` | `0x0041b250` | Confirmed in class `CExplosion`; `this` = `CExplosion *` |
| `decompile_function` | collect / apply / damage | `CGaming::CExplosion_Collect*`; `ApplyAreaDamage` passes `local_1010 = this->pGaming` |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CExplosion.md](./CExplosion.md) — R4 todo 8 apply log.

## Remaining UNK

- `CGaming.field_0x31c` — entity view list head (`+0x54` chained walk); rename when `CGaming` interior sliced.
- `CExplosion` header gap `pPad_30` @ `+0x30..+0x83` (drawable prefix).
- `CExplosion_OnEvent` — callback `this` at `IDSAnim` facet `+0x8C` (separate from blast helpers).
