#!/usr/bin/env python3
"""Emit agent_todos_50_r5.md checklist from manifest."""
import json
from pathlib import Path

base = Path(__file__).parent
tasks = json.loads((base / "agent_todos_50_r5.json").read_text(encoding="utf-8"))
lines = [
    "# Agent todos round 5 (2026-05-30)",
    "",
    "Manifest: [agent_todos_50_r5.json](./agent_todos_50_r5.json) | Results: [agent_todos_50_r5_results.jsonl](./agent_todos_50_r5_results.jsonl) | Protocol: [ROUND5_TASK_PROTOCOL.md](./ROUND5_TASK_PROTOCOL.md)",
    "",
    "Coordinator: worker **50** — [round5_worker_50_report.md](struct_recovery/round5_worker_50_report.md) (no Ghidra; tasks **1–49** only in manifest).",
    "",
    "Handoffs derived from R4 **Remaining UNK**; primary tool: **`set_function_this_type`** (+ prototype/decompile proof).",
    "",
    "| id | pri | src | title |",
    "|----|-----|-----|-------|",
]
for t in tasks:
    title = t["title"].replace("|", "\\|")
    lines.append(f"| {t['id']} | {t['priority']} | {t['source']} | {title} |")
lines.append("")
(base / "agent_todos_50_r5.md").write_text("\n".join(lines) + "\n", encoding="utf-8")
print("wrote checklist")
