# Struct recovery batch 19/50

> **Note:** This file documents batch **18** structs (`CSessionItem`, `CSessionList`). Canonical index: [batch_18_summary.md](./batch_18_summary.md). Manifest index **19** is `CSetupDlg` / `CSwitch` (`batches_50.json`). Follow-up: [batch_19_followup_summary.md](./batch_19_followup_summary.md).

**Index:** 19 (`batches_50.json`)  
**Types:** `CSetupDlg`, `CSwitch` (pending)  
**Date:** 2026-05-30

## Results

| Struct | Status | Size | Notes |
|--------|--------|------|-------|
| `CSessionItem` | VERIFIED | `0x24` | `CListBoxItem` base (`0x14`) + four dwords at `+0x14..+0x20` |
| `CSessionList` | PARTIAL | `0x7c` | `CWindow` dialog; children `+0x70` listbox, `+0x74` status, `+0x78` join |

## Key evidence

- **CSessionItem alloc:** `PUSH 0x24` in `CSessionList_AppendEnumSession` (`0x0040f3a1`).
- **CSessionList alloc:** `PUSH 0x7c` in `CreateObject` (`0x0040e9a1`); stack `[124]` in `CMenu_PickSession` (`0x004141a3`).
- **Do not use `0xf4` as struct size** — that value is only the dialog height passed to `CWindow_BuildAt`.

## Artifacts

- `ghidra_analysis/engine/struct_recovery/CSessionItem.md`
- `ghidra_analysis/engine/struct_recovery/CSessionList.md`
- Ghidra: structs rebuilt via inline script; `get_struct_layout` → `CSessionItem` 36 B, `CSessionList` 124 B; `save_program bulanci.exe` saved.

## Follow-ups

- ~~Resolve `CWindow`/`CDSView` size vs `+0x70` tail~~ → done in [batch_19_followup_summary.md](./batch_19_followup_summary.md).
- ~~Name `CSessionItem+0x14..+0x20` with DirectPlay header~~ → `DPEnumSessionInfo` + typed `CSessionItem_Initialize` param.
