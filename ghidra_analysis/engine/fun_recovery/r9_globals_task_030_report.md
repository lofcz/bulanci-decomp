# Round 9 `_Globals` FUN — Task 030 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 30 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim *(manifest cluster label — VA is CRT/`doexit` SEH band, not gameplay sim)* |
| **seed_address** | `0x0044c3e1` |
| **ghidra_name** | `FUN_0044C3E1` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R5 worker 48 `__unlock` rename @ `0x00449b1b`; R8 FUN manifest (`_gen_r8_fun_manifest.py`, 1 xref); R9 task 039 sibling `_raise` epilog @ `0x004500e2` (`__unlock(0)`) |

## Status

**PARTIAL** — Role proven (MSVC `_doexit` SEH epilog calling `__unlock(8)`); `FUN_*` kept per ROUND9 (no IJG/libmad/bulanci game symbol; CRT fragment rename deferred). Ghidra MCP live: comment + prototype refreshed; `save_program bulanci.exe`.

## Function

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x0044c3e1` | `FUN_0044C3E1` | **`_doexit` SEH epilog thunk:** if `[EBP+0x10] != 0`, `__unlock(8)` @ `0x00449b1b`; else `RET`. Pure CRT glue — not game/sim reachable. | **Ghidra disasm (15 B, `mapping.csv` size `0xf`):** `CMP [EBP+0x10],0` / `JZ +8` / `PUSH 8` / `CALL __unlock` / `POP ECX` / `RET`. **PE bytes (`orig/bulanci_insturmented.exe`):** `83 7d 10 00 74 08 6a 08 e8 2d d7 ff ff 59 c3` (byte-identical). **Single caller:** `doexit` @ `0x0044c328`, `CALL` @ `0x0044c3bd` ([UNCONDITIONAL_CALL]). **Callee:** `__unlock` @ `0x00449b1b` (R5 worker 48). **Parent:** Ghidra `doexit` — *Library Function - Single Match `_doexit`*, Visual Studio 2005 Release; body uses `__lock(8)` @ `0x00449bf3` and inline `__unlock(8)` @ `0x0044c3d0` after this epilog returns. |

### Disassembly (Ghidra + PE)

```
0044c3e1  CMP  dword ptr [EBP+0x10], 0
0044c3e5  JE   0044c3ef
0044c3e7  PUSH 8
0044c3e9  CALL 0x00449b1b          ; __unlock(lockIndex=8)
0044c3ee  POP  ECX
0044c3ef  RET
```

### Caller context @ `0x0044c3bd` (within `doexit`)

```
0044c3b6  MOV  dword ptr [EBP-0x4], 0xfffffffe   ; SEH state
0044c3bd  CALL 0x0044c3e1                      ; this task
0044c3c2  CMP  dword ptr [EBP+0x10], 0
0044c3c6  JNZ  0x0044c3f0
0044c3c8  ...
0044c3ce  PUSH 8
0044c3d0  CALL 0x00449b1b                      ; inline __unlock(8) on normal exit path
```

### Decompile (Ghidra, post-delta)

```c
void _Globals::FUN_0044c3e1(void)
{
  if (*(int *)(unaff_EBP + 0x10) != 0) {
    __unlock(8);
  }
  return;
}
```

### mapping.csv stub (not trusted)

```
;_Globals::FUN_0044c3e1;0x44c3e1;0xf;__stdcall;;uchar
```

*(Size `0xf` matches PE/Ghidra; `__stdcall` / `uchar` incorrect — body is void thunk, `RET` only.)*

### `_Globals.cpp` stub (not trusted)

```cpp
uchar _Globals::FUN_0044c3e1() { STUB_BODY(); return 0; }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0044c3e1` | Replaced `UNCERTAIN: …` with confirmed `doexit` SEH epilog / `__unlock(8)` note |
| `set_function_prototype` | `0x0044c3e1` | `void FUN_0044c3e1(void)` (was `undefined __stdcall`) |
| `save_program` | `bulanci.exe` | Saved |

No `rename_function_by_address` — CRT SEH fragment; no unique bulanci/upstream export name.

## Frida

**none** — CRT-only xref closure; Ghidra disasm/decompile + PE bytes sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Meaningful rename | **Deferred** — same class as R9 task 039 / R8 `FUN_004500e2` (`_raise` epilog); protocol forbids generic ordinals |
| `mapping.csv` / `_Globals.cpp` signature sync | **Out of scope** — separate mapping pass |
| Exact SEH frame field at `[EBP+0x10]` symbolic name | **Cosmetic** — MSVC injected epilog frame; not required for role proof |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round5_worker_48_report.md](../struct_recovery/round5_worker_48_report.md) — `__unlock` @ `0x00449b1b`
- [r9_globals_task_039_report.md](r9_globals_task_039_report.md) — `_raise` epilog `__unlock(0)` @ `0x004500e2`
- [round8_fun_task_30_report.md](round8_fun_task_30_report.md) — unrelated R8 task 30 (`0x00454ace`)
