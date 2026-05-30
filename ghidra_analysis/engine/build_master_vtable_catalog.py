"""Build master vtable summary from Ghidra-exported vftable_methods.csv.

Run after DumpVftablesCsv (Ghidra MCP inline script writes
ghidra_analysis/engine/vftable_methods.csv).

    python ghidra_analysis/engine/build_master_vtable_catalog.py
"""

from __future__ import annotations

import csv
import re
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SLOT_CSV = Path(__file__).resolve().parent / "vftable_methods.csv"
OUT_CSV = Path(__file__).resolve().parent / "master_vtable_catalog.csv"
OUT_MD = Path(__file__).resolve().parent / "master_vtable_catalog.md"
DECOMP = ROOT / "bulanci.ghidra.exe.c"

# Globals / bases not emitted as Class::vftable symbols (see app_shell.md).
MANUAL_VFTABLES: list[dict[str, object]] = [
    {
        "class_name": "CDSApp",
        "vftable_addr": "0048700c",
        "obj_offset": 0,
        "role": "primary",
        "slots": 34,
        "slot0": "CDSApp_GetClassTable",
        "slot1": "CDSApp_DtorScalar",
        "slot2": "CDSApp_dtor",
        "notes": "g_pCDSApp_vftable; COL@00487008; documented in app_shell.md",
    },
]

# Typical MI face sizes in this binary (for role inference).
FACE_HINTS: list[tuple[str, int, int]] = [
    ("primary", 24, 40),
    ("IDSChained", 5, 7),
    ("IDSEventHandler", 4, 6),
    ("IDSReferenced", 3, 5),
    ("IDSUpdated", 3, 6),
    ("IDSAnim", 4, 6),
    ("CDSObject", 2, 4),
    ("CDSChain", 2, 7),
    ("stream", 3, 6),
]


def infer_role(slot_count: int, addr: int) -> str:
    for name, lo, hi in FACE_HINTS:
        if lo <= slot_count <= hi:
            return name
    return f"face_{slot_count}slots"


def parse_ctor_offsets() -> dict[str, dict[int, str]]:
    """Map class -> {obj_offset: vftable_addr} from mov [reg+off], offset patterns."""
    text = DECOMP.read_text(encoding="utf-8", errors="replace")
    pat = re.compile(
        r"\*\((?:undefined \*\*\*|u32 \*)\)\((?:this|param_1)(?: \+ (0x[0-9a-f]+))?\)\s*=\s*"
        r"([A-Za-z0-9_]+)::vftable",
    )
    out: dict[str, dict[int, str]] = defaultdict(dict)
    for m in pat.finditer(text):
        off_s, cls = m.group(1), m.group(2)
        off = int(off_s, 16) if off_s else 0
        # vftable symbol address unknown here; keyed later by class+off
        out[cls][off] = ""
    return out


def load_slots() -> dict[tuple[str, str], list[dict[str, str]]]:
    by_vt: dict[tuple[str, str], list[dict[str, str]]] = defaultdict(list)
    with SLOT_CSV.open(encoding="utf-8") as f:
        for row in csv.DictReader(f, delimiter=";"):
            by_vt[(row["class_name"], row["vftable_addr"])].append(row)
    return by_vt


def short_name(fn: str) -> str:
    if fn == "__no_func__":
        return fn
    return fn.split("::")[-1] if "::" in fn else fn


def main() -> None:
    by_vt = load_slots()
    ctor_offs = parse_ctor_offsets()
    rows_out: list[dict[str, str]] = []

    # Manual entries first.
    for m in MANUAL_VFTABLES:
        rows_out.append({k: str(v) for k, v in m.items()})

    classes = sorted({k[0] for k in by_vt})
    for cls in classes:
        vts = sorted(
            [(addr, slots) for (c, addr), slots in by_vt.items() if c == cls],
            key=lambda x: int(x[0], 16),
        )
        # Infer object offsets: sort by address within class, match ctor offset keys.
        ctor_map = ctor_offs.get(cls, {})
        sorted_ctor_offs = sorted(ctor_map.keys())

        for i, (addr, slots) in enumerate(vts):
            n = len(slots)
            s0 = short_name(slots[0]["method_name"]) if n else ""
            s1 = short_name(slots[1]["method_name"]) if n > 1 else ""
            s2 = short_name(slots[2]["method_name"]) if n > 2 else ""
            pure = sum(1 for s in slots if s["is_pure_virtual"] == "1")

            # Heuristic obj offset: largest vtable is usually +0.
            if n >= 24:
                role = "primary"
                obj_off = 0
            elif i < len(sorted_ctor_offs):
                obj_off = sorted_ctor_offs[i]
            else:
                obj_off = ""
                role = infer_role(n, int(addr, 16))
            if n >= 24:
                role = "primary"
            elif obj_off == 4:
                role = "IDSChained"
            elif obj_off == 0x10:
                role = "IDSEventHandler"
            elif obj_off == 0x18:
                role = "IDSReferenced"
            elif obj_off in (0x68, 0x6c, 0x78):
                role = {0x68: "IDSUpdated", 0x6c: "IDSAnim", 0x78: "IDSAnim_chain"}.get(
                    obj_off, infer_role(n, int(addr, 16))
                )
            else:
                role = infer_role(n, int(addr, 16))

            key_methods = "; ".join(
                f"[{j}]={short_name(slots[j]['method_name'])}"
                for j in range(min(n, 6))
            )

            rows_out.append(
                {
                    "class_name": cls,
                    "vftable_addr": f"0x{addr}",
                    "obj_offset": f"0x{obj_off:x}" if isinstance(obj_off, int) else "",
                    "role": role,
                    "slots": str(n),
                    "purecall_slots": str(pure),
                    "slot0": s0,
                    "slot1": s1,
                    "slot2": s2,
                    "key_methods": key_methods,
                    "notes": "",
                }
            )

    fieldnames = [
        "class_name",
        "vftable_addr",
        "obj_offset",
        "role",
        "slots",
        "purecall_slots",
        "slot0",
        "slot1",
        "slot2",
        "key_methods",
        "notes",
    ]
    with OUT_CSV.open("w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=fieldnames)
        w.writeheader()
        w.writerows(rows_out)

    # Markdown summary for priority CDS* / game classes.
    priority_prefixes = (
        "CDS",
        "CGame",
        "CGaming",
        "CLevel",
        "CBulanci",
        "CBulanek",
        "CMenu",
        "CGameView",
        "CMovie",
        "CWindow",
        "CButton",
        "CEdit",
    )
    lines = [
        "# Master vtable catalog",
        "",
        f"Generated from `{SLOT_CSV.name}` ({len(by_vt)} Ghidra `vftable` symbols, "
        f"{sum(len(v) for v in by_vt.values())} slots).",
        "",
        "## Destructor pattern",
        "",
        "MSVC Itanium-style layout is consistent across CDS/game types:",
        "",
        "| slot | typical role |",
        "|------|----------------|",
        "| 0 | `GetClassTable` / type metadata |",
        "| 1 | scalar-deleting dtor (`~Class` + `operator delete`) |",
        "| 2 | complete object destructor body |",
        "",
        "Primary **CDSView**-shaped faces use 28 slots (widgets) or **34** for `CDSApp`/`CBulanci` app shell.",
        "Shared MI thunks at `+0x04/+0x10/+0x18` reuse `CDSChain` / input faces; `+0x68/+0x6c` are update/anim.",
        "",
        "## Priority classes (summary)",
        "",
        "| class | vftable | off | role | # | slot0 | slot1 | slot2 |",
        "|-------|---------|-----|------|---|-------|-------|-------|",
    ]
    for r in rows_out:
        if not any(r["class_name"].startswith(p) for p in priority_prefixes):
            continue
        lines.append(
            f"| {r['class_name']} | {r['vftable_addr']} | {r['obj_offset']} | {r['role']} | "
            f"{r['slots']} | `{r['slot0']}` | `{r['slot1']}` | `{r['slot2']}` |"
        )

    lines += [
        "",
        "## CDSApp / CDSView / CBulanci inheritance",
        "",
        "- `CDSApp` MI bases: `0x00486fc0` (+0x18), `0x00486fd4` (+0x10), `0x00486fec` (+0x04).",
        "- `CDSApp` **34-slot primary** at `0x0048700c` (`g_pCDSApp_vftable`) — see `app_shell.md`.",
        "- `CDSView` **28-slot primary** at `0x0047f954`; widgets (`CMenu`, `CSwitch`, …) patch this table.",
        "- `CBulanci` **34-slot** app override at `0x0047f834` (replaces app entry points).",
        "",
        "## Still-unnamed slot targets",
        "",
        "~393 faces list `FUN_*` in early slots (mostly shared MI thunks).",
        "",
        "## Full spreadsheet",
        "",
        f"See `{OUT_CSV.name}` for all {len(rows_out)} rows.",
        "",
    ]
    OUT_MD.write_text("\n".join(lines), encoding="utf-8")
    print(f"Wrote {OUT_CSV} ({len(rows_out)} rows)")
    print(f"Wrote {OUT_MD}")


if __name__ == "__main__":
    main()
