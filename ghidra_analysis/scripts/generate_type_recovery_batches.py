#!/usr/bin/env python3
"""Build type-recovery agent batches from committed exports (optional MCP probe).

Reads mapping.csv, modules.csv, units_listing.csv, decomp, vftable_methods.csv,
and optional layout hints from ghidra_analysis/*.md.

Outputs under ghidra_analysis/type_recovery_wave{N}/:
  summary.json, summary.md, agent_instructions.md
  batch_01.json .. batch_NN.json

Run from repo root:

    python ghidra_analysis/scripts/generate_type_recovery_batches.py --wave 1 --game-only
"""

from __future__ import annotations

import argparse
import csv
import json
import re
import sys
import urllib.error
import urllib.request
from collections import defaultdict
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any

REPO_ROOT = Path(__file__).resolve().parents[2]
MAPPING_CSV = REPO_ROOT / "config" / "bulanci" / "mapping.csv"
MODULES_CSV = REPO_ROOT / "config" / "bulanci" / "modules.csv"
UNITS_CSV = REPO_ROOT / "config" / "bulanci" / "units_listing.csv"
VFTABLE_CSV = REPO_ROOT / "ghidra_analysis" / "engine" / "vftable_methods.csv"
DECOMP_C = REPO_ROOT / "bulanci.ghidra.exe.c"
HINT_MD_GLOBS = (
    "ghidra_analysis/widgets.md",
    "ghidra_analysis/main_menu.md",
    "ghidra_analysis/gameplay/main_menu.md",
    "ghidra_analysis/engine/player_controls.md",
)
DEFAULT_BATCH_COUNT = 50
DEFAULT_BATCH_SIZE = 10
DEFAULT_MAX_ITEMS = 500
DEFAULT_MCP_URL = "http://127.0.0.1:8089"

P0_NAMESPACES = frozenset({"CBulanci", "CBulanek", "CGaming", "CGame", "CMenu", "CDSApp"})
P1_NAMESPACES = frozenset({"CDSView", "CDSChained", "CDSChain"})
P1_PREFIXES = ("CButton", "CEdit", "CList", "CScroll", "CPanel", "CWindow", "CDialog", "CMsg", "CHelp", "CHistory", "CSetup", "CStartGame", "CExit", "CGameType", "CTcpIp", "CSession", "CRadio", "CSwitch", "CRuch", "CPoem", "CProgress", "CVolume", "CColor", "CNum", "CStatic", "CIcon", "CChat", "CScore", "CLoading", "CPause", "CMovie", "CBlack", "CAnim", "CBitmap", "CAdvertising")

EXCLUDED_UNITS = frozenset(
    {
        "_Globals",
        "ATL",
        "Runtime",
        "std",
        "type_info",
        "exception",
        "zlib",
        "_LocaleUpdate",
        "ODSImage",
    }
)
EXCLUDED_NS_SUBSTR = re.compile(
    r"(?i)(libjpeg|jpeg_|zlib|msvcrt|crt|throw_|operator_|_purecall|_invalid)"
)

RAW_OFFSET_RE = re.compile(r"(?:this|param_1)\s*\+\s*(0x[0-9a-fA-F]+)", re.I)
FUNC_HEADER_RE = re.compile(
    r"^[\w\s\*]+\s+(?:__\w+\s+)*((?:_Globals|[A-Za-z0-9_]+)::([A-Za-z0-9_]+)|_Globals::([A-Za-z0-9_]+))\s*\("
)
CLASS_SECTION_RE = re.compile(r"^##\s+\d+\.\s+`([A-Za-z0-9_]+)`")
LAYOUT_SIZE_RE = re.compile(r"Layout\s*\(size\s*~(0x[0-9a-fA-F]+)", re.I)
FIELD_ROW_RE = re.compile(r"\|\s*`(\+0x[0-9a-fA-F]+)`\s*\|\s*([^|]+?)\s*\|\s*([^|]+?)\s*\|")
FUNC_MAP_ROW_RE = re.compile(
    r"\|\s*`(0x[0-9a-fA-F]+)`\s*\|\s*`([^`]+)`\s*\|", re.I
)


@dataclass
class FuncRow:
    address: str
    name: str
    namespace: str
    module: str
    size: int
    proto: str = ""

    @property
    def address_hex(self) -> str:
        a = self.address.strip()
        if not a.startswith("0x"):
            a = f"0x{a}"
        return a.lower()

    @property
    def leaf(self) -> str:
        return self.name.rsplit("::", 1)[-1]


@dataclass
class DecompFuncInfo:
    raw_offset_count: int = 0
    raw_offsets: list[str] = field(default_factory=list)


@dataclass
class ClassHint:
    class_name: str
    struct_size_hint: str | None = None
    field_hints: list[dict[str, str]] = field(default_factory=list)
    doc_refs: list[str] = field(default_factory=list)
    anchor_from_docs: list[dict[str, str]] = field(default_factory=list)


def wave_out_dir(wave: int) -> Path:
    return REPO_ROOT / "ghidra_analysis" / f"type_recovery_wave{wave}"


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


def load_game_units() -> set[str]:
    if not UNITS_CSV.exists():
        return set()
    units: set[str] = set()
    with UNITS_CSV.open(encoding="utf-8", newline="") as f:
        for row in csv.reader(f):
            if not row:
                continue
            unit = row[0].strip()
            if unit and unit not in EXCLUDED_UNITS:
                units.add(unit)
            for cell in row[1:]:
                cell = cell.strip()
                if cell and "::" not in cell and cell not in EXCLUDED_UNITS:
                    units.add(cell.split("::", 1)[0])
    return units


def load_functions_from_mapping(ns_modules: dict[str, str]) -> list[FuncRow]:
    if not MAPPING_CSV.exists():
        return []
    rows: list[FuncRow] = []
    with MAPPING_CSV.open(encoding="utf-8", newline="") as f:
        for row in csv.reader(f, delimiter=";"):
            if len(row) < 4 or not row[1]:
                continue
            qname = row[1].strip()
            if "::" in qname:
                ns, _leaf = qname.rsplit("::", 1)
            else:
                ns, _leaf = "_Globals", qname
            try:
                size = int(row[3], 16)
            except ValueError:
                size = 0
            proto = row[4] if len(row) > 4 else ""
            rows.append(
                FuncRow(
                    address=row[2],
                    name=qname,
                    namespace=ns,
                    module=module_for_namespace(ns, ns_modules),
                    size=size,
                    proto=proto,
                )
            )
    return rows


def is_p1_namespace(ns: str) -> bool:
    if ns in P1_NAMESPACES:
        return True
    return any(ns.startswith(p) for p in P1_PREFIXES)


def priority_tier(ns: str) -> int:
    if ns in P0_NAMESPACES:
        return 0
    if is_p1_namespace(ns):
        return 1
    return 2


def is_excluded_namespace(ns: str, game_units: set[str], *, game_only: bool) -> bool:
    if EXCLUDED_NS_SUBSTR.search(ns):
        return True
    if ns in EXCLUDED_UNITS or ns == "_Globals":
        return True
    if game_only and game_units and ns not in game_units:
        # Allow nested ns like CListBox::CListBox if parent unit is game
        base = ns.split("::", 1)[0]
        if base not in game_units:
            return True
    return False


def load_excluded_addresses(globs: list[str]) -> set[str]:
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
                for key in ("address",):
                    addr = (item.get(key) or "").strip().lower()
                    if addr:
                        if not addr.startswith("0x"):
                            addr = f"0x{addr}"
                        excluded.add(addr)
                for anchor in item.get("anchor_functions") or []:
                    addr = (anchor.get("address") or "").strip().lower()
                    if addr:
                        if not addr.startswith("0x"):
                            addr = f"0x{addr}"
                        excluded.add(addr)
    return excluded


def build_decomp_index(path: Path) -> dict[str, DecompFuncInfo]:
    if not path.exists():
        return {}
    index: dict[str, DecompFuncInfo] = {}
    current_qname: str | None = None
    depth = 0
    info = DecompFuncInfo()
    offsets_seen: set[str] = set()

    def flush() -> None:
        nonlocal current_qname, depth, info, offsets_seen
        if current_qname:
            info.raw_offset_count = len(info.raw_offsets)
            index[current_qname] = info
        current_qname = None
        depth = 0
        info = DecompFuncInfo()
        offsets_seen = set()

    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        stripped = line.strip()
        if depth == 0:
            m = FUNC_HEADER_RE.match(stripped)
            if m:
                flush()
                _g1, leaf1, leaf2 = m.group(1), m.group(2), m.group(3)
                if leaf2:
                    current_qname = f"_Globals::{leaf2}"
                else:
                    ns = m.group(1).rsplit("::", 1)[0] if "::" in m.group(1) else "_Globals"
                    current_qname = f"{ns}::{leaf1}"
                info = DecompFuncInfo()
                offsets_seen = set()
                if "{" in stripped:
                    depth = stripped.count("{") - stripped.count("}")
                continue
            if current_qname and stripped == "{":
                depth = 1
                continue
            continue
        for off_m in RAW_OFFSET_RE.finditer(line):
            off = off_m.group(1).lower()
            if off not in offsets_seen:
                offsets_seen.add(off)
                info.raw_offsets.append(off)
        depth += line.count("{") - line.count("}")
        if depth <= 0:
            flush()

    flush()
    return index


def load_layout_hints() -> dict[str, ClassHint]:
    hints: dict[str, ClassHint] = {}
    for pattern in HINT_MD_GLOBS:
        for path in sorted(REPO_ROOT.glob(pattern)):
            if not path.is_file():
                continue
            rel = str(path.relative_to(REPO_ROOT)).replace("\\", "/")
            text = path.read_text(encoding="utf-8", errors="replace")
            current_class: str | None = None
            for line in text.splitlines():
                sec = CLASS_SECTION_RE.match(line)
                if sec:
                    current_class = sec.group(1)
                    if current_class not in hints:
                        hints[current_class] = ClassHint(class_name=current_class)
                    if rel not in hints[current_class].doc_refs:
                        hints[current_class].doc_refs.append(rel)
                    continue
                if not current_class:
                    continue
                ch = hints[current_class]
                ls = LAYOUT_SIZE_RE.search(line)
                if ls and not ch.struct_size_hint:
                    ch.struct_size_hint = ls.group(1).lower()
                fm = FIELD_ROW_RE.match(line)
                if fm and len(ch.field_hints) < 24:
                    ch.field_hints.append(
                        {
                            "offset": fm.group(1).lower().lstrip("+"),
                            "type": fm.group(2).strip(),
                            "name": fm.group(3).strip(),
                        }
                    )
                fmr = FUNC_MAP_ROW_RE.match(line)
                if fmr and len(ch.anchor_from_docs) < 8:
                    ch.anchor_from_docs.append(
                        {
                            "address": fmr.group(1).lower(),
                            "name": fmr.group(2).strip(),
                            "role": "doc_map",
                        }
                    )
    return hints


def load_vftables(ns_modules: dict[str, str], game_units: set[str], game_only: bool) -> list[dict[str, Any]]:
    if not VFTABLE_CSV.exists():
        return []
    grouped: dict[str, dict[str, Any]] = {}
    with VFTABLE_CSV.open(encoding="utf-8", newline="") as f:
        for row in csv.DictReader(f, delimiter=";"):
            cls = row["class_name"]
            if is_excluded_namespace(cls, game_units, game_only=game_only):
                continue
            addr = row["vftable_addr"].lower()
            if not addr.startswith("0x"):
                addr = f"0x{addr}"
            if addr not in grouped:
                grouped[addr] = {
                    "class_name": cls,
                    "slots": 0,
                    "unnamed_methods": 0,
                }
            g = grouped[addr]
            g["slots"] += 1
            if "FUN_" in row["method_name"]:
                g["unnamed_methods"] += 1
    rows: list[dict[str, Any]] = []
    for addr, g in grouped.items():
        cls = g["class_name"]
        rows.append(
            {
                "kind": "vtable_struct",
                "address": addr,
                "class_name": cls,
                "namespace": cls,
                "module": module_for_namespace(cls, ns_modules),
                "slot_count": g["slots"],
                "unnamed_method_count": g["unnamed_methods"],
                "notes": "",
            }
        )
    return rows


def pick_anchor_functions(
    class_name: str,
    by_ns: dict[str, list[FuncRow]],
    decomp: dict[str, DecompFuncInfo],
    hint: ClassHint | None,
    *,
    max_anchors: int = 5,
) -> list[dict[str, Any]]:
    anchors: list[dict[str, Any]] = []
    seen: set[str] = set()

    def add_row(fr: FuncRow, role: str) -> None:
        if fr.address_hex in seen:
            return
        seen.add(fr.address_hex)
        di = decomp.get(fr.name)
        anchors.append(
            {
                "address": fr.address_hex,
                "name": fr.name,
                "role": role,
                "size": fr.size,
                "raw_offset_hits": (di.raw_offsets[:12] if di else []),
            }
        )

    if hint:
        for a in hint.anchor_from_docs:
            addr = a["address"].lower()
            if not addr.startswith("0x"):
                addr = f"0x{addr}"
            if addr in seen:
                continue
            seen.add(addr)
            anchors.append(
                {
                    "address": addr,
                    "name": a.get("name", ""),
                    "role": a.get("role", "doc_map"),
                    "size": 0,
                    "raw_offset_hits": [],
                }
            )

    candidates = list(by_ns.get(class_name, []))
    role_order = (
        ("_ctor", 0),
        ("_BuildAt", 1),
        ("_dtor", 2),
        ("_Render", 3),
        ("_OnDraw", 4),
        ("_OnTick", 5),
    )

    def role_rank(leaf: str) -> tuple:
        for suffix, rank in role_order:
            if suffix in leaf:
                return (rank, -len(leaf))
        full = f"{class_name}::{leaf}"
        di = decomp.get(full)
        raw_n = di.raw_offset_count if di else 0
        return (10, -raw_n, -999)

    for fr in sorted(candidates, key=lambda r: (role_rank(r.leaf), -r.size, r.address_hex)):
        if len(anchors) >= max_anchors:
            break
        leaf = fr.leaf
        role = "method"
        for suffix, _ in role_order:
            if suffix in leaf:
                role = suffix.lstrip("_")
                break
        add_row(fr, role)
    return anchors[:max_anchors]


def build_class_struct_items(
    funcs: list[FuncRow],
    decomp: dict[str, DecompFuncInfo],
    hints: dict[str, ClassHint],
    game_units: set[str],
    ns_modules: dict[str, str],
    *,
    game_only: bool,
    class_quota: int,
    excluded: set[str],
) -> list[dict[str, Any]]:
    by_ns: dict[str, list[FuncRow]] = defaultdict(list)
    for f in funcs:
        if is_excluded_namespace(f.namespace, game_units, game_only=game_only):
            continue
        by_ns[f.namespace].append(f)

    class_scores: list[tuple[tuple, str]] = []
    for ns, flist in by_ns.items():
        if ns.startswith("FUN_"):
            continue
        raw_total = sum(decomp.get(f.name, DecompFuncInfo()).raw_offset_count for f in flist)
        has_hint = 1 if ns in hints else 0
        tier = priority_tier(ns)
        class_scores.append(((tier, -has_hint, -raw_total, -len(flist)), ns))

    class_scores.sort()
    items: list[dict[str, Any]] = []
    for _score, cls in class_scores:
        if len(items) >= class_quota:
            break
        if cls in EXCLUDED_UNITS:
            continue
        hint = hints.get(cls)
        anchors = pick_anchor_functions(cls, by_ns, decomp, hint)
        if not anchors and not hint:
            continue
        item: dict[str, Any] = {
            "kind": "class_struct",
            "class_name": cls,
            "namespace": cls,
            "module": module_for_namespace(cls, ns_modules),
            "struct_size_hint": hint.struct_size_hint if hint else None,
            "field_hints": hint.field_hints[:20] if hint else [],
            "anchor_functions": anchors,
            "doc_refs": hint.doc_refs if hint else [],
            "notes": "",
        }
        items.append(item)
    return items


def build_func_signature_items(
    funcs: list[FuncRow],
    decomp: dict[str, DecompFuncInfo],
    game_units: set[str],
    *,
    game_only: bool,
    min_size: int,
    excluded: set[str],
    anchor_addrs: set[str],
    limit: int,
) -> list[dict[str, Any]]:
    candidates: list[tuple[tuple, FuncRow, DecompFuncInfo]] = []
    for f in funcs:
        if f.address_hex in excluded or f.address_hex in anchor_addrs:
            continue
        if f.size < min_size:
            continue
        if is_excluded_namespace(f.namespace, game_units, game_only=game_only):
            continue
        if f.leaf.startswith("FUN_") or f.leaf.startswith("thunk_FUN_"):
            continue
        di = decomp.get(f.name)
        if not di or di.raw_offset_count == 0:
            continue
        tier = priority_tier(f.namespace)
        candidates.append(
            (
                (tier, -di.raw_offset_count, -f.size, f.address_hex),
                f,
                di,
            )
        )
    candidates.sort(key=lambda x: x[0])
    items: list[dict[str, Any]] = []
    for _key, f, di in candidates[:limit]:
        items.append(
            {
                "kind": "func_signature",
                "address": f.address_hex,
                "name": f.name,
                "namespace": f.namespace,
                "module": f.module,
                "size": f.size,
                "proto": f.proto,
                "raw_offset_count": di.raw_offset_count,
                "raw_offset_hits": di.raw_offsets[:16],
                "notes": "",
            }
        )
    return items


def priority_key_vft(item: dict[str, Any]) -> tuple:
    cls = item["class_name"]
    return (priority_tier(cls), -item.get("unnamed_method_count", 0), -item.get("slot_count", 0), item["address"])


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


def stats_by_kind(items: list[dict[str, Any]]) -> dict[str, int]:
    out: dict[str, int] = defaultdict(int)
    for it in items:
        out[it["kind"]] += 1
    return dict(out)


def write_agent_instructions(path: Path) -> None:
    path.write_text(
        """# Type recovery batch — agent instructions (evidence-only)

Program: **bulanci.exe** (pass `program: bulanci.exe` on every MCP call when multiple programs are open).

Read MCP tool schemas under `mcps/user-ghidra-mcp/tools/` before calling (`create_struct`, `add_struct_field`, `set_function_prototype`, `set_parameter_type`, `set_local_variable_type`, `decompile`, `save_program`, etc.).

## Rules (mandatory)

1. **Never invent types or field names.** Every struct field and prototype change needs evidence: decompiler body, xrefs, vtable layout, strings, or committed docs (`ghidra_analysis/*.md`, `widgets.md`).
2. Work **slowly**. Partial progress with `UNCERTAIN:` comments is correct; speculative typedefs are not.
3. Allowed Ghidra edits when confident:
   - `create_struct` / `add_struct_field` for class layouts (size + offsets must match evidence)
   - `set_function_prototype` / `set_parameter_type` / `set_local_variable_type` when `this` / `param_1` typing is justified
   - `set_decompiler_comment` or plate comment: `UNCERTAIN: <reason>` when not confident
4. Do **not** rename symbols in this wave unless required to apply a type (prefer type tools).
5. Cross-check batch `field_hints` / `doc_refs` against decompilation; docs can be stale.
6. Call **`save_program` once** after all Ghidra edits in the batch (not after every item).

## Per-batch workflow

1. Read `batch_NN.json` (up to 10 items).
2. For each item:
   - **`class_struct`**: use `anchor_functions` addresses — decompile, list observed `(this|param_1)+offset` uses, create/extend struct, re-decompile anchors to verify.
   - **`func_signature`**: decompile @ address; fix `this`/`param_1` type and prototype; ensure field accesses use struct members where evidenced.
   - **`vtable_struct`**: inspect slots in `ghidra_analysis/engine/vftable_methods.csv`; type function pointers only with evidence.
3. Write `batch_NN_results.json` beside the batch file:

```json
{
  "wave": 1,
  "batch": 1,
  "program": "bulanci.exe",
  "entries": [
    {
      "kind": "func_signature",
      "address": "0x00401c30",
      "status": "applied|partial|uncertain|skipped",
      "ghidra_applied": false,
      "struct_created": null,
      "prototype": null,
      "evidence": ["bullet 1"],
      "notes": ""
    }
  ]
}
```

`status` meanings: **applied** (types saved), **partial** (some fields/types), **uncertain** (comments only), **skipped** (no safe change).

## Static fallbacks

If MCP is down: use `bulanci.ghidra.exe.c`, `config/bulanci/mapping.csv`, and cited markdown — do not guess types.
""",
        encoding="utf-8",
    )


def write_baseline_metrics(path: Path, game_units: set[str]) -> None:
    if not DECOMP_C.exists():
        return
    this_re = re.compile(r"this\s*\+\s*0x", re.I)
    p1_re = re.compile(r"param_1\s*\+\s*0x", re.I)
    # Broader game header check: any listed unit prefix
    def is_game_header(line: str) -> bool:
        if "_Globals::" in line:
            return False
        for u in sorted(game_units, key=len, reverse=True):
            if f"{u}::" in line:
                return True
        return False

    total_this = 0
    total_p1 = 0
    game_this = 0
    game_p1 = 0
    in_game_fn = False
    depth = 0
    pending = False
    for line in DECOMP_C.read_text(encoding="utf-8", errors="replace").splitlines():
        stripped = line.strip()
        if depth == 0:
            if FUNC_HEADER_RE.match(stripped):
                in_game_fn = is_game_header(stripped)
                pending = "{" not in stripped
                if not pending:
                    depth = stripped.count("{") - stripped.count("}")
                continue
            if pending and stripped == "{":
                depth = 1
                pending = False
                continue
            continue
        if this_re.search(line):
            total_this += 1
            if in_game_fn:
                game_this += 1
        if p1_re.search(line):
            total_p1 += 1
            if in_game_fn:
                game_p1 += 1
        depth += line.count("{") - line.count("}")
        if depth <= 0:
            in_game_fn = False
            depth = 0

    path.write_text(
        json.dumps(
            {
                "source": str(DECOMP_C.relative_to(REPO_ROOT)),
                "decomp_total": {
                    "this_plus_offset_lines": total_this,
                    "param_1_plus_offset_lines": total_p1,
                },
                "game_namespace_functions": {
                    "this_plus_offset_lines": game_this,
                    "param_1_plus_offset_lines": game_p1,
                },
                "note": "Crude line counts inside functions whose header mentions a game unit from units_listing.csv",
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--wave", type=int, default=1)
    parser.add_argument("--batch-count", type=int, default=DEFAULT_BATCH_COUNT)
    parser.add_argument("--batch-size", type=int, default=DEFAULT_BATCH_SIZE)
    parser.add_argument("--max-items", type=int, default=DEFAULT_MAX_ITEMS)
    parser.add_argument("--out-dir", type=Path, default=None)
    parser.add_argument("--game-only", action="store_true")
    parser.add_argument("--min-size", type=int, default=32)
    parser.add_argument("--class-quota", type=int, default=25)
    parser.add_argument("--vtable-quota", type=int, default=10)
    parser.add_argument("--probe-mcp", action="store_true")
    parser.add_argument("--mcp-url", default=DEFAULT_MCP_URL)
    parser.add_argument("--no-instructions", action="store_true")
    parser.add_argument("--no-baseline-metrics", action="store_true")
    parser.add_argument(
        "--exclude-glob",
        action="append",
        default=[],
        metavar="GLOB",
        help="Repo-relative globs of prior batch JSON to skip addresses",
    )
    args = parser.parse_args()

    out_dir = args.out_dir or wave_out_dir(args.wave)
    ns_modules = load_namespace_modules()
    game_units = load_game_units()
    all_funcs = load_functions_from_mapping(ns_modules)
    excluded = load_excluded_addresses(args.exclude_glob)
    decomp = build_decomp_index(DECOMP_C)
    hints = load_layout_hints()

    class_items = build_class_struct_items(
        all_funcs,
        decomp,
        hints,
        game_units,
        ns_modules,
        game_only=args.game_only,
        class_quota=args.class_quota,
        excluded=excluded,
    )

    anchor_addrs: set[str] = set()
    for it in class_items:
        for a in it.get("anchor_functions") or []:
            addr = (a.get("address") or "").lower()
            if addr:
                anchor_addrs.add(addr)

    vft_all = load_vftables(ns_modules, game_units, game_only=args.game_only)
    vft_all.sort(key=priority_key_vft)
    vft_items = [v for v in vft_all if v["address"] not in excluded][: args.vtable_quota]

    cap = min(args.max_items, args.batch_count * args.batch_size)
    used = len(class_items) + len(vft_items)
    func_limit = max(0, cap - used)
    func_items = build_func_signature_items(
        all_funcs,
        decomp,
        game_units,
        game_only=args.game_only,
        min_size=args.min_size,
        excluded=excluded,
        anchor_addrs=anchor_addrs,
        limit=func_limit,
    )

    work_items: list[dict[str, Any]] = []
    # Interleave: spread class_struct across early batches
    ci = 0
    fi = 0
    vi = 0
    while len(work_items) < cap and (ci < len(class_items) or fi < len(func_items) or vi < len(vft_items)):
        if ci < len(class_items) and len(work_items) < cap:
            work_items.append(class_items[ci])
            ci += 1
        if vi < len(vft_items) and len(work_items) < cap:
            work_items.append(vft_items[vi])
            vi += 1
        if fi < len(func_items) and len(work_items) < cap:
            work_items.append(func_items[fi])
            fi += 1

    mcp_status = probe_mcp(args.mcp_url) if args.probe_mcp else {"probed": False}
    non_empty_batches = sum(
        1
        for i in range(args.batch_count)
        if work_items[i * args.batch_size : (i + 1) * args.batch_size]
    )
    top_classes = [it["class_name"] for it in class_items[:15]]

    summary: dict[str, Any] = {
        "wave": args.wave,
        "out_dir": str(out_dir.relative_to(REPO_ROOT)),
        "sources": {
            "mapping_csv": str(MAPPING_CSV.relative_to(REPO_ROOT)),
            "modules_csv": str(MODULES_CSV.relative_to(REPO_ROOT)),
            "units_listing_csv": str(UNITS_CSV.relative_to(REPO_ROOT)),
            "vftable_methods": str(VFTABLE_CSV.relative_to(REPO_ROOT)),
            "decomp_c": str(DECOMP_C.relative_to(REPO_ROOT)),
            "layout_hint_files": list(HINT_MD_GLOBS),
        },
        "filters": {
            "game_only": args.game_only,
            "min_size": args.min_size,
            "game_unit_count": len(game_units),
            "excluded_addresses_count": len(excluded),
        },
        "decomp_index": {
            "functions_indexed": len(decomp),
            "with_raw_offsets": sum(1 for v in decomp.values() if v.raw_offset_count > 0),
        },
        "layout_hints": {
            "classes_with_hints": len(hints),
            "classes_queued_with_hints": sum(1 for c in class_items if c.get("field_hints")),
        },
        "queue": {
            "max_items": args.max_items,
            "cap_applied": cap,
            "actual_items": len(work_items),
            "by_kind": stats_by_kind(work_items),
            "class_quota": args.class_quota,
            "class_struct_queued": len(class_items),
            "vtable_quota": args.vtable_quota,
            "vtable_struct_queued": len(vft_items),
            "func_signature_queued": len(func_items),
            "top_classes_queued": top_classes,
        },
        "batches": {
            "requested_batch_count": args.batch_count,
            "batch_size": args.batch_size,
            "non_empty_batches": non_empty_batches,
            "empty_batches": args.batch_count - non_empty_batches,
        },
        "ghidra_mcp": mcp_status,
    }

    out_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / "summary.json").write_text(
        json.dumps(summary, indent=2, ensure_ascii=False) + "\n",
        encoding="utf-8",
    )

    md = [
        "# Type recovery wave summary",
        "",
        f"- Wave: **{args.wave}** → `{out_dir.relative_to(REPO_ROOT)}`",
        f"- Work queue: **{len(work_items)}** items ({stats_by_kind(work_items)})",
        f"- Batches: **{non_empty_batches}** non-empty / {args.batch_count} requested",
        f"- Class structs queued: **{len(class_items)}** (top: {', '.join(top_classes[:8]) or '—'})",
        f"- Func signatures queued: **{len(func_items)}**",
        f"- Decomp functions with raw offsets: **{summary['decomp_index']['with_raw_offsets']}**",
        "",
    ]
    if mcp_status.get("available"):
        md.append(f"- Ghidra MCP: **available** ({mcp_status.get('current_program')})")
    elif args.probe_mcp:
        md.append(f"- Ghidra MCP: **unavailable** ({mcp_status.get('error', '?')})")
    (out_dir / "summary.md").write_text("\n".join(md) + "\n", encoding="utf-8")

    if not args.no_instructions:
        write_agent_instructions(out_dir / "agent_instructions.md")

    for batch_idx in range(args.batch_count):
        start = batch_idx * args.batch_size
        chunk = work_items[start : start + args.batch_size]
        payload = {
            "wave": args.wave,
            "batch": batch_idx + 1,
            "batch_of": args.batch_count,
            "batch_size": args.batch_size,
            "items": chunk,
            "item_count": len(chunk),
        }
        (out_dir / f"batch_{batch_idx + 1:02d}.json").write_text(
            json.dumps(payload, indent=2, ensure_ascii=False) + "\n",
            encoding="utf-8",
        )

    if not args.no_baseline_metrics:
        write_baseline_metrics(out_dir / "metrics_baseline.json", game_units)

    print(f"Wrote {out_dir}")
    print(f"  work queue: {len(work_items)} items, {non_empty_batches} non-empty batches")
    print(f"  kinds: {stats_by_kind(work_items)}")
    print(f"  top classes: {', '.join(top_classes[:10])}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
