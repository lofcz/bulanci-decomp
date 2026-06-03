# Round 8 FUN — Task 49 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 49 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x0046b000` |
| **title** | FUN recovery: FUN_0046b000 @ 0x0046b000 (xrefs=1) |
| **prior_hint** | *(empty)* |

## Status

**DONE** — Live Ghidra MCP (`connect_instance bulanci`, 2026-06-03). h2v1_downsample.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x0046b000` | `FUN_0046b000` | **`h2v1_downsample`** | IJG jcsample.c h2v1_downsample 2:1 horizontal box filter | Sole DATA xref from `FUN_0046b590` @ `0x0046b651` |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Reference (sole)** | `0x0046b651` | `FUN_0046b590` | DATA |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0046b000` | `FUN_0046b000` → `h2v1_downsample` |
| `save_program` | `0x0046b000` | Success |
| `save_program` | `bulanci.exe` | Saved (batch 41–50) |

## Frida

**none**

## Remaining UNK

- *(none for rename scope)*
