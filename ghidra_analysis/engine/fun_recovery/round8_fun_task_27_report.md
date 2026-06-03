# Round 8 FUN — Task 27 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 27 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x0045011e` |
| **title** | FUN recovery: FUN_0045011E @ 0x0045011e (xrefs=1) |
| **prior_hint** | round5_worker_15 — no game/jpeg/mad callers |

## Status

**PARTIAL** — Live Ghidra MCP confirms a **10-byte** MSVCRT **`__init_pointers`** split thunk: stores **`__encoded_null()`** into **`DAT_004b8f98`**. **No rename:** no **`__decode_pointer`** consumer for that global in `bulanci.exe` and no unique VS2005 **`__initp_*`** export name for this slot (R8 no-guess rule). Prototype/comment refresh applied; name stays **`FUN_0045011e`**.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x0045011e` | `FUN_0045011e` | **`FUN_0045011e`** | **`__init_pointers` encoded-null store** — `DAT_004b8f98 = encoded_null; return` | 3 insn / **0xA** B; disasm below; **1×** CODE xref from **`__init_pointers@0x0044c457`**; **`get_xrefs_to(0x004b8f98)`** → write-only (no reads) |

### Disassembly proof

```
0045011e  MOV EAX,dword ptr [ESP+0x4]   ; encoded_null (__cdecl arg)
00450122  MOV [0x004b8f98],EAX          ; DAT_004b8f98
00450127  RET
```

### `__init_pointers` call order (VS2005 MSVCRT)

| Order | Callee | Target global | Known consumer |
|-------|--------|---------------|----------------|
| 1 | `FUN_0044a916` | `DAT_004b873c` | `__callnewh` |
| 2 | `FUN_0044faab` | `DAT_004b8f80` | `InitializeCriticalSectionAndSpinCount` |
| 3 | `FUN_0044a942` | `DAT_004b8744` | `__invalid_parameter` |
| 4 | `FUN_0044d594` | `DAT_004b8e34` | `CDSException::__purecall` |
| **5** | **`FUN_0045011e`** | **`DAT_004b8f98`** | **none in export** |
| 6 | `__initp_misc_winsig` | `DAT_004b8f84`–`0x004b8f90` | `___get_sigabrt` / `_raise` |
| 7 | `FUN_00448a97` | — | empty CRT placeholder |
| 8 | `__initp_eh_hooks` | EH hook slots | SEH init |

`DAT_004b8f98` sits **+0x18** bytes above the winsig handler cluster initialized by **`__initp_misc_winsig`** on the next line of **`__init_pointers`**, but is written by this separate thunk and never decoded in the linked binary.

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole CODE)** | `0x0044c457` | `Runtime::MSVCRT::__init_pointers` | `CALL FUN_0045011e` after `FUN_0044d594(uVar1)`; arg is `uVar1 = __encoded_null()` |
| **Global write** | `0x00450122` | `DAT_004b8f98` | Sole xref to global — **WRITE only** |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x0045011e` | `void __cdecl FUN_0045011e(undefined4 encoded_null)` |
| `set_decompiler_comment` | `0x0045011e` | `__init_pointers` thunk + orphan-slot rationale |
| `set_plate_comment` | `0x0045011e` | CRT encoded-null store → `DAT_004b8f98` |
| `force_decompile` | `0x0045011e` | Param renamed `encoded_null`; comments refreshed |
| `save_program` | `bulanci.exe` | Saved |

**Not applied:** `rename_function_by_address` — no unique MSVCRT symbol; sibling thunks (`FUN_0044a916`, `FUN_0044faab`, …) also remain `FUN_*` with slot comments only.

## Frida

**none** — CRT startup init; static xref + global xref closure sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| `DAT_004b8f98` semantic | No decode/read site in `bulanci.exe`; likely dead CRT pointer slot or VS2005 linker artifact |
| Canonical `__initp_*` name | Not present as a named library function in this PE (unlike `__initp_misc_winsig`) |
| Sibling thunks | `FUN_0044a916` / `FUN_0044faab` / `FUN_0044a942` / `FUN_0044d594` — same band, out of R8 scope |
| `mapping.csv` / `_Globals.cpp` | Still `_Globals::FUN_0045011e` stub returning `uchar` — export sync out of scope |

## Cross-links

- [round5_worker_15_report.md](../struct_recovery/round5_worker_15_report.md) — prior note: no game/jpeg/mad callers
- MSVC **`__init_pointers@0x0044c457`** — sole caller chain
