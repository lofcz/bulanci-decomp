# CExitDlg

## Status

**PARTIAL** — allocation size `0x7C` verified; extends `CWindow` / `CDSChained` dialog base through `+0x6C` (same layout as `CMsgDialog` / `CPauseDlg` batch 13). Tail `+0x70..+0x78` is the shared `0x7C`-dialog extension slot; on `CExitDlg` all three dwords are ctor-zeroed only (children are heap widgets via `CDSView__AddChild`, not cached here).

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CExitDlg) == 0x7C` | `0x0041212d` | `_Globals::CreateObject` → `OperatorNewWithBadAlloc(0x7c)` → `CExitDlg_ctor` |
| Same size in menu path | `0x00425a0f` | `CMenu_CmdDispatch` case `0xcb` → `OperatorNewWithBadAlloc(0x7c)` → `CExitDlg_ctor` |
| Base `CWindow` ~`0x70` | `0x00405560` | `CWindow_BuildAt` comment: layout ~`0x70` before subclass extension; `CExitDlg` fields at `+0x70` |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|-------|------|------|------|----------------------|
| 0x00 | 4 | `void *` | `pVftable_primary` | `CExitDlg_ctor@0x00411b50` (after `CWindow_BuildAt`) |
| 0x04 | 4 | `void *` | `pVftable_IDSChained` | `g_pCExitDlg_vftable_IDSChained@0x00411b50` |
| 0x08 | 4 | `dword` | `dwField_08` | `CDSChained_ctor@0x004032d0` (via `CWindow_BuildAt`) |
| 0x0C | 4 | `dword` | `dwTimelineFlags` | `FUN_004390d0@0x004390d0` read/modify; `TM_Play` on `this+0x10` |
| 0x10 | 4 | `void *` | `pVftable_IDSEventHandler` | `CExitDlg_ctor@0x00411b50` |
| 0x14 | 2 | `ushort` | `wViewFlags` | `CWindow_BuildAt@0x00405560` `\|= 0x77f` |
| 0x18 | 4 | `void *` | `pVftable_IDSReferenced` | `CExitDlg_ctor@0x00411b50` |
| 0x1C | 4 | `dword` | `dwField_1c` | `CDSChained_ctor@0x004032d0` |
| 0x20 | 4 | `int` | `nBbox_left` | `CWindow_BuildAt(0xdd,0x18,0x2fa,0x239,0)@0x00411b50` |
| 0x24 | 4 | `int` | `nBbox_top` | same |
| 0x28 | 4 | `int` | `nBbox_right` | same |
| 0x2C | 4 | `int` | `nBbox_bottom` | same |
| 0x30 | 4 | `dword` | `dwField_30` | `CDSChained_ctor@0x004032d0` |
| 0x34 | 4 | `dword` | `dwField_34` | `CDSChained_ctor@0x004032d0` |
| 0x38 | 4 | `dword` | `dwField_38` | `CDSChained_ctor@0x004032d0` |
| 0x3C | 4 | `dword` | `dwField_3c` | `CDSChained_ctor@0x004032d0` |
| 0x46 | 2 | `ushort` | `wKbFocusFlag` | `CWindow_BuildAt@0x00405560` `\|= 1` |
| 0x4C | 4 | `void *` | `pParent` | `CExitDlg_RouteSyntheticCloseEvent@0x0040b290` `*(this+0x4c)+0x10` PostMessage |
| 0x54 | 4 | `void *` | `pVftable_CDSChain_IDSReferenced` | `CDSChained_ctor@0x004032d0` |
| 0x58 | 4 | `void *` | `pVftable_CDSChain_IDSChained` | `CDSChained_ctor@0x004032d0` |
| 0x5C | 4 | `dword` | `dwField_5c` | `CDSChained_ctor@0x004032d0` |
| 0x60 | 4 | `dword` | `dwField_60` | `CDSChained_ctor@0x004032d0` |
| 0x64 | 4 | `dword` | `dwField_64` | `CDSChained_ctor@0x004032d0` |
| 0x68 | 1 | `byte` | `bModalFlag` | `CWindow_BuildAt` stores `param_5` (`0` in ctor) |
| 0x6C | 4 | `void *` | `pDefaultFocusChild` | `CWindow_BuildAt@0x00405560` `this+0x6c = 0` |
| 0x70 | 4 | `void *` | `pLinkedAnim` | `CExitDlg_ctor@0x00411b50` `this->pLinkedAnim=0`; optional `RouteSyntheticCloseEvent` → `CAnim_SetTimelineActive(pLinkedAnim+0x98,0)` |
| 0x74 | 4 | `dword` | `reserved_74` | `CExitDlg_ctor@0x00411b50` `dwReserved_74=0` only (cf. `CPauseDlg` `pBtnPrimary`) |
| 0x78 | 4 | `dword` | `reserved_78` | `CExitDlg_ctor@0x00411b50` `dwReserved_78=0` only (cf. `CPauseDlg` `pBtnSecondary`) |

## Class registry

| Claim | Address | Evidence |
|-------|---------|----------|
| `GetClassTable` → `g_CExitDlg_classMeta` | `CExitDlg_GetClassTable@0x00411de0` | `return &DAT_004b3654` |
| **classId = 2054 (`0x806`)** | `CreateObject@0x004120f0` | `OperatorNew(0x7c)` → `CExitDlg_ctor`; menu `CMenu_CmdDispatch` case `0xcb` @ `0x00425a0f` |

## Ghidra apply (slice 07, 2026-05-30)

```
get_struct_layout CExitDlg → 124 B (0x7C)
  tail: pParent@0x4C, pLinkedAnim@0x70, dwReserved_74/78
set_function_prototype CExitDlg_ctor → CExitDlg * __fastcall CExitDlg_ctor(CExitDlg *this)
rename FUN_004390d0 → CAnim_SetTimelineActive (timeline gate used on linked anim +0x98)
save_program bulanci.exe
```

## Key functions

| Symbol | Address |
|--------|---------|
| `CExitDlg_ctor` | `0x00411b50` |
| `CExitDlg_RouteSyntheticCloseEvent` | `0x0040b290` |
| `CExitDlg_GetClassTable` | `0x00411de0` |
| `CAnim_SetTimelineActive` | `0x004390d0` |

## UNK

- **No non-zero writer** for `pLinkedAnim` / `reserved_74` / `reserved_78` in any of the three `CExitDlg_*` symbols (`ctor`, `RouteSyntheticCloseEvent`, `GetClassTable`); program-wide `MOV [reg+0x70], EAX` stores appear on `CSessionList_BuildDialog`, `CPauseDlg_Build`, `CGameTypeDlg_BuildUi`, etc., but not on `CExitDlg`. Quit flow uses heap children only (`main_menu.md` §5.3).
- Offsets `0x40–0x45`, `0x48–0x4B` (chain/child linkage on sibling dialogs — see `CPauseDlg.md`).
- `header_unk` between `+0x30` and `+0x46` not individually xrefs’d on `CExitDlg`.
- Decor `CAnim`×2, `CStaticText`, `CButton` are separate `OperatorNew` objects, not embedded at `+0x70`.
