# Agent todo 43 — CDSUpdatedItem IDSUpdated facet @ +0x00

| Field | Value |
|-------|-------|
| **id** | 43 |
| **title** | CDSUpdatedItem IDSUpdated facet @ +0x00 |
| **status** | **DONE** |
| **program** | `bulanci.exe` (saved) |

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| `CDSUpdatedItem_ctor` skips embed `+0` | `0x0042f060` | First store `*(this+4)`; PRE comment on ctor |
| `CAdvertising` host facet | `0x0040e670` | `(updatedItem).pVftable_IDSUpdated = g_pCAdvertising_vftable_IDSUpdated` |
| `CShot` host facet | `0x0041ee61` | `g_pCShot_vftable_CDSUpdatedItem_88` on `updatedItem` |
| `CDSVideoPlayer` scheduler embed | `0x00439cb6` | `pVftable_IDSUpdated = 0x487770` after ctor @ `ConstructTrackManager` |
| All ctor xrefs | `0x0042f060` | `get_xrefs_to` → **29** callers; each stores host vtable at embed `+0` after ctor |

## Ghidra deltas

- `CDSUpdatedItem.pVftable_IDSUpdated` @ offset 0 (confirmed)
- `modify_struct_field`: `CShot.pUpdatedItem` → `updatedItem` (`CDSUpdatedItem`); `CGame+4` → `scheduler`; `CMenu+0x68` → `updatedItem`
- `set_decompiler_comment` @ `0x0042f060`, `0x0040e670`, `0x00439cb6`
- `save_program bulanci.exe` (×2)

## Docs

- [CDSUpdatedItem.md](./CDSUpdatedItem.md) — host facet catalog table
- [batch_41_followup_summary.md](./batch_41_followup_summary.md) — `+0x00` UNK closed

## Remaining

- `dwEventSlots` vs `cEventSlots` label on `CDSUpdatedItem` (cosmetic)
- `CEdit` / `CGaming` / `CWindow` still need full struct reparent (out of scope)
