# Round 9 `_Globals` — Task 041 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 41 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec *(manifest cluster label — VA is CRT/SEH band `0x450000`–`0x454fff`, not libjpeg)* |
| **seed_address** | `0x00452869` |
| **ghidra_name (expected)** | `FUN_00452869` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R8 FUN task 28 (`round8_fun_task_28_report.md`); R5 worker 15 skip table; pair `FUN_00452f0b` @ `0x00452f0b` (`__write` unlock epilog, R8 task 29) |

## Status

**PARTIAL** — Live Ghidra MCP re-verification confirms MSVC `__lseeki64` SEH epilog thunk calling `MSVCRT___unlock_fhandle`; CRT-only xref closure; no IJG/bulanci game symbol for rename. **`FUN_00452869` kept.**

## Function

| Address | Ghidra name (before → after) | Role summary | Evidence |
|---------|------------------------------|--------------|----------|
| `0x00452869` | `FUN_00452869` → **`FUN_00452869`** *(unchanged)* | **`__lseeki64` locked-path epilog:** `MSVCRT___unlock_fhandle(*(int*)(ebp+8))` then `RET`. Compiler-split SEH helper — not a standalone MSVCRT export. | **Size 10 B (`0xa`)** per Ghidra + `mapping.csv`. **Disasm:** `PUSH [EBP+8]` / `CALL 0x00454ad7` / `POP ECX` / `RET`. **PE bytes @ `orig/bulanci_insturmented.exe`:** `ff 75 08 e8 66 22 00 00 59 c3`. **Single CODE xref:** `__lseeki64@0x00452858` (`CALL 0x00452869`; plate: *unlock epilog after __lseeki64_nolock / error paths*). **Callee:** `MSVCRT___unlock_fhandle@0x00454ad7`. **Transitive closure:** `__lseeki64` callers = `__flsbuf@0x0044eed6`, `__flswbuf@0x00454efe` only — pure CRT stdio, no game/jpeg/mad named entry ([round5_worker_15_report.md](../struct_recovery/round5_worker_15_report.md)). |

### Disassembly (Ghidra)

```
00452869  PUSH  dword ptr [EBP + 0x8]
0045286c  CALL  MSVCRT___unlock_fhandle      ; 0x00454ad7
00452871  POP   ECX
00452872  RET
```

### Caller context @ `0x00452858` (within `__lseeki64`)

After `__lseeki64_nolock` / error paths on the file-handle-locked branch (`TEST byte [handle+4], 1` @ `0x00452814`), parent loads return values from `[EBP-0x24]`/`[EBP-0x20]` and falls through SEH cleanup @ `0x00452863`. Unlock epilog runs immediately before that cleanup when the locked seek path was taken.

### Decompile (Ghidra, live)

```c
void _Globals::FUN_00452869(void)
{
  int unaff_EBP;
  MSVCRT___unlock_fhandle(*(uint *)(unaff_EBP + 8));
  return;
}
```

*(PRE_COMMENT documents SEH epilog role and sole caller `__lseeki64@0x452858`; pair note for `FUN_00452f0b` / `__write`.)*

### mapping.csv stub (not trusted)

```
;_Globals::FUN_00452869;0x452869;0xa;__stdcall;;void
```

*(Size `0xa` matches PE/Ghidra; `__stdcall` is Ghidra’s label for this compiler-split epilog — body reads parent `[EBP+8]`, not callee stack args.)*

### `_Globals.cpp` stub (not trusted)

```cpp
void _Globals::FUN_00452869() { STUB_BODY(); }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| *(none this session)* | — | R8 task 28 `set_decompiler_comment` already present and accurate; live `force_decompile` confirms. No rename (CRT epilog — protocol forbids generic ordinals; no upstream export name). No `save_program`. |

## Frida

**none** — CRT-only xref closure; static Ghidra disasm/decompile + PE byte match sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Meaningful rename | **Deferred** — compiler-split MSVCRT epilog; no standalone CRT export or bulanci game symbol |
| Calling convention in Ghidra (`__stdcall void`) | **Cosmetic** — epilog reads parent frame, not a normal callable API |
| Manifest band `jpeg_codec` vs VA placement | **Documented** — address sits in CRT/SEH slice adjacent to embedded libjpeg objects ([round5_worker_15_report.md](../struct_recovery/round5_worker_15_report.md)) |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round8_fun_task_28_report.md](round8_fun_task_28_report.md)
- [round8_fun_task_29_report.md](round8_fun_task_29_report.md) — `FUN_00452f0b` / `__write` unlock pair
- [round5_worker_15_report.md](../struct_recovery/round5_worker_15_report.md)
