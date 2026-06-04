# Round 9 `_Globals` FUN — Task 029 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 29 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim *(manifest cluster label — VA is MSVC CRT `__onexit` lock glue, not gameplay sim)* |
| **seed_address** | `0x0044c1e2` |
| **ghidra_name** | `FUN_0044C1E2` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R5 worker 48 (`__unlock@0x00449b1b`); R5 worker 16 skip table (`0x00449386`–`0x0044cc24` tiny lock/unlock wrappers) |

## Status

**PARTIAL** — MSVC CRT **`__unlock(8)` epilog thunk** for `Runtime::MSVCRT::__onexit`; disasm, xref closure, and paired `__lock(8)` call proven live in Ghidra. **No rename** (CRT glue wrapper — protocol forbids generic ordinals; real export is `__unlock@0x00449b1b`). Prototype corrected `uchar` → `void __stdcall`; decompiler comment upgraded from UNCERTAIN to proof text.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x0044c1e2` | `FUN_0044C1E2` | `FUN_0044C1E2` | **`void __stdcall` thunk:** `__unlock(8)` @ `0x00449b1b`; sole caller `FUN_00447e6c` (6 B `CALL`/`RET` epilog inside `__onexit`) | See below |

### Disasm / decompile (Ghidra MCP)

```
0044c1e2  PUSH 0x8
0044c1e4  CALL 0x00449b1b          ; __unlock
0044c1e9  POP  ECX
0044c1ea  RET
```

```c
void __stdcall _Globals::FUN_0044c1e2(void)
{
  __unlock(8);
  return;
}
```

### Xrefs (1 caller, Ghidra MCP)

| From | Kind | Context |
|------|------|---------|
| `0x00447e6c` | UNCONDITIONAL_CALL | `FUN_00447e6c` body: `CALL 0x0044c1e2` / `RET` (6 B) |

**Caller chain:**

| Step | Address | Function | Proof |
|------|---------|----------|-------|
| 1 | `0x00447e5e` | `Runtime::MSVCRT::__onexit` | `CALL 0x00447e6c` after `__onexit_nolock` |
| 2 | `0x00447e6c` | `FUN_00447e6c` | Single xref from `__onexit`; calls this task VA |
| 3 | `0x0044c1e2` | `FUN_0044C1E2` | `PUSH 8` / `CALL __unlock` |

**Paired lock (same CRT routine, not this task):**

| Address | Body | Role |
|---------|------|------|
| `0x00447e42` | `CALL 0x0044c1d9` | `__onexit` prologue — lock before `__onexit_nolock` |
| `0x0044c1d9` | `PUSH 8` / `CALL __lock@0x00449bf3` / `POP ECX` / `RET` | `__lock(8)` sibling thunk (task 028) |

### Callee

| Address | Ghidra name | Body proof |
|---------|-------------|------------|
| `0x00449b1b` | `__unlock` | `LeaveCriticalSection((LPCRITICAL_SECTION)(&DAT_004b1038)[lockIndex * 2])` — R5 worker 48 |

### Signature / size

| Source | Value |
|--------|-------|
| Ghidra (after R9) | `void __stdcall FUN_0044c1e2(void)`; body `0044c1e2`–`0044c1ea` (**9 B**) |
| `config/bulanci/mapping.csv` | `;_Globals::FUN_0044c1e2;0x44c1e2;0x9;__stdcall;;uchar` — size/cc OK; **`uchar` return incorrect** |
| `src/bulanci/_Globals.cpp` | `STUB_BODY()` stub only — not trusted |

### Cluster note (sim band)

Same 9-byte **`PUSH imm8` / `CALL __lock|__unlock` / `POP ECX` / `RET`** pattern as `FUN_0044c895` (`__unlock(0xd)`), `FUN_0044cc24`, etc. R5 worker 16 documents these as CRT lock-index wrappers — not gameplay-reachable.

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x0044c1e2` | `void FUN_0044c1e2(void)` **`__stdcall`** (was `undefined __stdcall` / decompiler `uchar`) |
| `set_decompiler_comment` | `0x0044c1e2` | Proof text: `__onexit` epilog / `__unlock(8)` / paired `FUN_0044c1d9` |
| `force_decompile` | `0x0044c1e2` | Decompiler shows `__unlock(8);` |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — CRT-only xref closure; static Ghidra disasm + `__onexit` caller proof sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Meaningful rename | CRT index-8 wrapper only; real symbol is `__unlock`; no unique bulanci/game export |
| `FUN_00447e6c` rename | Out of scope (separate FUN) |
| `mapping.csv` / `_Globals.cpp` `uchar` → `void` | Export-tree sync deferred |
| Lock index 8 semantic label in CRT | Index proven; MSVCRT internal slot name not in PE |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round5_worker_48_report.md](../struct_recovery/round5_worker_48_report.md) — `__unlock` @ `0x00449b1b`
- [round5_worker_16_report.md](../struct_recovery/round5_worker_16_report.md) — skip band for tiny lock/unlock wrappers
- [r9_globals_task_039_report.md](r9_globals_task_039_report.md) — analogous CRT `__unlock(0)` SEH epilog thunk (PARTIAL/BLOCKED pattern)
