# Round 9 `_Globals` — Task 021 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 21 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x0044a942` |
| **ghidra_name** | `FUN_0044A942` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R8 FUN task 27 (`round8_fun_task_27_report.md`) — slot 3 in `__init_pointers` table |

## Status

**PARTIAL** — Live Ghidra confirms a **10-byte** VS2005 MSVCRT **`__init_pointers`** split thunk: stores **`__encoded_null()`** into **`DAT_004b8744`**, decoded and invoked by **`Runtime::MSVCRT::__invalid_parameter`**. **No rename:** this PE has no standalone **`__initp_*`** symbol for this slot (unlike in-image **`__initp_misc_winsig`** / **`__initp_eh_hooks`**); R9 no-guess rule matches R8 sibling thunks. Prototype + comments refreshed; name stays **`FUN_0044a942`**.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x0044a942` | `FUN_0044a942` | **`FUN_0044a942`** | **`__init_pointers` encoded-null store** — `DAT_004b8744 = encoded_null; return` | 3 insn / **0xA** B; disasm below; **1×** CODE xref from **`__init_pointers@0x0044c44b`**; **`get_xrefs_to(0x004b8744)`** → **WRITE** here, **READ** only in **`__invalid_parameter`** |

### Disassembly proof

```
0044a942  MOV EAX,dword ptr [ESP+0x4]   ; encoded_null (__cdecl arg)
0044a946  MOV [0x004b8744],EAX          ; DAT_004b8744
0044a94b  RET
```

### `__init_pointers` call order (VS2005 MSVCRT, live decompile)

| Order | Callee | Target global | Known consumer |
|-------|--------|---------------|----------------|
| 1 | `FUN_0044a916` | `DAT_004b873c` | `__callnewh` (`__decode_pointer` @ `0x0044a920`) |
| 2 | `FUN_0044faab` | `DAT_004b8f80` | `InitializeCriticalSectionAndSpinCount` |
| **3** | **`FUN_0044a942`** | **`DAT_004b8744`** | **`__invalid_parameter`** (`__decode_pointer` @ `0x0044aa4b`) |
| 4 | `FUN_0044d594` | `DAT_004b8e34` | `CDSException::__purecall` |
| 5 | `FUN_0045011e` | `DAT_004b8f98` | *(none — write-only in PE)* |
| 6 | `__initp_misc_winsig` | `DAT_004b8f84`–`0x004b8f90` | `___get_sigabrt` / `_raise` |
| 7 | `FUN_00448a97` | — | empty CRT placeholder |
| 8 | `__initp_eh_hooks` | EH hook slots | SEH init |

Caller disasm @ `0x0044c44b`: `PUSH ESI` / `CALL 0x0044a942` with `ESI = __encoded_null()` immediately after `FUN_0044faab`.

### Consumer proof (`__invalid_parameter`)

Ghidra labels **`Runtime::MSVCRT::__invalid_parameter`** @ `0x0044aa48` (Library: Visual Studio 2005 Release). Decompile:

```c
UNRECOVERED_JUMPTABLE = (code *)__decode_pointer(DAT_004b8744);
if (UNRECOVERED_JUMPTABLE != (code *)0x0) {
    (*UNRECOVERED_JUMPTABLE)();
    return;
}
```

Disasm @ `0x0044aa4b`: `PUSH dword ptr [0x004b8744]` before handler dispatch (`CALL 0x0044aad8` / indirect jump path).

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole CODE)** | `0x0044c44b` | `Runtime::MSVCRT::__init_pointers` | 3rd `CALL` after `FUN_0044faab`; arg = `encoded_null` |
| **Global write** | `0x0044a946` | `DAT_004b8744` | Sole **WRITE** |
| **Global read** | `0x0044aa4b` | `__invalid_parameter` | Sole **READ** |

### mapping.csv / stub (not trusted)

```
;_Globals::FUN_0044a942;0x44a942;0xa;__cdecl;;uchar;uint
```

Size **0xa** matches live body; **`uchar` return** and bare **`uint`** param are incorrect — live role is **`void __cdecl(encoded_null)`** store thunk.

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x0044a942` | `void __cdecl FUN_0044a942(undefined4 encoded_null)` |
| `set_decompiler_comment` | `0x0044a942` | `__init_pointers` slot + `__invalid_parameter` reader proof |
| `set_plate_comment` | `0x0044a942` | CRT encoded-null store → `DAT_004b8744` |
| `force_decompile` | `0x0044a942` | Param `encoded_null`; comments in decompile header |
| `save_program` | `bulanci.exe` | Saved |

**Not applied:** `rename_function_by_address` — no unique MSVCRT **`__initp_*`** function name present in this PE for this 10-byte split thunk.

## Frida

**none** — CRT startup init; static xref + global xref closure sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Canonical `__initp_*` name | Not exported/named in `bulanci.exe` (contrast `__initp_misc_winsig@0x0044ff14`) |
| `DAT_004b8744` label | Out of scope — FUN-only task; global still `DAT_004b8744` |
| `mapping.csv` / `_Globals.cpp` | Still `_Globals::FUN_0044a942` stub returning `uchar` — export sync out of scope |
| Sibling thunks | `FUN_0044a916` / `FUN_0044faab` / `FUN_0044d594` — same band, separate R9 tasks |

## Cross-links

- [round8_fun_task_27_report.md](./round8_fun_task_27_report.md) — `__init_pointers` table + R8 PARTIAL on slot 5 (`FUN_0045011e`)
- MSVC **`__init_pointers@0x0044c437`** — sole caller chain
