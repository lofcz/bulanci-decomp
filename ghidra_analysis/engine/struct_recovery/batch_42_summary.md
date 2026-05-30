# Struct recovery batch 42/50

**Index:** 42 (`batches_50.json`)  
**Structs:** `CDSUpdatedItem`, `CDSVideoPlayer`  
**Program:** `bulanci.exe` (saved once at end)

## Results

| Struct | Status | Size | Ghidra |
|--------|--------|------|--------|
| `CDSUpdatedItem` | VERIFIED | 24 (`0x18`) | Placeholder deleted; `create_struct` → `get_struct_layout` confirms 24 bytes |
| `CDSVideoPlayer` | VERIFIED | 72 (`0x48`) | Nested scheduler + track-manager fields; layout 72 bytes |

## Evidence summary

- **CDSUpdatedItem:** Cooperative scheduler client — ctor registers `this+4` in `g_pTaskList`; `CDSApp_PulseTasks` dispatches `Scheduler_DispatchDueEvents` on the host object; dtor unregisters and clears slot storage at `+0x0c`.
- **CDSVideoPlayer:** `ConstructTrackManager` defines full `0x48`-byte object: `CDSUpdatedItem` at `+0x04`, 8-byte track vector at `+0x1c`, frame stepping via `TM_AdvanceFrame`, teardown via `CDSVideoPlayer_TM_Destructor`.
- **Deliverables:** [CDSUpdatedItem.md](./CDSUpdatedItem.md), [CDSVideoPlayer.md](./CDSVideoPlayer.md)

## Notes

- Prior Ghidra types were 1-byte placeholders; both replaced via `delete_data_type` + `create_struct`.
- `FUN_004312c0` remains mis-attributed in some catalogs (body is collection insert, not `CDSUpdatedItem`).
- Next batch (index 43): `CDSWav`, `CDSWavStream`.
