# Round 8 FUN — Task 25 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 25 |
| **round** | 8 |
| **band** | eh |
| **seed_address** | `0x00472420` |
| **title** | FUN recovery: FUN_00472420 @ 0x00472420 (xrefs=2) |
| **prior_hint** | — |

## Status

**PARTIAL** — Ghidra MCP `connect_instance(bulanci)`; single seed `0x00472420`.

## Function

| Address | Ghidra name (before → after) | Role summary | Evidence |
|---------|------------------------------|--------------|----------|
| `0x00472420` | `FUN_00472420` → **`FUN_00472420`** | zlib 1.1.3 deflate stored-block emitter: bi_windup, LEN/NLEN, copy buf to pending out. Fragment of _tr_stored_block. | Callers: send_bits@0x004728e1 and 0x00472908. Callee bi_windup@0x004723a0. |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `0x00472420` | set_decompiler_comment | zlib _tr_stored_block |

## Frida

**none** — Static Ghidra disasm/decompile + xref closure.

## Remaining UNK

| Item | Reason |
|------|--------|
| Merge with send_bits | Split deflate helper cluster |

## Cross-links

- [ROUND8_FUN_PROTOCOL.md](../ROUND8_FUN_PROTOCOL.md)
- [gzip_stream.md](../../formats/gzip_stream.md)
