# Struct recovery batch 39 follow-up (round 2)

**Prior:** `batch_39_summary.md`  
**Status:** **FOLLOWUP_DONE**  
**Date:** 2026-05-30

## Actions taken

| Follow-up (batch 39) | Result |
|----------------------|--------|
| Recover `CDSChain` / `CDSFilterStream` / `CDSQueueStream` bases | **`CDSChain`** VERIFIED `0x14` — new `CDSChain.md`, Ghidra struct applied. **`CDSFilterStream`** PARTIAL `0x38` — new `CDSFilterStream.md`, Ghidra struct applied. **`CDSQueueStream`** unchanged (batch 35 `CDSQueueStream.md`; `streamState` `0x20` closed pattern cross-referenced). |
| Map `dwM_streamFlags` (`0x20` vs `1`) | **PARTIAL** — writers proven: ctor/`Close` → `0x20`, `FUN_00446ea0` → `1` (lifecycle / closed sentinel, same convention as `CDSFilterStream::streamState`). No read xref in scope. |
| `nM_chain_count` semantics | **VERIFIED** — lives at **`CDSChain+0x10`** (outer `+0x28`); incremented in `FUN_0042f9b0`, drained in `FUN_0042fab0`. Prior layout had count at `+0x24` — corrected via `CDSChain` embed. |

## Ghidra deltas

- `delete_data_type` + `create_struct` **`CDSChain`** (20 bytes / `0x14`)
- `delete_data_type` + `create_struct` **`CDSFilterStream`** (56 bytes / `0x38`)
- `get_struct_layout` confirmed both sizes > 1
- `save_program bulanci.exe` (once)

## Artifacts updated

- `CDSChain.md` (new)
- `CDSFilterStream.md` (new)
- `CDSSafeStream.md` (chain embed, `dwM_streamFlags` table, MI note vs filter layout)

## Remaining UNK

- `dwM_streamFlags` / `CDSFilterStream::field_10` — write-only or ctor-only in follow-up scope.
- Standalone `CDSChained` prefix (dialogs/views) — separate manifest batch, not 30–36 file indices.
- `CDSSafeStream` upgrade to **VERIFIED** blocked on read-side proof for `dwM_streamFlags` and filter-vs-safe field reconciliation beyond RTTI.
