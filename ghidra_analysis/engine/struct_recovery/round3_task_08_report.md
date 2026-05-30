# Round 3 — Task 08 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 8 |
| **title** | Prototype CExplosion blast helpers with CGaming* arg |
| **one_liner** | `CollectEntitiesInBlastRect` / `CollectLandminesInBlastRect` ECX is `CGaming*` (`explosion->pGaming` @ `+0x84`), not `CExplosion*`. |
| **acceptance** | Ghidra decompiler shows `CGaming *this` on collect helpers; `ApplyAreaDamage` passes `this->pGaming`; evidence in CExplosion.md |
| **source_batches** | 07 |
| **structs** | CExplosion, CGaming, CAnim |

## Status

**DONE**

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| Collect helper ECX is `CGaming*` | `0x004183d0` | Disasm entry: `MOV ECX,[ECX+0x31c]`; `ADD ECX,0x54` → view-chain walk (not explosion layout) |
| Entity list head @ `CGaming+0x31c` | `0x004183d0`, `0x0041a2f0` | After `set_function_this_type`: decompile `*(int *)&this->field_0x31c + 0x54` |
| Caller passes `explosion->pGaming` | `0x0041e175` | `ApplyAreaDamage`: `MOV ECX,[ESI+0x84]` then `CALL 0x004183d0` |
| Landmine collect same ECX rule | `0x0041e288` | `MOV ECX,[ESP+0x40]` (`local_1010` = `pGaming`) → `CALL 0x0041a2f0` |
| `DamageAtPoint` ECX is `CExplosion*` | `0x0041e1e6` | `MOV ECX,ESI` (`ESI` = explosion) before `CALL 0x0041b250`; uses `this->pGaming`, `this->bOwnerSlot` |
| `pGaming` field on explosion | `CExplosion` struct | `get_struct_layout`: `pGaming` `CGaming *` @ `+0x84` (132) |
| Secondary caller | `0x004193e1` | `CBulanek_CheckSlotCollision` → `CollectEntitiesInBlastRect` (slot/collision spatial query) |
| Mine filter class id | `0x0041a2f0` | Decompile: meta `*(iVar1+8)==0x816` (CMina landmine) |

### Prototype split (resolved)

| Function | ECX (`this`) | Stack args |
|----------|--------------|------------|
| `CExplosion_CollectEntitiesInBlastRect` | `CGaming *` | `int *blastRect`, `int *entityBuf`, `char includeHidden` |
| `CExplosion_CollectLandminesInBlastRect` | `CGaming *` | `int *blastRect`, `int *mineBuf` |
| `CExplosion_ApplyAreaDamage` | `CExplosion *` | — |
| `CExplosion_DamageAtPoint` | `CExplosion *` | `tagRECT *`, entity buf/count, ray/octant flags |

## Ghidra deltas

- `set_function_this_type@0x004183d0` → `CGaming *` (moved into `CGaming` namespace; decompiler `CGaming::CExplosion_CollectEntitiesInBlastRect`).
- `set_function_this_type@0x0041a2f0` → `CGaming *` (`CGaming::CExplosion_CollectLandminesInBlastRect`).
- `set_function_this_type@0x0041e140` / `@0x0041b250` → `CExplosion *` (confirmed member functions).
- `set_decompiler_comment` @ `0x004183d0`, `0x0041a2f0`.
- Verified `CExplosion.pGaming` @ `+0x84` already `CGaming *` (244 B struct).
- `save_program bulanci.exe`.

## Struct doc updates

- [CExplosion.md](./CExplosion.md) — R3 todo 8 apply log + UNK trimmed for collect-helper ECX.

## Remaining UNK

- `CGaming.field_0x31c` — rename to semantic entity-list / view-root field when `CGaming` interior is next sliced.
- `CExplosion` header gap `pPad_30` @ `+0x30..+0x83` (drawable prefix; batch-3 `CAnim` mirror).
- `CExplosion_OnEvent` — callback `this` at `IDSAnim` facet `+0x8C` (separate from blast helpers).
