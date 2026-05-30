#!/usr/bin/env python3
"""List unnamed FUN_* functions and vftables; emit agent naming batches.

Reads committed Ghidra exports (no live Ghidra required). Optionally probes
Ghidra MCP at http://127.0.0.1:8089 when --probe-mcp is set.

Outputs under ghidra_analysis/naming_batches/:
  summary.json, summary.md
  batch_01.json .. batch_NN.json  (default 50 batches × 10 items)
  agent_instructions.md (written once; use --no-instructions to skip)

Run from repo root:

    python ghidra_analysis/scripts/generate_naming_batches.py
"""

from __future__ import annotations

import argparse
import csv
import json
import re
import sys
import urllib.error
import urllib.parse
import urllib.request
from collections import defaultdict
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any

REPO_ROOT = Path(__file__).resolve().parents[2]
FUNCS_DUMP = REPO_ROOT / "config" / "bulanci" / "ghidra_functions_dump.csv"
MAPPING_CSV = REPO_ROOT / "config" / "bulanci" / "mapping.csv"
MODULES_CSV = REPO_ROOT / "config" / "bulanci" / "modules.csv"
VFTABLE_CSV = REPO_ROOT / "ghidra_analysis" / "engine" / "vftable_methods.csv"
DECOMP_C = REPO_ROOT / "bulanci.ghidra.exe.c"
OUT_DIR = REPO_ROOT / "ghidra_analysis" / "naming_batches"
DEFAULT_BATCH_COUNT = 50
DEFAULT_BATCH_SIZE = 10
DEFAULT_MCP_URL = "http://127.0.0.1:8089"

FUN_LEAF_RE = re.compile(r"^(?:thunk_)?FUN_[0-9a-fA-F]+$")
GENERIC_VFTABLE_DECL_RE = re.compile(r"^pointer(?:\[(\d+)\])?\s+vftable\s*;\s*$")
G_P_VFTABLE_SYM_RE = re.compile(r"\bg_p([A-Za-z0-9_]+)_vftable")


@dataclass
class FuncRow:
    address: str
    name: str
    namespace: str
    module: str
    size: int
    is_thunk: bool

    @property
    def address_hex(self) -> str:
        a = self.address.strip()
        if not a.startswith("0x"):
            a = f"0x{a}"
        return a.lower()

    @property
    def leaf(self) -> str:
        return self.name.rsplit("::", 1)[-1]

    def is_unnamed(self) -> bool:
        return bool(FUN_LEAF_RE.match(self.leaf))


@dataclass
class VftableRow:
    address: str
    class_name: str
    module: str
    slot_count: int
    unnamed_method_count: int
    decomp_symbol: str | None
    generic_decl: bool = False

    @property
    def address_hex(self) -> str:
        a = self.address.strip()
        if not a.startswith("0x"):
            a = f"0x{a}"
        return a.lower()


def load_namespace_modules() -> dict[str, str]:
    if not MODULES_CSV.exists():
        return {}
    out: dict[str, str] = {}
    with MODULES_CSV.open(encoding="utf-8", newline="") as f:
        for row in csv.DictReader(f):
            out[row["namespace"]] = row["module"]
    return out


def module_for_namespace(ns: str, ns_modules: dict[str, str]) -> str:
    if not ns:
        return "global"
    return ns_modules.get(ns, "global")


def load_functions_from_mapping(ns_modules: dict[str, str]) -> list[FuncRow]:
    """Build function rows from committed mapping.csv (post-refresh Ghidra names)."""
    if not MAPPING_CSV.exists():
        return []
    rows: list[FuncRow] = []
    with MAPPING_CSV.open(encoding="utf-8", newline="") as f:
        for row in csv.reader(f, delimiter=";"):
            if len(row) < 4 or not row[1]:
                continue
            qname = row[1].strip()
            if "::" in qname:
                ns, leaf = qname.rsplit("::", 1)
            else:
                ns, leaf = "_Globals", qname
            try:
                size = int(row[3], 16)
            except ValueError:
                size = 0
            is_thunk = leaf.startswith("thunk_FUN_")
            rows.append(
                FuncRow(
                    address=row[2],
                    name=qname,
                    namespace=ns,
                    module=module_for_namespace(ns, ns_modules),
                    size=size,
                    is_thunk=is_thunk,
                )
            )
    return rows


def load_excluded_addresses(globs: list[str]) -> set[str]:
    """Collect item addresses from prior batch JSON files (wave exclusion)."""
    excluded: set[str] = set()
    for pattern in globs:
        for path in sorted(REPO_ROOT.glob(pattern)):
            if not path.is_file() or path.name.endswith("_results.json"):
                continue
            try:
                data = json.loads(path.read_text(encoding="utf-8"))
            except (json.JSONDecodeError, OSError):
                continue
            for item in data.get("items") or []:
                addr = (item.get("address") or "").strip().lower()
                if addr:
                    if not addr.startswith("0x"):
                        addr = f"0x{addr}"
                    excluded.add(addr)
    return excluded


def load_functions(ns_modules: dict[str, str]) -> list[FuncRow]:
    if not FUNCS_DUMP.exists():
        sys.stderr.write(f"warning: missing {FUNCS_DUMP}\n")
        return []
    rows: list[FuncRow] = []
    with FUNCS_DUMP.open(encoding="utf-8", newline="") as f:
        reader = csv.DictReader(f, delimiter=";")
        for row in reader:
            try:
                size = int(row["size"], 16)
            except ValueError:
                size = 0
            ns = (row.get("namespace") or "").strip() or "_Globals"
            rows.append(
                FuncRow(
                    address=row["address"],
                    name=row["name"],
                    namespace=ns,
                    module=module_for_namespace(ns, ns_modules),
                    size=size,
                    is_thunk=row.get("is_thunk") == "1",
                )
            )
    return rows


def load_decomp_vftable_symbols() -> dict[str, set[str]]:
    """class_name -> set of g_p vftable symbol stems found in decomp header."""
    if not DECOMP_C.exists():
        return {}
    text = DECOMP_C.read_text(encoding="utf-8", errors="replace")
    by_class: dict[str, set[str]] = defaultdict(set)
    for m in G_P_VFTABLE_SYM_RE.finditer(text):
        by_class[m.group(1)].add(m.group(0))
    return by_class


def load_generic_vftable_decls() -> list[tuple[int, str]]:
    if not DECOMP_C.exists():
        return []
    out: list[tuple[int, str]] = []
    for i, line in enumerate(DECOMP_C.read_text(encoding="utf-8", errors="replace").splitlines(), 1):
        m = GENERIC_VFTABLE_DECL_RE.match(line.strip())
        if m:
            slots = int(m.group(1)) if m.group(1) else 1
            out.append((slots, line.strip()))
    return out


def load_vftables(ns_modules: dict[str, str], decomp_by_class: dict[str, set[str]]) -> list[VftableRow]:
    if not VFTABLE_CSV.exists():
        sys.stderr.write(f"warning: missing {VFTABLE_CSV}\n")
        return []
    grouped: dict[str, dict[str, Any]] = {}
    with VFTABLE_CSV.open(encoding="utf-8", newline="") as f:
        reader = csv.DictReader(f, delimiter=";")
        for row in reader:
            addr = row["vftable_addr"].lower()
            if addr not in grouped:
                grouped[addr] = {
                    "class_name": row["class_name"],
                    "slots": 0,
                    "unnamed_methods": 0,
                }
            g = grouped[addr]
            g["slots"] += 1
            if "FUN_" in row["method_name"]:
                g["unnamed_methods"] += 1

    rows: list[VftableRow] = []
    for addr, g in grouped.items():
        cls = g["class_name"]
        ns_mod = module_for_namespace(cls, ns_modules)
        syms = decomp_by_class.get(cls, set())
        decomp_sym = next(iter(syms), None) if len(syms) == 1 else None
        rows.append(
            VftableRow(
                address=addr,
                class_name=cls,
                module=ns_mod,
                slot_count=g["slots"],
                unnamed_method_count=g["unnamed_methods"],
                decomp_symbol=decomp_sym,
            )
        )
    return rows


def mapping_stats() -> dict[str, int]:
    if not MAPPING_CSV.exists():
        return {}
    total = 0
    unnamed = 0
    with MAPPING_CSV.open(encoding="utf-8", newline="") as f:
        for row in csv.reader(f, delimiter=";"):
            if not row or len(row) < 2:
                continue
            qname = row[1]
            if not qname:
                continue
            total += 1
            leaf = qname.rsplit("::", 1)[-1]
            if leaf.startswith("FUN_") or leaf.startswith("thunk_FUN_"):
                unnamed += 1
    named = total - unnamed
    return {
        "mapping_total_functions": total,
        "mapping_unnamed_fun": unnamed,
        "mapping_named_functions": named,
    }


def probe_mcp(url: str) -> dict[str, Any]:
    try:
        req = urllib.request.Request(
            f"{url.rstrip('/')}/list_open_programs",
            headers={"Accept": "application/json"},
        )
        with urllib.request.urlopen(req, timeout=5.0) as resp:
            data = json.loads(resp.read().decode("utf-8", errors="replace"))
        programs = data.get("programs") or []
        return {
            "available": True,
            "url": url,
            "program_count": data.get("count", len(programs)),
            "current_program": data.get("current_program"),
            "programs": [p.get("name") for p in programs],
        }
    except (urllib.error.URLError, TimeoutError, json.JSONDecodeError) as exc:
        return {"available": False, "url": url, "error": str(exc)}


def priority_key_func(r: FuncRow) -> tuple:
    # _Globals first, then larger bodies, stable address
    global_first = 0 if r.namespace == "_Globals" else 1
    return (global_first, -r.size, r.address)


def priority_key_vft(r: VftableRow) -> tuple:
    # generic/unnamed symbol first, more unnamed slots, stable address
    unnamed_sym = 0 if r.decomp_symbol is None else 1
    return (unnamed_sym, -r.unnamed_method_count, -r.slot_count, r.address)


def build_work_queue(
    funcs: list[FuncRow],
    vftables: list[VftableRow],
    *,
    target_items: int,
    vftable_quota: int,
    excluded_addresses: set[str] | None = None,
) -> list[dict[str, Any]]:
    excluded = excluded_addresses or set()

    def excluded_addr(addr: str) -> bool:
        a = addr.strip().lower()
        if not a.startswith("0x"):
            a = f"0x{a}"
        return a in excluded

    unnamed_funcs = sorted(
        [f for f in funcs if f.is_unnamed() and not excluded_addr(f.address_hex)],
        key=priority_key_func,
    )
    unnamed_vft = sorted(
        [
            v
            for v in vftables
            if (v.unnamed_method_count > 0 or v.decomp_symbol is None)
            and not excluded_addr(v.address_hex)
        ],
        key=priority_key_vft,
    )

    items: list[dict[str, Any]] = []
    for v in unnamed_vft[:vftable_quota]:
        items.append(
            {
                "kind": "vftable",
                "address": v.address_hex,
                "name": v.decomp_symbol or "vftable",
                "class_name": v.class_name,
                "namespace": v.class_name,
                "module": v.module,
                "slot_count": v.slot_count,
                "unnamed_method_count": v.unnamed_method_count,
                "notes": "",
            }
        )
    remaining = max(0, target_items - len(items))
    for f in unnamed_funcs[:remaining]:
        items.append(
            {
                "kind": "func",
                "address": f.address_hex,
                "name": f.name,
                "namespace": f.namespace,
                "module": f.module,
                "size": f.size,
                "is_thunk": f.is_thunk,
                "notes": "",
            }
        )
    return items


def stats_by_module(items: list[dict[str, Any]]) -> dict[str, dict[str, int]]:
    out: dict[str, dict[str, int]] = defaultdict(lambda: {"func": 0, "vftable": 0})
    for it in items:
        mod = it.get("module") or "global"
        out[mod][it["kind"]] += 1
    return dict(out)


def write_agent_instructions(path: Path) -> None:
    path.write_text(
        """# Ghidra naming batch — agent instructions

## Rules (mandatory)

1. **Never guess names.** Use xrefs, strings, callers/callees, vtable layout, and Ghidra MCP tools (`decompile`, xref search, `rename_function_by_address`, etc.).
2. Work **slowly**. Partial notes labeled uncertain are fine; **hallucinated names are unacceptable**.
3. Apply Ghidra changes only when confident:
   - `rename_function_by_address` / `rename_or_label` for symbols
   - Create structs only with evidence (size, first field)
   - Otherwise add `set_decompiler_comment` / plate comment: `UNCERTAIN: <reason>`
4. Many symbols still live in the **global** module (`_Globals` namespace). Re-home into real modules/categories only when evidence supports it; else keep module `global` and document in notes.
5. Prefer updating the Ghidra program **and** batch results JSON; sync with existing `ghidra_analysis/` markdown conventions when you find a matching doc.

## Per-batch workflow

1. Read your `batch_NN.json` (10 items).
2. For each entry:
   - **func**: decompile @ address, list xrefs to/from, check namespace and vtable callers.
   - **vftable**: inspect slots in `ghidra_analysis/engine/vftable_methods.csv`, xref to ctor/factory, RTTI/class name.
3. Record outcome in `batch_NN_results.json`:
   ```json
   {
     "batch": 1,
     "entries": [
       {
         "address": "0x00401060",
         "kind": "func",
         "status": "renamed|uncertain|skipped",
         "proposed_name": "OptionalName",
         "ghidra_applied": false,
         "evidence": ["bullet 1", "bullet 2"],
         "notes": ""
       }
     ]
   }
   ```
4. Call `save_program` after Ghidra edits.

## MCP

Read tool schemas under the project's `mcps/user-ghidra-mcp/tools/` before calling. Pass `program: bulanci.exe` when multiple programs are open.

## Static fallbacks

If MCP fails: use `bulanci.ghidra.exe.c`, `config/bulanci/mapping.csv`, and `ghidra_analysis/engine/vftable_methods.csv` — do not invent names.
""",
        encoding="utf-8",
    )


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--batch-count", type=int, default=DEFAULT_BATCH_COUNT)
    parser.add_argument("--batch-size", type=int, default=DEFAULT_BATCH_SIZE)
    parser.add_argument("--out-dir", type=Path, default=OUT_DIR)
    parser.add_argument("--probe-mcp", action="store_true", help="HTTP probe Ghidra MCP")
    parser.add_argument("--mcp-url", default=DEFAULT_MCP_URL)
    parser.add_argument("--no-instructions", action="store_true")
    parser.add_argument("--vftable-quota", type=int, default=50, help="Max vftables in work queue")
    parser.add_argument(
        "--wave",
        type=int,
        default=1,
        help="Wave number stored in batch metadata (default 1)",
    )
    parser.add_argument(
        "--func-source",
        choices=("auto", "mapping", "dump"),
        default="auto",
        help="Where to list functions: mapping.csv, ghidra_functions_dump.csv, or auto",
    )
    parser.add_argument(
        "--exclude-glob",
        action="append",
        default=[],
        metavar="GLOB",
        help="Repo-relative globs of prior batch_*.json to skip (repeatable)",
    )
    parser.add_argument(
        "--exclude-only-if-named",
        action="store_true",
        help=(
            "When excluding prior batch addresses, drop any address that is still "
            "an unnamed FUN_* leaf in the current function list so unfinished "
            "symbols can be re-queued in a later wave."
        ),
    )
    args = parser.parse_args()

    target_items = args.batch_count * args.batch_size
    ns_modules = load_namespace_modules()
    if args.func_source == "mapping":
        all_funcs = load_functions_from_mapping(ns_modules)
        func_source = "mapping.csv"
    elif args.func_source == "dump":
        all_funcs = load_functions(ns_modules)
        func_source = "ghidra_functions_dump.csv"
    else:
        if MAPPING_CSV.exists():
            all_funcs = load_functions_from_mapping(ns_modules)
            func_source = "mapping.csv"
        else:
            all_funcs = load_functions(ns_modules)
            func_source = "ghidra_functions_dump.csv"

    excluded = load_excluded_addresses(args.exclude_glob)
    if args.exclude_only_if_named:
        still_unnamed_addrs = {f.address_hex for f in all_funcs if f.is_unnamed()}
        excluded = excluded - still_unnamed_addrs
    decomp_vft = load_decomp_vftable_symbols()
    all_vft = load_vftables(ns_modules, decomp_vft)
    generic_decls = load_generic_vftable_decls()

    unnamed_funcs = [f for f in all_funcs if f.is_unnamed()]
    work_items = build_work_queue(
        all_funcs,
        all_vft,
        target_items=target_items,
        vftable_quota=min(args.vftable_quota, target_items),
        excluded_addresses=excluded,
    )

    mcp_status = probe_mcp(args.mcp_url) if args.probe_mcp else {"probed": False}
    map_stats = mapping_stats()

    total_funcs = len(all_funcs)
    named_funcs = total_funcs - len(unnamed_funcs)
    pct_named = (100.0 * named_funcs / total_funcs) if total_funcs else 0.0

    by_ns_unnamed: dict[str, int] = defaultdict(int)
    for f in unnamed_funcs:
        by_ns_unnamed[f.namespace] += 1

    summary: dict[str, Any] = {
        "wave": args.wave,
        "sources": {
            "function_list": func_source,
            "functions_dump": str(FUNCS_DUMP.relative_to(REPO_ROOT)),
            "vftable_methods": str(VFTABLE_CSV.relative_to(REPO_ROOT)),
            "mapping_csv": str(MAPPING_CSV.relative_to(REPO_ROOT)),
            "decomp_c": str(DECOMP_C.relative_to(REPO_ROOT)),
        },
        "excluded_addresses_count": len(excluded),
        "functions": {
            "total_in_dump": total_funcs,
            "unnamed_fun_leaf": len(unnamed_funcs),
            "named_in_dump": named_funcs,
            "pct_named_in_dump": round(pct_named, 2),
            "unnamed_by_namespace_top20": dict(
                sorted(by_ns_unnamed.items(), key=lambda kv: -kv[1])[:20]
            ),
            **map_stats,
        },
        "vftables": {
            "unique_in_csv": len(all_vft),
            "generic_vftable_decomp_decls": len(generic_decls),
            "classes_with_g_p_vftable_symbol_in_decomp": len(decomp_vft),
        },
        "batches": {
            "requested_batch_count": args.batch_count,
            "batch_size": args.batch_size,
            "target_items": target_items,
            "actual_items": len(work_items),
            "actual_batch_count": (len(work_items) + args.batch_size - 1) // args.batch_size,
            "vftable_quota_used": min(args.vftable_quota, sum(1 for i in work_items if i["kind"] == "vftable")),
            "by_module_in_queue": stats_by_module(work_items),
        },
        "ghidra_mcp": mcp_status,
    }

    args.out_dir.mkdir(parents=True, exist_ok=True)
    (args.out_dir / "summary.json").write_text(
        json.dumps(summary, indent=2, ensure_ascii=False) + "\n",
        encoding="utf-8",
    )

    md_lines = [
        "# Naming batch summary",
        "",
        f"- Functions in dump: **{total_funcs}** ({named_funcs} named, {len(unnamed_funcs)} unnamed FUN_* / thunk_FUN_*)",
        f"- Named (dump): **{pct_named:.1f}%**",
        f"- Unique vftables (CSV): **{len(all_vft)}**",
        f"- Generic `vftable` decls in decomp: **{len(generic_decls)}**",
        f"- Work queue: **{len(work_items)}** items → **{summary['batches']['actual_batch_count']}** batches of {args.batch_size}",
        "",
    ]
    if map_stats:
        md_lines.append(
            f"- mapping.csv: {map_stats.get('mapping_named_functions', 0)} named / "
            f"{map_stats.get('mapping_total_functions', 0)} total "
            f"({100 - 100 * map_stats.get('mapping_unnamed_fun', 0) / max(1, map_stats.get('mapping_total_functions', 1)):.1f}% named)"
        )
    if mcp_status.get("available"):
        md_lines.append(f"- Ghidra MCP: **available** ({mcp_status.get('current_program')})")
    elif args.probe_mcp:
        md_lines.append(f"- Ghidra MCP: **unavailable** ({mcp_status.get('error', 'unknown')})")
    else:
        md_lines.append("- Ghidra MCP: not probed (use `--probe-mcp`)")
    if len(work_items) < target_items:
        md_lines.append(
            f"\n> Only **{len(work_items)}** items queued (requested {target_items}). "
            f"Reduce `--batch-count` or increase sources."
        )
    (args.out_dir / "summary.md").write_text("\n".join(md_lines) + "\n", encoding="utf-8")

    if not args.no_instructions:
        write_agent_instructions(args.out_dir / "agent_instructions.md")

    for batch_idx in range(args.batch_count):
        start = batch_idx * args.batch_size
        chunk = work_items[start : start + args.batch_size]
        batch_num = batch_idx + 1
        payload = {
            "wave": args.wave,
            "batch": batch_num,
            "batch_of": args.batch_count,
            "batch_size": args.batch_size,
            "items": chunk,
            "item_count": len(chunk),
        }
        path = args.out_dir / f"batch_{batch_num:02d}.json"
        path.write_text(json.dumps(payload, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")

    print(f"Wrote {args.out_dir}")
    print(f"  unnamed functions: {len(unnamed_funcs)}")
    print(f"  work queue: {len(work_items)} items in {summary['batches']['actual_batch_count']} non-empty batches")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
