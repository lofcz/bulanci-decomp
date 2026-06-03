# Round 8 FUN — Task 47 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 47 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x0046adf0` |
| **title** | FUN recovery: FUN_0046adf0 @ 0x0046adf0 (xrefs=1) |
| **prior_hint** | *(empty)* |

## Status

**DONE** — Live Ghidra MCP (`connect_instance bulanci`, 2026-06-03). sep_downsample.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x0046adf0` | `FUN_0046adf0` | **`sep_downsample`** | IJG jcsample.c sep_downsample per-component dispatch | Sole DATA xref from `FUN_0046b590` @ `0x0046b5b5` |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Reference (sole)** | `0x0046b5b5` | `FUN_0046b590` | DATA |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0046adf0` | `FUN_0046adf0` → `sep_downsample` |
| `save_program` | `0x0046adf0` | Success |
| `save_program` | `bulanci.exe` | Saved (batch 41–50) |

## Frida

**none**

## Remaining UNK

- *(none for rename scope)*
