# Round 9 `_Globals` — Task 017 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 17 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim *(manifest cluster label — VA is MSVC CRT heap-lock band, not game sim logic)* |
| **seed_address** | `0x00449386` |
| **ghidra_name** | `FUN_00449386` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R5 worker 48 (`__unlock` @ `0x00449b1b`, `__lock` @ `0x00449bf3`); R5 worker 16 micro-stub skip; [r9_globals_task_010_report.md](r9_globals_task_010_report.md) (identical 9 B `__unlock(4)` pattern for `_free`) |

## Status

**PARTIAL** — Role proven: MSVC CRT `_V6_HeapAlloc` SBH-path SEH epilog thunk calling `__unlock(4)`. **No rename** (protocol: CRT micro-stub; no unique IJG/libmad/bulanci game symbol; same deferred `FUN_*` SEH epilog policy as task 010). Ghidra prototype corrected; decompiler comment refreshed; `save_program bulanci.exe`.

## Function

| Address | Ghidra name | Proposed name | Role | Evidence |
|---------|-------------|---------------|------|----------|
| `0x00449386` | `FUN_00449386` | *(keep `FUN_*`)* | **`_V6_HeapAlloc` heap-lock unlock epilog:** 9-byte thunk `push 4` → `__unlock` @ `0x00449b1b` → `pop ecx` → `ret`; pairs `__lock(4)` in `Runtime::MSVCRT::_V6_HeapAlloc` | See below |

### Disassembly (Ghidra + PE)

```
00449386  PUSH 0x4
00449388  CALL 0x00449b1b          ; __unlock
0044938d  POP  ECX
0044938e  RET
```

PE bytes @ `orig/bulanci_insturmented.exe` (image base `0x00400000`): **`6a 04 e8 8e 07 00 00 59 c3`** (9 B, matches `mapping.csv` size `0x9`).

### Caller closure (single xref)

| From | In function | Context |
|------|-------------|---------|
| `0x00449378` `CALL 0x00449386` | `Runtime::MSVCRT::_V6_HeapAlloc` @ `0x00449340` (size `0x46`, Ghidra **Library: Visual Studio 2005 Release** match) | After `__lock(4)` @ `0x0044935d` → `__lock` @ `0x00449bf3`, `___sbh_alloc_block` @ `0x0044a440`; SEH cookie `-4` → `-2` then unlock epilog |

Lock/unlock pairing in `_V6_HeapAlloc` (`param_1 <= DAT_004ba1e4` branch):

```
0044935b  PUSH 0x4
0044935d  CALL 0x00449bf3          ; __lock(4)
00449368  CALL 0x0044a440          ; ___sbh_alloc_block
...
00449378  CALL 0x00449386          ; this stub → __unlock(4)
```

`Runtime.cpp` export span `449340-449386` (`size 0x46`) ends at this VA — epilog is the tail of `_V6_HeapAlloc` in the stub map; Ghidra splits it as a separate `FUN_*` because of the outgoing `CALL`.

### Callee

| VA | Ghidra name | Role |
|----|-------------|------|
| `0x00449b1b` | `__unlock` | MSVC lock-table unlock: `LeaveCriticalSection((&DAT_004b1038)[param*2])` ([round5_worker_48_report.md](../struct_recovery/round5_worker_48_report.md)) |

### Sibling stubs (same bytes, different CRT parents — not this task)

| VA | Caller | Lock pair |
|----|--------|-----------|
| `0x004473e8` | `Runtime::MSVCRT::_free` @ `0x00447392` | `__lock(4)` / `__unlock(4)` (task 010) |
| `0x004494ec` | `Runtime::MSVCRT::__msize` @ `0x00449452` | `__lock(4)` / `__unlock(4)` (task 018) |
| `0x0044751c` | `__calloc_impl` | `__lock(4)` / `__unlock(4)` |
| `0x004476c1` | `_realloc` | `__lock(4)` / `__unlock(4)` |

### mapping.csv / stub (not trusted for semantics)

```
;_Globals::FUN_00449386;0x449386;0x9;__stdcall;;uchar
```

Size matches PE; **`__stdcall` / `uchar` incorrect** — body is void `RET` with no meaningful AL (corrected in Ghidra to `void __cdecl`).

```cpp
uchar _Globals::FUN_00449386() { STUB_BODY(); return 0; }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x00449386` | `void FUN_00449386(void)`, `__cdecl` |
| `set_decompiler_comment` | `0x00449386` | `_V6_HeapAlloc` / `__unlock(4)` proof comment |
| `force_decompile` | `0x00449386` | Refreshed; shows `__unlock(4)` |
| `save_program` | `bulanci.exe` | Saved |
| `rename_function_by_address` | — | **Not applied** (PARTIAL) |

## Frida

**none** — CRT-only xref closure; static Ghidra disasm + PE bytes sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Meaningful Ghidra rename | **Deferred** — identical 9 B `__unlock(4)` thunks exist for `_free` / `__msize` / `_calloc` / `_realloc`; no unique MSVC export symbol at this VA |
| `mapping.csv` / `_Globals.cpp` stub signature | Out of scope — separate mapping pass |
| Move stub to `Runtime::MSVCRT` namespace | Cosmetic; behavior proven under `_Globals` |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round5_worker_48_report.md](../struct_recovery/round5_worker_48_report.md) — `__unlock` / `__lock` rename
- [round5_worker_16_report.md](../struct_recovery/round5_worker_16_report.md) — micro-stub skip policy
- [r9_globals_task_010_report.md](r9_globals_task_010_report.md) — parallel `_free` unlock epilog (task 010)
