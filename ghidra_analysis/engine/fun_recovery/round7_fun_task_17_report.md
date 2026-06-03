# Round 7 FUN — Task 17 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 17 |
| **round** | 7 |
| **band** | sim |
| **seed_address** | `0x0042afd0` |
| **title** | FUN recovery: FUN_0042AFD0 @ 0x0042afd0 (xrefs=1) |
| **prior_hint** | *(empty)* |

## Status

**DONE** — Partial `CDSApp` shell initialization for MSVC `CreateObject` factory (`OperatorNew(0x280)`). Renamed `CDSApp_InitCreateObjectShell`; distinct from full `CDSApp_ctor@0x0042b170`.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x0042afd0` | `FUN_0042afd0` | `CDSApp_InitCreateObjectShell` | `undefined4 * __fastcall`: `CDSChained_ctor`; four `g_pCDSApp_vftable` words; zero dirty-rect / view fields; back-buffer vtables @ `+0x7c`; `CDSDirectSound_ctor` @ `+0x200`; SEH `LAB_00479180` | Decompile field init; xref `CreateObject@0x0042b950`; size `0x280` = 640 B `CDSApp`; [round6_logic_task_14_report.md](../logic_recovery/round6_logic_task_14_report.md) |

### Xrefs (1)

| From | Context |
|------|---------|
| `0x0042b950` | `CreateObject` — static factory after `OperatorNewWithBadAlloc(0x280)` |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0042afd0` → `CDSApp_InitCreateObjectShell` | Success |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none**

## Remaining UNK

| Item | Reason |
|------|--------|
| vs `CDSApp_ctor` | Overlapping field init; which paths use factory-only shell not re-xref'd |
| `set_function_this_type` | Still `undefined4 *param_1` in decompiler |
