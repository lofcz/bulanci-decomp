# Round 7 FUN — Task 50 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 50 |
| **round** | 7 |
| **band** | dispatch |
| **seed_address** | `0x00466920` |
| **title** | FUN recovery: FUN_00466920 @ 0x00466920 (xrefs=1) |

## Status

**PARTIAL** — Role and xref closure proven from live Ghidra MCP (`connect_instance bulanci`). **No rename:** IJG/zlib export symbol not uniquely verified (Round 7 no-guess rule).

## Function

| Address | Ghidra name | Size | CC | Role summary | Evidence |
|---------|-------------|-----:|-----|--------------|----------|
| `0x00466920` | `FUN_00466920` | 231 B | __cdecl | Nested factorial loop over param_2[]; optional remap via jpeg_natural_order@0x0049df50 when param_1[0xb]==2; returns alloc size to FUN_00466a50; JERR 0x38 if bound<2. | Decompile + xref_to; merged-upsampler ordered-dither table size calc (not zlib gen_codes) |

### Xref closure

| From | Site | Type |
|------|------|------|
| `FUN_00466a50` | `0x00466a6a` | UNCONDITIONAL_CALL |

### Callees (selected)

| Callee | Role |
|--------|------|
| — | — |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_plate_comment` | `0x00466920` | applied |
| `save_program` | — | single save at task 41 |

## Frida

**none** — JPEG decompress/quantizer path; static xref + decompile sufficient.

## Remaining UNK

| Item | Why still UNK |
|------|----------------|
| IJG export name | Caller FUN_00466a50 on merged-upsampler init chain; corrects R6 zlib/gen_codes attribution; COFF symbol UNK. |
