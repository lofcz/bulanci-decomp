# CGameTypeDlg

## Status

**PARTIAL** — heap size `0x94` verified; extends `CWindow` (`0x70`) with nine cached child pointers (`+0x70..+0x90`). Full UI built by `CGameTypeDlg_BuildUi` — stack **`CGameTypeDlg` (0x94 B)** from `CStartGame2_OnCmd` case **`0xDA`** ([CStartGame2.md](./CStartGame2.md)).

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CGameTypeDlg) == 0x94` | `0x0040ee40` | `CGameTypeDlg_CreateObject` → `OperatorNewWithBadAlloc(0x94)` |
| Last member `+0x90` | `0x0040d930` | `CGameTypeDlg_BuildUi` stores primary OK `CButton*` at `this+0x90` |
| `CWindow` prefix `0x70` | `0x0040d930` | `CWindow_BuildAt(this,0,0,0x140,0xda,1)` then vtable swap |

## Class registry

| Claim | Address | Evidence |
|-------|---------|----------|
| **classId = 2039 (`0x7f7`)** | static init comment | `HandleClassRegister(classId 0x7f7/2039, factory @ 0x40ee40)` |
| `GetClassTable` | `0x0040bc00` | Returns registry blob pointer |
| Factory body | `0x0040ee40` | Minimal alloc + `CDSChained_ctor` + vtables; `puVar1[0x1b]=0` → `+0x6C` cleared |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00..0x6F | — | — | `CWindow` prefix | `CGameTypeDlg_BuildUi@0x0040d930` via `CWindow_BuildAt` (see [CPauseDlg.md](./CPauseDlg.md) / `widgets.md` §16) |
| 0x70 | 4 | `CRadio *` | `pRadioModeA` | `BuildUi` store; `CGameTypeDlg_OnNotify@0x0040ab00` compares to `param_2` on notify `0xCE` |
| 0x74 | 4 | `CRadio *` | `pRadioModeB` | `BuildUi` store; hidden when mode A selected |
| 0x78 | 12 | `CStaticText *[3]` | `pLabelGroup` | `BuildUi` stores `[0..2]`; `OnNotify@0x0040ab00` `Show(pLabelGroup[radio+0x68])` |
| 0x84 | 12 | `CNumEdit *[3]` | `pNumEditGroup` | `BuildUi` stores `[0..2]`; hide loop walks `pNumEditGroup` with `[-3]` label pairs |
| 0x90 | 4 | `CButton *` | `pBtnOk` | `BuildUi` store; `CGameTypeDlg_RefreshFocusOrEnable@0x0040aad0` `IsDlgButtonChecked(0x8002)` enables `param_1[0x24]` child |

## Behavior (leaf evidence)

| Handler | Address | Notes |
|---------|---------|-------|
| `CGameTypeDlg_OnNotify` | `0x0040ab00` | WM notify `0xCE`: read `*(byte*)(radio+0x68)` as rules-group index; hide three (label,numedit) pairs; show selected pair; `CGameTypeDlg_RefreshFocusOrEnable` |
| `CGameTypeDlg_RefreshFocusOrEnable` | `0x0040aad0` | If dialog button `0x8002` checked → `CDSView_EnableWidget(child[9])` else `SetAsDefaultFocusChild(child[9])` |
| `CGameTypeDlg_BuildUi` | `0x0040d930` | Deathmatch/team radios, three `CNumEdit` rule fields (players/frags/time), OK + second button (second not cached on `this`) |

## Ghidra apply

**Applied (R3 task 9, 2026-05-30):** `get_struct_layout` → 148 B: `CWindow win` @0 (112 B) + typed tail `pRadioModeA`..`pBtnOk` @0x70..0x90. `CGameTypeDlg_BuildUi@0x40d930` in class `CGameTypeDlg` with `CGameTypeDlg *` this (`&this->win`, `this->pRadioModeA`, …). `OnNotify` / `RefreshFocusOrEnable` / `CreateObject` prototyped. `save_program bulanci.exe`.

**Applied (R4 task 9, 2026-05-30):** `recreate_struct` — `pLabelGroup[3]` @0x78, `pNumEditGroup[3]` @0x84 (fixes decompiler `(&win+1)` indexing). `OnNotify` decompile: `pLabelGroup[bVar1]` / `pNumEditGroup[bVar1]`. `RefreshFocusOrEnable` `__thiscall` + `this->pBtnOk` @+0x90. `save_program bulanci.exe`.

## UNK

- Exact Czech UI strings / game-mode enum for radio `+0x68` byte (read from `CRadio` object, not `CGameTypeDlg`).
- Secondary bottom `CButton` in `BuildUi` (added as child only, no `this+` store).
- `CRadio` notify byte at `radio+0x68` semantics (group index for rule triplets).
