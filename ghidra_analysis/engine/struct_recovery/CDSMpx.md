# CDSMpx

## Status

**PARTIAL** — allocation size `0x98c8` verified; libmad `stream`/`frame`/`synth` shells typed in Ghidra; I/O tail and PCM scratch proven. MI vptrs at `+0x04`/`+0x18` share bytes with `mad_stream.buffer` / `mad_stream.this_frame` during decode; restored only in `CDSMpx_dtor` (round 3 task 39). `mad_frame.body` interior still opaque.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDSMpx) == 0x98c8` | `0x00446b38` | `CreateFromHandle@0x00446b00` → `OperatorNewWithBadAlloc(0x98c8)` before `AttachBitstream` |
| Object ends at `+0x98c4` | `0x00446a00` | `AttachBitstream` writes `initFlag` at `+0x98bc`, `sampleFormatPacked` at `+0x98c0`; alloc is `+8` above last dword |
| `inputBuffer` span `0x4000` | `0x004466c0` | `RefillInputBuffer` memmove/read at `this+0x58bc`; `ResetDecoderState` passes `this+0x58bc` to `mad_stream_buffer` |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|-------|------|------|------|----------------------|
| 0x0000 | 4 | `void *` | `pVftable` | `AttachBitstream@0x00446a00` writes `CDSMpxDecoder` vtable; `CDSMpx_dtor@0x00432f40` restores `0x487318` |
| 0x0004 | 4 | `void *` | `pVftable_MI_audio` | `CDSMpx_dtor@0x00432f40` `param_1[1]=0x4872f4` (`face_8slots`); decode: `stream.buffer` (`CDSMpx_mad_stream_buffer@0x00456820`) |
| 0x0018 | 4 | `void *` | `pVftable_MI_event` | `CDSMpx_dtor@0x00432f40` `param_1[6]=0x4872e0` (`IDSEventHandler`); decode: `stream.this_frame` (`stream+0x14`, `mad_stream_buffer` `param_1[5]`) |
| 0x0020 | 4 | `void *` | `pHeldRef` | `CDSMpx_dtor@0x00432f40` `param_1[8]` → `vt[2]` release |
| 0x0004 | 0x40 | `mad_stream` | `stream` | `ResetDecoderState@0x004465e0` → `CDSMpx_InitMadDecoderFields`, `CDSMpx_mad_stream_buffer` |
| 0x0044 | 0x2434 | `mad_frame` | `frame` | `mad_frame_init(this+0x44)`; `ReadOneFrame(frame, stream)` |
| 0x2478 | 0x1008 | `mad_synth_bulanci` | `synth` | `mad_synth_init(this+0x2478)`; `mad_synth_init` writes `phase`/`offset` at `+0x1000`/`+0x1004` |
| 0x3480 | 2 | `ushort` | `numChannels` | `DecodeFrame@0x00446770` `*(short*)(this+0x3480)==2` stereo branch |
| 0x3482 | 2 | `ushort` | `samplesPerFrameMinusOne` | `DecodeFrame@0x00446770` loop bound from `*(ushort*)(this+0x3482)` |
| 0x3484 | 0x1200 | `int[1152]` | `channelL` | `DecodeFrame@0x00446770` iterates `this+0x3484` |
| 0x4684 | 0x1200 | `int[1152]` | `channelR` | `DecodeFrame@0x00446770` iterates `this+0x4684` |
| 0x5888 | 8 | `u64` | `streamBaseOffset` | `AttachBitstream@0x00446a00` Tell64 snapshot; `RefillInputBuffer@0x004466c0` seek addend |
| 0x5890 | 4 | `u32` | `inputCursor` | `AttachBitstream` zero; `RefillInputBuffer` increment; `ReinitDecoder@0x00446640` zero |
| 0x5894 | 4 | `u32` | `inputLimit` | `AttachBitstream` `= param_2`; `RefillInputBuffer` bounds read |
| 0x5898 | 4 | `CDSStreamStorage *` | `inputStream` | `AttachBitstream` assign/release; `RefillInputBuffer` vt read/seek |
| 0x589c | 0x10 | — | `outputQueue` | `AttachBitstream` `CDSMemQueue_InitDefault`; `ReadPCM@0x00446940` read; `CDSMpxDecoder_dtor@0x004468b0` dtor |
| 0x58ac | 4 | `u32` | `maxSamplePacketBytes` | `ReadPCM@0x00446940` caps dequeue size |
| 0x58bc | 0x4000 | `byte[16384]` | `inputBuffer` | `RefillInputBuffer` memmove/fill; `ResetDecoderState` stream buffer base |
| 0x98bc | 4 | `u32` | `initFlag` | `AttachBitstream` zero; `DecodeFrame` set on first frame |
| 0x98c0 | 4 | `u32` | `sampleFormatPacked` | `AttachBitstream` zero; `DecodeFrame` copy from `frame+0x14` (`this+0x58`) |

### MI vptr vs `mad_stream` overlay (round 3 task 39)

On the live `0x98c8` decoder (`CreateFromHandle` → `AttachBitstream`):

| Object `+off` | Dtor restores (MI) | While decoding (`mad_stream` field) |
|---------------|-------------------|-------------------------------------|
| `0x04` | `0x4872f4` (`face_8slots`) | `buffer` — written `CDSMpx_mad_stream_buffer`, `CDSMpx_InitMadDecoderFields` zero |
| `0x18` | `0x4872e0` (`IDSEventHandler`) | `this_frame` (`mad_stream+0x14`) — `CDSMpx_mad_stream_buffer` `param_1[5]`; `CDSMpx_dtor` `param_1[6]` |

`AttachBitstream@0x00446a00` sets only `+0x00` to the 3-slot `CDSMpxDecoder` table, then `ResetDecoderState` clobbers `+0x04`/`+0x18` before the first `DecodeFrame`. Program-wide stores of `0x4872f4` / `0x4872e0`: `CDSMpx_dtor` and `CDSMpx_InitPrimaryVtables@0x004469c0` (`OperatorNew(0x38)` class-registration shell — not the decoder alloc).

### `mad_stream` (+0x04, 0x40 bytes)

Ghidra type `mad_stream` (64 B). Libmad-canonical names in **Name**; Ghidra field id in **Ghidra** (MCP `modify_struct_field` rename on nested libmad types is a no-op — use offset map in decompiler).

| Off | Size | Name | Ghidra | Evidence |
|-----|------|------|--------|----------|
| 0x00 | 4 | `buffer` | `buffer` | `CDSMpx_mad_stream_buffer@0x00456820` |
| 0x04 | 4 | `bufend` | `bufend` | `CDSMpx_mad_stream_buffer`; `RefillInputBuffer@0x004466c0` (`bufend - next_frame` via `CDSMpx+8` / `+0x1c`) |
| 0x08 | 4 | `skiplen` | `dwSkiplen` | `CDSMpx_InitMadDecoderFields` zeros; libmad `mad_stream.skiplen` |
| 0x0c | 4 | `sync` | `dwSync` | `CDSMpx_mad_stream_buffer` `param_1[3]=1` |
| 0x10 | 4 | `freerate` | `dwFreerate` | init zero; libmad `mad_stream.freerate` |
| 0x14 | 4 | `this_frame` | `this_frame` | `CDSMpx_mad_stream_buffer` `param_1[5]`; MI `+0x18` overlay |
| 0x18 | 4 | `next_frame` | `next_frame` | `CDSMpx_mad_stream_buffer` `param_1[6]` |
| 0x1c | 8 | `main_bit` | `main_bit` | `mad_bit_init@0x00459650`; `mad_header_decode` `stream+0x1c` |
| 0x24 | 8 | `anc_bit` | `anc_bit` | `CDSMpx_InitMadDecoderFields@0x004567b0` second `mad_bit_init` |
| 0x2c | 4 | `aux` | `aux` | libmad `mad_stream.aux`; init zero only (`CDSMpx_InitMadDecoderFields` `param_1[0xb]`) |
| 0x30 | 4 | `aux_data` | `aux_data` | libmad `mad_stream.aux_data`; init zero only (`param_1[0xc]`) |
| 0x34 | 4 | — | `dwInit_zero_pad_34` | init zero only (`param_1[0xd]`); no non-init stores |
| 0x38 | 4 | — | `dwInit_zero_pad_38` | init zero only (`param_1[0xe]`); no non-init stores |
| 0x3c | 4 | `error` | `dwError` | `mad_header_decode` `0x101`..`0x104`; `DecodeFrame` `*(this+0x40)==1` |

### `mad_frame` (+0x44, 0x2434 bytes)

Ghidra `mad_frame` (9268 B): `header` + `pBody` (`body`).

| Off | Size | Name | Ghidra | Evidence |
|-----|------|------|--------|----------|
| 0x00 | 0x28 | `header` | `header` (`MpegAudioFrameInfo`) | `mad_header_init@0x00458ee0`; `mad_header_decode` |
| 0x14 | 4 | `dwSampleRateHz` | *(in `header`)* | `DecodeFrame` copies `*(this+0x58)` → `sampleFormatPacked` (`header+0x14`) |
| 0x28 | 9228 | `body` | `pBody` | `mad_frame_init` `param_1[0xb]`, `param_1[0x90c]`; layer decoders |

### `mad_synth_bulanci` (+0x2478, 0x1008 bytes)

Ghidra `mad_synth_bulanci` (4104 B): `pFilter` / `dwPhase` / `dwOffset`. `numChannels` / `samplesPerFrameMinusOne` live on `CDSMpx` @ `+0x3480` (alias `synth+0x1008` in `mad_synth_init`).

| Off | Size | Name | Ghidra | Evidence |
|-----|------|------|--------|----------|
| 0x0000 | 0x1000 | `filter` | `pFilter` | `mad_synth_mute` region |
| 0x1000 | 4 | `phase` | `dwPhase` | `mad_synth_init@0x00458eb0` `synth[0x400]` |
| 0x1004 | 4 | `offset` | `dwOffset` | `mad_synth_init` `synth[0x401]` |
| 0x1008 | 2 | `numChannels` | `CDSMpx.numChannels` | `mad_synth_init` `(short*)(synth+0x402)` |
| 0x100a | 2 | `samplesPerFrameMinusOne` | `CDSMpx.samplesPerFrameMinusOne` | `mad_synth_init` @ `this+0x3482` |

## Ghidra apply

**Slice 34 (2026-05-30):** `get_struct_layout CDSMpx` → **39112 (0x98c8)** — `mad_stream`/`mad_frame`/`mad_synth_bulanci` + I/O tail applied.

**Agent todo 37 r2 (2026-05-30):** Renamed `mad_stream.aux`/`aux_data`; `init_zero_pad_*` @ +0x34/+0x38 (zero-only proof); deleted orphan `mad_synth` (4108 B); decompiler comments @ `mad_frame_init@0x00459620` (body `+0x2430` / `param_1[0x90c]`) and `CDSMpx_InitMadDecoderFields@0x004567b0`.

**Agent todo 37 r3 (2026-05-30):** `recreate_struct` `mad_stream`/`mad_frame`/`mad_synth_bulanci`; re-linked `CDSMpx`/`CDSMpxDecoder` `stream`/`frame`/`synth` after recreate; decompiler comments @ `mad_frame_init@0x00459620`, `CDSMpx_InitMadDecoderFields@0x004567b0`, `mad_header_decode@0x00458f50`, `mad_synth_init@0x00458eb0`; orphan `mad_synth` absent; `save_program`. Ghidra `get_struct_layout` still auto-prefixes uint/pointer members (`dwSkiplen`, `pBody`, …) — libmad-canonical **Name** column below is authoritative for decompiler offset math.

```
delete_data_type mad_stream, mad_frame, mad_synth_bulanci
create_struct mad_stream (64 B), mad_frame (9268 B), mad_synth_bulanci (4104 B)
modify_struct_field CDSMpx/CDSMpxDecoder: stream, frame, synth → libmad types
get_struct_layout CDSMpx → 39112 (0x98c8)
save_program bulanci.exe
```

## Follow-up (round 3 task 39)

- **Proven:** `+0x04` / `+0x18` MI vptrs alias `mad_stream.buffer` / `mad_stream.this_frame` on the `0x98c8` object; restore only in `CDSMpx_dtor@0x00432f40`; `AttachBitstream` / `CDSMpxDecoder_dtor` do not rewrite them (see overlay table above).
- **Separate:** `CDSMpx_InitPrimaryVtables@0x004469c0` installs the three vptrs on a `0x38`-byte class-registration shell.

## UNK

- Virtual calls through `face_8slots` / `IDSEventHandler` on an active decoder after `AttachBitstream` (none in `DecodeFrame` / `ReadPCM` path).
- `mad_frame.body` layer-III granule/side-info interior (`param_1[0x90c]` @ `frame+0x2430`).
- Semantic name for dword at `frame+0x14` copied to `sampleFormatPacked` (may be header field, not a distinct sample-format slot).
- Gap `+0x5884..+0x5887` and `+0x58b0..+0x58bb` (no independent xrefs).
- `ResolveResource@0x00446b90` reads `+0x1c`/`+0x24`/`+0x28` — likely base-class resource path, not remapped here.
