# Round 8 FUN — Task 45 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 45 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x0046abd0` |
| **title** | FUN recovery: FUN_0046abd0 @ 0x0046abd0 (xrefs=1) |
| **prior_hint** | *(empty)* |

## Status

**PARTIAL** — Live Ghidra MCP (`connect_instance bulanci`, 2026-06-03). coef row merge LOCAL.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x0046abd0` | `FUN_0046abd0` | **`FUN_0046abd0`** | Multi-pass sample row merge before compress_output | Sole CALL xref from `FUN_0046acf0` @ `0x0046ad3d` |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Reference (sole)** | `0x0046ad3d` | `FUN_0046acf0` | CALL |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0046abd0` | Success |
| `save_program` | `0x0046abd0` | Success |
| `save_program` | `bulanci.exe` | Saved (batch 41–50) |

## Frida

**none**

## Remaining UNK

- rename blocked: LOCAL helper without IJG export
