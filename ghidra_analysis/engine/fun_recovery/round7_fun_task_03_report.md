# Round 7 — FUN Task 03 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 3 |
| **band** | sim |
| **seed_address** | `0x0042b3d0` |
| **prior_hint** | (none) |

## Status

**DONE** — Already renamed; verified callers and shutdown path.

## Function

| Address | Ghidra name (after) | Role | Evidence |
|---------|---------------------|------|----------|
| `0x0042b3d0` | `CDSApp_ShutdownFromScalarDtor` | Scalar-dtor prefix: restore four `CDSApp` vtable words; `CDsStringAssignFromHandle` on `+0x6C`; HKLM `RegWriteDword(Windowed)`; tail `FUN_0042b48c` | **Xrefs_to:** `CDSApp_DtorScalar@0x0042b983`, `CBulanci_DestroyEmbedFields@0x00402976`, SEH unwind stubs. **Not** full `CDSApp_dtor@0x42b560` |

## Ghidra deltas

**none** — prior round applied `CDSApp_ShutdownFromScalarDtor`.

## Frida

**none**

## Remaining UNK

- Public rename for tail callee `FUN_0042b48c` (full member teardown after registry write).
