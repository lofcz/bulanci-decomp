"""Throwaway: pretty-print objdiff side-by-side for a single function."""
import json
import subprocess
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]


def get_diff(unit, fn):
    cmd = [
        str(REPO / "tools" / "objdiff-cli.exe"),
        "diff",
        "--project",
        str(REPO),
        "--unit",
        unit,
        "--output",
        "-",
        "--format",
        "json",
        fn,
    ]
    p = subprocess.run(cmd, capture_output=True, text=True, check=True)
    return json.loads(p.stdout)


def fmt_func(symbols, want):
    for s in symbols:
        if s.get("kind") == "SYMBOL_FUNCTION" and s.get("name") == want:
            lines = []
            for ins in s.get("instructions", []):
                i = ins.get("instruction")
                if not i:
                    lines.append((-2, "<gap>", 0))
                    continue
                lines.append((int(i.get("address", "0")), i["formatted"], i["size"]))
            return s.get("size", "?"), lines
    return None, []


def summary(unit):
    data = get_diff(unit, "CDSResourceSign::ReadDateField")
    print(f"OVERALL match: {data['left']['sections'][0]['match_percent']:.2f}%")
    print()
    print(f"  {'FUNCTION':<55s} {'SIZE':>5s} {'MATCH':>8s}")
    for s in data["left"]["symbols"]:
        if s.get("kind") == "SYMBOL_FUNCTION" and s.get("size") and int(s["size"]) > 0:
            name = s["name"]
            sz = s["size"]
            pct = s.get("match_percent")
            pct_s = f"{pct:.1f}%" if pct is not None else "??"
            print(f"  {name:<55s} {sz:>5s} {pct_s:>8s}")


def main():
    if len(sys.argv) == 2:
        summary(sys.argv[1])
        return
    unit = sys.argv[1]
    fn = sys.argv[2]
    data = get_diff(unit, fn)
    lsize, lleft = fmt_func(data["left"]["symbols"], fn)
    rsize, rright = fmt_func(data["right"]["symbols"], fn)
    print(f"{fn} | target={lsize}B | built={rsize}B")
    n = max(len(lleft), len(rright))
    print(f"  {'TARGET':<45s} | {'BUILT':<45s}")
    print("  " + "-" * 45 + " | " + "-" * 45)
    for i in range(n):
        l = lleft[i] if i < len(lleft) else (-1, "", 0)
        r = rright[i] if i < len(rright) else (-1, "", 0)
        lstr = "" if l[0] < 0 else f"{l[0]:>3}: {l[1]}"
        rstr = "" if r[0] < 0 else f"{r[0]:>3}: {r[1]}"
        print(f"  {lstr:<45s} | {rstr:<45s}")


if __name__ == "__main__":
    main()
