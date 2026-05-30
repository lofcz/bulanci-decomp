# Round 3 — task 38 report

## Task

| Field | Value |
|-------|--------|
| **id** | 38 |
| **title** | CDSMpxStream persist band vs live mad_stream |
| **one_liner** | Reconcile `mpxFormatTail@+0x08` / `SaveMpxFile`/`LoadMpxFile` adjustor `primary+0x38` with live `mad_stream` at runtime. |
| **acceptance** | `CDSMpxStream.md` documents which persistence fields alias decoder memory vs file header only; plate on `SaveMpxFile@0x00432eb0` |
| **structs** | `CDSMpxStream`, `CDSMpx`, `CDSQueueStream` |
| **addresses** | `0x00432eb0`, `0x00433180`, `0x00446b00`, `0x00446a00`, `0x004290c0` |

## Status

**DONE** — persist band `P+0x08..+0x30` is the same byte footprint as the head of `mad_stream` (`P+0x04`) plus I/O tail dwords; Save/Load and `CreateFromHandle` use persist semantics; `AttachBitstream`/`ResetDecoderState` repurpose those bytes as libmad state.

## Evidence

| Claim | Func @ addr | Finding |
|-------|-------------|---------|
| Facet `this = P+0x38` | `SaveMpxFile@0x00432eb0`, `LoadMpxFile@0x00433180` | `ECX` = persist `IDSChained`; disasm `LEA [ESI-0x30]`→`P+0x08`, `[ESI-0x8]`→`P+0x30`, `[ESI-0x18]`→`P+0x20`, `[ESI-0x10/0xc]`→`P+0x28/+0x2c` |
| 12-byte header tail | `SaveMpxFile@0x00432eb0` | `PUSH 0xc` + `LEA [ESI-0x30]` — writes `mpxFormatTail` (`MpxFileHeader` bytes 4..15) |
| `dwDataSize` separate dword | `SaveMpxFile@0x00432eb0` | `LEA EBX,[ESI-0x8]` + `PUSH 4` — `dwPayloadBytes@P+0x30` |
| Load mirrors Save | `LoadMpxFile@0x00433180` | Same adjustors; `Tell64` → `[EDI-0x10]`/`[EDI-0xc]`; stream stored at `[EDI-0x18]` |
| `mad_stream` base | `ResetDecoderState@0x004465e0` | `param_1+4` — stream at `P+0x04` on decoder (`CDSMpx.md` task 39) |
| `mpxFormatTail` byte span | layout | 12 B @ `P+0x08..+0x13` = `mad_stream+0x04..+0x0f` = `bufend`+`skiplen`+`sync` (not `freerate`/`this_frame`) |
| Persist band before decode | `CreateFromHandle@0x00446b00` | `Seek64` on `*(handle+0x1c)` with `+0x24/+0x28`; `AttachBitstream(..., *(+0x2c))` — `handle` at `P+4` ⇒ `pPayloadStream@+0x20`, `payloadStart*@+0x28/+0x2c`, `dwPayloadBytes@+0x30` |
| Decode clobbers stream head | `AttachBitstream@0x00446a00` | Primary vtable only; `ResetDecoderState` → `mad_stream_buffer` overwrites `buffer`/`this_frame` (task 39) |
| On-disk layout | `mpx_audio_format.md` | `MpxFileHeader`: `dwDataSize` + 12-byte WAVEFORMAT tail + MPEG bitstream |

### Semantic vs physical storage

| Phase | `P+0x08..+0x13` meaning | `P+0x20..+0x30` meaning |
|-------|-------------------------|-------------------------|
| After `LoadMpxFile` / before `AttachBitstream` | Saved format descriptor (`wChannels`, `wBitsPerSample`, rates) | Stream handle, 64-bit payload offset, byte count |
| During `DecodeFrame` | Live `mad_stream.bufend` / `skiplen` / `sync` | `main_bit` / `anc_bit` / `aux` bytes (libmad) |
| `SaveMpxFile` | Writes 12-byte file tail (not libmad pointers) | Reads persist fields for seek + copy |

## Ghidra deltas

- `MpxFormatTail_persist` (12 B) on `CDSMpxStream.mpxFormatTail@+0x08`
- `CDSMpxPersistFacet` (56 B) + `SaveMpxFile`/`LoadMpxFile` prototypes (`__thiscall`, facet ECX)
- Plate / decompiler comments @ `0x00432eb0`, `0x00433180` (persist band vs decoder RAM)
- `save_program bulanci.exe` (prior worker r3-38; verified this pass)

**Note:** Decompiler may still show `CDSMpxStream *this` on Save/Load; plates document `CDSMpxPersistFacet *` @ `primary+0x38`.

## Struct doc updates

- [CDSMpxStream.md](./CDSMpxStream.md) — persist-band vs `mad_stream` table; `mpxFormatTail` typed `MpxFormatTail_persist`; UNK on overlap resolved

## Remaining UNK

- Registry shell `0x48` vs `CreateFromHandle` `0x98c8` — same allocation or not in live game.
- Sub-byte WAVEFORMAT ↔ dword alias inside `mpxFormatTail` (dword alignment and Save/Load size proven only).
- `pSubObjStash@+0x3c` pointed-to type.
- Ghidra `CDSMpxStream` layout listing offset drift (`dwPayloadStart*` listed @ `+0x2c` in `get_struct_layout` vs doc `+0x28` — follow disasm `this-0x10`).
