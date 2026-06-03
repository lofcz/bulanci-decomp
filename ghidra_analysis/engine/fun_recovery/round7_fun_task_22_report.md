# Round 7 FUN — Task 22 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 22 |
| **round** | 7 |
| **band** | sim |
| **seed_address** | `0x0042c190` |
| **title** | FUN recovery: FUN_0042C190 @ 0x0042c190 (xrefs=1) |

## Status

**DONE** — Live Ghidra MCP decompile + disasm + sole xref prove symmetric **remove** counterpart to task 21 `CGameView_InsertBeforeSiblingAnchor`. Renamed `CGameView_RemoveFromSiblingAnchor`; `CGameView *` / `__thiscall`; program saved.

## Function

| Address | Ghidra name (before → after) | Role summary | Evidence |
|---------|------------------------------|--------------|----------|
| `0x0042c190` | `FUN_0042c190` → **`CGameView_RemoveFromSiblingAnchor`** | Unlink `this` from parent’s embedded sibling list (`pChainParent+0x54`) via `CDSChained_RemoveWithHeadFixup@0x0042fa50`; return **`uchar`** from primary vfn[9] `CDSView_InvalidateRectClipped(0,0)` | **Disasm:** `[ESI+0x4c]+0x54` → `CALL 0x0042fa50`; `CALL [vftable+0x24]`. **Xref:** sole caller `CGaming_InsertEntityByDepth@0x00418507` (head reinsert path). **Pair:** task 21 insert @ `0x0042c160` |

### Disassembly (`0x0042c190`–`0x0042c1b2`)

```
0042c190  MOV  EAX, [ESP+4]       ; anchor sibling
0042c196  MOV  ESI, ECX           ; this (entity view)
0042c197  MOV  ECX, [ESI+0x4c]    ; pChainParent
0042c19c  ADD  ECX, 0x54          ; embedded CDSChain head
0042c19f  CALL 0x0042fa50         ; CDSChained_RemoveWithHeadFixup
0042c1a6  CALL [EDX+0x24]         ; InvalidateRectClipped
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0042c190` | `CGameView_RemoveFromSiblingAnchor` |
| `set_function_prototype` | `0x0042c190` | `uchar CGameView_RemoveFromSiblingAnchor(int anchor)` + `__thiscall` |
| `set_function_this_type` | `0x0042c190` | `CGameView *` |
| `set_decompiler_comment` | `0x0042c190` | Role + caller + pair with insert |
| `force_decompile` | `0x0042c190` | `pChainParent+0x54` field access |
| `save_program` | `bulanci.exe` | saved |

## Frida

**none** — static disasm/decompile + single caller + vtable slot 9 sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| `anchor` parameter type | Caller passes sibling view pointer; callee third arg still typed `int` on `RemoveWithHeadFixup` |
| `mapping.csv` | Still `CBulanek::FUN_0042c190` stub until export sync |
| Callee `CDSChained_RemoveWithHeadFixup` middle arg | Still `CBulanek *` in decompiler — chain-head typing deferred |

## Cross-links

- [round7_fun_task_21_report.md](round7_fun_task_21_report.md) — insert pair
- [round5_worker_08_report.md](../struct_recovery/round5_worker_08_report.md) — head-fixup callees
- [CGameView.md](../struct_recovery/CGameView.md) — `pChainParent` @ `+0x4c`, chain @ `+0x54`
