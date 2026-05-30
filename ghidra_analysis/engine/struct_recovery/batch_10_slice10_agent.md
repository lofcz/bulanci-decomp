# Slice 10 agent report (parallel RE batch 10/50)

**Structs:** `CHelpScript`, `CHistoryDlg`  
**Date:** 2026-05-30

## Types mapped

| Type | Size | Key offsets / leaves |
|------|------|----------------------|
| `CHelpScript` | `0x444` | `CDSScript` prefix `+0x00..+0x42b`; MI tail `+0x430..+0x43f`; `pBoundView` `+0x440` |
| `CHistoryDlg` | `0x9c` | `CIntList` `+0x70..+0x7c`; `pHistoryView` `+0x84`; `pActiveScript` `+0x88`; nav `CIcon*` `+0x8c..+0x98` |

## Leaf functions (evidence)

| Address | Name | Role |
|---------|------|------|
| `0x004215e0` | `CHelpScript::ctor` | `OperatorNew(0x444)`; opcode ext @ slot 45 |
| `0x004217ed` | `CHelpHistoryScript::HhAddChildToParentView` | `CDSView__AddChild(*(this+0x440), child)` |
| `0x00422620` | `_Globals::CDSScript_SetBoundParentView` | `*(script+0x440) = parentView` |
| `0x004231d0` | `CHistoryDlg::CHistoryDlg_ctor` | `OperatorNew(0x9c)`; class filter `0x802` |
| `0x00422f70` | `CHistoryDlg::CHistoryDlg_LoadHistoryPage` | bind script/view, `CallExport` |

## Ghidra actions

- `modify_struct_field`: `CHelpScript` — `pad_afterOpcodeTable`, `field_0x43c`
- `modify_struct_field`: `CHistoryDlg` — pointer types + `pHistoryView` / `pActiveScript` / `pIcon*` names (offset:132..152)
- `rename_function_by_address` `0x422620` → `_Globals::CDSScript_SetBoundParentView`
- `set_function_prototype` `CHistoryDlg_ctor`; decompiler comment on `0x422620`
- `save_program bulanci.exe`

## Files changed

- `CHelpScript.md`, `CHistoryDlg.md`, `batch_10_slice10_agent.md`

## Blockers

- ~~`CHelpScript::ctor` cannot use `CHelpScript *`~~ **resolved (agent todo 18):** embedded `CDSScript script` @ 0 + `CHelpScript_ctor` prototype @ `0x004215e0`.
- ~~`CHistoryDlg` / `CHelpDlg` `CWindow` prefix~~ **resolved (agent todo 18 r2):** embedded `CWindow win` @ 0; decompile `CWindow_BuildAt(&this->win,…)`.
- `CHelpScript+0x43c` — ctor zero only on help/history path.
- Decompiler may show spurious 3rd arg on `CDSScript_SetBoundParentView` (stack cookie); true signature is 2-arg `__thiscall`.
