# Struct recovery batch 34/50

**Structs:** `CDSMpx`, `CDSMpxDecoder`  
**Program:** `bulanci.exe`  
**Evidence:** Ghidra MCP decompile + `OperatorNew` / ctor / decode-loop xrefs only

## Results

| Struct | Status | Size | Proven fields |
|--------|--------|------|---------------|
| `CDSMpx` | PARTIAL | `0x98c8` | libmad state blobs + I/O tail (`+0x5888`..`+0x98c0`); PCM scratch `channelL/R` at `+0x3484`/`+0x4684` |
| `CDSMpxDecoder` | PARTIAL | `0x98c8` | Same allocation as `CDSMpx`; `IDSReferenced` vtable face at `+0x00` only |

## Key evidence anchors

- `CreateFromHandle` @ `0x00446b00` — sole alloc: `OperatorNewWithBadAlloc(0x98c8)` @ `0x00446b38`.
- `AttachBitstream` @ `0x00446a00` — wires `inputStream`/`streamBaseOffset`/`outputQueue`; installs `CDSMpxDecoder` vtable at `+0x00`.
- `ResetDecoderState` @ `0x004465e0` — `mad_stream` @ `+0x04`, `mad_frame` @ `+0x44`, `mad_synth` @ `+0x2478`.
- `DecodeFrame` @ `0x00446770` — `numChannels`/`samplesPerFrameMinusOne`, stereo/mono PCM enqueue from `+0x3484`/`+0x4684`.
- `RefillInputBuffer` @ `0x004466c0` — 16 KiB `inputBuffer` @ `+0x58bc`; seek/read via `+0x5898`.
- `CDSMpxDecoder_dtor` @ `0x004468b0` — no separate alloc; tears down `+0x5898`/`+0x589c` on full decoder object.

## Ghidra actions

- [x] `create_struct` / inline replace `CDSMpx` (size `0x98c8`)
- [x] `create_struct` `CDSMpxDecoder` (size `0x98c8`, alias body)
- [x] `get_struct_layout` verified size > 1 for both (`39112` bytes)
- [x] `save_program bulanci.exe`

## Follow-ups

- Per-field libmad map inside `mad_stream`/`mad_frame`/`mad_synth` blobs (see `mpx_audio_format.md` §2).
- MI vtable slots at `+0x04` / `+0x18` vs live decoder overlay (`CDSMpx_dtor@0x00432f40`).
- `CDSMpxStream` (batch index 35) for container header fields that feed `AttachBitstream`.
