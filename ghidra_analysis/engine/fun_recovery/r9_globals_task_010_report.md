# Round 9 `_Globals` — Task 010 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 10 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim *(manifest cluster label — VA is MSVC CRT heap-lock band, not game sim logic)* |
| **seed_address** | `0x004473e8` |
| **ghidra_name** | `FUN_004473E8` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R5 worker 48 (`__unlock` @ `0x00449b1b`, `__lock` @ `0x00449bf3`); R8 FUN pool entry; sibling stubs `FUN_0044751c` / `FUN_004476c1` (identical 9 B bodies) |

## Status

**PARTIAL** — Role proven: MSVC CRT `_free` SBH-path SEH epilog thunk calling `__unlock(4)`. **No rename** (protocol: CRT micro-stub; no unique IJG/libmad/bulanci game symbol; same pattern as R8/R9 deferred `FUN_*` SEH epilogs). Ghidra prototype corrected; decompiler comment refreshed; `save_program bulanci.exe`.

## Function

| Address | Ghidra name | Proposed name | Role | Evidence |
|---------|-------------|---------------|------|----------|
| `0x004473e8` | `FUN_004473e8` | *(keep `FUN_*`)* | **`_free` heap-lock unlock epilog:** 9-byte thunk `push 4` → `__unlock` @ `0x00449b1b` → `pop ecx` → `ret`; pairs `__lock(4)` in `Runtime::MSVCRT::_free` SBH branch | See below |

### Disassembly (Ghidra + PE)

```
004473e8  PUSH 0x4
004473ea  CALL 0x00449b1b          ; __unlock
004473ef  POP  ECX
004473f0  RET
```

PE bytes @ `orig/bulanci.exe` (image base `0x00400000`): **`6a 04 e8 2c 27 00 00 59 c3`** (9 B, matches `mapping.csv` size `0x9`).

### Caller closure (single xref)

| From | In function | Context |
|------|-------------|---------|
| `0x004473d8` `CALL 0x004473e8` | `Runtime::MSVCRT::_free` @ `0x00447392` (size `0x85`, Ghidra library match) | After `__lock(4)` @ `0x004473b0` → `__lock` @ `0x00449bf3`, `___sbh_find_block`, optional `___sbh_free_block`; SEH cookie `-4` → `-2` then unlock epilog |

Lock/unlock pairing in `_free` SBH path (`DAT_004ba1f4 == 3`):

```
004473ae  PUSH 0x4
004473b0  CALL 0x00449bf3          ; __lock(4)
...
004473d8  CALL 0x004473e8          ; this stub → __unlock(4)
```

### Callee

| VA | Ghidra name | Role |
|----|-------------|------|
| `0x00449b1b` | `__unlock` | MSVC lock-table unlock: `LeaveCriticalSection((&DAT_004b1038)[param*2])` ([round5_worker_48_report.md](../struct_recovery/round5_worker_48_report.md)) |

### Sibling stubs (same bytes, different CRT parents — not this task)

| VA | Caller | Lock pair |
|----|--------|-----------|
| `0x0044751c` | `__calloc_impl` @ `0x004474b8` | `__lock(4)` / `__unlock(4)` |
| `0x004476c1` | `_realloc` @ `0x0044768e` | `__lock(4)` / `__unlock(4)` |

### mapping.csv / stub (not trusted for semantics)

```
;_Globals::FUN_004473e8;0x4473e8;0x9;__stdcall;;uchar
```

Size matches PE; **`__stdcall` / `uchar` incorrect** — body is void `RET` with no meaningful AL (corrected in Ghidra to `void __cdecl`).

```cpp
uchar _Globals::FUN_004473e8() { STUB_BODY(); return 0; }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x004473e8` | `void FUN_004473e8(void)`, `__cdecl` |
| `set_decompiler_comment` | `0x004473e8` | SBH `_free` epilog / `__unlock(4)` proof comment |
| `force_decompile` | `0x004473e8` | Refreshed; shows `__unlock(4)` |
| `save_program` | `bulanci.exe` | Saved |
| `rename_function_by_address` | — | **Not applied** (PARTIAL) |

## Frida

**none** — CRT-only xref closure; static Ghidra disasm + PE bytes sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Meaningful Ghidra rename | **Deferred** — identical 9 B `__unlock(4)` thunks exist for `_calloc` / `_realloc`; no unique MSVC export symbol at this VA |
| `mapping.csv` / `_Globals.cpp` stub signature | Out of scope — separate mapping pass |
| Move stub to `Runtime::MSVCRT` namespace | Cosmetic; behavior proven under `_Globals` |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round5_worker_48_report.md](../struct_recovery/round5_worker_48_report.md) — `__unlock` / `__lock` rename
- [round5_worker_16_report.md](../struct_recovery/round5_worker_16_report.md) — micro-stub skip policy
- [r9_globals_task_039_report.md](r9_globals_task_039_report.md) — parallel CRT SEH epilog task pattern
