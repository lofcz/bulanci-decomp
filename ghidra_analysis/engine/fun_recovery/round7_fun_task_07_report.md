# Round 7 — FUN Task 07 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 7 |
| **band** | sim |
| **seed_address** | `0x0042c140` |
| **prior_hint** | (none) |

## Status

**DONE** — Already renamed; disasm proof re-verified.

## Function

| Address | Ghidra name (after) | Role | Evidence |
|---------|---------------------|------|----------|
| `0x0042c140` | `CDSView_InputChain_OnDetach` | Clear input-chain flag: `AND word [this+0x44], 0xFFF7`; if `byte [this+0x14] & 8` → vtable+`0x48` | **Xrefs_to:** `CDSApp_SetInputChainHead@0x0042c836`, `CDSView_UpdateInputChainOnMouseMove@0x0042c8b5` |

## Ghidra deltas

**none** — R6/R7 prior rename held.

## Frida

**none**

## Remaining UNK

- Published export name for vtable+`0x48` callee on detach path.
