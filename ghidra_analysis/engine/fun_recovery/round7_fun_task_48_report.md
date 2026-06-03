# Round 7 FUN — Task 48 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 48 |
| **round** | 7 |
| **band** | dispatch |
| **seed_address** | `0x00463ee0` |
| **title** | FUN recovery: FUN_00463EE0 @ 0x00463ee0 (xrefs=1) |

## Status

**PARTIAL** — Role and xref closure proven from live Ghidra MCP (`connect_instance bulanci`). **No rename:** IJG/zlib export symbol not uniquely verified (Round 7 no-guess rule).

## Function

| Address | Ghidra name | Size | CC | Role summary | Evidence |
|---------|-------------|-----:|-----|--------------|----------|
| `0x00463ee0` | `FUN_00463EE0` | 113 B | __cdecl | alloc_large 0x54 -> cinfo+0x19c; *vtbl=start_pass_dcolor; per-component 0x100-byte LUT via alloc_large+memset. | Decompile + xref_to; jdcolor.c decompress color-deconverter init |

### Xref closure

| From | Site | Type |
|------|------|------|
| `FUN_00460200` | `0x004602ed` | UNCONDITIONAL_CALL |

### Callees (selected)

| Callee | Role |
|--------|------|
| `_memset` | LUT zero |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_plate_comment` | `0x00463ee0` | applied |
| `save_program` | — | single save at task 41 |

## Frida

**none** — JPEG decompress/quantizer path; static xref + decompile sufficient.

## Remaining UNK

| Item | Why still UNK |
|------|----------------|
| IJG/zlib export name | jinit_color_deconverter name taken @0x00464e00 (different body); rename deferred. |
