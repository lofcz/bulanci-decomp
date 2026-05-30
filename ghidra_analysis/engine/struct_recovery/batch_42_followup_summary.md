# Struct recovery batch 42 follow-up (round 2)

**Index:** 42 (`batches_50.json`)  
**Prior:** [batch_42_summary.md](./batch_42_summary.md)  
**Structs:** `CDSUpdatedItem`, `CDSVideoPlayer`  
**Status:** **FOLLOWUP_COMPLETE**  
**Program:** `bulanci.exe` (saved once)

## Actions taken

| Item (from batch 42 / struct UNK) | Result |
|-----------------------------------|--------|
| `FUN_004312c0` mis-attributed as `CDSUpdatedItem` method | Ghidra: function already `CDSCollection_InsertKeyed`; set prototype `CDSCollection *this`; plate comment documents `CDSCollection*`. Decompiler still shows stale `CDSUpdatedItem*` on `this` (Ghidra ECX limitation). |
| `CDSVideoPlayer` ctor address conflation (`0x439730` vs `0x439c70`) | Docs corrected: `CDSObject::ConstructTrackManager@0x00439c70`; `0x00439730` = `TM_LookupTrackIndex` (track-vector binary search). |
| `+0x20` track-vector field | Confirmed inner-vector `+4`: zeroed in ctor, used by `CDSVideoPlayer_EnsureCapacity` (`this+4` on `&pTracks`); Ghidra field `dwTracksAllocated` @ offset 32. |
| Helper-tier `FUN_00439730` / `FUN_00439a70` | Ghidra names aligned with decompiler: `TM_LookupTrackIndex`, `TM_InsertTrackAt` (parallel rename `CDSTrackVector_*` superseded). |
| `CDSUpdatedItem+0x00` embed UNK | `CShot_Ctor@0x0041edf0`: facet vtable at embed `+0` after ctor; `ConstructTrackManager` sets `0x487770` on `CDSVideoPlayer` scheduler embed. Documented in [CDSUpdatedItem.md](./CDSUpdatedItem.md). |

## Ghidra deltas

- `CDSCollection_InsertKeyed@0x004312c0`: prototype + plate comment.
- `CDSVideoPlayer.dwTracksAllocated` @ +0x20 (field rename from placeholder).
- Track helpers at `0x00439730` / `0x00439a70`: `TM_LookupTrackIndex`, `TM_InsertTrackAt`.
- `save_program bulanci.exe` executed.

## Remaining UNK

- `CDSCollection_InsertKeyed`: decompiler `this` type not fully retyped to `CDSCollection*`.
- ~~`CDSUpdatedItem+0x00` on hosts without facet vtable write~~ — closed by agent todo 43 (`CAdvertising+0x70` and all 29 ctor xrefs write host facet at embed `+0`).
- ~~Optional nested `CDSTrackVector` Ghidra struct at `CDSVideoPlayer+0x1c`~~ — **done** (agent todo 44): `CDSTrackVector` 16 B; `CDSVideoPlayer.trackVector` @ +0x1c; helper prototypes set.

## Deliverables touched

- [CDSUpdatedItem.md](./CDSUpdatedItem.md)
- [CDSVideoPlayer.md](./CDSVideoPlayer.md)
