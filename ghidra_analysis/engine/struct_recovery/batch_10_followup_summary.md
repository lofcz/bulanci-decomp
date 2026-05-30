# Struct recovery batch 10 follow-up (round 2)

**Prior:** `batch_10_summary.md`  
**Structs:** `CHelpScript`, `CHistoryDlg`  
**Status:** **FOLLOWUP_COMPLETE**

## Actions taken

| Item | Resolution |
|------|------------|
| `CHelpScript` gap `+0x100..+0x42b` | Proven padding: 53-entry `pOpcode` ends at `+0x100`; `pCallExportScratch` at `+0x42c`. Ghidra field renamed `pad_afterOpcodeTable`. |
| `CHelpScript` `+0x440` | **`pBoundView`**: `FUN_00422620` (help/history page load), `HhAddChildToParentView@0x004217ed` reads `*(this+0x440)`. Type fixed to `void*` in Ghidra. |
| `CHelpScript` `+0x43c` | Still ctor-zero only on help path; no help/history consumers (level script uses MI adjust elsewhere). |
| `CHistoryDlg` `+0x7c` | **`m_pageIds.m_growthChunk`**: ctor `=8`; `CIntList_EnsureCapacity@0x00401d30` reads list `+0xc` (dialog `+0x7c`). Matches `CHelpDlg` embedded list. |
| `CHistoryDlg` `CWindow` prefix | **Done** (R3 todo 11): embedded `CWindow win` @ 0; decompile `(this->win).pVftable_*` through `+0x6f` — see `round3_task_11_report.md`. |

## Ghidra deltas

- `CHelpScript`: `nField_0x440` → `pBoundView` (`void*`); `pPad_0x100` → `pad_afterOpcodeTable`.
- `CHistoryDlg`: `nField_0x7c` → `m_pageIds_growthChunk`.
- `save_program bulanci.exe` (once).

## Docs updated

- `CHelpScript.md` — layout + UNK
- `CHistoryDlg.md` — `CIntList` four-dword layout at `+0x70..+0x7c`

## Agent todo 18 (2026-05-30)

- Rebuilt `CHelpScript` with `CDSScript script` @ 0 (was flat prefix); deleted stale 1 B `/CHelpScript/CHelpScript`; `CHelpScript *` + `CHelpScript_ctor@0x004215e0` prototype; `save_program bulanci.exe`.

## Remaining UNK

- `CHelpScript+0x43c` on help/history scripts (zero-init only).
- ~~`CHelpScript` / `CHistoryDlg` `CWindow` prefix~~ — **done** R3 todo 11 (`CHelpDlg`/`CHistoryDlg` only; `CHelpScript` uses `CDSScript` @ 0).
- `CDSScript` pad dwords `+0x0c` / `+0x14` / `+0x1c` (base class; see `CDSScript.md`).
