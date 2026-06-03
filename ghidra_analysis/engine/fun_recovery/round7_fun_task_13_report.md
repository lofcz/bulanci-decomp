# Round 7 FUN — Task 13 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 13 |
| **round** | 7 |
| **band** | sim |
| **seed_address** | `0x0042a070` |
| **title** | FUN recovery: FUN_0042A070 @ 0x0042a070 (xrefs=1) |
| **prior_hint** | *(empty)* |

## Status

**DONE** — Dirty-rect vector search (linear or binary via comparator); sole caller `CDSApp_DirtyRectList_UpsertRect`. Name already `CDSApp_DirtyRectList_FindIndex`; verified and saved.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x0042a070` | `CDSApp_DirtyRectList_FindIndex` | *(unchanged)* | `int __thiscall(void *list, int *rect, cmp_fn, count)`: exact 16-byte `RECT` match scan, or binary search with `cmp_fn`; returns index or `~insertPos` | Disasm: `ADD ECX,0x10` stride, `SAR ESI,1` bisect, `CALL [ESP+0x20]` comparator; xref `CDSApp_DirtyRectList_UpsertRect@0x0042ae70`; parallel `FUN_00439730` for tracks ([round6_logic_task_13_report.md](../logic_recovery/round6_logic_task_13_report.md)) |

### Xrefs (1)

| From | Context |
|------|---------|
| `0x0042ae70` | `CDSApp_DirtyRectList_UpsertRect` |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0042a070` → `CDSApp_DirtyRectList_FindIndex` | Idempotent success |
| `save_program` | `bulanci.exe` | Saved (batch) |

## Frida

**none**

## Remaining UNK

| Item | Reason |
|------|--------|
| `set_function_this_type` | List header type not applied (`void *this`) |
| Comparator fn type | Third param `undefined *` — caller-supplied ordering |
