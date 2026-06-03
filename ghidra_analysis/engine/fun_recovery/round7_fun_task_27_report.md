# Round 7 FUN — Task 27 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 27 |
| **round** | 7 |
| **band** | sim |
| **seed_address** | `0x00436f20` |
| **title** | FUN recovery: FUN_00436F20 @ 0x00436f20 (xrefs=1) |

## Status

**DONE** — Disasm + decompile + xref prove a thin **`CDSTrackVector`** wrapper around `CDSPtrSlotVec_AssignFromDesc@0x00436d20` that also copies live track count `desc[2]` → `this+0x8`. Renamed; typed `CDSTrackVector *` / `__thiscall`; program saved.

## Function

| Address | Ghidra name (before → after) | Role summary | Evidence |
|---------|------------------------------|--------------|----------|
| `0x00436f20` | `FUN_00436f20` → **`CDSTrackVector_AssignFromDesc`** | Zero `dwTracks`; **`CDSPtrSlotVec_AssignFromDesc((CDSPtrSlotVec*)this, desc)`** (resize+copy slot array); restore **`dwTracks = desc[2]`**. Descriptor layout **`{pSlots, cAllocated, cTracks}`** | **Disasm:** `MOV [ESI+8],0`; `CALL 0x00436d20`; `MOV [ESI+8],[EDI+8]`. **Xref (1):** `FUN_004370b0@0x004370fb` — stack-local vec from `parent+0x38`, iterate vtable+8 for track switch before `SetCurrentTrack` |

### Descriptor layout (3 × `uint32`)

| Index | Field | Use |
|-------|-------|-----|
| `desc[0]` | `pSlots` | Source pointer array for `MemMoveDword` |
| `desc[1]` | capacity | Passed to `CDSPtrSlotVec_Resize` |
| `desc[2]` | live count | Stored at `CDSTrackVector+0x8` (`dwTracks`) after copy |

### Caller chain

```mermaid
flowchart LR
  SCT[SetCurrentTrack path FUN_004370b0]
  Assign[CDSTrackVector_AssignFromDesc 0x436f20]
  Core[CDSPtrSlotVec_AssignFromDesc 0x436d20]
  SCT --> Assign --> Core
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x00436f20` | `CDSTrackVector_AssignFromDesc` |
| `set_function_prototype` | `0x00436f20` | `void CDSTrackVector_AssignFromDesc(undefined4 * desc)` + `__thiscall` |
| `set_function_this_type` | `0x00436f20` | `CDSTrackVector *` |
| `set_decompiler_comment` | `0x00436f20` | Descriptor layout + caller chain |
| `force_decompile` | `0x00436f20` | Calls typed `CDSPtrSlotVec_AssignFromDesc` |
| `save_program` | `bulanci.exe` | saved |

## Frida

**none** — three-instruction wrapper; slot iteration visible in `FUN_004370b0` decompile.

## Remaining UNK

| Item | Reason |
|------|--------|
| `FUN_004370b0` | Track-switch iterator — out of R7 scope; sole caller of this wrapper |
| Formal `CDSTrackVectorDesc` struct | Three-word bundle inferred; not created in DT manager |
| `mapping.csv` | Still `_Globals::FUN_00436f20` comment stub |

## Cross-links

- [round7_fun_task_26_report.md](round7_fun_task_26_report.md) — `CDSPtrSlotVec_AssignFromDesc` callee
- [round6_logic_task_38_report.md](../logic_recovery/round6_logic_task_38_report.md) — stale CDSAudioBank label corrected
- [CDSTrackVector.md](../struct_recovery/CDSTrackVector.md) — `pTracks` / `dwTracks` layout
