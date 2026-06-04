# Round 9 `_Globals` FUN — Task 025 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 25 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x0044ae46` |
| **ghidra_name** | `FUN_0044AE46` |
| **prior_hint** | *(empty)* |

## Status

**PARTIAL** — MSVC CRT `__freefls@4` SEH epilog thunk calling `__unlock(12)` is proven from live Ghidra disasm, decompile, and single-caller xref closure. **`FUN_0044AE46` kept** (protocol forbids generic CRT thunk names; no IJG/libmad/bulanci game symbol). Ghidra comment + `void __stdcall` prototype refreshed; **`save_program bulanci.exe`**.

## Function

| Address | Ghidra name | Proposed name | Role | Evidence |
|---------|-------------|---------------|------|----------|
| `0x0044ae46` | `FUN_0044AE46` | *(none — keep `FUN_*`)* | **`__freefls@4` SEH epilog:** unconditional `__unlock(0xc)` after locale/tlocinfo teardown guarded by `Runtime::MSVCRT::__lock(0xc)` | See below |

### Disassembly (Ghidra live)

```
0044ae46  PUSH 0xc
0044ae48  CALL 0x00449b1b          ; __unlock
0044ae4d  POP  ECX
0044ae4e  RET
```

| Metric | Value |
|--------|-------|
| Size | **9 B** (`0x9`) — matches `mapping.csv` |
| Instructions | 4 |
| Callee | `__unlock@0x00449b1b` (R5 worker 16/48 rename; `LeaveCriticalSection` on `DAT_004b1038` lock table) |
| Immediate | lock index **12** (`0xc`) |

### Caller closure (single xref)

| Caller | Call site | Context |
|--------|-----------|---------|
| `Runtime::MSVCRT::__freefls@4` @ `0x0044ad2e` | `0x0044ae23` | After `__lock(0xc)` @ `0x0044ade5`, `___removelocaleref` / `___freetlocinfo` on `[param_1+0x6c]`, SEH `[EBP-0x4]` epilog state |

Paired acquire in same function:

```
0044ade3  PUSH 0xc
0044ade5  CALL 0x00449bf3          ; Runtime::MSVCRT::__lock
...
0044ae23  CALL 0x0044ae46          ; this task — __unlock(0xc) epilog
```

Sibling stub `FUN_0044AE3A@0x0044ae3a` (R9 task 024 band-mate) does the same for lock **13** (`0xd`): `__lock(0xd)` @ `0x0044adac` → `CALL FUN_0044AE3A` @ `0x0044adde`.

### Decompile (post-mutation)

```c
void _Globals::FUN_0044ae46(void)
{
  /* __freefls@4 SEH epilog: __unlock(12); pairs Runtime::MSVCRT::__lock(0xc) in
     locale/tlocinfo path. */
  __unlock(0xc);
  return;
}
```

### mapping.csv / stub (not trusted)

```
;_Globals::FUN_0044ae46;0x44ae46;0x9;__stdcall;;uchar
```

Size `0x9` matches; `uchar` return incorrect — body is void `RET` with no meaningful AL.

### Prior art

| Source | Note |
|--------|------|
| R8 FUN task 26 (`round8_fun_task_26_report.md`) | Same CRT SEH-epilog / `__unlock` thunk pattern for `_raise` @ `0x004500e2` |
| R5 worker 16/48 | `__unlock@0x00449b1b` rename + lock-table proof |
| R5 worker 15 skip table | MSVC lock epilog thunks in `0x450000`–`0x454fff` band — callers CRT-only |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0044ae46` | `__freefls@4 SEH epilog: __unlock(12); pairs Runtime::MSVCRT::__lock(0xc) in locale/tlocinfo path.` |
| `set_function_prototype` | `0x0044ae46` | `void FUN_0044ae46(void)` + `__stdcall` |
| `force_decompile` | `0x0044ae46` | Refreshed |
| `save_program` | `bulanci.exe` | Saved |

No rename (`FUN_0044AE46` unchanged).

## Frida

**none** — CRT-only xref closure; static Ghidra disasm/decompile sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Meaningful symbol rename | **Deferred** — MSVC CRT SEH epilog micro-stub; no unique bulanci or upstream export name |
| MSVC lock index 12 human label | CRT-internal (`__lock`/locale table slot); not required for game recovery |
| `mapping.csv` / `_Globals.cpp` stub signature | Out of scope — separate mapping pass |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round8_fun_task_26_report.md](round8_fun_task_26_report.md) — `_raise` `__unlock(0)` epilog sibling pattern
- [round5_worker_16_report.md](../struct_recovery/round5_worker_16_report.md) — `__unlock` rename @ `0x00449b1b`
- [round5_worker_15_report.md](../struct_recovery/round5_worker_15_report.md) — CRT epilog thunk skip band
