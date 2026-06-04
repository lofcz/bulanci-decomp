# Round 9 `_Globals` — Task 019 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 19 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x00449bea` |
| **ghidra_name (before)** | `FUN_00449BEA` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R5 worker 16 (`__unlock@0x00449b1b`); R8 FUN task 30 (`__unlock_10@0x00454ace`); mapping.csv `__unlock_10` row |

## Status

**DONE** — Ghidra MCP live pass: 9-byte CRT epilog thunk `push 10` / `call __unlock` / `pop ecx` / `ret`; sole caller `Runtime::MSVCRT::__mtinitlocknum` after `__lock(10)`. Renamed to **`__unlock_10_mtinit_epilog`** (disambiguates byte-identical twin `__unlock_10@0x00454ace`).

## Function

| Address | Ghidra name (before → after) | Role summary | Evidence |
|---------|------------------------------|--------------|----------|
| `0x00449bea` | `FUN_00449BEA` → **`__unlock_10_mtinit_epilog`** | **`__mtinitlocknum` SEH epilog:** releases MSVC lock **10** via `__unlock(10)` after paired `__lock(10)` in the same function. | See below |

### Disassembly (Ghidra)

```
00449bea  PUSH 0xa
00449bec  CALL 0x00449b1b          ; __unlock
00449bf1  POP  ECX
00449bf2  RET
```

Size **`0x9`** — matches `mapping.csv`.

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (1)** | `0x00449bdc` | `Runtime::MSVCRT::__mtinitlocknum` | `CALL 0x00449bea` after crit-sec init / free paths |
| **Callee** | `0x00449b1b` | `__unlock` | R5 rename; `LeaveCriticalSection` on `DAT_004b1038` table |

### Caller pairing (`__mtinitlocknum`)

```
00449b93  PUSH 0xa
00449b95  CALL 0x00449bf3          ; __lock (Runtime::MSVCRT::__lock)
...
00449bdc  CALL 0x00449bea          ; __unlock_10_mtinit_epilog
```

Decompile: `__lock(10);` … init / `_free` branches … `__unlock_10_mtinit_epilog();`

Ghidra library match on parent: **`__mtinitlocknum`** (VS2005 Release).

### Duplicate thunk (rename rationale)

| VA | Ghidra name | Bytes | Caller |
|----|-------------|-------|--------|
| `0x00454ace` | `__unlock_10` | `6A 0A E8 … 59 C3` | `___lock_fhandle` |
| **`0x00449bea`** | **`__unlock_10_mtinit_epilog`** | **identical** | **`__mtinitlocknum`** |

Cannot share `__unlock_10` — symbol collision. Suffix `_mtinit_epilog` is caller-proven, not generic.

### mapping.csv / stub (not trusted for role)

```
;_Globals::FUN_00449bea;0x449bea;0x9;__stdcall;;uchar
```

`uchar` return incorrect — body is `void` thunk (`RET` only).

```cpp
uchar _Globals::FUN_00449bea() { STUB_BODY(); return 0; }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x00449bea` | `void __stdcall __unlock_10_mtinit_epilog(void)` |
| `set_decompiler_comment` | `0x00449bea` | `__mtinitlocknum` epilog + `__unlock_10` twin note |
| `set_plate_comment` | `0x00449b30` | `__mtinitlocknum` uses epilog @ `0x00449bea` |
| `force_decompile` | `0x00449bea`, `0x00449b30` | Refreshed decompiler |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — single static caller + disasm + CRT library match sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| `_Globals.cpp` / `mapping.csv` stub sync | Out of scope — FUN-only task |
| Sibling `FUN_00449386` / `FUN_004494ec` (`__unlock(4)` twins) | Tasks 017–018 |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round5_worker_16_report.md](../struct_recovery/round5_worker_16_report.md) — `__unlock` rename
- [round8_fun_task_30_report.md](round8_fun_task_30_report.md) — `__unlock_10@0x00454ace`
