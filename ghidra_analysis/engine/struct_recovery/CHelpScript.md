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
| `+0x43c` | 4 | `int` | `field_0x43c` | `CHelpScript::ctor@0x004215e0` `=0`; no reads in help/history script methods |
| `+0x440` | 4 | `void*` | `pBoundView` | ctor zero; `FUN_00422620@0x00422620` from `CHelpDlg_LoadHelpPage`; read `CHelpHistoryScript::HhAddChildToParentView@0x004217ed` |

## Ghidra apply

- `create_struct` `CHelpScript` size `0x444` (1092 B); `get_struct_layout` OK.
- Slice **10** (2026-05-30): `pad_afterOpcodeTable` @ `+0x100`; `field_0x43c`; `pBoundView` @ `+0x440`.
- Leaf: `CHelpHistoryScript::HhAddChildToParentView@0x004217ed`; bind helper renamed `_Globals::CDSScript_SetBoundParentView@0x00422620` (was `CDSScriptSubclass_BindBoundView` / `FUN_00422620`).
- Class id **2076** (`0x81c`); extension opcodes `45..52` via `g_apfnCHelpScriptOpcodeExt` @ `0x004af2fc` (`script_dispatch_table.md`).
- **Agent todo 18 (2026-05-30, r1+r2):** Embedded **`CDSScript script` @ 0** + MI tail `0x430..0x443`. `CHelpScript_ctor@0x004215e0` → `CDSScript::ctor(&this->script)`, `InstallOpcodeTable(&this->script,0x2d,…)`, `pBoundView` @ `+0x440`. Round 2: struct recreate + prototype restore after dialog `CWindow win` pass; `save_program`.
- **R3 todo 11 (2026-05-30):** `set_function_this_type` on `CHelpScript_ctor`, `CDSScript_SetBoundParentView@0x00422620`, `HhAddChildToParentView@0x004217e0` (`CHelpScript *`); `nestedRefcount` / `field_0x43c` / `pBoundView` field names. *(No `CWindow` prefix — script object, not dialog.)*

## UNK

- `+0x0c`, `+0x14`, `+0x1c`: zeroed in `CDSScript::ctor` only (see `CDSScript.md`).
- `+0x43c`: ctor zero only on help/history scripts (`CLevelScript` reuses offset for MI adjust — not proven here).
