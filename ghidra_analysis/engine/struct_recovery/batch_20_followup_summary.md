# Struct recovery batch 20 follow-up (round 2)

**Prior:** `batch_20_summary.md` (`CDSSafeStream`, `CDSSafeStreamInfo`)  
**Status:** **FOLLOWUP_DONE**  
**Program:** `bulanci.exe`

## Follow-ups addressed

| Item (batch 20) | Result |
|-----------------|--------|
| Recover `CDSFilterStream` base for MI layout | **Done** — `CDSFilterStream.md`; `OperatorNew(0x38)` @ `FUN_00434760`; shared `0x00..0x14` header with `CDSSafeStream`, not a size-extension layout |
| Recover `CDSChain` / name chain fields | **Done** — `CDSChain.md` (`0x14` embedded); `m_pHead` / `m_pSentinel` / `m_nChildCount` at `CDSSafeStream+0x20..+0x28` |
| Name `FUN_00446ea0` / `FUN_00446d90` / `FUN_00446c30` | **Done** — Ghidra renames below |
| `CDSSafeStreamInfo` link fields | **Done** — `m_link_next` / `m_link_prev` tied to `FUN_0042f780` / `CDSChained_AppendChild` |

## Ghidra deltas

| Action | Detail |
|--------|--------|
| Renamed | `FUN_0042f9b0` → `CDSChained_AppendChild` |
| Renamed | `FUN_0042fab0` → `CDSChained_ClearChildren` |
| Renamed | `FUN_00446ea0` → `CDSSafeStream_RegisterThreadSlice` |
| Renamed | `FUN_00446d90` → `CDSSafeStream_GetThreadSlice` (already named) |
| Renamed | `FUN_00446c30` → `CDSSafeStream_ClearThreadSlices` |
| Struct | `CDSChain` rebuilt → **20 bytes** (`0x14` logical) |
| Struct | `CDSFilterStream` present → **52 bytes** in DT (heap **0x38**) |
| Saved | `save_program bulanci.exe` |

## Artifacts

- `CDSChain.md` (new)
- `CDSFilterStream.md` (new)
- Updated: `CDSSafeStream.md`, `CDSSafeStreamInfo.md`

## Remaining UNK

- `dwM_streamFlags` `0x20` vs `1` on `CDSSafeStream`.
- `CDSChain.m_pSentinel` heap vs. circular self-pointer (`FUN_0042f800`).
- Full standalone `CBulanci` `CDSChain` object (ctor @ `0x0040a680`) — not the embedded `0x14` slice.
- `CDSChained` dialog/view base (`0x68`+) — separate batch in `batches_50.json`.
