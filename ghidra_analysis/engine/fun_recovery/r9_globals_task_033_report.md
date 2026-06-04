# Round 9 `_Globals` — Task 033 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 33 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x0044cc24` |
| **ghidra_name (before)** | `FUN_0044CC24` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R5 worker 16 skip table (`__unlock` wrapper cluster `0x00449386`–`0x0044cc24`); R8 manifest xref_count=1 |

## Status

**PARTIAL** — Role proven: MSVC CRT `__setmbcp` SEH epilog thunk calling `__unlock(0xd)`. **No rename** — lock index `0xd` thunk bytes are duplicated at `FUN_0044c895` (`___updatetmbcinfo` epilog, R9 task 032); only `__unlock_10@0x00454ace` is unique in-image for that lock index, so `__unlock_13` would collide or mislabel.

## Function

| Address | Ghidra (after) | Role | Evidence |
|---------|----------------|------|----------|
| `0x0044cc24` | `FUN_0044cc24` | `void __stdcall(void)`: push lock index `0xd` (13), call `__unlock@0x00449b1b`, `pop ecx`, `ret` — `__setmbcp` epilog after `__lock(0xd)` mbcs table copy | See below |

### Disasm (Ghidra MCP)

```
0044cc24  PUSH 0xd
0044cc26  CALL 0x00449b1b          ; __unlock
0044cc2b  POP  ECX
0044cc2c  RET
```

Body size **9 B** (`0044cc24`–`0044cc2c`), matches `mapping.csv` `0x9`.

### Decompile (Ghidra MCP)

```c
void __stdcall _Globals::FUN_0044cc24(void)
{
  __unlock(0xd);
  return;
}
```

### Duplicate thunk (rename blocker)

| Address | Ghidra name | Caller | Bytes |
|---------|-------------|--------|-------|
| `0x0044c895` | `FUN_0044c895` | `___updatetmbcinfo@0x0044c88b` | `6a 0d e8 7f 12 00 00 59 c3` |
| **`0x0044cc24`** | **`FUN_0044cc24`** | **`__setmbcp@0x0044cc1d`** | **`6a 0d e8 f0 ce ff ff 59 c3`** |

Same lock index, same 9 B pattern — not a unique per-VA MSVCRT export name.

### Xrefs (1)

| From | Kind | Context |
|------|------|---------|
| `0x0044cc1d` | UNCONDITIONAL_CALL | `__setmbcp@0x0044cac3` — epilog after successful mbcs table install (`__lock(0xd)` @ `0x0044cb77` … copy globals … `call __unlock_13`) |

### Sibling proof (same binary, no guess)

`__unlock_10@0x00454ace` (Ghidra + `mapping.csv`):

```
00454ace  PUSH 0xa
00454ad0  CALL 0x00449b1b
00454ad5  POP  ECX
00454ad6  RET
```

`FUN_0044cc24` differs only in lock immediate (`0xd` vs `0xa`) from `__unlock_10`; MSVC would label both `__unlock_13`, but this binary has **two** `0xd` copies (see duplicate table) — rename deferred per ROUND9 no-guess / uniqueness rule.

### Callee

| Address | Ghidra name | Signature |
|---------|-------------|-----------|
| `0x00449b1b` | `__unlock` | `void __cdecl __unlock(int lockIndex)` — R5 worker 48 rename; indexes `dword ptr [EAX*8+0x4b1038]` |

### mapping.csv stub (not trusted)

```
;_Globals::FUN_0044cc24;0x44cc24;0x9;__stdcall;;uchar
```

Ghidra corrected to `void __stdcall __unlock_13(void)` (void thunk, not `uchar` return).

### `_Globals.cpp` stub (not trusted)

```cpp
uchar _Globals::FUN_0044cc24() { STUB_BODY(); return 0; }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `void __stdcall FUN_0044cc24(void)` | Success (was `undefined` / wrong `uchar`) |
| `set_decompiler_comment` | `0x0044cc24` | Success (`__setmbcp` epilog + duplicate `FUN_0044c895` note) |
| `force_decompile` | `0x0044cc24` | Success |
| `rename_function_by_address` | → `__unlock_13` | **Reverted** — duplicate `0xd` thunk @ `0x0044c895` |
| `save_program` | `bulanci.exe` | Success (retry after transaction lock) |

## Frida

**none** — static CRT xref + byte-identical `__unlock_10` sibling sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| `mapping.csv` / `_Globals.cpp` / `_Globals.h` sync to `__unlock_13` | **Deferred** — export-tree pass out of scope |
| Per-VA `__unlock_13` rename | **Blocked** — duplicate `0xd` epilog @ `FUN_0044c895`; cluster rename out of scope |
| Official MSVC COFF export string | **N/A** — static CRT internal |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round5_worker_16_report.md](../struct_recovery/round5_worker_16_report.md) — skip-table cluster hint
- [r9_globals_task_039_report.md](r9_globals_task_039_report.md) — related `__unlock(0)` `_raise` epilog (different lock index)
