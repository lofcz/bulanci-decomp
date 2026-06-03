# Round 8 FUN — Task 23 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 23 |
| **round** | 8 |
| **band** | sim |
| **seed_address** | `0x0044ab59` |
| **title** | FUN recovery: FUN_0044AB59 @ 0x0044ab59 (xrefs=2) |

## Status

**DONE** — Disasm + dual xref in `_threadstartex` + MSVC CRT `threadex.c` / MS docs prove internal **`__get_flsindex`** (returns global FLS slot index `DAT_004b1158`). Renamed in Ghidra; `int __stdcall`; program saved.

## Function

| Address | Ghidra name (before → after) | Role summary | Evidence |
|---------|------------------------------|--------------|----------|
| `0x0044ab59` | `FUN_0044ab59` → **`__get_flsindex`** | Return CRT global **`DAT_004b1158`** (allocated FLS index for per-thread `_tiddata`); used before `__fls_getvalue` / `__fls_setvalue` in thread bootstrap | **Disasm:** `MOV EAX,[0x004b1158]`; `RET`. **Xrefs:** `_threadstartex@0x00449672` @ `0x00449678`, `0x0044968c` (matches `__fls_getvalue(__get_flsindex())` / `__fls_setvalue(__get_flsindex(), ptd)` in VS2005 `threadex.c`). **Doc:** [Internal CRT globals](https://learn.microsoft.com/en-us/cpp/c-runtime-library/internal-crt-globals-and-functions) lists `__get_flsindex` |

### Disassembly (`0x0044ab59`–`0x0044ab5e`)

```
0044ab59  MOV  EAX, dword ptr [0x004b1158]
0044ab5e  RET
```

### Caller context (`_threadstartex`)

```
00449673  CALL 0x0044ab5f          ; ___set_flsgetvalue
00449678  CALL 0x0044ab59          ; __get_flsindex → EAX = FLS index
0044967e  CALL 0x0044ab44          ; __fls_getvalue(index)  [still FUN_*]
...
0044968c  CALL 0x0044ab59          ; __get_flsindex (second path)
00449692  CALL 0x0044ab89          ; ___fls_setvalue@8
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0044ab59` | `__get_flsindex` |
| `set_function_prototype` | `0x0044ab59` | `int __get_flsindex(void)` + `__stdcall` |
| `set_decompiler_comment` | `0x0044ab59` | CRT role + caller + pair callees |
| `force_decompile` | `0x0044ab59` | `return DAT_004b1158` |
| `save_program` | `bulanci.exe` | saved |

## Frida

**none** — 2-instruction getter; xref closure + upstream CRT source sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| `DAT_004b1158` label | Global still `DAT_*`; not renamed in this task (FUN-only scope) |
| `FUN_0044ab44` @ `0x0044ab44` | Thread-start `__fls_getvalue` wrapper — R8 task 44 (1 xref) |
| Decompiler namespace | Shows `_Globals::__get_flsindex` until export/namespace cleanup |
| `mapping.csv` | Still `;_Globals::FUN_0044ab59` stub until sync |

## Cross-links

- MSVC `threadex.c` — `_threadstartex` FLS bootstrap
- Ghidra-matched neighbors: `___set_flsgetvalue@0x0044ab5f`, `___fls_setvalue_8@0x0044ab89`, `__mtterm` clears `DAT_004b1158`
- [round5_worker_16_report.md](../struct_recovery/round5_worker_16_report.md) — noted `FUN_0044ab44` TLS/FLS path
