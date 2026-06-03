# Round 7 — FUN Task 01 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 1 |
| **band** | cluster |
| **seed_address** | `0x0041b3b0` |
| **prior_hint** | (none) |

## Status

**DONE** — Custom-event case `0xF5` handler renamed; `CGaming *` typing confirmed.

## Function

| Address | Ghidra name (after) | Role | Evidence |
|---------|---------------------|------|----------|
| `0x0041b3b0` | `CGaming_OnCustomEvent_0xF5_RemoveEntitiesBySlotId` | Reverse-walk `vecSlotVec_2f8` (`+0x2F8`); if `entity+0x3C == param_1`, remove slot, `CGaming_UnregisterAndRemoveObject`, vtable `Release` (max 2) | **Caller:** `CGaming_OnCustomEvent@0x0042075c` `case 0xf5:` → `CALL 0x0041b3b0`. **Disasm:** `CMP [ESI+0xF0], ECX` (slot id). **Callees:** `CDynPtrArray_RemoveRange@0x004046c0`, `CGaming_UnregisterAndRemoveObject@0x00419ca0` |

## Ghidra deltas

| Action | Detail |
|--------|--------|
| `rename_function_by_address` | `CGaming_OnCustomEvent_0xF5_PurgeSlotVecEntities` → `CGaming_OnCustomEvent_0xF5_RemoveEntitiesBySlotId` |
| `save_program` | `bulanci.exe` (batch with tasks 5–10) |

## Frida

**none** — dispatch case and slot-id field offset proven statically.

## Remaining UNK

- Whether `param_1` is strictly a network slot id vs. another entity key (field `+0x3C` semantics on all entity types).
