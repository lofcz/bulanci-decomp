# Round 9 `_Globals` — Task 024 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 24 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x0044ae3a` |
| **ghidra_name** | `FUN_0044AE3A` |
| **prior_hint** | *(empty)* |
| **prior art** | R5 worker 16/48 (`__unlock@0x00449b1b`, `__lock@0x00449bf3`); R9 task 011 (`FUN_0044751C` SEH epilog pattern); R8 FUN task 26 (`FUN_004500E2`); sibling `FUN_0044AE46@0x0044ae46` (`__unlock(0xc)` in same parent) |

## Status

**PARTIAL** — Role proven as MSVC **`__freefls@4` SEH epilog** calling **`__unlock(0xd)`**; **`FUN_0044AE3A` kept** (no protocol-safe unique rename). Ghidra MCP live; decompiler comment refreshed; **`save_program bulanci.exe`**.

## Function

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x0044ae3a` | `FUN_0044ae3a` | **`__freefls@4` SEH epilog thunk:** unconditional `__unlock(0xd)` after InterlockedDecrement / optional `_free` on `param_1+0x68`; pure MSVCRT glue, not game/sim logic. | See below |

### Disassembly (Ghidra live)

```
0044ae3a  PUSH 0xd
0044ae3c  CALL 0x00449b1b          ; __unlock
0044ae41  POP  ECX
0044ae42  RET
```

**Size:** 9 B (`0x9`), matches `mapping.csv`.

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole)** | `0x0044adde` | `Runtime::MSVCRT::__freefls@4@0x0044ad2e` | After `__lock(0xd)` block on `param_1+0x68`; SEH `[EBP-4]` set to `0xfffffffe` at `0x0044add7` immediately before call |
| **Callee** | `0x00449b1b` | `__unlock` | R5 worker 16/48 rename; `LeaveCriticalSection` on lock table index |
| **Lock pair** | `0x0044adac` | `__lock(0xd)` → `Runtime::MSVCRT::__lock@0x00449bf3` | Before `InterlockedDecrement` on `*(param_1+0x68)` |
| **Sibling stub** | `0x0044ae46` | `FUN_0044ae46` | `__unlock(0xc)` epilog; called @ `0x0044ae23` after second `__lock(0xc)` in same parent |

### Caller SEH context (`__freefls@4`)

Ghidra library match: **`__freefls@4`**, **Visual Studio 2005 Release**.

```
0044adaa  PUSH 0xd
0044adac  CALL 0x00449bf3          ; __lock(0xd)
0044adb1  POP  ECX
0044adb2  MOV  dword ptr [EBP-0x4], 0x0    ; SEH try
0044adb6  MOV  EDI, dword ptr [ESI+0x68]
...
0044add7  MOV  dword ptr [EBP-0x4], 0xfffffffe
0044adde  CALL 0x0044ae3a          ; this task
0044ade3  PUSH 0xc
0044ade5  CALL 0x00449bf3          ; __lock(0xc)
...
0044ae23  CALL 0x0044ae46          ; __unlock(0xc) epilog
```

Decompile @ `__freefls@4`: `__lock(0xd); ... InterlockedDecrement ...; _Globals::FUN_0044ae3a(); __lock(0xc); ... _Globals::FUN_0044ae46();`

### mapping.csv / stub (not trusted)

```
;_Globals::FUN_0044ae3a;0x44ae3a;0x9;__stdcall;;uchar
```

Body is void thunk (`PUSH`/`CALL`/`POP`/`RET`); `uchar __stdcall` return is incorrect.

### Rename decision

| Candidate | Verdict |
|-----------|---------|
| `__unlock` | **Rejected** — duplicate of real impl @ `0x00449b1b` |
| Generic ordinal / address suffix | **Forbidden** by protocol |
| MSVC upstream export name | **None** — compiler-generated SEH epilog split, not a documented CRT export |

Same rationale as R9 task 011 (`FUN_0044751C`) and R8 task 26 (`FUN_004500E2`).

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0044ae3a` | `__freefls@4 SEH epilog: __unlock(0xd); paired __lock(0xd) @ 0x0044adac in Runtime::MSVCRT::__freefls@4@0x0044ad2e.` |
| `save_program` | `bulanci.exe` | saved |

## Frida

**none** — CRT-only xref closure; static Ghidra disasm/decompile sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Meaningful rename | **Deferred** — SEH epilog micro-stub; no unique IJG/libmad/bulanci game symbol |
| Correct calling convention / return type in Ghidra | **Pending** — PE/Ghidra show void thunk, not `uchar __stdcall` |
| `FUN_0044AE46` / `FUN_0044AC8A` sibling stubs | Separate R9 tasks 023/025 |

## Cross-links

- [r9_globals_task_011_report.md](r9_globals_task_011_report.md) — same 9 B `__unlock(N)` SEH epilog pattern
- [round5_worker_16_report.md](../struct_recovery/round5_worker_16_report.md) — `__unlock` rename + micro-stub skip policy
- [round5_worker_48_report.md](../struct_recovery/round5_worker_48_report.md) — `__unlock` proof
- [round8_fun_task_26_report.md](round8_fun_task_26_report.md) — SEH epilog thunk precedent (`FUN_004500E2`)
