#!/usr/bin/env python3
"""Generate agent_todos_50_r6_logic.json — logic/internals pass over three address bands."""
from __future__ import annotations

import json
import urllib.request
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
OUT_JSON = Path(__file__).resolve().parent / "agent_todos_50_r6_logic.json"
MCP_URL = "http://127.0.0.1:8089/list_functions_enhanced"

RANGES = {
    "cluster_41ab": (0x0041A000, 0x0041BFFF, "0x0041a–0x0041b: gaming entities, scheduler tick surface"),
    "sim_429_436": (0x00429000, 0x00436FFF, "0x00429–0x00436: game state, simulation, per-frame"),
    "dispatch_45a_468": (0x0045A000, 0x00468FFF, "0x0045a–0x00468: input, network, UI dispatch"),
}

# Scheduler-adjacent seeds (cluster band) — assign to workers 1–10 first
CLUSTER_SEEDS = [
    ("0x0041aed0", "CBulanek_SchedulerTick", "high"),
    ("0x0041af70", "FUN_0041af70", "high"),
    ("0x0041b180", "CWeapon_OnSchedulerEvent", "high"),
    ("0x0041b1d0", "CShot_SchedulerTick", "high"),
    ("0x0041b210", "CMina_OnSchedulerHook", "high"),
    ("0x0041a390", "CBulanek_AddEntity", "medium"),
    ("0x0041a2f0", "CExplosion_CollectLandminesInBlastRect", "medium"),
    ("0x0041ab70", "CGaming_Ctor", "medium"),
    ("0x0041a140", "CGaming_RespawnPlayerAtSafeLocation", "medium"),
    ("0x0041a020", "CGaming_OnPlayerCollectItem", "medium"),
]

WORKER_SLICES = [
    (1, 10, "cluster_41ab"),
    (11, 38, "sim_429_436"),
    (39, 49, "dispatch_45a_468"),
]


def fetch_functions() -> list[dict]:
    allf: list[dict] = []
    offset = 0
    while True:
        body = json.dumps({"program": "bulanci.exe", "limit": 5000, "offset": offset}).encode()
        req = urllib.request.Request(
            MCP_URL,
            data=body,
            headers={"Content-Type": "application/json"},
            method="POST",
        )
        with urllib.request.urlopen(req, timeout=120) as r:
            data = json.loads(r.read())
        res = data.get("result", data)
        if isinstance(res, str):
            res = json.loads(res)
        funcs = res["functions"]
        allf.extend(funcs)
        if len(funcs) < 5000:
            break
        offset += 5000
    return allf


def addr_hex(raw: str) -> str:
    a = int(raw, 16)
    return f"0x{a:08x}"


def in_range(addr: int, lo: int, hi: int) -> bool:
    return lo <= addr <= hi


def main() -> None:
    funcs = fetch_functions()
    by_band: dict[str, list[dict]] = {k: [] for k in RANGES}
    for f in funcs:
        a = int(f["address"], 16)
        for key, (lo, hi, _) in RANGES.items():
            if in_range(a, lo, hi):
                by_band[key].append(
                    {
                        "address": addr_hex(f["address"]),
                        "name": f.get("name", ""),
                        "isThunk": f.get("isThunk", False),
                    }
                )
    for key in by_band:
        by_band[key].sort(key=lambda x: x["address"])

    tasks: list[dict] = []
    seed_addrs = {s[0].lower() for s in CLUSTER_SEEDS}

    # Workers 1–10: cluster with explicit seeds
    cluster = by_band["cluster_41ab"]
    cluster_rest = [f for f in cluster if f["address"].lower() not in seed_addrs]
    per_seed_rest = max(1, (len(cluster_rest) + 9) // 10)
    for i, (addr, label, pri) in enumerate(CLUSTER_SEEDS, start=1):
        extra = cluster_rest[(i - 1) * per_seed_rest : i * per_seed_rest]
        addrs = [addr] + [e["address"] for e in extra]
        tasks.append(
            {
                "id": i,
                "title": f"Logic cluster: {label} + scheduler/gaming neighbors",
                "range": "cluster_41ab",
                "range_note": RANGES["cluster_41ab"][2],
                "addresses": addrs,
                "seed_address": addr,
                "priority": pri,
                "source": "seed",
                "ghidra_actions": [
                    "decompile",
                    "disassemble",
                    "get_xrefs_to",
                    "set_function_this_type",
                    "set_function_prototype",
                    "set_decompiler_comment",
                    "rename_function_by_address",
                    "save_program",
                ],
                "evidence_paths": [
                    "ghidra_analysis/engine/ROUND6_LOGIC_PROTOCOL.md",
                    "ghidra_analysis/engine/struct_recovery/AGENT_PROTOCOL.md",
                    "ghidra_analysis/engine/tick_system.md",
                    "ghidra_analysis/gameplay/combat_projectiles.md",
                    "ghidra_analysis/engine/struct_recovery/CBulanek.md",
                    "ghidra_analysis/engine/struct_recovery/CGaming.md",
                ],
                "acceptance": (
                    f"Document control flow and callers for seed {label}@{addr}; "
                    "fix this/reparent via set_function_this_type only with disasm proof; "
                    "Frida script if behavior unproven static-only."
                ),
            }
        )

    def slice_band(w_start: int, w_end: int, band_key: str) -> None:
        items = by_band[band_key]
        n_workers = w_end - w_start + 1
        chunk = max(1, (len(items) + n_workers - 1) // n_workers)
        for w in range(w_start, w_end + 1):
            idx = w - w_start
            sl = items[idx * chunk : (idx + 1) * chunk]
            if not sl and w == w_end:
                sl = items[-chunk:] if items else []
            addrs = [x["address"] for x in sl]
            names_preview = ", ".join(x["name"][:24] for x in sl[:3])
            lo_a = sl[0]["address"] if sl else "n/a"
            hi_a = sl[-1]["address"] if sl else "n/a"
            tasks.append(
                {
                    "id": w,
                    "title": f"Logic {band_key}: {lo_a}–{hi_a} ({len(sl)} funcs)",
                    "range": band_key,
                    "range_note": RANGES[band_key][2],
                    "addresses": addrs,
                    "function_names": [x["name"] for x in sl],
                    "priority": "medium" if band_key == "sim_429_436" else "medium",
                    "source": "slice",
                    "ghidra_actions": [
                        "decompile",
                        "disassemble",
                        "get_xrefs_to",
                        "set_function_this_type",
                        "set_function_prototype",
                        "set_decompiler_comment",
                        "save_program",
                    ],
                    "evidence_paths": [
                        "ghidra_analysis/engine/ROUND6_LOGIC_PROTOCOL.md",
                        "ghidra_analysis/engine/struct_recovery/AGENT_PROTOCOL.md",
                        "ghidra_analysis/main_menu.md",
                        "ghidra_analysis/engine/player_controls.md",
                    ],
                    "acceptance": (
                        f"Per-function logic notes for slice ({names_preview}…); "
                        "no guessed semantics; Frida under scripts/frida/ when needed."
                    ),
                }
            )

    slice_band(11, 38, "sim_429_436")
    slice_band(39, 49, "dispatch_45a_468")

    tasks.append(
        {
            "id": 50,
            "title": "R6 logic coordinator: protocol, manifest, checklist, aggregate status",
            "range": "coordinator",
            "addresses": [],
            "priority": "high",
            "source": "coordinator",
            "ghidra_actions": [],
            "evidence_paths": [
                "ghidra_analysis/engine/ROUND6_LOGIC_PROTOCOL.md",
                "ghidra_analysis/engine/agent_todos_50_r6_logic.json",
            ],
            "acceptance": "Publish protocol + checklist; no Ghidra mutations; scan logic_recovery reports 1–49.",
        }
    )

    tasks.sort(key=lambda t: t["id"])
    summary = {
        "bands": {k: len(v) for k, v in by_band.items()},
        "workers": len(tasks),
    }
    OUT_JSON.write_text(
        json.dumps({"summary": summary, "tasks": tasks}, indent=2) + "\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    print("wrote", OUT_JSON)


if __name__ == "__main__":
    main()
