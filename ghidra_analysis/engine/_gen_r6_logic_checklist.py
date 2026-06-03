#!/usr/bin/env python3
import json
from pathlib import Path

base = Path(__file__).resolve().parent
raw = json.loads((base / "agent_todos_50_r6_logic.json").read_text(encoding="utf-8"))
summary = raw.get("summary", {}) if isinstance(raw, dict) else {}
tasks = raw["tasks"] if isinstance(raw, dict) else raw
bands = summary.get("bands", {})
lines = [
    "# Agent todos round 6 — logic/internals (2026-06-03)",
    "",
    "Manifest: [agent_todos_50_r6_logic.json](./agent_todos_50_r6_logic.json) | "
    "Results: [agent_todos_50_r6_logic_results.jsonl](./agent_todos_50_r6_logic_results.jsonl) | "
    "Protocol: [ROUND6_LOGIC_PROTOCOL.md](./ROUND6_LOGIC_PROTOCOL.md)",
    "",
    f"Bands: cluster={bands.get('cluster_41ab', '?')} | sim={bands.get('sim_429_436', '?')} | "
    f"dispatch={bands.get('dispatch_45a_468', '?')}",
    "",
    "Coordinator: worker **50** (no Ghidra).",
    "",
    "| id | pri | range | title |",
    "|----|-----|-------|-------|",
]
for t in tasks:
    title = t["title"].replace("|", "\\|")[:90]
    lines.append(f"| {t['id']} | {t.get('priority', '?')} | {t.get('range', '?')} | {title} |")
lines.append("")
(base / "agent_todos_50_r6_logic.md").write_text("\n".join(lines), encoding="utf-8")
print("wrote checklist", len(tasks), "tasks")
