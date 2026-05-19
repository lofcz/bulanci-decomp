"""Group Ghidra's 140 namespaces into ~20 logical modules.

`bulanci.exe`'s code is partitioned by Ghidra's RTTI analyzer plus our
own promotion passes into ~140 namespaces (one per recovered C++ class
plus the CRT / STL / ATL holdovers). For everyday navigation and
progress reporting that granularity is too fine; this script rolls
them up into the conceptual subsystems the original developers built:
DataStudio engine layers (`Engine.DS.*`), game-specific logic
(`Game.*`), and platform/runtime glue (`Runtime.*`).

Inputs (produced by the Ghidra MCP scripts in this session):

* `config/bulanci/ghidra_functions_dump.csv`
    `address;size;namespace;name;is_thunk;has_custom_name;calling_convention`
    One row per function. Sizes are hex strings (no `0x` prefix).
* `config/bulanci/vftable_methods.csv`
    `class_name;vftable_addr;slot;method_addr;method_name;is_pure_virtual`
    One row per vftable slot. Used only to count vftables per class.

Outputs:

* `config/bulanci/modules.csv`
    `module,namespace,function_count,total_bytes,vftable_count,first_addr,last_addr`
* `docs/MODULES.md`
    Human-readable summary: layered overview, per-module class list,
    address-range layout, residual `_Globals` breakdown.

Run from repo root:

    python scripts/internal/build_module_map.py
"""
from __future__ import annotations

import csv
from collections import Counter, defaultdict
from dataclasses import dataclass, field
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
DUMP_FUNCS = REPO_ROOT / "config" / "bulanci" / "ghidra_functions_dump.csv"
DUMP_VFTBL = REPO_ROOT / "config" / "bulanci" / "vftable_methods.csv"
OUT_MODULES = REPO_ROOT / "config" / "bulanci" / "modules.csv"
OUT_DOC = REPO_ROOT / "docs" / "MODULES.md"


# ----------------------------------------------------------------------
# Module assignments
# ----------------------------------------------------------------------
#
# Each entry is `module_id -> (description, [namespaces...])`. The lists
# are ordered alphabetically; new namespaces are easy to drop in.
# Anything not listed here falls into a fallback bucket (see
# `assign_module`).

MODULE_DEFS: dict[str, tuple[str, list[str]]] = {
    # ---- DataStudio engine (CDS* prefix in the original code) -------
    "Engine.DS.Core": (
        "Object/lifecycle base classes, application shell, scripting.",
        [
            "CDSObject", "CDSChain", "CDSChained", "CDSCollection",
            "CDSDSPtr", "CDSWorkingThread", "CDSApp", "CDSScript",
            "CDSView",
        ],
    ),
    "Engine.DS.Stream": (
        "Stream I/O: memory, file, filter, compression, queue.",
        [
            "CDSEasyMemStream", "CDSFileStream", "CDSFilterStream",
            "CDSGZipStream", "CDSGZipStreamData", "CDSMemQueue",
            "CDSQueueStream", "CDSSafeStream", "CDSSafeStreamInfo",
            "CDSStreamStorage",
        ],
    ),
    "Engine.DS.Resource": (
        "Resource archives (.eap), DSM/FLX containers, signatures.",
        [
            "CDSResInfo", "CDSResourceSign", "CDSStrmResInfo",
            "CDSStrmStgLoadingInfo", "CDSUpdatedItem", "CDSDsmFile",
            "CDSFlxFile",
        ],
    ),
    "Engine.DS.Graphics": (
        "2D rendering: bitmaps, back buffer, fonts, JPEG, animations.",
        [
            "CDSBitmap", "CDSBackBuffer", "CDSBmpImage", "CDSImage",
            "CDSJpegImage", "CDSFont", "CDSImageMouse", "CDSAnim",
            "CDSMouse", "ODSImage",
        ],
    ),
    "Engine.DS.Audio": (
        "DirectSound wrapper, WAV decoders, audio-bank playback.",
        [
            "CDSAudioBank", "CDSAudioBankSample", "CDSAudioPlayer",
            "CDSAudioVideoPlayer", "CDSWav", "CDSWavStream",
            "CDSDirectSound",
        ],
    ),
    "Engine.DS.Video": (
        "MPX-format video playback.",
        [
            "CDSVideoPlayer", "CDSMpx", "CDSMpxDecoder", "CDSMpxStream",
        ],
    ),
    "Engine.DS.Input": (
        "DirectInput keyboard wrapper.",
        ["CDirectKeyb"],
    ),
    "Engine.DS.Network": (
        "DirectPlay multiplayer transport.",
        ["CDSDirectPlay", "CDSDirectPlaySender"],
    ),
    "Engine.DS.Exception": (
        "Exception class hierarchy (DirectX, registry, resource, ...).",
        [
            "CDSException", "CDSStreamException", "CDSResourceException",
            "CDSMemoryException", "CDSSimpleException",
            "CDSApiException", "CDSRegKeyException",
            "CDSDirectXException",
        ],
    ),
    "Engine.DS.Text": (
        "Localised UI text store.",
        ["CDSStaticTexts"],
    ),
    "Engine.DS.Interface": (
        "Pure-abstract interface classes (RTTI only, no member code).",
        [
            "IDSAnim", "IDSAudioSource", "IDSBitmap", "IDSClonable",
            "IDSEventHandler", "IDSImage", "IDSInterface",
            "IDSReferenced", "IDSResource", "IDSRunnable", "IDSStorage",
            "IDSStream", "IDSStreamable", "IDSSubject", "IDSTexts",
            "IDSUpdated", "IDSUserData", "IDSVideoSource",
        ],
    ),

    # ---- Game-specific code ----------------------------------------
    "Game.Entities": (
        "Player characters, death animations, combat objects.",
        [
            "CBulanci", "CBulanek", "CBulPicture", "CBulAnim", "CDeath",
            "CDeath2", "CRuch", "CShot", "CWeapon", "CMina", "CObstacle",
            "CExplosion", "CIcon", "CSpells", "CTeleportPoint",
            "CItemInfo",
        ],
    ),
    "Game.Logic": (
        "Match / level orchestration, scoring, loading.",
        [
            "CGame", "CGaming", "CGameCounter", "CShotCounter",
            "CNumCounter", "CScore", "CLevelList", "CLevelScore",
            "CLevelScript", "CLoadingLevel",
        ],
    ),
    "Game.View": (
        "Full-screen views (game, help, history, intro/outro movies).",
        [
            "CBlackView", "CGameView", "CHelpView", "CHistoryView",
            "CMovieView",
        ],
    ),
    "Game.Dialog": (
        "Modal dialogs (exit, setup, pause, message, ...).",
        [
            "CExitDlg", "CGameTypeDlg", "CHelpDlg", "CHistoryDlg",
            "CMsgDialog", "CPauseDlg", "CSetupDlg", "CTcpIpConfig",
        ],
    ),
    "Game.UI.Widget": (
        "Reusable UI widgets (buttons, edits, scrollers, menus, ...).",
        [
            "CButton", "CColorSet", "CColorSwitch", "CChatEdit",
            "CChatList", "CEdit", "CGunMouse", "CKeybShow", "CListBox",
            "CListBoxItem", "CListViewer", "CMenu", "CNumEdit", "CPanel",
            "CPoem", "CPoemScroller", "CProgressBar", "CRadio",
            "CScrollBar", "CScroller", "CScoreItem", "CSessionItem",
            "CSessionList", "CStartGame1", "CStartGame2", "CStaticText",
            "CSwitch", "CVolume", "CWindow", "CBitmap", "CAnim",
        ],
    ),
    "Game.Script": (
        "Scripted UI sequences (help, history book).",
        ["CHelpScript", "CHistoryScript"],
    ),
    "Game.Misc": (
        "Top-level glue not yet classified into another module.",
        ["CAdvertising"],
    ),

    # ---- Platform / runtime ---------------------------------------
    "Runtime.ATL": (
        "ATL helpers and CAtlException.",
        ["ATL", "CAtlException"],
    ),
    "Runtime.STL": (
        "MSVC STL exception classes + RTTI helpers.",
        [
            "std", "std::exception", "std::bad_alloc",
            "std::bad_exception", "type_info", "exception",
            "_LocaleUpdate",
        ],
    ),
    "Runtime.Imports": (
        "DLL import-thunk namespaces created by Ghidra.",
        [
            "KERNEL32.DLL", "USER32.DLL", "GDI32.DLL", "OLE32.DLL",
            "ADVAPI32.DLL", "WINMM.DLL", "DDRAW.DLL", "DINPUT.DLL",
        ],
    ),
    # `_Globals` is everything still at file scope; broken down further
    # in the markdown report under "Residual _Globals breakdown".
    "_Globals": (
        "Functions not yet attributed to a class (CRT, EH glue, "
        "unclassified game/engine code).",
        [""],
    ),
}


def _module_index() -> dict[str, str]:
    """Flat namespace -> module-id lookup."""
    out: dict[str, str] = {}
    for module, (_desc, namespaces) in MODULE_DEFS.items():
        for ns in namespaces:
            out[ns] = module
    return out


def assign_module(namespace: str, lookup: dict[str, str]) -> str:
    """Resolve a namespace to its module, applying fallback heuristics."""
    if namespace == "" or namespace == "Global":
        return "_Globals"
    if namespace in lookup:
        return lookup[namespace]
    # Conservative fallbacks for namespaces not in the hardcoded table:
    # IDS* are abstract bases by convention; CDS* engine, C* game.
    if namespace.startswith("IDS"):
        return "Engine.DS.Interface"
    if namespace.startswith("CDS"):
        return "Engine.DS.Core"
    if namespace.startswith("std"):
        return "Runtime.STL"
    if namespace.endswith(".DLL"):
        return "Runtime.Imports"
    if namespace.startswith("C"):
        return "Game.Misc"
    return "_Globals"


# ----------------------------------------------------------------------
# CSV ingest
# ----------------------------------------------------------------------

@dataclass
class FuncRow:
    address: int
    size: int
    namespace: str
    name: str
    is_thunk: bool


def _load_funcs() -> list[FuncRow]:
    rows: list[FuncRow] = []
    with DUMP_FUNCS.open(newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f, delimiter=";")
        for r in reader:
            try:
                addr = int(r["address"], 16)
                size = int(r["size"], 16)
            except (KeyError, ValueError):
                continue
            rows.append(FuncRow(
                address=addr,
                size=size,
                namespace=(r.get("namespace") or "").strip(),
                name=(r.get("name") or "").strip(),
                is_thunk=(r.get("is_thunk") == "1"),
            ))
    return rows


def _load_vftable_counts() -> Counter:
    counts: Counter = Counter()
    if not DUMP_VFTBL.exists():
        return counts
    seen: set[tuple[str, str]] = set()
    with DUMP_VFTBL.open(newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f, delimiter=";")
        for r in reader:
            cls = (r.get("class_name") or "").strip()
            vft = (r.get("vftable_addr") or "").strip()
            if not cls or not vft:
                continue
            key = (cls, vft)
            if key in seen:
                continue
            seen.add(key)
            counts[cls] += 1
    return counts


# ----------------------------------------------------------------------
# Per-namespace + per-module roll-up
# ----------------------------------------------------------------------

@dataclass
class NamespaceStat:
    namespace: str
    module: str
    function_count: int = 0
    total_bytes: int = 0
    vftable_count: int = 0
    first_addr: int = 0xFFFFFFFF
    last_addr: int = 0


@dataclass
class ModuleStat:
    module: str
    description: str
    namespaces: list[NamespaceStat] = field(default_factory=list)

    @property
    def function_count(self) -> int:
        return sum(ns.function_count for ns in self.namespaces)

    @property
    def total_bytes(self) -> int:
        return sum(ns.total_bytes for ns in self.namespaces)

    @property
    def vftable_count(self) -> int:
        return sum(ns.vftable_count for ns in self.namespaces)


def _residual_globals_breakdown(funcs: list[FuncRow]) -> list[tuple[str, int, int]]:
    """Partition the file-scope functions by name pattern, so the
    `_Globals` row in the modules CSV is not a black hole."""
    buckets: dict[str, list[int]] = defaultdict(list)
    for f in funcs:
        if f.namespace and f.namespace != "Global":
            continue
        name = f.name
        if name.startswith("thunk_FUN_"):
            bucket = "thunk_FUN"
        elif name.startswith("FUN_"):
            bucket = "FUN_unattributed"
        elif name.startswith("Catch@"):
            bucket = "Catch@"
        elif name.startswith("Unwind@"):
            bucket = "Unwind@"
        elif name.startswith("switchD_"):
            bucket = "switchD_"
        elif name == "CreateObject":
            bucket = "CreateObject"
        elif name.startswith("_") or name.startswith("$"):
            bucket = "CRT_runtime"
        else:
            bucket = "named_other"
        buckets[bucket].append(f.size)
    out = [(b, len(sizes), sum(sizes)) for b, sizes in buckets.items()]
    out.sort(key=lambda r: -r[2])
    return out


def build_modules() -> tuple[list[ModuleStat], list[tuple[str, int, int]]]:
    lookup = _module_index()
    funcs = _load_funcs()
    vftbl_counts = _load_vftable_counts()

    per_ns: dict[str, NamespaceStat] = {}
    for f in funcs:
        ns = f.namespace if f.namespace else ""
        module = assign_module(ns, lookup)
        key = ns or "_Globals"
        stat = per_ns.get(key)
        if stat is None:
            stat = NamespaceStat(namespace=ns, module=module)
            per_ns[key] = stat
        stat.function_count += 1
        stat.total_bytes += f.size
        if f.address < stat.first_addr:
            stat.first_addr = f.address
        end = f.address + f.size
        if end > stat.last_addr:
            stat.last_addr = end
    for ns_name, count in vftbl_counts.items():
        stat = per_ns.get(ns_name)
        if stat is not None:
            stat.vftable_count = count

    # Build the per-module roll-up in MODULE_DEFS order, plus any modules
    # we discovered via fallback rules that weren't pre-declared.
    declared = list(MODULE_DEFS.keys())
    seen: set[str] = set(declared)
    extras = [m for st in per_ns.values()
              for m in [st.module] if m not in seen and not seen.add(m)]
    modules: list[ModuleStat] = []
    for module_id in declared + extras:
        desc = MODULE_DEFS.get(module_id, ("(no description)", []))[0]
        members = sorted(
            (s for s in per_ns.values() if s.module == module_id),
            key=lambda s: -s.total_bytes,
        )
        modules.append(ModuleStat(module=module_id, description=desc,
                                  namespaces=members))
    return modules, _residual_globals_breakdown(funcs)


# ----------------------------------------------------------------------
# Output writers
# ----------------------------------------------------------------------

def write_csv(modules: list[ModuleStat]) -> None:
    OUT_MODULES.parent.mkdir(parents=True, exist_ok=True)
    with OUT_MODULES.open("w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow([
            "module", "namespace", "function_count", "total_bytes",
            "vftable_count", "first_addr", "last_addr",
        ])
        for m in modules:
            for ns in m.namespaces:
                w.writerow([
                    m.module,
                    ns.namespace or "_Globals",
                    ns.function_count,
                    ns.total_bytes,
                    ns.vftable_count,
                    f"{ns.first_addr:08x}" if ns.first_addr != 0xFFFFFFFF else "",
                    f"{ns.last_addr:08x}" if ns.last_addr else "",
                ])


def _fmt_bytes(n: int) -> str:
    return f"{n:,}"


def write_doc(modules: list[ModuleStat],
              residual: list[tuple[str, int, int]]) -> None:
    OUT_DOC.parent.mkdir(parents=True, exist_ok=True)

    total_funcs = sum(m.function_count for m in modules)
    total_bytes = sum(m.total_bytes for m in modules)
    nontrivial_modules = [m for m in modules if m.function_count > 0]

    lines: list[str] = []
    lines.append("# Module map\n")
    lines.append(
        "Logical grouping of the namespaces Ghidra has recovered inside "
        "`bulanci.exe`. Generated by `scripts/internal/build_module_map.py` "
        "from the live Ghidra database via the MCP scripts in "
        "[`docs/DECOMP.md`](DECOMP.md). Re-run that script after every "
        "round of namespace promotion to keep this document current.\n"
    )
    lines.append("## Segments\n")
    lines.append("From `list_segments` (Ghidra MCP):\n")
    lines.append("```")
    lines.append("Headers : 00400000 - 00400fff")
    lines.append(".text   : 00401000 - 0047efff   (code)")
    lines.append(".rdata  : 0047f000 - 004adfff   (vftables, RTTI, string literals)")
    lines.append(".data   : 004ae000 - 004ba1fb   (globals, RTTI type descriptors)")
    lines.append(".rsrc   : 004bb000 - 004bcfff   (Win32 resources)")
    lines.append("```\n")
    lines.append("## Address-range layout of `.text`\n")
    lines.append(
        "Empirically the linker placed the original `.obj` blocks in this "
        "order (boundaries are approximate, taken from the histogram of "
        "function attribution in `ghidra_functions_dump.csv`):\n"
    )
    lines.append("| range | content | notes |")
    lines.append("|-------|---------|-------|")
    lines.append("| `0x401000 - 0x447000` | Game + engine code | Mix of attributed (CBulanci, CDS*, ...) and still-unattributed `FUN_xxx`. |")
    lines.append("| `0x447000 - 0x456000` | MSVCRT (CRT helpers) | `_free`, `_malloc`, `__SEH_prolog4`, `___tmainCRTStartup`, ... |")
    lines.append("| `0x456000 - 0x470000` | More CRT + ATL + std | Locale, `__crtMessageBoxA`, `_isdigit`, exception ctors. |")
    lines.append("| `0x470000 - 0x47f000` | MSVC EH unwind funclets | 1063 tiny `Unwind@xxxxx` thunks, ~27 KB, almost all global-scope. |\n")

    lines.append("## Modules\n")
    lines.append(
        f"`{total_funcs}` total functions, `{_fmt_bytes(total_bytes)}` "
        f"bytes of code, partitioned across `{len(nontrivial_modules)}` "
        f"modules (plus `Engine.DS.Interface`, which is pure-abstract "
        f"RTTI with no function bodies).\n"
    )
    lines.append("| module | namespaces | funcs | bytes | vftables | notes |")
    lines.append("|--------|-----------:|------:|------:|---------:|-------|")
    for m in modules:
        if m.function_count == 0 and len(m.namespaces) == 0:
            continue
        desc = m.description.replace("|", "\\|")
        lines.append(
            f"| `{m.module}` | {len(m.namespaces)} | {m.function_count} | "
            f"{_fmt_bytes(m.total_bytes)} | {m.vftable_count} | {desc} |"
        )
    lines.append("")

    for m in modules:
        if not m.namespaces:
            continue
        lines.append(f"### `{m.module}`\n")
        lines.append(f"_{m.description}_\n")
        lines.append("| namespace | funcs | bytes | vftables | first - last |")
        lines.append("|-----------|------:|------:|---------:|--------------|")
        for ns in m.namespaces:
            first = f"`{ns.first_addr:08x}`" if ns.first_addr != 0xFFFFFFFF else "-"
            last = f"`{ns.last_addr:08x}`" if ns.last_addr else "-"
            label = ns.namespace or "_Globals"
            lines.append(
                f"| `{label}` | {ns.function_count} | "
                f"{_fmt_bytes(ns.total_bytes)} | {ns.vftable_count} | "
                f"{first} - {last} |"
            )
        lines.append("")

    if residual:
        lines.append("## Residual `_Globals` breakdown\n")
        lines.append(
            "What `_Globals` actually contains today, broken down by "
            "Ghidra-assigned name pattern. The big targets for further "
            "promotion are `FUN_unattributed` (real game/engine code) "
            "and the `Catch@` / `Unwind@` machinery (MSVC exception glue "
            "that rides along with whichever function it unwinds for).\n"
        )
        lines.append("| bucket | funcs | bytes |")
        lines.append("|--------|------:|------:|")
        for bucket, count, total in residual:
            lines.append(f"| `{bucket}` | {count} | {_fmt_bytes(total)} |")
        lines.append("")

    lines.append("## Next steps to shrink `_Globals`\n")
    lines.append(
        "1. **Constant-propagated `this`.** Walk decompiler output for "
        "every `FUN_unattributed`; when the first argument is a "
        "`CClass*` whose type Ghidra has inferred, reparent into that "
        "class. Likely picks up several hundred funcs.\n"
        "2. **Allocation-site analysis.** Functions called immediately "
        "after `operator new` whose return slot is typed `CClass*` are "
        "almost always `CClass::Init`/`CClass::Setup`.\n"
        "3. **MSVC8 CRT fingerprint table.** Match the `CRT_runtime` "
        "bucket (~232 funcs, ~42 KB) against a hand-built name table of "
        "well-known VS2005 CRT entry points so they leave `_Globals` for "
        "an explicit `Runtime.MSVCRT` unit.\n"
        "4. **EH funclet attribution.** Every `Unwind@xxxx` belongs to "
        "the function whose `_FuncInfo` references it. Walk "
        "`/ghidra/CxxFrameHandler3` GS handler tables to rewrite these "
        "into `CClass::Unwind_xxxx`. Trivial code, removes 1023 rows.\n"
        "5. **Manual seeds.** A handful of leaf helpers will resist all "
        "of the above; rename them by hand in Ghidra and re-run "
        "`promote_namespaces.py` so `PropagateCallerNamespaces.java` "
        "cascades the new ownership.\n"
    )
    lines.append(
        "Re-run `scripts/internal/build_module_map.py` after each round "
        "of promotion to refresh this document.\n"
    )

    OUT_DOC.write_text("\n".join(lines), encoding="utf-8")


def main() -> int:
    if not DUMP_FUNCS.exists():
        print(f"missing {DUMP_FUNCS}")
        print("Run the Ghidra MCP probe first (see docs/MODULES.md header).")
        return 1
    modules, residual = build_modules()
    write_csv(modules)
    write_doc(modules, residual)
    print(f"wrote {OUT_MODULES.relative_to(REPO_ROOT)}")
    print(f"wrote {OUT_DOC.relative_to(REPO_ROOT)}")
    nontrivial = [m for m in modules if m.function_count > 0]
    print(f"modules: {len(nontrivial)}")
    for m in sorted(nontrivial, key=lambda m: -m.total_bytes):
        print(f"  {m.module:<24s} {len(m.namespaces):>3d} ns  "
              f"{m.function_count:>5d} fns  "
              f"{m.total_bytes:>7,d} B  "
              f"{m.vftable_count:>4d} vftables")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
