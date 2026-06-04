# Round 9 `_Globals` FUN — Task 018 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 18 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x004494ec` |
| **ghidra_name** | `FUN_004494EC` |
| **prior_hint** | (empty) |

## Status

**PARTIAL** — Live Ghidra disasm + xref closure prove MSVC CRT **heap lock #4 SEH epilog stub** (`push 4` / `call __unlock` / `pop ecx` / `ret`). Proposed name `__unlock_4` matches `__unlock_10@0x00454ace` precedent but is **not unique** (five identical 9 B stubs across `_free` / `_realloc` / `_V6_HeapAlloc` / `__msize`). **`FUN_*` kept**; decompiler comment + `void __stdcall` prototype refreshed; **`save_program` applied**.

## Function

| Address | Ghidra name | Proposed name | Role | Evidence |
|---------|-------------|---------------|------|----------|
| `0x004494ec` | `FUN_004494ec` | `__unlock_4` *(deferred)* | **MSVCRT SBH heap-lock epilog:** releases CRT lock index **4** after `__lock(4)` in `Runtime::MSVCRT::__msize`; pure CRT glue, not game/sim logic | See below |

### Disassembly (Ghidra live @ `0x004494ec`, 9 B)

```
004494ec  PUSH 0x4
004494ee  CALL 0x00449b1b          ; __unlock(int lock_num)
004494f3  POP  ECX
004494f4  RET
```

PE bytes (`orig/bulanci_insturmented.exe`): `6a 04 e8 28 06 00 00 59 c3` — matches Ghidra listing; `CALL` rel32 targets `__unlock@0x00449b1b`.

### Caller / lock pairing (single xref)

| From | Type | Context |
|------|------|---------|
| `Runtime::MSVCRT::__msize@0x00449452` | `CALL` @ `0x004494c2` | After SBH path: `PUSH 4` / `CALL __lock@0x00449bf3` @ `0x00449496`, `___sbh_find_block` via thunk @ `0x004494a0`, SEH cookie `[EBP-0x4]=0xfffffffe`, then epilog `CALL 0x004494ec` |

Decompile excerpt (`__msize`):

```c
__lock(4);
puVar3 = _Globals::___sbh_find_block(this, _Memory);
/* ... size from block header ... */
_Globals::FUN_004494ec();   /* __unlock(4) epilog */
```

Ghidra **Library Function - Single Match**: `__msize` / Visual Studio 2005 Release.

### Duplicate stubs (rename blocker)

PE scan finds **five** identical 9 B `push 4` / `call __unlock` / `pop ecx` / `ret` epilogs (lock index **4**); only `CALL` rel32 differs:

| Address | Caller (Ghidra) | Bytes (first 9 B) |
|---------|-----------------|-------------------|
| `0x004473e8` | `_free` (R9 task 010) | `6a 04 e8 2c 27 00 00 59 c3` |
| `0x0044751c` | `_realloc` | `6a 04 e8 f8 25 00 00 59 c3` |
| `0x004476c1` | `_realloc` | `6a 04 e8 53 24 00 00 59 c3` |
| `0x00449386` | `_V6_HeapAlloc` (R9 task 017) | `6a 04 e8 8e 07 00 00 59 c3` |
| **`0x004494ec`** | **`__msize`** | **`6a 04 e8 28 06 00 00 59 c3`** |

`__unlock_4` would be ambiguous across five COMDAT epilogs. R5 worker 16: defer `_unlock` thunk aliases until coordinated ([round5_worker_16_report.md](../struct_recovery/round5_worker_16_report.md)).

### mapping.csv / stub (not trusted for body)

| Source | Value |
|--------|-------|
| `config/bulanci/mapping.csv` | `_Globals::FUN_004494ec`; `0x9` B; `__stdcall`; return `uchar` *(incorrect — body is void `RET`)* |
| `src/bulanci/_Globals.cpp` | `uchar FUN_004494ec() { STUB_BODY(); }` |

### Naming precedent (not applied here)

| Address | Name | Lock arg | Xrefs |
|---------|------|----------|-------|
| `0x00449b1b` | `__unlock` | `int lock_num` | 18 (R5 worker 48) |
| `0x00454ace` | `__unlock_10` | fixed **10** | 1 (`___lock_fhandle`) |

`__unlock_4` dry-run rename succeeded but would collide across **five** lock-4 epilogs; coordinator should rename all together or leave as `FUN_*`.

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x004494ec` | SEH epilog role + caller @ `0x4494c2` + five lock-4 duplicate note |
| `set_function_prototype` | `0x004494ec` | `void stub(void)` + `__stdcall` |
| `save_program` | `bulanci.exe` | saved |

No `rename_function_by_address` — non-unique `__unlock_4` candidate.

## Frida

**none** — CRT-only xref closure; static Ghidra disasm/decompile sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Rename to `__unlock_4` | Blocked by five identical lock-4 epilogs (`0x4473e8`–`0x4494ec`) |
| `mapping.csv` / `_Globals.cpp` sync | Out of scope for FUN-only task; still `FUN_004494ec` / bogus `uchar` return |
| Move `__unlock` cluster to `Runtime::MSVCRT` namespace | Cosmetic (R5 worker 16 note) |

## Cross-links

- [round5_worker_16_report.md](../struct_recovery/round5_worker_16_report.md) — skip table + duplicate stub note
- [round5_worker_48_report.md](../struct_recovery/round5_worker_48_report.md) — `__unlock@0x00449b1b`
- [round8_fun_task_22_report.md](round8_fun_task_22_report.md) — `__msize` SBH path + `___sbh_find_block`
- [round8_fun_task_30_report.md](round8_fun_task_30_report.md) — `__unlock_10` naming precedent
- [r9_globals_task_039_report.md](r9_globals_task_039_report.md) — similar CRT SEH epilog thunk policy
