# CBitmap

## Status

**PARTIAL** — Ghidra type **`CBitmap` (152 B / `0x98`)** applied (follow-up round 2). Same layout as **`CGameView`** heap allocation (`CreateImage` → `CGameView_ctor`); `CBitmap` is the vtable identity patched in that ctor. Header through `gaming_host` matches `CAnim`; tail is embedded **`ODSImage`** at `+0x88` (`TM_TickBlit(this+0x88)`).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CBitmap view) == 0x98` | `0x0041d820` | `CLevelScriptOpExt_CreateImage`: `OperatorNewWithBadAlloc(0x98)` → `CGameView_ctor` (patches `CBitmap` vtables) |
| Same layout in `CAnim` base | `0x00419070` | **`CBitmap_ViewHeader_Init`**: `CDSChained_ctor` + `CBitmap` vtables + `SetOwner(ods_image, this)` — shared MI header before `CAnim` extension |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `pointer` | `vftable_primary` | `CGameView_ctor@0x004191a0` then `CBitmap::g_pCBitmap_vftable_primary` |
| `+0x04` | 4 | `pointer` | `vftable_IDSChained` | `CGameView_ctor@0x004191a0` / `CBitmap_ViewHeader_Init@0x00419070` |
| `+0x10` | 4 | `pointer` | `vftable_IDSReferenced` | `CGameView_ctor@0x004191a0` (`…IDSReferenced_18`) |
| `+0x14` | 2 | `ushort` | `wViewFlags` | `CDSView_SetRect@0x0042c480` |
| `+0x18` | 4 | `pointer` | `vftable_IDSEventHandler` | `CGameView_ctor@0x004191a0` (`…IDSEventHandler_10`) |
| `+0x1C` | 4 | `uint` | `dwField_1c` | `CDSChained` mirror (same as `CAnim`) |
| `+0x20` | 4 | `int` | `origin_x` | `CGameView_ctor@0x004191a0` |
| `+0x24` | 4 | `int` | `origin_y` | `CGameView_ctor@0x004191a0` |
| `+0x28` | 8 | `byte[8]` | `pPad_28` | gap before bounds; `+0x2c` read in `GetSpatialBucketKey@0x00416f50` |
| `+0x30` | 16 | `RECT` (4×`int`) | `bounds` | `CDSView_SetRect@0x0042c480` (`nBounds_*` in Ghidra) |
| `+0x40` | 4 | `uint` | `dwField_40` | no direct consumer |
| `+0x44` | 4 | `uint` | `view_flags` | `CDSView_SetRect@0x0042c480` |
| `+0x48` | 12 | `byte[12]` | `pChain_pad_48` | before `child_chain` |
| `+0x54` | 4 | `pointer` | `child_chain` | `CDSView_SetRect@0x0042c480` |
| `+0x58` | 16 | `byte[16]` | `pHeader_tail_58` | opaque tail through `+0x67` |
| `+0x68` | 1 | `byte` | `view_state_68` | `CGameView_InitGamingFields@0x00416590` |
| `+0x69` | 1 | `byte` | `view_state_69` | `FUN_00416590@0x00416590` |
| `+0x6a` | 1 | `byte` | `view_state_6a` | `FUN_00416590@0x00416590` |
| `+0x70` | 1 | `byte` | `gaming_slot_id` | `FUN_00416590@0x00416590` init `0xff`; `CLevelScript_FireOnBitmapEvt_FromView@0x00418260` reads `*(byte*)(param_1+0x70)` where `param_1` is view base (`CBitmap_FireOnBitmapEvtFromView@0x00419280`: `this-0x8c`) |
| `+0x74` | 4 | `int` | `field_74` | `CGameView_ctor@0x004191a0` `=0` |
| `+0x78` | 4 | `int` | `field_78` | `CGameView_ctor@0x004191a0` `=0` |
| `+0x7c` | 4 | `int` | `field_7c` | `CGameView_ctor@0x004191a0` `=0` |
| `+0x80` | 4 | `int` | `field_80` | `CGameView_ctor@0x004191a0` `=0` |
| `+0x84` | 4 | `CGaming *` | `gaming_host` | `FUN_00416590@0x00416590` `=0`; `CGameView_Update@0x00419010` (`CGaming_InsertEntityByDepth`) |
| `+0x88` | 16 | `ODSImage` | `ods_image` | `ODSImage_ctor@0x00418c00` in `CGameView_ctor`; `CBitmap_OnDraw@0x00419130` `TM_TickBlit(this+0x88)` |
| `+0x8c` | 4 | `void *` | `ods_image.pVf_odsimage` (IDSAnim face) | `CGameView_ctor@0x004191a0` `MOV [ESI+0x8c],0x481f00` after `ODSImage_ctor` (default `0x481ab4` at `ODSImage+4`); `CBitmap_FireOnBitmapEvtFromView@0x00419280` — `ECX`=view`+0x8c`, `ADD ECX,-0x8c` → view base |

## IDSAnim vtable at `+0x8c`

| Item | Address | Evidence |
|------|---------|----------|
| `CBitmap::vftable` (IDSAnim, 5 slots) | `0x00481f00` | `CGameView_ctor@0x00419240`; `master_vtable_catalog.csv` — `[4]=CBitmap_FireOnBitmapEvtFromView@0x00419280` (FLX opcode **0x0C** FrameTimeHint subscriber via `BroadcastFrameTimeHint`) |
| Default `ODSImage::pVf_odsimage` before repatch | `0x00481ab4` | `ODSImage_ctor@0x00418c2a` `MOV [ESI+4],0x481ab4` |
| Thunk `this` adjustment | `0x00419280` | `ADD EAX,-0x8c` before `JMP CLevelScript_FireOnBitmapEvt_FromView`; reads `byte [this-0x1c]` → `gaming_slot_id@+0x70`, `[this-0x8]` → `gaming_host@+0x84` |

| Slot | Target @ `0x00481f00` | Role |
|------|----------------------|------|
| `[0]` | `IDSAnim_NotifyEvents@0x00438f20` | 16-byte event batch → `IDSEventHandler+0x24` |
| `[1]` | `IDSAnim_BindUserData@0x00438f60` | `FUN_0042cc30` on chained face |
| `[2]` | `IDSAnim_SetSequence_thunk@0x004391d0` | `ODSImage__SetImage` |
| `[3]` | `CDSView_EmptyHook27@0x00438f80` | no-op |
| `[4]` | `CBitmap_FireOnBitmapEvtFromView@0x00419280` | FLX `0x0C` → `eventCode` for `OnBitmapEvt` (R4 task 31) |

Same **IDSAnim** interface family as `CAnim` at `+0x8c` (`0x00482008` in `anim_runtime.md`); `CBitmap` uses the shorter `0x98`-byte view layout (no `+0x98` anim-inner subobject).

## Ghidra apply

**Slice 03 (2026-05-30):** Verified **`CBitmap` (152 B)** — `ODSImage ods_image` @ `+0x88`; `get_struct_layout CBitmap` → Size 152. Prototype `CGameView_ctor@0x004191a0` as `CBitmap * __thiscall`. Renamed shared header init **`CBitmap_ViewHeader_Init@0x00419070`** (was `FUN_00419070`). `save_program bulanci.exe` (with `CAnim` rebuild in same slice).

**Agent todo 5 (2026-05-30):** Header band `+0x28..+0x67` field names aligned with `CAnim`/`CGameView` (`pChain_pad_48`, `pHeader_tail_58`, `view_flags`, `bView_flag_6b`, `dwView_aux_6c`). `save_program bulanci.exe`.

**Agent todo 15 (2026-05-30):** `gaming_host` @ `+0x84` → **`CGaming *`** (pairs with `CAnim.pGaming_host`). `CGameView_Update` prototype refreshed @ `0x00419010`.

## CGameView alias

`CLevelScriptOpExt_CreateImage@0x0041d820` allocates `0x98` and calls `CGameView_ctor` — same memory layout as **`CBitmap`** documented here (no separate heap type).

## UNK

- `+0x08..+0x0f` — not written in `CGameView_ctor` / `FUN_00419070`.
- `pPad_28`, `dwField_40`, `pChain_pad_48`, `pHeader_tail_58` — named pads; no ctor init (see `CGameView.md`).
- `OnBitmapEvt` **`eventCode`** constants per map (script layer; not in `.exe`).

## Follow-up

- `+0x8c` / IDSAnim vtable `0x00481f00` — proven in layout + **IDSAnim vtable** sections above (`CGameView_ctor`, `CBitmap_FireOnBitmapEvtFromView`).

## Notes

- Script `OnBitmapEvt` export (`CLevelScript_FireOnBitmapEvt_FromView`) uses **`this+0x344`** on the **script host** (`CBulanci`), not on the view — only the slot byte at view `+0x70` is view-relative.
- `CBitmap_FireOnBitmapEvtFromView` runs on an **`IDSAnim` thunk** (`this` = view`+0x8c`); checks `this[-0x1c]` and outer `this[-8]` visibility (`+0x44` on outer) before firing.
