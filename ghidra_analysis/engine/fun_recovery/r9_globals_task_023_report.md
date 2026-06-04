# Round 9 `_Globals` — Task 023 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 23 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x0044ac8a` |
| **ghidra_name** | `FUN_0044AC8A` |
| **prior_hint** | *(empty)* |
| **prior art** | R5 worker 16/48 (`__unlock@0x00449b1b`, `__lock@0x00449bf3`); R9 task 011 (`FUN_0044751C` — same 9 B SEH epilog pattern); R9 task 039 (`FUN_004500E2` — conditional unlock epilog in `_raise`) |

## Status

**PARTIAL** — Role proven as MSVC **`__initptd` SEH epilog** calling **`__unlock(12)`**; **`FUN_0044AC8A` kept** (no protocol-safe unique rename). Ghidra MCP live; decompiler comment refreshed from uncertain → proven pairing.

## Function

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x0044ac8a` | `FUN_0044ac8a` | **`__initptd` SEH epilog thunk:** unconditional `__unlock(12)` after per-thread data init try/finally; pure MSVCRT glue (sim band = CRT/TLS cluster, not gameplay). | See below |

### Disassembly (Ghidra live)

```
0044ac8a  PUSH 0xc
0044ac8c  CALL 0x00449b1b          ; __unlock
0044ac91  POP  ECX
0044ac92  RET
```

**Size:** 9 B (`0x9`), matches `mapping.csv`.

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole)** | `0x0044ac7f` | `Runtime::MSVCRT::__initptd@0x0044abdf` | SEH `[EBP-4]` set to `0xfffffffe` at `0x0044ac78` immediately before call; function body ends at `0x0044ac89`, epilog split at `0x0044ac8a` |
| **Callee** | `0x00449b1b` | `__unlock` | R5 worker 48 rename; `LeaveCriticalSection` on lock table index |
| **Lock pair** | `0x0044ac51` | `__lock(12)` → `Runtime::MSVCRT::__lock@0x00449bf3` | Before locale/thread setup in same guarded path |

### Caller SEH context (`__initptd`)

Ghidra: **`__initptd`**, signature `void __cdecl __initptd(_ptiddata _Ptd, pthreadlocinfo _Locale)`.

```
0044ac51  PUSH 0xc
0044ac53  CALL 0x00449bf3          ; __lock(12)
0044ac58  POP  ECX
0044ac59  AND  dword ptr [EBP-0x4], 0x0    ; SEH try body
...
0044ac78  MOV  dword ptr [EBP-0x4], 0xfffffffe
0044ac7f  CALL 0x0044ac8a          ; this task
0044ac84  CALL 0x0044a769          ; epilog continuation
0044ac89  RET
```

Decompile @ `__initptd`: ends with `FUN_0044ac8a();` after guarded init block.

### mapping.csv / stub (not trusted)

```
;_Globals::FUN_0044ac8a;0x44ac8a;0x9;__stdcall;;uchar
```

Body is void thunk (`PUSH`/`CALL`/`POP`/`RET`); `uchar __stdcall` return is incorrect.

`_Globals.cpp` places this between `Runtime::MSVCRT::_initptd` (`44ABDF-44AC8A` range label) and `_getptd_noexit@0x0044ac93` — the 9 B at `0x0044ac8a` is the split-out epilog, not part of the `_initptd` stub body.

### Rename decision

| Candidate | Verdict |
|-----------|---------|
| `__unlock` | **Rejected** — duplicate of real impl @ `0x00449b1b` |
| Generic ordinal / address suffix | **Forbidden** by protocol |
| MSVC upstream export name | **None** — compiler-generated SEH epilog split, not a documented CRT export |

Same rationale as R9 task 011 (`FUN_0044751C` kept, comment only).

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0044ac8a` | `__initptd SEH epilog: __unlock(12); paired __lock(12) @ 0x0044ac51 in Runtime::MSVCRT::__initptd@0x0044abdf.` |
| `save_program` | `bulanci.exe` | saved |

## Frida

**none** — CRT-only xref closure; static Ghidra disasm/decompile sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Meaningful rename | **Deferred** — SEH epilog micro-stub; no unique IJG/libmad/bulanci game symbol |
| Correct calling convention / return type in Ghidra | **Pending** — PE/Ghidra show void thunk, not `uchar __stdcall` |
| Merge epilog back into `__initptd` function body | Cosmetic Ghidra layout; out of scope |

## Cross-links

- [r9_globals_task_011_report.md](r9_globals_task_011_report.md) — identical 9 B `__unlock(N)` epilog pattern (`FUN_0044751C`, lock index 4)
- [round5_worker_48_report.md](../struct_recovery/round5_worker_48_report.md) — `__unlock` proof
- [rng.md](../../rng.md) — `__getptd` / TLS cluster @ `0x0044ad16`
