# Round 7 — FUN Task 09 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 9 |
| **band** | sim |
| **seed_address** | `0x0042ec90` |
| **prior_hint** | (none) |

## Status

**DONE** — Renamed; `CDSView *` this typing applied.

## Function

| Address | Ghidra name (after) | Role | Evidence |
|---------|---------------------|------|----------|
| `0x0042ec90` | `CDSView_DispatchSyntheticEventIfMasked` | If `(this+4) & eventMask` non-zero: `*event=this`, vtable+`0x10` dispatch; else return 0 | **Xrefs_to:** `BroadcastEvent@0x0042f03f`, `CDSApp_BroadcastSyntheticEventToChildren@0x0042c7ab`. **Disasm:** `TEST [ECX+4], DX` / `MOV [EAX], ECX` / `CALL [EDX+0x10]` |

## Ghidra deltas

| Action | Detail |
|--------|--------|
| `rename_function_by_address` | `IDSEventHandler_DispatchIfMask` → `CDSView_DispatchSyntheticEventIfMasked` |
| `set_function_this_type` | `CDSView *` |
| `save_program` | batch save |

## Frida

**none**

## Remaining UNK

- Relationship to `IDSEventHandler` interface name vs. concrete `CDSView` child views (`child+0x10` in broadcast loop).
