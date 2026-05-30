# CGameView

## Status

**PARTIAL** — **`0x98` (152 B)** image/shell layout proven via `CGameView_ctor` @ `0x004191a0` (patched to **`CBitmap`** vtables). **`0x88` (136 B)** partial shell via `CGameView_CreateObject` @ `0x00418c60`. **`CBulanek`** reuses the same header through `+0x87` then replaces `+0x88` with `CDSUpdatedItem` (extends to **`0x19c`** — see `CBulanek.md`).

Ghidra: **`CGameView` 152 B** + **`CGameView *`** registered (agent todo 3, 2026-05-30); layout mirrors **`CBitmap`** through `+0x87` (`ODSImage` @ `+0x88`). Prototypes: `CGameView_ctor` → **`CGameView *`**, `CGameView_InitGamingFields` / `CGameView_Update` use **`CGameView *`** (InitGamingFields via `set_parameter_type` on `__fastcall` ECX).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof == 0x98` | `0x0041d820` | `CLevelScriptOpExt_CreateImage`: `OperatorNew(0x98)` → `CGameView_ctor` |
| `sizeof == 0x88` | `0x00418c60` | `CGameView_CreateObject`: `OperatorNew(0x88)` — no `ODSImage_ctor` |
| `sizeof == 0xB0` | `0x00417030` | `CShot_SubobjectCtor` — shot shell extends header |
| `sizeof == 0x19c` | spawn path | **`CBulanek`** child (`CBulanek.md`) |

## Layout table (`+0x00`–`+0x97`)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `pointer` | `vftable_primary` | `CGameView_ctor@0x004191a0` → `CBitmap` primary `0x00481f74` |
| `+0x04` | 4 | `pointer` | `vftable_IDSChained` | ctor |
| `+0x10` | 4 | `pointer` | `vftable_IDSReferenced` | ctor |
| `+0x14` | 2 | `ushort` | `wViewFlags` | `CDSView_SetRect@0x0042c480` |
| `+0x18` | 4 | `pointer` | `vftable_IDSEventHandler` | ctor |
| `+0x1C` | 4 | `uint` | `dwField_1c` | `CDSChained` mirror (same as `CAnim`) |
| `+0x20` | 4 | `int` | `origin_x` | `CGameView_ctor` |
| `+0x24` | 4 | `int` | `origin_y` | `CGameView_ctor` |
| `+0x28` | 8 | `byte[8]` | `pPad_28` | gap before bounds; `+0x2c` read in `GetSpatialBucketKey@0x00416f50` |
| `+0x30` | 16 | `int[4]` | `bounds` | `CDSView_SetRect@0x0042c480` (`nBounds_*` in Ghidra) |
| `+0x40` | 4 | `uint` | `dwField_40` | no direct consumer |
| `+0x44` | 4 | `uint` | `view_flags` | `CDSView_SetRect@0x0042c480` `(field & 0x40)` |
| `+0x48` | 12 | `byte[12]` | `pChain_pad_48` | before `child_chain` |
| `+0x54` | 4 | `pointer` | `child_chain` | `CDSChained_GetFirstChildView(this+0x54)` @ `CDSView_SetRect` |
| `+0x58` | 16 | `byte[16]` | `pHeader_tail_58` | opaque tail through `+0x67` (no `pLinked_bulanek` on view shell) |
| `+0x68` | 1 | `byte` | `view_state_68` | `CGameView_InitGamingFields@0x00416590` |
| `+0x69` | 1 | `byte` | `view_state_69` | `FUN_00416590` |
| `+0x6A` | 1 | `byte` | `view_state_6a` | `FUN_00416590` |
| `+0x70` | 1 | `byte` | `gaming_slot_id` | `FUN_00416590` init `0xFF`; script slot reads |
| `+0x74` | 4 | `int` | `field_74` | ctor `= 0`; `CGameView_Update` vtable use |
| `+0x78` | 4 | `int` | `field_78` | ctor `= 0` |
| `+0x7C` | 4 | `int` | `field_7c` | ctor `= 0` |
| `+0x80` | 4 | `int` | `field_80` | ctor `= 0` |
| `+0x84` | 4 | `pointer` | `gaming_host` | `FUN_00416590` `= 0`; `CGameView_Update` |
| `+0x88` | 16 | `ODSImage` | `ods_image` | `ODSImage_ctor` in full ctor; **`CBulanek` replaces with `CDSUpdatedItem`** |
| `+0x8C` | 4 | `pointer` | `ods_image.pVf_odsimage` | ctor patches `0x00481f00` (IDSAnim) |

## Key methods

| Symbol | Address |
|--------|---------|
| `CGameView_ctor` | `0x004191a0` |
| `CGameView_CreateObject` | `0x00418c60` |
| `CGameView_Update` | `0x00419010` |
| `CGameView_OnEvent` | `0x0041acf0` |
| `CGameView_GetWorldCollisionRect` | `0x00417210` |
| `CGameView_GetSpatialBucketKey` | `0x00416f50` |
| `CBulanekCtor` | `0x0041e4b0` |

## Ghidra apply

Slice **01** (2026-05-30): `create_struct CGameView` 152 B; `CGameView_InitGamingFields@0x00416590`.

**Agent todo 3** (2026-05-30):

- Removed stale **1 B** `CGameView` placeholder; recreated **`CGameView` 152 B** (30 fields, `ODSImage` @ `+0x88`).
- `create_pointer_type` → **`CGameView *`**.
- `set_function_prototype` — `CGameView_ctor@0x004191a0` → **`CGameView * __thiscall …(CGameView *this, …)`** (return was `CBitmap *`).
- `set_function_prototype` + `set_parameter_type` — `CGameView_InitGamingFields@0x00416590` → **`CGameView *`** param (field writes `bView_state_*`, `bGaming_slot_id`, `gaming_host`).
- `set_function_prototype` — `CGameView_Update@0x00419010` → **`CGameView *this`**.
- `save_program bulanci.exe`.

Subclasses (`CBulanek`, `CShot`) replace `+0x88` tail — do not force single type on all instances.

**Agent todo 5 (2026-05-30):** Named header band `+0x28..+0x67` on **`CGameView`** / **`CBitmap`**: `wViewFlags`/`wPad_16`/`dwField_1c` @ `+0x14..+0x1f`; `pChain_pad_48`, `pHeader_tail_58`, `view_flags`; `bView_flag_6b`/`dwView_aux_6c` @ `+0x6b..+0x6f` (aux read in `GetSpatialBucketKey`). `save_program bulanci.exe`.

**Agent todo 4 r3 (2026-05-30):** `set_function_this_type` → **`CGameView *`** @ `CGameView_InitGamingFields@0x00416590`, `CGameView_GetSpatialBucketKey@0x00416f50`, `CGameView_OnEvent@0x0041acf0`; `force_decompile`: `this->dwView_aux_6c + this->nSpatial_bucket_y`; OnEvent `this->gaming_host` / `this->bGaming_slot_id`. `CAnim_ctor` / `CAnim_RenderAnimFrame` plated **`CAnim *`** separately. `save_program bulanci.exe`.

## UNK

- `+0x08..+0x0F` — chain-header mirror (`dwPad_08`/`dwPad_0c` in Ghidra).
- `pPad_28` (`+0x28..+0x2f`), `dwField_40`, `pChain_pad_48`, `pHeader_tail_58` — no ctor init.
- `dwView_aux_6c` @ `+0x6c` — read in `GetSpatialBucketKey@0x00416f50`; not zeroed in `CGameView_InitGamingFields`.

## Follow-up

- ~~Typed decompile for `GetSpatialBucketKey` / `CGameView_OnEvent` with **`CGameView *`** params.~~ **Done (agent todo 3 r2, 2026-05-30)** — `CGameView_GetSpatialBucketKey` shows `this->dwView_aux_6c` / `pPad_28`; `CGameView_OnEvent` uses `eventId` / `outParam`; `CGameView_InitGamingFields` **`CGameView *this`** (`__fastcall`, `_Globals::` listing prefix only).
- `gaming_host` still `undefined *` in decompile for `OnEvent` slot paths — type as **`CGaming *`** when `CGaming` layout stable (todo 12/14 chain).
