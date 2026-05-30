# Round 5 — Worker 15 report

## Task

| Field | Value |
|-------|--------|
| **worker** | 15 / 50 |
| **mode** | WRITE |
| **scope** | `FUN_*` MSVCRT / runtime glue in `.text` **`0x00450000`–`0x00470000`** |
| **rule** | Document / label only when **game-reachable** (caller in `0x401000`–`0x446fff` or named CDSMpx / libmad / libjpeg cluster); **skip pure CRT** |
| **acceptance** | Proof table for in-scope `FUN_*`; Ghidra plate comments + proven renames; `save_program bulanci.exe` |

## Status

**DONE** — 38 game-reachable `FUN_*` annotated; 55 pure-CRT `FUN_*` skipped; 3 renames applied.

## Method

1. Enumerated all `FUN_*` with entry in `[0x450000, 0x470000)` via Ghidra MCP `list_functions_enhanced` (**93** symbols).
2. Classified each with transitive xref walk (`get_xrefs_to`): **game** if any caller is in the main game band **or** has a non-`FUN_` name in the embedded **libmad** / **libjpeg** / **CDSMpx** clusters (not `MSVCRT::`, `Unwind@`, `___*`, etc.).
3. **Skipped** symbols whose call graph stays inside CRT/SEH glue only.
4. Applied `set_plate_comment` on all 38 game-reachable targets; renamed three with direct, single-purpose proof.

## Address bands (within scope)

| Range (approx.) | Content | Worker action |
|-----------------|---------|----------------|
| `0x450000`–`0x454fff` | MSVC SEH / lock epilog thunks (`FUN_004500e2`, `FUN_00452869`, …) | **Skipped** — callers are `FUN_00449b1b` / CRT only |
| `0x456000`–`0x45dfff` | Embedded **libmad** (MPX audio) + helpers | **12** `FUN_*` annotated; 2 renamed |
| `0x45d000`–`0x46ffff` | Embedded **libjpeg** (CDSJpegImage path) | **26** `FUN_*` plate comments |
| `0x470000`–`0x470000` | (end exclusive) EH unwind thunks start at `0x470000` | Out of slice |

Overlap with [MODULES.md](../../../docs/MODULES.md) “CRT + ATL” band is expected: linker placed **mad** / **jpeg** object files in the same `.obj` region as MSVCRT.

## Game-reachable `FUN_*` (annotated)

| Address | Ghidra name (after) | Game entry / caller proof | Role (plate) |
|---------|---------------------|---------------------------|--------------|
| `0x00456800` | `CDSMpx_FreeMadAuxAlloc` | `CDSMpx_FinishMadFrame@0x00446630` | Free aux buffer at `+0x30` after `mad_frame_finish` |
| `0x0045a220` | `III_huffdec_fixed_mul` | `III_huffdecode@0x0045a2c0` (8 call sites) | libmad fixed-point magnitude helper |
| `0x0045a9d0` | `FUN_0045a9d0` | `III_decode@0x0045bde6` | III_decode granule helper (block 0/1) |
| `0x0045ab00` | `FUN_0045ab00` | `III_decode@0x0045bd49` | III_decode granule helper (block 2) |
| `0x0045b530` | `FUN_0045b530` | `III_decode@0x0045be8b`, `0x0045bf56` | Stereo alias-reduction path |
| `0x0045ba90` | `FUN_0045ba90` | `III_decode@0x0045be48` … | Mid/side stereo path |
| `0x0045bb40` | `FUN_0045bb40` | `III_decode@0x0045bec3` … | Intensity stereo path |
| `0x0045d030` | `CDSMpx_MadTimerAccumulate` | `SeekToValidFrame@0x004593f0` | MAD timer / sample-rate accumulator |
| `0x0045dd20` | `FUN_0045dd20` | `save_marker@0x0045df26` | libjpeg marker writer helper |
| `0x0045ed80` | `FUN_0045ed80` | `jpeg_start_compress@0x0045ef10` | start_compress helper |
| `0x0045f1b0` | `FUN_0045f1b0` | `jpeg_set_quality@0x0045f248` | set_quality helper |
| `0x0045f7f0` | `FUN_0045f7f0` | `jinit_d_coef_controller`, `jinit_d_main_controller`, `jinit_upsampler` | Controller sizing helper |
| `0x0045f880` | `FUN_0045f880` | `jpeg_decompress_data`, `jpeg_compress_data`, `compress_first_pass` | Row-buffer alloc helper |
| `0x004604d0` | `FUN_004604d0` | `jpeg_start_decompress@0x0045ec1f` | start_decompress helper |
| `0x00460ad0` | `FUN_00460ad0` | `jinit_marker_writer@0x00460d7a` | marker writer helper |
| `0x00461540` | `FUN_00461540` | `jpeg_decompress_data@0x004617a9` | decompress MCU sizing |
| `0x004619e0` | `FUN_004619e0` | `jinit_d_coef_controller@0x00462431` | d_coef controller helper |
| `0x00462980` | `FUN_00462980` | `decode_mcu@0x00462a22` | MCU bit-buffer helper |
| `0x00465180` | `FUN_00465180` | `jinit_color_deconverter@0x00465563` | color deconverter helper |
| `0x004665c0` | `FUN_004665c0` | `jinit_2pass_quantizer@0x0046690a` | 2-pass quantizer pass 1 |
| `0x004666a0` | `FUN_004666a0` | `jinit_2pass_quantizer@0x004667ed` | 2-pass quantizer pass 2 |
| `0x00467600` | `FUN_00467600` | `jinit_compress_master@0x00460e51` | compress_master helper |
| `0x00467690` | `FUN_00467690` | `jpeg_compress_data@0x0046771d` | compress_data buffer helper |
| `0x00467930` | `FUN_00467930` | `compress_first_pass@0x00467cfd` | first-pass helper |
| `0x00467dc0` | `FUN_00467dc0` | `jinit_compress_master@0x00460e49` | compress_master variant |
| `0x004680f0` | `FUN_004680f0` | `emit_bits_s@0x00468188` | short emit_bits helper |
| `0x00468be0` | `FUN_00468be0` | `jinit_huff_encoder@0x00468d89` | huff encoder init helper |
| `0x00468dd0` | `FUN_00468dd0` | `emit_bits_e`, `emit_restart` | emit_bits / restart helper |
| `0x00468ed0` | `FUN_00468ed0` | `emit_restart@0x00468fe4` | restart wrapper |
| `0x00468f20` | `FUN_00468f20` | `emit_eobrun@0x00468fb8` | EOB run helper |
| `0x00469060` | `FUN_00469060` | `start_pass_huff_decoder@0x004697a6` | huff decoder pass helper |
| `0x004696a0` | `FUN_004696a0` | `start_pass_huff_decoder@0x004697ea` | huff decoder AC helper |
| `0x00469900` | `FUN_00469900` | `jinit_compress_master@0x00460e1d` | compress_master variant |
| `0x0046a840` | `FUN_0046a840` | `compress_output@0x0046ab30` | compress_output alloc |
| `0x0046acf0` | `FUN_0046acf0` | `jinit_compress_master@0x00460de8` | compress_master variant |
| `0x0046b590` | `FUN_0046b590` | `jinit_compress_master@0x00460de0` | compress_master variant |
| `0x0046c8f0` | `FUN_0046c8f0` | `jinit_compress_master@0x00460dc8` | compress_master variant |
| `0x0046db50` | `FUN_0046db50` | `start_pass_dcolor@0x00463c22` | dcolor pass helper |

**Internal cluster only (not in table):** `FUN_0045b480@0x0045b480` — sole caller `FUN_0045b530@0x0045b538`; reachable from `III_decode` via `FUN_0045b530` but left unlabeled (no unique semantic proof).

## Skipped pure CRT / SEH `FUN_*` (sample)

| Address | Evidence |
|---------|----------|
| `0x004500e2` | SEH epilog; calls `FUN_00449b1b(0)` (`__unlock`) only |
| `0x0045011e` | No game/jpeg/mad named callers |
| `0x00452869`, `0x00452f0b`, `0x00452fb7`, `0x00454ace` | CRT-only xref closure |
| `0x0045b480` | Internal to `FUN_0045b530` only (see above) |
| `0x0045d1d0` | `jpeg_destroy` cluster only via `alloc_small` — CRT heap shim |
| … | **55 total** — full list omitted; all lack game-band or mad/jpeg named entry |

Already-renamed **non-`FUN_*`** in the same band (out of scope but adjacent): `mad_stream_sync@0x456850`, `jpeg_fdct_islow@0x45b7a0`, `CDSMpx::mad_layer_III@0x45c100`, etc. — see [mpx_audio_format.md](../../formats/mpx_audio_format.md) and `config/bulanci/mapping.csv`.

## Ghidra deltas

1. **`set_plate_comment`** — 38 game-reachable `FUN_*` (see table).
2. **`rename_function_by_address`**
   - `0x00456800` → `CDSMpx_FreeMadAuxAlloc`
   - `0x0045d030` → `CDSMpx_MadTimerAccumulate`
   - `0x0045a220` → `III_huffdec_fixed_mul` (was erroneous `III_requantize` in DB)
3. **`save_program bulanci.exe`**

## Struct doc updates

None — scope is freestanding codec/CRT glue, not class layout.

## Remaining UNK

- **35** game-reachable helpers still carry `FUN_*` names — need deeper libmad/libjpeg symbol alignment before rename.
- `FUN_0045b480` — sub-helper inside `FUN_0045b530`; defer until `FUN_0045b530` is named.
- EH **`Unwind@*`** thunks at `0x470000+` are out of slice (worker 15 ends at `0x470000`).

## Cross-refs

- [mpx_audio_format.md](../../formats/mpx_audio_format.md) — libmad proof for `0x456850`–`0x459xxx` band
- [MODULES.md](../../../docs/MODULES.md) — `.text` band map
