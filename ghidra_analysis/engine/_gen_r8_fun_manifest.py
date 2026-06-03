#!/usr/bin/env python3
"""Generate Round 8 single-FUN worker manifest (50 tasks from live FUN_* pool)."""
import json
from pathlib import Path

# Live Ghidra search_functions_enhanced ^FUN_ bulanci.exe (2026-06-03)
POOL = [
    ("0x00448a97", 5),
    ("0x0044fcdf", 4),
    ("0x00452fb7", 4),
    ("0x0046a840", 3),
    ("0x0046c3d0", 3),
    ("0x00439710", 2),
    ("0x004466a0", 2),
    ("0x00449c90", 2),
    ("0x0044ab59", 2),
    ("0x0044fe54", 2),
    ("0x00466a30", 2),
    ("0x00467300", 2),
    ("0x0046db50", 2),
    ("0x00472420", 2),
    ("0x00405280", 1),
    ("0x004370b0", 1),
    ("0x0043c9b0", 1),
    ("0x0043d5b0", 1),
    ("0x0043dae0", 1),
    ("0x0043e040", 1),
    ("0x0043e740", 1),
    ("0x004438a0", 1),
    ("0x00443db0", 1),
    ("0x004442c0", 1),
    ("0x0044714a", 1),
    ("0x004473e8", 1),
    ("0x0044751c", 1),
    ("0x004476c1", 1),
    ("0x00447e6c", 1),
    ("0x004486a4", 1),
    ("0x00448765", 1),
    ("0x00449386", 1),
    ("0x004494ec", 1),
    ("0x00449bea", 1),
    ("0x0044a916", 1),
    ("0x0044a942", 1),
    ("0x0044ab44", 1),
    ("0x0044ac8a", 1),
    ("0x0044ae3a", 1),
    ("0x0044ae46", 1),
    ("0x0044b1cf", 1),
    ("0x0044b490", 1),
    ("0x0044c1d9", 1),
    ("0x0044c1e2", 1),
    ("0x0044c3e1", 1),
    ("0x0044c482", 1),
    ("0x0044c895", 1),
    ("0x0044cc24", 1),
    ("0x0044cf75", 1),
    ("0x0044d594", 1),
    ("0x0044faab", 1),
    ("0x0044fe57", 1),
    ("0x004500e2", 1),
    ("0x0045011e", 1),
    ("0x00452869", 1),
    ("0x00452f0b", 1),
    ("0x00454ace", 1),
    ("0x0045b480", 1),
    ("0x00460200", 1),
    ("0x00460f30", 1),
    ("0x00461080", 1),
    ("0x00461160", 1),
    ("0x004613e0", 1),
    ("0x00461460", 1),
    ("0x00466920", 1),
    ("0x00466a10", 1),
    ("0x00466a50", 1),
    ("0x00466cc0", 1),
    ("0x00466d40", 1),
    ("0x00466f00", 1),
    ("0x00467020", 1),
    ("0x00467150", 1),
    ("0x00467340", 1),
    ("0x00467d10", 1),
    ("0x004683a0", 1),
    ("0x00468590", 1),
    ("0x00468be0", 1),
    ("0x00469060", 1),
    ("0x004696a0", 1),
    ("0x00469900", 1),
    ("0x0046a870", 1),
    ("0x0046abd0", 1),
    ("0x0046acf0", 1),
    ("0x0046adf0", 1),
    ("0x0046ae80", 1),
    ("0x0046b000", 1),
    ("0x0046b0b0", 1),
    ("0x0046b400", 1),
    ("0x0046b590", 1),
    ("0x0046c690", 1),
    ("0x0046c850", 1),
    ("0x0046c8f0", 1),
    ("0x004713d0", 1),
]

# R7 PARTIAL / open threads — prioritize in R8
R7_CARRY = {
    "0x004370b0": "R7 task 26/27 — track-switch iterator; caller SetCurrentTrack",
    "0x0045b480": "R7 task 36 — libmad IMDCT half before III_imdct_l",
    "0x00460200": "R7 task 40 — jdmaster.c decompress master_selection split",
    "0x00460f30": "R7 task 42 — jcprepct edge expand; caller prep vtable",
    "0x00461080": "R7 task 43 — jcprepct row replicate",
    "0x00461160": "R7 task 44 — jcprepct row shift",
    "0x004613e0": "R7 task 42/43 — prep vtable start_pass homolog",
    "0x00461460": "R7 task 45 — decompress main buffer init",
    "0x00466920": "R7 task 50 — merged-upsampler dither table size",
    "0x00466a30": "R7 task 33 — zlib gen_codes idiv helper",
    "0x00467300": "R7 task 34 — jdmerge row alloc helper",
    "0x00467340": "R7 task 34 — caller of row alloc",
    "0x00466a50": "R7 task 50 — caller of dither size calc",
}

R6_HINTS = {
    "0x00439710": "R6 — sim band UNK",
    "0x004466a0": "R6 — sim UNK",
    "0x0046a840": "R6 — dispatch UNK",
    "0x0046c3d0": "R6 — dispatch UNK",
    "0x0046db50": "R6 — dispatch UNK",
    "0x00448a97": "R6 — high-xref sim/other",
    "0x0044fcdf": "R6 — sim cluster",
    "0x00452fb7": "R6 — sim cluster",
}


def band(addr: str) -> str:
    a = int(addr, 16)
    if a < 0x00420000:
        return "cluster"
    if a < 0x00450000:
        return "sim"
    if a < 0x00470000:
        return "dispatch"
    if a < 0x00480000:
        return "eh"
    return "other"


def pick(pool: list[tuple[str, int]], n: int = 50) -> list[tuple[str, int, str]]:
    by_addr = {a: (a, x) for a, x in pool}
    chosen: list[tuple[str, int, str]] = []
    seen: set[str] = set()

    def add(addr: str) -> None:
        if addr in seen or addr not in by_addr:
            return
        a, x = by_addr[addr]
        seen.add(addr)
        chosen.append((a, x, band(a)))

    # 1) R7 carry-over
    for addr in sorted(R7_CARRY, key=lambda s: -by_addr.get(s, ("", 0))[1]):
        add(addr)

    # 2) Rest: prefer dispatch+sim+cluster by xref, then eh
    band_order = {"dispatch": 0, "sim": 1, "cluster": 2, "other": 3, "eh": 4}
    rest = sorted(
        [p for p in pool if p[0] not in seen],
        key=lambda p: (-p[1], band_order.get(band(p[0]), 9), p[0]),
    )
    for addr, xrefs in rest:
        if len(chosen) >= n:
            break
        add(addr)

    return chosen[:n]


def main() -> None:
    repo = Path(__file__).resolve().parent
    picked = pick(POOL, 50)
    tasks = []
    for i, (addr, xrefs, b) in enumerate(picked, 1):
        name = f"FUN_{addr[2:].upper()}"
        hint = R7_CARRY.get(addr) or R6_HINTS.get(addr, "")
        tasks.append(
            {
                "id": i,
                "round": 8,
                "title": f"FUN recovery: {name} @ {addr} (xrefs={xrefs})",
                "band": b,
                "seed_address": addr,
                "ghidra_name": name,
                "xref_count": xrefs,
                "addresses": [addr],
                "evidence_paths": [
                    "ghidra_analysis/engine/GHIDRA_MCP.md",
                    "ghidra_analysis/engine/ROUND8_FUN_PROTOCOL.md",
                    "ghidra_analysis/engine/fun_recovery/round7_fun_task_*_report.md",
                    "config/bulanci/mapping.csv",
                ],
                "prior_hint": hint,
            }
        )
    out = {
        "round": 8,
        "kind": "fun_single",
        "remaining_fun_total": len(POOL),
        "workers": 50,
        "left_after_round": len(POOL) - len(picked),
        "tasks": tasks,
    }
    path = repo / "agent_todos_50_fun_r8.json"
    path.write_text(json.dumps(out, indent=2), encoding="utf-8")

    md = repo / "agent_todos_50_fun_r8.md"
    lines = [
        "# Round 8 — 50× single `FUN_*` workers",
        "",
        f"Pool: **{len(POOL)}** live `FUN_*` (post-R7). This round: **{len(picked)}**; **{len(POOL) - len(picked)}** deferred.",
        "",
        "Manifest: [`agent_todos_50_fun_r8.json`](agent_todos_50_fun_r8.json)  ",
        "Protocol: [`ROUND8_FUN_PROTOCOL.md`](ROUND8_FUN_PROTOCOL.md)  ",
        "Results: [`agent_todos_50_fun_r8_results.jsonl`](agent_todos_50_fun_r8_results.jsonl)  ",
        "Reports: [`fun_recovery/round8_fun_task_NN_report.md`](fun_recovery/)",
        "",
        "| ID | Address | Xrefs | Band | Ghidra name | Prior hint |",
        "|----|---------|------:|------|-------------|------------|",
    ]
    for t in tasks:
        h = (t["prior_hint"] or "")[:60].replace("|", "/")
        lines.append(
            f"| {t['id']:02d} | `{t['seed_address']}` | {t['xref_count']} | {t['band']} | {t['ghidra_name']} | {h} |"
        )
    md.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"wrote {path} ({len(tasks)} tasks, {out['left_after_round']} left)")


if __name__ == "__main__":
    main()
