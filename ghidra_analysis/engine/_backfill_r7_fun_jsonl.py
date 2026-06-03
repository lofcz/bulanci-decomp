#!/usr/bin/env python3
"""Backfill agent_todos_50_fun_r7_results.jsonl from round7_fun_task_* reports."""
import json
import re
from pathlib import Path

REPO = Path(__file__).resolve().parent
JSONL = REPO / "agent_todos_50_fun_r7_results.jsonl"
MANIFEST = REPO / "agent_todos_50_fun_r7.json"
REPORTS = REPO / "fun_recovery"


def parse_report(path: Path) -> dict | None:
    text = path.read_text(encoding="utf-8")
    m_id = re.search(r"\*\*id\*\*\s*\|\s*(\d+)", text)
    if not m_id:
        return None
    tid = int(m_id.group(1))
    status = "PARTIAL"
    if re.search(r"\*\*DONE\*\*", text):
        status = "DONE"
    elif re.search(r"\*\*BLOCKED\*\*", text):
        status = "BLOCKED"
    m_addr = re.search(r"`(0x[0-9a-f]+)`\s*\|\s*`FUN_", text, re.I)
    seed = m_addr.group(1) if m_addr else ""
    after = ""
    m_after = re.search(
        r"`0x[0-9a-f]+`\s*\|\s*`FUN_[^`]+`\s*\|\s*`([^`]+)`",
        text,
        re.I,
    )
    if m_after:
        after = m_after.group(1)
    renamed = after and not after.upper().startswith("FUN_")
    return {
        "id": tid,
        "round": 7,
        "status": status,
        "seed_address": seed,
        "report": f"ghidra_analysis/engine/fun_recovery/{path.name}",
        "ghidra_name_before": f"FUN_{seed[2:].upper()}" if seed else "",
        "ghidra_name_after": after if renamed else "",
        "ghidra_actions": ["backfill_from_report"],
        "blockers": [],
        "note": "jsonl backfilled by coordinator",
    }


def main() -> None:
    tasks = {t["id"]: t for t in json.loads(MANIFEST.read_text(encoding="utf-8"))["tasks"]}
    by_id: dict[int, dict] = {}
    if JSONL.exists():
        for line in JSONL.read_text(encoding="utf-8").splitlines():
            if not line.strip():
                continue
            row = json.loads(line)
            if isinstance(row.get("id"), int):
                by_id[row["id"]] = row

    for report in sorted(REPORTS.glob("round7_fun_task_*_report.md")):
        parsed = parse_report(report)
        if not parsed:
            continue
        tid = parsed["id"]
        if tid in by_id and by_id[tid].get("ghidra_actions") != ["backfill_from_report"]:
            continue
        t = tasks.get(tid, {})
        parsed["title"] = t.get("title", parsed.get("title", ""))
        parsed["band"] = t.get("band", "")
        by_id[tid] = parsed

    lines = [json.dumps(by_id[k], ensure_ascii=False) for k in sorted(by_id)]
    JSONL.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"jsonl lines: {len(lines)} (from {len(list(REPORTS.glob('round7_fun_task_*_report.md')))} reports)")


if __name__ == "__main__":
    main()
