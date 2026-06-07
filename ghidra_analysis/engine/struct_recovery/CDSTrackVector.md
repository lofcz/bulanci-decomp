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

**Agent todo 44 R4 (2026-05-30):** `set_function_this_type` + prototype @ `InsertOrFindTrack@0x00439bd0` → class `CDSTrackVector`, `CDSTrackVector *this`; decompile `TM_LookupTrackIndex(this,…,this->dwTracks)` (live count @ `+8`). Asm `MOV ESI,ECX` / `MOV EAX,[ESI+0x8]`. `modify_struct_field` / `recreate_struct` for `cTracks*` — MCP success but `get_struct_layout` may still list `dwTracks*`. Report: [round4_task_44_report.md](./round4_task_44_report.md).

## Growth / realloc (R10 task 15)

| Step | Function @ VA | Algorithm |
|------|---------------|-----------|
| Insert prelude | `TM_InsertTrackAt@0x00439a70` / `InsertOrFindTrack@0x00439bd0` | `CDSTrackVector_EnsureCapacity(this, cTracks + 1)` before splice or append |
| Round-up growth | `CDSTrackVector_EnsureCapacity@0x00439900` | If `cTracksAllocated < minCount`: require `cTrackCapacity != 0` else `CDSSimpleException_Throw(11,13)`; `newCap = (minCount / cTrackCapacity + 1) * cTrackCapacity` |
| Heap resize | `TM_ClearTracks@0x004398b0` | `pTracks = Runtime_ReallocOrThrow(pTracks, newCap * 8)`; `cTracksAllocated = newCap` (does **not** change `cTracks`) |
| Splice tail | `TM_ShiftTrackEntries@0x004397e0` | 8-byte forward/reverse memmove; stride proven by `ADD EAX,0x8` @ `0x0043980c` and `LEA EAX,[ESI+ECX*8]` @ `0x00439820` |
| Ctor default | `ConstructTrackManager@0x00439c70` | `cTrackCapacity = 8`; `pTracks/cTracksAllocated/cTracks = 0` |

**Element size:** 8 bytes — `{ uint32 sortKey; IDSStream* pSeq }` (`index * 8` in insert/lookup; `newCapacity * 8` in realloc).

## UNK

- Ghidra struct listing may still label count fields `dwTracks*` after MCP rename attempts; logical names remain `cTracks*` in markdown.
- `CDSTrackVector_EnsureCapacity@0x00439900` renamed from `CDSVideoPlayer_EnsureCapacity` (R10); decompile `this` may remain `void*` (MCP cannot retype ECX).
- `TM_ClearTracks` name is legacy — function only resizes storage, does not clear live count.
