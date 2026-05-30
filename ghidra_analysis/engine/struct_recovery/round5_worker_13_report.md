# Round 5 — Worker 13/50 report

## Task

| Field | Value |
|-------|-------|
| **worker** | 13 / 50 |
| **title** | FUN_* CDSScript / help / history ~`0x00420000`–`0x00438000` |
| **mode** | WRITE (evidence-only renames) |
| **program** | `bulanci.exe` via user-ghidra-mcp |
| **address_range** | `0x00420000`–`0x00438000` |

## Status

**DONE** — Help/history/script cluster (`0x00421000`–`0x00423800`) has **zero** remaining `FUN_*` entry points (prior R3/R4 passes). **One** help/history-adjacent rename in the wider band: `CMenu_DetachChildWithVisibility@0x0042d160` (page-swap detach used by both dialogs). **64** other `FUN_*` in the band documented **SKIP** (audio, streams, CGaming HUD, image codecs — out of CDSScript/help/history scope). CDSScript VM core (`0x00438310`–`0x00438c40`) is **below** this VA window and was already fully named (0 `FUN_*` in `0x38300`–`0x38d00` scan).

## Evidence — inventory

### Help / history / script core (`0x00421000`–`0x00423800`)

| Claim | Proof |
|-------|--------|
| **0** `FUN_*` in core band | Ghidra `list_functions_enhanced` scan: no `FUN_*` with entry `0x421000 ≤ addr < 0x238000` |
| Extension opcodes named | Ghidra: `HelpBuildStaticTextAuto@0x00421940`, `HhBuildStaticTextAuto@0x00422b00`, `HhSetByteFieldAt0x18@0x00422810`, shared `HhPassFirstSubExpr` / `HhAddChildToParentView`, etc. |
| Dialogs / views named | `CHelpDlg_LoadHelpPage@0x00421c10`, `CHistoryDlg_LoadHistoryPage@0x00422f70`, `CHelpView_ctor`, `CHistoryView_ctor`, vtable slots (`CHelpDlg_GetClassTable`, `CHistoryDlg_ScalarDeletingDtor`, …) |
| CDSScript bind helper | `CDSScript_SetBoundParentView@0x00422620` (layout-compatible `CHelpScript` / `CHistoryScript` @ `+0x440`) |

### Renamed (help/history page flow)

| New name | Address | Evidence |
|----------|---------|----------|
| `CMenu_DetachChildWithVisibility` | `0x0042d160` | **Callers:** `CHelpDlg_LoadHelpPage@0x00421c61`, `CHistoryDlg_LoadHistoryPage@0x00422fc1` (also `CMenu_CloseCurrentSubScreen`, `CDSView_DoModal`, `CGame_StartGame`, …). **Body:** if `(child+0x44)&2` → `CDSView_ReleaseKeyboardFocus`; if `&1` → `Hide`; `CMenu_DetachChild@0x0042c000`; if was visible → `Show`. **Cast:** `(CMenu *)this` on dialog shell when swapping `pHelpView` / `pHistoryView`. |

### Post-rename decompile (page swap)

| Function | Address | Key line |
|----------|---------|----------|
| `CHelpDlg_LoadHelpPage` | `0x00421c10` | `CMenu::CMenu_DetachChildWithVisibility((CMenu *)this, (int *)this->pHelpView)` |
| `CHistoryDlg_LoadHistoryPage` | `0x00422f70` | Same pattern on `pHistoryView` |
| `CMenu_DetachChildWithVisibility` | `0x0042d160` | Typed `__thiscall CMenu *this, int *child` |

### CDSScript VM (out of band, closure note)

| Region | `FUN_*` count | Note |
|--------|---------------|------|
| `0x00438300`–`0x00438d00` | **0** | `CDSScript_InstallOpcodeTable`, `CDSScript_ReadSubExpr`, `CDSScript_Run`, `CDSScript_CallExport`, opcode handlers per [script_dispatch_table.md](../script_dispatch_table.md) |

## Evidence — SKIP (in band, not CDSScript/help/history)

Ghidra scan: **65** `FUN_*` at `0x420000 ≤ addr < 0x438000` before this worker; **64** after the single rename. None have xrefs from help/history symbols except `FUN_0042d160` (renamed). Representative SKIP buckets:

| Address band | Example `FUN_*` | Reason |
|--------------|-----------------|--------|
| `0x00427c30`–`0x00427c90` | HUD weapon/ammo refresh | CGaming in-match UI (R5 worker 12 deferred) |
| `0x00429880`–`0x00429bd0` | Audio player helpers | `CDSAudioPlayer` cluster (worker 14 blocked band overlap) |
| `0x0042a070`–`0x0042ecc0` | Scheduler / poem / panel / gun mouse | Generic UI/engine; no help/history xref |
| `0x0042f530`–`0x0042fdf0` | Chain splice / heap helpers | `CDSChain` engine (R5 task 40) |
| `0x00430a70`–`0x004370b0` | Streams, gzip, jpeg, font metrics | Resource I/O — not help script surface |

## Ghidra deltas

- `rename_function_by_address` `FUN_0042d160` → `CMenu_DetachChildWithVisibility`
- `set_decompiler_comment` @ `0x0042d160`, `0x00421c61`, `0x00422fc1`
- `force_decompile` @ `0x0042d160`, `0x00421c10`, `0x00422f70`
- `save_program bulanci.exe`

## Struct doc updates

- [CHelpDlg.md](./CHelpDlg.md) — R5 page-swap detach xref
- [CHistoryDlg.md](./CHistoryDlg.md) — R5 page-swap detach xref
- [CMenu.md](./CMenu.md) — `CMenu_DetachChildWithVisibility` in function table + Ghidra apply

## Remaining UNK

- **64** `FUN_*` remain in `0x00420000`–`0x00438000` outside help/history — assign to audio/stream/HUD workers (see SKIP table).
- `CHelpScript::CDSScript_SetBoundParentView` namespace on history page load — cosmetic (R5 manifest task 11).
- `IDSChainedTail_ClearSubObjStash` actual arg in help decompile still shows `&pCVar5[1].dwPc` — facet offset fix is R4 carry-over, not introduced here.
