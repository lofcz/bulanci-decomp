# Struct recovery — batch 34 follow-up (round 2)

**Prior:** `batch_34_summary.md` (`CDSMpx`, `CDSMpxDecoder`)  
**Status:** **FOLLOWUP_DONE** (partial UNK remain)

## Actions taken

### 1. libmad sub-structs (Ghidra)

Created and wired into `CDSMpx`:

| Type | Size | Notes |
|------|-----:|-------|
| `mad_bitptr` | 8 | `mad_bit_init@0x00459650` |
| `mad_stream` | 64 | `buffer`/`bufend`/`sync`/`this_frame`/`next_frame`/`main_bit`/`anc_bit`/`error` + `unk_*` pads |
| `mad_frame` | 9268 | `MpegAudioFrameInfo header` + `body[9228]` |
| `mad_synth_bulanci` | 4104 | `filter[4096]`, `phase`, `offset` |

`get_struct_layout CDSMpx` → **39112** bytes (`0x98c8`); fields `stream`, `frame`, `synth` typed (was `byte[]` blobs).

`save_program bulanci.exe` — once.

### 2. MI vtable slots vs decode overlay

**Proven** in `CDSMpx_dtor@0x00432f40`:

- `+0x00` → `0x487318` (primary `CDSMpx` / `IDSReferenced` restore)
- `+0x04` → `0x4872f4` (second MI — 8-slot audio interface)
- `+0x18` → `0x4872e0` (third MI)

`AttachBitstream@0x00446a00` sets only `+0x00` to `CDSMpxDecoder` 3-slot vtable; `mad_stream` at `+0x04` occupies the same bytes as the `+0x04` MI pointer during active decode.

### 3. Docs updated

- `CDSMpx.md` — nested `mad_stream` / `mad_frame` / `mad_synth_bulanci` tables; MI rows; corrected `sampleFormatPacked` source (`frame+0x14` / `this+0x58`).
- `CDSMpxDecoder.md` — MI follow-up note.

## Ghidra deltas

- New types: `mad_bitptr`, `mad_stream`, `mad_frame`, `mad_synth_bulanci` (also orphan `mad_synth` 4108 B from first attempt — unused).
- `CDSMpx`: `stream`→`mad_stream`, `frame`→`mad_frame`, `synth`→`mad_synth_bulanci`.

## Agent todo 37 (2026-05-30)

- Recreated `mad_stream` (64 B), `mad_frame` (9268 B), `mad_synth_bulanci` (4104 B); re-linked `CDSMpx` / `CDSMpxDecoder` `stream` / `frame` / `synth`.
- Documented libmad canonical names vs Ghidra `dw*` ids (`skiplen`/`sync`/`freerate`/`error`, `body`, `filter`/`phase`/`offset`) in `CDSMpx.md`.
- `save_program bulanci.exe`.

## Remaining UNK

| Item | Notes |
|------|-------|
| `mad_stream` `+0x2c`..`+0x38` | Zeroed in init; Ghidra `unk_*` / `dwUnk_*` |
| `mad_stream` `+0x08`, `+0x10` | Mapped to libmad `skiplen` / `freerate` in docs; Ghidra still `dwSkiplen` / `dwFreerate` |
| `mad_frame.body` | Layer III granule/side-info interior (`param_1[0x90c]` @ `+0x2430`) |
| `frame+0x14` semantic | Copied to `sampleFormatPacked`; may be `MpegAudioFrameInfo` field, not a separate format dword |
| `CDSMpxStream` container header | **Batch 35** scope (`batch_34_summary.md` follow-up #3) |
| `ResolveResource` `+0x1c`/`+0x24`/`+0x28` | Base-class resource path, not decoder body |

## Evidence anchors (new / reinforced)

| Func | Addr | Finding |
|------|------|---------|
| `CDSMpx_InitMadDecoderFields` | `0x004567b0` | 16×`dword` zero + dual `mad_bit_init` |
| `CDSMpx_mad_stream_buffer` | `0x00456820` | `buffer`/`bufend`/`this_frame`/`next_frame` |
| `mad_header_decode` | `0x00458f50` | `stream+0x1c` bitptr; errors at `stream+0x3c` |
| `mad_synth_init` | `0x00458eb0` | `phase`/`offset`; init aliases `numChannels` @ `this+0x3480` |
| `CDSMpx_dtor` | `0x00432f40` | MI vtables `0x4872f4` / `0x4872e0` |
