#!/usr/bin/env python3
"""Generate Round 7 FUN reports 41-50 and append jsonl."""
import json
from pathlib import Path

out_dir = Path(__file__).parent / "fun_recovery"
out_dir.mkdir(exist_ok=True)

tasks = [
    (41, "0x00460e80", "FUN_00460E80", 0xAA, "__stdcall", "jcprepct.c local (create_context_buffer head)",
     "Alloc per-component sample row pointer tables at decompress workspace +0x38/+0x3c.",
     [("FUN_00461460", "0x004614c6", "UNCONDITIONAL_CALL")], [],
     "No COFF export; IJG jcprepct.c static local name not byte-matched."),
    (42, "0x00460f30", "FUN_00460F30", 0x142, "__cdecl", "jcprepct.c local (context edge expand)",
     "Mirror/replicate edge rows for downsampling context buffer.",
     [("FUN_00461460", "0x0046142e", "UNCONDITIONAL_CALL")], [],
     "No IJG export symbol; vtable-driven from LAB_004613e0 in FUN_00461460."),
    (43, "0x00461080", "FUN_00461080", 0xD7, "__cdecl", "jcprepct.c local (h/v expand)",
     "Replicate sample rows for horizontal/vertical expansion factors during buffer setup.",
     [("FUN_00461460", "0x0046137d", "UNCONDITIONAL_CALL")], [],
     "Exact jcprepct.c symbol name unproven."),
    (44, "0x00461160", "FUN_00461160", 0x9A, "__cdecl", "jcprepct.c local (row shift)",
     "Shift sample rows before FUN_00460f30 edge expand; sets workspace +0x48.",
     [("FUN_00461460", "0x00461331", "UNCONDITIONAL_CALL")], [],
     "Call chain 0x461331 -> 0x46142e documented; export label deferred."),
    (45, "0x00461460", "FUN_00461460", 0xD3, "__cdecl", "decompress main-controller buffer init (split)",
     "Alloc 0x50-byte object at cinfo+0x184, wire vtable LAB_004613e0, per-component alloc_large buffers; calls FUN_00460e80 when context rows needed.",
     [("FUN_00460200", "0x00460354", "UNCONDITIONAL_CALL")],
     [("FUN_00460e80", "buffer row tables")],
     "May be compiler tail of jinit_d_main_controller; merge vs rename deferred."),
    (46, "0x00461b70", "FUN_00461B70", 0x160, "__stdcall", "jdcoefct.c coef-controller vtable method",
     "When cinfo+0xc8 clear and cinfo+0x8c set: alloc workspace+0x70 if needed; copy six int bounds per component from cinfo+0xc4 tracks into row buffer (+0x18 stride). Returns byte flag in AL.",
     [("LAB_00462320 cluster", "0x00462338", "UNCONDITIONAL_CALL")], [],
     "Installed as coef object method slot puVar3[2] in jinit_d_coef_controller; exact IJG method name UNK."),
    (47, "0x00462980", "FUN_00462980", 0x7E, "__stdcall", "jdhuff.c decode_mcu entropy helper",
     "Advance bit buffer, call entropy module callback +8, zero coef block array, reload restart_interval from cinfo+0xfc into coef+0x24.",
     [("decode_mcu", "0x00462a22", "UNCONDITIONAL_CALL")], [],
     "Distinct from process_restart@0x00462f80; not renamed to avoid duplicate IJG label."),
    (48, "0x00463ee0", "FUN_00463EE0", 0x71, "__cdecl", "jdcolor.c decompress color-deconverter init",
     "alloc_large 0x54 -> cinfo+0x19c; *vtbl=start_pass_dcolor; per-component 0x100-byte LUT via alloc_large+memset.",
     [("FUN_00460200", "0x004602ed", "UNCONDITIONAL_CALL")], [("_memset", "LUT zero")],
     "jinit_color_deconverter name taken @0x00464e00 (different body); rename deferred."),
    (49, "0x00465180", "FUN_00465180", 0x150, "__cdecl", "jdcolor.c YCbCr->BGR scanline color_convert",
     "Indexed Y/Cb/Cr samples through LUT block at cinfo+0x1a0 (+0x10..0x1c); writes 3-byte BGR tuples to output row (2-wide unroll + odd tail).",
     [("jinit_merged_upsampler", "0x00465563", "DATA")], [],
     "Method pointer installed at jinit_merged_upsampler+0x63 (DATA xref 0x00465563); exact IJG symbol UNK."),
    (50, "0x00466920", "FUN_00466920", 0xE7, "__cdecl", "quantizer/zlib alloc-size helper",
     "Nested factorial loop over param_2[]; optional remap via jpeg_natural_order@0x0049df50 when param_1[0xb]==2; returns alloc size to FUN_00466a50; JERR 0x38 if bound<2.",
     [("FUN_00466a50", "0x00466a6a", "UNCONDITIONAL_CALL")], [],
     "On gen_codes/Huffman path; zlib upstream symbol not COFF-verified."),
]

jsonl_path = Path(__file__).parent / "agent_todos_50_fun_r7_results.jsonl"
jsonl_lines = []

for tid, addr, name, size, cc, role, summary, xrefs, callees, unk in tasks:
    plate_new = tid != 45
    task_actions = [f"set_plate_comment@{addr}"] if plate_new else []
    if tid == 41:
        task_actions.append("save_program bulanci.exe")

    if xrefs:
        xref_md = "\n".join(f"| `{f}` | `{a}` | {t} |" for f, a, t in xrefs)
    else:
        xref_md = "| `jinit_merged_upsampler` | `0x00465563` | DATA (method ptr +0xc) |"

    if callees:
        callee_md = "\n".join(f"| `{c}` | {d} |" for c, d in callees)
    else:
        callee_md = "| — | — |"

    report = f"""# Round 7 FUN — Task {tid:02d} Report

## Task

| Field | Value |
|-------|-------|
| **id** | {tid} |
| **round** | 7 |
| **band** | dispatch |
| **seed_address** | `{addr}` |
| **title** | FUN recovery: {name} @ {addr} (xrefs=1) |

## Status

**PARTIAL** — Role and xref closure proven from live Ghidra MCP (`connect_instance bulanci`). **No rename:** IJG/zlib export symbol not uniquely verified (Round 7 no-guess rule).

## Function

| Address | Ghidra name | Size | CC | Role summary | Evidence |
|---------|-------------|-----:|-----|--------------|----------|
| `{addr}` | `{name}` | {size} B | {cc} | {summary} | Decompile + xref_to; {role} |

### Xref closure

| From | Site | Type |
|------|------|------|
{xref_md}

### Callees (selected)

| Callee | Role |
|--------|------|
{callee_md}

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
"""
    if task_actions:
        for act in task_actions:
            tool = act.split("@")[0]
            report += f"| `{tool}` | `{addr}` | applied |\n"
    else:
        report += "| — | — | R6 plate retained; no new mutation this task |\n"
    if tid > 41 and plate_new:
        report += "| `save_program` | — | single save at task 41 |\n"

    report += f"""
## Frida

**none** — JPEG decompress/quantizer path; static xref + decompile sufficient.

## Remaining UNK

| Item | Why still UNK |
|------|----------------|
| IJG/zlib export name | {unk} |
"""

    (out_dir / f"round7_fun_task_{tid:02d}_report.md").write_text(report, encoding="utf-8")

    entry = {
        "id": tid,
        "round": 7,
        "status": "PARTIAL",
        "title": f"FUN recovery: {name} @ {addr} (xrefs=1)",
        "seed_address": addr,
        "band": "dispatch",
        "ghidra_name_before": name,
        "ghidra_name_after": name,
        "report": f"ghidra_analysis/engine/fun_recovery/round7_fun_task_{tid:02d}_report.md",
        "ghidra_actions": task_actions,
        "blockers": ["rename blocked: no unique IJG/zlib COFF symbol"],
        "findings": {"role": role, "xref_count": len(xrefs) if xrefs else 1},
    }
    jsonl_lines.append(json.dumps(entry, separators=(",", ":")))

with jsonl_path.open("a", encoding="utf-8") as f:
    for line in jsonl_lines:
        f.write(line + "\n")

print(f"wrote {len(tasks)} reports and jsonl lines")
