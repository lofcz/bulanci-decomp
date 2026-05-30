# Round 4 — Task 44 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 44 |
| **round** | 4 (MCP `set_function_this_type` + struct label pass) |
| **title** | CDSTrackVector InsertOrFindTrack this; cTracks* layout labels |
| **one_liner** | Class-scope `InsertOrFindTrack` on `CDSTrackVector *`; rename count fields to `cTracks*`; verify live-count path passes inner `+8`. |
| **prior** | [round3_task_44_report.md](./round3_task_44_report.md) |
| **structs** | CDSTrackVector, CDSVideoPlayer |

## Status

**DONE** — `InsertOrFindTrack` moved into class `CDSTrackVector` with typed `this`; decompile uses `this->dwTracks` (logical `cTracks`). Struct archive field labels remain `dwTracks*` after MCP rename/recreate (known quirk).

## Evidence

| Claim | Address | Evidence |
|-------|---------|----------|
| ECX is vector base | `0x00439bd0` | Entry `MOV ESI,ECX`; lookups use `[ESI+0]`, `[ESI+0x8]` |
| Live count to lookup | `0x00439bea` | `MOV EAX,[ESI+0x8]` → stack arg to `TM_LookupTrackIndex@0x00439730` |
| Class-scoped decompile | `0x00439bd0` | `CDSTrackVector::InsertOrFindTrack(CDSTrackVector *this,…)`; `TM_LookupTrackIndex(this,record,compareFn,this->dwTracks)` |
| TM helpers typed | `0x00439730`, `0x00439a70` | `CDSTrackVector::TM_LookupTrackIndex` / `TM_InsertTrackAt`; `searchCount==-1` → `this->dwTracksAllocated` |
| Host init band | `0x00439c70` | `ConstructTrackManager` zeros `trackVector` through `+0x28`, capacity `8` @ inner `+0xc` |
| Struct size | layout | `CDSTrackVector` 16 B; `CDSVideoPlayer` 72 B, `trackVector` @ `+0x1c` |

### Prototype (verified)

| Function | ECX (`this`) | Notes |
|----------|--------------|-------|
| `InsertOrFindTrack` | `CDSTrackVector *` | Was `_Globals::` + `void *this` (R3 UNK) |
| `TM_LookupTrackIndex` | `CDSTrackVector *` | `searchCount==-1` uses allocated count @ `+4` |
| `TM_InsertTrackAt` | `CDSTrackVector *` | Insert/splice on sorted 8-byte records |
| `ConstructTrackManager` | `CDSVideoPlayer *` | Initializes nested `trackVector` @ host `+0x1c` |

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `set_function_this_type` | `0x00439bd0` | Moved into class `CDSTrackVector`; `this` = `CDSTrackVector *` |
| `set_function_prototype` | `0x00439bd0` | `int __thiscall InsertOrFindTrack(CDSTrackVector *this, undefined4 *record, void *compareFn, int flags)` |
| `modify_struct_field` | `CDSTrackVector` | Renames `dwTracks*` → `cTracks*` reported success |
| `recreate_struct` | `CDSTrackVector` | 16 B rebuild; `get_struct_layout` still lists `dwTracks*` |
| `set_decompiler_comment` | `0x00439bd0` | R4 todo 44 ECX / live-count note |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CDSTrackVector.md](./CDSTrackVector.md) — R4 apply log; `InsertOrFindTrack` class-scoped.
- [CDSVideoPlayer.md](./CDSVideoPlayer.md) — R4 cross-ref on track-vector helpers.

## Remaining UNK

- `get_struct_layout CDSTrackVector` may keep `dwTracks*` display ids despite logical `cTracks*` in markdown (MCP/archive quirk, same as R3).
- `CDSVideoPlayer_EnsureCapacity` remains `_Globals::` namespace; called with `CDSTrackVector *` vector base (capacity growth helper).
- Nested embed in `ConstructTrackManager` decompile may show `trackVector.field_0x4` until Ghidra re-syncs nested component names.
