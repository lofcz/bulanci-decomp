# Round 7 FUN — Task 14 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 14 |
| **round** | 7 |
| **band** | sim |
| **seed_address** | `0x0042a130` |
| **title** | FUN recovery: FUN_0042A130 @ 0x0042a130 (xrefs=1) |
| **prior_hint** | *(empty)* |

## Status

**DONE** — 16-byte record slide for vector insert; renamed from `CDSDirtyRectList_ShiftEntries` to `CDSApp_DirtyRectList_SlideRecords` (R6 `CDSApp_DirtyRectList_*` family).

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x0042a130` | `CDSDirtyRectList_ShiftEntries` | `CDSApp_DirtyRectList_SlideRecords` | `void __cdecl(dst, src, count)`: memmove 4×`DWORD` records (`0x10` bytes each), forward if `dst<src` else reverse | Decompile loop copies four dwords per element; xref sole caller `CDSApp_DirtyRectList_InsertAt@0x0042ac4a`; [round6_logic_task_13_report.md](../logic_recovery/round6_logic_task_13_report.md) |

### Xrefs (1)

| From | Context |
|------|---------|
| `0x0042ac4a` | `CDSApp_DirtyRectList_InsertAt` — open gap before copy |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0042a130` → `CDSApp_DirtyRectList_SlideRecords` | Success |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none**

## Remaining UNK

| Item | Reason |
|------|--------|
| `__cdecl` vs engine convention | Matches legacy export; no prototype mutation applied |
