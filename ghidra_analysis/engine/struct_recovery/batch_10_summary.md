# Struct recovery batch 10/50

**Batch index:** 10 (`batches_50.json`)  
**Structs:** `CHelpScript`, `CHistoryDlg`  
**Protocol:** evidence-only (`AGENT_PROTOCOL.md`)

## Results

| Struct | Status | Size | Ghidra apply |
|--------|--------|------|--------------|
| `CHelpScript` | VERIFIED | `0x444` (1092 B) | Deleted placeholder → `create_struct` (19 fields); `get_struct_layout` OK |
| `CHistoryDlg` | VERIFIED | `0x9c` (156 B) | Deleted placeholder → `create_struct` (12 fields); `get_struct_layout` OK |

## Evidence highlights

- **CHelpScript:** `OperatorNewWithBadAlloc(0x444)`; extends `CDSScript` with 53-entry opcode table (`0x2d` base + `8` help extensions @ slot 45); MI vtables at `+0x430`/`+0x438`; class id 2076 (`0x81c`).
- **CHistoryDlg:** `OperatorNewWithBadAlloc(0x9c)`; `CWindow` dialog with `CIntList` page registry at `+0x70`, current script/view at `+0x88`/`+0x84`, four nav `CIcon*` at `+0x8c..+0x98`; mirrors `CHelpDlg` layout.

## Deliverables

- `CHelpScript.md`
- `CHistoryDlg.md`
- `save_program bulanci.exe` (end of batch)

## UNK carried forward

- `CHelpScript` gap `+0x100..+0x42b`; tail fields `+0x43c`/`+0x440`.
- `CHistoryDlg` `+0x7c` initializer `8` without readers.
- Full `CWindow` prefix naming for both dialogs.
