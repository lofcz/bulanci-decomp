# Struct recovery batch 41/50

**Program:** `bulanci.exe`  
**Batch index:** `41` (`batches_50.json[41]`)  
**Structs:** `CDSUpdatedItem`, `CDSVideoPlayer`

## Results

| Struct | Status | Size | Ghidra struct |
|--------|--------|------|---------------|
| `CDSUpdatedItem` | `VERIFIED` | `0x18` (24) | Pre-existing; matches ctor/scheduler |
| `CDSVideoPlayer` | `VERIFIED` | `0x48` (72) | Pre-existing; matches `ConstructTrackManager` |

## Evidence highlights

- **CDSUpdatedItem** — Scheduler registration object: vtable at `+4`, self at `+8`, slot pointer table at `+0xc`, slot count at `+0x10`, live flag at `+0x14`. Driven by `CDSUpdatedItem_ctor@0x0042f060`, `Scheduler_DispatchDueEvents@0x0042eb30`, `Scheduler_RegisterEventSlot@0x0042f210` (slot heap nodes `0x1c` bytes).
- **CDSVideoPlayer** — Misnamed track/frame manager (see `formats/status.md`): primary vtable at `+0`, embedded `CDSUpdatedItem` at `+4`, track array at `+0x1c`, playback state through `+0x44`. Built by `CDSObject::ConstructTrackManager@0x00439730`; frame tick `CDSVideoPlayer_AdvanceFrameAndPauseIfDone@0x00439b90`.

## Deliverables

- `ghidra_analysis/engine/struct_recovery/CDSUpdatedItem.md`
- `ghidra_analysis/engine/struct_recovery/CDSVideoPlayer.md`

## Ghidra

- `get_struct_layout` for both structs: size > 1, layouts consistent with evidence.
- `save_program bulanci.exe` — end of batch.

## Notes

- Index `41` in `batches_50.json` is the **42nd** entry (`CDSUpdatedItem`, `CDSVideoPlayer`). Index `40` is `CDSStreamStorage` / `CDSStrmResInfo` (previous pair).
