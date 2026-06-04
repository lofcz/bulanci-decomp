# Round 9 `_Globals` FUN — Task 020 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 20 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim *(manifest cluster label — VA is VS2005 MSVCRT `__init_pointers` split thunk, not game sim)* |
| **seed_address** | `0x0044a916` |
| **ghidra_name (before)** | `FUN_0044A916` |
| **prior_hint** | *(empty)* |
| **prior art** | R8 FUN task 27 (`round8_fun_task_27_report.md`) — sibling-thunk table slot #1 |

## Status

**PARTIAL** — Live Ghidra confirms a **10-byte** MSVCRT **`__init_pointers`** split thunk: stores **`__encoded_null()`** into **`DAT_004b873c`**, decoded by **`Runtime::MSVCRT::__callnewh`**. **No rename:** no unique VS2005 **`__initp_*`** export name for this slot (R8/R9 no-guess rule). Prototype + comments refreshed; name stays **`FUN_0044a916`**.

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x0044a916` | **`FUN_0044a916`** | **`__init_pointers` encoded new-handler store** — `DAT_004b873c = encoded_null; return` | 3 insn / **0xA** B; disasm below; **1×** CODE xref from **`Runtime::MSVCRT::__init_pointers@0x0044c43f`** (first callee after `__encoded_null()`); **`get_xrefs_to(0x004b873c)`** → WRITE @ `0x0044a91a`, READ @ `0x0044a920` in **`__callnewh`** |

### Disassembly proof

```
0044a916  MOV EAX,dword ptr [ESP+0x4]   ; encoded_null (__cdecl arg)
0044a91a  MOV [0x004b873c],EAX          ; DAT_004b873c
0044a91f  RET
```

### `__init_pointers` call order (VS2005 MSVCRT)

| Order | Callee | Target global | Known consumer |
|-------|--------|---------------|----------------|
| **1** | **`FUN_0044a916`** | **`DAT_004b873c`** | **`Runtime::MSVCRT::__callnewh`** |
| 2 | `FUN_0044faab` | `DAT_004b8f80` | `InitializeCriticalSectionAndSpinCount` |
| 3 | `FUN_0044a942` | `DAT_004b8744` | `__invalid_parameter` |
| 4 | `FUN_0044d594` | `DAT_004b8e34` | `CDSException::__purecall` |
| 5 | `FUN_0045011e` | `DAT_004b8f98` | *(no decode/read in PE — R8 task 27)* |
| 6 | `__initp_misc_winsig` | `DAT_004b8f84`–`0x004b8f90` | `___get_sigabrt` / `_raise` |
| 7 | `FUN_00448a97` | — | empty CRT placeholder |
| 8 | `__initp_eh_hooks` | EH hook slots | SEH init |

Live decompile @ `0x0044c457`:

```c
encoded_null = __encoded_null();
_Globals::FUN_0044a916(encoded_null);
_Globals::FUN_0044faab(encoded_null);
// ...
```

### Consumer proof (`__callnewh`)

```c
pcVar1 = (code *)__decode_pointer(DAT_004b873c);
if (pcVar1 != (code *)0x0) {
  iVar2 = (*pcVar1)(_Size);
  ...
}
```

Ghidra labels this body **`Runtime::MSVCRT::__callnewh`** (Library: Visual Studio 2005 Release).

### mapping.csv / stub (not trusted)

```
;_Globals::FUN_0044a916;0x44a916;0xa;__cdecl;;uchar;uint
```

`_Globals.cpp` stub returns `uchar` from `uint` — export signature wrong; Ghidra body is `void` store thunk.

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x0044a916` | `void __cdecl FUN_0044a916(undefined4 encoded_null)` |
| `set_decompiler_comment` | `0x0044a916` | `__init_pointers` thunk #1 + `__callnewh` consumer |
| `set_plate_comment` | `0x0044a916` | Encoded handler store → `DAT_004b873c` |
| `force_decompile` | `0x0044a916` | Param `encoded_null`; plate + block comment |
| `save_program` | `bulanci.exe` | Saved |

**Not applied:** `rename_function_by_address` — no unique MSVCRT symbol in PE (unlike named `__initp_misc_winsig` / `__initp_eh_hooks`).

## Frida

**none** — CRT startup init; static xref + `DAT_004b873c` read/write closure sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Canonical `__initp_*` name for slot #1 | Not present as a named library function in this PE |
| Sibling thunks | `FUN_0044faab` / `FUN_0044a942` / `FUN_0044d594` — separate R9 tasks |
| `mapping.csv` / `_Globals.cpp` | Still `_Globals::FUN_0044a916` stub `uchar` return — export sync out of scope |

## Cross-links

- [round8_fun_task_27_report.md](round8_fun_task_27_report.md) — `__init_pointers` table + slot #5 orphan global
- [round8_fun_task_14_report.md](round8_fun_task_14_report.md) — `__init_pointers` CRT band context
- `Runtime.cpp` comment `44A780-44A916` — SEH/`__callnewh` cluster adjacent to this thunk tail
