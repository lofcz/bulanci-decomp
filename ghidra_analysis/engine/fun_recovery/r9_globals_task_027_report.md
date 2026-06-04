# Round 9 `_Globals` FUN — Task 027 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 27 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim *(manifest cluster — VA is MSVC CRT C++ EH, not game logic)* |
| **seed_address** | `0x0044b490` |
| **ghidra_name** | `FUN_0044B490` |
| **prior_hint** | *(empty)* |
| **prior art** | R5 worker 16 skip table; [EHExceptionRecord.md](../struct_recovery/EHExceptionRecord.md); sibling [r9_globals_task_026_report.md](./r9_globals_task_026_report.md) (`FUN_0044b1cf`) |

## Status

**PARTIAL** — Live Ghidra MCP (`connect_instance bulanci`, 2026-06-04). Role and xref closure proven: MSVC **`CallCatchBlock` catch epilogue** (frame unlink, TLS restore, conditional `___DestructExceptionObject`). **Sim body written** in `src/bulanci/_Globals.cpp` from live decompile/disasm. **No rename** — no separate VS2005 CRT export for this 118-byte tail; protocol forbids invented helper names.

## Function

| Address | Ghidra name | Proposed name | Role | Evidence |
|---------|-------------|---------------|------|----------|
| `0x0044b490` | `FUN_0044b490` | *(keep `FUN_*`)* | **`CallCatchBlock` epilog:** restore `[EDI-4]` from `[EBP-0x24]`; `__FindAndUnlinkFrame([EBP-0x28])`; restore `__getptd()->_curexception` / `_curcontext` from `[EBP-0x2c]` / `[EBP-0x30]`; if C++ EH record (`0xE06D7363`, 3 params, magic `0x19930520/21/22`, `[EBP-0x34]==0`, `[EBP-0x1c]!=0`) and `__IsExceptionObjectToBeDestroyed` on thrown object → `___DestructExceptionObject` | See below |

### Caller / callee closure (Ghidra live)

| Kind | Address | Symbol | Notes |
|------|---------|--------|-------|
| **To (sole caller)** | `0x0044b477` | `CallCatchBlock` | `CALL 0x0044b490` after catch completes (`[EBP-0x4]=0xfffffffe`, `[EBP+0x10]=0`) |
| **From (callees)** | `0x00447b2b` | `__FindAndUnlinkFrame` | `PUSH [EBP-0x28]` |
| | `0x0044ad16` | `__getptd` | Twice; stores at `EAX+0x88` / `EAX+0x8c` (`_curexception` / `_curcontext`) |
| | `0x00447b0a` | `__IsExceptionObjectToBeDestroyed` | `PUSH [ESI+0x18]` (thrown object) |
| | `0x0044b22f` | `___DestructExceptionObject` | `PUSH ESI` when destroy test non-zero |

`get_xrefs_to@0x0044b490` → **1** xref. No game reachability.

### Disassembly (live, 118 B)

```
0044b490  MOV  EAX, [EBP-0x24]
0044b493  MOV  [EDI-0x4], EAX
0044b496  PUSH [EBP-0x28]
0044b499  CALL __FindAndUnlinkFrame
0044b49f  CALL __getptd
0044b4a7  MOV  [EAX+0x88], ECX          ; _curexception ← [EBP-0x2c]
0044b4ad  CALL __getptd
0044b4b5  MOV  [EAX+0x8c], ECX          ; _curcontext ← [EBP-0x30]
0044b4bb  CMP  [ESI], 0xE06D7363
0044b4c3  CMP  [ESI+0x10], 3
0044b4c9  MOV  EAX, [ESI+0x14]
0044b4cc  CMP  EAX, 0x19930520 / 21 / 22
0044b4e1  CMP  [EBP-0x34], 0
0044b4e7  CMP  [EBP-0x1c], 0
0044b4ed  PUSH [ESI+0x18]
0044b4f0  CALL __IsExceptionObjectToBeDestroyed
0044b4fa  PUSH [EBP+0x10]
0044b4fd  PUSH ESI
0044b4fe  CALL ___DestructExceptionObject
0044b505  RET
```

**No function prologue** — body uses **CallCatchBlock’s `EBP`**; `ESI` = `ExceptionRecord`, `EDI` = `Frame` from caller setup @ `0x0044b378` / `0x0044b37b`.

### `EHExceptionRecord` field proof (this function)

| Offset | Field | Use @ `0x0044b490` |
|--------|-------|---------------------|
| `0x00` | `ExceptionCode` | `== 0xE06D7363` |
| `0x10` | `NumberParameters` | `== 3` |
| `0x14` | `ExceptionInformation[0]` | magic `0x19930520` / `21` / `22` |
| `0x18` | `ExceptionInformation[1]` | arg to `__IsExceptionObjectToBeDestroyed` |

See [EHExceptionRecord.md](../struct_recovery/EHExceptionRecord.md).

### Signature / mapping stub

| Source | Value |
|--------|-------|
| `config/bulanci/mapping.csv` | `;_Globals::FUN_0044b490;0x44b490;0x76;__stdcall;;uchar` |
| Live Ghidra | `undefined __stdcall FUN_0044b490(void)` — **`void` epilog; `uchar` unused** |

### Sim write (`_Globals.cpp`)

Decompile-faithful C++ with `__asm` capture of caller `EBP`/`ESI`/`EDI` on entry (no binary prologue). Offsets `+0x88` / `+0x8c` on `_tiddata` match `CallCatchBlock` disasm @ `0x0044b3a1` / `0x0044b3af`. Callees via `Runtime::MSVCRT::*` stubs.

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0044b490` | Epilog role + sole caller `0x0044b477` + callee/offset summary |
| `save_program` | `bulanci.exe` | Saved after comment |

**Not applied:** `rename_function_by_address` (no upstream CRT export name for this tail).

## Frida

**none** — CRT-only; static xref + disasm sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Official MSVC symbol for extracted `CallCatchBlock` epilog tail | **UNK** — inlined in CRT; keep `FUN_0044b490` |
| Fix `mapping.csv` / decl (`uchar` → `void`) | Out of scope for FUN-only task |
| Sim fidelity when not called from `CallCatchBlock` stack layout | **UNK** — requires caller `EBP`/`ESI`/`EDI`; documented in source comment |
| `CallCatchBlock` full sim | Separate symbol (`CallCatchBlock@0x0044b36a`, not `_Globals`) |

## Cross-links

- [round5_worker_16_report.md](../struct_recovery/round5_worker_16_report.md) — CRT EH catch epilogue skip note
- [r9_globals_task_026_report.md](./r9_globals_task_026_report.md) — adjacent `___FrameUnwindToState` epilog @ `0x0044b1cf`
