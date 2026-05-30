# Slice 11 agent report (parallel RE batch 11)

**Assignment:** `batches_50.json[11]` → `CHistoryScript`, `CHistoryView`  
**Program:** `bulanci.exe`  
**Date:** 2026-05-30

## Types mapped

| Struct | Size | Ghidra | Key members / anchors |
|--------|------|--------|------------------------|
| `CHistoryScript` | `0x444` | Verified | `script` (`CDSScript` @ 0), tail `0x430`–`0x440` (`pVftable_IDSEventHandler`, `nNestedRefCount`, `vf_IDSChained_tail`, `pSubObjStash`, `pBoundView`) |
| `CHistoryView` | `0x68` | Verified | `chain` (`CDSChained` @ 0); ctor bbox `530×510`, `wViewFlags \|= 0x67f` |
| `CDSScript` | `0x430` | (dependency) | Embedded base for history script |
| `CDSChained` | `0x68` | (dependency) | Embedded in `CHistoryView` |

## Functions (leaf evidence)

| Address | Name | Role |
|---------|------|------|
| `0x004226c0` | `CHistoryScript::ctor` | `CDSScript::ctor`, MI vtables, `InstallOpcodeTable(0x2d, …, 8)` |
| `0x004227b0` | `CHistoryScript_dtor` | `IDSChainedTail_ClearSubObjStash(&vf_IDSChained_tail)`; `CDSScript_dtor` |
| `0x004229b0` | `CreateObject` | `OperatorNew(0x444)` |
| `0x00422620` | `CDSScript_SetBoundParentView` | `*(script+0x440) = view` (help/history page load) |
| `0x00434250` | `IDSChainedTail_ClearSubObjStash` | Clears `pSubObjStash` subtree |
| `0x00422a70` | `CHistoryView_ctor` | `CDSChained::FUN_0040b560`, history vtables |
| `0x00422670` | `CHistoryView_OnEvent` | Movie `0xf0`/`0xf1` → menu fade via `parent+0x4c` |
| `0x00422f70` | `CHistoryDlg_LoadHistoryPage` | Alloc `0x68` view, bind script, `CallExport` |

## Ghidra actions

- Verified `get_struct_layout` for `CHistoryScript` (1092) and `CHistoryView` (104).
- Renamed `FUN_00422620` → `CDSScript_SetBoundParentView`; plate comment @ `0x00422620`.
- Renamed `FUN_00434250` → `IDSChainedTail_ClearSubObjStash`.
- `set_function_prototype` `CHistoryScript_dtor(CHistoryScript *)` @ `0x004227b0`.
- `save_program bulanci.exe`.

## Files changed

- `ghidra_analysis/engine/struct_recovery/CHistoryScript.md`
- `ghidra_analysis/engine/struct_recovery/CHistoryView.md`
- `ghidra_analysis/engine/gameplay_struct_backlog.md`
- `ghidra_analysis/engine/struct_recovery/batch_11_slice11_report.md` (this file)

## Blockers / UNK

- `CDSScript_SetBoundParentView` __thiscall `this` type stays `void*` in decompiler (Ghidra API limitation).
- `CHistoryDlg` field names in decompile still auto (`field6_0x84` etc.) — out of slice scope.
- Class COL for ids `0x802` / `0x801` not decoded.
- `nNestedRefCount` / `pSubObjStash` writers on history path beyond ctor/dtor.
