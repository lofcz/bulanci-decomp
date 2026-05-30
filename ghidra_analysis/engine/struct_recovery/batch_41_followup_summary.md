# Struct recovery follow-up — batch 41/50

**Status:** `FOLLOWUP_DONE`  
**Prior:** `batch_41_summary.md`, `CDSUpdatedItem.md`, `CDSVideoPlayer.md`  
**Program:** `bulanci.exe`

## Actions

| Item | Result |
|------|--------|
| `CDSVideoPlayer+0x20` (`dwField_0x20`) | **Resolved** → `cTracksAllocated`: allocated slot count for the track buffer; `TM_ClearTracks` / `CDSVideoPlayer_EnsureCapacity` use inner-vector `this+4` (`param_1+7` in ctor) |
| Track-vector helper `this` base | **Documented**: `InsertOrFindTrack`, `TM_LookupTrackIndex`, `TM_InsertTrackAt`, `TM_ClearTracks`, `EnsureCapacity` take `&pTracks` (`+0x1c`), not the outer `CDSVideoPlayer*` |
| `FUN_00439730` / `FUN_00439a70` | Renamed in Ghidra → `TM_LookupTrackIndex`, `TM_InsertTrackAt` |
| `CDSUpdatedItem+0x00` embed | **Resolved** (agent todo 43): `pVftable_IDSUpdated` on struct; all 29 ctor xrefs cataloged; host embeds typed (`CShot`, `CGame`, `CMenu`, …) |
| ConstructTrackManager address in batch_41_summary | Note: factory is **`0x00439c70`**, not `0x00439730` (lookup helper) |

## Ghidra deltas

- `modify_struct_field`: `CDSVideoPlayer` field @ `0x20` → `dwTracksAllocated` (was `dwField_0x20` / `dwFindIndexCountFallback`); `dwField_0x40` → `dwSchedulerCookie`
- `rename_function_by_address`: `0x00439730` → `TM_LookupTrackIndex`; `0x00439a70` → `TM_InsertTrackAt`
- `get_struct_layout CDSVideoPlayer` — 72 bytes, `cTracksAllocated` @ +32 (0x20)
- `save_program bulanci.exe`

## Docs updated

- `CDSVideoPlayer.md` — field names, ctor address, track-vector subobject note, UNK cleared
- `CDSUpdatedItem.md` — `+0x00` embed evidence, narrowed UNK

## Remaining UNK

- Optional nested `CDSTrackVector` type at `CDSVideoPlayer+0x1c` (cosmetic; agent todo 44)
- `CDSUpdatedItem.cEventSlots` Ghidra label may still read `dwEventSlots` (cosmetic)
