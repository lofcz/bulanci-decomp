#!/usr/bin/env python3
"""Extract round-3 tasks from batch followup summaries and struct UNK sections."""
import re
import json
from pathlib import Path

base = Path(__file__).parent
tasks = []
seen = set()


def add_task(title, one_liner, batches, priority, structs, acceptance, key=None):
    k = key or (tuple(structs), one_liner[:100])
    if k in seen:
        return
    seen.add(k)
    tasks.append(
        {
            "title": title[:60],
            "one_liner": one_liner[:280],
            "source_batches": sorted(set(batches)),
            "priority": priority,
            "structs": structs or ["(misc)"],
            "acceptance": acceptance[:400],
        }
    )


def infer_priority(item: str) -> str:
    low = item.lower()
    if any(
        w in low
        for w in [
            "factory",
            "sizeof",
            "merge",
            "wrong size",
            "create_struct",
            "deferred struct",
            "872 b",
            "0x248",
            "disassemble",
            "create_function",
        ]
    ):
        return "high"
    if any(w in low for w in ["optional", "later:", "minor", "doc only", "naming if desired"]):
        return "low"
    return "med"


def extract_structs(text: str) -> list[str]:
    found = re.findall(
        r"\b(C[A-Z][A-Za-z0-9_]+|CDS[A-Z][A-Za-z0-9_]+|EH[A-Z][A-Za-z0-9_]+|Exception[A-Za-z]*)\b",
        text,
    )
    skip = {"CREATE", "CALL", "DATA", "SKIP", "VERIFIED", "PARTIAL", "UNK", "RTTI"}
    out = []
    for s in found:
        if s in skip or len(s) < 4:
            continue
        if s not in out:
            out.append(s)
    return out[:4] or ["(misc)"]


def skip_item(item: str) -> bool:
    low = item.lower()
    skips = [
        "no ghidra",
        "no changes",
        "not batch queue",
        "documented only — not",
        "doc unk unchanged",
        "same as ",
        "layouts already applied",
        "no batch-05 follow-ups",
        "batch 43 had **skip**",
        "unchanged — doc only",
    ]
    return any(s in low for s in skips)


# --- batch followup summaries ---
for fp in sorted(base.glob("batch_*_followup_summary.md")):
    m = re.search(r"batch_(\d+)_followup", fp.name)
    batch = int(m.group(1)) if m else -1
    text = fp.read_text(encoding="utf-8", errors="replace")

    # Explicit "Follow-up items" / deferred bullets outside Remaining UNK
    for sec_name in [
        "Follow-up items",
        "Follow-ups from",
        "Prior batch follow-ups",
        "Not in scope",
        "Deferred",
    ]:
        pat = rf"##[^\n]*{re.escape(sec_name.split()[0])}[^\n]*\n(.*?)(?=\n## |\Z)"
        for block in re.findall(pat, text, re.DOTALL | re.IGNORECASE):
            for line in block.split("\n"):
                line = line.strip()
                if not line.startswith(("-", "*")):
                    continue
                item = re.sub(r"^[-*]\s*\[[ x]\]\s*", "", line).strip()
                item = item.replace("**", "")
                if len(item) < 20 or skip_item(item):
                    continue
                if "deferred" in item.lower() or "follow-up" in item.lower() or "@" in item:
                    structs = extract_structs(item)
                    add_task(
                        structs[0],
                        item,
                        [batch],
                        infer_priority(item),
                        structs,
                        f"Resolve in Ghidra with xref evidence: {item[:120]}",
                        key=("followup", item[:150]),
                    )

    # Remaining UNK sections
    for block in re.findall(
        r"## Remaining UNK[^\n]*\n(.*?)(?=\n## |\Z)", text, re.DOTALL | re.IGNORECASE
    ):
        for line in block.split("\n"):
            line = line.strip()
            if not line.startswith(("-", "*")):
                continue
            item = re.sub(r"^[-*]\s*", "", line).strip().replace("**", "")
            if len(item) < 12 or skip_item(item):
                continue
            structs = extract_structs(item)
            add_task(
                structs[0],
                item,
                [batch],
                infer_priority(item),
                structs,
                f"Prove or document UNK in {structs[0]}.md with func@addr evidence",
                key=("unk", item[:150]),
            )

# --- struct .md Follow-up / UNK actionable lines ---
for fp in sorted(base.glob("*.md")):
    if fp.name.startswith("batch_") or fp.name in {
        "AGENT_PROTOCOL.md",
        "ROUND3_TASK_PROTOCOL.md",
    }:
        continue
    if fp.name.startswith("_"):
        continue
    struct = fp.stem
    text = fp.read_text(encoding="utf-8", errors="replace")
    for sec in re.findall(
        r"## (?:UNK|Follow-up)[^\n]*\n(.*?)(?=\n## |\Z)", text, re.DOTALL | re.IGNORECASE
    ):
        for line in sec.split("\n"):
            line = line.strip()
            if not line.startswith(("-", "*", "|")):
                continue
            if line.startswith("|") and "---" in line:
                continue
            item = re.sub(r"^[-*]\s*", "", line).strip().replace("**", "")
            if len(item) < 20 or skip_item(item):
                continue
            low = item.lower()
            if not any(
                w in low
                for w in [
                    "factory",
                    "deferred",
                    "disassemble",
                    "merge",
                    "rename",
                    "create_",
                    "sizeof",
                    "unk",
                    "0x",
                    "ghidra",
                    "follow-up",
                    "todo",
                ]
            ):
                continue
            batches = []
            bm = re.findall(r"batch[_ ](\d+)", item, re.I)
            batches = [int(x) for x in bm]
            add_task(
                struct,
                f"{struct}: {item}",
                batches or [],
                infer_priority(item),
                [struct],
                f"Update {struct}.md; Ghidra mutation if evidence supports",
                key=("md", struct, item[:120]),
            )

# Sort: high first, then med, then low; shorter structs with factories first
prio_order = {"high": 0, "med": 1, "low": 2}
tasks.sort(key=lambda t: (prio_order[t["priority"]], t["structs"][0]))

out = base / "_round3_extracted.json"
out.write_text(json.dumps(tasks, indent=2), encoding="utf-8")
print(f"total_tasks={len(tasks)}")
print(f"wrote {out}")
