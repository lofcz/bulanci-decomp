# Round 6 — Dispatch-band codec apply (2026-06-03)

Evidence-only IJG/libmad renames from logic workers **39–49** (`0x0045a000`–`0x00468fff`).

## Script

[`../_apply_r6_dispatch_codec.py`](../_apply_r6_dispatch_codec.py) — HTTP `user-ghidra-mcp` @ `127.0.0.1:8089`.

## Results

| Metric | Count |
|--------|------:|
| MCP operations succeeded | 64 |
| Failed | 8 |
| `save_program bulanci.exe` | **yes** |

Full log: [`r6_dispatch_codec_apply_log.json`](./r6_dispatch_codec_apply_log.json)

## Applied highlights

- **libmad L3:** `III_exponents`, `III_reorder`, `III_stereo`, `imdct36`, `III_imdct_s`, `III_overlap`, `III_freqinver` (corrected misnamed `jpeg_fdct_*` / `III_scalefactors`)
- **jdmarker / decompress API:** marker reader chain, `jpeg_CreateDecompress`, consume/finish/header/scanlines
- **Quantizer / upsampler:** `jpeg_add_quant_table`, merged upsampler, 2-pass quant helpers
- **Huff emit (compress):** `emit_byte_e`, `flush_bits_e`, `emit_buffered_bits`
- **Decompiler comments:** `III_decode` @ `0x0045bbb0` vs `CDSMpx_MadTimerAccumulate` @ `0x0045d030`; mislabeled Huff/marker writers
- **Plate comments (UNK bodies):** `FUN_00460200`, `FUN_00460160`, `FUN_00461460`

## Failed renames (not separate Ghidra functions)

These addresses are **labels inside parent IJG routines**, not standalone `Function` entries:

| Address | Intended name | Note |
|---------|---------------|------|
| `0x004604a0` | `start_pass_dpost` | Inside `jinit_d_post_controller` region |
| `0x00460ab0` | `write_marker_byte` | Inline in marker writer |
| `0x00460b20` | `write_frame_header` | Inline |
| `0x00460cc0` | `write_file_trailer` | Inline |
| `0x00464130` | `start_pass_main` | Inline in main controller |
| `0x00465570` | `accumulate_histogram` | Inline in quantizer |
| `0x00466670` | `new_color_map` | Inline |
| `0x004667c0` | `finish_pass_1` | Inline |

Use **decompiler / plate comments** on the parent function if these need names in the UI.

## Doc fixes (repo, not Ghidra)

- [`../../formats/libmad_function_map.md`](../../formats/libmad_function_map.md) — `III_decode` @ `0x0045bbb0`; `0x0045d030` = `CDSMpx_MadTimerAccumulate`
- [`../tick_system.md`](../tick_system.md) — `CDSView_DoModal` @ `0x0042d1a0` (was stale `0x0042d240`)

## Still deferred

- Second pass on remaining `FUN_*` in dispatch band only after live xref/disasm
- Sim-band modal helpers (`FUN_0042cf60` / `cfa0` / `cff0`), `CDSString` handle `this`, `tagRECT` on intersect helpers (see [`r6_ghidra_apply_summary.md`](./r6_ghidra_apply_summary.md))
