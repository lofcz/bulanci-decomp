# Round 8 FUN — Task 50 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 50 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x0046b0b0` |
| **title** | FUN recovery: FUN_0046b0b0 @ 0x0046b0b0 (xrefs=1) |
| **prior_hint** | *(empty)* |

## Status

**DONE** — Live Ghidra MCP (`connect_instance bulanci`, 2026-06-03). h2v2_downsample.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x0046b0b0` | `FUN_0046b0b0` | **`h2v2_downsample`** | IJG jcsample.c h2v2_downsample (corrected from h2v2_upsample misname) | Sole DATA xref from `FUN_0046b590` @ `0x0046b67f` |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Reference (sole)** | `0x0046b67f` | `FUN_0046b590` | DATA |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0046b0b0` | `FUN_0046b0b0` → `h2v2_downsample` |
| `save_program` | `0x0046b0b0` | Success |
| `save_program` | `bulanci.exe` | Saved (batch 41–50) |

## Frida

**none**

## Remaining UNK

- *(none for rename scope)*
