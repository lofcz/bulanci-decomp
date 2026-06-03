# Round 7 — FUN Task 02 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 2 |
| **band** | sim |
| **seed_address** | `0x0042ecc0` |
| **prior_hint** | (none) |

## Status

**DONE** — Already correctly named in Ghidra; verified only.

## Function

| Address | Ghidra name (after) | Role | Evidence |
|---------|---------------------|------|----------|
| `0x0042ecc0` | `CStartGame2_EnqueueEvent_NullSafe` | `__cdecl` null-check wrapper → `CStartGame2_EnqueueEvent@0x0042ec40`; returns `AL=0` if `param_1==NULL` | **Xrefs_to (7):** `CScrollBar_SetValue`, `CEdit_SubmitText`, `CListViewer_SetItemSelected`, `CRadio_SetSelected`, `CColorSet_SetColorIndex`, `Button_Click` (×2). **Disasm:** `TEST ECX,ECX` / `CALL 0x0042ec40` |

## Ghidra deltas

**none** — symbol and body unchanged this session.

## Frida

**none**

## Remaining UNK

- Formal cdecl parameter types for `CStartGame2 *` first arg (Ghidra still shows `undefined param_1` on signature).
