# Struct recovery batch 35/50

**Index:** 35 (`batches_50.json`)  
**Structs:** `CDSMpxStream`, `CDSQueueStream`  
**Program:** `bulanci.exe` (saved once at end)

## Results

| Struct | Status | Size | Ghidra |
|--------|--------|------|--------|
| `CDSQueueStream` | PARTIAL | `0x24` | Applied — 9 fields, 36 bytes |
| `CDSMpxStream` | PARTIAL | `0x98c8` | Applied — `body` = `CDSMpx` (39112 bytes) |

## Evidence highlights

- **CDSQueueStream** — `IDSStream` at outer `+0x04` (unique among streams). `ReadBytes` / `WriteBytes` / `SeekPosition` / `SetStreamSize` pin `pMemQueue`, `usedBytes`, and `cursor`. `CloseStream` detaches queue via `FUN_0043be90` and sets closed state `0x20`. DSM handles allocate `0x30` bytes with queue initialized at `handle+0x0c` (`HandleAcquireResource@0x004290c0`).
- **CDSMpxStream** — Thin RTTI wrapper over `CDSMpx`: factory `CreateFromHandle@0x00446b00` allocates `0x98c8` and calls `AttachBitstream`. `SaveMpxFile` / `LoadMpxFile` persist the 4+12 byte MPx header plus bitstream payload (`mpx_audio_format.md`). Dtor tears down `CDSMpx` and `FUN_00434250(this+0x3c)`.

## Deliverables

- `CDSMpxStream.md`
- `CDSQueueStream.md`
- `batch_35_summary.md` (this file)

## Slice 35 agent (2026-05-30)

| Action | Detail |
|--------|--------|
| `CDSQueueStream` | `pMemQueue` typed `CDSMemQueue *`; `FUN_0043be90` → `CDSQueueStream_DetachQueue`; IDSStream `+4` decompiler note |
| `CDSMpxStreamRegistry` | New Ghidra struct `0x44` for factory @ `0x00433110` vs `0x98c8` decode path |
| Prototypes | `CDSQueueStream_CreateObject` / `CDSMpxStream_CreateObject` return typed pointers |
| Docs | Function leaf tables; registry shell table in `CDSMpxStream.md` |

## Agent todo 38 (2026-05-30)

| Action | Detail |
|--------|--------|
| `CDSMpxStream` | Rebuilt **39112 B** with named persistence band `+0x08..+0x3c` + decoder tail (`frame` @ `+0x44` …) |
| `CDSMpxPersistFacet` | **0x38** facet struct for `IDSChained` persist vtable @ `+0x38` |
| Prototypes | `SaveMpxFile` / `LoadMpxFile` (`CDSMpxPersistFacet *`); `CDSMpx_CreateFromHandle(int ctx_at_face8)` |
| Registry | `CDSMpxStreamRegistry` pad fields renamed (`pad_face8_to_event`, `pad_event_to_chained`) |

## Follow-ups

- `CDSDsmFile` embedded queue view vs standalone `CDSQueueStream` field overlay.
- `CDSMpxStream` `mpxFormatTail@+0x08` vs live `mad_stream` at runtime (overlay documented; Ghidra layout uses persist names, not `mad_stream` @ `+4`).
- `SaveMpxFile` / `LoadMpxFile` decompiler `this` type (ECX retype API limitation).
