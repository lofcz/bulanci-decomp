# Round 9 `_Globals` — Task 013 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 13 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim *(manifest cluster — VA is MSVC CRT `__onexit` band, not game sim)* |
| **seed_address** | `0x00447e6c` |
| **ghidra_name** | `FUN_00447E6C` |
| **prior_hint** | *(empty)* |
| **prior art** | R9 task 011 (`FUN_0044751C` SEH epilog pattern); R5 worker 16/48 (`__unlock`/`__lock`); sibling `FUN_0044c1e2` / `FUN_0044c1d9` (tasks 029/028) |

## Status

**PARTIAL** — Role proven as MSVC **`__onexit` SEH epilog** delegating to **`FUN_0044c1e2` → `__unlock(8)`**; **`FUN_00447E6C` kept** (no protocol-safe unique rename). Ghidra MCP live; decompiler + plate comments refreshed; `save_program bulanci.exe`.

## Function

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x00447e6c` | `FUN_00447e6c` | **`__onexit` SEH epilog thunk:** `CALL FUN_0044c1e2; RET` after `__onexit_nolock`; unlocks CRT lock index **8** via nested 9 B stub. Pure MSVCRT glue. | See below |

### Disassembly (Ghidra live)

```
00447e6c  CALL 0x0044c1e2          ; __unlock(8) micro-stub (task 029)
00447e71  RET
```

**Size:** 6 B (`0x6`), matches `mapping.csv`.

Nested callee `FUN_0044c1e2` @ `0x0044c1e2`:

```
0044c1e2  PUSH 0x8
0044c1e4  CALL 0x00449b1b          ; __unlock
0044c1e9  POP  ECX
0044c1ea  RET
```

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole)** | `0x00447e5e` | `Runtime::MSVCRT::__onexit@0x00447e36` | After `__onexit_nolock@0x00447d5a`; `[EBP-4] = 0xfffffffe` at `0x00447e57` immediately before call |
| **Callee** | `0x0044c1e2` | `FUN_0044c1e2` | 9 B `__unlock(8)` wrapper (R9 task 029 seed) |
| **Ultimate callee** | `0x00449b1b` | `__unlock` | R5 worker 16/48 rename |
| **Lock pair** | `0x00447e42` | `FUN_0044c1d9` → `__lock(8)` | Start of `__onexit` guarded region |

### Caller context (`__onexit`)

Ghidra library match: **`__onexit`**, **Visual Studio 2005 Release**.

```
00447e36  PUSH 0xc
00447e38  PUSH 0x4ac5d0
00447e3d  CALL 0x0044a724
00447e42  CALL 0x0044c1d9          ; __lock(8)
00447e47  AND  dword ptr [EBP-0x4], 0
00447e4b  PUSH dword ptr [EBP+0x8]
00447e4e  CALL 0x00447d5a          ; __onexit_nolock
00447e53  POP  ECX
00447e54  MOV  dword ptr [EBP-0x1c], EAX
00447e57  MOV  dword ptr [EBP-0x4], 0xfffffffe
00447e5e  CALL 0x00447e6c          ; this task
00447e63  MOV  EAX, dword ptr [EBP-0x1c]
00447e66  CALL 0x0044a769
00447e6b  RET
```

Decompile @ `__onexit`:

```c
_Globals::FUN_0044c1d9();
p_Var1 = (_onexit_t)__onexit_nolock(_Func);
_Globals::FUN_00447e6c();
return p_Var1;
```

`Runtime.cpp` export span `447E36–447E6C` (`0x36` B) ends at this VA; Ghidra correctly splits the 6 B epilog as a separate `_Globals` symbol.

### mapping.csv / stub (not trusted)

```
;_Globals::FUN_00447e6c;0x447e6c;0x6;__stdcall;;uchar
```

Body is void thunk (`CALL`/`RET`); `uchar __stdcall` return is incorrect.

### Rename decision

| Candidate | Verdict |
|-----------|---------|
| `__unlock` | **Rejected** — real impl @ `0x00449b1b`; this VA only forwards to `FUN_0044c1e2` |
| `__onexit` | **Rejected** — parent function @ `0x00447e36` already library-matched |
| Generic ordinal / address suffix | **Forbidden** by protocol |
| MSVC upstream export name | **None** — compiler-generated SEH epilog split (6 B tail), not a documented CRT export |

Same rationale as R9 task 011 (`FUN_0044751C`) and R8 task 26 (`FUN_004500E2`).

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x00447e6c` | `__onexit SEH epilog: calls FUN_0044c1e2 → __unlock(8) …` |
| `set_plate_comment` | `0x00447e6c` | epilog / sole caller note |
| `force_decompile` | `0x00447e36` | refreshed `__onexit` after comment |
| `save_program` | `bulanci.exe` | saved |

## Frida

**none** — CRT-only xref closure; static Ghidra disasm/decompile sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Meaningful rename | **Deferred** — 6 B forwarder to task-029 `FUN_0044c1e2`; no unique game/IJG symbol |
| Ghidra calling convention / return type | **Pending** — void thunk, not `uchar __stdcall` |
| Whether to merge epilog into `__onexit` function body | Cosmetic Ghidra layout; out of scope |
| `FUN_0044c1e2` / `FUN_0044c1d9` rename | **Tasks 028–029** (sibling seeds in manifest) |

## Cross-links

- [r9_globals_task_011_report.md](r9_globals_task_011_report.md) — `__calloc_impl` SEH epilog `__unlock(4)` precedent
- [round5_worker_16_report.md](../struct_recovery/round5_worker_16_report.md) — `__unlock` rename + micro-stub policy
- [round8_fun_task_26_report.md](round8_fun_task_26_report.md) — `_raise` SEH epilog thunk
- [round8_fun_task_30_report.md](round8_fun_task_30_report.md) — `___lock_fhandle` epilog PARTIAL
