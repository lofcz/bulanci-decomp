# Round 9 `_Globals` — Task 035 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 35 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x0044faab` |
| **ghidra_name (expected)** | `FUN_0044FAAB` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R8 FUN task 27 (`round8_fun_task_27_report.md`) — `__init_pointers` slot table; R9 task 040 (`r9_globals_task_040_report.md`) — sibling `FUN_0045011e` |

## Status

**PARTIAL** — Live Ghidra MCP confirms a **10-byte** MSVCRT **`__init_pointers`** split thunk: stores **`__encoded_null()`** (in **`ESI`**, pushed as sole arg) into **`DAT_004b8f80`**. **Consumer proven:** `___crtInitCritSecAndSpinCount@0x0044fac5` **`__decode_pointer(DAT_004b8f80)`** and may resolve **`InitializeCriticalSectionAndSpinCount`** via **`GetProcAddress("kernel32.dll", …)`** or rewrite the slot at `0x0044fb36`. **No rename:** no unique VS2005 **`__initp_*`** export name for this split thunk (ROUND9 no-guess rule; same rationale as R8 task 27 / R9 task 040). Prototype + plate/decompiler comments refreshed; name stays **`FUN_0044faab`**.

## Function

| Address | Ghidra (this session) | Role summary | Evidence |
|---------|----------------------|--------------|----------|
| `0x0044faab` | `FUN_0044faab` | **`__init_pointers` encoded-pointer store (slot 2)** — `DAT_004b8f80 = encoded_null; return` | 3 insn / **0xA** B; disasm below; **1×** CODE xref from **`__init_pointers@0x0044c445`**; **`get_xrefs_to(0x004b8f80)`** → **WRITE** @ `0x0044faaf`, **READ** @ `0x0044fad6`, **WRITE** @ `0x0044fb36` (consumer self-host path) |

### Disassembly (Ghidra live)

```
0044faab  MOV EAX,dword ptr [ESP+0x4]   ; encoded_null (__cdecl arg)
0044faaf  MOV [0x004b8f80],EAX          ; DAT_004b8f80
0044fab4  RET
```

### Decompile (Ghidra live, post-refresh)

```c
void __cdecl _Globals::FUN_0044faab(undefined4 encoded_null)
{
  DAT_004b8f80 = encoded_null;
  return;
}
```

### Consumer closure (`DAT_004b8f80`)

`Runtime::MSVCRT::___crtInitCritSecAndSpinCount@0x0044fac5` (Ghidra library match, VS2005 Release):

```c
pcVar1 = (code *)__decode_pointer(DAT_004b8f80);
if (pcVar1 != (FARPROC)0x0) goto LAB_0044fb3b;
/* ... GetModuleHandleA("kernel32.dll"); GetProcAddress(..., "InitializeCriticalSectionAndSpinCount"); ... */
DAT_004b8f80 = __encode_pointer((int)pcVar1);
LAB_0044fb3b:
iVar3 = (*pcVar1)(param_1, param_2);
```

String xrefs in consumer: **`kernel32.dll`**, **`InitializeCriticalSectionAndSpinCount`** — ties slot `DAT_004b8f80` to that Win32 API (not guessed from thunk name alone).

### `__init_pointers` call order @ `0x0044c436` (Ghidra live)

| Order | Call @ | Callee | Target global / notes |
|------:|--------|--------|------------------------|
| 0 | `0x0044c437` | `__encoded_null@0x0044aacf` | → **`ESI`** |
| 1 | `0x0044c43f` | `FUN_0044a916` | `DAT_004b873c` (`__callnewh` slot) |
| **2** | **`0x0044c445`** | **`FUN_0044faab`** | **`DAT_004b8f80`** (**this task**) |
| 3 | `0x0044c44b` | `FUN_0044a942` | `DAT_004b8744` |
| 4 | `0x0044c451` | `FUN_0044d594` | `DAT_004b8e34` |
| 5 | `0x0044c457` | `FUN_0045011e` | `DAT_004b8f98` (orphan — R9 task 040) |
| 6 | `0x0044c45d` | `__initp_misc_winsig@0x0044ff14` | winsig cluster |
| 7 | `0x0044c463` | `FUN_00448a97` | thread hook no-op |
| 8 | `0x0044c469` | `__initp_eh_hooks@0x0044bd49` | EH hooks |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole CODE)** | `0x0044c445` | `Runtime::MSVCRT::__init_pointers` | `CALL FUN_0044faab` after `FUN_0044a916`; arg is `ESI = __encoded_null()` |
| **Global write (init)** | `0x0044faaf` | `DAT_004b8f80` | This thunk — seeds encoded null |
| **Global read (consumer)** | `0x0044fad6` | `DAT_004b8f80` | `___crtInitCritSecAndSpinCount` — `__decode_pointer` |
| **Global write (consumer)** | `0x0044fb36` | `DAT_004b8f80` | Consumer may replace with `__encode_pointer(InitializeCriticalSectionAndSpinCount)` |

### mapping.csv stub (not trusted)

```
;_Globals::FUN_0044faab;0x44faab;0xa;__cdecl;;uchar;uint
```

*(Size `0xa` matches Ghidra body; **`uchar` return incorrect** — body is `void` ending in `RET` with no meaningful AL; param is encoded pointer, not game `uint`.)*

### `_Globals.cpp` stub (not trusted)

```cpp
uchar _Globals::FUN_0044faab(uint param_1) { STUB_BODY(); return 0; }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x0044faab` | `void __cdecl FUN_0044faab(undefined4 encoded_null)` |
| `set_decompiler_comment` | `0x0044faab` | Slot 2 + `___crtInitCritSecAndSpinCount` / `InitializeCriticalSectionAndSpinCount` consumer proof |
| `set_plate_comment` | `0x0044faab` | `CRT __init_pointers encoded pointer store -> DAT_004b8f80 (InitializeCriticalSectionAndSpinCount slot)` |
| `force_decompile` | `0x0044faab` | Param `encoded_null`; prior `UNCERTAIN` comment replaced |
| `save_program` | `bulanci.exe` | Saved |

**Not applied:** `rename_function_by_address` — no unique MSVCRT `__initp_*` symbol for this split thunk; sibling thunks remain `FUN_*` with slot comments.

## Frida

**none** — CRT startup init; static xref + `___crtInitCritSecAndSpinCount` decompile + string xrefs sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Canonical `__initp_*` export name for this 10 B thunk | **UNK** — not a named library function in this PE (unlike `__initp_misc_winsig`) |
| Sibling thunks | `FUN_0044a916` / `FUN_0044a942` / `FUN_0044d594` — separate R9 tasks |
| `mapping.csv` / `_Globals.cpp` export sync | Out of scope for FUN-only task (`uchar` stub vs `void` live) |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round8_fun_task_27_report.md](round8_fun_task_27_report.md)
- [r9_globals_task_040_report.md](r9_globals_task_040_report.md)
- `Runtime::MSVCRT::__init_pointers@0x0044c436` — sole caller chain
