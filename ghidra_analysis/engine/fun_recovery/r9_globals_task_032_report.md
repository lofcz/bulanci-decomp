# Round 9 `_Globals` — Task 032 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 32 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim *(manifest cluster — VA is MSVC CRT `___updatetmbcinfo` SEH epilog, not game sim logic)* |
| **seed_address** | `0x0044c895` |
| **ghidra_name** | `FUN_0044C895` |
| **prior_hint** | *(empty)* |
| **prior art** | R5 worker 16/48 (`__unlock@0x00449b1b`, `__lock@0x00449bf3`); R9 task 011 (`FUN_0044751C` — `__calloc_impl` epilog); R9 task 025 (`FUN_0044AE46` — `__freefls@4` epilog); R8 FUN task 26 (`FUN_004500E2` — `_raise` epilog) |

## Status

**PARTIAL** — Role proven as MSVC **`___updatetmbcinfo` SEH epilog** calling **`__unlock(0xd)`**; **`FUN_0044C895` kept** (no protocol-safe unique rename). Ghidra MCP live; decompiler comment refreshed; program saved.

## Function

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x0044c895` | `FUN_0044c895` | **`___updatetmbcinfo` SEH epilog thunk:** unconditional `__unlock(0xd)` after MBC info update try/finally; pure MSVCRT glue. | See below |

### Disassembly (Ghidra live)

```
0044c895  PUSH 0xd
0044c897  CALL 0x00449b1b          ; __unlock
0044c89c  POP  ECX
0044c89d  RET
```

**Size:** 9 B (`0x9`), matches `mapping.csv`.

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole)** | `0x0044c88b` | `Runtime::MSVCRT::___updatetmbcinfo@0x0044c7fa` | After `InterlockedIncrement` on default `ptmbcinfo`; SEH `[EBP-4]` set to `0xfffffffe` at `0x0044c884` immediately before call |
| **Callee** | `0x00449b1b` | `__unlock` | R5 worker 16/48 rename; `LeaveCriticalSection` on lock table index |
| **Lock pair** | `0x0044c836` | `__lock(0xd)` → `Runtime::MSVCRT::__lock@0x00449bf3` | Start of guarded MBC update path (`TEST`/`JZ` at `0x0044c812`–`0x0044c815`) |

### Caller SEH context (`___updatetmbcinfo`)

Ghidra library match: **`___updatetmbcinfo`**, **Visual Studio 2005 Release**.

```
0044c834  PUSH 0xd
0044c836  CALL 0x00449bf3          ; __lock(0xd)
0044c83b  POP  ECX
0044c83c  AND  dword ptr [EBP-0x4], 0x0    ; SEH try
...
0044c884  MOV  dword ptr [EBP-0x4], 0xfffffffe
0044c88b  CALL 0x0044c895          ; this task
0044c890  JMP  0x0044c820
```

Decompile @ `___updatetmbcinfo`: `__lock(0xd); … InterlockedIncrement(…); _Globals::FUN_0044c895();`

### mapping.csv / stub (not trusted)

```
;_Globals::FUN_0044c895;0x44c895;0x9;__stdcall;;uchar
```

Body is void thunk (`PUSH`/`CALL`/`POP`/`RET`); `uchar __stdcall` return is incorrect.

### Rename decision

| Candidate | Verdict |
|-----------|---------|
| `__unlock` | **Rejected** — duplicate of real impl @ `0x00449b1b` |
| Generic ordinal / address suffix | **Forbidden** by protocol |
| MSVC upstream export name | **None** — compiler-generated SEH epilog split, not a documented CRT export |

Same rationale as R9 task 011 (`FUN_0044751C` kept) and R8 task 26 (`FUN_004500E2` kept).

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0044c895` | `___updatetmbcinfo SEH epilog: __unlock(0xd); paired __lock(0xd) @ 0x0044c836 in Runtime::MSVCRT::___updatetmbcinfo@0x0044c7fa.` |
| `save_program` | `bulanci.exe` | saved |

## Frida

**none** — CRT-only xref closure; static Ghidra disasm/decompile sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Meaningful rename | **Deferred** — SEH epilog micro-stub; no unique IJG/libmad/bulanci game symbol |
| Correct calling convention / return type in Ghidra | **Pending** — body is void thunk, not `uchar __stdcall` |
| Move `__unlock`/`__lock` into `Runtime::MSVCRT` namespace | Cosmetic (R5 worker 16 note); out of scope |

## Cross-links

- [r9_globals_task_011_report.md](r9_globals_task_011_report.md) — `__calloc_impl` epilog precedent
- [r9_globals_task_025_report.md](r9_globals_task_025_report.md) — `__freefls@4` epilog sibling pattern
- [round5_worker_16_report.md](../struct_recovery/round5_worker_16_report.md) — `__unlock` rename + micro-stub skip policy
- [round5_worker_48_report.md](../struct_recovery/round5_worker_48_report.md) — `__unlock` proof
- [round8_fun_task_26_report.md](round8_fun_task_26_report.md) — SEH epilog thunk precedent (`FUN_004500E2`)
