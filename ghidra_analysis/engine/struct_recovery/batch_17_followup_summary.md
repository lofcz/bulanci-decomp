# Struct recovery batch 17 — follow-up round 2

**Index:** 17 (`batches_50.json`)  
**Prior:** `batch_17_summary.md`  
**Date:** 2026-05-30  
**Status:** **HANDOFF_PARTIAL**

## Follow-ups addressed

| Prior item | Result |
|------------|--------|
| Name `field_08` / `field_0c` on `CScoreItem` | **Done** — `m_link_next` / `m_link_prev` via `FUN_0042f820` / `FUN_0042f780` (intrusive list; same offsets as `CDSSafeStreamInfo`) |
| Confirm `field_6c` on `CScore` | **Done** — not CScore-specific padding; renamed **`pDefaultFocusChild`** (`CWindow_BuildAt@0x00405560`, `CScore::Create` `puVar1[0x1b]=0`) |
| Expand `CScore` `base_CWindow_MI` | **Done** (agent todo 17) — Ghidra `CScore` field-split through `+0x6b` (`nBbox_*`, `wViewFlags`, `pParent`, `bModalFlag`, …); `get_struct_layout` → 116 B |

## New evidence (round 2)

- **`CScoreItem::Serialize@0x00408f50`:** WString at `+0xc`, then 4-byte writes at `+0x10` and `+0x14` only — **`m_deaths` (`+0x18`) not serialized** (disasm confirmed).
- **`m_link_prev` / stream I/O:** **Resolved** (agent todo 17 r2) — `IDSChained*` `this` at `+4`; `this+0xc` → `m_name` (+0x10); `m_link_prev` at absolute `+0xc` is list-only (`CDSChained_InsertListNode`).

## Ghidra deltas

- `CScoreItem`: `field_08` → `m_link_next`, `field_0c` → `m_link_prev` (types `pointer`).
- `CScore`: `field_6c` → `pDefaultFocusChild` (type `pointer`).
- **Agent todo 17:** `delete_data_type` / `create_struct` `CScore` — `pBase_CWindow_MI` → named dialog prefix (`nBbox_*`, `wViewFlags`, `pParent`, `bModalFlag`, …); `get_struct_layout` → 116 B.
- `save_program bulanci.exe` (once).

## Remaining UNK

- ~~Split `CScore.base_to_6c`~~ — **done** (agent todo 17, 2026-05-30).
- ~~Reconcile `CScoreItem.m_link_prev` vs `CDsString` stream use at `+0xc`~~ — **done** (agent todo 17 r2).
- ~~`CScoreItem.m_deaths` persistence~~ — **done** (agent todo 18 r3): not in `Serialize`/`Deserialize`; Ghidra comments + `CScoreItem.md`.

## Artifacts updated

- `CScore.md`, `CScoreItem.md`
- This file: `batch_17_followup_summary.md`
