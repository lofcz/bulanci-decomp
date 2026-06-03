# Round 7 — FUN Task 06 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 6 |
| **band** | sim |
| **seed_address** | `0x0042a550` |
| **prior_hint** | (none) |

## Status

**DONE** — Renamed; `CBulanci *` this typing applied.

## Function

| Address | Ghidra name (after) | Role | Evidence |
|---------|---------------------|------|----------|
| `0x0042a550` | `CBulanci_HandleDirtyRectBitBltHresult` | After `IDirectDrawSurface::BitBlt` HRESULT: if success return 0; on `0x887601C2` / `0x8876021C` call `CDSApp_RestoreLostSurfaces` then optional vtable+`0x24` invalidate | **Xrefs_to:** `CDSApp_FlushDirtyRects@0x0042bb5b`, `@0x0042bbbc`. **Disasm:** `CMP EAX,0x887601c2` / `CALL 0x00429bd0` |

## Ghidra deltas

| Action | Detail |
|--------|--------|
| `rename_function_by_address` | `CDSApp_HandleBitBltHresult` → `CBulanci_HandleDirtyRectBitBltHresult` (caller passes shell `CBulanci *`) |
| `set_function_this_type` | `CBulanci *` |
| `save_program` | batch save |

## Frida

**none**

## Remaining UNK

- Exact DDERR mnemonic mapping for `0x8876021C` (second compared code).
