# CColorSet

## Status

**VERIFIED** — `sizeof == 0x70` (112 B); **byte-identical layout to [CWindow.md](./CWindow.md)** with tail overlay @ `+0x68..+0x6c` for lobby color pillar (`BitmapSpecial` **`0x10013`**). Post-match team-setup slot widget in `CStartGame2_ctor`. Team slot index @ `+0x69` (`bTeamTint`); swatch index @ `+0x68` (`bColorIndex`). Does **not** call `CGame_BuildTeamPaletteLut` (anim palette only in same slot loop).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CColorSet) == 0x70` | `0x0040e9f0` | `CColorSet_Allocate`: `OperatorNewWithBadAlloc(0x70)` → `CDSChained_ctor` + vtables `0x480914` / `0x4808f8` / `0x4808e0` / `0x4808cc` |
| Same size in lobby ctor loop | `0x00410657` | `CStartGame2_ctor`: `OperatorNewWithBadAlloc(0x70)` → `CColorSet_ctor_slotPillar` |
| Ghidra struct size | — | `get_struct_layout CColorSet` → **112 bytes** (matches `CWindow`) |
| Disasm ↔ layout | `0x0040aed0` | `[ESI+0x44]` tint flags; `[ESI+0x30..0x3c]` blit rect; `[ESI+0x68]` color index; `[ESI+0x6c]` bitmap |

## Layout table

**Prefix `+0x00..+0x67`:** same offsets/names as `CWindow` ([CWindow.md](./CWindow.md)). **Tail overlay** reuses dialog modal/focus bytes for pillar state.

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `void *` | `pVftable_primary` | `CColorSet_Allocate@0x0040e9f0` `=0x480914`; `CColorSet_ctor_slotPillar@0x0040ffd0` |
| `+0x04` | 4 | `void *` | `pVftable_IDSChained` | `=0x4808f8` |
| `+0x08` | 4 | `uint` | `dwChainField_08` | `CDSChained_ctor` |
| `+0x0c` | 4 | `uint` | `dwChainField_0c` | chain base |
| `+0x10` | 4 | `void *` | `pVftable_IDSReferenced` | `=0x4808e0` |
| `+0x14` | 2 | `ushort` | `wViewFlags` | ctor `\|= 0x10` @ `0x0040ffd0` |
| `+0x16` | 2 | `ushort` | `wPad_16` | padding |
| `+0x18` | 4 | `void *` | `pVftable_IDSEventHandler` | `=0x4808cc` |
| `+0x1c` | 4 | `uint` | `dwField_1c` | `CDSChained_ctor` zero |
| `+0x20` | 4 | `int` | `nBbox_left` | ctor `= x`; `OnMouseDown@0x0040f580` grid origin |
| `+0x24` | 4 | `int` | `nBbox_top` | ctor `= y` |
| `+0x28` | 4 | `int` | `nBbox_right` | ctor: bitmap width `+2` + left; hit-test inner width |
| `+0x2c` | 4 | `int` | `nBbox_bottom` | ctor: bitmap height `+2` + top |
| `+0x30` | 4 | `int` | `nScreenBbox_left` | `Render@0x0040aed0` blit/highlight rect |
| `+0x34` | 4 | `int` | `nScreenBbox_top` | same |
| `+0x38` | 4 | `int` | `nScreenBbox_right` | same |
| `+0x3c` | 4 | `int` | `nScreenBbox_bottom` | same |
| `+0x40` | 4 | `dword` | `dwChainHead_40` | `CDSChained_ResetChainCounters` (shared shell; no pillar-specific writes) |
| `+0x44` | 2 | `ushort` | `wViewStateFlags` | `Render`: `WidgetStateFlags_ToTintColor((byte)wViewStateFlags & 2)` |
| `+0x46` | 2 | `ushort` | `wWidgetFlags` | ctor `\|= 1` |
| `+0x48` | 2 | `ushort` | `wChainCounter_48` | chain band (inherits `CWindow`; no pillar ctor writes) |
| `+0x4a` | 2 | `ushort` | `wChainCounter_4a` | same |
| `+0x4c` | 4 | `void *` | `pParent` | `SetColorIndex@0x0040cc60` → `pParent+0x10` for event dispatch |
| `+0x50` | 4 | `dword` | `dwChainField_50` | `CDSChained_ctor` / reset |
| `+0x54` | 4 | `void *` | `pVftable_CDSChain_IDSReferenced` | chain MI |
| `+0x58` | 4 | `void *` | `pVftable_CDSChain_IDSChained` | chain MI |
| `+0x5c` | 4 | `dword` | `dwField_5c` | ctor zero |
| `+0x60` | 4 | `dword` | `dwField_60` | ctor zero |
| `+0x64` | 4 | `void *` | `pOverlapEntity` | ctor zero (gameplay-only on other views) |
| `+0x68` | 1 | `byte` | `bColorIndex` | ctor `=0xff`; `SetColorIndex` clamp `0..7`; highlight when `!=0xff` in `Render` |
| `+0x69` | 1 | `byte` | `bTeamTint` | ctor `= teamTint` (**lobby slot index** `0..3` from `CStartGame2_ctor` loop); event `0xd1` high byte |
| `+0x6a` | 2 | `byte[2]` | `pPad_6a` | padding before pointer |
| `+0x6c` | 4 | `void *` | `pPillarBitmap` | ctor loads res `0x10013`; overlays `CWindow.pDefaultFocusChild`; `Render` tint @ `(char*)(bitmap+0x18)` |

### Tail overlay vs `CWindow`

| `CWindow` @ | `CColorSet` @ | Role |
|-------------|---------------|------|
| `bModalFlag` `+0x68` | `bColorIndex` | `0xff` = no swatch highlight |
| `pPad_69` `+0x69..+0x6b` | `bTeamTint` + `pPad_6a` | slot index + pad |
| `pDefaultFocusChild` `+0x6c` | `pPillarBitmap` | pillar `CDSBitmap` / image; released in ctor on reload |

## `bTeamTint` vs `CGameGetPlayerColorByte`

| Path | Address | Payload |
|------|---------|---------|
| Local click → `SetColorIndex` | `0x0040cc60` | Updates `bColorIndex`; broadcasts `0xd1` with `CONCAT11(bColorIndex, bTeamTint)` |
| Network/custom `0xde` | `0x0040d6f2` | `CGameGetPlayerColorByte(pParentState, slot)` → `SetColorIndex(color)` — **does not read `bTeamTint`** |
| Lobby slot ctor | `0x00410657` | `teamTint` loop counter → `bTeamTint`; separate `CGameGetPlayerColorByte` @ `0x00410808` feeds **CBulAnim** LUT only |

`bTeamTint` is the **lobby player-slot index** (matches `CBulAnim::bTeamId` on event `0xd1`); `CGameGetPlayerColorByte` reads **`player*0x23+0xdc`** in game state (palette row / color byte).

## Palette / LUT builders (evidence)

`CColorSet` has **no xref** to `CGame_BuildTeamPaletteLut` (`0x00413c50`) or `CGame_BuildPaletteLut` (`0x00413220`).

| Helper | Address | Role relative to `CColorSet` |
|--------|---------|------------------------------|
| `WidgetStateFlags_ToTintColor` | `0x00402f40` | `Render`: `(wViewStateFlags & 2)` → RGB tint |
| `CGame_BuildTeamPaletteLut` | `0x00413c50` | Lobby: `CStartGame2_ctor` after pillar — **CBulAnim** track only |
| `CGame_BuildPaletteLut` | `0x00413220` | Default LUT remap |
| `CGame_CopyDefaultPaletteSeed` | `0x00413200` | Per-player seed |

Lobby integration (one slot in `CStartGame2_ctor`):

```
OperatorNew(0x70) → CColorSet_ctor_slotPillar(x, 10, slotIndex)
AddChild(pillar)
… CBulAnim …
CGame_BuildTeamPaletteLut(stack, CGameGetPlayerColorByte(...))   // anim only
```

Pillar tint at draw: `Render` sets `*(char*)(pPillarBitmap+0x18) = tint` before `BlitDispatch`.

## Key methods

| Address | Symbol | Role |
|---------|--------|------|
| `0x0040ffd0` | `CColorSet_ctor_slotPillar` | Lobby pillar factory; load `0x10013` |
| `0x0040aed0` | `CColorSet_Render` | Tint + `BlitDispatch` + 4×2 selection outline |
| `0x0040cc60` | `CColorSet_SetColorIndex` | Grid index `0..7`; event `0xd1` |
| `0x0040f580` | `CColorSet_OnMouseDown` | 20×15 px cells → `SetColorIndex` |
| `0x0040ccc0` | `CColorSet_LoadData` | `SetColorIndex(*param_1)` |
| `0x0040e9f0` | `CColorSet_Allocate` | Heap shell (`StaticClassRegister` @ `0x0047b650`) |
| `0x0040d6f2` | `CStartGame2_OnCustomMsg` case `0xde` | `CGameGetPlayerColorByte` → `SetColorIndex` on `pSlotColorSet[slot]` |

## Ghidra apply

**R4:** `bViewStateFlags` @ `+0x44`; palette/LUT comments; `save_program`.

**R5 worker 39 (2026-05-30):** Rebuilt `CColorSet` layout = `CWindow` + tail overlay; added `dwChainHead_40` @ `+0x40`; renamed bbox/screen/chain fields; `set_function_this_type` `CColorSet *` on ctor/render/mouse; `CColorSet_ctor_slotPillar` decompile uses `nBbox_*` / `bColorIndex` / `pPillarBitmap`; `save_program bulanci.exe`.

**R5 worker 42 (2026-05-30):** Re-confirmed `CColorSet *` on `CColorSet_ctor_slotPillar@0x0040ffd0` / `CColorSet_Render@0x0040aed0`; plate comment on ctor. See [round5_worker_42_report.md](./round5_worker_42_report.md) (C–D UNK sweep).

## UNK

- Semantics of `dwField_08` / `dwField_0c` / `dwField_30..3c` / `dwField_5c..60` beyond `CDSChained_ctor` zero/init (shared with all `CWindow` children).
- Whether `nScreenBbox_*` are ever updated independently of `nBbox_*` after ctor (no xref found in pillar methods).

## Follow-up

- Sibling **`CColorSwitch`** — swatch atlas `0x10014`, palette buffer @ `+0x6c` (different widget).
- Primary vtable `0x00480914` — `master_vtable_catalog.csv`

## Notes

- Consumed from **`CStartGame2`** (`post_match_lobby.md` §4).
- Vtable primary slot 14 = `CColorSet_Render`; slot 19 = `CColorSet_OnMouseDown`.
