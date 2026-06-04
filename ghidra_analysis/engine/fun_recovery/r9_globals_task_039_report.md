# Round 9 `_Globals` — Task 039 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 39 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec *(manifest cluster label — VA is CRT/SEH band, not libjpeg)* |
| **seed_address** | `0x004500e2` |
| **ghidra_name (expected)** | `FUN_004500E2` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R8 FUN task 26 (`round8_fun_task_26_report.md`); R5 worker 15 skip table; R5 worker 48 `__unlock` rename @ `0x00449b1b`; R9 task 011 (`FUN_0044751C` same SEH-epilog class) |

## Status

**PARTIAL** — Live Ghidra MCP (`connect_instance("bulanci")`) confirms MSVC **`_raise` SEH epilog** thunk: conditional **`__unlock(0)`** when `[EBP-0x1c] != 0`. **`FUN_004500E2` kept** (compiler-generated CRT glue; protocol forbids guess rename). R8 decompiler comment still present; no Ghidra mutations this session.

## Function

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x004500e2` | `FUN_004500e2` | **`_raise` SEH epilog thunk:** if `[EBP-0x1c] != 0`, `__unlock(0)` @ `0x00449b1b`; else `RET`. Pure MSVCRT glue — not game/jpeg reachable. | See below |

### Disassembly (Ghidra live)

```
004500e2  CMP  dword ptr [EBP-0x1c], 0
004500e6  JZ   004500f0
004500e8  PUSH 0
004500ea  CALL 0x00449b1b          ; __unlock
004500ef  POP  ECX
004500f0  RET
```

**Size:** 15 B (`0xf`), matches `mapping.csv` and prior PE dump.

### Xref closure (Ghidra live)

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole)** | `0x004500c8` | `_raise@0x0044ff6e` | `int __cdecl _raise(int _SigNum)`; body `0x0044ff6e`–`0x0045011d` |
| **Callee** | `0x00449b1b` | `__unlock` | `void __cdecl __unlock(int lockIndex)`; R5 worker 48 rename |

### Decompile (Ghidra live, post-`force_decompile`)

```c
void _Globals::FUN_004500e2(void)
{
  if (*(int *)(unaff_EBP + -0x1c) != 0) {
    __unlock(0);
  }
  return;
}
```

PRE_COMMENT (R8 task 26, still present): documents `[EBP-0x1c]` / `local_20`, paired `__lock(0)` @ `0x450052`, sole xref `_raise@0x4500c8`.

### Caller context @ `0x004500c8` (within `_raise`)

```
004500c8  CALL 0x004500e2
004500cd  CMP  EBX, 8
...
004500dd  CMP  dword ptr [EBP-0x1c], 0
004500e1  JE   ...
004500e3  PUSH 0
004500e5  CALL 0x00449b1b          ; inline duplicate epilog path in _raise
```

### Rename decision

| Candidate | Verdict |
|-----------|---------|
| `__unlock` | **Rejected** — duplicate of real impl @ `0x00449b1b` |
| `jpeg_*` / IJG export | **Rejected** — band label only; zero jpeg xrefs |
| Generic ordinal / address suffix | **Forbidden** by protocol |
| MSVC upstream export name | **None** — compiler-generated SEH epilog split |

Same rationale as R8 task 26 and R9 task 011 (`FUN_0044751C`).

### mapping.csv stub (not trusted)

```
;_Globals::FUN_004500e2;0x4500e2;0xf;__stdcall;;uchar
```

Ghidra signature still `undefined __stdcall FUN_004500e2(void)` — body is void thunk (`RET` only); `uchar __stdcall` incorrect (same as task 011 pending item).

### `_Globals.cpp` stub (not trusted)

```cpp
uchar _Globals::FUN_004500E2() { STUB_BODY(); return 0; }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| *(none)* | — | Comment already complete from R8; no prototype/rename change applied |

## Frida

**none** — CRT-only xref closure; static Ghidra disasm/decompile sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Meaningful rename | **Deferred** — SEH epilog micro-stub; no unique IJG/libmad/bulanci game symbol |
| Correct calling convention / return type in Ghidra | **Pending** — live body void `RET`; Ghidra lists `undefined __stdcall`, mapping `uchar __stdcall` |
| Move `__unlock` into `Runtime::MSVCRT` namespace | Cosmetic (R5 worker 48 note); out of scope |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round8_fun_task_26_report.md](round8_fun_task_26_report.md)
- [r9_globals_task_011_report.md](r9_globals_task_011_report.md)
- [round5_worker_15_report.md](../struct_recovery/round5_worker_15_report.md)
- [round5_worker_48_report.md](../struct_recovery/round5_worker_48_report.md)
