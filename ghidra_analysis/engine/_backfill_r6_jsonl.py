#!/usr/bin/env python3
"""Append coordinator batch-apply line + stub lines for workers missing jsonl."""
import json
from pathlib import Path

REPO = Path(__file__).resolve().parent
JSONL = REPO / "agent_todos_50_r6_logic_results.jsonl"
LOG = REPO / "logic_recovery" / "r6_ghidra_apply_log.json"

def main() -> None:
    existing_ids = set()
    lines = []
    if JSONL.exists():
        for line in JSONL.read_text(encoding="utf-8").splitlines():
            if not line.strip():
                continue
            lines.append(line)
            try:
                existing_ids.add(json.loads(line)["id"])
            except (json.JSONDecodeError, KeyError):
                pass

    apply_log = json.loads(LOG.read_text(encoding="utf-8"))
    batch_line = {
        "id": "batch_apply",
        "round": 6,
        "role": "parent_apply",
        "status": "done",
        "title": "R6 queued Ghidra mutations batch apply",
        "ghidra_actions": ["rename_function_by_address", "set_function_this_type", "set_function_prototype", "set_decompiler_comment", "save_program"],
        "findings": {"ok": apply_log["ok"], "fail": apply_log["fail"], "log": "ghidra_analysis/engine/logic_recovery/r6_ghidra_apply_log.json"},
    }
    if "batch_apply" not in existing_ids:
        lines.append(json.dumps(batch_line, ensure_ascii=False))

    for n in range(1, 50):
        if n in existing_ids:
            continue
        report = REPO / "logic_recovery" / f"round6_logic_task_{n:02d}_report.md"
        if not report.exists():
            continue
        lines.append(
            json.dumps(
                {
                    "id": n,
                    "round": 6,
                    "status": "backfill",
                    "title": f"report exists; jsonl backfilled {n:02d}",
                    "report": f"ghidra_analysis/engine/logic_recovery/round6_logic_task_{n:02d}_report.md",
                    "note": "original worker jsonl line missing; see report for status",
                },
                ensure_ascii=False,
            )
        )

    JSONL.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"jsonl lines: {len(lines)}")


if __name__ == "__main__":
    main()
