# CExplosion

## Status

**PARTIAL** — allocation size `0xF4` verified; extends `CAnim` (`0xF0` base). Derived field `ownerSlot` at `+0xF0` proven (ctor write + `DamageAtPoint` read). Ghidra `CExplosion` tail `+0x9C..+0xEF` now mirrors batch-3 `CAnim_recovered` inner/track fields; `+0x28..+0x67` header gap still opaque.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CExplosion) == 0xF4` | `0x0041ce30` | `CExplosion_Ctor` after `OperatorNewWithBadAlloc(0xf4)` |
| Same in combat spawn | `0x004183d0` area / `CShot_ResolveHit` | `OperatorNewWithBadAlloc(0xf4)` → `CExplosion_Ctor` @ `0x84284` |
| `CAnim` base `0xF0` | — | `sizeof` `0xF4` − `ownerSlot` @ `+0xF0` (4 bytes) |
| `CAnim` child alloc | `0x00411b50` | `CExitDlg_ctor` allocates decor `CAnim` with `0xf0` (independent proof of anim object size) |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|-------|------|------|------|----------------------|
| 0x00 | 4 | `void *` | `pVftable_primary` | `CExplosion_Ctor@0x0041ce30`; `CExplosion_InitVtables@0x0041aad0` |
| 0x04 | 4 | `void *` | `pVftable_IDSChained` | same |
| 0x10 | 4 | `void *` | `pVftable_field10` | same |
| 0x18 | 4 | `void *` | `pVftable_IDSEventHandler` | same |
| 0x14 | 2 | `ushort` | `wViewFlags` | `CExplosion_Ctor@0x0041ce30` `\|= 0x200` |
| 0x20 | 4 | `int` | `bbox_left` | ctor centers rect; `ApplyAreaDamage@0x0041e140` reads `+0x20` |
| 0x24 | 4 | `int` | `bbox_top` | `ApplyAreaDamage` reads `+0x24` |
| 0x28 | 4 | `int` | `bbox_right` | `ApplyAreaDamage` reads `+0x28` |
| 0x2C | 4 | `int` | `bbox_bottom` | `ApplyAreaDamage` reads `+0x2c` |
| 0x84 | 4 | `CGaming *` | `pGaming` | `CExplosion_ApplyAreaDamage@0x0041e140` `param_1->pGaming`; `CExplosion_DamageAtPoint@0x0041b250` |
| 0x88 | 4 | `void *` | `pVftable_IDSUpdated` | `CExplosion_Ctor@0x0041ce30` |
| 0x8C | 4 | `void *` | `pVftable_IDSAnim` | same; `CExplosion_OnEvent@0x0041efe0` — **ECX = object+0x8C** (see plate comment) |
| 0x90 | 4 | `CBulanci *` | `pOds_owner` | `ODSImage` mixin (mirrors `CAnim+0x90`) |
| 0x94 | 4 | `CDSImage *` | `pOds_drawable` | same |
| 0x98 | 4 | `void *` | `pVftable_anim_sub` | ctor; `TM_BindSequence(this+0x98,…)@0x0041ce30` |
| 0x9C | 4 | `int` | `anim_inner_field_9c` | `CAnim_SubobjectCtor@0x00419870` (via `CExplosion_Ctor` → `CAnim_SubobjectCtor`) |
| 0xA0 | 4 | `int` | `anim_inner_field_a0` | same |
| 0xA4 | 4 | `int` | `anim_inner_field_a4` | same |
| 0xA8 | 72 | `byte[72]` | `track_manager` | `ConstructTrackManager` / `TM_Destructor` on `CAnim` (`CAnim.md`); opaque blob in Ghidra |
| 0xF0 | 1 | `byte` | `ownerSlot` | `CExplosion_Ctor` `this[0xf0]=param_3`; `DamageAtPoint` `this->bOwnerSlot` |

## Ghidra apply (slice 07, 2026-05-30)

```
get_struct_layout CExplosion → 244 B (0xF4)
  pGaming@0x84 (CGaming *), pOds_owner/drawable@0x90, track_manager@0xA8, bOwnerSlot@0xF0
rename ApplyAreaDamage/DamageAtPoint/Collect* / OnEvent → CExplosion_* @ 0x41e140/0x41b250/0x4183d0/0x41a2f0/0x41efe0
set_function_prototype Collect* → __thiscall (CGaming *this, …)  [ECX still void* in decompiler per MCP limit]
plate comment CExplosion_OnEvent: real object = (CExplosion *)((char *)this - 0x8C)
save_program bulanci.exe
```

## Ghidra apply (R3 todo 8, 2026-05-30)

```
modify_struct_field CExplosion pGaming@0x84 → CGaming *  [verified 244 B layout]
set_function_this_type@0x004183d0 / @0x0041a2f0 → CGaming *  (moved into CGaming class; decompiler fixed)
set_function_this_type@0x0041e140 / @0x0041b250 → CExplosion *
ApplyAreaDamage@0x41e175: MOV ECX,[ESI+0x84] before Collect*; DamageAtPoint uses ECX=ESI (explosion)
set_decompiler_comment@0x004183d0 / @0x0041a2f0
save_program bulanci.exe
```

Decompile after apply: `CGaming::CExplosion_CollectEntitiesInBlastRect(CGaming *this,…)` reads `this->field_0x31c` (+0x54 chain); `ApplyAreaDamage` calls collect with `local_1010 = this->pGaming`.

## Key functions

| Symbol | Address |
|--------|---------|
| `CExplosion_Ctor` | `0x0041ce30` |
| `CExplosion_ApplyAreaDamage` | `0x0041e140` |
| `CExplosion_DamageAtPoint` | `0x0041b250` |
| `CExplosion_CollectEntitiesInBlastRect` | `0x004183d0` |
| `CExplosion_CollectLandminesInBlastRect` | `0x0041a2f0` |
| `CExplosion_OnEvent` | `0x0041efe0` |

## UNK

- `+0x28..+0x67` drawable header gap (see `CAnim.md` `header_unk_28`).
- `track_manager` inner layout (`+0xAC..+0xEF` on standalone `CAnim`) not field-resolved.
- `CExplosion_OnEvent` decompile still shows `this[-1].pPad_30` until callback `this` is retyped to `IDSAnim` at `+0x8C`.
- `CGaming.field_0x31c` on collect helpers — entity view list head (`+0x54` chained walk); rename when `CGaming` interior sliced.
- Duplicate type `CExplosion_recovered` (241 B) created during field repair — safe to delete manually if desired.
