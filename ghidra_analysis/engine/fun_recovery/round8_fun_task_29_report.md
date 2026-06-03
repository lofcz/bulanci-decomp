# Round 8 FUN — Task 29 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 29 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x00452f0b` |
| **title** | FUN recovery: FUN_00452F0B @ 0x00452f0b (xrefs=1) |
| **prior_hint** | — |

## Status

**PARTIAL** — Ghidra MCP `connect_instance(bulanci)`; single seed `0x00452f0b`.

## Function

| Address | Ghidra name (before → after) | Role summary | Evidence |
|---------|------------------------------|--------------|----------|
| `0x00452f0b` | `FUN_00452F0B` → **`FUN_00452F0B`** | __write epilog: MSVCRT___unlock_fhandle(*(EBP+8)). | Xref: __write@0x00452efd. |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `0x00452f0b` | set_decompiler_comment | write unlock |

## Frida

**none** — Static Ghidra disasm/decompile + xref closure.

## Remaining UNK

| Item | Reason |
|------|--------|
| — | — |

## Cross-links

- [ROUND8_FUN_PROTOCOL.md](../ROUND8_FUN_PROTOCOL.md)
- [round5_worker_15_report.md](../struct_recovery/round5_worker_15_report.md)
