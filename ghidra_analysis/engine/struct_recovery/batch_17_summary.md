# Struct recovery batch 17/50

**Index:** 17 (`batches_50.json`)  
**Types:** `CScore`, `CScoreItem`  
**Date:** 2026-05-30

## Results

| Struct | Status | Size | Notes |
|--------|--------|------|-------|
| `CScoreItem` | VERIFIED | `0x1c` | Chained high-score row: name `+0x10`, kills/deaths `+0x14`/`+0x18`; serialize via IDSChained `this` |
| `CScore` | PARTIAL | `0x74` | Post-match `CWindow` modal; MI vtables `+0/+4/+10/+18`; match-end audio `+0x70` |

## Key evidence

- **CScoreItem alloc:** `OperatorNewWithBadAlloc(0x1c)` in `CLevelScore_InitializeDefaultScores` (`0x00409620`) and `CLevelScore_AddPlayerScore` (`0x00409b10`).
- **CScoreItem fields:** `CScoreItem::Serialize`/`Deserialize` (`0x00408f50`/`0x00408f90`); `CScoreItem_CompareByNetScore` (`0x00408fd0`); `CScoreItem::Destructor` releases `+0x10` string.
- **CScore alloc:** `CScore::Create` (`0x0040efd0`) → `OperatorNewWithBadAlloc(0x74)`.
- **CScore ctor:** `CScore_ctor` (`0x00411010`) — `CWindow_BuildAt(800×600)`; ranks players; optional SP high-score branch; stores audio @ `+0x70`.
- **CScore dtor:** `CScore::Destructor` (`0x0040e120`) — stops/releases `+0x70`.
- **Dismiss:** `CScore::OnKeyPress` (`0x0040abe0`) — Esc/Enter → `EndModal(0x8003)`.

## Artifacts

- `ghidra_analysis/engine/struct_recovery/CScore.md`
- `ghidra_analysis/engine/struct_recovery/CScoreItem.md`
- Ghidra: `CScoreItem` 28 B (named fields); `CScore` 116 B (MI vtables + `byte[80]` window body + tail); renames `CScore_ctor`, `CScore_RenderHighScoreRow`, `CScoreItem_MatchesKillsDeathsAndName`; `save_program bulanci.exe`

## Follow-ups

- Expand `CScore.base_CWindow_MI` when global `CWindow` recovery catches up (mirror `CExitDlg` bbox/parent fields).
- ~~`CScoreItem_MatchesKillsDeathsAndName` wrong `this`~~ — **resolved** r4 todo 18: `__thiscall` `CScoreItem*` template in `ECX`, `pRow` on stack (`0x004119d5`).
- `m_deaths` omitted from `Serialize` — confirm network vs disk schema.
