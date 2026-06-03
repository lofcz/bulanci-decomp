#!/usr/bin/env python3
"""Rebuild agent_todos_50_r6_logic_results.jsonl from reports + apply logs."""
from __future__ import annotations

import json
import re
from pathlib import Path

REPO = Path(__file__).resolve().parent
JSONL = REPO / "agent_todos_50_r6_logic_results.jsonl"
REPORTS = REPO / "logic_recovery"
MANIFEST = REPO / "agent_todos_50_r6_logic.json"

STATUS_RE = re.compile(r"\*\*(PARTIAL|DONE|done)\*\*", re.I)
TITLE_RE = re.compile(r"\|\s*\*\*title\*\*\s*\|\s*(.+?)\s*\|")
RANGE_RE = re.compile(r"\|\s*\*\*range\*\*\s*\|\s*`?([^`|]+)`?\s*\|", re.I)


def parse_report(path: Path) -> dict:
    text = path.read_text(encoding="utf-8", errors="replace")
    m = re.search(r"round6_logic_task_(\d+)_report", path.name)
    tid = int(m.group(1)) if m else None
    status = "PARTIAL"
    sm = STATUS_RE.search(text)
    if sm:
        status = "DONE" if sm.group(1).lower() == "done" else "PARTIAL"
    title = None
    tm = TITLE_RE.search(text)
    if tm:
        title = tm.group(1).strip()
    rng = None
    rm = RANGE_RE.search(text)
    if rm:
        rng = rm.group(1).strip()
    ghidra = "save_program" in text.lower() and "applied" in text.lower()
    blockers = []
    if "not connected" in text.lower() or "mcp unavailable" in text.lower():
        blockers.append("ghidra-mcp unavailable during worker run")
    return {
        "id": tid,
        "round": 6,
        "status": status,
        "title": title or f"task {tid:02d}",
        "range": rng,
        "report": f"ghidra_analysis/engine/logic_recovery/{path.name}",
        "ghidra_actions": ["save_program"] if ghidra else [],
        "blockers": blockers,
        "source": "backfill_v2",
    }


def load_manifest_titles() -> dict[int, str]:
    if not MANIFEST.exists():
        return {}
    data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    out: dict[int, str] = {}
    for t in data.get("tasks", []):
        if "id" in t and "title" in t:
            out[int(t["id"])] = t["title"]
    return out


def main() -> None:
    titles = load_manifest_titles()
    lines: list[str] = []

    for n in range(1, 51):
        if n == 50:
            p = REPORTS / "round6_logic_worker_50_report.md"
            if p.exists():
                row = parse_report(p)
                row["id"] = 50
                row["role"] = "coordinator"
                if 50 in titles:
                    row["title"] = titles[50]
                lines.append(json.dumps(row, ensure_ascii=False))
            continue
        p = REPORTS / f"round6_logic_task_{n:02d}_report.md"
        if not p.exists():
            continue
        row = parse_report(p)
        if n in titles:
            row["title"] = titles[n]
        lines.append(json.dumps(row, ensure_ascii=False))

    for log_name, role in [
        ("r6_ghidra_apply_log.json", "batch_apply_pass1"),
        ("r6_dispatch_codec_apply_log.json", "batch_apply_dispatch"),
        ("r6_pass3_apply_log.json", "batch_apply_pass3"),
    ]:
        log_path = REPORTS / log_name
        if not log_path.exists():
            continue
        log = json.loads(log_path.read_text(encoding="utf-8"))
        lines.append(
            json.dumps(
                {
                    "id": role,
                    "round": 6,
                    "role": "parent_apply",
                    "status": "done",
                    "title": f"R6 Ghidra {role}",
                    "ghidra_actions": ["batch_http_apply", "save_program"],
                    "findings": {"ok": log.get("ok"), "fail": log.get("fail"), "log": f"ghidra_analysis/engine/logic_recovery/{log_name}"},
                },
                ensure_ascii=False,
            )
        )

    JSONL.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"wrote {len(lines)} lines -> {JSONL}")


if __name__ == "__main__":
    main()
