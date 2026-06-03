# Round 8 FUN — Task 46 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 46 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x0046acf0` |
| **title** | FUN recovery: FUN_0046acf0 @ 0x0046acf0 (xrefs=1) |
| **prior_hint** | *(empty)* |

## Status

**DONE** — Live Ghidra MCP (`connect_instance bulanci`, 2026-06-03). Renamed `jinit_c_prep_controller` — alloc 0x40 prep object at cinfo+0x144; caller `jinit_compress_master`.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x0046acf0` | `FUN_0046acf0` | **`jinit_c_prep_controller`** | Alloc 0x40 at cinfo+0x144; wires FUN_0046a870 or compress_output path | Sole CALL xref from `jinit_compress_master` @ `0x00460de8` |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Reference (sole)** | `0x00460de8` | `jinit_compress_master` | CALL |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0046acf0` | Success |
| `save_program` | `0x0046acf0` | Success |
| `save_program` | `bulanci.exe` | Saved (batch 41–50) |

## Frida

**none**

## Remaining UNK

- rename blocked: not jinit_c_prep/coef_controller GLOBAL
