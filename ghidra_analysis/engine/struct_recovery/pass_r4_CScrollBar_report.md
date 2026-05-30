# R4 pass — CScrollBar struct recovery

## Goal

Map and document **CScrollBar** in Ghidra: layout, ctors, consumers, vtables. Deliverables: [CScrollBar.md](./CScrollBar.md), this report, jsonl append.

## Status

**DONE** — Ghidra `CScrollBar` rebuilt to **204 B** with full `CDSChained` prefix, named `CDSImage*` bitmap band, and scroll-state fields; `CVolume` embed fixed. Program saved.

## Size proof

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| `0xcc` heap size | `CScroller_BuildAt@0x004089c0` | `OperatorNewWithBadAlloc(0xcc)` before each `CScrollBar_BuildAt` |
| Placement through tail | `CScrollBar_BuildAt@0x00408370` | Stores `nDragMode` @ `+0xc0`, `nDragStartMouse` @ `+0xc8` |
| `CVolume` embed | `get_struct_layout CVolume` | `CScrollBar scrollbar` @ 0, `pAudio_preview` @ 204 (208 B total) |

## Ghidra deltas

| Action | Detail |
|--------|--------|
| `recreate_struct` | **CScrollBar** 204 B — `CDSChained` fields `+0x00..+0x67`, `scheduler` @ `+0x68`, `pBitmap_*` @ `+0x80..+0xa0`, metrics `+0xa4..+0xc8` |
| `recreate_struct` | **CVolume** — `CScrollBar scrollbar` @ 0; `CDSAudioPlayer * pAudio_preview` @ 204 |
| `set_function_this_type` | `CScrollBar *` @ `0x00404790`, `0x00408370`, `0x00403530`, `0x00403860`, `0x00404fe0`, `0x00407460` |
| `force_decompile` | `CScrollBar_BuildAt`, `CScrollBar_Render` — `BuildAt` now uses `this->pBitmap_topNormal`, `nBbox_*`, `nCurrentValue`, etc. |
| `save_program` | `bulanci.exe` |

## Layout highlights (post-apply)

- **Prefix** `+0x00..+0x67`: aligned with [CDSChained.md](./CDSChained.md) (`nBbox_*`, `nScreenBbox_*`, chain band, `pOverlapEntity`).
- **Scheduler** `+0x68`: `CDSUpdatedItem`; `BuildAt` registers event slot **7** (press-and-hold tick).
- **Bitmaps** `+0x80..+0xa0`: nine `CDSImage*` (top/thumb/bottom × normal/pressed/disabled); default resource IDs `0x1008c..` (H) / `0x1008e..` (V) when `bitmaps==NULL`.
- **Metrics** `+0xa8..+0xc8`: `nMinValue`, `nMaxValue`, `nCurrentValue`, `nStep`, `nPage`, orientation/drag band — matches [widgets.md](../../widgets.md) §12.

## Vtables

Five MI faces (see [vftable_methods.csv](../vftable_methods.csv)):

| Offset | Vtable | Primary slots |
|--------|--------|----------------|
| `+0x00` | `0x0047fbdc` | 14=`CScrollBar_Render`, 16=`dtor`, 18=`OnTimerTick`, 19–20 mouse |
| `+0x04` | `0x0047fbbc` | `IDSChained` / chain |
| `+0x10` | `0x0047fb90` | `IDSEventHandler` |
| `+0x18` | `0x0047fba4` | view facet |
| `+0x68` | `0x0047fb78` | `IDSUpdated` on `scheduler` |

## Ctors

| Symbol | Address | Notes |
|--------|---------|-------|
| `CScrollBar_ctor` | `0x00404790` | Heap path; xref from `CreateObject`, `CVolume_Allocate` |
| `CScrollBar_BuildAt` | `0x00408370` | Placement; xrefs: `CScroller_BuildAt` (×2), `CVolume_BuildAt` |
| `CScrollBar_dtor` / `vDtor` | `0x00404fe0` / `0x00406430` | Bitmap vector teardown; thumb-drag value commit |

## Consumers

| Consumer | Address | Relationship |
|----------|---------|----------------|
| `CScroller_BuildAt` | `0x004089c0` | Allocates H/V child scrollbars (`pHorizontalScrollBar`, `pVerticalScrollBar`) |
| `CVolume_BuildAt` | `0x0040e1b0` | Embedded slider in setup dialog |
| `CSetupDlg_SetVolumeBinding` | `0x0040e590` | `CScrollBar_SetValue` on `pVolume` |
| `CSetupDlg_OnVolumeFocus` | `0x0040e5b0` | `notifyCode==7` → `nCurrentValue` @ `+0xb0` for pan preview |
| List hierarchy | `CListViewer` / `CListBox` / … | Via `CScroller` children ([widgets.md](../../widgets.md) §13) |

## Struct doc updates

- [CScrollBar.md](./CScrollBar.md) — **VERIFIED**, full layout table, vtables, consumers
- [CSetupDlg.md](./CSetupDlg.md) — cross-ref unchanged (`+0xb0` on `CVolume` embed)
- [CDSChained.md](./CDSChained.md) — `CScrollBar_Render` consumer of `nScreenBbox_*` (prior R4 task 21)

## Remaining UNK

- Shared-shell fields `dwField_08`, `dwField_0c`, `dwField_1c`, `dwField_50`, `dwField_5c`, `dwField_60` (no scrollbar-specific writers).
- `CScrollBar_Render` fastcall `param_1` not retyped to `CScrollBar *` in decompiler output.
- Global helpers `CScrollBar_SetValue` / `SetScrollRange` not moved into class namespace.
