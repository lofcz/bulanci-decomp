# Round 5 worker 05 — `.rdata` help/history shell vtables (`0x482800`..`0x483200`)

## Task

| Field | Value |
|-------|-------|
| **Worker** | 5 / 50 |
| **Mode** | WRITE |
| **Address slice** | `0x00482800` .. `0x00483200` (band `0x00480000`..`0x004A0000`, 50-way split, stride `0xA00`) |
| **Focus** | App-shell / menu-adjacent hooks: `CHelpScript`, `CHelpView`, `CHelpDlg`, start of `CHistoryScript` vtables; tail of `.rdata` tables (`gAWeaponDefsTable`, speed-formula floats, script opcode metadata) |
| **Scope rule** | Evidence from disasm (MI adjustor thunks), ctor `MOV [ESI+off],imm32` vtable installs, and menu page-load call chains |

## Status

**DONE** — Slice contained **zero** remaining `FUN_*` code symbols (prior rounds already renamed help/history methods). Applied **2** MI thunk disambiguations, **5** vtable global labels, **5** disasm/plate comments; `save_program bulanci.exe`.

## Slice map

| Range | Content |
|-------|---------|
| `0x00482800`..`0x004829f3` | Static data: `gAWeaponDefsTable` rows @ `0x00482820`, wchar UI strings, script opcode dispatch tables, `g_kSpeedFormulaNumerator_47p25` / `g_kSpeedFormulaNeutral_100p0` @ `0x00482908`/`0x00482910` (documented in `anim_runtime.md`) |
| `0x004829f4`..`0x00483087` | **`CHelpScript`** + **`CHelpView`** + **`CHelpDlg`** vtable clusters |
| `0x00483088`..`0x004831ff` | **`CHistoryScript`** MI tail + primary faces (partial; primary continues past slice end) |

## Vtable evidence (help / history menu shell)

### `CHelpScript` @ `g_pCHelpScript_vftable_IDSChained` (`0x004829f4`)

| Slot | Address | Symbol | Proof |
|------|---------|--------|-------|
| 0 | `0x00409470` | `GetResourceName` | Standard CDS resource facet |
| 1 | `0x00422770` | `CDSChain_AdjustThisOffset_ThisMinus434` | Shared script-chain adjustor (`this-0x1B2` / tail facet) |
| 2 | `0x00421810` | `CLevelScript_ReleaseChild` | Shared `CDSScript` child release |
| 3 | `0x004216a0` | `CHelpScript_ScalarDeletingDtor_thunk_Sub438` | **Disasm:** `SUB ECX,0x438`; `JMP CHelpScript_ScalarDeletingDtor` — MI facet @ `this+0x438` |
| 4 | `0x00409480` | `AlwaysReturnsZero` | Catalog default |

**Ctor install:** `CHelpScript_ctor@0x0042164a` DATA xref → `0x004829f4`.

**Menu hook:** `CHelpDlg_LoadHelpPage@0x00421c10` loads `classId==0x81c` script, `CDSScript_SetBoundParentView`, `CDSScript::CallExport(0,1,&pageIndex)`, `CDSView__AddChild`.

### `CHelpScript` opcode 51 (shared with history)

| Address | Symbol | Proof |
|---------|--------|-------|
| `0x004217e0` | `CHelpScript::HhAddChildToParentView` | **Decompile:** `ReadSubExpr(&script->script)`; `CDSView__AddChild(script->pBoundView, child, 0)`; plate comment added. Installed in `g_apfnCHelpScriptOpcodeExt` / history twin (`CHelpScript.md`, `CHistoryScript.md`). |

### `CHelpView` primary @ `g_pCHelpView_vftable_primary` (`0x00482a94`)

| Slot | Address | Symbol | Proof |
|------|---------|--------|-------|
| 0 | `0x00421900` | `CHelpView_GetClassTable` | Returns class table ptr |
| 1 | `0x00402a90` | `CDSView_DtorScalar` | Shared view scalar dtor |
| 2 | `0x00402fff0` | `CDSObject_ReleaseViaVtable` | Engine release |
| 3..5 | `0x0042c0*` | `CDSView_GetDataSize` / `SaveData` / `LoadData` | Standard `CDSView` primary |

**Page factory:** `CHelpDlg_LoadHelpPage` → `OperatorNew(0x68)` + `CHelpView_ctor@0x004218a0`.

### `CHelpDlg` primary @ `g_pCHelpDlg_vftable_primary` (`0x00482b84`)

| Slot | Address | Symbol | Proof |
|------|---------|--------|-------|
| 0 | `0x004220a0` | `CHelpDlg_GetClassTable` | |
| 1 | `0x004222f0` | `CHelpDlg_ScalarDeletingDtor` | **Disasm callers:** thunks @ `+0x4`, `+0x10`, `+0x18` (below) |
| 2 | `0x00402fff0` | `CDSObject_ReleaseViaVtable` | |

**MI deleting thunks (disasm proof):**

| Thunk @ | Adjustor | New name (R5) |
|---------|----------|---------------|
| `0x004220c0` | `SUB ECX,4` | `CHelpDlg_AdjustorThunk04_Dtor` (unchanged) |
| `0x004220d0` | `SUB ECX,0x10` | `CHelpDlg_ScalarDeletingDtor_thunk_Sub10` |
| `0x004220b0` | `SUB ECX,0x18` | `CHelpDlg_ScalarDeletingDtor_thunk_Sub18` |

**Ctor xref:** `CHelpDlgCtor@0x00421e9c` → `0x00482b84`; dtor `CHelpDlg_dtor@0x00422108`.

### `CHistoryScript` tail facet @ `g_pCHistoryScript_vftable_IDSChained_tail` (`0x00483088`)

| Slot | Address | Symbol | Proof |
|------|---------|--------|-------|
| 3 | `0x00422790` | `CHistoryScript_ScalarDeletingDtor_thunk_Sub438` | **Disasm:** `SUB ECX,0x438`; `JMP CHistoryScript_ScalarDeletingDtor@0x00422ae0` |
| 0..2 | (same as help script) | `GetResourceName` / chain adjustor / `CLevelScript_ReleaseChild` | Parallel layout to `CHelpScript` (`CHistoryScript.md`) |

**Menu path:** `CHistoryDlg_LoadHistoryPage@0x00422f70` mirrors help page load (script `0x81d`, `pBoundView@+0x440`).

## FUN_* inventory (code targets referenced from slice)

Ghidra `search_functions FUN_00421*` / `FUN_00422*` → **no matches**. All code pointers in this `.rdata` band already carry semantic names from R3/R4 struct-recovery passes.

| Representative hook | Address | Role |
|---------------------|---------|------|
| `CHelpDlg_LoadHelpPage` | `0x00421c10` | Help menu page swap: tear down prior `CHelpView`/`CDSScript`, bind new script + view |
| `CHelpDlg_ctor` | `0x00421e40` | Enumerates menu resources `classId==0x81c`, fills `m_pageIds` |
| `CHelpScript_ctor` | `0x004215e0` | Installs vtables including `0x004829f4` |
| `CLevelScript_Serialize` | `0x00438e40` | Shared script IDSChained slot 4 (`CHelpScript` / `CHistoryScript` 6-face tables) |

## Ghidra deltas

| Action | Target |
|--------|--------|
| `rename_function_by_address` | `CHelpDlg_ScalarDeletingDtor_thunk` → `…_thunk_Sub18` @ `0x004220b0`; `…_thunk_Sub10` @ `0x004220d0` |
| `rename_or_label` | `g_pCHelpScript_vftable_IDSChained`, `g_pCHelpScript_vftable_IDSEventHandler`, `g_pCHelpView_vftable_primary`, `g_pCHelpView_vftable_IDSEventHandler`, `g_pCHelpDlg_vftable_primary`, `g_pCHelpDlg_vftable_IDSEventHandler` |
| `set_disassembly_comment` | MI thunks @ `0x004216a0`, `0x004220b0`, `0x004220c0`, `0x004220d0`, `0x00422790` |
| `set_plate_comment` | `CHelpScript::HhAddChildToParentView@0x004217e0` |
| `save_program` | `bulanci.exe` |

## Struct doc updates

- [CHelpDlg.md](./CHelpDlg.md) — R5 w05 vtable globals + MI thunk names
- [CHelpView.md](./CHelpView.md) — `g_pCHelpView_vftable_*` labels
- [CHelpScript.md](./CHelpScript.md) — `g_pCHelpScript_vftable_*`; opcode-51 plate
- [CHistoryScript.md](./CHistoryScript.md) — `g_pCHistoryScript_vftable_IDSChained_tail` (slice start)

## Remaining UNK (this slice)

- `CHelpDlg_LoadHelpPage` still calls `CMenu::FUN_0042d160` for view detach — callee **outside** slice (`0x0042d160`); belongs to menu-tree worker band
- `CHelpDlg` / `CHelpView` secondary MI tables (`0x00482a60`, `0x00482b4c`, …) not individually labeled (only primary + script faces in this pass)
- Decompiler `_Globals::` prefix on `CDSView__AddChild` / `OperatorNew` — cosmetic namespace, not slice hooks
