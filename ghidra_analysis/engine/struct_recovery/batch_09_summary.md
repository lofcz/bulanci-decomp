# Struct recovery batch 9/50

**Structs:** `CHistoryScript`, `CHistoryView`  
**Program:** `bulanci.exe`  
**Evidence:** Ghidra MCP decompile + `OperatorNew` / ctor / consumer xrefs only

## Results

| Struct | Status | Size | Proven fields |
|--------|--------|------|---------------|
| `CHistoryScript` | PARTIAL | `0x444` | Tail MI block `0x430–0x440` (5 fields); base `CDSScript` unpadded |
| `CHistoryView` | PARTIAL | `0x68` | 19 fields via `FUN_0040b560` + ctor + `OnEvent`; two gap ranges UNK |

## Key evidence anchors

- `CHistoryScript::ctor` @ `0x00422700` — extends `CDSScript`, installs opcode ext table at slot 45.
- `CreateObject` @ `0x004229b0` — `OperatorNew(0x444)`.
- `CHistoryView_ctor` @ `0x00422a70` — rect `(0,0,530,510)`, flags `\|= 0x67f`.
- `CreateObject` @ `0x00422f20` / `CHistoryDlg_LoadHistoryPage` @ `0x00422f70` — `OperatorNew(0x68)`.
- `CHistoryView_OnEvent` @ `0x00422670` — reads `pParent` at `+0x4c` for menu audio fade on movie events `0xf0`/`0xf1`.

## Ghidra actions

- [x] `create_struct` `CHistoryScript` (size 0x444)
- [x] `create_struct` `CHistoryView` (size 0x68)
- [x] `get_struct_layout` verified size > 1 for both
- [x] `save_program bulanci.exe`

## Follow-ups

- Recover `CDSScript` base (batch index 2 in manifest) to upgrade `CHistoryScript` to VERIFIED full layout.
- Recover `CDSChained` to deduplicate view-class structs (`CHistoryView`, `CHelpView`, …).
- Map gap fields 0x40–0x4B in view hierarchy via `CDSChained_ResetChainCounters`.
