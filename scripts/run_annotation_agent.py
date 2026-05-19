"""Bundle everything an annotation agent needs to work on one class.

Usage:

    # Claim and dump a class to stdout (JSON):
    python scripts/run_annotation_agent.py --agent agent-073 --pick

    # Work on a specific class you already claimed:
    python scripts/run_annotation_agent.py --agent agent-073 --class CBulanci

    # Just dump context without claiming (read-only inspection):
    python scripts/run_annotation_agent.py --class CBulanci --readonly

The output is a single JSON document an LLM agent can consume. The
agent is expected to:

  1. Read ``state/agent_prompts/annotate_class.md`` for the task spec.
  2. Use the Ghidra MCP HTTP API (default ``http://localhost:8089``)
     to rename / retype / restructure the class.
  3. Call ``scripts/agent_coord.py release <CLASS> --agent <ID>
     --attempted <funcs>`` when done.

The script does NOT call an LLM itself - it just bundles context. Wire
up your preferred agent runner around it (Cursor, Claude SDK, OpenAI
API, ...). The bundle is structured so an agent prompt can include
the whole thing verbatim and still fit in a typical context window.

MCP base URL is taken from $GHIDRA_MCP_URL (default
http://localhost:8089). Endpoints are defined by the ghidra-mcp
plugin; we treat any 404 as "endpoint not available" so older plugin
builds keep working.
"""

from __future__ import annotations

import argparse
import csv
import json
import os
import sys
import urllib.error
import urllib.parse
import urllib.request
from collections import defaultdict
from pathlib import Path
from typing import Any

REPO_ROOT = Path(os.path.realpath(__file__)).resolve().parents[1]
sys.path.insert(0, str(REPO_ROOT / "scripts"))

from agent_coord import ClaimStore  # noqa: E402  (after path tweak)

MAPPING_FILE = REPO_ROOT / "config" / "bulanci" / "mapping.csv"
UNITS_FILE = REPO_ROOT / "config" / "bulanci" / "units_listing.csv"
PROMPT_FILE = REPO_ROOT / "docs" / "agent_prompts" / "annotate_class.md"

MCP_BASE = os.environ.get("GHIDRA_MCP_URL", "http://localhost:8089").rstrip("/")
MCP_TIMEOUT = 8.0


# --------------------------------------------------------------------- mapping

def _load_class_functions(class_name: str) -> list[dict[str, Any]]:
    """Read every function attributed to `class_name` from mapping.csv."""
    out: list[dict[str, Any]] = []
    if not MAPPING_FILE.exists():
        return out
    with MAPPING_FILE.open(newline="", encoding="utf-8") as f:
        for row in csv.reader(f, delimiter=";"):
            if not row or len(row) < 5:
                continue
            # Row layout: ;<qualified_name>;<addr>;<size>;<cc>;;<ret>;<args...>
            qname = row[1]
            if "::" not in qname:
                if class_name != "_Globals":
                    continue
                ns, leaf = "_Globals", qname
            else:
                ns, leaf = qname.rsplit("::", 1)
            if ns != class_name:
                continue
            out.append({
                "qualified_name": qname,
                "leaf_name": leaf,
                "address": row[2],
                "size_bytes": int(row[3], 0) if row[3] else 0,
                "calling_convention": row[4] or None,
                "return_type": row[6] if len(row) > 6 else "",
                "param_types": row[7:] if len(row) > 7 else [],
                "is_default_named": leaf.startswith("FUN_"),
            })
    out.sort(key=lambda r: int(r["address"], 0))
    return out


def _load_class_list() -> list[str]:
    """Distinct class namespaces known to the build."""
    if not UNITS_FILE.exists():
        return []
    seen: set[str] = set()
    with UNITS_FILE.open(newline="", encoding="utf-8") as f:
        for row in csv.reader(f):
            if row:
                seen.add(row[0])
    return sorted(seen)


# ------------------------------------------------------------------------ MCP

def _mcp_get(path: str, **params: Any) -> dict[str, Any] | list[Any] | None:
    if params:
        path = path + "?" + urllib.parse.urlencode(params)
    url = f"{MCP_BASE}{path}"
    try:
        with urllib.request.urlopen(url, timeout=MCP_TIMEOUT) as r:
            body = r.read()
            ct = r.headers.get("content-type", "")
            if ct.startswith("application/json"):
                return json.loads(body)
            return {"text": body.decode("utf-8", "replace")}
    except urllib.error.HTTPError as e:
        if e.code == 404:
            return None
        return {"error": f"HTTP {e.code} {e.reason}"}
    except (urllib.error.URLError, ConnectionError, TimeoutError) as e:
        return {"error": str(e)}


def _probe_mcp() -> dict[str, Any]:
    """Quick aliveness check + program metadata."""
    status = _mcp_get("/server/status")
    meta = _mcp_get("/get_metadata") or {}
    return {
        "base_url": MCP_BASE,
        "status": status,
        "program": meta,
    }


def _per_function_context(fn_row: dict[str, Any]) -> dict[str, Any]:
    """Best-effort enrichment via MCP. Always returns the mapping row."""
    out: dict[str, Any] = dict(fn_row)
    name = fn_row["qualified_name"]
    addr = fn_row["address"]

    callers = _mcp_get("/get_function_callers", address=addr)
    callees = _mcp_get("/get_function_callees", address=addr)
    xrefs = _mcp_get("/get_function_xrefs", address=addr)

    out["mcp"] = {
        "callers": _shrink_call_list(callers),
        "callees": _shrink_call_list(callees),
        "xrefs":   _shrink_call_list(xrefs),
    }
    return out


def _shrink_call_list(payload: Any) -> Any:
    """Strip the response down to (name, address) pairs to fit prompts."""
    if payload is None or isinstance(payload, dict) and "error" in payload:
        return payload
    if isinstance(payload, dict):
        items = payload.get("callers") or payload.get("callees") or \
                payload.get("xrefs") or payload.get("results") or payload.get("data") or []
    elif isinstance(payload, list):
        items = payload
    else:
        return payload
    out = []
    for it in items[:32]:
        if isinstance(it, dict):
            out.append({
                "name": it.get("name") or it.get("function") or it.get("symbol"),
                "address": it.get("address") or it.get("from") or it.get("to"),
            })
        else:
            out.append({"raw": str(it)})
    return out


# --------------------------------------------------------------------- bundle

def build_bundle(class_name: str, include_mcp: bool = True) -> dict[str, Any]:
    fns = _load_class_functions(class_name)
    bundle: dict[str, Any] = {
        "schema_version": 1,
        "class_name": class_name,
        "function_count": len(fns),
        "default_named_count": sum(1 for f in fns if f["is_default_named"]),
        "total_bytes": sum(f["size_bytes"] for f in fns),
        "prompt_path": str(PROMPT_FILE.relative_to(REPO_ROOT)),
        "mcp_base_url": MCP_BASE,
        "functions": fns,
    }
    if include_mcp:
        bundle["mcp_probe"] = _probe_mcp()
        if bundle["mcp_probe"].get("status") is not None:
            bundle["functions"] = [_per_function_context(f) for f in fns]
    bundle["sibling_classes"] = _siblings(class_name)
    return bundle


def _siblings(class_name: str) -> list[dict[str, Any]]:
    """Per-class size summary so the agent knows neighbours by code mass."""
    by_class: dict[str, list[int]] = defaultdict(list)
    if not MAPPING_FILE.exists():
        return []
    with MAPPING_FILE.open(newline="", encoding="utf-8") as f:
        for row in csv.reader(f, delimiter=";"):
            if not row or len(row) < 4:
                continue
            qname = row[1]
            ns = qname.rsplit("::", 1)[0] if "::" in qname else "_Globals"
            try:
                size = int(row[3], 0)
            except ValueError:
                size = 0
            by_class[ns].append(size)
    rows = [
        {"class": cls, "function_count": len(sizes), "total_bytes": sum(sizes)}
        for cls, sizes in by_class.items()
    ]
    rows.sort(key=lambda r: -r["total_bytes"])
    return rows[:15]


# ---------------------------------------------------------------------- main

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--agent", help="Agent ID for claim tracking.")
    parser.add_argument("--class", dest="class_name",
                        help="Class namespace to bundle. Required unless --pick.")
    parser.add_argument("--pick", action="store_true",
                        help="Atomically pick an unclaimed class for --agent.")
    parser.add_argument("--readonly", action="store_true",
                        help="Skip claim/release; just print context.")
    parser.add_argument("--no-mcp", action="store_true",
                        help="Don't probe the Ghidra MCP server.")
    parser.add_argument("--prompt", action="store_true",
                        help="Print the prompt template first, then the JSON bundle.")
    args = parser.parse_args()

    if args.pick and not args.agent:
        parser.error("--pick requires --agent.")
    if not args.pick and not args.class_name:
        parser.error("either --class or --pick is required.")

    store = ClaimStore()
    class_name = args.class_name

    if args.pick:
        # Use the same algorithm as agent_coord.py "pick" command.
        from agent_coord import _load_units
        snapshot = store.snapshot()
        import time
        now = time.time()
        free = [u for u in _load_units() if u not in snapshot or snapshot[u].is_expired(now)]
        for cls in free:
            if store.claim(cls, args.agent):
                class_name = cls
                print(f"# claimed {cls} for {args.agent}", file=sys.stderr)
                break
        if class_name is None:
            print("(no unclaimed classes)", file=sys.stderr)
            return 1
    elif not args.readonly:
        if args.agent and not store.claim(class_name, args.agent):
            print(f"BUSY: {class_name} is held by another agent", file=sys.stderr)
            return 1

    bundle = build_bundle(class_name, include_mcp=not args.no_mcp)

    if args.prompt:
        if PROMPT_FILE.exists():
            sys.stdout.write(PROMPT_FILE.read_text(encoding="utf-8"))
            sys.stdout.write("\n\n----- CONTEXT BUNDLE -----\n\n")
        else:
            sys.stdout.write(f"(prompt file missing: {PROMPT_FILE})\n\n")
    json.dump(bundle, sys.stdout, indent=2)
    sys.stdout.write("\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
