# Round 5 — worker 44 report (struct_recovery H–M UNK)

## Task

| Field | Value |
|-------|-------|
| **worker** | 44 / 50 |
| **mode** | WRITE |
| **scope** | `struct_recovery` types **`C[H-M]*`** — second letter **H..M** (14 deliverables) |
| **acceptance** | Evidence-only field/function proofs; Ghidra apply where provable; update `{Type}.md`; `round5_worker_44_report.md` + jsonl; one `save_program` if mutated |

## Status

**DONE** (primary proofs on **CMina** / **CHelpView** / **CHistoryView** / **CMsgDialog**; remaining H–M types audited — no new xref-grade closures without overlapping other R5 workers)

## Inventory (H–M)

| Type | Status this pass | Notes |
|------|------------------|-------|
| `CHelpDlg` | audited | UNK cosmetic (`pM_` prefixes) only |
| `CHelpScript` | audited | `pSubObjStash` closed R4-12 |
| `CHelpView` | **proved** | `wViewFlags` bit **0x100** = `CWindow` preset delta vs history |
| `CHistoryDlg` | audited | cosmetic decompiler prefixes |
| `CHistoryScript` | audited | stash writers closed R4-12 |
| `CHistoryView` | **doc** | cross-link `0x100` view-flag bit |
| `CItemInfo` | audited | `dwReserved_08` still zero-only |
| `CLevelList` | audited | `CListViewer` embed — out of `CLevelList` scope |
| `CLevelScore` | audited | link-field / namespace follow-ups remain |
| `CListBoxItem` | audited | `dwPad_04` still no consumer |
| `CMenu` | audited | mid-band pads unchanged |
| `CMina` | **proved** | trace flag + danger-zone node path |
| `CMovieView` | audited | R4 closed `InitTrackSequence` |
| `CMsgDialog` | **closed** | chain `+0x48/+0x4a` ≡ `CDSChained` flags |

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| **`bTraceAreasActive`** | `CMina_UpdateTraceAreas@0x00419fd0` | Decompile: `(animBase).bTraceAreasActive != 0` → loop `0x80..0x7e` calling `CMina_CheckTraceAreasForSlot`; uses `(animBase.pGaming_host)->game` byte @ `CGaming+0xd8` |
| **`pDangerZoneNode`** | `CMina_RegisterDangerZone@0x0041c0d0` | `CGaming_AppendDangerZoneNode` return stored @ `updatedItem+0x1c` ≡ `CMina+0x10C`; asm `MOV [ESI+0x1c],EAX` @ `0x0041c128` |
| Danger-zone alloc | `CGaming_AppendDangerZoneNode@0x0041b420` | `OperatorNew(0x20)`; rect → `node+0x08`; append `CIntListInsertSortedOrAppend(this+0x2d8)`; tail-call `CMina_UpdateTraceAreas` |
| Gaming host ECX | `0x0041c10f` | `MOV ECX,[ESI+0x20]` before call — `CGaming **` from `updatedItem+0x20` (weapon path stores host in `nDeployOrOwnerCtx` band) |
| Release node | `CMina_ReleaseDangerZoneNode@0x0041b4a0` | Dtor path: `_free` after `CDynPtrArray_RemoveRange` on gaming `+0x2d8` list |
| **`wViewFlags` 0x100** | `CHelpView_ctor@0x004218a0` | Help `\|= 0x77f`; history `\|= 0x67f` @ `0x00422a70`; delta **0x100** matches `CWindow_BuildAt@0x00405560` (`0x77f` mask) |
| No `TEST` on 0x100 | program scan | `search_instructions` `test`/`and` on `[reg+0x14], 0x100` — no game-view hits (CRT only) |
| **Dialog `+0x48/+0x4a`** | `CDSChained_ResetChainCounters@0x0042beb0` | Sole init `=0`; aligns with `CDSChained.wChainFlag48/4a` ([CDSChained.md](./CDSChained.md)) |

## Ghidra deltas

- `rename_function_by_address` `0x0041b420` → **`CGaming_AppendDangerZoneNode`** (was `CBulanci_AppendZoneRect`)
- `set_function_this_type` `0x0041b420` → **`CGaming *`**
- `rename_function_by_address` `0x0041b4a0` → **`CMina_ReleaseDangerZoneNode`**
- `set_function_this_type` `0x0041b4a0` → **`CMina *`**
- `modify_struct_field` `CAnim.bTraceAreasActive` (was `dwChainField_0c`)
- `modify_struct_field` `CMina.pDangerZoneNode` (was `pAux`)
- `set_decompiler_comment` @ `0x00419fd0`, `0x004218eb`, `0x0041c122`
- `save_program bulanci.exe`

## Struct doc updates

- [CMina.md](./CMina.md) — trace flag, danger-zone node, R5 apply
- [CAnim.md](./CAnim.md) — `bTraceAreasActive` rename + CMina consumer
- [CHelpView.md](./CHelpView.md) — `wViewFlags` 0x100 preset note
- [CHistoryView.md](./CHistoryView.md) — sibling flag cross-ref
- [CMsgDialog.md](./CMsgDialog.md) — chain counter UNK closed via `CDSChained`

## Remaining UNK (H–M slice)

- COL display strings for classIds **0x81d** / **0x80f** (ids/factories proven).
- `CDangerZoneNode` field names inside `0x20` B node (alloc layout proven, names not).
- `CLevelList` / `CMenu` / `CLevelScore` interior pads — need dedicated widget/game passes.
- `CListBoxItem.dwPad_04` / `CItemInfo.dwReserved_08` — still no read xrefs.
