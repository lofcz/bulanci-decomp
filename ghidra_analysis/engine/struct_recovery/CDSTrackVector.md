# CDSTrackVector

## Status

`VERIFIED` (size `0x10`; embedded in `CDSVideoPlayer` @ `+0x1c`)

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof == 0x10` (16 bytes) | `0x00439c70` | `ConstructTrackManager` writes four dwords at `this+0x1c`..`+0x28`; `TM_*` helpers use `this+0`/`+4`/`+8`/`+0xc` |
| Heap elements | `0x00439bd0` | Sorted 8-byte `{key, IDSStream*}` records referenced via `pTracks` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `void*` | `pTracks` | `ConstructTrackManager@0x00439c70`; `TM_LookupTrackIndex@0x00439730` (`*(int*)this`) |
| `+0x04` | 4 | `uint32` | `cTracksAllocated` | `ConstructTrackManager`; `TM_LookupTrackIndex` when `searchCount==-1` |
| `+0x08` | 4 | `uint32` | `cTracks` | `ConstructTrackManager`; `TM_InsertTrackAt@0x00439a70` (`this+8`) |
| `+0x0c` | 4 | `uint32` | `cTrackCapacity` | `ConstructTrackManager` (`=8`); `CDSVideoPlayer_EnsureCapacity@0x00439900` |

## Ghidra apply

```
get_struct_layout CDSTrackVector  → 16 bytes (agent todo 44 R3, 2026-05-30)
CDSVideoPlayer.trackVector @ +0x1c (nested embed, 16 B)
set_function_this_type CDSTrackVector* @ TM_LookupTrackIndex@0x00439730 / TM_InsertTrackAt@0x00439a70
ConstructTrackManager decompile: (this->trackVector).pTracks / .dwTracks* / .cTrackCapacity=8
save_program bulanci.exe
```

## UNK

- Ghidra struct listing may still label count fields `dwTracks*` after MCP rename attempts; logical names remain `cTracks*`.
- `InsertOrFindTrack@0x00439bd0` not class-scoped to `CDSTrackVector` yet.
