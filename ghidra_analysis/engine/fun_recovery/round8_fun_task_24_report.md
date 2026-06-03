# Round 8 FUN — Task 24 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 24 |
| **round** | 8 |
| **band** | sim |
| **seed_address** | `0x0044fe54` |
| **title** | FUN recovery: FUN_0044FE54 @ 0x0044fe54 (xrefs=2) |
| **prior_hint** | — |

## Status

**PARTIAL** — Ghidra MCP `connect_instance(bulanci)`; single seed `0x0044fe54`.

## Function

| Address | Ghidra name (before → after) | Role summary | Evidence |
|---------|------------------------------|--------------|----------|
| `0x0044fe54` | `FUN_0044FE54` → **`FUN_0044FE54`** | SEH unwind dispatch: indirect tail-call (*in_EAX)() from __local_unwind4/2. | Xrefs: __local_unwind4@0x0044fc07, __local_unwind2@0x0044fdf0. Size 3 B. |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `0x0044fe54` | set_decompiler_comment | SEH unwind |

## Frida

**none** — Static Ghidra disasm/decompile + xref closure.

## Remaining UNK

| Item | Reason |
|------|--------|
| EAX target | NLG dispatch; not resolved statically |

## Cross-links

- [ROUND8_FUN_PROTOCOL.md](../ROUND8_FUN_PROTOCOL.md)
