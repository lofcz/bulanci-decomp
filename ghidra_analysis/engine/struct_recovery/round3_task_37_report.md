# Round 3 — task 37 report

## Task

| Field | Value |
|-------|--------|
| **id** | 37 |
| **title** | mad_stream/mad_frame libmad Ghidra rename pass |
| **one_liner** | Recreate libmad nested types with canonical field layout; document zero-only stream tail dwords and `mad_frame.body` interior; re-link `CDSMpx` decoder embeds. |
| **acceptance** | Update CDSMpx.md; Ghidra mutation if evidence supports |

**Types:** `CDSMpx`, `CDSMpxDecoder`, `mad_stream`, `mad_frame`, `mad_synth_bulanci`

## Status

**DONE** — libmad shells rebuilt (64 / 9268 / 4104 B); `CDSMpx`/`CDSMpxDecoder` `stream`/`frame`/`synth` retyped; decompiler offset comments at init/decode sites; orphan `mad_synth` (4108 B) absent.

## Evidence

| Claim | Func @ addr | Finding |
|-------|-------------|---------|
| `mad_stream` base `this+4` | `ResetDecoderState@0x004465e0` | `CDSMpx_InitMadDecoderFields(param_1+4)`; `CDSMpx_mad_stream_buffer(param_1+4, …)` |
| Zero-only `skiplen`/`freerate`/`aux`/`aux_data`/pads | `CDSMpx_InitMadDecoderFields@0x004567b0` | `param_1[2..6]`, `[0xb..0xf]` cleared; no non-init stores in decode path |
| `mad_stream.error` @ `+0x3c` | `mad_header_decode@0x00458f50` | Writes `0x101`..`0x104` at `bitstream+0x3c` |
| Decode polls `stream.error` | `DecodeFrame@0x00446770` | `*(param_1+0x40)==1` before `RefillInputBuffer` retry (`object+4` + `0x3c`) |
| `mad_frame.body` tail dword | `mad_frame_init@0x00459620` | `param_1[0xb]=0`; `param_1[0x90c]=0` → `frame+0x28` body base, last dword `+0x2430` |
| `mad_synth_bulanci` phase/offset | `mad_synth_init@0x00458eb0` | `synth[0x400]` / `synth[0x401]`; channel fields alias `CDSMpx+0x3480` |
| `CDSMpx` alloc size | `CreateFromHandle@0x00446b00` | `OperatorNewWithBadAlloc(0x98c8)`; `get_struct_layout CDSMpx` → 39112 |

### `mad_stream` libmad vs Ghidra id (object-relative `CDSMpx+4`)

| Off | libmad **Name** | `get_struct_layout` id | Evidence |
|-----|-----------------|------------------------|----------|
| 0x08 | `skiplen` | `dwSkiplen` | init zero only |
| 0x0c | `sync` | `dwSync` | `CDSMpx_mad_stream_buffer` `param_1[3]=1` |
| 0x10 | `freerate` | `dwFreerate` | init zero only |
| 0x2c | `aux` | `aux` | init `param_1[0xb]` |
| 0x30 | `aux_data` | `aux_data` | init `param_1[0xc]` |
| 0x34 | `init_zero_pad_34` | `dwInit_zero_pad_34` | init `param_1[0xd]` only |
| 0x38 | `init_zero_pad_38` | `dwInit_zero_pad_38` | init `param_1[0xe]` only |
| 0x3c | `error` | `dwError` | `mad_header_decode`; `DecodeFrame` `+0x40` |

### `mad_frame` / `mad_synth_bulanci`

| Off | libmad **Name** | Ghidra id | Evidence |
|-----|-----------------|-----------|----------|
| `frame+0x28` | `body` | `pBody` | `mad_frame_init` `param_1[0xb]`; 9228 B |
| `frame+0x2430` | body tail | `param_1[0x90c]` | last 4 B of 9268 B struct |
| `synth+0` | `filter` | `pFilter` | `mad_synth_mute` region |
| `synth+0x1000` | `phase` | `dwPhase` | `mad_synth_init` |
| `synth+0x1004` | `offset` | `dwOffset` | `mad_synth_init` |

## Ghidra deltas

- `recreate_struct` `mad_stream` (64 B, 14 fields), `mad_frame` (9268 B), `mad_synth_bulanci` (4104 B) — `force=true`
- `modify_struct_field` `CDSMpx` / `CDSMpxDecoder`: `stream`→`mad_stream`, `frame`→`mad_frame`, `synth`→`mad_synth_bulanci` (fixes `-BAD-` after recreate)
- Decompiler comments: `mad_frame_init@0x00459620`, `CDSMpx_InitMadDecoderFields@0x004567b0`, `mad_header_decode@0x00458f50`, `mad_synth_init@0x00458eb0`, `CDSMpx_mad_stream_buffer@0x00456820`
- `delete_data_type mad_synth` — not present (orphan already removed in r2)
- `save_program bulanci.exe`

## Struct doc updates

- `CDSMpx.md` — r3 apply note; UNK trimmed (`mad_stream` dword band documented in nested table)

## Remaining UNK

- Ghidra auto-prefix on nested libmad members (`dw*`/`p*`) — MCP `modify_struct_field` / `recreate_struct` field names do not stick; use **Name** column + decompiler comments for canonical libmad ids.
- `mad_frame.body` layer-III interior (granule/side-info) — size proven (`9228` B, tail `+0x2430`); semantic fields not recovered.
