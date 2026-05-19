"""Deprecated alias for `scripts/build_gdt.py --target dx`.

Kept for backwards compatibility with shell history / older docs.
"""
from __future__ import annotations

import sys

from build_gdt import main as _main


if __name__ == "__main__":
    args = list(sys.argv[1:])
    if "--target" not in args:
        args = ["--target", "dx", *args]
    raise SystemExit(_main(args))
