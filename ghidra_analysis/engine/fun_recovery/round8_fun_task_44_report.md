# Round 8 FUN — Task 44 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 44 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x0046a870` |
| **title** | FUN recovery: FUN_0046a870 @ 0x0046a870 (xrefs=1) |
| **prior_hint** | *(empty)* |

## Status

**DONE** — Live Ghidra MCP matches **IJG libjpeg-6b `jcprepct.c` `pre_process_data`** (simple / no-context-row compression preprocessing). Renamed `FUN_0046a870` → **`pre_process_data`**; prototype and comments applied; `bulanci.exe` saved.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x0046a870` | `FUN_0046a870` | **`pre_process_data`** | Calls `color_convert` then `downsample`; pads via `FUN_0046a840` (`expand_bottom_edge` homolog) | Sole DATA xref `jinit_c_prep_controller@0x0046ad45` → `prep+4` when downsampler needs no context rows |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Reference (sole)** | `0x0046ad45` | `jinit_c_prep_controller` | DATA install |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function` | `0x0046a870` | `pre_process_data` |
| `set_function_prototype` | `0x0046a870` | Applied |
| `set_decompiler_comment` | `0x0046a870` | Applied |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none**

## Remaining UNK

- Mislabeled `compress_output` @ `0x0046aa00` is **`pre_process_context`** (context-row path); true `compress_output` @ `0x00467930` (R7).
