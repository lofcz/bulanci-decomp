"""Pull Ghidra MCP context for one function and feed it into decomp-goal.

Wires the running Ghidra (via the ghidra-mcp HTTP plugin on
``http://localhost:8089``) into ``decomp-goal-harness``'s steering /
decompilers store. For one function this writes:

  * the decompiled C (``--source ghidra``)
  * the disassembly (``--source ghidra-asm``)

so subsequent ``decomp-goal goal --unit <U>`` prompts can ingest the
ground-truth body the agent should be re-implementing.

Usage:

    python scripts/ghidra_to_decomp_goal.py \\
        --unit CDSBitmap \\
        --function CDSBitmap::FUN_0040a000 \\
        --address 0x0040a000

If ``--function`` is omitted, the script enumerates every function in
``--unit`` from ``mapping.csv`` and feeds them all (cheap; each call is
a single HTTP GET + one ``decomp-goal decompilers`` invocation).

Requires:
  * Ghidra is open with bulanci.exe and the ghidra-mcp plugin bound
    to :8089 (override via ``$GHIDRA_MCP_URL``).
  * ``decomp-goal`` is on PATH (or its venv path in the default
    location used by ``scripts/seed_steering.py``).
"""
from __future__ import annotations

import argparse
import csv
import os
import shutil
import subprocess
import sys
import tempfile
import urllib.error
import urllib.parse
import urllib.request
from pathlib import Path

REPO_ROOT = Path(os.path.realpath(__file__)).resolve().parents[1]
MAPPING_FILE = REPO_ROOT / "config" / "bulanci" / "mapping.csv"
MCP_BASE = os.environ.get("GHIDRA_MCP_URL", "http://localhost:8089").rstrip("/")
DECOMP_GOAL_BIN = (
    Path(r"C:\Users\mstagl-dev\Documents\GitHub\decomp-goal-harness\.venv\Scripts\decomp-goal.exe")
    if os.name == "nt"
    else Path("decomp-goal")
)


def _resolve_decomp_goal() -> Path | str:
    if DECOMP_GOAL_BIN.exists():
        return DECOMP_GOAL_BIN
    found = shutil.which("decomp-goal")
    if found:
        return Path(found)
    raise SystemExit("decomp-goal not found - install the harness or pass --bin.")


def _mcp_get(path: str, **params) -> str:
    """GET the MCP endpoint; return the raw text body (Response.text)."""
    if params:
        path = path + "?" + urllib.parse.urlencode(params)
    url = f"{MCP_BASE}{path}"
    try:
        with urllib.request.urlopen(url, timeout=60) as r:
            body = r.read().decode("utf-8", "replace")
        return body
    except urllib.error.HTTPError as e:
        return f"// MCP HTTP {e.code} {e.reason} for {url}\n"
    except (urllib.error.URLError, ConnectionError, TimeoutError) as e:
        raise SystemExit(f"MCP unreachable at {MCP_BASE}: {e}")


def _list_unit_functions(unit: str) -> list[tuple[str, str]]:
    """Return [(qualified_name, address), ...] for a unit from mapping.csv."""
    out: list[tuple[str, str]] = []
    if not MAPPING_FILE.exists():
        return out
    with MAPPING_FILE.open(newline="", encoding="utf-8") as f:
        for row in csv.reader(f, delimiter=";"):
            if not row or len(row) < 3:
                continue
            qname = row[1]
            addr = row[2]
            ns = qname.rsplit("::", 1)[0] if "::" in qname else "_Globals"
            if ns == unit:
                out.append((qname, addr))
    return out


def _push_lead(bin_path: Path | str, unit: str, function: str,
               source: str, text_or_file: str, is_file: bool,
               notes: str | None = None) -> None:
    cmd = [
        str(bin_path),
        "decompilers",
        "--repo", str(REPO_ROOT),
        "--unit", unit,
        "--function", function,
        "--source", source,
    ]
    if is_file:
        cmd += ["--file", text_or_file]
    else:
        cmd += ["--pseudocode", text_or_file]
    if notes:
        cmd += ["--notes", notes]
    subprocess.run(cmd, check=True, cwd=str(REPO_ROOT))


def _push_function(bin_path: Path | str, unit: str, qname: str, addr: str) -> None:
    print(f"[ghidra->decomp-goal] {qname} @ {addr}")
    pseudo = _mcp_get("/decompile_function", address=addr)
    asm = _mcp_get("/disassemble_function", address=addr)

    with tempfile.TemporaryDirectory(prefix="ghidra_lead_") as tmp:
        pseudo_path = Path(tmp) / "ghidra_decomp.c"
        asm_path = Path(tmp) / "ghidra_asm.txt"
        pseudo_path.write_text(pseudo, encoding="utf-8")
        asm_path.write_text(asm, encoding="utf-8")
        _push_lead(bin_path, unit, qname, "ghidra", str(pseudo_path),
                   is_file=True,
                   notes=f"Decompiled by Ghidra at address {addr}.")
        _push_lead(bin_path, unit, qname, "ghidra-asm", str(asm_path),
                   is_file=True,
                   notes=f"Disassembly snapshot from Ghidra at {addr}.")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--unit", required=True)
    parser.add_argument("--function", default=None,
                        help="Qualified function name. If omitted, every "
                             "function in --unit is exported.")
    parser.add_argument("--address", default=None,
                        help="Function entry point (only needed when "
                             "--function is given and not in mapping.csv).")
    parser.add_argument("--limit", type=int, default=0,
                        help="Cap the number of functions exported (0 = no cap).")
    args = parser.parse_args()

    bin_path = _resolve_decomp_goal()
    targets: list[tuple[str, str]]

    if args.function:
        addr = args.address
        if not addr:
            # Look up the address from mapping.csv
            for qname, qaddr in _list_unit_functions(args.unit):
                if qname == args.function:
                    addr = qaddr
                    break
        if not addr:
            raise SystemExit(
                f"address not provided and {args.function} not found in mapping.csv"
            )
        targets = [(args.function, addr)]
    else:
        targets = _list_unit_functions(args.unit)
        if args.limit:
            targets = targets[: args.limit]
    if not targets:
        raise SystemExit(f"no functions for unit {args.unit}")

    print(f"using {bin_path}")
    print(f"unit {args.unit}: feeding {len(targets)} function(s) from Ghidra MCP at {MCP_BASE}")
    for qname, addr in targets:
        _push_function(bin_path, args.unit, qname, addr)
    print("Done. The leads will appear in the next `decomp-goal goal` / `codex` prompt.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
