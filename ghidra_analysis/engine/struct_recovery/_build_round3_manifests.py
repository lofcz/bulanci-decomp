#!/usr/bin/env python3
"""Build round3 task manifests from extracted tasks."""
import json
import re
from pathlib import Path

base = Path(__file__).parent
raw = json.loads((base / "_round3_extracted.json").read_text(encoding="utf-8"))

# Deduplicate by struct + normalized one_liner stem
def norm(s: str) -> str:
    s = s.lower()
    s = re.sub(r"0x[0-9a-f]+", "ADDR", s)
    s = re.sub(r"\s+", " ", s)
    return s[:100]


def score(t: dict) -> int:
    s = 0
    pri = {"high": 30, "med": 10, "low": 0}[t["priority"]]
    s += pri
    ol = t["one_liner"].lower()
    for w, pts in [
        ("factory", 25),
        ("create_struct", 25),
        ("merge", 20),
        ("872", 20),
        ("sizeof", 18),
        ("disassemble", 18),
        ("create_function", 18),
        ("deferred", 12),
        ("rename_function", 10),
        ("vtable", 8),
        ("+0x", 5),
    ]:
        if w in ol:
            s += pts
    if t["source_batches"]:
        s += 3
    if t["structs"][0] not in ("(misc)", "Exception"):
        s += 2
    return s


# Group: keep highest-scored per (struct, norm prefix)
groups: dict[tuple, dict] = {}
for t in raw:
    struct = t["structs"][0]
    key = (struct, norm(t["one_liner"])[:70])
    sc = score(t)
    if key not in groups or sc > score(groups[key]):
        # merge batch lists
        if key in groups:
            t["source_batches"] = sorted(
                set(groups[key]["source_batches"] + t["source_batches"])
            )
        groups[key] = t

deduped = list(groups.values())
deduped.sort(key=score, reverse=True)

assigned = deduped[:50]
remaining = deduped[50:]

# Assign ids
tasks = []
for i, t in enumerate(assigned):
    tasks.append(
        {
            "id": i,
            "title": t["title"],
            "one_liner": t["one_liner"],
            "source_batches": t["source_batches"],
            "priority": t["priority"],
            "structs": t["structs"],
            "acceptance": t["acceptance"],
        }
    )

remaining_out = [
    {"one_liner": t["one_liner"]} for t in remaining
]

(base / "round3_tasks.json").write_text(
    json.dumps(tasks, indent=2), encoding="utf-8"
)
(base / "round3_tasks_remaining.json").write_text(
    json.dumps(remaining_out, indent=2), encoding="utf-8"
)

# spawn manifest
lines = [
    "# Round 3 spawn manifest",
    "",
    f"- **total_tasks_found:** {len(deduped)} (from {len(raw)} raw extractions, deduplicated)",
    f"- **assigned_count:** {len(tasks)}",
    f"- **remaining_count:** {len(remaining_out)}",
    "",
    "## Assigned tasks (round 3)",
    "",
    "| id | priority | struct | one_liner |",
    "|----|----------|--------|-----------|",
]
for t in tasks:
    ol = t["one_liner"].replace("|", "\\|")[:120]
    lines.append(
        f"| {t['id']} | {t['priority']} | {t['structs'][0]} | {ol} |"
    )
lines += [
    "",
    "## Remaining tasks (deferred)",
    "",
    "| one_liner |",
    "|-----------|",
]
for t in remaining_out[:200]:
    ol = t["one_liner"].replace("|", "\\|")[:200]
    lines.append(f"| {ol} |")
if len(remaining_out) > 200:
    lines.append(f"| … and {len(remaining_out) - 200} more in `round3_tasks_remaining.json` |")

(base / "round3_spawn_manifest.md").write_text("\n".join(lines), encoding="utf-8")
print(f"deduped={len(deduped)} assigned={len(tasks)} remaining={len(remaining_out)}")
