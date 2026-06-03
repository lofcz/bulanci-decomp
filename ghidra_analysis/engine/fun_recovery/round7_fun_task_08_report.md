# Round 7 — FUN Task 08 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 8 |
| **band** | sim |
| **seed_address** | `0x0042c1c0` |
| **prior_hint** | (none) |

## Status

**DONE** — Already renamed; disasm + xref + decompile re-verified in Ghidra MCP.

## Function

| Address | Ghidra name (after) | Role | Evidence |
|---------|---------------------|------|----------|
| `0x0042c1c0` | `CDSView_ResolveInputChainFromHitTest` | `__thiscall` walker: if modal global `DAT_004b3b94` set → return it; else walk intrusive list at `this+0x54` for node with `(byte[+0x44] & 0x84) == 0x80` and primary vfn `+0x1C(param_1)` true; advance via parent `+0x4C` sibling links; return hit node or original `this` | **Xrefs_to (2):** `CDSApp_RefreshInputChainHitTest@0x0042c71c`, `CDSView_UpdateInputChainOnMouseMove@0x0042c8cd` |

### Disasm proof (Ghidra live)

| Offset | Instruction | Meaning |
|--------|-------------|---------|
| `+0x00` | `MOV EAX,[0x004b3b94]` / `TEST` / `JNZ ret` | Early-out modal-focus global |
| `+0x12` | `LEA ECX,[EDI+0x54]` / `CALL 0x0042f7d0` | First child from chain head @ `this+0x54` |
| `+0x24` | `MOV AL,[ESI+0x44]` / `AND AL,0x84` / `CMP AL,0x80` | Active-chain flag test (`flags1` bit `0x80`) |
| `+0x2D` | `CALL [EDX+0x1c]` with `ECX=ESI` | Primary vtable hit-test slot |
| `+0x3B` | `LEA ECX,[EAX+0x54]` / `CALL 0x0042f7e0` | Sibling advance via parent list |
| `+0x51` | `MOV EAX,EDI` | No hit → return starting `this` |

### Caller closure

- `CDSApp_RefreshInputChainHitTest@0x0042c700`: when `g_pInputChainHead == 0`, calls `CDSView_ResolveInputChainFromHitTest(g_pApp, g_pApp+0xf0)` then walks result for `+0x46 & 0x10` + vfn `+0x18` hit.
- `CDSView_UpdateInputChainOnMouseMove@0x0042c880`: mouse-move path uses same resolver before `CDSApp_SetInputChainHead`.

Pairs with input-chain helpers `CDSView_InputChain_OnAttach@0x0042c120` (task 20) and `CDSView_InputChain_OnDetach@0x0042c140` (task 07).

## Ghidra deltas

**none this session** — R6/R7 prior rename held (`CDSView *` this typing present).

## Frida

**none**

## Remaining UNK

- Published export name for primary vfn `+0x1C` (hit-test predicate taking app key-state pointer).
- Whether walk start `this` is always `g_pApp` root vs. arbitrary chain node (callers pass app/key-state context).
