# CPoemScroller

## Status

**PARTIAL** — heap size **`0x128` (296 B)** verified; menu poem carousel tail (`poemSlots`, scroll state, text surface, fade ramps) mapped. **`CDSView` prefix `+0x00..+0x67`** partially padded (only bounds + vtable slots proven). Shared **`BlitDispatch`** engine at `g_pApp+0x80` documented elsewhere (`CDSImage.md`, `status.md`).

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CPoemScroller) == 0x128` | `CPoemScroller_CreateObject@0x004265b0` | `OperatorNewWithBadAlloc(0x128)` → `CPoemScroller_Constructor` |
| Dtor span | `CPoemScroller::Destructor@0x00425285` | `CDSView_dtor` after tail teardown through `+0x118` vector |
| Embedded `CDSImage` | `CPoemScroller_Constructor@0x004262c0` | `CDSImage_InitDefaults(&fadePlane)` at `this+0xb8` (`0x60` bytes → ends `0x118`) |
| Scheduler embed | `CPoemScroller_Constructor@0x004262c0` | `CDSUpdatedItem_ctor(&scheduler)` at `+0x68`; tick `OnScrollTick@0x00426030` uses `this-0x68` |

## Class registry

| Claim | Address | Evidence |
|-------|---------|----------|
| `GetClassIdentifier` | `CPoemScroller::GetClassIdentifier@0x00425da0` | `return &DAT_004b3998` |
| **classId = 2044 (`0x7fc`)** | static init @ `0x0047c1b0` region | `HandleClassRegister` — factory `0x426570`, meta `0x4b3998` (plate @ `FUN_0047e440` / `_atexit` stub cluster) |
| Factory | `CPoemScroller_CreateObject@0x004265b0` | `OperatorNew(0x128)` + ctor |
| Menu embed | `CMenu_ctor_with_ui@0x00426b78` | Stack / sibling `CPoemScroller_Constructor` (see `main_menu.md` §9.3) — **live path**; factory `0x7fc` registered but menu builds in-place |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `pVftable_primary` | `CPoemScroller_Constructor@0x004262c0` → `g_pCPoemScroller_vftable_primary` |
| `0x04` | 4 | `void *` | `pVftable_IDSChained_04` | ctor secondary vtables |
| `0x08` | 4 | `int` | `dwField_08` | `CDSChained_ctor` |
| `0x0c` | 4 | `int` | `dwField_0c` | `CDSChained_ctor` |
| `0x10` | 4 | `void *` | `pVftable_IDSEventHandler` | ctor |
| `0x14` | 2 | `ushort` | `wViewFlags` | ctor / `CDSView` family |
| `0x18` | 4 | `void *` | `pVftable_field18` | ctor |
| `0x20` | 4 | `int` | `nBounds_left` | ctor `= 10` |
| `0x24` | 4 | `int` | `nBounds_top` | ctor `= 0x180` |
| `0x28` | 4 | `int` | `nBounds_right` | ctor `= 0xd7` |
| `0x2c` | 4 | `int` | `nBounds_bottom` | ctor `= 0x226` |
| `0x30` | 36 | — | `pad_30_67` | UNK — chain / parent band (see `CGameView.md`) |
| `0x68` | 24 | `CDSUpdatedItem` | `scheduler` | `CDSUpdatedItem_ctor`; `Scheduler_RegisterEventSlot(..., 0x78, 6)` — **120 ms** scroll tick |
| `0x80` | 8 | `CDSPtrSlotVec` | `poemSlots` | ctor zero; `CIntListInsertSortedOrAppend` for each `CPoem` |
| `0x88` | 4 | `int` | `cPoemCount` | `CPoemScroller_PickNextPoem@0x00425df0` `*(this+0x88)` |
| `0x8c` | 4 | `int` | `cPoemCapacity` | ctor `= 8` |
| `0x90` | 4 | `int` | `iPoemIndex` | `PickNextPoem` writes chosen index |
| `0x94` | 4 | `int` | `fontSize` | ctor `= 0x10`; passed to `TextShaper_LayOutAndRender` |
| `0x98` | 4 | `int` | `fontFlags98` | ctor `= 1` |
| `0x9c` | 4 | `int` | `fontFlags9c` | ctor `= 1` |
| `0xa0` | 4 | `int *` | `pFontColorPair` | ctor `= &nTextColorFg` |
| `0xa4` | 4 | `int` | `field_a4` | ctor `= -1` |
| `0xa8` | 4 | `void *` | `pBackgroundRes` | ctor loads resource **`0x100af`** |
| `0xac` | 4 | `int` | `scrollY` | `PickNextPoem` init; `OnScrollTick` decrements via event slot `+0x44` |
| `0xb0` | 4 | `int` | `scrollEndY` | `PickNextPoem` `= -bbox.height`; tick compares slot `+0x48` |
| `0xb4` | 4 | `void *` | `pTextSurface` | `PickNextPoem` — `CDSObject_CtorWithImage` raster target |
| `0xb8` | 96 | `CDSImage` | `fadePlane` | `CDSImage_InitDefaults`; `CDSImage__Allocate` in `PickNextPoem` |
| `0x118` | 4 | `void *` | `pAlphaRampTop` | `PickNextPoem` `malloc(stride*0x28)` |
| `0x11c` | 4 | `void *` | `pAlphaRampBottom` | same |
| `0x120` | 4 | `int` | `dwTextColorFg` | ctor `0` / `0xffffffff` init |
| `0x124` | 4 | `int` | `dwTextColorBg` | ctor `0xb0b0b0` |

## Key methods

| Symbol | Address | Role |
|--------|---------|------|
| `CPoemScroller_Constructor` | `0x004262c0` | Build poem list, scheduler, first `PickNextPoem` |
| `CPoemScroller_CreateObject` | `0x004265b0` | Factory (`OperatorNew(0x128)`) |
| `CPoemScroller_PickNextPoem` | `0x00425df0` | Random poem, layout text, bake alpha ramps |
| `CPoemScroller_OnScrollTick` | `0x00426030` | **1 px / 120 ms** scroll; re-pick at end |
| `CPoemScroller_Render` | `0x004240f0` | Primary vtable slot 14 — draw scrolled text |
| `CPoemScroller::BlitDispatch` | `0x004368d0` | Shared compositor (`TM_TickBlit@0x00439080` on `g_pApp+0x80`) |
| `CPoemScroller::DrawSolidRect` | (alias of fill path) | Used by `CBlackView::OnDraw` (`movie_cinema_views.md`) |

## Ghidra apply

```
delete_data_type CPoemScroller
create_struct CPoemScroller → Size: 296 (32 fields; embeds CDSUpdatedItem, CDSPtrSlotVec, CDSImage)
get_struct_layout CPoemScroller
rename_function 0x004262c0 → CPoemScroller_Constructor
rename_function 0x004265b0 → CPoemScroller_CreateObject
set_function_prototype CPoemScroller_PickNextPoem@0x00425df0
set_function_prototype CPoemScroller_OnScrollTick@0x00426030
save_program bulanci.exe
```

Decompile of `CPoemScroller_Constructor` after apply shows typed fields (`param_1->poemSlots`, `param_1->nBounds_left`, etc.).

## UNK

- `pad_30_67` — full `CDSChained` / `CDSView` child-chain band (`+0x30..+0x67`).
- ~~`CPoemScroller` classId in `HandleClassRegister` block~~ — **closed R5 w45:** **classId `0x7fc` (2044)** @ static init `0x0047c1b0`; factory `CPoemScroller_CreateObject@0x004265b0` (`0x426570` push).
- Exact `TextShaper` font descriptor layout at `+0x94` (three dwords + color pair pointer).

## Parent / consumers

- **`CMenu`** — credits scroller sibling in main menu UI (`main_menu.md`).
- **`g_pApp+0x80`** — engine singleton subobject hosting **`BlitDispatch`** for the whole app (`status.md`, `anim_runtime.md`).
- **`CPoem`** — elements in `poemSlots` (`CPoem.md`).
