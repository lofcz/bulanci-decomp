"""Quick health-check that counts how many functions in
``config/bulanci/mapping.csv`` carry a "rich" signature.

A signature is *rich* when at least one argument or the return type is
not a default Ghidra primitive / opaque pointer. We use this to gauge
how far the decompiler's type propagation has reached.

Outputs three line-counters and a top-types histogram so before/after
diffs of repeated CommitDecompilerSignatures runs are obvious.
"""
from __future__ import annotations

import argparse
import sys
from collections import Counter
from pathlib import Path

PRIMITIVES = {
    "void",
    "bool", "char", "uchar", "short", "ushort", "int", "uint",
    "long", "ulong", "longlong", "ulonglong", "float", "double",
    "wchar_t", "size_t", "ptrdiff_t",
}


def _is_primitive(ty: str) -> bool:
    base = ty.rstrip("*").rstrip()
    return base in PRIMITIVES or base in {"undefined", "undefined1",
                                          "undefined2", "undefined4", "undefined8"}


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--mapping", default="config/bulanci/mapping.csv")
    parser.add_argument("--top", type=int, default=30)
    args = parser.parse_args(argv)

    rows = 0
    rich = 0
    total_params = 0
    typed_params = 0
    type_hist: Counter[str] = Counter()

    with Path(args.mapping).open("r", encoding="utf-8") as f:
        for line in f:
            line = line.rstrip("\n")
            if not line.strip():
                continue
            parts = line.split(";")
            if len(parts) < 7:
                continue
            rows += 1
            ret_ty = parts[6]
            argtys = parts[7:]
            is_rich = not _is_primitive(ret_ty)
            for a in argtys:
                if a == "":
                    continue
                total_params += 1
                if not _is_primitive(a):
                    typed_params += 1
                    is_rich = True
                    type_hist[a] += 1
            if is_rich:
                rich += 1

    print(f"rows                     {rows}")
    print(f"rich signatures          {rich}  ({rich/max(rows,1)*100:5.2f}%)")
    print(f"params total             {total_params}")
    print(f"params with rich types   {typed_params}  ({typed_params/max(total_params,1)*100:5.2f}%)")
    print(f"\ntop {args.top} non-primitive arg types:")
    for ty, n in type_hist.most_common(args.top):
        print(f"  {n:5d}  {ty}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
