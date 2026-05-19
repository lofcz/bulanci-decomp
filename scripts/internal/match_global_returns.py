"""Auto-match tiny `return &DAT_xxxxxxxx;` functions.

Pattern 1 in `near_miss.py`'s `OFF` bucket: hundreds of size-6
methods that compile to `mov eax, offset DAT_xxxxxxxx; ret`.  All
the boilerplate is identical; the only per-function variable is
which DAT_ symbol gets returned.

This script:

  1. Reads `config/bulanci/mapping.csv` and picks candidates with
     `size == 6`, return type starting with `uchar*` (or any pointer),
     no stack args.
  2. Asks Ghidra MCP for each one's decompilation via `batch_decompile`.
  3. If the decompilation is exactly `return &DAT_xxxxxxxx;` (modulo
     whitespace/casts), rewrites the matching `// !FUNC <addr>` block
     in `src/bulanci/<unit>.cpp` to use that body.
  4. Reports counts.

Requires Ghidra MCP running on http://localhost:8089 (or wherever
`GHIDRA_MCP_URL` points) and the externs header to already include
the relevant DAT_ symbols (`scripts/internal/extract_externs.py`).
"""

import argparse
import json
import os
import re
import sys
import urllib.request
import urllib.parse
from pathlib import Path

WORKSPACE = Path(__file__).resolve().parents[2]
MCP_URL = os.environ.get("GHIDRA_MCP_URL", "http://127.0.0.1:8089")

# Match the Ghidra decompiler's `return &DAT_xxxxxxxx;` line.  The
# decompiler always returns CRLF-terminated source, and Python's
# `$` anchor in MULTILINE mode doesn't play nicely with CR before LF,
# so we don't anchor.  The decompiler also never wraps trivial getters
# across lines, so a substring search is unambiguous.
RETURN_DAT = re.compile(r"return\s*&(?P<sym>DAT_[0-9a-fA-F]{8})\s*;")

FUNC_BLOCK = re.compile(
    r"(// !FUNC 0x(?P<addr>[0-9a-fA-F]+) BEGIN\s*\n"
    r"/\* [^\n]*\*/\s*\n"
    r")(?P<sig>[^\n]+?)\s*\{[^}]*STUB_BODY\([^}]*\}\s*(\n// !FUNC 0x[0-9a-fA-F]+ END)",
    re.MULTILINE)


def mcp_get(path, params=None):
    url = MCP_URL.rstrip("/") + "/" + path.lstrip("/")
    if params:
        url += "?" + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=30) as f:
        return f.read().decode("utf-8", "replace")


def batch_decompile(addresses):
    """Call Ghidra MCP `batch_decompile` and return dict addr->source."""
    fns = ",".join(addresses)
    raw = mcp_get("batch_decompile", {"functions": fns})
    try:
        return json.loads(raw)
    except json.JSONDecodeError:
        # Some MCP endpoints return double-encoded JSON.
        return json.loads(json.loads(raw))


def read_mapping(path):
    rows = []
    for line in path.read_text(encoding="utf-8").splitlines():
        parts = line.split(";")
        if len(parts) < 7 or not parts[1]:
            continue
        try:
            size = int(parts[3], 16)
        except ValueError:
            continue
        rows.append({
            "name": parts[1], "addr": parts[2], "size": size,
            "conv": parts[4], "ret": parts[6],
            "args": [a for a in parts[7:] if a],
        })
    return rows


def find_candidates(mapping_rows, target_size):
    out = []
    for r in mapping_rows:
        if r["size"] != target_size:
            continue
        if r["args"]:
            continue
        # Want plain pointer returns (uchar*, void*, char*, ...).
        if not r["ret"].endswith("*"):
            continue
        if "::vftable" in r["name"]:
            continue
        if "Catch_" in r["name"] or "Unwind_" in r["name"]:
            continue
        out.append(r)
    return out


def patch_block(text, addr, dat_sym, ret_type):
    """Replace the stub block at `// !FUNC 0xaddr BEGIN` with the matched body."""
    pattern = re.compile(
        r"(// !FUNC 0x" + addr + r" BEGIN\s*\n"
        r"/\* [^\n]*\*/\s*\n)"
        r"([^\n]+?)\s*\{[^}]*STUB_BODY\([^}]*\}",
        re.MULTILINE)
    def _sub(m):
        sig = m.group(2).rstrip()
        return (
            m.group(1)
            + sig + " {\n"
            + f"    return reinterpret_cast<{ret_type}>(&{dat_sym});\n"
            + "}"
        )
    new, n = pattern.subn(_sub, text, count=1)
    return new, n


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--mapping", type=Path,
                        default=WORKSPACE / "config/bulanci/mapping.csv")
    parser.add_argument("--src-dir", type=Path,
                        default=WORKSPACE / "src/bulanci")
    parser.add_argument("--externs", type=Path,
                        default=WORKSPACE / "include/bulanci/_externs.h")
    parser.add_argument("--size", type=int, default=6,
                        help="Target function size (default 6 for "
                             "`mov eax, offset DAT; ret`).")
    parser.add_argument("--limit", type=int, default=None,
                        help="Stop after N matches.  Useful for smoke "
                             "tests; default is no limit.")
    parser.add_argument("--batch", type=int, default=15,
                        help="How many functions to ask Ghidra to "
                             "decompile in one MCP call.  The MCP "
                             "endpoint silently caps batches around "
                             "20; keep this below that to be safe.")
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    rows = read_mapping(args.mapping)
    candidates = find_candidates(rows, args.size)
    print(f"size-{args.size} pointer-return candidates: {len(candidates)}")

    if args.limit is not None:
        candidates = candidates[:args.limit * 4]

    extern_set = set(re.findall(r"extern unsigned char (DAT_[0-9a-fA-F]{8})",
                                args.externs.read_text()))
    print(f"DAT_ externs available: {len(extern_set)}")

    # Bucket candidates by their containing unit (so we read+write
    # each .cpp at most once).
    unit_for_class = {}
    units_csv = WORKSPACE / "config/bulanci/units_listing.csv"
    for line in units_csv.read_text(encoding="utf-8").splitlines():
        parts = [p for p in line.split(",") if p]
        if not parts:
            continue
        unit = parts[0]
        for ns in parts[1:]:
            unit_for_class[ns] = unit

    matches: dict[str, list[tuple[str, str, str]]] = {}
    total_seen = 0
    for i in range(0, len(candidates), args.batch):
        chunk = candidates[i:i + args.batch]
        addrs = [r["addr"] for r in chunk]
        try:
            decomp = batch_decompile(addrs)
        except Exception as e:
            print(f"  MCP error on chunk {i}: {e}", file=sys.stderr)
            continue
        for r in chunk:
            src = decomp.get(r["addr"]) or decomp.get(r["addr"].lower())
            if not src:
                continue
            m = RETURN_DAT.search(src)
            if not m:
                continue
            sym = m.group("sym")
            if sym not in extern_set:
                continue
            cls = r["name"].split("::")[0]
            unit = unit_for_class.get(cls)
            if not unit:
                continue
            matches.setdefault(unit, []).append((r["addr"], sym, r["ret"]))
            total_seen += 1
            if args.limit is not None and total_seen >= args.limit:
                break
        if args.limit is not None and total_seen >= args.limit:
            break

    print(f"matched function bodies to rewrite: {total_seen} "
          f"across {len(matches)} unit(s)")

    if args.dry_run:
        for unit, rows in matches.items():
            print(f"  {unit}: {len(rows)}")
            for addr, sym, ret in rows[:5]:
                print(f"    0x{addr.lower().lstrip('0x'):>8s} -> {sym} ({ret})")
            if len(rows) > 5:
                print(f"    ... ({len(rows) - 5} more)")
        return 0

    written = 0
    for unit, rows in matches.items():
        path = args.src_dir / f"{unit}.cpp"
        if not path.exists():
            print(f"  ! skip {unit}: {path} missing")
            continue
        text = path.read_text(encoding="utf-8")
        n_changed = 0
        for addr, sym, ret in rows:
            addr_lc = addr.lower().lstrip("0x").zfill(8)
            text, n = patch_block(text, addr_lc, sym, ret)
            n_changed += n
        if n_changed:
            path.write_text(text, encoding="utf-8")
            print(f"  + {unit}.cpp: {n_changed} block(s) rewritten")
            written += n_changed

    print(f"{written} function bodies rewritten across {len(matches)} unit(s)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
