# Round 9 `_Globals` FUN — Task 026 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 26 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim *(manifest cluster — VA is MSVC CRT C++ EH, not game logic)* |
| **seed_address** | `0x0044b1cf` |
| **ghidra_name** | `FUN_0044B1CF` |
| **prior_hint** | *(empty)* |
| **prior art** | R8 FUN pool entry (`_gen_r8_fun_manifest.py`, 1 xref); [EHRegistrationNode.md](../struct_recovery/EHRegistrationNode.md); [batch_48_summary.md](../struct_recovery/batch_48_summary.md) |

## Status

**PARTIAL** — Live Ghidra MCP (`connect_instance bulanci`, 2026-06-04). Role and xref closure are proven: MSVC `___FrameUnwindToState` epilog that decrements `__getptd()->_ProcessingThrow`. **No rename** — no separate Visual Studio 2005 CRT export symbol for this 27-byte tail; protocol forbids invented helper names.

## Function

| Address | Ghidra name | Proposed name | Role | Evidence |
|---------|-------------|---------------|------|----------|
| `0x0044b1cf` | `FUN_0044b1cf` | *(keep `FUN_*`)* | **`___FrameUnwindToState` epilog:** if `__getptd()->_ProcessingThrow > 0`, decrement it; `void`, 0 args | See below |

### Caller / callee closure (Ghidra live)

| Kind | Address | Symbol | Notes |
|------|---------|--------|-------|
| **To (sole caller)** | `0x0044b1b1` | `___FrameUnwindToState` | `CALL 0x0044b1cf` after unwind `while` completes (`0044b1aa` sets SEH state `0xfffffffe`) |
| **From (callees)** | `0x0044ad16` | `__getptd` | Called twice on decrement path (matches parent `INC` path style) |

`get_xrefs_to@0x0044b1cf` → **1** xref (`UNCONDITIONAL_CALL` from `___FrameUnwindToState`). No game / `_Globals` gameplay reachability.

### Paired `_ProcessingThrow` bump in parent

`___FrameUnwindToState@0x0044b109` (Ghidra: **Library Function - Single Match**, Visual Studio 2005 Release):

```
0044b130  CALL  __getptd
0044b135  ADD  EAX, 0x90
0044b13a  INC  dword ptr [EAX]          ; ++_ProcessingThrow
...
0044b1b1  CALL  FUN_0044b1cf            ; epilog: --_ProcessingThrow if >0
```

`FUN_0044b1cf` disasm (27 B, size `0x1b` per `mapping.csv`):

```
0044b1cf  CALL  0x0044ad16              ; __getptd
0044b1d4  CMP   dword ptr [EAX+0x90], 0
0044b1db  JLE   0x0044b1e9
0044b1dd  CALL  0x0044ad16
0044b1e2  ADD   EAX, 0x90
0044b1e7  DEC   dword ptr [EAX]
0044b1e9  RET
```

Decompile (live) matches export in `bulanci.ghidra.exe.c`: conditional `--p_Var1->_ProcessingThrow` after `__getptd()`.

### Signature / mapping stub

| Source | Value |
|--------|-------|
| `config/bulanci/mapping.csv` | `;_Globals::FUN_0044b1cf;0x44b1cf;0x1b;__stdcall;;uchar` |
| Live Ghidra | `undefined __stdcall FUN_0044b1cf(void)` — body is `void` (`RET` only); **`uchar` return not used** |

`_Globals.cpp` stub is `STUB_BODY()` only — not trusted.

### CRT / EH context (not guessed)

- Parent `___FrameUnwindToState` documented in [EHRegistrationNode.md](../struct_recovery/EHRegistrationNode.md) (`state` @ `EHRegistrationNode+0x08`, byte vs dword by `maxState`).
- `__getptd@0x0044ad16` — MSVCRT TLS accessor ([rng.md](../../rng.md), `Runtime.h`).
- Offset **`+0x90`** on `_ptiddata` is `_ProcessingThrow` in decompiler types (live decompile at seed and parent).

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0044b1cf` | Epilog role + sole caller `0x0044b1b1` + `__getptd/+0x90` pairing with `INC @ 0x0044b130` |
| `save_program` | `bulanci.exe` | Saved after comment (first attempt: lock busy; retry OK) |

**Not applied:** `rename_function_by_address` (no upstream CRT export name for this tail), `set_function_prototype` (signature cleanup deferred).

## Frida

**none** — CRT-only; static xref + disasm sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Official MSVC symbol name for this extracted epilog tail | **UNK** — inlined in CRT sources; no IJG/libmad/bulanci game symbol |
| Fix `mapping.csv` / `_Globals.cpp` (`uchar` → `void`, namespace note) | Out of scope for FUN-only task |
| Retype `___FrameUnwindToState` param_1 → `EHRegistrationNode *` | See [batch_48_summary.md](../struct_recovery/batch_48_summary.md) (separate EH pass) |

## Cross-links

- [round8_fun_task_26_report.md](./round8_fun_task_26_report.md) — R8 task 26 was a different VA (`0x004500e2`); this seed was pool-listed only
- [r9_globals_task_039_report.md](./r9_globals_task_039_report.md) — similar CRT epilog thunk pattern (blocked MCP that session)
