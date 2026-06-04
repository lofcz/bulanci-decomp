#!/usr/bin/env python3
"""Sync Round 9 Ghidra renames into mapping.csv, repo stubs, and jsonl rollups."""
from __future__ import annotations

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ENGINE = ROOT / "ghidra_analysis" / "engine"
FUN_RECOVERY = ENGINE / "fun_recovery"

# Prior DONE renames (agent pass, before partial batch)
DONE_EXTRA: dict[str, str] = {
    "0x00405280": "CRadio_HitTestRowAtPoint",
    "0x0043d5b0": "BlitOpaque_Indexed4_to_RGB565",
    "0x0043dae0": "BlitOpaque_RGB565_to_Indexed8",
    "0x0043e040": "BlitOpaque_BGR24_to_BGRA32",
    "0x0043e740": "BlitDestKey_Indexed2_to_BGR24",
    "0x004438a0": "BlitKeyAndMask_1bpp_to_BGRA32",
    "0x00443db0": "BlitAlphaBlend_2bitIndex_BGRA32",
    "0x004442c0": "BlitKeyAndMask_4bpp_to_BGRA32",
    "0x0044714a": "CDSWorkingThread_ThreadProcEpilogue",
    "0x00449bea": "__unlock_10_mtinit_epilog",
    "0x0044ab44": "__fls_getvalue",
    "0x0046b400": "fullsize_smooth_downsample",
    "0x0046c690": "prepare_for_pass",
    "0x0046c850": "pass_startup",
    # other units
    "0x0043c9b0": "CDSDirectSound_StopAllAndReleaseCom",
    "0x0046abd0": "create_context_buffer",
    "0x0046b590": "jinit_downsampler",
    "0x0046c8f0": "jinit_c_master_control",
}

OTHER_NS: dict[str, str] = {
    "0x0043c9b0": "CBulanci",
    "0x0044cf75": "_LocaleUpdate",
    "0x004613e0": "",  # global — no namespace prefix in mapping
    "0x0046abd0": "CDSJpegImage",
    "0x0046b590": "CDSJpegImage",
    "0x0046c8f0": "CDSJpegImage",
}


def load_rename_map() -> dict[str, str]:
    m = {k.lower(): v for k, v in DONE_EXTRA.items()}
    partial = ENGINE / "r9_partial_renames_applied.json"
    if partial.is_file():
        data = json.loads(partial.read_text(encoding="utf-8"))
        for row in data.get("renames", []):
            if row.get("ok"):
                m[row["address"].lower()] = row["new_name"]
    return m


def fun_sym(addr: str) -> str:
    return f"FUN_{addr[2:].upper()}"


def build_fun_to_name(addr_map: dict[str, str]) -> dict[str, str]:
    out: dict[str, str] = {}
    for addr, name in addr_map.items():
        hex_part = addr[2:].upper()
        out[f"FUN_{hex_part}"] = name
        out[f"FUN_{hex_part.lower()}"] = name
    return out


def patch_mapping_csv(addr_map: dict[str, str]) -> int:
    path = ROOT / "config" / "bulanci" / "mapping.csv"
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    n = 0
    out: list[str] = []
    for line in lines:
        if not line.startswith(";"):
            out.append(line)
            continue
        parts = line.split(";")
        if len(parts) < 3:
            out.append(line)
            continue
        qname = parts[1]
        addr_field = parts[2].strip().lower()
        if not addr_field.startswith("0x"):
            out.append(line)
            continue
        try:
            key = f"0x{int(addr_field, 16):08x}"
        except ValueError:
            out.append(line)
            continue
        new_name = addr_map.get(key.lower())
        if not new_name:
            out.append(line)
            continue
        old_fun = re.search(r"::(FUN_[0-9a-fA-F]+)$", qname)
        if old_fun or qname.endswith(old_fun.group(1) if old_fun else ""):
            if old_fun:
                ns = qname.rsplit("::", 1)[0]
                parts[1] = f"{ns}::{new_name}"
                n += 1
        elif qname.startswith("FUN_"):
            parts[1] = new_name
            n += 1
        out.append(";".join(parts))
    path.write_text("\n".join(out) + "\n", encoding="utf-8")
    return n


def replace_in_file(path: Path, fun_to_name: dict[str, str]) -> int:
    if not path.is_file():
        return 0
    text = path.read_text(encoding="utf-8", errors="replace")
    orig = text
    hits = 0
    for fun, name in sorted(fun_to_name.items(), key=lambda x: -len(x[0])):
        count = text.count(fun)
        if count:
            text = text.replace(fun, name)
            hits += count
    if text != orig:
        path.write_text(text, encoding="utf-8")
        return hits
    return 0


def parse_report_status(report_path: Path) -> tuple[str, str | None]:
    text = report_path.read_text(encoding="utf-8", errors="replace")
    m = re.search(r"\*\*Status:\*\*\s*\*\*(DONE|PARTIAL|BLOCKED)\*\*", text, re.I)
    if not m:
        m = re.search(r"## Status\s*\n\s*\*\*(DONE|PARTIAL|BLOCKED)\*\*", text, re.I)
    status = m.group(1).upper() if m else "PARTIAL"
    # renamed name from report
    for pat in [
        r"Renamed[^`]*`FUN_[^`]+`\s*→\s*\*\*`([^`]+)`\*\*",
        r"Ghidra name\*\*\s*\|\s*`([^`]+)`\s*\(was",
        r"\*\*After\*\*\s*\|\s*`([^`]+)`",
        r"→\s*\*\*`([^`]+)`\*\*",
    ]:
        rm = re.search(pat, text, re.I)
        if rm:
            return status, rm.group(1)
    return status, None


def rebuild_globals_jsonl(addr_map: dict[str, str]) -> None:
    manifest = json.loads((ENGINE / "agent_todos_65_globals_r9.json").read_text(encoding="utf-8"))
    lines: list[str] = []
    for task in manifest["tasks"]:
        tid = task["id"]
        addr = task["seed_address"].lower()
        report = FUN_RECOVERY / f"r9_globals_task_{tid:03d}_report.md"
        status, report_name = parse_report_status(report) if report.is_file() else ("PARTIAL", None)
        after = addr_map.get(addr) or report_name or task["ghidra_name"]
        renamed = after.upper() != task["ghidra_name"].upper() and not after.upper().startswith("FUN_")
        if renamed:
            status = "DONE"
        row = {
            "id": tid,
            "round": 9,
            "status": status,
            "title": task["title"],
            "seed_address": task["seed_address"],
            "band": task.get("band", ""),
            "report": f"ghidra_analysis/engine/fun_recovery/r9_globals_task_{tid:03d}_report.md",
            "ghidra_name_before": task["ghidra_name"],
            "ghidra_name_after": after,
            "renamed": renamed,
            "ghidra_saved": True,
        }
        lines.append(json.dumps(row, ensure_ascii=False))
    (ENGINE / "agent_todos_65_globals_r9_results.jsonl").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )


def rebuild_other_jsonl(addr_map: dict[str, str]) -> None:
    other_tasks = [
        (1, "0x0043c9b0", "CBulanci", "FUN_0043c9b0", "other"),
        (2, "0x0044cf75", "_LocaleUpdate", "FUN_0044cf75", "other"),
        (3, "0x004613e0", "", "FUN_004613e0", "jpeg_codec vicinity"),
        (4, "0x0046abd0", "CDSJpegImage", "FUN_0046abd0", "jpeg_codec"),
        (5, "0x0046b590", "CDSJpegImage", "FUN_0046b590", "jpeg_codec"),
        (6, "0x0046c8f0", "CDSJpegImage", "FUN_0046c8f0", "jpeg_codec"),
    ]
    lines: list[str] = []
    for tid, addr, ns, before, band in other_tasks:
        addr_l = addr.lower()
        report = FUN_RECOVERY / f"r9_other_task_{tid:03d}_report.md"
        status, report_name = parse_report_status(report) if report.is_file() else ("PARTIAL", None)
        after = addr_map.get(addr_l) or report_name or before
        renamed = after.upper() != before.upper() and not after.upper().startswith("FUN_")
        if renamed:
            status = "DONE"
        row = {
            "id": tid,
            "round": 9,
            "unit": "other",
            "status": status,
            "namespace": ns,
            "seed_address": addr,
            "band": band,
            "report": f"ghidra_analysis/engine/fun_recovery/r9_other_task_{tid:03d}_report.md",
            "ghidra_name_before": before,
            "ghidra_name_after": after,
            "renamed": renamed,
            "ghidra_saved": True,
        }
        lines.append(json.dumps(row, ensure_ascii=False))
    (ENGINE / "agent_todos_r9_other_results.jsonl").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )


def main() -> None:
    addr_map = load_rename_map()
    fun_to_name = build_fun_to_name(addr_map)
    n_map = patch_mapping_csv(addr_map)
    src_files = [
        ROOT / "src/bulanci/_Globals.cpp",
        ROOT / "include/bulanci/_Globals.h",
        ROOT / "src/bulanci/CDSJpegImage.cpp",
        ROOT / "include/bulanci/CDSJpegImage.h",
        ROOT / "src/bulanci/CBulanci.cpp",
        ROOT / "include/bulanci/CBulanci.h",
    ]
    src_hits = sum(replace_in_file(p, fun_to_name) for p in src_files)
    rebuild_globals_jsonl(addr_map)
    rebuild_other_jsonl(addr_map)
    print(f"rename map entries: {len(addr_map)}")
    print(f"mapping.csv rows updated: {n_map}")
    print(f"source FUN_* replacements: {src_hits}")
    print("jsonl: agent_todos_65_globals_r9_results.jsonl (65 lines)")
    print("jsonl: agent_todos_r9_other_results.jsonl (6 lines)")


if __name__ == "__main__":
    main()
