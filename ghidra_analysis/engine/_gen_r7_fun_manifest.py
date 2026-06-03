#!/usr/bin/env python3
"""Generate Round 7 single-FUN worker manifest (50 tasks)."""
import json
from pathlib import Path

PICK = [
    ("0x0041b3b0", 1, "cluster"),
    ("0x0042ecc0", 7, "sim"),
    ("0x0042b3d0", 4, "sim"),
    ("0x004298d0", 3, "sim"),
    ("0x00429bd0", 2, "sim"),
    ("0x0042a550", 2, "sim"),
    ("0x0042c140", 2, "sim"),
    ("0x0042c1c0", 2, "sim"),
    ("0x0042ec90", 2, "sim"),
    ("0x0042fdf0", 2, "sim"),
    ("0x00429880", 1, "sim"),
    ("0x004298b0", 1, "sim"),
    ("0x0042a070", 1, "sim"),
    ("0x0042a130", 1, "sim"),
    ("0x0042a590", 1, "sim"),
    ("0x0042a9c0", 1, "sim"),
    ("0x0042afd0", 1, "sim"),
    ("0x0042b3ae", 1, "sim"),
    ("0x0042b48c", 1, "sim"),
    ("0x0042c120", 1, "sim"),
    ("0x0042c160", 1, "sim"),
    ("0x0042c190", 1, "sim"),
    ("0x0042e4e0", 1, "sim"),
    ("0x0042ea40", 1, "sim"),
    ("0x00436160", 1, "sim"),
    ("0x00436d20", 1, "sim"),
    ("0x00436f20", 1, "sim"),
    ("0x00461540", 3, "dispatch"),
    ("0x00467690", 3, "dispatch"),
    ("0x0045b530", 2, "dispatch"),
    ("0x0045d1d0", 2, "dispatch"),
    ("0x004619e0", 2, "dispatch"),
    ("0x00466a30", 2, "dispatch"),
    ("0x00467300", 2, "dispatch"),
    ("0x00467930", 2, "dispatch"),
    ("0x0045b480", 1, "dispatch"),
    ("0x0045dd20", 1, "dispatch"),
    ("0x0045f860", 1, "dispatch"),
    ("0x00460160", 1, "dispatch"),
    ("0x00460200", 1, "dispatch"),
    ("0x00460e80", 1, "dispatch"),
    ("0x00460f30", 1, "dispatch"),
    ("0x00461080", 1, "dispatch"),
    ("0x00461160", 1, "dispatch"),
    ("0x00461460", 1, "dispatch"),
    ("0x00461b70", 1, "dispatch"),
    ("0x00462980", 1, "dispatch"),
    ("0x00463ee0", 1, "dispatch"),
    ("0x00465180", 1, "dispatch"),
    ("0x00466920", 1, "dispatch"),
]

HINTS = {
    "0x0045b480": "round6_logic_task_39 — libmad IMDCT prep; pair with 0x0045b530",
    "0x0045b530": "round6_logic_task_39 — III_imdct_l window",
    "0x0045d1d0": "round6_logic_task_40 — MAD slice UNK",
    "0x0045dd20": "round6_logic_task_40",
    "0x004619e0": "round6_logic_task_45 — jdcoef helper",
    "0x00462980": "round6_logic_task_45 — MCU bit-buffer",
    "0x00460200": "round6_logic_task_43/47 — jpeg compress controller",
    "0x00466920": "round6_logic_task_47 — zlib/dither helper",
    "0x00436160": "round6_logic_task_37 — blit UNK",
}


def main() -> None:
    repo = Path(__file__).resolve().parent
    tasks = []
    for i, (addr, xrefs, band) in enumerate(PICK, 1):
        name = f"FUN_{addr[2:].upper()}"
        hint = HINTS.get(addr, "")
        tasks.append(
            {
                "id": i,
                "round": 7,
                "title": f"FUN recovery: {name} @ {addr} (xrefs={xrefs})",
                "band": band,
                "seed_address": addr,
                "ghidra_name": name,
                "xref_count": xrefs,
                "addresses": [addr],
                "evidence_paths": [
                    "ghidra_analysis/engine/GHIDRA_MCP.md",
                    "ghidra_analysis/engine/ROUND7_FUN_PROTOCOL.md",
                    "config/bulanci/mapping.csv",
                ],
                "prior_hint": hint,
            }
        )
    out = {
        "round": 7,
        "kind": "fun_single",
        "remaining_fun_total": 133,
        "workers": 50,
        "tasks": tasks,
    }
    path = repo / "agent_todos_50_fun_r7.json"
    path.write_text(json.dumps(out, indent=2), encoding="utf-8")
    print(f"wrote {path} ({len(tasks)} tasks)")


if __name__ == "__main__":
    main()
