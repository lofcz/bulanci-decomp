"""One-shot objdiff inspector for the libmad ports in CDSMpx.

Runs `objdiff-cli` against `bulanci/CDSMpx` and prints a compact table of
the per-symbol match% for the six bit.c functions we just hand-ported, plus
optional per-instruction diff for a single symbol.

Usage:
    python scripts/_tmp_inspect_mpx.py             # summary table
    python scripts/_tmp_inspect_mpx.py mad_bit_skip   # per-instruction diff
"""

import json
import subprocess
import sys

UNIT = "bulanci/CDSMpx"
TARGETS = [
    "CDSMpx::mad_bit_init",
    "CDSMpx::mad_bit_length",
    "CDSMpx::mad_bit_nextbyte",
    "CDSMpx::mad_bit_skip",
    "CDSMpx::mad_bit_read",
    "CDSMpx::mad_bit_crc",
    "CDSMpx::mad_stream_sync",
    "CDSMpx::mad_header_init",
    "CDSMpx::mad_frame_mute",
    "CDSMpx::mad_frame_init",
    "CDSMpx::mad_synth_mute",
    "CDSMpx::mad_synth_init",
]

result = subprocess.run(
    [
        r"tools/objdiff-cli.exe",
        "diff",
        "--project",
        ".",
        "--unit",
        UNIT,
        "--format",
        "json",
        "--output",
        "-",
    ],
    capture_output=True,
    text=False,
    check=True,
)
doc = json.loads(result.stdout)
left = doc.get("left", {})
right = doc.get("right", {})
# In objdiff JSON, `left` is `target_path` (the delinker-produced ORIG obj)
# and `right` is `base_path` (the obj we just compiled, "mine").
orig_syms = {s["name"]: s for s in left.get("symbols", [])}
mine_syms = {s["name"]: s for s in right.get("symbols", [])}

want = sys.argv[1] if len(sys.argv) > 1 else None
if want is None:
    print(f"  {'symbol':35s} {'size':>5s}  match%")
    print(f"  {'-' * 35} {'-' * 5}  ------")
    for name in TARGETS:
        o = orig_syms.get(name)
        m = mine_syms.get(name)
        size = o.get("size") if o else m.get("size") if m else "?"
        mp = o.get("match_percent") if o else None
        mp_str = f"{mp:.1f}%" if mp is not None else "  -  "
        marker = "" if o else "  (no orig)"
        print(f"  {name:35s} {size:>5}  {mp_str}{marker}")
    sec = next((s for s in left.get("sections", []) if s["name"] == ".text"), {})
    print()
    print(
        f"  CDSMpx .text: {sec.get('match_percent', 0):.2f}%  ({sec.get('size')} B)"
    )
else:
    for label, syms in (("ORIG (target_path)", orig_syms),
                        ("MINE (base_path)", mine_syms)):
        sym = next(
            (s for n, s in syms.items() if want in n), None
        )
        if not sym:
            print(f"{label}: no symbol matching '{want}'")
            continue
        print(f"=== {label}: {sym['name']!r} size={sym.get('size')} match={sym.get('match_percent')} ===")
        for ins in sym.get("instructions", []):
            i = ins.get("instruction") or {}
            fmt = i.get("formatted", "")
            addr = i.get("address", "0")
            diff = ins.get("diff_kind") or ""
            relocs = ins.get("relocations") or i.get("relocation") or []
            tag = f" [{diff}]" if diff else ""
            print(f"    @{addr} {fmt}{tag}")
