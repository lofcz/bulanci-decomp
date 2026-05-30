# CHelpScript

## Status

**VERIFIED** — `sizeof == 0x444` (heap allocation + ctor/dtor span).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CHelpScript) == 0x444` | `0x004872f4` | `OperatorNewWithBadAlloc(0x444)` in `CHelpScript` factory before `CHelpScript::ctor@0x004215e0` |
| Last tail field at `+0x440` | `0x004215e0` | ctor zero; runtime bind `FUN_00422620`; consumer `HhAddChildToParentView@0x004217ed` |
| Subclass tail starts at `+0x430` | `0x004215e0` | ctor sets vftables at `+0x430`, `+0x438` after `CDSScript::ctor` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x000` | `0x430` | `CDSScript` | `script` | `CHelpScript_ctor@0x004215e0` → `CDSScript::ctor`; `InstallOpcodeTable(&this->script,0x2d,…)` |

`CHelpScript` tail (`+0x430` .. `+0x443`):

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x430` | 4 | `void*` | `vftable_IDSEventHandler` | `CHelpScript::ctor@0x004215e0` |
| `+0x434` | 4 | `int` | `nestedRefcount` | `CHelpScript::ctor` `=1`; `CLevelScript::CLevelScript_ReleaseNestedRefcount@0x004216??` (shared helper) |
| `+0x438` | 4 | `void*` | `vftable_IDSChained` | `CHelpScript::ctor`; `CHelpScript_dtor@0x004216f0` passes `this+0x438` to `FUN_00434250` |
| `+0x100` | `0x32c` | — | `pad_afterOpcodeTable` | `pOpcode[53]` ends at `0x2c+0xd4=0x100`; next proven field `pCallExportScratch` @ `+0x42c` |
| `+0x43c` | 4 | `void*` | `pSubObjStash` | `CHelpScript::ctor@0x004215e0` `=0`; cleared via `IDSChainedTail_ClearSubObjStash(&vf_IDSChained_tail)` on help page load (same facet layout as `CHistoryScript`) |
| `+0x440` | 4 | `void*` | `pBoundView` | ctor zero; `FUN_00422620@0x00422620` from `CHelpDlg_LoadHelpPage`; read `CHelpHistoryScript::HhAddChildToParentView@0x004217ed` |

## Ghidra apply

- `create_struct` `CHelpScript` size `0x444` (1092 B); `get_struct_layout` OK.
- Slice **10** (2026-05-30): `pad_afterOpcodeTable` @ `+0x100`; `field_0x43c`; `pBoundView` @ `+0x440`.
- Leaf: `CHelpHistoryScript::HhAddChildToParentView@0x004217ed`; bind helper renamed `_Globals::CDSScript_SetBoundParentView@0x00422620` (was `CDSScriptSubclass_BindBoundView` / `FUN_00422620`).
- Class id **2076** (`0x81c`); extension opcodes `45..52` via `g_apfnCHelpScriptOpcodeExt` @ `0x004af2fc` (`script_dispatch_table.md`).
- **Agent todo 18 (2026-05-30, r1+r2):** Embedded **`CDSScript script` @ 0** + MI tail `0x430..0x443`. `CHelpScript_ctor@0x004215e0` → `CDSScript::ctor(&this->script)`, `InstallOpcodeTable(&this->script,0x2d,…)`, `pBoundView` @ `+0x440`. Round 2: struct recreate + prototype restore after dialog `CWindow win` pass; `save_program`.
- **R3 todo 11 (2026-05-30):** `set_function_this_type` on `CHelpScript_ctor`, `CDSScript_SetBoundParentView@0x00422620`; embedded `CDSScript script` / `pBoundView` @ `+0x440`. *(No `CWindow` prefix — script object, not dialog.)*
- **R4 todo 11 (2026-05-30):** `HhAddChildToParentView@0x004217e0` → **`void * __cdecl …(CHelpScript *script)`** (stack script ctx, not __thiscall); decompile `ReadSubExpr(&script->script)` + `CDSView__AddChild(script->pBoundView, …)`. `CDSScript_SetBoundParentView` → `(CHelpScript *this, void *pBoundView)`. `pSubObjStash` @ `+0x43c`.
- **R5 worker 05 (2026-05-30):** `g_pCHelpScript_vftable_IDSChained` @ `0x004829f4` (`CHelpScript_ctor@0x0042164a`); `g_pCHelpScript_vftable_IDSEventHandler` @ `0x00482a0c`. Facet dtor thunk `CHelpScript_ScalarDeletingDtor_thunk_Sub438` @ `0x004216a0` (`SUB ECX,0x438` → `CHelpScript_ScalarDeletingDtor`). Plate on opcode 51 handler. [round5_worker_05_report.md](./round5_worker_05_report.md).

## UNK

- `+0x0c`, `+0x14`, `+0x1c`: zeroed in `CDSScript::ctor` only (see `CDSScript.md`).
- `pSubObjStash@+0x43c`: no non-zero writer on help path (ctor zero; page-load clear via `IDSChainedTail_ClearSubObjStash` on `+0x438` facet).
