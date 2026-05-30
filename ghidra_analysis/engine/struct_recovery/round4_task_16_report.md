# Round 4 — Task 16 report

## Task

| Field | Value |
|-------|--------|
| **id** | 16 |
| **title** | Rename CPauseDlg pGame component at +0x70 |
| **priority** | low (handoff) |
| **supersedes** | R3 todo 16 (`set_function_this_type` ECX — **DONE** in R3) |
| **addresses** | `0x00411df0`, `0x00420423`, `0x00420380` |
| **acceptance** | `get_struct_layout CPauseDlg` shows **`pGame`** @ `+0x70` (`CGame *`); `CPauseDlg_Build` decompile uses `this->pGame` (not `field32_0x70` / unnamed) |

**Types:** `CPauseDlg`, `CGame`, `CMsgDialog`, `CGaming`

## Status

**DONE** — struct component renamed; decompiler verified after `force_decompile`.

## Before / after

| | Struct layout @ +0x70 | `CPauseDlg_Build` body |
|---|----------------------|-------------------------|
| **Before (R4 entry)** | `CGame *` **(unnamed)** | `this->field32_0x70 = pGame`; `this->field32_0x70->pRecvBuf` |
| **After** | `CGame *` **`pGame`** | `this->pGame = pGame`; `this->pGame->pRecvBuf` |

R3 had already fixed **`CPauseDlg::CPauseDlg_Build(CPauseDlg *this, CGame *pGame)`** via `set_function_this_type`; R4 closed the remaining decompiler field label gap.

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Store `pGame` @ `+0x70` | `CPauseDlg_Build@0x00411df0` | Asm `MOV [ESI+0x70], …` @ `0x00411e6b`; decompile `this->pGame = pGame` |
| Lobby chat widens bbox | `CPauseDlg_Build@0x00411df0` | `if (this->pGame->pRecvBuf != NULL)` → `nBbox_bottom += 200` |
| Consumer uses `pGame` | `CPauseDlg_OnKeyDown@0x0040ac20` | `CGame_NetSendKick_t0a(this->pGame, 0)` |
| Heap path | `CGaming_ctor@0x00420380` | `OperatorNew(0x7c)` → `CPauseDlg_Build` @ `0x00420423` |

## Ghidra deltas

1. **`modify_struct_field`** `CPauseDlg` **`offset:0x70`** → **`new_name: pGame`**, **`new_type: CGame *`**
2. **`force_decompile`** @ `0x00411df0`
3. **`set_decompiler_comment`** @ `0x00411e6b` — store to `CPauseDlg::pGame`
4. **`save_program bulanci.exe`**

## Struct doc updates

- [CPauseDlg.md](./CPauseDlg.md) — R4 task 16 apply block; removed UNK for `field32_0x70` label lag

## Remaining UNK

- `CBulanci::CDsStringAssignFromLiteral` on stack temporaries inside `Build` (static helper namespace; unrelated to `pGame` offset)
- `wChainCounter_48` / `wChainCounter_4a` semantics (no `CPauseDlg`-local readers)
- Whether overlay calls `InitializeByClassId(2056)` vs gameplay `CPauseDlg_Build` path
