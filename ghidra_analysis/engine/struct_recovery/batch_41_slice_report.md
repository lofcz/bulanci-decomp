# Slice 41 — CDSUpdatedItem / CDSVideoPlayer

**Agent:** parallel RE slice **41** of 50  
**Batch index:** 41 (`batches_50.json`)  
**Date:** 2026-05-30  
**Program:** `bulanci.exe` (saved)

## Types mapped

| Type | Size | Status |
|------|------|--------|
| `CDSUpdatedItem` | `0x18` (24 B) | VERIFIED — Ghidra struct + ctor prototype |
| `CDSVideoPlayer` | `0x48` (72 B) | VERIFIED — nested `scheduler` @ +4, track vector @ +0x1c |
| `CBulanek` (critical-path embed) | — | PARTIAL — `scheduler` @ +0x88, `videoTrackManager` @ +0xA8 typed |

## Ghidra actions

| Action | Target |
|--------|--------|
| `modify_struct_field` | `CDSUpdatedItem`: `bM_isLive`→`bIsLive`, `dwEventSlots`→`cEventSlots` |
| `set_function_prototype` | `CDSUpdatedItem_ctor@0x0042f060`, `ConstructTrackManager@0x00439c70`, `Scheduler_RegisterEventSlot@0x0042f210`, `CDSVideoPlayer_CreateTrackManagerHeap@0x00439f50` |
| `modify_struct_field` | `CBulanek.pSchedulerBase` → `scheduler` (`CDSUpdatedItem`) |
| `add_struct_field` | `CBulanek.videoTrackManager` (`CDSVideoPlayer`) @ offset 168 |
| `save_program` | `bulanci.exe` |

## Evidence anchors

- `CDSUpdatedItem_ctor@0x0042f060` — **29** xref call sites
- `ConstructTrackManager@0x00439c70` — **12** embed + heap factory
- `CBulanekCtor@0x0041e4b0` — typed `CDSUpdatedItem_ctor(this+0x88)`, `ConstructTrackManager(this+0xa8)`

## Files changed

- [CDSUpdatedItem.md](./CDSUpdatedItem.md)
- [CDSVideoPlayer.md](./CDSVideoPlayer.md)
- [CBulanek.md](./CBulanek.md)
- [gameplay_struct_backlog.md](../gameplay_struct_backlog.md)
- This report

## Blockers

- `Scheduler_RegisterEventSlot` decompiler `this` remains `void*` (__thiscall ECX limitation).
- `CBulanek` mid-object (`+0xA0..+0xA7`, `+0xF0..`) still opaque pads in Ghidra; tail fields not re-laid.
- Optional nested `CDSTrackVector` @ `CDSVideoPlayer+0x1c` not created.
