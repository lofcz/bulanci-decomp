# Round 8 FUN — Task 30 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 30 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x00454ace` |
| **title** | FUN recovery: FUN_00454ACE @ 0x00454ace (xrefs=1) |
| **prior_hint** | — |

## Status

**PARTIAL** — Ghidra MCP `connect_instance(bulanci)`; single seed `0x00454ace`.

## Function

| Address | Ghidra name (before → after) | Role summary | Evidence |
|---------|------------------------------|--------------|----------|
| `0x00454ace` | `FUN_00454ACE` → **`FUN_00454ACE`** | ___lock_fhandle epilog: __unlock(10) after __lock(10). | Xref: ___lock_fhandle@0x00454a99. Decompile: __unlock(10). |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `0x00454ace` | set_decompiler_comment | lock_fhandle epilog |

## Frida

**none** — Static Ghidra disasm/decompile + xref closure.

## Remaining UNK

| Item | Reason |
|------|--------|
| — | — |

## Cross-links

- [ROUND8_FUN_PROTOCOL.md](../ROUND8_FUN_PROTOCOL.md)
- [round5_worker_15_report.md](../struct_recovery/round5_worker_15_report.md)
