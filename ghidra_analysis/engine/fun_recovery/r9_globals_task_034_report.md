# Round 9 `_Globals` FUN — Task 034 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 34 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x0044d594` |
| **ghidra_name** | `FUN_0044D594` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R8 FUN task 27 (`round8_fun_task_27_report.md`) — `__init_pointers` slot 4 table |

## Status

**PARTIAL** — Live Ghidra MCP confirms a **10-byte** MSVC **`__init_pointers`** split thunk: stores **`__encoded_null()`** (passed as `encoded_null`) into **`DAT_004b8e34`**, the encoded handler slot consumed by **`__purecall@0x00448a6d`** (`__decode_pointer` then indirect call). **No rename:** no unique VS2005 **`__initp_*`** export for this slot; sibling thunks remain `FUN_*` (R8/R9 no-guess rule). Prototype and comments refreshed; **`save_program bulanci.exe`** succeeded.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x0044d594` | `FUN_0044d594` | **`FUN_0044d594`** | **`__init_pointers` encoded-null store** — `DAT_004b8e34 = encoded_null; return` | 3 insn / **0xA** B; disasm below; **1×** CODE xref from **`__init_pointers@0x0044c451`**; **`get_xrefs_to(0x004b8e34)`** → **WRITE** here, **READ** in `__purecall` |

### Disassembly proof (Ghidra MCP)

```
0044d594  MOV EAX,dword ptr [ESP+0x4]   ; encoded_null (__cdecl arg)
0044d598  MOV [0x004b8e34],EAX          ; DAT_004b8e34
0044d59d  RET
```

### Decompile (Ghidra MCP, post-mutation)

```c
void __cdecl _Globals::FUN_0044d594(undefined4 encoded_null)
{
  DAT_004b8e34 = encoded_null;
  return;
}
```

### `__init_pointers` call order (VS2005 MSVCRT, live disasm @ `0x0044c436`)

| Order | Callee | Target global | Known consumer |
|-------|--------|---------------|----------------|
| 1 | `FUN_0044a916` | `DAT_004b873c` | `__callnewh` |
| 2 | `FUN_0044faab` | `DAT_004b8f80` | `InitializeCriticalSectionAndSpinCount` |
| 3 | `FUN_0044a942` | `DAT_004b8744` | `__invalid_parameter` |
| **4** | **`FUN_0044d594`** | **`DAT_004b8e34`** | **`__purecall`** |
| 5 | `FUN_0045011e` | `DAT_004b8f98` | none in export (R8 task 27) |
| 6 | `__initp_misc_winsig` | `DAT_004b8f84`–`0x004b8f90` | `___get_sigabrt` / `_raise` |
| 7 | `FUN_00448a97` | — | CRT empty thread hook (R9 task 16) |
| 8 | `__initp_eh_hooks` | EH hook slots | SEH init |

Arg to slots 1–5: `ESI` from **`__encoded_null()`** @ `0x0044aacf` immediately before the thunk chain.

### Consumer closure (`DAT_004b8e34`)

| Step | Address | Symbol | Proof |
|------|---------|--------|-------|
| Write | `0x0044d598` | `FUN_0044d594` | Sole **WRITE** xref to global |
| Read | `0x00448a6d` | `__purecall` | `PUSH dword ptr [0x004b8e34]` → `CALL __decode_pointer` → `TEST EAX` / `CALL EAX` if non-zero |
| Vtable DATA | `0x0048752c` | `CDSException` vftable slot 3 | `read_memory@0x00487520` → slot3 = `0x00448a6d` |
| Vtable DATA | `0x00481ab4`, `0x00481abc` | `ODSImage` | `get_xrefs_to(__purecall)` — purecall stubs |

`vftable_methods.csv`: `CDSException;00487520;;3;00448a6d;CDSException::__purecall;1` — slot label is vtable role, not this 10-byte thunk.

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole CODE)** | `0x0044c451` | `__init_pointers` | `CALL FUN_0044d594` after slot 3 (`FUN_0044a942`); before `FUN_0045011e` |
| **Global write** | `0x0044d598` | `DAT_004b8e34` | Sole write site |

### Signature / size

| Source | Value |
|--------|-------|
| Ghidra (after) | `void __cdecl FUN_0044d594(undefined4 encoded_null)`; body `0044d594`–`0044d59d` (**0xA** B) |
| `config/bulanci/mapping.csv` | `;_Globals::FUN_0044d594;0x44d594;0xa;__cdecl;;uchar;uint` — size OK; **return `uchar` incorrect** (void thunk) |
| `src/bulanci/_Globals.cpp` | `uchar FUN_0044d594(uint)` stub — not trusted |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x0044d594` | `void __cdecl FUN_0044d594(undefined4 encoded_null)` |
| `set_decompiler_comment` | `0x0044d594` | Slot 4 + `__purecall` / vtable consumer chain |
| `set_plate_comment` | `0x0044d594` | `__init_pointers` thunk → `DAT_004b8e34` |
| `force_decompile` | `0x0044d594` | Param `encoded_null`; comments applied |
| `save_program` | `bulanci.exe` | Saved (second attempt after transient lock) |

**Not applied:** `rename_function_by_address` — no unique MSVCRT `__initp_*` symbol for this VA (same rationale as R8 task 27 / sibling `FUN_0044a916` …).

## Frida

**none** — CRT startup init; static xref + global read closure sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Canonical `__initp_*` thunk name | Not a named export in this PE (unlike `__initp_misc_winsig`) |
| `DAT_004b8e34` data label | Out of scope for single-FUN task |
| `mapping.csv` / `_Globals.cpp` sync | Export-tree update deferred |

## Cross-links

- [round8_fun_task_27_report.md](round8_fun_task_27_report.md) — sibling slot 5 (`FUN_0045011e`) + full `__init_pointers` table
- [r9_globals_task_016_report.md](r9_globals_task_016_report.md) — slot 7 empty thread hook in same caller
- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
