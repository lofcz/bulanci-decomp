# Round 4 — agent todo 37 report (R4 worker)

## Task

| Field | Value |
|-------|-------|
| **id** | 37 |
| **title** | Manual libmad member renames; mad_frame.body interior map |
| **source** | `agent_todos_50_r4.json` / `todos_gather_r4_3.json` (blocker; supersedes R3 todo 37) |
| **types** | `CDSMpx`, `CDSMpxDecoder`, `mad_stream`, `mad_frame`, `mad_synth_bulanci` |
| **addresses** | `0x004567b0`, `0x00456820`, `0x00458f50`, `0x00459620`, `0x00458eb0`, `0x00432f40`, `0x0045c100`, `0x0045c4e0` |

## Status

**DONE** — `mad_frame` split into `header` / `options` (+pad) / `sbsample[2304]` / `overlap` pointer; layer decoders xref `frame+0x30` and `frame+0x2430`. `mad_stream`/`mad_synth_bulanci` field renames attempted via `modify_struct_field` + `recreate_struct`; `get_struct_layout` still reports `dw*`/`p*` on some uint/array members (Ghidra auto-prefix; decompiler comments carry libmad names).

## Evidence

| Claim | func@addr | Finding |
|-------|-----------|---------|
| `mad_frame` interior | `get_struct_layout mad_frame` | 40 B `header`, `options` @+0x28, `options_pad` @+0x2c, `int[2304] sbsample` @+0x30 (9216 B), `overlap` pointer @+0x2430 |
| Overlap alloc | `mad_layer_III@0x0045c100` | `*(frame+0x2430)==0` → `calloc(0x480,4)` (4608 B = `2×32×18` `mad_fixed_t`) |
| Overlap init | `mad_frame_init@0x00459620` | `param_1[0x90c]=0` nulls overlap pointer before first L3 decode |
| L1 sbsample writes | `mad_layer_I@0x0045c4e0` | Stores at `frame+0x30 + (granule+subband)*4`, stride `0x480` per channel |
| L3 uses header fields | `mad_layer_III@0x0045c100` | `*(frame+4)` channel mode; CRC @ `frame+0x18`/`+0x1a`; flags @ `frame+0x1c`/`+0x20`/`+0x2c` (within `MpegAudioFrameInfo`) |
| Stream tail zeros | `CDSMpx_InitMadDecoderFields@0x004567b0` | `skiplen`/`freerate`/`aux`/`init_zero_pad_*` cleared only |
| `CDSMpx` size | `get_struct_layout CDSMpx` | 39112 (0x98c8); `stream`@+4, `frame`@+68, `synth`@+9336 |

### `mad_frame` layout (Ghidra, post-R4)

| Off | Size | libmad **Name** | Ghidra id | Evidence |
|-----|------|-----------------|-----------|----------|
| 0x00 | 40 | `header` | `header` | `MpegAudioFrameInfo` |
| 0x28 | 4 | `options` | `dwOptions` | L3 ORs sideinfo flags into `*(frame+0x1c)` band (header `dwFlags1`) |
| 0x2c | 4 | — | `dwOptions_pad` | Padding before `sbsample` base |
| 0x30 | 9216 | `sbsample` | `pSbsample` | `mad_layer_I`/`II`/`III` fixed-point subband samples |
| 0x2430 | 4 | `overlap` | `overlap` | L3 heap buffer pointer (not a zero-only tail dword) |

### `mad_stream` / `mad_synth_bulanci` renames

| Struct | MCP action | `get_struct_layout` after save |
|--------|------------|--------------------------------|
| `mad_stream` | `recreate_struct` + `modify_struct_field` on `dwSkiplen`…`dwError` | Still `dwSkiplen`, `dwSync`, … (14 fields, 64 B) |
| `mad_synth_bulanci` | `recreate_struct` + rename `pFilter`/`dwPhase`/`dwOffset` | Still `pFilter`, `dwPhase`, `dwOffset` |

Canonical names documented in decompiler comments @ `CDSMpx_InitMadDecoderFields@0x004567b0`, `mad_synth_init@0x00458eb0` (unchanged from R3).

## Ghidra deltas

- `recreate_struct` `mad_frame` (9268 B): `header`, `options`, `options_pad`, `sbsample int[2304]`, `overlap` pointer @+0x2430
- `recreate_struct` `mad_stream` (64 B), `mad_synth_bulanci` (4104 B) with libmad field names in JSON
- `modify_struct_field` `CDSMpx`: re-link `stream`/`frame`/`synth`; name embeds @ offset +4/+68/+9336
- `set_decompiler_comment` @ `mad_frame_init@0x00459620`, `mad_layer_III@0x0045c100`, `mad_layer_I@0x0045c4e0`
- `get_struct_layout` `mad_frame`, `mad_stream`, `CDSMpx`
- `save_program bulanci.exe`

## Struct doc updates

- [CDSMpx.md](./CDSMpx.md) — R4 `mad_frame` interior table; overlap pointer; UNK trimmed

## Remaining UNK

- Ghidra `get_struct_layout` auto-prefix on `mad_stream` uints and `mad_synth_bulanci` `filter`/`phase`/`offset` (MCP rename reports success but layout export unchanged).
- Layer-III Huffman/granule **sub-structure** inside `sbsample` scratch and `III_decode` locals — not split into nested structs (out of scope).
- `mad_frame.options` vs `MpegAudioFrameInfo.dwFlags1` @+0x1c alias semantics during L3 decode (flags ORed in place).
