"""Patch vftable_methods.csv method_name from _cds_vtable_renames.tsv."""
import csv
from pathlib import Path

ROOT = Path(__file__).resolve().parent
TSV = ROOT / "_cds_vtable_renames.tsv"
CSV = ROOT / "vftable_methods.csv"

renames = {}
for line in TSV.read_text(encoding="utf-8").splitlines()[1:]:
    addr, name = line.split("\t", 1)
    renames[addr.strip().lower()] = name.strip()

rows = []
updated = 0
with CSV.open(encoding="utf-8", newline="") as f:
    reader = csv.DictReader(f, delimiter=";")
    fields = reader.fieldnames
    for row in reader:
        addr = row["method_addr"].lower()
        if addr in renames:
            new = f"{row['class_name']}::{renames[addr]}"
            if row["method_name"] != new:
                row["method_name"] = new
                updated += 1
        rows.append(row)

with CSV.open("w", encoding="utf-8", newline="") as f:
    writer = csv.DictWriter(f, fieldnames=fields, delimiter=";")
    writer.writeheader()
    writer.writerows(rows)

print(f"Updated {updated} rows in {CSV}")
