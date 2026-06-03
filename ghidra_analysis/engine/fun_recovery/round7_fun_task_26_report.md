# Round 7 FUN — Task 26 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 26 |
| **title** | FUN recovery: FUN_00436D20 @ 0x00436d20 (xrefs=1) |
| **band** | sim |
| **seed_address** | `0x00436d20` |
| **prior_hint** | R6 task 38 — generic `CDSPtrSlotVec` resize+copy; stale export comment tagged CDSAudioBank |

## Status

**DONE** — Live Ghidra MCP confirms body is **`CDSPtrSlotVec_Resize` + `MemMoveDword`** on a 3-word source descriptor. Renamed to **`CDSPtrSlotVec_AssignFromDesc`**, typed `CDSPtrSlotVec *` / `__thiscall`. Not audio-bank-specific despite sole caller chain reaching `FUN_004370b0` (track-switch helper).

## Function

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x00436d20` | `CDSPtrSlotVec_AssignFromDesc` | **`CDSPtrSlotVec_Resize(this, desc[1])`**; if `cCapacity != 0`, **`MemMoveDword(pSlots, desc[0], cCapacity)`**. `desc[2]` unused (wrapper `FUN_00436f20` writes sibling count @ `this+8`). | Disasm: `CALL 0x00406340` then `CALL 0x00403240`; decompile field access `this->pSlots` / `this->nCapacity`; xref **1** from `FUN_00436f20@0x00436f30`; callee chain `FUN_004370b0` → `SetCurrentTrack` audio path |

### Disassembly proof

```
00436d22  MOV EDI,[ESP+0xc]      ; desc
00436d26  MOV EAX,[EDI+0x4]      ; desc[1] = new capacity
00436d2a  MOV ESI,ECX            ; this
00436d2c  CALL CDSPtrSlotVec_Resize
00436d31  MOV EAX,[ESI+0x4]      ; cCapacity
00436d36  JZ  skip
00436d38  MOV ECX,[EDI]          ; desc[0] = src pSlots
00436d3a  MOV EDX,[ESI]          ; this->pSlots
00436d3f  CALL MemMoveDword
```

### Caller closure

| Address | Name | Use |
|---------|------|-----|
| `0x00436f20` | `FUN_00436f20` | `*(this+8)=0`; `CDSPtrSlotVec_AssignFromDesc(this, param_1)`; `*(this+8)=param_1[2]` — copies `{pSlots,cCapacity,cCount}` bundle |
| `0x004370b0` | `FUN_004370b0` | Stack-local vec; `FUN_00436f20(&local, parent+0x38)`; iterate slots calling vtable+8 (track switch) |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x00436d20` | `void __thiscall CDSPtrSlotVec_AssignFromDesc(undefined4 * desc)` |
| `set_function_this_type` | `0x00436d20` | `CDSPtrSlotVec *` — moved into `CDSPtrSlotVec` class namespace |
| `rename_function_by_address` | `0x00436d20` | `CDSPtrSlotVec_AssignFromDesc` |
| `set_decompiler_comment` | `0x00436d20` | Descriptor layout + caller chain note |
| `force_decompile` | `0x00436d20` | Field-aware decompile (`this->pSlots`, `this->nCapacity`) |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — Resize+copy semantics fully closed from disasm/xrefs; runtime would only duplicate slot counts already visible in `FUN_004370b0` stack locals.

## Remaining UNK

| Item | Reason |
|------|--------|
| `FUN_00436f20` / `FUN_004370b0` | Wrapper and track-switch iterator — out of scope (tasks 27+); `FUN_00436f20` extends vec with `+8` live count |
| Formal `CDSPtrSlotVecDesc` struct | Three-word `{pSlots,cCapacity,cCount}` inferred from wrapper; not created in DT manager this session |
| Other potential call sites | Only **1** xref today; if future xrefs appear, name remains generic (not audio-only) |
