# Struct recovery batch 35 follow-up (35/50)

**Status:** `FOLLOWUP_DONE`  
**Prior:** `batch_35_summary.md`  
**Structs:** `CDSQueueStream`, `CDSMpxStream`

## Actions

| Follow-up | Result |
|-----------|--------|
| Reconcile `CDSQueueStream` `0x24` vs `0x20` DSM embed | **Done** — standalone layout is **0x24** (fields through `dwCursor` at `+0x1c`, 4-byte tail at `+0x20` without Read/Seek/Set xrefs). `CDSDsmFile::HandleAcquireResource@0x004290c0` inits **`CDSMemQueue` at `handle+0x0c`** over **`0x20` bytes** before `chunkCounter` at `+0x2c` (not a full `CDSQueueStream` ctor). Documented in `CDSQueueStream.md`. |
| Map `SaveMpxFile` / `LoadMpxFile` adjustor offsets | **Done** — disasm @ `0x00432eb0` / `0x00433180` with `this = primary+0x38` (`IDSChained` vtable `0x00487340`). Absolute fields: `dwPayloadBytes@+0x30`, `mpxFormatTail@+0x08`, `pPayloadStream@+0x20`, `payloadStartLo/Hi@+0x28/+0x2c`. MI table added to `CDSMpxStream.md`. |
| Avoid duplicating libmad in `CDSMpxStream` | **Done** — persistence fields reference `CDSMpx.md` for decoder interior only. |

## Ghidra deltas

- `CDSQueueStream` — recreated with corrected field names (`dwUsedBytes` @ `0x18`, `dwCursor` @ `0x1c`, etc.); size still **36**.
- Plate comments on `SaveMpxFile@0x00432eb0`, `LoadMpxFile@0x00433180`.
- `save_program bulanci.exe` (once).

## Remaining UNK

- `CDSQueueStream` standalone dword at `+0x20` purpose.
- `CDSMpxStream` `mpxFormatTail@+0x08` vs live `mad_stream` overlay at runtime.
- `CDSMpxStream` `pSubObjStash` pointed-to type (facet @ `+0x3c` / stash @ `+0x40` resolved — **round-3 task 28**).
