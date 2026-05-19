#! /usr/bin/env python3
"""Per-function byte-equality scan across all paired units.

For each unit in report.json, compare the .text bytes of every paired
symbol in target vs base.  Report:

  EXACT   - bytes identical, exact same size       (objdiff will mark these matched)
  CLOSE   - same size, <=10% of bytes differ        (signature is right, body needs tweaks)
  LENGTH  - size differs by <=2 bytes              (likely a calling-conv / arg mismatch)
  OFF     - bigger size gap (sigs probably wrong)

Symbols are paired by exact raw-name equality, which the demangler in
`compile_unit.py` already ensures by rewriting our compiled .obj's
MSVC-mangled names down to Ghidra's qualified form.

Run this after a clean build:
    ninja && python scripts/internal/near_miss.py
"""

import argparse
import json
import struct
from pathlib import Path


def coff_function_bytes(path: Path) -> dict[str, bytes]:
    """Return {symbol_name: function_bytes} for every code function."""
    data = path.read_bytes()
    n_sections = struct.unpack_from("<H", data, 2)[0]
    sym_tbl_off = struct.unpack_from("<I", data, 8)[0]
    n_syms = struct.unpack_from("<I", data, 12)[0]
    opthdr = struct.unpack_from("<H", data, 16)[0]

    # Section table starts after file header + optional header.
    sec_tbl = 20 + opthdr
    sections = []
    for i in range(n_sections):
        s_off = sec_tbl + 40 * i
        name = data[s_off:s_off + 8].rstrip(b"\x00").decode("latin1", "replace")
        raw_size = struct.unpack_from("<I", data, s_off + 16)[0]
        raw_ptr = struct.unpack_from("<I", data, s_off + 20)[0]
        sections.append((name, raw_size, raw_ptr))

    # Walk symbol table.
    strtbl = sym_tbl_off + 18 * n_syms
    syms = []
    i = 0
    while i < n_syms:
        off = sym_tbl_off + 18 * i
        if data[off:off + 4] == b"\x00\x00\x00\x00":
            name_off = struct.unpack_from("<I", data, off + 4)[0]
            end = data.index(b"\x00", strtbl + name_off)
            name = data[strtbl + name_off:end].decode("latin1", "replace")
        else:
            name = data[off:off + 8].rstrip(b"\x00").decode("latin1", "replace")
        value = struct.unpack_from("<I", data, off + 8)[0]
        section_num = struct.unpack_from("<h", data, off + 12)[0]
        storage_cls = data[off + 16]
        n_aux = data[off + 17]
        syms.append((name, value, section_num, storage_cls, n_aux))
        i += 1 + n_aux

    # Build a sorted list of (section_num, value, name) for code symbols
    # so we can infer each function's size from the next symbol in its section.
    code_syms = [(sec, val, name)
                 for (name, val, sec, sc, _) in syms
                 if sec > 0 and sec <= len(sections)
                 and sections[sec - 1][0].startswith(".text")
                 and sc == 2]
    code_syms.sort()
    out: dict[str, bytes] = {}
    for j, (sec, val, name) in enumerate(code_syms):
        s_name, s_size, s_off = sections[sec - 1]
        if j + 1 < len(code_syms) and code_syms[j + 1][0] == sec:
            end = code_syms[j + 1][1]
        else:
            end = s_size
        if end <= val:
            continue
        out[name] = data[s_off + val:s_off + end]
    return out


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--report", type=Path, default=Path("report.json"))
    parser.add_argument("--close-threshold", type=float, default=0.1,
                        help="If <=N fraction of bytes differ at equal length, "
                             "call it CLOSE (default 0.10).")
    parser.add_argument("--print", choices=("EXACT", "CLOSE", "LENGTH", "OFF", "ALL"),
                        default="EXACT", help="Which bucket to print.")
    parser.add_argument("--limit", type=int, default=200)
    args = parser.parse_args()

    cfg = json.load(open("objdiff.json"))
    units = {u["name"]: (Path(u["target_path"]), Path(u["base_path"]))
             for u in cfg["units"]}

    totals = {"EXACT": 0, "CLOSE": 0, "LENGTH": 0, "OFF": 0, "UNPAIRED": 0}
    rows = {"EXACT": [], "CLOSE": [], "LENGTH": [], "OFF": []}

    for uname, (tpath, bpath) in units.items():
        if not tpath.exists() or not bpath.exists():
            continue
        tfns = coff_function_bytes(tpath)
        bfns = coff_function_bytes(bpath)
        for name, tb in tfns.items():
            if name not in bfns:
                totals["UNPAIRED"] += 1
                continue
            bb = bfns[name]
            ts, bs = len(tb), len(bb)
            if tb == bb:
                bucket = "EXACT"
                diffs = 0
            elif ts == bs and ts > 0:
                diffs = sum(1 for x, y in zip(tb, bb) if x != y)
                bucket = "CLOSE" if diffs / ts <= args.close_threshold else "OFF"
            elif abs(ts - bs) <= 2:
                bucket = "LENGTH"
                diffs = abs(ts - bs)
            else:
                bucket = "OFF"
                diffs = abs(ts - bs)
            totals[bucket] += 1
            rows[bucket].append((ts, bs, diffs, uname, name))

    print("== summary ==")
    for k in ("EXACT", "CLOSE", "LENGTH", "OFF", "UNPAIRED"):
        print(f"  {k:10s} {totals[k]}")
    print()
    bucket = args.print
    if bucket == "ALL":
        buckets = ("EXACT", "CLOSE", "LENGTH", "OFF")
    else:
        buckets = (bucket,)
    for b in buckets:
        print(f"== {b} (up to {args.limit}) ==")
        for ts, bs, d, u, n in sorted(rows[b])[:args.limit]:
            print(f"  tsz={ts:<4d} bsz={bs:<4d} diff={d:<4d}  {u}::{n}")


if __name__ == "__main__":
    main()
