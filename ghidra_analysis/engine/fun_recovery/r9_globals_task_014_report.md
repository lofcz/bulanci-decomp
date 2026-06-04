# Round 9 `_Globals` FUN — Task 014 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 14 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x004486a4` |
| **ghidra_name** | `FUN_004486A4` |
| **prior_hint** | *(empty)* |

## Status

**PARTIAL** — Role proven via live Ghidra MCP: MSVC CRT **SEH epilog split** from `eh_vector_constructor_iterator` @ `0x00448657`. Conditionally calls `Runtime::MSVCRT::__ArrayUnwind` @ `0x004486bc` when `[EBP-0x20]==0`. **No rename** — compiler-split LOCAL with no standalone COFF/export symbol (same rule as R7 task 18, R8 task 26, R9 task 25).

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x004486a4` | `FUN_004486a4` | **SEH epilog of `eh_vector_constructor_iterator`:** if construction loop did not set success flag `[EBP-0x20]`, unwind partially constructed elements via `__ArrayUnwind(base, stride, count, dtor)`; else `RET`. | See below |

### Parent CRT symbol (Ghidra library match)

| Address | Name | Ghidra note |
|---------|------|-------------|
| `0x00448657` | `` `eh_vector_constructor_iterator' `` | Library Function — Single Match VS2005/2008/2010 Release |
| `0x004486bc` | `Runtime::MSVCRT::__ArrayUnwind` | Library Function — Single Match VS2005/2008/2010 Release |

`mapping.csv` row: `` ??_L@YGXPAXIHP6EX0@Z1@Z;`eh_vector_constructor_iterator';0x448657;0x4d ``.

### Xrefs (1)

| From | Context |
|------|---------|
| `0x00448697` | `` `eh_vector_constructor_iterator' `` — normal completion path after loop sets `[EBP-0x20]=1` @ `0x00448689` |

No direct game callers; parent is invoked from many ctors (`CDSChain_ctor`, `CGame_ctor`, `CPanel_ctor`, etc.) via `_eh_vector_constructor_iterator_`.

### Disasm @ `0x004486a4` (8 insns, size `0x18`)

```
004486a4  CMP  dword ptr [EBP-0x20], 0
004486a8  JNZ  004486bb
004486aa  PUSH dword ptr [EBP+0x18]    ; dtor (parent param_5)
004486ad  PUSH dword ptr [EBP-0x1c]    ; loop counter (constructed count)
004486b0  PUSH dword ptr [EBP+0xc]     ; stride (parent param_2)
004486b3  PUSH dword ptr [EBP+0x8]     ; base (parent param_1)
004486b6  CALL 0x004486bc              ; __ArrayUnwind
004486bb  RET
```

Push order matches `void __stdcall __ArrayUnwind(void *, uint, int, void (__thiscall*)(void *))`.

### Flag / frame-slot proof (parent @ `0x00448657`)

| Frame slot | Parent use | Fragment use |
|------------|------------|--------------|
| `[EBP+0x8]` | Loop base pointer | `__ArrayUnwind` arg 1 |
| `[EBP+0xc]` | Element stride | `__ArrayUnwind` arg 2 |
| `[EBP+0x10]` | Loop bound (`param_3`) | *(loop only)* |
| `[EBP+0x14]` | Constructor fn ptr | *(loop only)* |
| `[EBP+0x18]` | Destructor fn ptr | `__ArrayUnwind` arg 4 |
| `[EBP-0x1c]` | Loop counter | `__ArrayUnwind` arg 3 (partial count) |
| `[EBP-0x20]` | Zeroed @ prolog; set to `1` @ `0x00448689` on successful loop exit | Guard: unwind only when still `0` |

### Sibling pattern (task 15 neighbor)

| Address | Caller | Guard | Callee |
|---------|--------|-------|--------|
| `0x004486a4` | `` `eh_vector_constructor_iterator' `` @ `0x00448697` | `[EBP-0x20]==0` | `__ArrayUnwind` |
| `0x00448765` | `` `eh_vector_destructor_iterator' `` @ `0x00448758` | `[EBP-0x1c]==0` | `__ArrayUnwind` |

Same 0x18 B shape; both are MSVC SEH epilog splits, not game logic.

### Signature correction

| Source | Value |
|--------|-------|
| `mapping.csv` (stale) | `uchar __stdcall` |
| Ghidra (verified) | `void __stdcall FUN_004486a4(void)` — bare `RET`, no return value |

## Ghidra deltas

| Action | Detail |
|--------|--------|
| `set_function_prototype` | `void FUN_004486a4(void)` + `__stdcall` |
| `set_plate_comment` | R9 task 14 SEH epilog role + sole caller |
| `set_decompiler_comment` | Corrects stale “CBulanci::_qsort” text → `eh_vector_constructor_iterator` |
| `force_decompile` | Refreshed decompiler output |
| `save_program bulanci.exe` | Applied |

**No rename** — `FUN_004486a4` kept.

## Frida

**none** — static xref + disasm + parent library match sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Merge with parent in Ghidra | Would require EH table / function-boundary edit; out of scope for FUN-only task |
| `mapping.csv` / `_Globals.cpp` stub | Still list `uchar` return and standalone export; separate mapping pass |

## Cross-links

- [round7_fun_task_18_report.md](round7_fun_task_18_report.md) — SEH epilog fragment precedent (keep `FUN_*`)
- [round8_fun_task_26_report.md](round8_fun_task_26_report.md) — `_raise` SEH epilog thunk precedent
- [r9_globals_task_025_report.md](r9_globals_task_025_report.md) — `__freefls` SEH epilog thunk (R9 sim band)
- [CDSChain.md](../struct_recovery/CDSChain.md) — game caller of parent `eh_vector_constructor_iterator`
