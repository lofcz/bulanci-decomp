# CScrollBar

## Status

**VERIFIED** — Ghidra struct **204 B** (`0xcc`); full `CDSChained` prefix `+0x00..+0x67`, `CDSUpdatedItem` scheduler `+0x68`, nine `CDSImage*` state bitmaps `+0x80..+0xa0`, scroll metrics `+0xa4..+0xc8`. Canonical UI semantics: [widgets.md](../../widgets.md) §12.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| Heap allocation `0xcc` | `CScroller_BuildAt@0x004089c0` | `OperatorNewWithBadAlloc(0xcc)` before each `CScrollBar_BuildAt` |
| Placement ctor footprint | `CScrollBar_BuildAt@0x00408370` | Writes through `+0xc8`; `eh_vector_constructor` 9×4 B @ `+0x80` |
| Subclass tail | `CVolume` | `get_struct_layout` → **208 B** = `CScrollBar` @ 0 + `audio_preview` @ 204 |
| Factory path | `CVolume_Allocate@0x0040f140` | `CScrollBar_ctor` on `malloc` block (same layout as `BuildAt` init band) |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `void *` | `pVf_primary` | `CScrollBar_ctor@0x00404790`, `CScrollBar_BuildAt@0x004083cc` → `0x0047fbdc` |
| `+0x04` | 4 | `void *` | `pVf_IDSChained` | ctor / BuildAt → `0x0047fbbc` |
| `+0x08` | 4 | `uint` | `dwField_08` | `CDSChained_ctor` zero; shared shell ([CDSChained.md](./CDSChained.md)) |
| `+0x0c` | 4 | `uint` | `dwField_0c` | same |
| `+0x10` | 4 | `void *` | `pVf_IDSEventHandler` | ctor / BuildAt → `0x0047fb90` |
| `+0x14` | 2 | `ushort` | `wViewFlags` | `BuildAt@0x004084b8` `\|= 0x38`; `SetScrollRange@0x004051d0` show/hide |
| `+0x16` | 2 | `ushort` | `wPad_16` | padding |
| `+0x18` | 4 | `void *` | `pVf_field18` | ctor / BuildAt → `0x0047fba4` |
| `+0x1c` | 4 | `uint` | `dwField_1c` | `CDSChained_ctor` zero |
| `+0x20` | 4 | `int` | `nBbox_left` | `BuildAt@0x0040848f` copies ctor rect |
| `+0x24` | 4 | `int` | `nBbox_top` | same |
| `+0x28` | 4 | `int` | `nBbox_right` | same (adjusted by top-button size) |
| `+0x2c` | 4 | `int` | `nBbox_bottom` | same |
| `+0x30` | 4 | `int` | `nScreenBbox_left` | `CScrollBar_Render@0x004035ad` blit origin |
| `+0x34` | 4 | `int` | `nScreenBbox_top` | same |
| `+0x38` | 4 | `int` | `nScreenBbox_right` | same |
| `+0x3c` | 4 | `int` | `nScreenBbox_bottom` | same |
| `+0x40` | 4 | `uint` | `dwChainRoot` | `CDSChained_ResetChainCounters` |
| `+0x44` | 2 | `ushort` | `wChainInit44` | `ResetChainCounters` `= 1`; `Render@0x00403537` `WidgetStateFlags_ToTintColor(wChainInit44 & 6)` |
| `+0x46` | 2 | `ushort` | `wChainFlag46` | same |
| `+0x48` | 2 | `ushort` | `wChainFlag48` | `CScroller_BuildAt` sets child cmd `0xd`/`0xe` on scrollbar `+0x48` |
| `+0x4a` | 2 | `ushort` | `wChainFlag4a` | chain band |
| `+0x4c` | 4 | `void *` | `pParent` | `CScrollBar_SetValue@0x00404f70` notify parent |
| `+0x50` | 4 | `uint` | `dwField_50` | ctor zero only on shell |
| `+0x54` | 4 | `void *` | `pVftable_CDSChain_IDSReferenced` | `CDSChained_ctor` |
| `+0x58` | 4 | `void *` | `pVftable_CDSChain_IDSChained` | same |
| `+0x5c` | 4 | `uint` | `dwField_5c` | ctor zero |
| `+0x60` | 4 | `uint` | `dwField_60` | ctor zero |
| `+0x64` | 4 | `void *` | `pOverlapEntity` | ctor zero (gameplay views only; not scrollbar path) |
| `+0x68` | 24 | `CDSUpdatedItem` | `scheduler` | `CDSUpdatedItem_ctor@0x004047cb`; `Scheduler_RegisterEventSlot(...,7)@BuildAt` |
| `+0x80` | 4 | `CDSImage *` | `pBitmap_topNormal` | `BuildAt` load loop; `Render@0x0040354f` indexed by `bPackedState & 3` |
| `+0x84` | 4 | `CDSImage *` | `pBitmap_topPressed` | BuildAt `bitmaps[1]` |
| `+0x88` | 4 | `CDSImage *` | `pBitmap_topDisabled` | BuildAt `bitmaps[2]` |
| `+0x8c` | 4 | `CDSImage *` | `pBitmap_thumbNormal` | `Render` thumb blit; `HitTest@0x00403860` size from `+0x8c` |
| `+0x90` | 4 | `CDSImage *` | `pBitmap_thumbPressed` | BuildAt |
| `+0x94` | 4 | `CDSImage *` | `pBitmap_thumbDisabled` | `CScrollBar_dtor@0x00404fe0` thumb-drag math |
| `+0x98` | 4 | `CDSImage *` | `pBitmap_bottomNormal` | `Render` bottom cap |
| `+0x9c` | 4 | `CDSImage *` | `pBitmap_bottomPressed` | BuildAt |
| `+0xa0` | 4 | `CDSImage *` | `pBitmap_bottomDisabled` | BuildAt |
| `+0xa4` | 1 | `byte` | `bPackedState` | `BuildAt` `&= 0xc0`; `Render` 2-bit groups for top/thumb/bottom |
| `+0xa8` | 4 | `int` | `nMinValue` | `BuildAt` / `SetScrollRange@0x004051d0` |
| `+0xac` | 4 | `int` | `nMaxValue` | same |
| `+0xb0` | 4 | `int` | `nCurrentValue` | `SetValue@0x00404f70`; `CSetupDlg_OnVolumeFocus@0x0040e5b0` (`CVolume` embed) |
| `+0xb4` | 4 | `int` | `nStep` | `BuildAt` `= 1` |
| `+0xb8` | 4 | `int` | `nPage` | `BuildAt` `= 0x14` |
| `+0xbc` | 1 | `byte` | `bOrientation` | `BuildAt` aspect test; `Render` / `HitTest` branch |
| `+0xbd` | 1 | `byte` | `bDragging` | `OnMouseDown@0x00407460`; `dtor` commit when `nDragMode==3` |
| `+0xc0` | 4 | `int` | `nDragMode` | `HitTest` returns 1..5; `OnTimerTick` auto-repeat |
| `+0xc4` | 4 | `int` | `nDragStartOffset` | drag state |
| `+0xc8` | 4 | `int` | `nDragStartMouse` | drag state |

## Vtables (MSVC MI, five faces)

| Object offset | Vtable @ | Role | Notable slots |
|---------------|----------|------|----------------|
| `+0x00` | `0x0047fbdc` | `CDSView` primary | 14=`Render`, 16=`dtor`, 18=`OnTimerTick`, 19–20 mouse |
| `+0x04` | `0x0047fbbc` | `IDSChained` / `CDSChain` | adjustor dtor @+3 |
| `+0x10` | `0x0047fb90` | `IDSEventHandler` | |
| `+0x18` | `0x0047fba4` | secondary view facet | |
| `+0x68` (`scheduler+0`) | `0x0047fb78` | `IDSUpdated` on embed | slots 0–2 `CScroller` helpers; 4=`Invalidate` |

Full slot map: [vftable_methods.csv](../vftable_methods.csv) rows `CScrollBar;0047fb*`.

## Ctors / dtors

| Symbol | Address | Role |
|--------|---------|------|
| `CScrollBar_ctor` | `0x00404790` | Heap: `CDSChained_ctor` + `scheduler` + vtables + 9× default-constructed `CDSImage*` shells |
| `CScrollBar_BuildAt` | `0x00408370` | Placement: rect, optional `bitmaps[9]`, range defaults, `Scheduler_RegisterEventSlot(...,7)` |
| `CScrollBar_dtor` | `0x00404fe0` | Thumb-drag value commit; vector dtor bitmap band |
| `CScrollBar_vDtor` | `0x00406430` | Scalar deleting |
| `CScrollBar_GetTypeDescriptor` | `0x00404840` | RTTI |
| `CScrollBar_AdjustorThunk*_Dtor` | `0x00404850`–`0x00404880` | MI adjustors (`+4`, `+0x10`, `+0x18`, `+0x68`) |

## Consumers

| Caller | Address | Use |
|--------|---------|-----|
| `CScroller_BuildAt` | `0x004089c0` | `OperatorNew(0xcc)` ×2 → horizontal @ `+0x94`, vertical @ `+0x90` of `CScroller` |
| `CVolume_BuildAt` | `0x0040e1b0` | `CScrollBar_BuildAt` on embedded `CVolume.scrollbar` |
| `CVolume_Allocate` | `0x0040f140` | `CScrollBar_ctor` on factory buffer |
| `CreateObject` | `0x00406400` | Generic `CScrollBar_ctor` |
| `CSetupDlg_SetVolumeBinding` | `0x0040e590` | `CScrollBar_SetValue(pVolume, idx)` |
| `CSetupDlg_OnVolumeFocus` | `0x0040e5b0` | reads `pVolume→scrollbar.nCurrentValue` @ `+0xb0` |
| `_Globals::CScrollBar_SetValue` | `0x00404f70` | clamp `nCurrentValue`, invalidate, `FUN_0042ecc0` notify |
| `_Globals::CScrollBar_SetScrollRange` | `0x004051d0` | `nMinValue`/`nMaxValue`, show/hide when empty range |

## Subclasses

| Type | Size | Tail |
|------|------|------|
| `CVolume` | `0xd0` (208 B) | `pAudio_preview` @ `+0xcc` (204) — [CSetupDlg.md](./CSetupDlg.md), [CVolume via widgets.md §10](../../widgets.md) |

## Ghidra apply (R4 pass CScrollBar)

```
recreate_struct CScrollBar 204 B — CDSChained prefix + scheduler + 9× CDSImage* + metrics
recreate_struct CVolume — CScrollBar scrollbar @ 0; CDSAudioPlayer * pAudio_preview @ 204
set_function_this_type CScrollBar * on ctor/BuildAt/Render/HitTest/dtor/OnMouseDown
save_program bulanci.exe
```

`get_struct_layout CScrollBar` → **204 bytes**, 47 fields (2026-05-30).

## Ghidra apply (R5 worker 34)

```
set_function_this_type CScrollBar * @ Render/SetValue/SetScrollRange/OnMouseMove/OnTimerTick
set_function_prototype __thiscall on above + force_decompile
set_decompiler_comment @ 0x00403537 (wChainInit44 tint), @ 0x00404f70 (parent notify)
save_program bulanci.exe
```

Post-R5 decompile: `CScrollBar_Render` uses `this->nScreenBbox_*`, `pBitmap_*`, `nMinValue`/`nMaxValue`/`nCurrentValue`; `CScrollBar_SetValue`/`SetScrollRange` live in class `CScrollBar` (were `_Globals::`).

## UNK

- `dwField_08` / `dwField_0c` / `dwField_1c` — **R5 sweep:** no object-relative read/write in any `CScrollBar_*` body (`search_instructions` on Render/HitTest/BuildAt/SetValue/SetScrollRange/dtor); inherited `CDSChained_ctor` zero only ([CDSChained.md](./CDSChained.md)).
- `dwField_50` / `dwField_5c` / `dwField_60` — same (no `+0x50`/`+0x5c`/`+0x60` operand hits in scrollbar methods); `ResetChainCounters` owns `+0x50` on shared shell.
- `CScrollBar_Render` — **closed (R5):** `__thiscall` + `CScrollBar *` this; thumb/bottom bitmap paths may still alias through `pPad_a5` in decompiler while layout @ `+0x80..+0xa0` is correct in struct editor.
- `FUN_0042ecc0` parent notify from `SetValue` — still `_Globals::` symbol; callee semantics unchanged.
