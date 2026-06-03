# Round 8 FUN — Task 26 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 26 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x004500e2` |
| **title** | FUN recovery: FUN_004500E2 @ 0x004500e2 (xrefs=1) |
| **prior_hint** | — |

## Status

**PARTIAL** — Ghidra MCP `connect_instance(bulanci)`; single seed `0x004500e2`.

## Function

| Address | Ghidra name (before → after) | Role summary | Evidence |
|---------|------------------------------|--------------|----------|
| `0x004500e2` | `FUN_004500E2` → **`FUN_004500E2`** | _raise SEH epilog: __unlock(0) if [EBP-0x1c] != 0. | Xref: _raise@0x004500c8. Decompile: conditional __unlock(0). |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `0x004500e2` | set_decompiler_comment | _raise epilog |

## Frida

**none** — Static Ghidra disasm/decompile + xref closure.

## Remaining UNK

| Item | Reason |
|------|--------|
| — | — |

## Cross-links

- [ROUND8_FUN_PROTOCOL.md](../ROUND8_FUN_PROTOCOL.md)
- [round5_worker_15_report.md](../struct_recovery/round5_worker_15_report.md)
