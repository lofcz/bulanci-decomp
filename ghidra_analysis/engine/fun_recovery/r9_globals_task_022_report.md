# Round 9 `_Globals` FUN — Task 022 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 22 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x0044ab44` |
| **ghidra_name (before)** | `FUN_0044AB44` |
| **prior_hint** | *(empty)* |
| **prior art** | [round8_fun_task_23_report.md](round8_fun_task_23_report.md) — caller sequence + `__get_flsindex` rename; [round5_worker_16_report.md](../struct_recovery/round5_worker_16_report.md) — TLS/FLS note |

## Status

**DONE** — Disasm + sole `_threadstartex` xref + MSVC CRT `threadex.c` / `tidtable.c` `FLS_GETVALUE` pattern prove internal **`__fls_getvalue`**. Renamed in Ghidra; `void * __stdcall __fls_getvalue(uint)`; program saved.

## Function

| Address | Ghidra name (before → after) | Role summary | Evidence |
|---------|------------------------------|--------------|----------|
| `0x0044ab44` | `FUN_0044ab44` → **`__fls_getvalue`** | MSVC CRT wrapper: load `FlsGetValue` proc from TLS slot **`DAT_004b115c`**, call with FLS index (`dwFlsIndex`); returns per-thread slot value (`_ptiddata *` in `_threadstartex`) | **Disasm:** `PUSH [ESP+4]`; `PUSH [0x004b115c]`; `CALL TlsGetValue`; `CALL EAX`; `RET 4`. **Xref (1):** `_threadstartex@0x00449672` @ `0x0044967e` immediately after `___set_flsgetvalue` + `__get_flsindex` — matches `__fls_getvalue(__get_flsindex())` in VS CRT `threadex.c`. **Upstream:** [tidtable.c `__fls_getvalue`](https://github.com/shihyu/learn_c/blob/master/vc_lib_src/src/tidtable.c) / `FLS_GETVALUE` macro (TlsGetValue on get-proc slot, indirect call). **Neighbors:** `___set_flsgetvalue@0x0044ab5f`, `__get_flsindex@0x0044ab59`, `___fls_setvalue@8@0x0044ab89` |

### Disassembly (`0x0044ab44`–`0x0044ab58`)

```
0044ab44  PUSH dword ptr [ESP + 0x4]    ; dwFlsIndex (__get_flsindex result)
0044ab48  PUSH dword ptr [0x004b115c]   ; TLS index for FlsGetValue proc ptr
0044ab4e  CALL dword ptr [0x0047f0f0]   ; TlsGetValue
0044ab54  CALL EAX                      ; FlsGetValue(dwFlsIndex)
0044ab56  RET 0x4
```

### Caller context (`_threadstartex`)

```
00449673  CALL 0x0044ab5f          ; ___set_flsgetvalue
00449678  CALL 0x0044ab59          ; __get_flsindex → EAX = FLS index
0044967d  PUSH EAX
0044967e  CALL 0x0044ab44          ; __fls_getvalue(index)
00449683  TEST EAX,EAX             ; ptd already in FLS?
00449685  JNZ  0x004496b2          ; merge path
...
0044968c  CALL 0x0044ab59          ; __get_flsindex
00449692  CALL 0x0044ab89          ; ___fls_setvalue@8(index, ptd)
```

### mapping.csv stub (not trusted for return type)

```
;_Globals::FUN_0044ab44;0x44ab44;0x15;__stdcall;;uchar;uint
```

Size **`0x15`** matches Ghidra body; **`uchar` return** disagrees with caller `TEST EAX` / CRT `PVOID` — prototype set to **`void *`**.

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0044ab44` | `__fls_getvalue` |
| `set_function_prototype` | `0x0044ab44` | `void * __fls_getvalue(uint dwFlsIndex)` + `__stdcall` |
| `set_decompiler_comment` | `0x0044ab44` | CRT role + caller + pair callees |
| `force_decompile` | `0x0044ab44` | `return (void *)(*TlsGetValue(...))(dwFlsIndex)` |
| `save_program` | `bulanci.exe` | saved |

## Frida

**none** — 5-instruction thunk; xref closure + CRT source sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| `DAT_004b115c` label | TLS slot index global still `DAT_*`; not in FUN-only scope |
| Decompiler namespace | Shows `_Globals::__fls_getvalue` until export/namespace cleanup |
| `mapping.csv` / `_Globals.cpp` | Still `FUN_0044ab44` / `uchar` stub until mapping sync pass |

## Cross-links

- MSVC `threadex.c` — `_threadstartex` FLS bootstrap ([cansou/msvcrt](https://github.com/cansou/msvcrt/blob/master/src/threadex.c))
- R8 task 23 — `__get_flsindex` @ `0x0044ab59` (paired caller proof)
- MS doc lists `__get_flsindex` among [internal CRT functions](https://learn.microsoft.com/en-us/cpp/c-runtime-library/internal-crt-globals-and-functions?view=msvc-170); `__fls_getvalue` named in CRT `tidtable.c` sources cited above
