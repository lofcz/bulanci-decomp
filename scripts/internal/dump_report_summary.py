"""One-shot helper: print a compact summary of report.json."""
from __future__ import annotations

import json
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]


def main() -> int:
    path = REPO_ROOT / "report.json"
    if not path.exists():
        print(f"report.json missing at {path}")
        return 1
    r = json.loads(path.read_text(encoding="utf-8"))
    m = r["measures"]
    print(f"units:             {len(r['units'])}")
    print(f"total functions:   {m.get('total_functions', 0)}")
    print(f"matched functions: {m.get('matched_functions', 0)}")
    print(f"matched fn %:      {m.get('matched_functions_percent', 0):.3f}%")
    print(f"total code (B):    {m.get('total_code', 0)}")
    print(f"matched code (B):  {m.get('matched_code', 0)}")
    print(f"matched code %:    {m.get('matched_code_percent', 0):.3f}%")
    print(f"complete units:    {m.get('complete_units', 0)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
