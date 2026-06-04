#!/usr/bin/env python3
"""Generate Round 9 _Globals FUN worker manifest (65 tasks)."""
import json
from pathlib import Path

GLOBALS_FUN = [
    "0x00405280",
    "0x0043d5b0",
    "0x0043dae0",
    "0x0043e040",
    "0x0043e740",
    "0x004438a0",
    "0x00443db0",
    "0x004442c0",
    "0x0044714a",
    "0x004473e8",
    "0x0044751c",
    "0x004476c1",
    "0x00447e6c",
    "0x004486a4",
    "0x00448765",
    "0x00448a97",
    "0x00449386",
    "0x004494ec",
    "0x00449bea",
    "0x0044a916",
    "0x0044a942",
    "0x0044ab44",
    "0x0044ac8a",
    "0x0044ae3a",
    "0x0044ae46",
    "0x0044b1cf",
    "0x0044b490",
    "0x0044c1d9",
    "0x0044c1e2",
    "0x0044c3e1",
    "0x0044c482",
    "0x0044c895",
    "0x0044cc24",
    "0x0044d594",
    "0x0044faab",
    "0x0044fcdf",
    "0x0044fe54",
    "0x0044fe57",
    "0x004500e2",
    "0x0045011e",
    "0x00452869",
    "0x00452f0b",
    "0x0045b480",
    "0x00460f30",
    "0x00461080",
    "0x00461160",
    "0x00461460",
    "0x00466920",
    "0x00466a10",
    "0x00466a30",
    "0x00466a50",
    "0x00466cc0",
    "0x00466d40",
    "0x00466f00",
    "0x00467020",
    "0x00467150",
    "0x00467300",
    "0x00467340",
    "0x0046a840",
    "0x0046b400",
    "0x0046c690",
    "0x0046c850",
    "0x0046db50",
    "0x004713d0",
    "0x00472420",
]

# From ghidra_analysis/engine/_gen_r8_fun_manifest.py R7_CARRY + R8 prior hints
PRIOR = {
    "0x0045b480": "R7/R8 — libmad IMDCT half before III_imdct_l",
    "0x00460f30": "R7/R8 — jcprepct edge expand; caller prep vtable",
    "0x00461080": "R7 — jcprepct row replicate",
    "0x00461160": "R7 — jcprepct row shift",
    "0x00461460": "R7 — decompress main buffer init",
    "0x00466920": "R7/R8 — merged-upsampler dither table size",
    "0x00466a30": "R7/R8 — zlib gen_codes idiv helper (PARTIAL)",
    "0x00466a50": "R8 — merged-upsampler colormap index tables (PARTIAL)",
    "0x00467300": "R7/R8 — jdmerge row alloc helper",
    "0x00467340": "R7 — caller of row alloc / upsampler vtable",
    "0x00448a97": "R6 — high-xref sim/other",
    "0x0044fcdf": "R6 — sim cluster",
    "0x0046a840": "R6 — dispatch UNK",
    "0x0046db50": "R6 — dispatch UNK",
    "0x004713d0": "R5 w09 — zlib FUN not audio",
    "0x00472420": "R5 w09 — zlib FUN not audio",
}


def band(addr: str) -> str:
    a = int(addr, 16)
    if a < 0x00420000:
        return "early"
    if a < 0x00450000:
        return "sim"
    if a < 0x00470000:
        return "jpeg_codec"
    return "zlib_tail"


def main() -> None:
    repo = Path(__file__).resolve().parent
    tasks = []
    for i, addr in enumerate(GLOBALS_FUN, 1):
        suffix = addr[2:].upper()
        tasks.append(
            {
                "id": i,
                "round": 9,
                "namespace": "_Globals",
                "title": f"_Globals FUN recovery: FUN_{suffix} @ {addr}",
                "band": band(addr),
                "seed_address": addr,
                "ghidra_name": f"FUN_{suffix}",
                "addresses": [addr],
                "evidence_paths": [
                    "ghidra_analysis/engine/GHIDRA_MCP.md",
                    "ghidra_analysis/engine/ROUND9_GLOBALS_FUN_PROTOCOL.md",
                    "ghidra_analysis/engine/fun_recovery/",
                    "config/bulanci/mapping.csv",
                    "src/bulanci/_Globals.cpp",
                    "scripts/frida/",
                ],
                "prior_hint": PRIOR.get(addr, ""),
            }
        )
    out = {
        "round": 9,
        "kind": "globals_fun_single",
        "workers": len(tasks),
        "tasks": tasks,
    }
    path = repo / "agent_todos_65_globals_r9.json"
    path.write_text(json.dumps(out, indent=2), encoding="utf-8")

    md = repo / "agent_todos_65_globals_r9.md"
    lines = [
        "# Round 9 — 65× `_Globals::FUN_*` workers",
        "",
        f"Manifest: [`agent_todos_65_globals_r9.json`](agent_todos_65_globals_r9.json)  ",
        "Protocol: [`ROUND9_GLOBALS_FUN_PROTOCOL.md`](ROUND9_GLOBALS_FUN_PROTOCOL.md)  ",
        "Results: [`agent_todos_65_globals_r9_results.jsonl`](agent_todos_65_globals_r9_results.jsonl)  ",
        "Reports: [`fun_recovery/r9_globals_task_NNN_report.md`](fun_recovery/)",
        "",
        "| ID | Address | Band | Ghidra name | Prior hint |",
        "|----|---------|------|-------------|------------|",
    ]
    for t in tasks:
        h = (t["prior_hint"] or "")[:50].replace("|", "/")
        lines.append(f"| {t['id']:03d} | `{t['seed_address']}` | {t['band']} | {t['ghidra_name']} | {h} |")
    md.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"wrote {path} ({len(tasks)} tasks)")


if __name__ == "__main__":
    main()
