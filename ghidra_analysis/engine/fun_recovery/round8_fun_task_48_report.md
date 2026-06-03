# Round 8 FUN — Task 48 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 48 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x0046ae80` |
| **title** | FUN recovery: FUN_0046ae80 @ 0x0046ae80 (xrefs=1) |
| **prior_hint** | *(empty)* |

## Status

**DONE** — Live Ghidra MCP (`connect_instance bulanci`, 2026-06-03). int_downsample.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x0046ae80` | `FUN_0046ae80` | **`int_downsample`** | IJG jcsample.c int_downsample integral-ratio box filter | Sole DATA xref from `FUN_0046b590` @ `0x0046b69c` |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Reference (sole)** | `0x0046b69c` | `FUN_0046b590` | DATA |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0046ae80` | `FUN_0046ae80` → `int_downsample` |
| `save_program` | `0x0046ae80` | Success |
| `save_program` | `bulanci.exe` | Saved (batch 41–50) |

## Frida

**none**

## Remaining UNK

- *(none for rename scope)*
