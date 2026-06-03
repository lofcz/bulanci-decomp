# Round 7 FUN — Task 19 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 19 |
| **round** | 7 |
| **band** | sim |
| **seed_address** | `0x0042b48c` |
| **title** | FUN recovery: FUN_0042B48C @ 0x0042b48c (xrefs=1) |
| **prior_hint** | *(empty)* |

## Status

**DONE** — Member teardown after registry write in scalar shutdown; renamed from `CDSApp_ShutdownTeardownBody` to `CDSApp_ReleaseMembers`.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x0042b48c` | `CDSApp_ShutdownTeardownBody` | `CDSApp_ReleaseMembers` | Release LUT, clear `g_pApp` / `g_pInputChainHead`, shrink dirty-rect vectors @ `+0x254`/`+0x264`, `~CDSDirectSound` @ `+0x200`, release views/menu, `CDSBackBuffer_dtor` @ `+0x7c`, string release, `CDSView_dtor` | Decompile call chain; sole xref `CDSApp_ShutdownFromScalarDtor@0x0042b475`; [round6_logic_task_15_report.md](../logic_recovery/round6_logic_task_15_report.md); task 03 renamed caller |

### Xrefs (1)

| From | Context |
|------|---------|
| `0x0042b475` | `CDSApp_ShutdownFromScalarDtor` — tail after `RegWriteDword(Windowed)` |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0042b48c` → `CDSApp_ReleaseMembers` | Success |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none**

## Remaining UNK

| Item | Reason |
|------|--------|
| `__stdcall` + register globals | Uses `unaff_ESI`/`unaff_EBP` from enclosing SEH frame — prototype not normalized |
| vs `CDSApp_dtor@0x0042b560` | Lightweight window destroy path not unified |
