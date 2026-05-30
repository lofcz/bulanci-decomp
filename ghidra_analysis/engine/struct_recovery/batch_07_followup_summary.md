# Struct recovery batch 7/50 — follow-up round 2

**Prior:** [`batch_07_summary.md`](batch_07_summary.md)  
**Structs:** `CExitDlg`, `CExplosion`  
**Status:** **FOLLOWUP_PARTIAL** (tail UNKs remain; all explicit batch-7 follow-ups addressed)

## Follow-up items (from batch 7)

| Item | Result |
|------|--------|
| Recover `CWindow` / `CDSChained` base for `CExitDlg` | **Done (docs)** — layout aligned with `CMsgDialog` / `CPauseDlg` (`CDSChained_ctor@0x004032d0`, `CWindow_BuildAt@0x00405560`). See [`CExitDlg.md`](CExitDlg.md). |
| Expand `CExplosion` with batch-3 `CAnim` | **Done** — Ghidra `CExplosion` size `0xF4`; `+0x9C..+0xEF` named from `CAnim_recovered` / `CAnim.md`. See [`CExplosion.md`](CExplosion.md). |
| Find writer for `CExitDlg+0x70` | **Closed (negative)** — no non-zero store in any `CExitDlg_*` symbol; ctor only `MOV [esi+0x70/0x74/0x78], 0`. Sibling dialogs (`CPauseDlg_Build`, `CSessionList_BuildDialog`) do populate `+0x70..+0x78`; `CExitDlg` uses heap children only. Renamed `+0x70` → `pLinkedAnim` (optional close-anim hook; never wired). |

## Ghidra MCP actions

1. Decompiled `CExitDlg_ctor`, `CExitDlg_RouteSyntheticCloseEvent`, `CWindow_BuildAt`, `CExplosion_Ctor`, `ApplyAreaDamage`, `DamageAtPoint`, `CMenu_CmdDispatch`.
2. Instruction search: `MOV [reg+0x70]` program-wide; scoped `CExitDlg_ctor` (zero stores only).
3. **`CExplosion` struct** — replaced `pPad_9c` with `nAnim_inner_field_9c/a0/a4`, `track_manager[72]`, `bOwnerSlot@0xF0`, `pPad_f1`; `get_struct_layout` → **244 bytes**.
4. **`save_program bulanci.exe`** (once).

## Remaining UNK

- `CExitDlg`: `pLinkedAnim` / `reserved_74` / `reserved_78` purpose (layout slot unused in this build); `0x40–0x4B` chain padding.
- `CExplosion`: `+0x28..+0x67` header; `track_manager` sub-fields; blast helper signatures (`CGaming*` first arg).

## Deliverables touched

- [`CExitDlg.md`](CExitDlg.md) — full dialog base + tail evidence
- [`CExplosion.md`](CExplosion.md) — `CAnim` tail fields through `+0xEF`
- This summary
