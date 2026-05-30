# CWindow

## Status

**VERIFIED** — dialog / container base size `0x70` (112 bytes); layout cloned from proven [CMsgDialog.md](./CMsgDialog.md) dialog prefix (`CDSChained` chain band `+0x40..+0x50`, bbox `+0x20..+0x2c`, modal `+0x68`, default focus `+0x6c`). Subclasses (`CMsgDialog`, `CPauseDlg`, `CSessionList`, `CExitDlg`, …) extend with tails at `+0x70` or embed `CWindow win` + pointers.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Prefix size `0x70` | `0x00405560` | `CWindow_BuildAt` plate comment: layout ~`0x70` before subclass extension |
| Heap `CMsgDialog` same base | `0x0040e780` | `CMsgDialog_Allocate` → `OperatorNew(0x70)` |
| Ghidra `CWindow` | — | `get_struct_layout` → **112 bytes**, 32 fields (agent todo 11, 2026-05-30) |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `pVftable_primary` | `CWindow_BuildAt@0x00405560` → `0x47fd5c` |
| `0x04` | 4 | `void *` | `pVftable_IDSChained` | same → `0x47fd3c` |
| `0x08` | 4 | `dword` | `dwField_08` | `CDSChained_ctor@0x004032d0` |
| `0x0C` | 4 | `dword` | `dwField_0c` | `CDSChained_ctor@0x004032d0` |
| `0x10` | 4 | `void *` | `pVftable_IDSEventHandler` | `CWindow_BuildAt` → `0x47fd24` |
| `0x14` | 2 | `ushort` | `wViewFlags` | `CWindow_BuildAt` `\|= 0x77f` |
| `0x18` | 4 | `void *` | `pVftable_field18` | `CWindow_BuildAt` → `0x47fd10` |
| `0x1C` | 4 | `dword` | `dwField_1c` | `CDSChained_ctor@0x004032d0` |
| `0x20` | 4 | `int` | `nBbox_left` | `CWindow_BuildAt` args `left` |
| `0x24` | 4 | `int` | `nBbox_top` | `top` |
| `0x28` | 4 | `int` | `nBbox_right` | `right` |
| `0x2C` | 4 | `int` | `nBbox_bottom` | `bottom` |
| `0x30` .. `0x3C` | 16 | `dword`×4 | `dwField_30` .. `dwField_3c` | `CDSChained_ctor@0x004032d0` |
| `0x40` | 4 | `dword` | `dwChainHead_40` | `CDSChained_ResetChainCounters@0x0042beb0` |
| `0x44` | 2 | `ushort` | `wViewStateFlags` | `CDSChained_ResetChainCounters` `=1` |
| `0x46` | 2 | `ushort` | `wWidgetFlags` | `CWindow_BuildAt` `\|= 1`; subclass ctors OR more |
| `0x48` | 2 | `ushort` | `wChainCounter_48` | `CDSChained_ResetChainCounters` `=0` |
| `0x4A` | 2 | `ushort` | `wChainCounter_4a` | same |
| `0x4C` | 4 | `void *` | `pParent` | `CDSChained_ResetChainCounters` `=0` |
| `0x50` | 4 | `dword` | `dwChainField_50` | `CDSChained_ResetChainCounters` `=0` |
| `0x54` | 4 | `void *` | `pVftable_CDSChain_IDSReferenced` | `CDSChained_ctor@0x004032d0` |
| `0x58` | 4 | `void *` | `pVftable_CDSChain_IDSChained` | same |
| `0x5C` .. `0x64` | 12 | `dword`×3 | `dwField_5c` .. `dwField_64` | `CDSChained_ctor` |
| `0x68` | 1 | `byte` | `bModalFlag` | `CWindow_BuildAt` `modalFlag` arg |
| `0x6C` | 4 | `void *` | `pDefaultFocusChild` | `CWindow_BuildAt` clears; `CDSView_SetAsDefaultFocusChild` |

## Key functions

| Symbol | Address | Role |
|--------|---------|------|
| `CWindow_BuildAt` | `0x00405560` | Base dialog ctor: `CDSChained_ctor`, four vtables, bbox, modal flag, view/widget flags |
| `CWindow_GetTypeDescriptor` | `0x004049b0` | Parent meta `g_CWindow_ClassRegEntry` (`0x4b335c`) for dialog registry |
| `CWindow_FocusSibling` | — | TAB-like focus walk on child chain |

## Ghidra apply

```
delete_data_type CWindow
create_struct CWindow  (112 B, fields match CMsgDialog prefix)
get_struct_layout CWindow → Size: 112 bytes
```

**Agent todo 11 (2026-05-30):** Rebuilt from stale 112 B layout with **gap at `+0x40..+0x4f`** and misnamed `wContainerFlags` / `dwParent` → full chain band aligned with `CMsgDialog`. Rebuilt `CSessionList` with embedded `CWindow win` @ `0` + child pointers @ `+0x70..+0x78`. `set_function_prototype` `CSessionList_BuildDialog@0x0040c2d0`. `save_program bulanci.exe`.

## UNK

- `dwField_08` / `dwField_0c` / `dwField_30..3c` / `dwField_5c..64` semantics beyond `CDSChained_ctor` zero/init.
- Subclass tails (`+0x70+`) are per-type — see `CMsgDialog`, `CSessionList`, `CExitDlg`, `CScore`.

## CDSView relationship (agent todo 11, 2026-05-30)

`CDSView` is the **128 B** (`0x80`) engine view shell: embedded **`CWindow win`** @ `+0x00` (same **112 B** / `0x70` dialog prefix as `CMsgDialog` / `CWindow_BuildAt`) plus a **16 B** band @ `+0x70..+0x7F` (`bGaming_slot_id`, `nDest_x`/`nDest_y`/`nSrc_x` on gameplay views; `CSessionList` reuses `+0x70..+0x78` for child pointers). Leaf dialogs that allocate only `OperatorNew(0x70)` are **`CWindow`-sized**, not full `CDSView`. See [CDSView.md](./CDSView.md).

## Cross-links

- [CMsgDialog.md](./CMsgDialog.md) — leaf dialog, no tail past `0x70`
- [CSessionList.md](./CSessionList.md) — `CWindow win` + three child pointers
- [CHelpDlg.md](./CHelpDlg.md) / [CHistoryDlg.md](./CHistoryDlg.md) — page-browser dialogs (`0x9c`); `CWindow win` @ 0 (R3 todo 11)
- [batch_19_followup_summary.md](./batch_19_followup_summary.md) — original `CWindow` rebuild rationale
