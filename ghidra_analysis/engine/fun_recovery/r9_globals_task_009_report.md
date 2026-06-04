# Round 9 `_Globals` FUN — Task 009 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 9 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x0044714a` |
| **ghidra_name (before)** | `FUN_0044714A` |
| **prior_hint** | *(empty)* |

## Status

**DONE** — Renamed to `CDSWorkingThread_ThreadProcEpilogue` in Ghidra with disasm + single-caller + field-offset proof. `save_program bulanci.exe` applied.

## Function

| Address | Ghidra (after) | Role | Evidence |
|---------|----------------|------|----------|
| `0x0044714a` | `CDSWorkingThread_ThreadProcEpilogue` | **Worker thread exit tail** for `CDSWorkingThread_ThreadProc`: set `m_bWorkerThreadExited` at `this+0x10`, call `__endthreadex(0)`, run shared SEH epilogue | See below |

### Caller closure (primary proof)

| From | Type | Context |
|------|------|---------|
| `0x0044712c` | `JMP` (xref: UNCONDITIONAL_CALL) | Last insn of `_Globals::CDSWorkingThread_ThreadProc@0x004470f0` after `CALL` through `vtable+0x10` |

`CDSWorkingThread_ThreadProc` decompile (post-rename):

```c
(**(code **)(*param_1 + 0x10))();
CDSWorkingThread_ThreadProcEpilogue();
```

`CDSObject::CDSWorkingThread_Start@0x004471d0` passes `_Globals::CDSWorkingThread_ThreadProc` to `__beginthreadex` with `this` as thread param — `ESI` in epilogue is that `CDSWorkingThread *`.

### Disassembly @ `0x0044714a` (Ghidra, size `0x23`)

```
0044714a  PUSH EDI
0044714b  MOV  byte ptr [ESI + 0x10], 0x1
0044714f  CALL 0x004495f8          ; Runtime::MSVCRT::__endthreadex(uint)
00447154  ADD  ESP, 0x4
00447157  XOR  EAX, EAX
00447159  MOV  ECX, dword ptr [EBP + -0xc]
0044715c  MOV  dword ptr FS:[0x0], ECX
00447163  POP  ECX
00447164  POP  EDI
00447165  POP  ESI
00447166  POP  EBX
00447167  MOV  ESP, EBP
00447169  POP  EBP
0044716a  RET  0x4
```

Thread proc sets `EDI = 0` (`XOR EDI,EDI` @ `0x00447123`) before `JMP` — `__endthreadex(0)` exit code.

### Field offset `+0x10` (documented bulanci symbol)

| Source | Claim |
|--------|-------|
| [CDSDirectSound.md](../struct_recovery/CDSDirectSound.md) | `m_bWorkerThreadExited` @ worker base `+0x34` on `CDSDirectSound`; worker embed base `+0x24` → **`+0x10` on `CDSWorkingThread`** |
| `CDSWorkingThread_PollExited@0x004470c0` | `if (*(char *)(param_1 + 0x10) == '\0') return false;` before `GetExitCodeThread` |

Epilogue write `*(ESI+0x10)=1` pairs with poll gate on the same byte.

### mapping.csv / stub (not trusted for naming)

```
;_Globals::FUN_0044714a;0x44714a;0x23;__stdcall;;uint
```

Size `0x23` matches Ghidra (`instruction_count`: 14). `__stdcall` / standalone signature is a Ghidra artifact — entry is **`JMP`**, not a normal call; registers inherited from `ThreadProc`.

### Prior art

| Source | Note |
|--------|------|
| [mcp_apply_log.md](../../netcode/mcp_apply_log.md) | `0x004470f0` → `CDSWorkingThread_ThreadProc` (R6 net apply) |
| R8 manifest | `0x0044714a`, xref_count 1 (sim band) |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0044714a` | `FUN_0044714a` → `CDSWorkingThread_ThreadProcEpilogue` |
| `set_decompiler_comment` | `0x0044714a` | ThreadProc tail / `ESI+0x10` / `__endthreadex` |
| `force_decompile` | `0x0044714a`, `0x004470f0` | Caller shows named epilogue call |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — static xref + field offset + `PollExited` cross-proof sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Ghidra `__stdcall` / `undefined4` return on epilogue fragment | Cosmetic — not independently entered; inherited frame from `ThreadProc` |
| `_Globals.h` / `_Globals.cpp` / `mapping.csv` stub update | Out of scope (FUN-only Ghidra pass) |
| `CDSWorkingThread` struct field name in DT manager | Documented in `CDSDirectSound.md`; struct apply may lag rename |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [GHIDRA_MCP.md](../GHIDRA_MCP.md)
- [CDSDirectSound.md](../struct_recovery/CDSDirectSound.md) — worker `m_bWorkerThreadExited` offset chain
