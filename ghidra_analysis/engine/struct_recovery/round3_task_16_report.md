# Round 3 — task 16 report

## Task

| Field | Value |
|-------|--------|
| **id** | 16 |
| **title** | Fix CPauseDlg_Build decompiler ECX opaque this |
| **priority** | high (blocker) |
| **addresses** | `0x00411df0`, `0x00420423`, `0x00420380`, `0x0040ac20` |
| **acceptance** | Decompiler shows `CPauseDlg::CPauseDlg_Build(CPauseDlg *this, …)` with typed `this->` field access (not `CBulanci *this` / `field_0x*`) |

**Structs:** `CPauseDlg`, `CGaming`, `CGame`

## Status

**DONE** — `set_function_this_type` (not plate-only) + `force_decompile` verified.

## Before / after

| | Signature (decompiler header) | Body `this` access |
|---|------------------------------|-------------------|
| **Before** | `CBulanci::CPauseDlg_Build(CBulanci *this, CGame *pGame)` | `field_0x4`, `field_0x70`, `field_0x74`, … |
| **After** | `CPauseDlg::CPauseDlg_Build(CPauseDlg *this, CGame *pGame)` | `pVftable_primary`, `pBtnPrimary`, `nBbox_bottom`, `wWidgetFlags`, … |

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| `this` is heap `CPauseDlg` | `CPauseDlg_Build@0x00411df0` | `MOV ESI,ECX` @ `0x00411e17`; `MOV [ESI+0x70], pGame` @ `0x00411e6b` |
| Sole caller | `CGaming_ctor@0x00420423` | `OperatorNew(0x7c)` → `CPauseDlg_Build(pause, owner)`; stored `CGaming+0x334` |
| Not `CBulanci` subobject | lifecycle | Pause dlg is heap sibling of `CGaming`, not embedded in `CBulanci` |
| Siblings already typed | `CPauseDlg_OnKeyDown@0x0040ac20` | `CPauseDlg *` + `this->pGame` |

## Ghidra deltas

1. **`set_function_this_type`** `CPauseDlg_Build@0x00411df0` → **`CPauseDlg *`** — moved function into class namespace `CPauseDlg`; auto-storage ECX derives from class struct.
2. **`set_function_prototype`** → `CPauseDlg * __thiscall CPauseDlg_Build(CPauseDlg *this, CGame *pGame)`.
3. **`force_decompile`** @ `0x00411df0` — cache refresh.
4. **`save_program bulanci.exe`**

Prior rounds used `set_function_prototype` + plate/disasm comments only; that left **`CBulanci *this`** because `__thiscall` ECX is an immutable auto-parameter tied to the parent class namespace.

## Verification snippet

```c
CPauseDlg * __thiscall CPauseDlg::CPauseDlg_Build(CPauseDlg *this, CGame *pGame)
{
  ...
  this->pVftable_primary = g_pCPauseDlg_vftable_primary;
  this->field32_0x70 = pGame;   /* +0x70 — name may lag; type CGame* */
  ...
  this->pBtnPrimary = pCVar1;
  this->pBtnSecondary = pCVar1;
  ...
  return this;
}
```

## Struct doc updates

- [CPauseDlg.md](./CPauseDlg.md) — Ghidra apply block + R3 task 16 note; removed ECX-opaque UNK.

## Remaining UNK

- `this->field32_0x70` vs `this->pGame` component label (offset/type correct; optional `rename` on struct component).
- `CBulanci::CDsStringAssignFromLiteral` on stack temporaries inside `Build` (unrelated static helper namespace).
