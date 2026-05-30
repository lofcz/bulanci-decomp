# Struct recovery batch 38/50

**Index:** 38 (`batches_50.json`)  
**Structs:** `CDSSafeStream`, `CDSSafeStreamInfo`  
**Protocol:** `AGENT_PROTOCOL.md` (evidence-only)  
**Agent:** slice 38 (2026-05-30)

## Results

| Struct | Status | Size | Ghidra |
|--------|--------|------|--------|
| `CDSSafeStream` | PARTIAL | **0x48** (72) | 13 fields; embedded chain at `+0x18..+0x28` (`nM_chain_count` @ `+0x28`) |
| `CDSSafeStreamInfo` | VERIFIED | **0x18** (24) | 6 fields — thread list node |

## Size evidence (summary)

- **CDSSafeStream:** `OperatorNewWithBadAlloc(0x48)` in `CBulanci::FUN_00434160@0x004341b2` and `CDSStreamStorage_CreateFilterSafeStream@0x0043483a` / `0x00434846` before `CDSSafeStream_ctor@0x433ab0`.
- **CDSSafeStreamInfo:** `OperatorNewWithBadAlloc(0x18)` in `CDSSafeStream_GetThreadSlice@0x446d90` and `CDSSafeStream_RegisterThreadSlice@0x446ea0`.

## Key layout notes

- **CDSSafeStream:** MI vtables `+0x00..+0x14`, flattened **`CDSChain`** list-head `+0x18..+0x28`, `CRITICAL_SECTION` `+0x2c`, wide name `m_streamName` `+0x44`.
- **CDSSafeStreamInfo:** intrusive doubly-linked node; `dwM_threadId` matched in `GetThreadSlice`; `m_streamSlice` released in dtor.

## Ghidra actions (slice 38)

| Action | Target |
|--------|--------|
| `get_struct_layout` | `CDSSafeStream`, `CDSSafeStreamInfo` — sizes confirmed |
| `set_function_prototype` | `CDSSafeStream_RegisterThreadSlice@0x446ea0` → `CDSSafeStream* this` |
| `set_plate_comment` | `0x446ea0` — documents true `this` type + field offsets |
| `save_program` | `bulanci.exe` |

## Deliverables

- `struct_recovery/CDSSafeStream.md` (layout synced to Ghidra flat chain fields)
- `struct_recovery/CDSSafeStreamInfo.md`
- `struct_recovery/batch_38_summary.md`

## Blockers / follow-up

- `dwM_streamFlags` — no read-side consumer found (writes only).
- `CDSSafeStream_RegisterThreadSlice` — decompiler `this` still mis-typed (`CDSSafeStreamInfo*`); ECX retype blocked by Ghidra API.
- Parent gameplay spine (`CBulanci`, `CGame`) out of scope for slice 38.
