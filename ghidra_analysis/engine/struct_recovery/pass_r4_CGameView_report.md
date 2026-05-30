# Pass R4 — CGameView remaining header pieces

## Task

Map remaining **`CGameView`** shell fields and **`__thiscall`** / member helpers: `pGaming_host`, spatial buckets, `wViewFlags`, collision rect, `InitGamingFields`, TM helpers, **`CBulanek`** prefix overlap. Sources: [CGameView.md](./CGameView.md), [CBulanek.md](./CBulanek.md), [round3_task_04_report.md](./round3_task_04_report.md), [round3_task_41_report.md](./round3_task_41_report.md).

## Status

**DONE**

## Struct deltas (`CGameView` 152 B)

| Offset | Name (after) | Evidence |
|--------|--------------|----------|
| `+0x14` | `wViewFlags` | unchanged; `CBulanekCtor` `\|= 0x200` |
| `+0x28` / `+0x2C` | `nSpatial_bucket_x` / `nSpatial_bucket_y` | `GetSpatialBucketKey` → `dwView_aux_6c + nSpatial_bucket_y` |
| `+0x30..+0x3F` | `nBounds_left/top/right/bottom` | `CDSView_SetRect` rect band |
| `+0x44` | `dwView_flags` | `TM_SetFrameDelayOverrideMs` assigns ms; `GetWorldCollisionRect` `(dwView_flags & 1)` |
| `+0x68..+0x6a` | `bActiveInWorld`, `bSkipSpatialWhenNonZero`, `bView_state_6a` | aligned with `CBulanek` prefix (R3 todo 41) |
| `+0x70` | `bPlayerSlot` | was `bGaming_slot_id`; `OnEvent` slot `0xd7`/`0xd8` |
| `+0x74..+0x80` | `nCollisionLeft/Top/Right/Bottom` | was `nField_74..80`; `GetWorldCollisionRect` + `CBulanekCtor` |
| `+0x84` | `pGaming_host` | **`CGaming *`** (R4 todo 4) |

**Prefix overlap:** `get_struct_layout CBulanek` bytes `+0x00..+0x87` use the same field names as `CGameView` except `+0x84` (`pGamingHostScratch` vs `pGaming_host`).

## `set_function_this_type` → `CGameView *`

| Address | Symbol | Decompile (post-pass) |
|---------|--------|------------------------|
| `0x00416590` | `CGameView_InitGamingFields` | `bActiveInWorld..bSkipSpatialWhenNonZero = 0`; `bPlayerSlot = 0xff`; `pGaming_host = 0` |
| `0x00416f50` | `CGameView_GetSpatialBucketKey` | `return dwView_aux_6c + nSpatial_bucket_y` |
| `0x00417210` | `CGameView_GetWorldCollisionRect` | `nCollisionLeft..Bottom` + origins when `(dwView_flags & 1)` |
| `0x00419010` | `CGameView_Update` | `bActiveInWorld` + `pGaming_host` → `CGaming_InsertEntityByDepth` |
| `0x004191a0` | `CGameView_ctor` | zeros collision ints; `InitGamingFields`; `ODSImage_ctor` @ `+0x88` |
| `0x00419130` | `CBitmap_OnDraw` | `TM_TickBlit((int)&this->ods_image)` — plated via `__thiscall` prototype fix |
| `0x0041acf0` | `CGameView_OnEvent` | `pGaming_host`, `bPlayerSlot`; `CGaming_UnregisterAndRemoveObject` @ `0xf9` |
| `0x00419280` | `CBitmap_FireOnBitmapEvtFromView` | moved into class `CGameView` |
| `0x00439720` | `TM_SetFrameDelayOverrideMs` | `this->dwView_flags = param_1` |

**Note:** `CBitmap_ViewHeader_Init@0x00419070` remains **`CAnim *`** when reached from `CAnim_ctor` (shared MI init).

## Ghidra commands

- `modify_struct_field` — collision / gaming byte renames on `CGameView`
- `set_function_prototype` — `CBitmap_OnDraw` → `void __thiscall CBitmap_OnDraw(CGameView *this)`
- `set_function_this_type` — addresses above
- `set_decompiler_comment` @ `0x00439720`
- `force_decompile` — proof sites
- `save_program bulanci.exe`

## Struct doc updates

- [CGameView.md](./CGameView.md) — full layout table, CBulanek overlap, TM / collision notes

## Remaining UNK

- `dwView_flags` dual semantics (ms override vs collision bit 0).
- Slot-event helpers still declare `(CGameView *)` for `CGaming *` first parameter.
- `CBitmap_ViewHeader_Init` dual-type entry (`CAnim` vs `CGameView`).
