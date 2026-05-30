# CColorSet

## Status

**PARTIAL** — `sizeof == 0x70`; Ghidra type **`CColorSet` (112 B)** applied (slice 05). Post-match lobby **slot pillar** widget (`resource 0x10013`); team tint @ `+0x69`, 4×2 color grid hit-test @ `CColorSet_OnMouseDown`.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CColorSet) == 0x70` | `0x0040e9f0` | `CColorSet_Allocate`: `OperatorNewWithBadAlloc(0x70)` → `CDSChained_ctor` + vtables `0x480914` / `0x4808f8` / `0x4808e0` / `0x4808cc` |
| In-place ctor (no separate heap) | `0x0040ffd0` | `CColorSet_ctor_slotPillar`: `CDSChained_ctor(this)` then same vtable patch + bitmap load |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `void *` | `pVftable_primary` | `CColorSet_Allocate@0x0040e9f0` `=0x480914`; `CColorSet_ctor_slotPillar@0x0040ffd0` |
| `+0x04` | 4 | `void *` | `pVftable_IDSChained` | `=0x4808f8` |
| `+0x10` | 4 | `void *` | `pVftable_IDSReferenced` | `=0x4808e0` |
| `+0x18` | 4 | `void *` | `pVftable_IDSEventHandler` | `=0x4808cc` |
| `+0x14` | 2 | `ushort` | `wViewFlags` | `CColorSet_ctor_slotPillar` `\|= 0x10` |
| `+0x20` | 4 | `int` | `nOrigin_x` | ctor `=param_1` (x) |
| `+0x24` | 4 | `int` | `nOrigin_y` | ctor `=param_2` (y) |
| `+0x28` | 4 | `int` | `nInner_w` | ctor: bitmap width `+2` + origin |
| `+0x2c` | 4 | `int` | `nInner_h` | ctor: bitmap height `+2` + origin |
| `+0x30` | 16 | 4×`int` | `nBounds_*` | `CColorSet_Render@0x0040aed0` blit rect; `CColorSet_OnMouseDown@0x0040f580` hit test |
| `+0x44` | 4 | `uint` | `dwView_flags` | `CColorSet_Render`: `(byte)dwView_flags & 2` → tint |
| `+0x46` | 2 | `ushort` | `wWidget_flags` | ctor `\|= 1` (overlaps high bytes of `+0x44`) |
| `+0x4c` | 4 | `void *` | `pParent_view` | `CColorSet_SetColorIndex@0x0040cc60` event parent `+0x10` |
| `+0x68` | 1 | `byte` | `bColorIndex` | ctor `=0xff`; `SetColorIndex` clamp `0..7`; highlight when `!=0xff` in `Render` |
| `+0x69` | 1 | `byte` | `bTeamTint` | ctor `=param_3`; `SetColorIndex` broadcasts `CONCAT11(bColorIndex,bTeamTint)` |
| `+0x6c` | 4 | `void *` | `pPillarBitmap` | ctor loads res `0x10013`; `Render` / `OnMouseDown` |

## Key methods

| Address | Symbol | Role |
|---------|--------|------|
| `0x0040ffd0` | `CColorSet_ctor_slotPillar` | Lobby pillar factory (was `CMenu::…`) |
| `0x0040aed0` | `CColorSet_Render` | `BlitDispatch` + 4×2 selection rect |
| `0x0040cc60` | `CColorSet_SetColorIndex` | Slot color commit + `0xd1` event |
| `0x0040f580` | `CColorSet_OnMouseDown` | Maps click → grid index 0..7 |
| `0x0040e9f0` | `CColorSet_Allocate` | Heap shell for non-embedded instances |

## Ghidra apply

Slice 05: deleted 1-byte placeholder; `create_struct CColorSet` (112 B). `get_struct_layout CColorSet` → size 112. Prototypes: `CColorSet_Render`, `CColorSet_SetColorIndex`, `CColorSet_Allocate`, `CColorSet_ctor_slotPillar` (`undefined1 teamTint`). `save_program bulanci.exe`.

## UNK

- `+0x08..+0x0f`, `+0x1c`, `+0x40..+0x43`, `+0x48..+0x4b`, `+0x50..+0x67` — no dedicated ctor writes (Ghidra pads).
- `+0x44` / `+0x46` overlap — treat as packed view/widget flag bytes.
- Primary vtable slots beyond `Render` / `SetColorIndex` / `LoadData` — see `master_vtable_catalog.csv` (`CColorSet` @ `0x00480914`).

## Notes

- Sibling **`CColorSwitch`** (`0x0040f060` region) uses palette table @ `+0x6c` and swatch atlas `0x10014` — different widget, same CDSView prefix family.
- Consumed from **`CStartGame2`** slot builder (`post_match_lobby.md` §4).
