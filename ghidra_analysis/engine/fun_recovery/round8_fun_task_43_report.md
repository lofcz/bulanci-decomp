# Round 8 FUN — Task 43 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 43 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x00469900` |
| **title** | FUN recovery: FUN_00469900 @ 0x00469900 (xrefs=1) |
| **prior_hint** | *(empty)* |

## Status

**DONE** — Live Ghidra MCP (`connect_instance bulanci`, 2026-06-03). Progressive Huffman entropy init renamed `jinit_phuff_encoder` (alloc 0x6c, `start_pass_huff_decoder` vfunc).

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x00469900` | `FUN_00469900` | **`jinit_phuff_encoder`** | Alloc 0x6c entropy object; start_pass=start_pass_huff_decoder when progressive | Sole CALL xref from `jinit_compress_master` @ `0x00460e1d` |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Reference (sole)** | `0x00460e1d` | `jinit_compress_master` | CALL |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x00469900` | Success |
| `save_program` | `0x00469900` | Success |
| `save_program` | `bulanci.exe` | Saved (batch 41–50) |

## Frida

**none**

## Remaining UNK

- rename blocked: binary split from jinit_huff_encoder; no separate IJG GLOBAL
