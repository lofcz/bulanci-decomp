# Round 9 `_Globals` — Task 012 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 12 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim *(manifest cluster — VA is MSVC CRT / `_realloc` glue, not gameplay sim)* |
| **seed_address** | `0x004476c1` |
| **ghidra_name** | `FUN_004476c1` |
| **prior_hint** | *(empty)* |
| **prior art** | R9 task 010 sibling `FUN_004473E8` (`_free` unlock-4 epilog); R8 FUN task 30; R5 worker 48 (`__unlock@0x00449b1b`); R8 FUN task 22 (`___sbh_find_block` in `_realloc`) |

## Status

**PARTIAL** — Role proven live in Ghidra: MSVC `_realloc` SBH-path SEH epilog calling `__unlock(4)`; paired `__lock(4)` in same function. Identical 9 B body to `FUN_004473E8` (`_free` unlock-4). No unique export name — **`FUN_004476c1` kept**. Prototype `void __cdecl`; decompiler comment; `save_program bulanci.exe`.

## Function

| Address | Ghidra name | Proposed name | Role | Evidence |
|---------|-------------|---------------|------|----------|
| `0x004476c1` | `FUN_004476c1` | *(none — keep `FUN_*`)* | **`_realloc` SEH epilog thunk:** `__unlock(4)` after SBH critical-section work | See below |

### Disassembly (Ghidra live)

Size **9 B** (`0x004476c1`–`0x004476c9`), matches `mapping.csv` `0x9`:

```
004476c1  PUSH 0x4
004476c3  CALL 0x00449b1b          ; __unlock (R5 worker 48)
004476c8  POP  ECX
004476c9  RET
```

### Xref closure

| Kind | Address | Context |
|------|---------|---------|
| **Caller (sole)** | `0x0044768e` | `CALL 0x004476c1` inside `Runtime::MSVCRT::_realloc` @ `0x004474e0` |
| **Callee** | `0x00449b1b` | `__unlock` — `LeaveCriticalSection` on lock table index 4 |

Caller site immediately follows SEH epilog marker write:

```
00447687  MOV  dword ptr [EBP-0x4], 0xfffffffe
0044768e  CALL 0x004476c1
00447693  CMP  dword ptr [EBP-0x20], 0
```

### Paired acquire (`__lock(4)`)

Within the same `_realloc` SBH loop (path when `DAT_004ba1f4 == 3`):

```
004475cc  PUSH 0x4
004475ce  CALL 0x00449bf3          ; Runtime::MSVCRT::__lock
004475d3  POP  ECX
...
004475d8  CALL 0x00449c6c          ; ___sbh_find_block thunk
```

Decompile shows `_Globals::FUN_004476c1()` invoked once after SBH resize/alloc/free block handling and before the `local_24 == 0` / `HeapReAlloc` fallback branch.

### mapping.csv / stub (not trusted)

```
;_Globals::FUN_004476c1;0x4476c1;0x9;__stdcall;;uchar
```

Size `0x9` matches Ghidra. `uchar` return and meaningful `__stdcall` arity are **not supported** — body is void thunk (`RET` with no EAX semantics).

### `_Globals.cpp` stub (not trusted)

```cpp
uchar _Globals::FUN_004476c1() { STUB_BODY(); return 0; }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x004476c1` | `void __cdecl` (was `undefined __stdcall` / mapping `uchar`) |
| `set_decompiler_comment` | `0x004476c1` | `_realloc` SBH-path SEH epilog + `__unlock(4)` pairing note |
| `force_decompile` | `0x004476c1` | Refreshed; shows `__unlock(4)` |
| `save_program` | `bulanci.exe` | Saved after mutations |

No `rename_function_by_address` — CRT epilog micro-stub; protocol forbids generic ordinals; no upstream named export at this VA.

## Frida

**none** — single static caller/callee closure; CRT-only.

## Remaining UNK

| Item | Status |
|------|--------|
| Meaningful rename | **Deferred** — compiler-generated SEH epilog slice; same class as R8 `FUN_00454ACE` / R9 task 039 `FUN_004500E2` |
| `mapping.csv` / `_Globals.cpp` signature (`uchar __stdcall`) | Out of scope — FUN-only task; live Ghidra shows void 9-byte thunk |
| Move symbol to `Runtime::MSVCRT` namespace | Cosmetic; behavior proven at current `_Globals` export |

## Cross-links

- [r9_globals_task_010_report.md](r9_globals_task_010_report.md) — sibling `FUN_004473E8` (`_free` unlock-4, same 9 B)
- [round8_fun_task_30_report.md](round8_fun_task_30_report.md) — `__unlock(N)` epilog stub pattern
- [round8_fun_task_22_report.md](round8_fun_task_22_report.md) — `___sbh_find_block` / `_realloc` pairing
- [round5_worker_48_report.md](../struct_recovery/round5_worker_48_report.md) — `__unlock@0x00449b1b` rename
