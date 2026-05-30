# Round 3 — Task 09 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 9 |
| **title** | Apply CGameCounter and CGameTypeDlg Ghidra layouts |
| **types** | `CGameCounter`, `CGameTypeDlg`, `CWindow` |
| **addresses** | `0x0040bc20`, `0x0040b140`, `0x0040d930`, `0x0040ee40` |
| **evidence** | [CGameCounter.md](./CGameCounter.md), [CGameTypeDlg.md](./CGameTypeDlg.md), batch_08 |

## Status

**DONE**

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CGameCounter) == 0x80` | factory `~0x0040eed0` | `OperatorNew(0x80)` → `CGameCounter_Constructor@0x0040bc20` |
| `CDSChained` base `0x68` + tail `0x68..0x7C` | `0x0040bc20` | `CDSChained_ctor`; vector ctor at `+0x68` ×2; four bitmap slots |
| `CGameCounter_OnEvent` uses tail | `0x0040b140` | `pProgressSlot[param_2]->trackImage` + `strPackFrame` / `strPackCancelBtn`; `param_2==4` → PostMessage on IDSUpdated facet |
| `sizeof(CGameTypeDlg) == 0x94` | `0x0040ee40` | `CGameTypeDlg_CreateObject` → `OperatorNew(0x94)` |
| `CWindow win` embed @ 0 | `0x0040d930` | `CWindow_BuildAt(&this->win, …)`; tail stores @ `+0x70..+0x90` |
| Modal stack path | `0x0040fcec` | `CStartGame2_OnCmd` case `0xDA`: `CGameTypeDlg_BuildUi(&local_110, …)` then `CDSView_DoModal` |
| classId **2041** / **2039** | static init | `CGameCounter` `0x7f9`; `CGameTypeDlg` `0x7f7` |

### Ghidra struct layouts (verified)

**CGameCounter** — 128 B (`0x80`):

| Offset | Name | Type |
|--------|------|------|
| `0x00` | `base` | `CDSChained` (104 B) |
| `0x68` | `strPackFrame` | `pointer` (pack handle; vector-constructed) |
| `0x6C` | `strPackCancelBtn` | `pointer` |
| `0x70..0x7C` | `pProgressSlot0..3` | `CDSBitmap *` |

**CGameTypeDlg** — 148 B (`0x94`):

| Offset | Name | Type |
|--------|------|------|
| `0x00` | `win` | `CWindow` (112 B) |
| `0x70` | `pRadioModeA` | `CRadio *` |
| `0x74` | `pRadioModeB` | `CRadio *` |
| `0x78..0x80` | `pLabelGroup0..2` | `CStaticText *` |
| `0x84..0x8C` | `pNumEditGroup0..2` | `CNumEdit *` |
| `0x90` | `pBtnOk` | `CButton *` |

## Ghidra deltas

- Verified / retained struct sizes and embedded bases (`CDSChained`, `CWindow win`).
- `modify_struct_field` — `CGameCounter` tail `pProgressSlot0..3` → `CDSBitmap *`; `CGameTypeDlg` tail widget pointers typed.
- `set_function_prototype` + `set_function_this_type` — `CGameCounter_OnEvent@0x40b140`, `CGameTypeDlg_BuildUi@0x40d930`, `CGameTypeDlg_OnNotify@0x40ab00`, `CGameTypeDlg_RefreshFocusOrEnable@0x40aad0`, `CGameTypeDlg_CreateObject@0x40ee40`.
- `CGameTypeDlg_BuildUi` moved into class namespace `CGameTypeDlg` (decompile uses `this->pRadioModeA`, `&this->win`, `this->pBtnOk`).
- Decompiler comments @ `0x0040b140`, `0x0040d930`, `0x0040ee40`.
- `save_program bulanci.exe`.

## Struct doc updates

- [CGameCounter.md](./CGameCounter.md) — Ghidra apply marked done (R3 task 9).
- [CGameTypeDlg.md](./CGameTypeDlg.md) — Ghidra apply marked done (R3 task 9).

## Remaining UNK

- `strPackFrame` / `strPackCancelBtn` remain `pointer` (no `CDSString` Ghidra type); semantics = menu-cache pack refs.
- `CGameTypeDlg_OnNotify` show path still decompiles as `*(…)(&this->win + 1) + (index)*4` instead of `pLabelGroupN` / `pNumEditGroupN` (hide loop and `pRadioModeA` compare are typed).
- Secondary OK `CButton` in `BuildUi` not cached on `this` (child only).
- `CGameTypeDlg_CreateObject` uses `CDSChained_ctor` + vtable patch (minimal factory; full shell from `BuildUi` on stack).
