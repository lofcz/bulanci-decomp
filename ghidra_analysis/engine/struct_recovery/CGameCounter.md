# CGameCounter

## Status

**PARTIAL** — heap size `0x80` verified; extends `CDSChained` (`0x68`) with `IDSUpdated` vtable at `+0x10`, two `CDSString` pack handles, four `CDSBitmap*` progress slots. Level-load HUD coordinator (`CBulanci::CGameCounterCtor`).

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CGameCounter) == 0x80` | `0x0040eed0` area | `CGameCounter` factory: `OperatorNewWithBadAlloc(0x80)` → `Constructor` |
| Stack embed `0x80` | `0x00410e20` | `CBulanci::CGame_StartGame`: `CGameCounter local_a8 [16]` is decompiler artifact; ctor path matches heap layout |
| Tail ends at `+0x7c` | `0x0040b140` | `CGameCounter_OnEvent` reads `*(this+0x7c)` when `param_2 >= 3` |
| `CDSChained` prefix `0x68` | `0x0040bc20` | `Constructor`: `CDSChained_ctor` then vtables / vector ctor at `+0x68` |

## Class registry

| Claim | Address | Evidence |
|-------|---------|----------|
| **classId = 2041 (`0x7f9`)** | static init comment | `HandleClassRegister(classId 0x7f9/2041, CGameCounter factory @ 0x40eed0)` |
| Factory | `0x0040eed0` | `OperatorNew(0x80)` + `Constructor` |
| `GetClassIdentifier` | `0x0040bca0` | RTTI string export |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `void *` | `pVftable_primary` | `Constructor@0x0040bc20`, `CGameCounterCtor@0x00410e20` |
| 0x04 | 4 | `void *` | `pVftable_IDSChained` | same |
| 0x08..0x64 | — | — | `CDSChained` fields | `CDSChained_ctor@0x004032d0` (see [CDSChained.md](./CDSChained.md)) |
| 0x10 | 4 | `void *` | `pVftable_IDSUpdated` | `Constructor` → `g_pCGameCounter_vftable_IDSUpdated` (overwrites `CDSChained` event slot) |
| 0x18 | 4 | `void *` | `pVftable_IDSEventHandler` | `Constructor` → `g_pCGameCounter_vftable_IDSEventHandler` |
| 0x68 | 4 | `CDSString` | `strPackFrame` | `_eh_vector_constructor_iterator_(this+0x68,4,2,...)`; `CGameCounterCtor` stores menu-cache bitmap `0x10023` ref |
| 0x6C | 4 | `CDSString` | `strPackCancelBtn` | `CGameCounterCtor` stores `0x10024` ref; `OnEvent@0x0040b140` `ODSImage__SetImage` source when `param_2 >= 3` |
| 0x70 | 4 | `CDSBitmap *` | `pProgressSlot0` | `CGameCounterCtor` loop ×4, x=`0x205` step `0x46`, y=`0x219` |
| 0x74 | 4 | `CDSBitmap *` | `pProgressSlot1` | same |
| 0x78 | 4 | `CDSBitmap *` | `pProgressSlot2` | same |
| 0x7C | 4 | `CDSBitmap *` | `pProgressSlot3` | same; `OnEvent` image + invalidate when `param_2 >= 3` |

## Behavior (leaf evidence)

| Handler | Address | Notes |
|---------|---------|-------|
| `CGameCounter_OnEvent` | `0x0040b140` | Event `0xE2`: sub `4` → `PostMessage(this+0x10, 0x100, 0x8002)`; sub `<3` → bank sample `0x12`, `ODSImage__SetImage` on `pProgressSlot[param_2]`; else sample `0x11` + slot3 |
| `CGameCounterCtor` | `0x00410e20` | Loads pack ids `0x10022`–`0x10024`; backdrop child via `CDSBitmap_ctor`; 4 horizontal progress bitmaps |
| `CGameCounter_UpdatePlayerScore` | `0x004276c0` | Global helper; drives per-player HUD strings (caller xrefs on `CGaming`) |

## Ghidra apply

**Applied (R3 task 9, 2026-05-30):** `get_struct_layout` → 128 B: `CDSChained base` @0 + `strPackFrame`/`strPackCancelBtn` @0x68/0x6C + `pProgressSlot0..3` (`CDSBitmap *`) @0x70..0x7C. `CGameCounter_Constructor@0x40bc20`, `CGameCounter_OnEvent@0x40b140` typed with `CGameCounter *` this; decompile uses `pProgressSlotN->trackImage`. `save_program bulanci.exe`.

## UNK

- Exact semantic names for `CDSString` pack entries at `+0x68` (frame vs cancel art).
- `CGameCounter_OnEvent` subcodes `0..3` mapping to round phases (inferred from audio `0x11`/`0x12` in `scoring_hud.md`).
- Full `CDSChained` mid-fields not re-xref'd on this class (inherited UNK from [CDSChained.md](./CDSChained.md)).
