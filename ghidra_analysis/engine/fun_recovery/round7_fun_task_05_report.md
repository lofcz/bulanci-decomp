# Round 7 — FUN Task 05 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 5 |
| **band** | sim |
| **seed_address** | `0x00429bd0` |
| **prior_hint** | (none) |

## Status

**DONE** — Renamed and typed as `CDSApp` member restore helper.

## Function

| Address | Ghidra name (after) | Role | Evidence |
|---------|---------------------|------|----------|
| `0x00429bd0` | `CDSApp_RestoreLostSurfaces` | `this+0x78` COM vtable+`0x6C` restore; on success `FUN_004298b0(this+0x7C)` back-buffer rebind | **Xrefs_to:** `CBulanci_HandleDirtyRectBitBltHresult@0x0042a56f`, `FUN_0042a590@0x0042a5a4`. **Disasm:** `MOV EAX,[ESI+0x78]` / `CALL [ECX+0x6C]` / `LEA ECX,[ESI+0x7C]` |

## Ghidra deltas

| Action | Detail |
|--------|--------|
| `rename_function_by_address` | `FUN_00429bd0` → `CDSApp_RestoreLostSurfaces` |
| `set_function_prototype` | `bool CDSApp_RestoreLostSurfaces(void)` / `__thiscall` |
| `set_function_this_type` | `CDSApp *` |
| `save_program` | batch save |

## Frida

**none**

## Remaining UNK

- HRESULT semantics of vtable+`0x6C` callee (restore vs. query) without COM interface label in binary.
