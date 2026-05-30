# Round 3 — Task 11 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 11 |
| **title** | Name CHelpScript CHistoryDlg CWindow prefix +0x00..+0x6f |
| **types** | `CHelpScript`, `CHistoryDlg`, `CWindow`, `CDSScript` |
| **addresses** | `0x004215e0`, `0x00405560`, `0x004217e0`, `0x00422620` |
| **supersedes** | agent todo 18 (r1) |

## Status

**DONE**

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| `CWindow` prefix **112 B** / `0x70` named | `CWindow_BuildAt@0x00405560` | `get_struct_layout CWindow` → 32 fields (`pVftable_primary` … `pDefaultFocusChild` @ `+0x6c`) |
| `CHistoryDlg` embeds `CWindow win` @ 0 | `CHistoryDlg_ctor@0x004231d0` | Decompile: `CWindow::CWindow_BuildAt(&this->win,…)`; `(this->win).pVftable_*` through `+0x6f` band |
| `CHelpDlg` same prefix | `CHelpDlgCtor@0x00421e40` | Identical `&this->win` / `(this->win).*` pattern; alloc `0x9c` |
| `CHelpScript` base is **`CDSScript`**, not `CWindow` | `CHelpScript_ctor@0x004215e0` | `CDSScript::ctor(&this->script)`; MI tail @ `+0x430..+0x443`; `pBoundView` @ `+0x440` |
| Page bind before export | `CHistoryDlg_LoadHistoryPage` | Calls `CDSScript_SetBoundParentView@0x00422620` → `this->pBoundView` |
| Opcode 51 parent attach | `HhAddChildToParentView@0x004217e0` | `CDSView__AddChild(*(this+0x440), child, 0)` (was flat `param_1+0x440`) |
| Dialog page list | `CHistoryDlg_ctor` / `CHelpDlgCtor` | `CIntList` @ `+0x70` (`m_pageIds`); growth chunk `8` @ list `+0xc` |

### `CWindow` field map (`+0x00..+0x6f`)

Canonical names match [CWindow.md](./CWindow.md) and [CPauseDlg.md](./CPauseDlg.md): vtables `+0x00/04/10/18`, `wViewFlags` `+0x14`, bbox `+0x20..+0x2c`, chain band `+0x40..+0x50`, `bModalFlag` `+0x68`, `pDefaultFocusChild` `+0x6c`.

## Ghidra deltas

- Verified **`CWindow`** layout (112 B) — no gap at `+0x40..+0x4f` (prior agent todo 11 rebuild).
- **`CHistoryDlg`** / **`CHelpDlg`**: embedded **`CWindow win`** @ 0 (156 B each); dialog tails unchanged (`m_pageIds`, views, icons).
- **`CHelpScript`**: embedded **`CDSScript script`** @ 0 (1092 B); renamed `nestedRefcount`, `field_0x43c`, `pBoundView`.
- **`CDSScript`**: `pPad_afterOpcodeTable` → `pad_afterOpcodeTable` (shared with help/history script padding band).
- **`set_function_this_type`**: `CHistoryDlg_ctor`, `CHelpDlgCtor`, `CHelpScript_ctor`, `CHistoryDlg_LoadHistoryPage`, `CHelpDlg_LoadHelpPage`, `CWindow_BuildAt`, `CDSScript_SetBoundParentView@0x00422620`.
- **`set_function_prototype` + `set_function_this_type`**: `HhAddChildToParentView@0x004217e0` → `CHelpScript *` __thiscall (opcode handler; shared with `CHistoryScript`).
- **`save_program bulanci.exe`**.

## Struct doc updates

- [CWindow.md](./CWindow.md) — cross-link help/history dialogs.
- [CHistoryDlg.md](./CHistoryDlg.md) — R3 apply note; `CWindow win` prefix **done**.
- [CHelpDlg.md](./CHelpDlg.md) — aligned with `CHistoryDlg`.
- [CHelpScript.md](./CHelpScript.md) — R3 `set_function_this_type` / bind helper names.
- [batch_10_followup_summary.md](./batch_10_followup_summary.md) — cleared deferred `CWindow` prefix UNK for help/history dialogs.

## Remaining UNK

- Ghidra **component prefix** on nested types (`(this->m_pageIds).pM_data`, `nM_currentPage`) — cosmetic; logical names `m_pageIds.*` / `m_currentPage` per docs (`CIntList` member prefixing).
- `HhAddChildToParentView` decompile still passes `in_stack_00000004` to `ReadSubExpr` after __thiscall retype — needs stack-frame refresh / shared `CHistoryScript` class association.
- `CHelpScript+0x43c` — ctor-zero only on help path (see `CHelpScript.md`).
- `CDSScript` pad dwords `+0x0c` / `+0x14` / `+0x1c` (base class).
