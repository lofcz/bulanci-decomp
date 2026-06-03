# Round 7 FUN — Task 49 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 49 |
| **round** | 7 |
| **band** | dispatch |
| **seed_address** | `0x00465180` |
| **title** | FUN recovery: FUN_00465180 @ 0x00465180 (xrefs=1) |

## Status

**PARTIAL** — Role and xref closure proven from live Ghidra MCP (`connect_instance bulanci`). **No rename:** IJG/zlib export symbol not uniquely verified (Round 7 no-guess rule).

## Function

| Address | Ghidra name | Size | CC | Role summary | Evidence |
|---------|-------------|-----:|-----|--------------|----------|
| `0x00465180` | `FUN_00465180` | 336 B | __cdecl | Indexed Y/Cb/Cr samples through LUT block at cinfo+0x1a0 (+0x10..0x1c); writes 3-byte BGR tuples to output row (2-wide unroll + odd tail). | Decompile + xref_to; jdcolor.c YCbCr->BGR scanline color_convert |

### Xref closure

| From | Site | Type |
|------|------|------|
| `jinit_merged_upsampler` | `0x00465563` | DATA |

### Callees (selected)

| Callee | Role |
|--------|------|
| — | — |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_plate_comment` | `0x00465180` | applied |
| `save_program` | — | single save at task 41 |

## Frida

**none** — JPEG decompress/quantizer path; static xref + decompile sufficient.

## Remaining UNK

| Item | Why still UNK |
|------|----------------|
| IJG/zlib export name | Method pointer installed at jinit_merged_upsampler+0x63 (DATA xref 0x00465563); exact IJG symbol UNK. |
