#!/usr/bin/env python3
"""Surface the cheapest functions to push us across 0.5% matched code."""
import json
import sys

r = json.load(open("report.json", "r"))
m = r["measures"]
matched = int(m["matched_code"])
total = int(m["total_code"])
target_bytes = int(total * 0.005)
needed = target_bytes - matched
print(f"current matched: {matched} bytes ({m['matched_code_percent']:.4f}%)")
print(f"target  matched: {target_bytes} bytes (0.5000%)")
print(f"need approximately {needed} more matched bytes")
print()

candidates = []
for unit in r["units"]:
    for f in unit.get("functions", []):
        pct = f.get("fuzzy_match_percent", 0.0)
        size = int(f.get("size", 0))
        if pct >= 100.0 or size <= 0:
            continue
        candidates.append((unit["name"], f["name"], size, pct, f.get("address", "0")))

print(f"=== Top 40 by descending fuzzy% (already close, just need finishing) ===")
candidates_by_fuzzy = sorted(candidates, key=lambda x: (-x[3], x[2]))
print(f"  {'unit':<32} {'function':<55} {'size':>5} {'fuzzy':>7}")
for c in candidates_by_fuzzy[:40]:
    print(f"  {c[0]:<32} {c[1]:<55} {c[2]:>5} {c[3]:>7.2f}%")

print()
print(f"=== Top 30 small unmatched (<= 60 bytes) by fuzzy%, easier to finish ===")
small = [c for c in candidates if c[2] <= 60]
small.sort(key=lambda x: (-x[3], x[2]))
print(f"  {'unit':<32} {'function':<55} {'size':>5} {'fuzzy':>7}")
for c in small[:30]:
    print(f"  {c[0]:<32} {c[1]:<55} {c[2]:>5} {c[3]:>7.2f}%")
