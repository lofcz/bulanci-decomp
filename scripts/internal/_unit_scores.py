"""Quick-look at per-unit match status for chosen units."""
import json
import sys

want = sys.argv[1:] if len(sys.argv) > 1 else None
r = json.load(open("report.json"))
for u in r["units"]:
    if want and not any(w in u["name"] for w in want):
        continue
    m = u["measures"]
    pct = float(m.get("matched_code_percent", 0))
    mf = int(m.get("matched_functions", 0))
    tf = int(m["total_functions"])
    mb = int(m.get("matched_code", 0))
    tb = int(m["total_code"])
    print(f'{u["name"]:38s} mf={mf}/{tf:<4d} mb={mb:>5d}/{tb:<6d} {pct:6.2f}%')
