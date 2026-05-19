import json, sys

path = sys.argv[1]
target = sys.argv[2]
d = json.load(open(path))


def dump(side, sym):
    print(f"=== {side}: {sym['name']} ({sym.get('size')} B) match={sym.get('match_percent')}%")
    for diff in sym.get("instructions", []):
        ins = diff.get("instruction")
        if ins is None:
            print(f"  ----  <gap>  {diff.get('branch_dest', '')}")
            continue
        addr = ins.get("address", "0")
        text = ins.get("formatted", "?")
        dk = diff.get("diff_kind", "")
        tag = f"[{dk}]" if dk else ""
        print(f"  {addr:>5}  {text:50s} {tag}")


for side in ("left", "right"):
    obj = d.get(side) or {}
    syms = obj.get("symbols", [])
    sym = next((s for s in syms if s.get("name") == target), None)
    if sym:
        dump(side, sym)
        print()
