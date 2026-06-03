#!/usr/bin/env python3
"""Backfill agent_todos_50_fun_r8_results.jsonl from round8_fun_task_* reports."""
import json
import re
from pathlib import Path

REPO = Path(__file__).resolve().parent
JSONL = REPO / "agent_todos_50_fun_r8_results.jsonl"
MANIFEST = REPO / "agent_todos_50_fun_r8.json"
REPORTS = REPO / "fun_recovery"


def parse_report(path: Path) -> dict | None:
    text = path.read_text(encoding="utf-8")
    m_tid = re.search(r"round8_fun_task_(\d+)_report", path.name)
    if not m_tid:
        return None
    tid = int(m_tid.group(1))
    status = "PARTIAL"
    status_m = re.search(
        r"## Status\s*\r?\n+\s*\*\*(DONE|PARTIAL|BLOCKED)\*\*",
        text,
        re.I,
    )
    if status_m:
        status = status_m.group(1).upper()
    m_addr = re.search(r"seed_address\*\*\s*\|\s*`(0x[0-9a-f]+)`", text, re.I)
    if not m_addr:
        m_addr = re.search(r"\*\*Address\*\*\s*\|\s*`(0x[0-9a-f]+)`", text, re.I)
    seed = (m_addr.group(1) if m_addr else "").lower()
    after = ""
    for pat in [
        r"After\*\*\s*\|\s*\*\*`([^`]+)`",
        r"Ghidra \(after\)\s*\|\s*\*\*`([^`]+)`",
        r"Renamed to\s+\*\*`([^`]+)`\*\*",
        r"Renamed.*?→\s*\*\*`([^`]+)`",
        r"→\s*\*\*`([^`]+)`\*\*",
        r"Renamed\s+`FUN_[^`]+`\s*→\s*\*\*`([^`]+)`",
        r"Renamed\s+`FUN_[^`]+`\s*→\s+`([^`]+)`",
    ]:
        m = re.search(pat, text, re.I)
        if m:
            after = m.group(1).strip()
            break
    renamed = after and not after.upper().startswith("FUN_")
    return {
        "id": tid,
        "status": status,
        "seed_address": seed,
        "ghidra_name_after": after if renamed else "",
        "renamed": renamed,
        "report": f"ghidra_analysis/engine/fun_recovery/{path.name}",
    }


def main() -> None:
    tasks = {t["id"]: t for t in json.loads(MANIFEST.read_text(encoding="utf-8"))["tasks"]}
    by_id: dict[int, dict] = {}
    for report in sorted(REPORTS.glob("round8_fun_task_*_report.md")):
        parsed = parse_report(report)
        if not parsed:
            continue
        tid = parsed["id"]
        t = tasks[tid]
        by_id[tid] = {
            "id": tid,
            "round": 8,
            "status": parsed["status"],
            "title": t["title"],
            "seed_address": parsed["seed_address"] or t["seed_address"],
            "band": t["band"],
            "xref_count": t["xref_count"],
            "report": parsed["report"],
            "ghidra_name_before": t["ghidra_name"],
            "ghidra_name_after": parsed["ghidra_name_after"],
            "renamed": parsed["renamed"],
        }
    lines = [json.dumps(by_id[k], ensure_ascii=False) for k in sorted(by_id)]
    JSONL.write_text("\n".join(lines) + "\n", encoding="utf-8")
    done = sum(1 for r in by_id.values() if r["status"] == "DONE")
    print(f"jsonl: {len(lines)} lines, DONE={done}, PARTIAL={len(lines)-done}")


if __name__ == "__main__":
    main()
