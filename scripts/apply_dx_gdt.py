"""Deprecated alias for `scripts/apply_gdt.py`.

Kept for backwards compatibility with shell history / older docs.
"""
from __future__ import annotations

import sys

from apply_gdt import main as _main


if __name__ == "__main__":
    raise SystemExit(_main(sys.argv[1:]))
