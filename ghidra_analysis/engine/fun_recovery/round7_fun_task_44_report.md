# Round 7 FUN — Task 44 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 44 |
| **round** | 7 |
| **band** | dispatch |
| **seed_address** | `0x00461160` |
| **title** | FUN recovery: FUN_00461160 @ 0x00461160 (xrefs=1) |

## Status

**PARTIAL** — Role and xref closure proven from live Ghidra MCP (`connect_instance bulanci`). **No rename:** IJG/zlib export symbol not uniquely verified (Round 7 no-guess rule).

## Function

| Address | Ghidra name | Size | CC | Role summary | Evidence |
|---------|-------------|-----:|-----|--------------|----------|
| `0x00461160` | `FUN_00461160` | 154 B | __cdecl | Shift sample rows before FUN_00460f30 edge expand; sets workspace +0x48. | Decompile + xref_to; jcprepct.c local (row shift) |

### Xref closure

| From | Site | Type |
|------|------|------|
| `FUN_00461460` | `0x00461331` | UNCONDITIONAL_CALL |

### Callees (selected)

| Callee | Role |
|--------|------|
| — | — |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_plate_comment` | `0x00461160` | applied |
| `save_program` | — | single save at task 41 |

## Frida

**none** — JPEG decompress/quantizer path; static xref + decompile sufficient.

## Remaining UNK

| Item | Why still UNK |
|------|----------------|
| IJG/zlib export name | Call chain 0x461331 -> 0x46142e documented; export label deferred. |
