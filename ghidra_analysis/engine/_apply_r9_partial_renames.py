#!/usr/bin/env python3
"""Apply Round 9 PARTIAL -> descriptive renames (no further evidence possible)."""
from __future__ import annotations

import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from _ghidra_mcp_client import post, check_connection, PROGRAM  # noqa: E402

# address -> new bare symbol name (namespace preserved by Ghidra per-function)
RENAMES: list[tuple[str, str]] = [
    # CRT __unlock epilogs (caller-disambiguated)
    ("0x004473e8", "__unlock_4_free_epilog"),
    ("0x0044751c", "__unlock_4_calloc_impl_epilog"),
    ("0x004476c1", "__unlock_4_realloc_epilog"),
    ("0x00447e6c", "__onexit_unlock8_epilog"),
    ("0x00449386", "__unlock_4_V6_HeapAlloc_epilog"),
    ("0x004494ec", "__unlock_4_msize_epilog"),
    ("0x0044ac8a", "__unlock_12_initptd_epilog"),
    ("0x0044ae3a", "__unlock_13_freefls_epilog"),
    ("0x0044ae46", "__unlock_12_freefls_epilog"),
    ("0x0044c1d9", "__lock_8_onexit_prolog"),
    ("0x0044c1e2", "__unlock_8_onexit_stub"),
    ("0x0044c3e1", "__unlock_8_doexit_epilog"),
    ("0x0044c895", "__unlock_13_updatetmbcinfo_epilog"),
    ("0x0044cc24", "__unlock_13_setmbcp_epilog"),
    ("0x0044cf75", "__unlock_12_updatetlocinfo_epilog"),
    ("0x004500e2", "__unlock_0_raise_epilog"),
    ("0x00452869", "__lseeki64_unlock_fhandle_epilog"),
    ("0x00452f0b", "__write_unlock_fhandle_epilog"),
    # CRT EH / helpers
    ("0x004486a4", "eh_vector_constructor_iterator_unwind_epilog"),
    ("0x00448765", "eh_vector_destructor_iterator_unwind_epilog"),
    ("0x00448a97", "__crt_default_thread_hook_nop"),
    ("0x0044b1cf", "__FrameUnwindToState_processing_throw_epilog"),
    ("0x0044b490", "CallCatchBlock_epilog"),
    ("0x0044c482", "__VEC_memcpy_movdqa_tail"),
    ("0x0044fcdf", "__clear_fatal_exit_flag_hook"),
    ("0x0044fe54", "__NLG_unwind_dispatch_stub"),
    ("0x0044fe57", "__get_sse2_info_probe"),
    # __init_pointers encoded-null slots
    ("0x0044a916", "__initp_encoded_null_callnewh"),
    ("0x0044a942", "__initp_encoded_null_invalid_parameter"),
    ("0x0044d594", "__initp_encoded_null_purecall"),
    ("0x0044faab", "__initp_encoded_null_critsec_spin"),
    ("0x0045011e", "__initp_encoded_null_slot5"),
    # libmad
    ("0x0045b480", "imdct36_post_dctIV_scatter"),
    # IJG jcprepct / jdmainct
    ("0x00460f30", "create_context_buffer_edge_replicate"),
    ("0x00461080", "pre_process_context_row_replicate"),
    ("0x00461160", "pre_process_context_row_shift"),
    ("0x004613e0", "start_pass_main"),
    ("0x00461460", "jinit_d_prep_controller_sample_bufs"),
    ("0x0046a840", "expand_bottom_edge"),
    # IJG jdmerge / merged upsampler
    ("0x00466920", "compute_sample_table_size"),
    ("0x00466a10", "scale_dither_index_to_byte"),
    ("0x00466a50", "build_colormap_sample_tables"),
    ("0x00466cc0", "alloc_scaled_quant_table_from_rdata"),
    ("0x00466d40", "init_scaled_quant_tables_dedup"),
    ("0x00466f00", "merged_upsample_row_non_rgb"),
    ("0x00467020", "h2v1_merged_upsample_rgb_row"),
    ("0x00467150", "h2v2_fancy_upsample_row"),
    ("0x00467300", "alloc_fancy_upsample_row_bufs"),
    ("0x00467340", "start_pass_merged_upsampler"),
    ("0x0046db50", "h2v1_merged_upsample_ycbcr_colormap"),
    # zlib
    ("0x00466a30", "gen_codes_rounded_idiv_stub"),
    ("0x004713d0", "gen_bitlen_tail"),
    ("0x00472420", "copy_block"),
]


def main() -> int:
    ok, msg = check_connection()
    if not ok:
        print(f"Ghidra MCP unavailable: {msg}")
        return 1

    results: list[dict] = []
    for addr, name in RENAMES:
        ok, msg = post(
            "/rename_function_by_address",
            {"function_address": addr, "new_name": name, "program": PROGRAM},
        )
        results.append({"address": addr, "new_name": name, "ok": ok, "msg": msg[:200]})
        status = "OK" if ok else "FAIL"
        print(f"{status} {addr} -> {name}: {msg[:120]}")

    ok_save, msg_save = post("/save_program", {"program": PROGRAM})
    print(f"save_program: {ok_save} {msg_save[:120]}")

    out = Path(__file__).resolve().parent / "r9_partial_renames_applied.json"
    out.write_text(json.dumps({"renames": results, "save": {"ok": ok_save, "msg": msg_save}}, indent=2))
    failed = [r for r in results if not r["ok"]]
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
