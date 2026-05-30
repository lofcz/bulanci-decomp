# Struct recovery batch 11/50

**Index:** `batches_50.json[11]` → `CHistoryScript`, `CHistoryView`  
**Program:** `bulanci.exe`  
**Evidence:** Ghidra MCP decompile, `OperatorNewWithBadAlloc`, ctor/dtor, consumer xrefs only

## Results

| Struct | Status | Size | Proven fields |
|--------|--------|------|---------------|
| `CHistoryScript` | VERIFIED | `0x444` | Embedded `CDSScript` + tail `0x430`–`0x440` (slice 11) |
| `CHistoryView` | VERIFIED | `0x68` | `CDSChained chain` @ 0; gaps resolved via `CDSChained.md` |

## Key evidence anchors

- `CHistoryScript::ctor` @ `0x004226c0` — `CDSScript::ctor`, opcode ext table slot `0x2d` (45), 8 handlers.
- `CreateObject` @ `0x004229b0` — `OperatorNew(0x444)`.
- `CHistoryView_ctor` @ `0x00422a70` — rect `(0,0,530,510)`, flags `\|= 0x67f`.
- `CreateObject` @ `0x00422f00` / `CHistoryDlg_LoadHistoryPage` @ `0x00422f70` — `OperatorNew(0x68)`.
- `CHistoryView_OnEvent` @ `0x00422670` — `pParent@+0x4c` → menu audio fade on movie events `0xf0`/`0xf1`.
- `CDSScript_SetBoundParentView` @ `0x00422620` — binds active view at `CHistoryScript+0x440`.

## Ghidra actions

- [x] `CHistoryScript` / `CHistoryView` structs present (`get_struct_layout` size 1092 / 104)
- [x] `save_program bulanci.exe`

## Follow-ups (slice 11 closed most items)

- See `batch_11_followup_summary.md` / `batch_11_slice11_report.md` for Ghidra renames and remaining UNK.
- `CDSScript_SetBoundParentView` decompiler still shows spurious third parameter (__thiscall API limit).

## Deliverables

- `ghidra_analysis/engine/struct_recovery/CHistoryScript.md`
- `ghidra_analysis/engine/struct_recovery/CHistoryView.md`
