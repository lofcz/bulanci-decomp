# Round 9 `_Globals` — Task 040 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 40 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec *(manifest cluster label — VA is CRT/`__init_pointers` band, not libjpeg)* |
| **seed_address** | `0x0045011e` |
| **ghidra_name (expected)** | `FUN_0045011E` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R8 FUN task 27 (`round8_fun_task_27_report.md`); R5 worker 15 skip table — no game/jpeg/mad callers |

## Status

**PARTIAL** — Live Ghidra MCP re-verifies a **10-byte** MSVCRT **`__init_pointers`** split thunk: stores **`__encoded_null()`** (in **`ESI`**, pushed as sole arg) into **`DAT_004b8f98`**. **No rename:** no **`__decode_pointer`** consumer for that global in `bulanci.exe` and no unique VS2005 **`__initp_*`** export name for this slot (ROUND9 no-guess rule). R8 prototype/plate/decompiler comments still present; name stays **`FUN_0045011e`**.

## Function

| Address | Ghidra (this session) | Role summary | Evidence |
|---------|----------------------|--------------|----------|
| `0x0045011e` | `FUN_0045011e` | **`__init_pointers` encoded-null store** — `DAT_004b8f98 = encoded_null; return` | 3 insn / **0xA** B; disasm below; **1×** CODE xref from **`__init_pointers@0x0044c457`**; **`get_xrefs_to(0x004b8f98)`** → **WRITE only** @ `0x00450122` (no reads) |

### Disassembly (Ghidra live)

```
0045011e  MOV EAX,dword ptr [ESP+0x4]   ; encoded_null (__cdecl arg)
00450122  MOV [0x004b8f98],EAX          ; DAT_004b8f98
00450127  RET
```

### Decompile (Ghidra live)

```c
void __cdecl _Globals::FUN_0045011e(undefined4 encoded_null)
{
  _DAT_004b8f98 = encoded_null;
  return;
}
```

### `__init_pointers` call order @ `0x0044c436` (Ghidra live)

| Order | Call @ | Callee | Arg |
|------:|--------|--------|-----|
| 0 | `0x0044c437` | `__encoded_null@0x0044aacf` | → **`ESI`** |
| 1 | `0x0044c43f` | `FUN_0044a916` | `PUSH ESI` |
| 2 | `0x0044c445` | `FUN_0044faab` | `PUSH ESI` |
| 3 | `0x0044c44b` | `FUN_0044a942` | `PUSH ESI` |
| 4 | `0x0044c451` | `FUN_0044d594` | `PUSH ESI` |
| **5** | **`0x0044c457`** | **`FUN_0045011e`** | **`PUSH ESI`** |
| 6 | `0x0044c45d` | `__initp_misc_winsig@0x0044ff14` | `PUSH ESI` |
| 7 | `0x0044c463` | `FUN_00448a97` | `PUSH ESI` |
| 8 | `0x0044c469` | `__initp_eh_hooks@0x0044bd49` | `PUSH ESI` |

`DAT_004b8f98` sits **+0x18** bytes above the winsig handler cluster (`0x004b8f84`–`0x004b8f90`) initialized on the **next** line by **`__initp_misc_winsig`**, but is written by this separate thunk and never decoded in the linked binary.

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole CODE)** | `0x0044c457` | `Runtime::MSVCRT::__init_pointers` | `CALL FUN_0045011e` after `FUN_0044d594`; arg is `ESI = __encoded_null()` |
| **Global write** | `0x00450122` | `DAT_004b8f98` | Sole xref to global — **WRITE only** |

### mapping.csv stub (not trusted)

```
;_Globals::FUN_0045011e;0x45011e;0xa;__cdecl;;void;uint
```

*(Size `0xa` matches Ghidra body; signature matches R8 prototype refresh.)*

### `_Globals.cpp` stub (not trusted)

```cpp
void _Globals::FUN_0045011e(uint param_1) { STUB_BODY(); }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| *(verify only)* | `0x0045011e` | R8 deltas still present — no new mutations this session |
| `set_function_prototype` | `0x0045011e` | *(R8)* `void __cdecl FUN_0045011e(undefined4 encoded_null)` — confirmed live |
| `set_decompiler_comment` | `0x0045011e` | *(R8)* `__init_pointers` thunk + orphan-slot rationale — confirmed live |
| `set_plate_comment` | `0x0045011e` | *(R8)* `CRT __init_pointers encoded-null store → DAT_004b8f98; no decode consumer` — confirmed live |
| `save_program` | — | **Skipped** — no changes applied |

**Not applied:** `rename_function_by_address` — no unique MSVCRT symbol; sibling thunks (`FUN_0044a916`, `FUN_0044faab`, …) also remain `FUN_*` with slot comments only.

## Frida

**none** — CRT startup init; static xref + global xref closure sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| `DAT_004b8f98` semantic | No decode/read site in `bulanci.exe`; likely dead CRT pointer slot or VS2005 linker artifact |
| Canonical `__initp_*` name | Not present as a named library function in this PE (unlike `__initp_misc_winsig`) |
| Sibling thunks | `FUN_0044a916` / `FUN_0044faab` / `FUN_0044a942` / `FUN_0044d594` — same band, out of R9 task scope |
| `mapping.csv` / `_Globals.cpp` export sync | Out of scope for FUN-only task |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round8_fun_task_27_report.md](round8_fun_task_27_report.md)
- [round5_worker_15_report.md](../struct_recovery/round5_worker_15_report.md)
