# Round 10 deep — Task 15 report

## Task

| Field | Value |
|-------|--------|
| **id** | 15 |
| **title** | Struct deep: CDSTrackVector memmove path |
| **kind** | struct |
| **struct_name** | `CDSTrackVector` |
| **seed** | `0x004397e0` |
| **addresses** | `0x004397e0`, `0x00439800`, `0x00439840` |
| **acceptance** | Vector growth/realloc algorithm; prove element size and capacity fields |

## Status

**DONE** — 8-byte element stride, 16-byte header layout, and growth round-up algorithm closed via live Ghidra + IDA correlation. R5 worker-14 **BLOCKED** entry for `0x004397e0` resolved; `0x00439840` mis-attributed as vector memmove corrected.

## Functions / struct

| VA | Symbol (live Ghidra) | Role | Evidence |
|----|----------------------|------|----------|
| `0x004397e0` | `TM_ShiftTrackEntries` | `__cdecl` memmove of **8-byte** track records | Disasm forward loop `ADD EAX,0x8` @ `0x0043980c`; reverse `LEA EAX,[ESI+ECX*8]` @ `0x00439820`; IDA `sub_4397E0` copies 2 dwords per iter (`v3 += 2`) |
| `0x00439800` | *(in `TM_ShiftTrackEntries`)* | Forward-memmove loop head | `MOV ESI,[ECX+EAX]` / `MOV [EAX+4],ESI` — 8-byte stride |
| `0x00439840` | `TM_PauseAndStampClock` | **Not** CDSTrackVector — `CDSVideoPlayer` scheduler pause | Decompile uses `bPaused`, `scheduler`, `dwSchedulerCookie`, `pRenderTarget`; R5 w14 blocked as “CDSTrackVector memmove” — **wrong function** |
| `0x004398b0` | `TM_ClearTracks` | Realloc backing store | `Runtime_ReallocOrThrow(pTracks, newCapacity * 8)`; writes `+4` (`cTracksAllocated`); IDA `sub_4398B0` `8 * a2` |
| `0x00439900` | `CDSTrackVector_EnsureCapacity` | Growth prelude | If `+4 < minCount`: `newCap = (minCount / +0xc + 1) * +0xc`; calls `TM_ClearTracks`; throw if `+0xc == 0` |
| `0x00439a70` | `CDSTrackVector::TM_InsertTrackAt` | Insert path | `EnsureCapacity(dwTracks+1)` → `TM_ShiftTrackEntries` @ `0x439a96` (sole xref) → write 8-byte record → `++dwTracks` |
| `0x00439c70` | `ConstructTrackManager` | Ctor init | `cTrackCapacity = 8`; zero `pTracks`, `dwTracksAllocated`, `dwTracks` |

### `CDSTrackVector` layout (16 bytes)

| Offset | Size | Logical name | Ghidra name | Use |
|--------|------|--------------|-------------|-----|
| `+0x00` | 4 | `pTracks` | `pTracks` | Heap array of 8-byte records |
| `+0x04` | 4 | `cTracksAllocated` | `dwTracksAllocated` | Capacity in **elements** (not bytes) |
| `+0x08` | 4 | `cTracks` | `dwTracks` | Live element count |
| `+0x0c` | 4 | `cTrackCapacity` | `dwTrackCapacity` | Growth chunk; ctor `= 8`; divisor in EnsureCapacity |

### Growth algorithm (pseudocode)

```
EnsureCapacity(vec, minCount):
  if vec.cTracksAllocated >= minCount: return
  if vec.cTrackCapacity == 0: throw(11, 13)
  newCap = (minCount / vec.cTrackCapacity + 1) * vec.cTrackCapacity
  TM_ClearTracks(vec, newCap)  // Realloc(vec.pTracks, newCap * 8); vec.cTracksAllocated = newCap

TM_InsertTrackAt(vec, record, index):
  EnsureCapacity(vec, vec.cTracks + 1)
  tail = vec.cTracks - index
  if tail: TM_ShiftTrackEntries(&vec.pTracks[index+1], &vec.pTracks[index], tail)
  vec.pTracks[index] = record  // 8 bytes
  vec.cTracks++
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x00439900` | `CDSVideoPlayer_EnsureCapacity` → **`CDSTrackVector_EnsureCapacity`** |
| `set_function_prototype` | `0x00439900` | `void CDSTrackVector::CDSTrackVector_EnsureCapacity(uint minCount)` `__thiscall` |
| `set_function_prototype` | `0x004398b0` | `void CDSTrackVector::TM_ClearTracks(int newCapacity)` `__thiscall` |
| `set_decompiler_comment` | `0x004397e0`, `0x00439840`, `0x004398b0`, `0x00439900` | R10 growth/memmove + R5 mis-block correction |
| `force_decompile` | `0x00439a70`, `0x00439900` | Insert path now calls `CDSTrackVector_EnsureCapacity` |
| `save_program` | `bulanci.exe` | Saved |

`set_function_this_type` unavailable in current MCP bridge — `this` remains `void*` in EnsureCapacity/TM_ClearTracks decompile despite class-scoped prototype string.

## Decomp corrections (IDA vs Ghidra)

| Issue | IDA (`bulanci.ida.exe.c`) | Ghidra before | Fix |
|-------|---------------------------|---------------|-----|
| EnsureCapacity namespace | `sub_439900(_DWORD *this, …)` on vector base (`this+0`/`+2` = `pTracks`/`cTracks`) | `_Globals::CDSVideoPlayer_EnsureCapacity` | Renamed **`CDSTrackVector_EnsureCapacity`**; callers `TM_InsertTrackAt` / `InsertOrFindTrack` pass `CDSTrackVector *` |
| `0x439840` identity | `sub_439840` — scheduler (`this+53`, `this+64`, `sub_42F300`) | Already `TM_PauseAndStampClock(CDSVideoPlayer*)` | Documented: **not** vector memmove (R5 w14 blocked list error) |
| Element size | `8 * index`, `8 * a2` in `sub_439A70` / `sub_4398B0` | Same in typed `TM_InsertTrackAt` | Confirmed 8-byte `{key, ptr}` records |
| `TM_ClearTracks` semantics | Realloc only; no count zero | Same | Name misleading; does not clear `cTracks` |

## Frida

Not required — static disasm + IDA/Ghidra agreement sufficient.

## Remaining UNK

- Ghidra `CDSTrackVector_EnsureCapacity` / `TM_ClearTracks` decompile `this` type (`void*`) until MCP supports ECX retype or manual HighVariable edit.
- `TM_ClearTracks` legacy name (resize-only); optional rename to `CDSTrackVector_SetCapacity` deferred.
- Ghidra struct field display ids still `dwTracks*` / `dwTrackCapacity` vs logical `cTracks*` (known R4 quirk).

## Struct doc

- [CDSTrackVector.md](../struct_recovery/CDSTrackVector.md) — growth/realloc table added (R10).
