# Round 4 — task 11 report

## Task

| Field | Value |
|-------|-------|
| **id** | 11 |
| **title** | Fix HhAddChildToParentView stack; name CDSScript pad band |
| **source** | blocker |
| **supersedes_todo_id** | 11 (R3) |
| **manifest** | `todos_gather_r4_1.json` id 11 |
| **types** | `CHelpScript`, `CHistoryScript`, `CDSScript`, `CDSView` |
| **addresses** | `0x004217e0`, `0x00422620`, `0x004215e0`, `0x0042302c` |

## Status

**DONE** — Opcode-51 handler retyped **cdecl** with stack `CHelpScript *` (not __thiscall); decompile shows `ReadSubExpr(&script->script)` and `CDSView__AddChild(script->pBoundView, child, 0)`. `CDSScript_SetBoundParentView` fixed to two-parameter __thiscall. `CHistoryDlg_LoadHistoryPage` stash clear uses `&pCVar5->vf_IDSChained_tail`. `CDSScript` pad fields verified named in Ghidra.

## Evidence

### Root cause: wrong calling convention (R3 residual)

| Claim | Address | Evidence |
|-------|---------|----------|
| Script ctx on **stack**, not ECX | `HhAddChildToParentView@0x004217e0` | Asm: `MOV EDI,[ESP+0xc]` after callee-saves; `MOV ECX,EDI` only for `ReadSubExpr` fastcall |
| Sibling handlers use **cdecl** `param_1` | `HhPassFirstSubExpr@0x004215c0`, `HhSetStaticTextStyleAttrs@0x00421750` | Decompile `CDSScript::ReadSubExpr(param_1)` — same dispatch convention |
| R3 __thiscall mis-typed stack slot | `0x004217e0` (pre-R4) | Decompile used `in_stack_00000004` for both `ReadSubExpr` and `+0x440` parent |

### Post-R4 decompile (opcode 51)

| Function | Address | Signature / body |
|----------|---------|------------------|
| `HhAddChildToParentView` | `0x004217e0` | `void * __cdecl HhAddChildToParentView(CHelpScript *script)` → `ReadSubExpr(&script->script)`; `CDSView__AddChild(script->pBoundView, child, 0)` |
| `CDSScript_SetBoundParentView` | `0x00422620` | `void __thiscall CDSScript_SetBoundParentView(CHelpScript *this, void *pBoundView)` → `this->pBoundView = pBoundView` (asm: `[ECX+0x440]=[ESP+4]`, `RET 4`) |
| `ReadSubExpr` | `0x004384c0` | `void __fastcall CDSScript::ReadSubExpr(CDSScript *script)` |
| `CHistoryDlg_LoadHistoryPage` | `0x00422f70` | `pCVar5` typed `CHistoryScript *`; `IDSChainedTail_ClearSubObjStash((int)&pCVar5->vf_IDSChained_tail)` @ `0x0042302c` |

### CDSScript pad band (Ghidra layout)

| Offset | Ghidra name | Evidence |
|--------|-------------|----------|
| `+0x0c` | `dwReserved_0c` | `get_struct_layout CDSScript` — ctor-zero only ([CDSScript.md](./CDSScript.md)) |
| `+0x14` | `dwReserved_14` | same |
| `+0x25` | `pPad_after_bReturnFlag` | 3 B after `bReturnFlag` |
| `+0xe0` | `pPad_afterOpcodeTable` | 844 B before `framePtr@+0x42c` |

### CHelpScript tail parity

| Offset | Name | Evidence |
|--------|------|----------|
| `+0x43c` | `pSubObjStash` (`void *`) | Renamed from `nField_0x43c`; aligns with `CHistoryScript` MI stash slot ([CHistoryScript.md](./CHistoryScript.md)) |

## Ghidra deltas

- `set_function_prototype` `HhAddChildToParentView@0x004217e0` → **`void * __cdecl …(CHelpScript *script)`** (replaces incorrect __thiscall)
- `set_function_prototype` `CDSScript_SetBoundParentView@0x00422620` → **`void __thiscall …(CHelpScript *this, void *pBoundView)`** (drops bogus second stack param)
- `set_function_this_type` `CHelpScript *` @ `0x00422620`, `0x004215e0`
- `set_function_prototype` `CDSScript::ReadSubExpr(CDSScript *script)` @ `0x004384c0`
- `set_decompiler_variable_type` `pCVar5` → `CHistoryScript *` in `CHistoryDlg_LoadHistoryPage@0x00422f70`
- `modify_struct_field` `CHelpScript.pSubObjStash` @ `+0x43c`
- `set_decompiler_comment` @ `0x004217e8`, `0x00422624`, `0x0042302c`
- `force_decompile` @ `0x004217e0`, `0x00422620`, `0x00422f70`
- `save_program bulanci.exe`

## Struct doc updates

- [CHelpScript.md](./CHelpScript.md) — R4 calling-convention fix; `pSubObjStash` @ `+0x43c`
- [CDSScript.md](./CDSScript.md) — R4 pad-band Ghidra apply note
- [CHistoryScript.md](./CHistoryScript.md) — cross-link shared opcode-51 handler convention

## Remaining UNK

- `HhAddChildToParentView` remains under `CHelpScript::` namespace though shared with `CHistoryScript` (same as other `CHelpHistoryScript` opcode helpers — cosmetic).
- `LoadHistoryPage` still casts `pActiveScript` to `CHelpScript *` for `CDSScript_SetBoundParentView` (layout-compatible; `CHistoryScript` tail matches).
- `CDSScript+0x18 scriptData` — no typed writer in base methods (unchanged).
- Ghidra **component prefix** on dialog `CIntList` members (`m_pageIds.*`) — cosmetic (R3 carry-over).
