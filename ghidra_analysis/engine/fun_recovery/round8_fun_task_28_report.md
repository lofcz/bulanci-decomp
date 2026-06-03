# Round 8 FUN — Task 28 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 28 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x00452869` |
| **title** | FUN recovery: FUN_00452869 @ 0x00452869 (xrefs=1) |
| **prior_hint** | — |

## Status

**PARTIAL** — Ghidra MCP `connect_instance(bulanci)`; single seed `0x00452869`.

## Function

| Address | Ghidra name (before → after) | Role summary | Evidence |
|---------|------------------------------|--------------|----------|
| `0x00452869` | `FUN_00452869` → **`FUN_00452869`** | __lseeki64 epilog: MSVCRT___unlock_fhandle(*(EBP+8)). | Xref: __lseeki64@0x00452858. |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `0x00452869` | set_decompiler_comment | lseek unlock |

## Frida

**none** — Static Ghidra disasm/decompile + xref closure.

## Remaining UNK

| Item | Reason |
|------|--------|
| — | — |

## Cross-links

- [ROUND8_FUN_PROTOCOL.md](../ROUND8_FUN_PROTOCOL.md)
- [round5_worker_15_report.md](../struct_recovery/round5_worker_15_report.md)
