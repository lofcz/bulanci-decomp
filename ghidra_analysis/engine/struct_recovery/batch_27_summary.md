# Struct recovery — batch 27/50

**Index:** 27 (`batches_50.json`)  
**Structs:** `CDSCollection`, `CDSDirectSound`  
**Protocol:** evidence-only (`AGENT_PROTOCOL.md`)  
**Ghidra:** `bulanci.exe` saved after apply

## Results

| Struct | Status | Size | Ghidra |
|--------|--------|------|--------|
| `CDSCollection` | VERIFIED | 0x18 (24) | 6 fields applied |
| `CDSDirectSound` | VERIFIED | 0x54 (84) | 17 fields in Ghidra (`m_updatedItem` + COM/worker tail); InitPrimary prototype + asm notes |

## Evidence highlights

- **CDSCollection:** Vector core at +0x08 (`m_items`), +0x0C (`m_count`), +0x10 (`m_capacity`), +0x14 (`m_growthChunk`) from `CDSCollection_Resize` @ 0x00431000, `_Globals::CDSCollection_Insert` @ 0x004310f0 / `Remove` @ 0x00431140 / `EnsureCapacity` @ 0x00431080, plus `CDSStreamStorage_ctor` embedded instance at +0x1c.
- **CDSDirectSound:** Size bound by `CBulanci::CDSApp_ctor` (+0x200..+0x253). `CDSObject::CDSDirectSound_ctor` @ 0x0043c7c0 builds `CDSUpdatedItem` (+0x04), event handler (+0x1c), worker thread block (+0x24), wake event at +0x50; `~CDSDirectSound` @ 0x0043c8b0 signals shutdown at +0x4c and joins worker.

## Deliverables

- `CDSCollection.md`
- `CDSDirectSound.md`

## Notes

- `CDSCollection_Load` / `CDSCollection_Save` take a chained sub-pointer (`this+4`); full-object access uses base at +0.
- `CDSDirectSound` worker methods use `this` adjusted to +0x24; decompiler often still labels it `CDSDirectSound *`.

## Ghidra actions (slice 27 agent)

- `set_function_prototype`: `CDSDirectSound_InitPrimary`, `CDSDirectSound_SignalEventIfVoicesActive`, `CDSDirectSound_WorkerThreadLoop`, `CDSCollection_InsertKeyed`
- `set_decompiler_comment` @ 0x0043cce1, 0x004312c0
- `save_program` (bulanci.exe)

## Blockers

- `CDSCollection_InsertKeyed`: prototype `CDSCollection *` applied; decompiler `this` still `CDSUpdatedItem *`
- `CDSDirectSound_InitPrimary`: may decompile as `CDSApp::` when called from app ctor (`CDSApp+0x200`)

## Agent todo 26

- `CDSDirectSound_SignalEventIfVoicesActive` @ `0x0043cce1`: **resolved** — struct tail correct (`m_bShutdown` +0x4c, `m_hWorkerEvent` +0x50); function asm uses +0x4c for `SetEvent` (documented binary quirk).
