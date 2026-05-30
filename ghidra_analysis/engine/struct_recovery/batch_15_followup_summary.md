# Struct recovery — batch 15 follow-up (round 2)

**Prior:** [batch_15_summary.md](./batch_15_summary.md)  
**Structs:** `CMsgDialog`, `CPauseDlg`  
**Status:** **FOLLOWUP_DONE**  
**Program:** `bulanci.exe` — `save_program` at end

## Follow-ups addressed (from batch 13 / per-struct UNK)

| Item | Result |
|------|--------|
| Recover `CDSChained` gap `0x40–0x4b` | **Done** — `CDSChained_ResetChainCounters@0x0042beb0` documents `dwChainHead_40`, `wViewStateFlags`, `wWidgetFlags`, `wChainCounter_48/4a`, `pParent`, `dwChainField_50`; applied to both struct `.md` files and Ghidra field renames |
| Name `pGame` as `CGame *` | **Done** — `CPauseDlg+0x70` is `CGame *`; `CPauseDlg_Build` prototype + plate comment (agent todo 14); decompiler ECX may still show `CBulanci*` |
| Heap `CPauseDlg` vs embedded | **Done** — gameplay path is `CGaming_ctor` heap `0x7c` + `CPauseDlg_Build`; stored at `CGaming+0x334`. `CPauseDlg_Allocate` is factory-only (DATA xref). Not embedded in `CBulanci` |

## Ghidra deltas

- `CMsgDialog` / `CPauseDlg`: renamed chain-band fields (`dwChainHead_40`, `wViewStateFlags`, `wWidgetFlags`, `wChainCounter_48`, …).
- `get_struct_layout CPauseDlg` → **124** bytes (`pGame`, `pBtnPrimary`, `pBtnSecondary` at `+0x70..+0x78`).
- `get_struct_layout CMsgDialog` → **112** bytes.
- Decompiler now uses `this->pGame` in `CPauseDlg_OnKeyDown@0x0040ac20`.

## Evidence added

- `CPauseDlg_OnNotify@0x0040b2f0` — `wViewStateFlags` bit 0 gates temporary `CDSView__Show`/`Hide` on dialog shell; child `+0x44` bit 2 for focus on primary/secondary buttons.
- `CPauseDlg_OnCommand` / `OnNotify` — `pParent+0x10` post target (`0x80cd`, `0xed`, etc.).

## Remaining UNK

- `wChainCounter_48` / `wChainCounter_4a` purpose (zero-init only).
- `CGame` typed member at `CPauseDlg+0x70`.
- RTTI class ids (`0x808` pause, `CMsgDialog` factory table).
- Ghidra `CPauseDlg.pParent` component offset **78** vs runtime **0x4c** (76) — 2-byte layout drift in DB; runtime xrefs use `+0x4c`.

## Per-struct notes

- [CMsgDialog.md](./CMsgDialog.md) — chain band `0x40–0x50` proven; still no tail past `CWindow`.
- [CPauseDlg.md](./CPauseDlg.md) — lifecycle table + chain band; tail pointers unchanged.
