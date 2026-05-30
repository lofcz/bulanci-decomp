# Round 4 — Task 14 report

## Task

| Field | Value |
|-------|--------|
| **id** | 14 |
| **title** | Retype CLevelScore helpers ECX; fix GetOrCreateLevelScore |
| **priority** | high (blocker) |
| **supersedes** | R3 todo 14 (struct `0x28` + `CListBoxItem.dwPad_04` — **DONE** in R3) |
| **addresses** | `0x00414280` / `0x004142b4`, `0x00409b10`, `0x00409620`, `0x00409330` |
| **acceptance** | `set_function_this_type` on each `__thiscall` site; decompile shows `CLevelScore::…(CLevelScore *this)` / `CGame::CGame_GetOrCreateLevelScore(CGame *this)` with `this->scoreChain` / `this->m_levelName` (not `CBulanci *` / `param_1 + 0x14` / `field_0x*`) |

**Types:** `CLevelScore`, `CGame`, `CScoreItem`, `CBulanci`

## Status

**DONE** — `set_function_this_type` + `__thiscall` prototype bridge + `force_decompile` verified; `save_program bulanci.exe`.

## Before / after

| Function | Address | Before (R3 residual) | After |
|----------|---------|----------------------|-------|
| `CLevelScore_dtor` | `0x00409330` | `__fastcall`, `param_1 + 5` chain dtor | `CLevelScore::CLevelScore_dtor(CLevelScore *this)` → `CDSChain_dtor(&this->scoreChain)`, `this->m_levelName` |
| `CLevelScore_InitializeDefaultScores` | `0x00409620` | `__fastcall`, `int param_1`, `(param_1 + 0x14)` | `…(CLevelScore *this)` → `CDSChained_AppendChild(&this->scoreChain, …)` |
| `CLevelScore_AddPlayerScore` | `0x00409b10` | `CBulanci *this`, `field_0x14` | `CLevelScore *this`, `&this->scoreChain`, `(this->scoreChain).dwChildCount` |
| `CGame_GetOrCreateLevelScore` | `0x00414280` | `CBulanci::…(CGame *param_1)`, `field_0x*` on game | `CGame::CGame_GetOrCreateLevelScore(CGame *this)`, `this->chain`, typed `pLevelScore->scoreChain` |

**Address note:** Todo seed `0x004142b4` is the in-function `PUSH 0x28` alloc site; function entry after R4 class move is **`0x00414280`**.

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Dtor `this` in ECX | `CLevelScore_dtor@0x00409330` | Asm `MOV ESI,ECX` @ `0x00409352`; `LEA ECX,[ESI+0x14]` → `CDSChain_dtor` |
| Default rows parent chain | `CLevelScore_InitializeDefaultScores@0x00409620` | Decompile `CDSChained_AppendChild(&this->scoreChain, puVar1)`; asm historically `param_1+0x14` |
| Live insert uses `scoreChain` | `CLevelScore_AddPlayerScore@0x00409b10` | `this_00 = &this->scoreChain`; prune loop on `dwChildCount` |
| Game allocator path | `CGame_GetOrCreateLevelScore@0x00414280` | `OperatorNewWithBadAlloc(0x28)` @ `0x004142b4`; `MOV ECX,ESI` @ `0x00414285` |
| `__thiscall` required | MCP | `CLevelScore_dtor` / `GetOrCreate` were `__fastcall`; `set_function_prototype` `void __thiscall …(void *this)` then `set_function_this_type` |

## Ghidra deltas

1. **`set_function_prototype`** `__thiscall` bridge (`void *this`) @ `0x00409330`, `0x00409620` where `CLevelScore *` failed direct parse (duplicate `/CLevelScore/CLevelScore` 1 B vs root 40 B type paths).
2. **`set_function_prototype`** + **`set_function_this_type`** `CGame *` @ `CGame_GetOrCreateLevelScore@0x00414280` (moved into class `CGame`).
3. **`set_function_this_type`** `CLevelScore *` @ `0x00409330`, `0x00409620`, `0x00409b10` (moved into class `CLevelScore`).
4. **`force_decompile`** on all four sites.
5. **`save_program bulanci.exe`**

## Verification snippet

```c
void __thiscall CLevelScore::CLevelScore_dtor(CLevelScore *this)
{
  CDSChain::CDSChain_dtor(&this->scoreChain);
  if (this->m_levelName != (undefined *)0x0) { … }
}

void __thiscall CGame::CGame_GetOrCreateLevelScore(CGame *this)
{
  …
  (((CLevelScore *)pLevelScore)->scoreChain).dwChildCount = 0;
  …
  CLevelScore::CLevelScore_AddPlayerScore((CLevelScore *)pLevelScore, …);
}
```

## Struct doc updates

- [CLevelScore.md](./CLevelScore.md) — R4 Ghidra apply block; `GetOrCreateLevelScore` entry `0x00414280`; UNK for `CScoreItem` local alias + `CGame_FindLevelScoreInChain` namespace.

## Remaining UNK

- `CLevelScore_AddPlayerScore`: new row heap object decompiler-typed as `CDSChain *` (should be `CScoreItem *` — R4 todo 18 overlap).
- Sort/prune callees `CBulanci::FUN_0042fae0` / `FUN_0042f8b0` / `CDSChain_RemoveListNode` — helper namespace, not `this` typing.
- `CGame_FindLevelScoreInChain` still `CBulanci::` in decompiler header.
- Nested 1 B `/CLevelScore/CLevelScore` duplicate type path (harmless after `void *` bridge; optional cleanup).
