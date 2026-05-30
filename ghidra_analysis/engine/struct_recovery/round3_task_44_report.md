# Round 3 — Task 44 report

## Task

| Field | Value |
|-------|-------|
| **id** | 44 |
| **title** | Cosmetic CDSTrackVector embed @ CDSVideoPlayer+0x1c |
| **one_liner** | Nest `CDSTrackVector` (16 B) inside `CDSVideoPlayer` @ `+0x1c`; type track-vector helpers with `CDSTrackVector *this`; confirm `dwSchedulerCookie` @ `+0x40`. |
| **acceptance** | Ghidra struct embed + helper prototypes; update CDSVideoPlayer.md / CDSTrackVector.md |

## Status

**DONE**

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Track vector init at host `+0x1c`..`+0x28` | `ConstructTrackManager@0x00439c70` | Disasm: `MOV [ESI+0x1c],EBX`..`MOV [ESI+0x28],8` (zero + capacity 8) |
| Nested field access in decompiler | same | `(this->trackVector).pTracks` / `.dwTracks` / `.dwTrackCapacity` |
| `TM_LookupTrackIndex` uses inner `+4`/`+0` | `TM_LookupTrackIndex@0x00439730` | `searchCount==-1` → `this->dwTracksAllocated`; linear/binary search on `this->pTracks` |
| `TM_InsertTrackAt` uses inner `+8` | `TM_InsertTrackAt@0x00439a70` | Insert path reads live count from vector `+8` (via callers) |
| Caller passes vector base | `InsertOrFindTrack@0x00439bd0` | `CDSTrackVector::TM_LookupTrackIndex(this,…,*(int *)((int)this+8))` |
| Ghidra embed layout | `get_struct_layout CDSVideoPlayer` | 72 B; `CDSTrackVector trackVector` @ offset **28** (`0x1c`) |
| Inner struct size | `get_struct_layout CDSTrackVector` | 16 B; `pTracks` + three `uint` count fields |
| Scheduler cookie name | `get_struct_layout CDSVideoPlayer` | `dwSchedulerCookie` @ `+0x40` (not `dwField_0x40`) |

## Ghidra deltas

- Verified existing `CDSTrackVector` (16 B) nested as `trackVector` @ `CDSVideoPlayer+0x1c`.
- `set_function_prototype` + `set_function_this_type` @ `0x00439730` / `0x00439a70` → `CDSTrackVector *` `__thiscall`; decompile uses `this->pTracks` / `this->dwTracksAllocated`.
- `set_decompiler_comment@0x00439cbc` — documents vector zero/init band.
- `modify_struct_field` rename `dwTracks*` → `cTracks*` on `CDSTrackVector` — **MCP reported success but `get_struct_layout` still lists `dwTracks*`** (display-name cosmetic incomplete).
- `save_program bulanci.exe`.

## Struct doc updates

- [CDSVideoPlayer.md](./CDSVideoPlayer.md) — layout + Ghidra apply (agent todo 44).
- [CDSTrackVector.md](./CDSTrackVector.md) — size proof + embed note.

## Remaining UNK

- `InsertOrFindTrack@0x00439bd0` still `void *this` (not moved into `CDSTrackVector` class).
- Ghidra may keep `dwTracks*` field labels despite logical `cTracks*` names in markdown.
- `TM_LookupTrackIndex` with `searchCount==-1` uses `cTracksAllocated` (+4), not live `cTracks` (+8); live callers pass explicit count from `+8`.
