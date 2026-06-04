# Round 9 `_Globals` — Task 011 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 11 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x0044751c` |
| **ghidra_name** | `FUN_0044751C` |
| **prior_hint** | *(empty)* |
| **prior art** | R5 worker 16/48 (`__unlock@0x00449b1b`, `__lock@0x00449bf3`); R8 FUN task 26 (SEH epilog thunk pattern for `_raise`); sibling `FUN_004476c1@0x004476c1` (same 9 B body, `_realloc` caller — task 012) |

## Status

**PARTIAL** — Role proven as MSVC **`__calloc_impl` SEH epilog** calling **`__unlock(4)`**; **`FUN_0044751C` kept** (no protocol-safe unique rename). Ghidra MCP live; decompiler comment refreshed.

## Function

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x0044751c` | `FUN_0044751c` | **`__calloc_impl` SEH epilog thunk:** unconditional `__unlock(4)` after small-block heap alloc try/finally; pure MSVCRT glue, not game/sim logic. | See below |

### Disassembly (Ghidra live)

```
0044751c  PUSH 0x4
0044751e  CALL 0x00449b1b          ; __unlock
00447523  POP  ECX
00447524  RET
```

**Size:** 9 B (`0x9`), matches `mapping.csv`.

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole)** | `0x004474b8` | `Runtime::MSVCRT::__calloc_impl@0x00447420` | After `___sbh_alloc_block`; SEH `[EBP-4]` set to `0xfffffffe` at `0x004474b1` immediately before call |
| **Callee** | `0x00449b1b` | `__unlock` | R5 worker 16/48 rename; `LeaveCriticalSection` on lock table index |
| **Lock pair** | `0x0044749c` | `__lock(4)` → `Runtime::MSVCRT::__lock@0x00449bf3` | Immediately before `___sbh_alloc_block@0x0044a440` in same guarded path |

### Caller SEH context (`__calloc_impl`)

Ghidra library match: **`__calloc_impl`**, **Visual Studio 2005 Release**.

```
0044749a  PUSH 0x4
0044749c  CALL 0x00449bf3          ; __lock(4)
004474a1  POP  ECX
004474a2  MOV  dword ptr [EBP-0x4], EDI    ; SEH try
004474a5  PUSH dword ptr [EBP+0x8]
004474a8  CALL 0x0044a440          ; ___sbh_alloc_block
004474ad  POP  ECX
004474ae  MOV  dword ptr [EBP-0x1c], EAX
004474b1  MOV  dword ptr [EBP-0x4], 0xfffffffe
004474b8  CALL 0x0044751c          ; this task
```

Decompile @ `__calloc_impl`: `__lock(4); piVar1 = ___sbh_alloc_block(_Size); _Globals::FUN_0044751c();`

### mapping.csv / stub (not trusted)

```
;_Globals::FUN_0044751c;0x44751c;0x9;__stdcall;;uchar
```

Body is void thunk (`PUSH`/`CALL`/`POP`/`RET`); `uchar __stdcall` return is incorrect.

### Rename decision

| Candidate | Verdict |
|-----------|---------|
| `__unlock` | **Rejected** — duplicate of real impl @ `0x00449b1b` |
| Generic ordinal / address suffix | **Forbidden** by protocol |
| MSVC upstream export name | **None** — compiler-generated SEH epilog split, not a documented CRT export |

Same rationale as R8 task 26 (`FUN_004500E2` kept, comment only).

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0044751c` | `__calloc_impl SEH epilog: __unlock(4); paired __lock(4) @ 0x0044749c in Runtime::MSVCRT::__calloc_impl@0x00447420.` |
| `save_program` | `bulanci.exe` | saved |

## Frida

**none** — CRT-only xref closure; static Ghidra disasm/decompile sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Meaningful rename | **Deferred** — SEH epilog micro-stub; no unique IJG/libmad/bulanci game symbol |
| Correct calling convention / return type in Ghidra | **Pending** — PE/Ghidra show void thunk, not `uchar __stdcall` |
| Move `__unlock`/`__lock` into `Runtime::MSVCRT` namespace | Cosmetic (R5 worker 16 note); out of scope |

## Cross-links

- [round5_worker_16_report.md](../struct_recovery/round5_worker_16_report.md) — `__unlock` rename + micro-stub skip policy
- [round5_worker_48_report.md](../struct_recovery/round5_worker_48_report.md) — `__unlock` proof
- [round8_fun_task_26_report.md](round8_fun_task_26_report.md) — SEH epilog thunk precedent (`FUN_004500E2`)
