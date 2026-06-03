#!/usr/bin/env python3
"""Apply Round 6 dispatch-band (0x45a–0x68) IJG/libmad renames from reports 39–49."""
from __future__ import annotations

import json
from pathlib import Path

from _ghidra_mcp_client import post

# Evidence-only renames from round6_logic_task_{39..49}_report.md (skip explicit UNK rows)
RENAMES: list[tuple[str, str]] = [
    # --- task 39 libmad L3 ---
    ("0x0045a060", "III_exponents"),
    ("0x0045a9d0", "III_reorder"),
    ("0x0045ab00", "III_stereo"),
    ("0x0045b250", "imdct36"),
    ("0x0045b7a0", "III_imdct_s"),
    ("0x0045ba90", "III_overlap"),
    ("0x0045bb40", "III_freqinver"),
    # --- task 41 jdmarker reader API ---
    ("0x0045dde0", "save_marker"),
    ("0x0045df60", "skip_variable"),
    ("0x0045dfe0", "next_marker"),
    ("0x0045e0c0", "first_marker"),
    ("0x0045e170", "read_markers"),
    ("0x0045e4d0", "read_restart_marker"),
    ("0x0045e540", "jpeg_resync_to_restart"),
    ("0x0045e5f0", "reset_marker_reader"),
    ("0x0045e620", "jinit_marker_reader"),
    ("0x0045e6a0", "jpeg_CreateDecompress"),
    ("0x0045e7a0", "default_decompress_parms"),
    ("0x0045e8d0", "jpeg_consume_input"),
    ("0x0045e9a0", "jpeg_finish_decompress"),
    ("0x0045ea70", "jpeg_read_header"),
    ("0x0045eaf0", "output_pass_setup"),
    ("0x0045eba0", "jpeg_read_scanlines"),
    ("0x0045ec10", "jpeg_start_decompress"),
    # --- task 42 compressor parms ---
    ("0x0045ed80", "jpeg_suppress_tables"),
    ("0x0045efe0", "jpeg_add_quant_table"),
    ("0x0045f1b0", "jpeg_set_linear_quality"),
    ("0x0045f7f0", "jround_up"),
    # --- task 43 decompress input/master ---
    ("0x0045feb0", "use_merged_upsample"),
    ("0x0045ff30", "jinit_d_main_controller"),
    ("0x004604d0", "jinit_d_post_controller"),
    ("0x004604a0", "start_pass_dpost"),
    # --- task 44 jcmarker writer ---
    ("0x00460ad0", "write_file_header"),
    ("0x00460bf0", "write_scan_header"),
    ("0x00460a70", "write_marker_header"),
    ("0x00460b20", "write_frame_header"),
    ("0x00460cc0", "write_file_trailer"),
    ("0x00460ab0", "write_marker_byte"),
    # --- task 46 decompress main/quantize ---
    ("0x00463fe0", "process_data_simple_main"),
    ("0x00464090", "process_data_context_main"),
    ("0x00464130", "start_pass_main"),
    ("0x004654f0", "jinit_merged_upsampler"),
    ("0x00465650", "update_box"),
    # --- task 47 colormap quantizer ---
    ("0x00465bb0", "fill_inverse_cmap"),
    ("0x00465570", "accumulate_histogram"),
    ("0x00465da0", "init_error_limit"),
    ("0x00465f70", "prescan_quantize"),
    ("0x004660f0", "start_pass_1_quant"),
    ("0x004665c0", "init_inverse_cmap"),
    ("0x004666a0", "start_pass_2"),
    ("0x00466670", "new_color_map"),
    ("0x004667c0", "finish_pass_1"),
    ("0x00466e40", "color_quantize"),
    # --- task 48 merged upsample + coef (compress side helpers) ---
    ("0x00467460", "jinit_merged_upsampler"),
    ("0x00467600", "jinit_c_main_controller"),
    ("0x00467dc0", "jinit_c_coef_controller"),
    # --- task 49 Huff emit (encoder) ---
    ("0x00468dd0", "emit_byte_e"),
    ("0x00468ed0", "flush_bits_e"),
    ("0x00468ef0", "emit_ac_symbol"),
    ("0x00468f50", "emit_eobrun"),
    ("0x00468f20", "emit_buffered_bits"),
]

COMMENTS: list[tuple[str, str]] = [
    ("0x0045a060", "R6: was III_scalefactors; body is III_exponents (libmad layer3.c)"),
    ("0x0045b250", "R6: was jpeg_fdct_ifast; body is libmad imdct36"),
    ("0x0045b7a0", "R6: was jpeg_fdct_islow; body is libmad III_imdct_s"),
    ("0x0045bbb0", "libmad III_decode (not 0x45d030; that is CDSMpx_MadTimerAccumulate)"),
    ("0x0045d030", "CDSMpx_MadTimerAccumulate seek helper; NOT III_decode"),
    ("0x00460bf0", "R6: was start_pass_huff; IJG write_scan_header (jcmarker.c)"),
    ("0x00460a70", "R6: duplicate emit_dri label; IJG write_marker_header"),
    ("0x004654f0", "R6: manifest jinit_color_deconverter dup; body is jinit_merged_upsampler"),
]

PLATE_COMMENTS: list[tuple[str, str]] = [
    ("0x00460200", "R6 task43: tail of jinit_d_post_controller init (decompress master)"),
    ("0x00460160", "R6 task43: main-controller range-limit table init (jdmainct.c)"),
    ("0x00461460", "R6 task44: decompress buffer init; caller jinit_d_post path @0x460354"),
]


def main() -> None:
    log: list[dict] = []
    ok = fail = 0

    for addr, name in RENAMES:
        good, msg = post("/rename_function_by_address", {"function_address": addr, "new_name": name})
        log.append({"op": "rename", "addr": addr, "name": name, "ok": good, "msg": msg})
        ok += good
        fail += not good

    for addr, text in COMMENTS:
        good, msg = post("/set_decompiler_comment", {"address": addr, "comment": text})
        log.append({"op": "decompiler_comment", "addr": addr, "ok": good, "msg": msg})
        ok += good
        fail += not good

    for addr, text in PLATE_COMMENTS:
        good, msg = post("/set_plate_comment", {"address": addr, "comment": text})
        log.append({"op": "plate_comment", "addr": addr, "ok": good, "msg": msg})
        ok += good
        fail += not good

    good, msg = post("/save_program", {})
    log.append({"op": "save_program", "ok": good, "msg": msg})

    out = Path(__file__).resolve().parent / "logic_recovery" / "r6_dispatch_codec_apply_log.json"
    out.write_text(json.dumps({"ok": ok, "fail": fail, "entries": log}, indent=2), encoding="utf-8")
    print(f"dispatch codec: ok={ok} fail={fail} save={good}")
    print(f"log -> {out}")


if __name__ == "__main__":
    main()
