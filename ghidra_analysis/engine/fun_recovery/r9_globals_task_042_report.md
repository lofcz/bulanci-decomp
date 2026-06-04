# Round 9 `_Globals` — Task 042 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 42 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec *(manifest cluster label — VA is MSVCRT `__write` band, not libjpeg)* |
| **seed_address** | `0x00452f0b` |
| **ghidra_name (expected)** | `FUN_00452F0B` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R8 FUN task 29 (`round8_fun_task_29_report.md`); R5 worker 15 CRT skip table; sibling `FUN_00452869` (`__lseeki64` unlock epilog, R8 task 28) |

## Status

**PARTIAL** — Role proven via live Ghidra MCP + PE byte match. MSVC `__write` locked-path epilog thunk calling `MSVCRT___unlock_fhandle(*(EBP+8))`. **No rename:** not a distinct CRT export or IJG/bulanci symbol (protocol forbids guessing `__write_unlock_epilog` etc.). `FUN_00452F0B` retained.

## Function

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x00452f0b` | `FUN_00452F0B` *(unchanged)* | **`__write` unlock epilog:** `push dword ptr [EBP+8]` → `MSVCRT___unlock_fhandle` @ `0x00454ad7` → `pop ecx` → `ret`. Uses caller `__write` frame slot `[EBP+8]` (file handle). | **Disasm (Ghidra + PE, 10 B):** `ff 75 08 e8 c4 1b 00 00 59 c3`. **Xrefs_to (1):** `__write` @ `0x00452efd` (`CALL` rel32 `+9`). **Callee:** `MSVCRT___unlock_fhandle` @ `0x00454ad7`. **Caller context:** after `__write_nolock` @ `0x00452873` on locked handle path (`TEST byte [fh+4],1` @ `0x00452ec3`); epilog at `0x00452efd` then `MOV EAX,[EBP-0x1c]` @ `0x00452f02`. **CRT-only closure** (R5 worker 15). |

### Disassembly (Ghidra)

```
00452f0b  PUSH  dword ptr [EBP + 0x8]
00452f0e  CALL  0x00454ad7          ; MSVCRT___unlock_fhandle
00452f13  POP   ECX
00452f14  RET
```

### Caller site @ `0x00452efd` (within `__write`)

```
00452eca  ...                       ; locked path: args to __write_nolock
00452ed3  CALL  0x00452873          ; __write_nolock
00452edb  MOV   [EBP-0x1c], EAX
00452ef6  MOV   [EBP-0x4], 0xfffffffe
00452efd  CALL  0x00452f0b          ; ← sole xref (this task)
00452f02  MOV   EAX, [EBP-0x1c]
00452f05  CALL  0x0044a769          ; SEH epilog helper
```

### Decompile (Ghidra `force_decompile`)

```c
void FUN_00452f0b(void)
{
  MSVCRT___unlock_fhandle(*(uint *)(unaff_EBP + 8));
  return;
}
```

### mapping.csv stub (not trusted)

```
;_Globals::FUN_00452f0b;0x452f0b;0xa;__stdcall;;uchar
```

*(Size `0xa` matches PE/Ghidra body; `__stdcall` / `uchar` return incorrect — body is void epilog ending in `RET`.)*

### `_Globals.cpp` stub (not trusted)

```cpp
uchar _Globals::FUN_00452f0b() { STUB_BODY(); return 0; }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x00452f0b` → `void FUN_00452f0b(void)` | Success (replaces stub `uchar __stdcall`) |
| `set_decompiler_comment` | `0x00452f0b` | R9 refresh: `__write` epilog / `MSVCRT___unlock_fhandle` / sole xref |
| `force_decompile` | `0x00452f0b` | Verified body |
| `save_program` | `bulanci.exe` | Saved after mutations |

*(R8 task 29 already applied `set_decompiler_comment`; R9 refreshed text + prototype fix.)*

## Frida

**none** — CRT-only xref closure; static Ghidra + PE proof sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Meaningful rename to MSVC export name | **N/A** — inlined epilog fragment, not `__unlock_fhandle` export itself |
| `mapping.csv` / `_Globals.cpp` stub signature | **Out of scope** — FUN-only task; stubs still show `uchar __stdcall` |
| Ghidra calling convention on prototype | Minor — shows `void __stdcall` after fix; epilog is not a normal stdcall entry |
| libjpeg / game reachability | **Proven absent** — only `__write` xref |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round8_fun_task_29_report.md](round8_fun_task_29_report.md)
- [round8_fun_task_28_report.md](round8_fun_task_28_report.md) — sibling `FUN_00452869` / `__lseeki64`
- [round5_worker_15_report.md](../struct_recovery/round5_worker_15_report.md)
