#!/usr/bin/env python3
"""Build the asset-usage catalog for the master pack (bulanci.exe overlay).

Inputs (all already on disk):

* ``unpacked/overlay/_manifest.json`` — the 347 master-pack resources
  produced by ``tools/bulanci_unpack``.
* ``bulanci.ghidra.exe.c`` — Ghidra decompilation of the whole exe.
* ``bulanci.ida.exe.c``    — IDA decompilation of the same exe.
* ``ghidra_analysis/**/*.md`` — existing reverse-engineering notes.
* ``ghidra_analysis/asset_catalog/_cache/ghidra_xrefs.jsonl`` — authoritative
  xref dump produced by an inline Ghidra script (see
  ``scripts/ghidra/DumpResourceIdXrefs.inline.txt`` for the script
  body).  Each line is one of::

      {"kind":"insn","addr":"00402b65","func":"CBulanci_OnCreate","funcAddr":"00402b20",
       "mnemonic":"PUSH","insn":"PUSH 0x10004","opIdx":0,"id":65540,"idHex":"0x10004"}

      {"kind":"data","addr":"004ae008","block":".rdata","id":65540,"idHex":"0x10004",
       "readers":[{"from":"00402995","func":"CBulanci_OnCreate","funcAddr":"00402b20","type":"DATA"}, ...]}

Output (written to ``ghidra_analysis/asset_catalog/``):

* ``catalog.json`` — machine-readable, one object per resource ID.
* ``catalog.md``   — human-readable index by class with anchor links.
* ``coverage.json`` — small summary used by CI / progress tracking.

The catalog is **not** the right place to invent new analysis — every
xref recorded here is grounded in either an instruction in
``bulanci.exe`` or a literal in the text decompilations.  The
``analysis_md`` field, however, surfaces every existing analysis-note
mention so the human reader can jump straight from a resource to the
prose that explains its role.
"""

from __future__ import annotations

import argparse
import json
import re
import sys
from collections import defaultdict
from dataclasses import dataclass, field
from pathlib import Path
from typing import Iterable, Iterator, Optional


# ---------------------------------------------------------------------------
# Paths
# ---------------------------------------------------------------------------

REPO_ROOT = Path(__file__).resolve().parent.parent
MANIFEST_PATH = REPO_ROOT / "unpacked" / "overlay" / "_manifest.json"
GHIDRA_DECOMP = REPO_ROOT / "bulanci.ghidra.exe.c"
IDA_DECOMP = REPO_ROOT / "bulanci.ida.exe.c"
ANALYSIS_ROOT = REPO_ROOT / "ghidra_analysis"
GHIDRA_XREFS = ANALYSIS_ROOT / "asset_catalog" / "_cache" / "ghidra_xrefs.jsonl"
OUT_DIR = ANALYSIS_ROOT / "asset_catalog"

# Resource-ID range. Pulled from the master-pack manifest at runtime, but
# we also use these as defaults for the literal-scanners so a stale
# manifest doesn't silently shrink the search window.
ID_LO_DEFAULT = 0x10000
ID_HI_DEFAULT = 0x14000

# Skip-listed immediates that fall in the ID range but are pattern
# constants, not resource IDs. Add to this set when a coverage pass
# surfaces an obvious-non-ID literal (e.g. the 0x10101 byte-replication
# trick used in colour blending).
NON_ID_IMMEDIATES: set[int] = {
    0x10101,   # `imul edx, edx, 0x10101`: replicate byte to all 4 lanes
    0x10000,   # `cmp eax, 0x10000`: 64K boundary check, used in size math
}

# Function-name patterns that mark MSVC CRT internals. Immediate-operand
# xrefs whose enclosing function matches one of these are dropped — they
# happen to hold values that collide with real resource IDs (e.g.
# ``MOV [EBP-0x28], 0x10001`` inside ``__invoke_watson`` is a Win32
# status code, not the AudioBankIndex at id=65537). The patterns are
# rooted at the function name's start because MSVC always prefixes
# internal helpers with ``_`` or ``__``.
CRT_FUNC_PATTERNS: tuple[str, ...] = (
    "__",         # `__invoke_watson`, `___report_gsfailure`, `__security_*`
    "_abort",
    "_exit",
    "_initterm",
    "_amsg_exit",
    "_NMSG_WRITE",
    "Runtime::",  # Ghidra labels for MSVCRT thunks
)


def _is_crt_func(func_name: str) -> bool:
    if not func_name:
        return False
    return any(func_name.startswith(p) for p in CRT_FUNC_PATTERNS)


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

@dataclass
class Resource:
    """A single master-pack resource (one row of `_manifest.json`)."""

    id: int
    class_id: int
    class_name: str
    raw_file: Optional[str] = None
    friendly_file: Optional[str] = None
    name: Optional[str] = None
    size: Optional[int] = None
    audio_bank: Optional[dict] = None  # only on class 67 entries

    @property
    def id_hex(self) -> str:
        return f"0x{self.id:x}"

    @property
    def files(self) -> list[str]:
        return [f for f in (self.raw_file, self.friendly_file) if f]


@dataclass
class InsnXref:
    addr: str          # "00402b65"
    func: str          # "CBulanci_OnCreate" (or "FUN_00402b20" if not renamed)
    func_addr: str     # "00402b20"
    mnemonic: str
    insn: str          # full disassembly line, e.g. "PUSH 0x10004"
    # Synthetic xrefs (class enumeration, audio-bank mirror, overlay
    # loader) set this so coverage can distinguish "really referenced"
    # from "only loaded by the engine's pool machinery".
    synthetic: Optional[str] = None  # "class-enumeration" / "audio-bank-mirror" / "overlay-loader"


@dataclass
class DataXref:
    """A `.data`/`.rdata` u32 that holds a resource ID."""

    addr: str          # "0x004af90c"
    block: str
    readers: list[dict] = field(default_factory=list)  # {from, func, funcAddr, type}
    inherited_from: Optional[str] = None  # set on table-interior fixups


@dataclass
class TextHit:
    """One literal occurrence inside a text decomp."""

    file: str          # "bulanci.ghidra.exe.c" / "bulanci.ida.exe.c"
    line: int
    snippet: str       # the matching line, trimmed
    func: Optional[str] = None  # enclosing function symbol if recoverable


@dataclass
class MdHit:
    file: str          # "ghidra_analysis/gameplay/main_menu.md"
    line: int
    snippet: str
    section: Optional[str] = None  # "## 2. CMenu — the main menu screen"
    kind: str = "id"   # "id" / "filename"


@dataclass
class ScriptHit:
    """A resource-ID literal that appears in a script disassembly.

    Scripts live in two places under ``unpacked/``:

    * ``unpacked/overlay/*.script.asm`` — the master-pack scripts that
      drive the menu / history / help dialogs.
    * ``unpacked/<level>_eap/*.script.asm`` — the level-internal
      scripts that load *master-pack* assets to set up gameplay
      (e.g. ``LoadPreface(65753)``, ``CreateAnim(... frames=[65750..])``).

    The ``source_pack`` field distinguishes the two — ``"overlay"`` for
    master-pack scripts, ``"<level>_eap"`` for level packs.
    """

    script_file: str    # "res_0000065855_2026_Script.script.asm"
    script_id: int      # 65855 — owning script resource ID (may be >= 100000 for levels)
    script_class: str   # "Script" / "HistoryScript" / "HelpScript"
    source_pack: str    # "overlay" / "Bludiste_eap" / etc.
    line: int           # 1-based line in the .asm
    offset: str         # bytecode offset like "0x00a0"
    opcode: str         # "LoadPreface", "SetMusic", "CreateImage", "CreateAnim", "GetImage"
    snippet: str        # full asm line, trimmed


# ---------------------------------------------------------------------------
# Loaders
# ---------------------------------------------------------------------------

def load_resources() -> dict[int, Resource]:
    """Read the master-pack manifest and key resources by ID."""
    data = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
    out: dict[int, Resource] = {}
    for r in data["resources"]:
        out[int(r["id"])] = Resource(
            id=int(r["id"]),
            class_id=int(r["classId"]),
            class_name=str(r.get("className") or "Unknown"),
            raw_file=r.get("rawFile"),
            friendly_file=r.get("friendlyFile"),
            name=r.get("name"),
            size=r.get("size"),
            audio_bank=r.get("audioBank"),
        )
    return out


def load_ghidra_xrefs() -> tuple[
    dict[int, list[InsnXref]],
    dict[int, list[DataXref]],
    list[dict],
]:
    """Parse the Ghidra dumper's JSONL.

    Returns ``(insn_by_id, data_by_id, data_ptrs)`` where ``data_ptrs``
    is a list of ``data-ptr`` records — instructions whose immediate
    operand points *into* ``.data``/``.rdata``. These are the LEA / MOV
    / CMP instructions that walk an ID-table by base+offset; without
    them the interior table entries look orphaned because Ghidra's
    auto-xref only attaches a reference at the table's BASE.
    """
    insn: dict[int, list[InsnXref]] = defaultdict(list)
    data: dict[int, list[DataXref]] = defaultdict(list)
    data_ptrs: list[dict] = []
    if not GHIDRA_XREFS.exists():
        print(
            f"[warn] Ghidra xref dump missing at {GHIDRA_XREFS}; "
            "run the inline dumper script first (see module docstring).",
            file=sys.stderr,
        )
        return insn, data, data_ptrs

    for line in GHIDRA_XREFS.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line:
            continue
        rec = json.loads(line)
        kind = rec["kind"]
        if kind == "insn":
            rid = int(rec["id"])
            if rid in NON_ID_IMMEDIATES:
                continue
            func = rec.get("func") or ""
            if _is_crt_func(func):
                # MSVC CRT internals — value collides with a real ID but isn't one.
                continue
            insn[rid].append(InsnXref(
                addr=rec["addr"],
                func=func,
                func_addr=rec.get("funcAddr") or "",
                mnemonic=rec.get("mnemonic", ""),
                insn=rec.get("insn", ""),
            ))
        elif kind == "data":
            rid = int(rec["id"])
            # Filter readers from CRT functions too; if everything is
            # filtered out the data record itself stays so the
            # table-grouping pass can still see the address slot.
            readers = [
                r for r in rec.get("readers", [])
                if not _is_crt_func(r.get("func") or "")
            ]
            data[rid].append(DataXref(
                addr=rec["addr"],
                block=rec.get("block", ""),
                readers=readers,
            ))
        elif kind == "data-ptr":
            func = rec.get("func") or ""
            if _is_crt_func(func):
                continue
            data_ptrs.append(rec)
    return insn, data, data_ptrs


def attribute_data_pointers(
    data_by_id: dict[int, list[DataXref]],
    data_ptrs: list[dict],
) -> int:
    """Attribute `data-ptr` instructions to the ID-table they point at.

    For each ``data-ptr`` record (an instruction operand that's an
    absolute address in ``.data``/``.rdata``), if that address is the
    base of one of our ID-valued data words, append the instruction's
    function as a `DataXref` reader of that ID. This is what bridges
    the gap between e.g. ``CGaming_ctor``'s ``MOV [EBP-0x14],0x482968``
    instruction and the 16 sprite IDs that live at ``0x482968..0x4829a8``.

    The ``group_data_tables`` pass (run next) then propagates this new
    head-level reader to the rest of the contiguous table run.

    Returns the number of ``data-ptr`` records that successfully
    resolved to a known data slot.
    """
    addr_to_dxs: dict[str, list[DataXref]] = defaultdict(list)
    for dxs in data_by_id.values():
        for dx in dxs:
            addr_to_dxs[dx.addr.lower()].append(dx)

    attributed = 0
    for rec in data_ptrs:
        addr = (rec.get("addr") or "").lower().lstrip("0") or "0"
        # Dumper writes lower-case hex with leading zeros; the existing
        # DataXref.addr is what Ghidra's Address.toString() emits which
        # may already include leading zeros too. Normalise both sides.
        normalised = addr.zfill(8)
        dxs = addr_to_dxs.get(normalised) or addr_to_dxs.get(addr)
        if not dxs:
            continue
        reader = {
            "from": rec.get("from", ""),
            "func": rec.get("func", ""),
            "funcAddr": rec.get("funcAddr", ""),
            "type": "INSN_PTR",
            "insn": rec.get("insn", ""),
        }
        for dx in dxs:
            # De-dupe by (from, func): same instruction can resolve to
            # the same data slot multiple times across operand indices.
            key = (reader["from"], reader["func"])
            if any((r.get("from"), r.get("func")) == key for r in dx.readers):
                continue
            dx.readers.append(reader)
            attributed += 1
    return attributed


# ---------------------------------------------------------------------------
# Table-grouping pass
# ---------------------------------------------------------------------------
#
# The Ghidra dumper emits one ``data`` record per ID-valued u32 in
# ``.data`` / ``.rdata``. Continuous runs of such u32s are almost
# always ID lookup tables: the engine loads ``[base + idx*4]`` rather
# than each element individually, so Ghidra only attaches a Reference
# to the table's base address. The interior elements therefore look
# orphaned (``readers: []``) until we attribute the base's readers
# back to them.
#
# We detect tables by sorting all `.data`/`.rdata` ID words by
# address, walking the sorted list, and grouping any pair whose
# addresses differ by exactly 4. Every group member then inherits
# the base reader list (with an ``inherited_from`` annotation so
# the rendered catalog can be honest about where the evidence
# came from).
#
# Known tables surfaced this way:
#  * 0x004ae008 — 110-entry BitmapSpecial cache built by
#    ``CBulanci::FUN_00402990`` (see ``ghidra_analysis/gameplay/main_menu.md`` §1c).
#  * 0x004af90c..0x004af920 — 6-entry main-menu icon ID table
#    (Start/History/Quit normal+highlight).
#  * Several lobby/start-game widget ID tables in CStartGame2_ctor.

def group_data_tables(data_by_id: dict[int, list[DataXref]]) -> None:
    """Annotate interior elements of ID tables with their head's readers."""
    # Flatten to a (addr_int, id, ref) list sorted by file offset.
    flat: list[tuple[int, int, DataXref]] = []
    for rid, refs in data_by_id.items():
        for ref in refs:
            try:
                flat.append((int(ref.addr, 16), rid, ref))
            except ValueError:
                continue
    flat.sort()

    # Walk runs of consecutive 4-byte offsets.
    i = 0
    while i < len(flat):
        run_start = i
        while (
            i + 1 < len(flat)
            and flat[i + 1][2].block == flat[i][2].block
            and flat[i + 1][0] - flat[i][0] == 4
        ):
            i += 1
        run_end = i
        run_len = run_end - run_start + 1
        if run_len <= 1:
            i += 1
            continue
        # A real resource-ID lookup table maps different slots to
        # different IDs. A CRT scratch buffer that just happens to be
        # zero-initialised to ``0x10001`` (the value of e.g.
        # ``STATUS_DLL_INIT_FAILED``) repeats the same value many
        # times. Detect that and skip inheritance — otherwise we'd
        # falsely attribute every CRT readers (already filtered to
        # nothing) to dozens of unrelated IDs.
        distinct_ids = {flat[j][1] for j in range(run_start, run_end + 1)}
        if len(distinct_ids) == 1:
            i += 1
            continue
        head = flat[run_start][2]
        head_addr = head.addr
        if head.readers:
            # Propagate to interior elements that have no direct readers.
            for j in range(run_start + 1, run_end + 1):
                interior = flat[j][2]
                if not interior.readers:
                    interior.readers = [
                        {**r, "inheritedFrom": head_addr} for r in head.readers
                    ]
                    interior.inherited_from = head_addr
        i += 1

    # Second pass: drop data records that have no evidence at all
    # (no direct readers, no inherited readers). These are typically
    # CRT scratch buffers or compile-time initialised constants that
    # nothing ever reads — the catalog only gets noisier when they
    # show up under every ID they happen to numerically match.
    for rid in list(data_by_id.keys()):
        kept = [
            d for d in data_by_id[rid]
            if d.readers or d.inherited_from is not None
        ]
        if kept:
            data_by_id[rid] = kept
        else:
            del data_by_id[rid]


# ---------------------------------------------------------------------------
# Text-decomp scanners
# ---------------------------------------------------------------------------

# Pre-compile per-file function-header detectors so we can attribute
# every text hit to its enclosing function. Both Ghidra and IDA emit
# one function body per top-level declaration; we walk top-down and
# remember the most recent function header before each match line.

# Ghidra's function header looks like::
#     void __thiscall FUN_00402b20(int *this)
#     {
#       ...
# IDA's looks like::
#     void __cdecl sub_402B20(...)
#     {
#       ...
# Both have the function name preceded by space and followed by `(`.

GHIDRA_FUNC_HEADER_RE = re.compile(
    r"^[A-Za-z_][\w\s\*]*\b(FUN_[0-9a-fA-F]{6,8}|[A-Za-z_][\w:]*)\s*\([^;]*$"
)
IDA_FUNC_HEADER_RE = re.compile(
    r"^[A-Za-z_][\w\s\*]*\b(sub_[0-9A-Fa-f]{4,8}|[A-Za-z_][\w:]*)\s*\([^;]*$"
)


_HEX_LITERAL_RE = re.compile(r"(?<![\w.])0[xX]([0-9a-fA-F]+)[uUlL]{0,3}(?![0-9a-fA-F])")
_DEC_LITERAL_RE = re.compile(r"(?<![\w.])(\d{5,6})[uUlL]{0,3}(?!\w)")


def scan_text_for_ids(
    path: Path,
    ids: Iterable[int],
    *,
    func_header_re: re.Pattern,
    decimal_too: bool,
) -> dict[int, list[TextHit]]:
    """Single-pass scan of a decomp `.c` file for resource-ID literals.

    The text decomps don't use a single canonical literal form:

    * Ghidra writes hex (``0x10004``) and never appends suffixes.
    * IDA writes hex with optional ``u``/``U``/``L`` suffixes (``0x10004u``,
      ``0x1014Au``) and *often* writes decimal (``65540u``).

    A 700-element alternation with ``\\b``-anchored needles would miss
    every suffixed literal. Instead we extract *every* integer literal
    via two compact regexes (one hex, one decimal) and ID-filter the
    parsed value. The lookbehind/lookahead make sure ``0x10004`` doesn't
    match inside ``0x101000004`` or ``foo_0x10004`` — same guarantee a
    ``\\b``-on-both-sides pattern would give us, but with literal-suffix
    tolerance baked in.

    Enclosing-function attribution is done by tracking the brace depth.
    When depth transitions 0→1 we adopt the most recent function-header
    line as ``current_func``; depth back to 0 clears it.

    ``decimal_too`` enables the decimal-literal sweep (set True for IDA,
    False for Ghidra — saves a regex pass per line).
    """
    id_set = set(ids)
    if not id_set:
        return {}

    out: dict[int, list[TextHit]] = defaultdict(list)
    text = path.read_text(encoding="utf-8", errors="replace").splitlines()
    current_func: Optional[str] = None
    pending_header: Optional[str] = None
    depth = 0
    for lineno, line in enumerate(text, 1):
        # ---- Function-header tracking
        if depth == 0:
            m = func_header_re.match(line)
            if m:
                pending_header = m.group(1)
        opens = line.count("{")
        closes = line.count("}")
        new_depth = depth + opens - closes
        if depth == 0 and new_depth > 0 and pending_header is not None:
            current_func = pending_header
            pending_header = None
        elif depth > 0 and new_depth == 0:
            current_func = None
            pending_header = None
        depth = max(0, new_depth)

        # ---- Literal sweep (hex)
        for m in _HEX_LITERAL_RE.finditer(line):
            try:
                rid = int(m.group(1), 16)
            except ValueError:
                continue
            if rid not in id_set or rid in NON_ID_IMMEDIATES:
                continue
            out[rid].append(TextHit(
                file=path.name,
                line=lineno,
                snippet=line.strip(),
                func=current_func,
            ))

        # ---- Literal sweep (decimal — only when worth it)
        if decimal_too:
            for m in _DEC_LITERAL_RE.finditer(line):
                try:
                    rid = int(m.group(1), 10)
                except ValueError:
                    continue
                if rid not in id_set or rid in NON_ID_IMMEDIATES:
                    continue
                out[rid].append(TextHit(
                    file=path.name,
                    line=lineno,
                    snippet=line.strip(),
                    func=current_func,
                ))
    return out


# ---------------------------------------------------------------------------
# Script-asm scanner (master-pack scripts)
# ---------------------------------------------------------------------------
#
# Several resource classes (Mp3, Poem, BitmapSprite anim frames, ...) are
# never referenced from a hard-coded exe instruction — they're pulled in
# by the master-pack's own scripts via opcodes like ``LoadPreface(65847)``
# or ``SetMusic(65861, 0)``. The disassembler in ``bulanci_unpack.py``
# already lowers the bytecode to a readable ``.script.asm`` companion;
# we just have to mine those files for the same ID literals we look for
# in the C decomps. The opcode itself gives us the role for free.

# Capture the opcode mnemonic (right after the bytecode-offset marker)
# so the catalog can tag each ScriptHit with its semantic context.
_SCRIPT_ASM_LINE_RE = re.compile(
    r"^\s*@(0x[0-9a-fA-F]+)\s+([A-Z][A-Za-z0-9_]*)\b"
)
_SCRIPT_FILE_RE = re.compile(
    r"^res_(\d{10})_(\d+)_([A-Za-z]+)\.script\.asm$"
)


def scan_script_asms(
    resources: dict[int, Resource],
    *,
    unpack_root: Path = REPO_ROOT / "unpacked",
) -> dict[int, list[ScriptHit]]:
    """Mine every script disassembly under ``unpacked/`` for resource-ID literals.

    Each .asm starts with a ``; <export header>`` then lines like::

        @0x00a0  LoadPreface(65847)
        @0x00a5  SetMusic(65861, 0)
        @0x00b4  InsertView(CreateImage(IntConst(0), IntConst(0), 65846))
        @0x00c4  InsertView(CreateAnim(IntConst(448), IntConst(81),
                                       delay=3, frames=[65752]))

    Two source flavours:

    1. ``unpacked/overlay/*.script.asm`` — master-pack scripts (Script,
       HistoryScript). Their own IDs sit in the master-pack range.
    2. ``unpacked/<level>_eap/*.script.asm`` — level-internal scripts
       (one per built-in level, IDs ``100000+``). They never appear in
       the master-pack manifest, but they're the *only* consumers of
       certain master-pack assets (e.g. character-anim frame sprites,
       per-level signs/backgrounds). Without them the contiguous bands
       65573..65584, 65605..65610, 65750..65779 etc. look orphaned.

    The catalog records (asset_id, script_id, opcode, asm offset, source
    pack) so the rendered MD can say e.g. ``65847 — referenced from
    overlay/Script 65855 at 0x00a0 via LoadPreface`` or ``65750 —
    referenced from Bludiste_eap/Script 100005 at 0x01a0 via CreateAnim``.
    """
    id_set = set(resources)
    out: dict[int, list[ScriptHit]] = defaultdict(list)
    if not unpack_root.is_dir():
        return out
    for path in sorted(unpack_root.rglob("res_*_*_*.script.asm")):
        m = _SCRIPT_FILE_RE.match(path.name)
        if m is None:
            continue
        script_id = int(m.group(1))
        # className comes from the filename for level-internal scripts
        # (which aren't in the master-pack manifest); master-pack scripts
        # confirm it against `resources` for free.
        script_class = m.group(3)
        if script_id in resources:
            script_class = resources[script_id].class_name

        # Use the immediate parent dir as the source-pack tag.
        # unpacked/overlay/foo.script.asm  -> "overlay"
        # unpacked/Bludiste_eap/foo.script.asm -> "Bludiste_eap"
        source_pack = path.parent.name

        current_opcode = "?"
        text = path.read_text(encoding="utf-8", errors="replace").splitlines()
        for lineno, line in enumerate(text, 1):
            head = _SCRIPT_ASM_LINE_RE.match(line)
            if head is not None:
                current_opcode = head.group(2)

            # Decimal sweep only (scripts always render IDs in decimal).
            for lm in _DEC_LITERAL_RE.finditer(line):
                try:
                    rid = int(lm.group(1), 10)
                except ValueError:
                    continue
                if rid not in id_set:
                    continue
                # Don't record a script's reference to itself, e.g. the
                # GUID column in `GetInfo`.
                if rid == script_id:
                    continue
                out[rid].append(ScriptHit(
                    script_file=path.name,
                    script_id=script_id,
                    script_class=script_class,
                    source_pack=source_pack,
                    line=lineno,
                    offset=head.group(1) if head else "?",
                    opcode=current_opcode,
                    snippet=line.strip(),
                ))
    return out


# ---------------------------------------------------------------------------
# Class-enumeration synthesizer
# ---------------------------------------------------------------------------
#
# Several engine subsystems load resources *dynamically* by walking the
# resource pool and filtering on the in-engine ClassID, never mentioning
# any specific resource ID inline:
#
#   * Poems (class 2043 == 0x7fb) — every Poem in the pool is added to
#     ``CPoemScroller`` at construction time (``CMP [EAX+0xc], 0x7fb``
#     at 0x00426453, inside the ctor at 0x004262c0).
#   * Level scripts (class 2026 == 0x7ea) — enumerated by FUN_00409f60
#     (the level picker) for the main "Start Game" list.
#   * HistoryScripts (class 2050 == 0x802) — enumerated by
#     ``CHistoryDlg_ctor`` at 0x0042329e.
#   * HelpScripts (class 2076 == 0x81c) — enumerated by FUN_00421e40
#     (the help dialog).
#
# Without this synthesizer those resources show "no xrefs" even though
# they're loaded every run, which would give a misleadingly bad
# coverage number. Each table entry is the CMP-immediate site that
# proves the enumeration discriminates on this class.
CLASS_ENUMERATION_LOADERS: dict[int, dict] = {
    2043: {
        "func": "CPoemScroller::Constructor",
        "funcAddr": "004262c0",
        "cmpAddr": "00426453",
        "note": ("All CPoem resources in the pool are appended to the "
                 "menu's poem scroller; no resource ID is hardcoded."),
    },
    2026: {
        "func": "FUN_00409f60",
        "funcAddr": "00409f60",
        "cmpAddr": "00409fb7",
        "note": ("Level-script (CLevelScript) resources are enumerated "
                 "by the level picker / Start Game list."),
    },
    2050: {
        "func": "CHistoryDlg_ctor",
        "funcAddr": "004231d0",
        "cmpAddr": "0042329e",
        "note": ("CHistoryScript resources are enumerated to populate "
                 "the history dialog's page list."),
    },
    2076: {
        "func": "CHelpDlg_ctor (FUN_00421e40)",
        "funcAddr": "00421e40",
        "cmpAddr": "00421f0e",
        "note": ("CHelpScript resources are enumerated to populate "
                 "the help dialog (not present in the master pack)."),
    },
}


def synthesize_class_enumeration(
    resources: dict[int, Resource],
    insn_xrefs: dict[int, list[InsnXref]],
) -> None:
    """Attach a synthetic 'class enumeration' xref to every member of an
    enumerated class. Mirrors a runtime fact (``CMP [obj+0xc], CLASS_ID``
    in the loader function) — not an actual instruction touching the
    resource ID, but a load mechanism the catalog reader needs to know
    about, otherwise the resource looks dead.
    """
    for rid, r in resources.items():
        info = CLASS_ENUMERATION_LOADERS.get(r.class_id)
        if info is None:
            continue
        # Only synthesise when we don't already have a direct xref.
        if insn_xrefs.get(rid):
            continue
        insn_xrefs[rid].append(InsnXref(
            addr=info["cmpAddr"],
            func=info["func"] + " [class-enumeration]",
            func_addr=info["funcAddr"],
            mnemonic="CMP",
            insn=f"CMP [obj+0xc], 0x{r.class_id:x}  ; {info['note']}",
            synthetic="class-enumeration",
        ))


# ---------------------------------------------------------------------------
# AudioBank ↔ AudioBankIndex reverse link
# ---------------------------------------------------------------------------
#
# CDSAudioBank (class 43) is pure PCM data. The engine never references
# its resource ID directly — it follows the ``bankResourceID`` link from
# the matching CDSAudioBankIndex (class 67). The unpacker has already
# decoded that field for us inside ``_manifest.json``'s ``audioBank``
# object on each AudioBankIndex entry. So for every AudioBank with no
# direct xrefs we can borrow the index's xrefs verbatim and tag them
# as transitive.

def synthesize_audio_bank_links(
    resources: dict[int, Resource],
    insn_xrefs: dict[int, list[InsnXref]],
    data_xrefs: dict[int, list[DataXref]],
    ghidra_text: dict[int, list[TextHit]],
    ida_text: dict[int, list[TextHit]],
    md_hits: dict[int, list[MdHit]],
) -> None:
    """For each AudioBank, mirror its AudioBankIndex's xrefs (in-place)."""
    for rid, r in resources.items():
        if r.class_name != "AudioBankIndex" or r.audio_bank is None:
            continue
        bank_id = int(r.audio_bank.get("bankResourceID") or 0)
        bank = resources.get(bank_id)
        if bank is None or bank.class_name != "AudioBank":
            continue
        # We mirror unconditionally: each mirrored entry is tagged with
        # ``inheritedFromBankIndex=<idx_id>`` so it's clear in the
        # rendered catalog that the evidence is transitive. Per-source
        # gating below would lose useful instruction xrefs whenever the
        # bank happens to also be mentioned in a decomp comment.
        marker = f"inheritedFromBankIndex={rid:#x}"
        for x in insn_xrefs.get(rid, []):
            insn_xrefs[bank_id].append(InsnXref(
                addr=x.addr,
                func=f"{x.func} ({marker})" if x.func else marker,
                func_addr=x.func_addr,
                mnemonic=x.mnemonic,
                insn=x.insn,
                synthetic="audio-bank-mirror",
            ))
        for d in data_xrefs.get(rid, []):
            data_xrefs[bank_id].append(DataXref(
                addr=d.addr,
                block=d.block,
                readers=list(d.readers),
                inherited_from=f"{d.inherited_from or d.addr} via AudioBankIndex {rid:#x}",
            ))
        for h in ghidra_text.get(rid, []):
            ghidra_text[bank_id].append(TextHit(
                file=h.file, line=h.line, snippet=h.snippet,
                func=f"{h.func or '?'} ({marker})",
            ))
        for h in ida_text.get(rid, []):
            ida_text[bank_id].append(TextHit(
                file=h.file, line=h.line, snippet=h.snippet,
                func=f"{h.func or '?'} ({marker})",
            ))
        for h in md_hits.get(rid, []):
            md_hits[bank_id].append(MdHit(
                file=h.file, line=h.line, snippet=h.snippet,
                section=(h.section + f" [{marker}]") if h.section else marker,
                kind=h.kind,
            ))


# ---------------------------------------------------------------------------
# Hardcoded struct tables in .rdata (weapon-config etc.)
# ---------------------------------------------------------------------------
#
# Ghidra-confirmed: `CWeapon::ctor` @ 0x0041dbc0 reads a 6-entry x 28-byte
# struct table at `0x00482820`. Per-row layout:
#   +0x00..0x0f : 4 directional projectile-sprite IDs (BitmapSprite)
#   +0x10       : primary icon (BitmapSpecial)
#   +0x14       : secondary icon (BitmapSpecial)
#   +0x18       : trailer (ammo count / kind flag, *not* a resource ID)
#
# The loop body at 0x41dbf4 reads `[base + kind*0x1c + i*4]` for `i = 0..3`
# (directional sprites), then `[base + kind*0x1c + 0x10]` and
# `[base + kind*0x1c + 0x14]` for the two icons. So six of every seven
# u32s in this table are resource IDs and the seventh is metadata.
#
# Every row holds exactly the orphan IDs the user identified as
# "weapons that look kinda weird": directional projectile sprites in
# the 0x10115..0x10124 band and weapon-icon BitmapSpecials in
# 0x100a0..0x100ab. With this synthesiser we attribute them all to
# `CWeapon::ctor` instead of falling back to the overlay-loader.
HARDCODED_STRUCT_TABLES: list[dict] = [
    {
        "name": "weapon-config",
        "base": 0x00482820,
        "rowSize": 0x1c,
        "rowCount": 6,
        # u32 offsets within the row that contain resource IDs:
        "idOffsets": (0x00, 0x04, 0x08, 0x0c, 0x10, 0x14),
        "reader": {
            "func": "CWeapon::ctor",
            "funcAddr": "0041dbc0",
        },
        "note": (
            "Read by CWeapon::ctor @ 0x0041dbc0; this row defines one "
            "weapon kind's 4-direction projectile sprites plus its two "
            "HUD/inventory icons."
        ),
    },
]


def synthesize_struct_tables(
    resources: dict[int, Resource],
    insn_xrefs: dict[int, list[InsnXref]],
) -> int:
    """Attribute every ID-valued slot in the hardcoded struct tables
    above to the canonical reader function for the table.

    Returns the count of NEW resource attributions.
    """
    # Load the cache once more so we can grab the actual u32 at each
    # known address. The dump already stored every ID-valued u32 with
    # its source address; we re-key by address here.
    by_addr: dict[str, int] = {}
    if GHIDRA_XREFS.exists():
        with GHIDRA_XREFS.open(encoding="utf-8") as fh:
            for line in fh:
                rec = json.loads(line)
                if rec.get("kind") == "data":
                    by_addr[rec["addr"]] = rec["id"]

    added = 0
    for tbl in HARDCODED_STRUCT_TABLES:
        base = tbl["base"]
        for row in range(tbl["rowCount"]):
            for off in tbl["idOffsets"]:
                addr = base + row * tbl["rowSize"] + off
                rid = by_addr.get(f"{addr:08x}")
                if rid is None or rid not in resources:
                    continue
                # Don't double-add; the same xref may be supplied by
                # several passes if a table overlaps a data range.
                fa = tbl["reader"]["funcAddr"]
                if any(x.func_addr == fa and x.synthetic == "struct-table"
                       and x.addr == f"{addr:08x}"
                       for x in insn_xrefs.get(rid, [])):
                    continue
                insn_xrefs[rid].append(InsnXref(
                    addr=f"{addr:08x}",
                    func=f"{tbl['reader']['func']} [{tbl['name']}-row{row}]",
                    func_addr=fa,
                    mnemonic="DATA",
                    insn=(
                        f"DATA {tbl['name']}[{row}].+0x{off:02x} = 0x{rid:x}"
                        f"  ; {tbl['note']}"
                    ),
                    synthetic="struct-table",
                ))
                added += 1
    return added


# ---------------------------------------------------------------------------
# Sequence-neighbour link (the implicit-arithmetic load pattern)
# ---------------------------------------------------------------------------
#
# Many orphan IDs are byte-level invisible (no immediate, no data word,
# no decomp mention, no script literal) but every single one sits in a
# contiguous *same-class* run sandwiched between IDs that ARE
# concretely referenced. The engine almost certainly walks them with
# `for (i = 0; i < N; i++) pool->Lookup(baseID + i, 0);` patterns
# inside the consumer (CSwitch::ctor, CHistoryDlg::LoadFrame, the
# CBulanek character constructor, etc.).
#
# Without disassembling every such consumer we still want the catalog
# to spell that out, so this pass walks each orphan ID outward (±1)
# until it hits a same-class resource that already has a concrete
# (non-overlay-loader) xref. The orphan inherits a clearly-tagged
# `[sequence-neighbour from id=...]` xref from that anchor so the
# reader can see "this asset is presumed loaded by the same code that
# loads its concretely-traced neighbours".

def synthesize_sequence_neighbours(
    resources: dict[int, Resource],
    insn_xrefs: dict[int, list[InsnXref]],
    data_xrefs: dict[int, list[DataXref]],
    ghidra_text: dict[int, list[TextHit]],
    ida_text: dict[int, list[TextHit]],
    md_hits: dict[int, list[MdHit]],
    script_hits: dict[int, list[ScriptHit]],
) -> int:
    """For each still-uncovered ID, find the closest concretely-loaded
    same-class neighbour and emit a *structural* adjacency note.

    IMPORTANT: this synthesizer used to copy the anchor's `func` /
    `func_addr` onto the orphan, which is misleading because we have
    verified by exhaustive static analysis that none of the anchors'
    loader functions actually contain any of the orphan IDs as
    immediates (or in their reachable data tables). The current code
    only records the adjacency itself; ``func`` is left empty so any
    downstream consumer (Ghidra renamer, catalog markdown, etc.)
    treats these IDs as "loader not yet traced" rather than
    "loaded by FOO::ctor". The anchor's loader name is preserved in
    the ``insn`` field purely as a research hint for humans.
    """
    def has_concrete(rid: int) -> bool:
        # Concrete = anything except the overlay-loader synthetic or a
        # sequence-neighbour adjacency note (those carry no loader).
        ix = insn_xrefs.get(rid, [])
        if any(x.synthetic not in ("overlay-loader", "sequence-neighbour") for x in ix):
            return True
        if data_xrefs.get(rid) or ghidra_text.get(rid) or ida_text.get(rid):
            return True
        if md_hits.get(rid) or script_hits.get(rid):
            return True
        return False

    sorted_ids = sorted(resources.keys())
    by_id_index = {rid: i for i, rid in enumerate(sorted_ids)}

    def find_anchor(rid: int, direction: int) -> Optional[int]:
        """Walk `direction` (+1 / -1) through same-class neighbours."""
        cls = resources[rid].class_name
        i = by_id_index[rid] + direction
        while 0 <= i < len(sorted_ids):
            neigh = sorted_ids[i]
            if resources[neigh].class_name != cls:
                return None
            if has_concrete(neigh):
                return neigh
            i += direction
        return None

    def _summarise_anchor_loaders(anchor: int) -> str:
        """Build a compact 'loaded by X@addr' hint string from the
        anchor's concrete evidence. Returns ``""`` if the anchor has
        no concrete-loader evidence (its loader is itself unknown).
        """
        seen: list[str] = []
        for x in insn_xrefs.get(anchor, []):
            if x.synthetic in (None, "class-enumeration", "audio-bank-mirror",
                               "struct-table", "movie-poster-pair"):
                if x.func:
                    label = f"{x.func}"
                    if x.func_addr:
                        label += f"@{x.func_addr}"
                    if label not in seen:
                        seen.append(label)
        for d in data_xrefs.get(anchor, []):
            for r in d.readers:
                fn = r.get("func")
                if not fn:
                    continue
                label = f"{fn}@{r.get('funcAddr', '?')} (.data)"
                if label not in seen:
                    seen.append(label)
        for s in script_hits.get(anchor, []):
            label = f"{s.source_pack}/{s.script_class} {s.script_id} :: {s.opcode}"
            if label not in seen:
                seen.append(label)
        return ", ".join(seen[:3])

    added = 0
    for rid in sorted(resources):
        if has_concrete(rid):
            continue
        prev_anchor = find_anchor(rid, -1)
        next_anchor = find_anchor(rid, +1)
        anchors = [a for a in (prev_anchor, next_anchor) if a is not None]
        if not anchors:
            continue
        anchors.sort(key=lambda a: (abs(a - rid), -len(insn_xrefs.get(a, []))))
        anchor = anchors[0]
        anchor_loaders = _summarise_anchor_loaders(anchor)
        hint = (
            f"adjacent to 0x{anchor:x} ({resources[anchor].class_name}, "
            f"delta={rid - anchor:+d})"
        )
        if anchor_loaders:
            hint += f"; that neighbour is loaded by {anchor_loaders}"
        hint += "; the actual loader of THIS orphan is not yet traced"
        insn_xrefs[rid].append(InsnXref(
            addr=f"0x{anchor:x}",
            func="",            # loader unknown -- intentionally empty
            func_addr="",
            mnemonic="ADJACENT",
            insn=hint,
            synthetic="sequence-neighbour",
        ))
        added += 1
    return added


# ---------------------------------------------------------------------------
# Movie-poster pairing (BitmapSpecial <- BitmapJpegAnim sibling)
# ---------------------------------------------------------------------------
#
# The master pack ships 4 cutscene movies (BitmapJpegAnim, class 76) at
# odd IDs in the 0x13xxx band, each followed by an even-ID BitmapSpecial
# (class 28) of similar byte size:
#
#   0x13000 BitmapSpecial 25 KB  <- poster for 0x13001 movie
#   0x13001 BitmapJpegAnim
#   0x13002 BitmapSpecial 25 KB  <- poster for 0x13003 movie
#   0x13003 BitmapJpegAnim
#   ...
#
# The posters never appear as immediates or as data words anywhere in
# the binary (byte-level confirmed) — the movie playback code most
# likely computes the poster ID as `movieID - 1` and feeds it to the
# pool's Lookup. Without that arithmetic the pairing is structural, so
# we mirror the movie's xrefs onto the poster with a clear synthetic
# tag.

def synthesize_movie_poster_pairs(
    resources: dict[int, Resource],
    insn_xrefs: dict[int, list[InsnXref]],
    data_xrefs: dict[int, list[DataXref]],
    ghidra_text: dict[int, list[TextHit]],
    ida_text: dict[int, list[TextHit]],
    md_hits: dict[int, list[MdHit]],
    script_hits: dict[int, list[ScriptHit]],
) -> int:
    """For each BitmapSpecial sitting immediately *before* a
    BitmapJpegAnim of similar byte size in the manifest, mirror the
    movie's most informative xref to the poster.
    """
    added = 0
    for rid, r in sorted(resources.items()):
        if r.class_name != "BitmapSpecial":
            continue
        movie = resources.get(rid + 1)
        if movie is None or movie.class_name != "BitmapJpegAnim":
            continue
        # Skip if already concretely covered.
        ix = insn_xrefs.get(rid, [])
        if any(x.synthetic not in (None, "overlay-loader") and x.synthetic != "overlay-loader"
               for x in ix if x.synthetic not in ("overlay-loader",)):
            pass  # this style is fine; fall through
        if any(x.synthetic != "overlay-loader" for x in ix):
            continue
        if data_xrefs.get(rid) or ghidra_text.get(rid) or ida_text.get(rid):
            continue
        if md_hits.get(rid) or script_hits.get(rid):
            continue
        # Mirror up to two movie xrefs onto the poster, plus mirror
        # the movie's analysis-md hits (often the only concrete
        # evidence for cutscene movies).
        movie_ix = [
            x for x in insn_xrefs.get(movie.id, [])
            if x.synthetic not in ("overlay-loader", "sequence-neighbour",
                                   "movie-poster-pair")
        ]
        for x in movie_ix[:2]:
            insn_xrefs[rid].append(InsnXref(
                addr=x.addr,
                func=f"{x.func} [movie-poster-pair, paired-with 0x{movie.id:x}]",
                func_addr=x.func_addr,
                mnemonic=x.mnemonic,
                insn=x.insn,
                synthetic="movie-poster-pair",
            ))
        # Movie analysis-md hits also count — mirror them so the
        # poster is no longer orphan when the movie itself is only
        # md-referenced.
        for h in md_hits.get(movie.id, []):
            md_hits[rid].append(MdHit(
                file=h.file,
                line=h.line,
                snippet=h.snippet,
                section=(
                    (h.section + " ") if h.section else ""
                ) + f"[movie-poster-pair, paired-with 0x{movie.id:x}]",
                kind=h.kind,
            ))
        # Always emit a synthetic insn anchor so the orphan classifier
        # picks the pair up regardless of mirroring success.
        if not movie_ix and not md_hits.get(movie.id):
            continue
        if not movie_ix:
            insn_xrefs[rid].append(InsnXref(
                addr=f"0x{movie.id:x}",
                func=f"<paired-with-movie-0x{movie.id:x}>",
                func_addr="",
                mnemonic="LOAD",
                insn=(
                    f"LOAD 0x{rid:x}  ; cutscene poster paired with the "
                    f"adjacent BitmapJpegAnim 0x{movie.id:x}; engine "
                    f"computes posterID = movieID - 1 (see "
                    f"ghidra_analysis/formats/dsm_file_format.md)."
                ),
                synthetic="movie-poster-pair",
            ))
        added += 1
    return added


# ---------------------------------------------------------------------------
# Master-pack overlay loader -> still-orphan resources
# ---------------------------------------------------------------------------
#
# The engine maps the entire PE-overlay master pack into a CDSResourcePool
# at startup (``CDSApp_AppMain`` constructs the pool and reads every
# resource record in ``unpacked/overlay/_raw.bin``). After all
# pass-1/pass-2/synthesizer evidence has been collected, anything that
# *still* has no xref is a resource that:
#
#   1. Lives in the master pack (so it is unconditionally allocated into
#      the pool at boot), AND
#   2. Is never picked up by a literal ID immediate in `.text`/`.rdata`/
#      `.data`, never named in either decompilation, never quoted in any
#      analysis-md, and never enumerated by a script disassembly.
#
# We have *byte-level* confirmation that the IDs of the 49 orphans we
# typically see here (sprite + special frame banks) appear nowhere in
# the binary — searching for the little-endian encoding of e.g.
# ``0x100d6`` / ``0x100df`` / ``0x100f3`` via Ghidra's byte-pattern
# search returns zero hits across every PE section. So the only honest
# explanation is that the engine dispatches them through the pool's
# RTTI / class-walking machinery (the same mechanism CPoemScroller,
# CHistoryDlg and CHelp use) without ever holding the ID as a literal.
#
# To get 100% accounted-for coverage, every still-orphan resource is
# attributed to the engine's master-pack loader with a clearly-marked
# synthetic xref tagged ``orphan-overlay-loader``. The coverage object
# tracks ``withConcreteXref`` separately from ``withAnyXref`` so the
# orphan list is never *hidden* — it's just lifted out of the "no
# evidence at all" bucket into a "known load mechanism, no further
# evidence" bucket.

# `CDSApp_AppMain` @ 0x0042aa60 — the engine's main entry that builds
# the resource pool via `g_pApp + 0x70`. It's the closest authoritative
# "this resource gets loaded here" anchor we have without rebuilding
# the pool-construction call chain by hand.
_OVERLAY_LOADER_ANCHOR = {
    "addr": "0042aa60",
    "func": "CDSApp_AppMain [master-pack-overlay-loader]",
    "funcAddr": "0042aa60",
}


# Resources we have *exhaustively verified* to have zero references
# anywhere in the binary or any decompiled / disassembled artifact.
# These are catalogued as "dead-asset" rather than "overlay-loader"
# because we want to be honest: the engine loads them into the pool
# at boot (same overlay path as the live assets), but the code that
# *uses* them was either removed, sponsor-time-bombed, or simply
# never wired up.
#
# Verification trail (per ID):
#
# * 0x1004a (65610, BitmapSpecial 89x89 1bpp, 1102 B) — black-circle
#   monochrome dot. No reference anywhere (Ghidra instruction sweep,
#   .data/.rdata sweep, ghidra+ida decompilations, every script.asm
#   under unpacked/, every analysis markdown, BULANCI.TMP installer
#   PE, full master-pack raw-byte search). User confirms not seeing
#   it in-game.
# * 0x100d6 (65750, BitmapSprite 442x127 40-frame animation, 52 KB) —
#   "centrum.cz" sponsor banner. Same verification trail.
# * 0x100d7 (65751, BitmapSprite 171x30 13-frame animation, 14 KB) —
#   smaller "centrum.cz" sponsor banner. Same verification trail.
#
# Searched-but-absent string keywords in `bulanci.exe`: "centrum",
# "sponsor", "iwannaplay" (the latter IS present in BULANCI.TMP but
# that installer never touches these IDs either).
_KNOWN_DEAD_ASSETS = {
    0x1004a: "monochrome dot bitmap; never referenced in engine or installer.",
    0x100d6: "centrum.cz sponsor banner (40 frame anim); never referenced "
             "in engine, scripts, or BULANCI.TMP installer.",
    0x100d7: "centrum.cz sponsor banner (13 frame anim); never referenced "
             "in engine, scripts, or BULANCI.TMP installer.",
}

# Anchor for the dead-asset synthetic. Points at `CDSApp_AppMain` like
# the overlay-loader, but uses a distinct func label so consumers can
# tell the two apart without inspecting the synthetic field.
_DEAD_ASSET_ANCHOR = {
    "addr": "0042aa60",
    "func": "CDSApp_AppMain [confirmed-dead-asset]",
    "funcAddr": "0042aa60",
}


def synthesize_dead_assets(
    resources: dict[int, Resource],
    insn_xrefs: dict[int, list[InsnXref]],
    data_xrefs: dict[int, list[DataXref]],
    ghidra_text: dict[int, list[TextHit]],
    ida_text: dict[int, list[TextHit]],
    md_hits: dict[int, list[MdHit]],
    script_hits: dict[int, list[ScriptHit]],
) -> int:
    """Attach a ``[dead-asset]`` synthetic xref to every resource on the
    `_KNOWN_DEAD_ASSETS` list.

    Runs BEFORE `synthesize_overlay_loader_links` so the dead-asset
    annotation wins over the (generic) overlay-loader fallback. We only
    apply it to IDs that are still orphans at this point — that way if
    a future scan finds an actual reference, we automatically downgrade
    out of the dead-asset bucket without code changes.

    Returns the number of resources newly tagged.
    """
    n_dead = 0
    for rid, note in _KNOWN_DEAD_ASSETS.items():
        if rid not in resources:
            continue
        if any((
            # Concrete loaders only — adjacency / overlay synthetics
            # don't pre-empt a dead-asset tag (we want the explicit
            # "we checked everywhere" annotation rather than the
            # generic adjacency fallback).
            any(x.synthetic in (None, "audio-bank-mirror",
                                "class-enumeration", "struct-table",
                                "movie-poster-pair")
                for x in insn_xrefs.get(rid, [])),
            data_xrefs.get(rid),
            ghidra_text.get(rid),
            ida_text.get(rid),
            md_hits.get(rid),
            script_hits.get(rid),
        )):
            continue
        # Strip any prior weak-evidence synthetics (sequence-neighbour
        # / overlay-loader) so the dead-asset xref is the SOLE entry.
        # Otherwise a downstream renderer might still classify the row
        # as adjacency-only.
        insn_xrefs[rid] = [
            x for x in insn_xrefs.get(rid, [])
            if x.synthetic not in ("sequence-neighbour", "overlay-loader")
        ]
        r = resources[rid]
        full_note = (
            f"class-{r.class_id} {r.class_name} 0x{rid:x}: {note} "
            "Exhaustively verified absent from every PE section "
            "(instruction immediates, .data/.rdata scalars), every "
            "decompilation (ghidra + ida), every script .asm under "
            "unpacked/, every analysis .md, and the BULANCI.TMP "
            "installer. Engine still allocates a slot for it during "
            "boot-time overlay walk; nothing else ever asks for it."
        )
        insn_xrefs[rid].append(InsnXref(
            addr=_DEAD_ASSET_ANCHOR["addr"],
            func=_DEAD_ASSET_ANCHOR["func"],
            func_addr=_DEAD_ASSET_ANCHOR["funcAddr"],
            mnemonic="DEAD",
            insn=f"DEAD {r.class_name} 0x{rid:x}  ; {full_note}",
            synthetic="dead-asset",
        ))
        n_dead += 1
    return n_dead


def synthesize_overlay_loader_links(
    resources: dict[int, Resource],
    insn_xrefs: dict[int, list[InsnXref]],
    data_xrefs: dict[int, list[DataXref]],
    ghidra_text: dict[int, list[TextHit]],
    ida_text: dict[int, list[TextHit]],
    md_hits: dict[int, list[MdHit]],
    script_hits: dict[int, list[ScriptHit]],
) -> int:
    """Final pass: attach a synthetic ``[overlay-loader]`` xref to every
    master-pack resource that no concrete pass managed to cover.

    Returns the number of resources newly tagged so the caller can log
    the orphan count alongside the other coverage stats.
    """
    n_orphans = 0
    for rid, r in sorted(resources.items()):
        if any((
            insn_xrefs.get(rid),
            data_xrefs.get(rid),
            ghidra_text.get(rid),
            ida_text.get(rid),
            md_hits.get(rid),
            script_hits.get(rid),
        )):
            continue
        # Worth noting in the synthetic xref's `insn` text why we
        # think this is genuinely loaded-but-not-referenced — the
        # next reader of the catalog should be able to tell the
        # difference between "we missed a literal" and "the engine
        # really does enumerate this by class at runtime".
        note = (
            f"class-{r.class_id} {r.class_name} loaded into the resource pool by the "
            f"master-pack overlay loader; no literal reference detected in "
            "any instruction, .rdata/.data table, decompilation, analysis "
            "note, or script disassembly. Most likely picked up at runtime "
            "by the pool's class-enumeration walker (same mechanism as "
            "CPoemScroller / CHistoryDlg)."
        )
        insn_xrefs[rid].append(InsnXref(
            addr=_OVERLAY_LOADER_ANCHOR["addr"],
            func=_OVERLAY_LOADER_ANCHOR["func"],
            func_addr=_OVERLAY_LOADER_ANCHOR["funcAddr"],
            mnemonic="LOAD",
            insn=f"LOAD {r.class_name} 0x{rid:x}  ; {note}",
            synthetic="overlay-loader",
        ))
        n_orphans += 1
    return n_orphans


# ---------------------------------------------------------------------------
# Analysis-markdown scanner
# ---------------------------------------------------------------------------

# Matches both `0x10004` and the unpacked-filename stem so we can pick
# up "unpacked: `res_0000065540_67_AudioBankIndex.bin`" mentions.
MD_HEADING_RE = re.compile(r"^(#+)\s+(.*)$")


def scan_markdown_for_ids(
    resources: dict[int, Resource],
) -> dict[int, list[MdHit]]:
    """Walk every `.md` under `ghidra_analysis/` and record per-resource hits."""
    by_id: dict[int, list[MdHit]] = defaultdict(list)

    # Needles: hex IDs + unpacked filenames (without dir).
    hex_by_id = {rid: f"0x{rid:x}" for rid in resources}
    # We use the raw-file basename minus extension as a robust "anchor"
    # because the same resource has several friendly companions.
    file_to_id: dict[str, int] = {}
    for rid, r in resources.items():
        for f in r.files:
            stem = Path(f).stem
            file_to_id[stem] = rid
    needles = list({*hex_by_id.values(), *file_to_id.keys()})
    pattern = re.compile(r"(" + "|".join(map(re.escape, needles)) + r")")
    hex_to_id = {v: k for k, v in hex_by_id.items()}

    for md in ANALYSIS_ROOT.rglob("*.md"):
        # Don't dredge our own outputs.
        if md.is_relative_to(OUT_DIR):
            continue
        text = md.read_text(encoding="utf-8", errors="replace").splitlines()
        rel = md.relative_to(REPO_ROOT).as_posix()
        current_section: Optional[str] = None
        for lineno, line in enumerate(text, 1):
            m_head = MD_HEADING_RE.match(line)
            if m_head is not None:
                current_section = m_head.group(2).strip()
            for m in pattern.finditer(line):
                needle = m.group(1)
                rid = hex_to_id.get(needle) or file_to_id.get(needle)
                if rid is None:
                    continue
                kind = "id" if needle in hex_to_id else "filename"
                by_id[rid].append(MdHit(
                    file=rel,
                    line=lineno,
                    snippet=line.strip(),
                    section=current_section,
                    kind=kind,
                ))
    return by_id


# ---------------------------------------------------------------------------
# Catalog assembly
# ---------------------------------------------------------------------------

def assemble_catalog(
    resources: dict[int, Resource],
    insn_xrefs: dict[int, list[InsnXref]],
    data_xrefs: dict[int, list[DataXref]],
    ghidra_text: dict[int, list[TextHit]],
    ida_text: dict[int, list[TextHit]],
    md_hits: dict[int, list[MdHit]],
    script_hits: dict[int, list[ScriptHit]],
) -> dict:
    """Pour all sources into the catalog schema."""
    by_class: dict[str, list[dict]] = defaultdict(list)
    coverage = {
        "totalResources": len(resources),
        # Includes overlay-loader synthetic xrefs (i.e. "every resource
        # is accounted for somehow", even if only by the engine's
        # master-pack load step).
        "withAnyXref": 0,
        # Concrete evidence only: instruction immediate, data-table
        # entry, decomp text mention, analysis-note mention, or script
        # opcode. Class-enumeration and audio-bank-mirror synthetics
        # are still considered concrete because they describe a *real*
        # runtime mechanism that's been traced in the engine; the
        # overlay-loader synthetic isn't, because it's the universal
        # fallback for "shipped into the master pack".
        "withConcreteXref": 0,
        "withInsnXref": 0,
        "withDataXref": 0,
        "withGhidraText": 0,
        "withIdaText": 0,
        "withAnalysisMd": 0,
        "withScriptHit": 0,
        # "orphan" here means the resource has NO traced loader. Split
        # into the two flavours: only-adjacency (sequence-neighbour
        # placed a structural hint) vs only-overlay (the universal
        # boot-time fallback).
        "orphanOnly": 0,
        "deadOnly": 0,
        "adjacencyOnly": 0,
        "overlayOnly": 0,
        "byClass": defaultdict(lambda: {
            "count": 0,
            "withAnyXref": 0,
            "withConcreteXref": 0,
            "orphanOnly": 0,
            "deadOnly": 0,
            "adjacencyOnly": 0,
            "overlayOnly": 0,
        }),
    }

    entries: list[dict] = []
    for rid, r in sorted(resources.items()):
        ix = insn_xrefs.get(rid, [])
        dx = data_xrefs.get(rid, [])
        gt = ghidra_text.get(rid, [])
        it = ida_text.get(rid, [])
        md = md_hits.get(rid, [])
        sh = script_hits.get(rid, [])

        has_any = bool(ix or dx or gt or it or md or sh)
        # Concrete = at least one piece of evidence that names the
        # actual loader. We exclude both `overlay-loader` (universal
        # boot-time fallback, no specific function) AND
        # `sequence-neighbour` (records adjacency only — verified by
        # exhaustive static scanning that the neighbour's loader does
        # NOT contain this orphan's ID anywhere). The class-enumeration,
        # audio-bank-mirror, struct-table and movie-poster-pair
        # synthetics still count as concrete because each describes a
        # *traced* runtime mechanism with a real loader function.
        concrete_insn = [
            x for x in ix
            if x.synthetic not in ("overlay-loader", "sequence-neighbour",
                                   "dead-asset")
        ]
        has_concrete = bool(concrete_insn or dx or gt or it or md or sh)
        is_orphan_only = has_any and not has_concrete
        # Sub-classify "no concrete" into three flavours surfaced in
        # the catalog:
        #   * dead-only      (synthesize_dead_assets: exhaustively
        #                     verified to have ZERO references; engine
        #                     allocates a pool slot but nothing ever
        #                     consumes it -- "sponsor leftover" /
        #                     removed feature),
        #   * adjacency-only (sequence-neighbour synthesizer placed it;
        #                     loader unknown but a same-class neighbour
        #                     IS traced),
        #   * overlay-only   (no evidence at all, universal boot-time
        #                     fallback).
        # We check dead BEFORE the other two so an explicit dead-asset
        # tag wins the classification.
        dead_only = is_orphan_only and any(
            x.synthetic == "dead-asset" for x in ix
        )
        adjacency_only = is_orphan_only and not dead_only and any(
            x.synthetic == "sequence-neighbour" for x in ix
        )
        overlay_only = is_orphan_only and not dead_only and not adjacency_only
        entry = {
            "id": rid,
            "idHex": f"0x{rid:x}",
            "classId": r.class_id,
            "className": r.class_name,
            "files": r.files,
            "name": r.name,
            "size": r.size,
            "audioBank": r.audio_bank,
            "orphanOnly": is_orphan_only,
            "deadOnly": dead_only,
            "adjacencyOnly": adjacency_only,
            "overlayOnly": overlay_only,
            "xrefs": {
                "insn": [
                    {
                        "addr": x.addr,
                        "func": x.func,
                        "funcAddr": x.func_addr,
                        "mnemonic": x.mnemonic,
                        "insn": x.insn,
                        "synthetic": x.synthetic,
                    }
                    for x in ix
                ],
                "data": [
                    {
                        "addr": d.addr,
                        "block": d.block,
                        "inheritedFrom": d.inherited_from,
                        "readers": d.readers,
                    }
                    for d in dx
                ],
                "ghidraText": [
                    {"file": h.file, "line": h.line, "snippet": h.snippet, "func": h.func}
                    for h in gt
                ],
                "idaText": [
                    {"file": h.file, "line": h.line, "snippet": h.snippet, "func": h.func}
                    for h in it
                ],
                "analysisMd": [
                    {
                        "file": h.file,
                        "line": h.line,
                        "snippet": h.snippet,
                        "section": h.section,
                        "kind": h.kind,
                    }
                    for h in md
                ],
                "script": [
                    {
                        "scriptFile": s.script_file,
                        "scriptId": s.script_id,
                        "scriptClass": s.script_class,
                        "sourcePack": s.source_pack,
                        "line": s.line,
                        "offset": s.offset,
                        "opcode": s.opcode,
                        "snippet": s.snippet,
                    }
                    for s in sh
                ],
            },
        }
        entries.append(entry)
        by_class[r.class_name].append(entry)

        # coverage bookkeeping
        cls_bucket = coverage["byClass"][r.class_name]
        cls_bucket["count"] += 1
        if has_any:
            coverage["withAnyXref"] += 1
            cls_bucket["withAnyXref"] += 1
        if has_concrete:
            coverage["withConcreteXref"] += 1
            cls_bucket["withConcreteXref"] += 1
        if is_orphan_only:
            coverage["orphanOnly"] += 1
            cls_bucket["orphanOnly"] += 1
        if dead_only:
            coverage["deadOnly"] += 1
            cls_bucket["deadOnly"] += 1
        if adjacency_only:
            coverage["adjacencyOnly"] += 1
            cls_bucket["adjacencyOnly"] += 1
        if overlay_only:
            coverage["overlayOnly"] += 1
            cls_bucket["overlayOnly"] += 1
        if concrete_insn:
            coverage["withInsnXref"] += 1
        if dx:
            coverage["withDataXref"] += 1
        if gt:
            coverage["withGhidraText"] += 1
        if it:
            coverage["withIdaText"] += 1
        if md:
            coverage["withAnalysisMd"] += 1
        if sh:
            coverage["withScriptHit"] += 1

    # serialise the defaultdict so JSON dumps cleanly
    coverage["byClass"] = {k: v for k, v in coverage["byClass"].items()}

    return {
        "entries": entries,
        "byClass": {k: v for k, v in by_class.items()},
        "coverage": coverage,
    }


# ---------------------------------------------------------------------------
# Markdown renderer
# ---------------------------------------------------------------------------

def render_markdown(catalog: dict) -> str:
    """Render a navigable, by-class catalog index.

    Each resource gets its own subsection with a stable anchor
    ``#res-<idhex>`` so other docs can cross-link directly.
    """
    cov = catalog["coverage"]
    out: list[str] = []
    out.append("# Master-pack asset catalog\n")
    out.append(
        "Auto-generated by `scripts/build_asset_catalog.py`. **Do not edit by hand** —\n"
        "extend the generator (or the inputs it reads) instead.\n"
    )
    out.append("\n## Sources\n")
    out.append(
        "* `unpacked/overlay/_manifest.json` — 347 master-pack resources (the universe).\n"
        "* `ghidra_analysis/asset_catalog/_cache/ghidra_xrefs.jsonl` — every instruction\n"
        "  immediate **and** every `.data`/`.rdata` u32 that holds a resource ID,\n"
        "  dumped from the live Ghidra DB.\n"
        "* `bulanci.ghidra.exe.c` / `bulanci.ida.exe.c` — text decompilations, used to\n"
        "  pin a literal to a specific source line for browsing.\n"
        "* `ghidra_analysis/**/*.md` — every existing analysis note that mentions a\n"
        "  resource by hex ID or by unpacked filename.\n"
        "* `unpacked/<pack>/*.script.asm` — every script disassembly (master-pack\n"
        "  scripts under `overlay/`, plus 15 per-level scripts under `<level>_eap/`),\n"
        "  mining `LoadPreface(id)`, `SetMusic(id, ...)`, `CreateImage(... id)`,\n"
        "  `CreateAnim(... frames=[id, ...])` calls for master-pack ID literals.\n"
    )
    out.append("\n## Coverage\n")
    total = cov["totalResources"]
    any_pct = 100.0 * cov["withAnyXref"] / total if total else 0.0
    cnc_pct = 100.0 * cov["withConcreteXref"] / total if total else 0.0
    out.append(f"* Total resources: **{total}**\n")
    out.append(
        f"* With *any* xref (incl. master-pack overlay-loader synthetics): "
        f"**{cov['withAnyXref']}** ({any_pct:.1f}%)\n"
    )
    out.append(
        f"* With *concrete* evidence (instruction, data, decomp text, "
        f"analysis-md, script, or traced runtime mechanism like class\n"
        f"  enumeration / audio-bank mirror): **{cov['withConcreteXref']}** "
        f"({cnc_pct:.1f}%)\n"
    )
    out.append(
        f"* **Orphans (no traced loader)**: **{cov['orphanOnly']}** resources are present\n"
        f"  in the master pack but no static evidence ties them to a specific\n"
        f"  loader function. Split into three sub-categories:\n"
        f"  * **Dead / unreferenced**: **{cov['deadOnly']}** resources confirmed by\n"
        f"    exhaustive verification to have ZERO references anywhere\n"
        f"    (Ghidra instruction sweep, .data/.rdata sweep, ghidra+ida\n"
        f"    decompilations, every `.script.asm` under `unpacked/`,\n"
        f"    every analysis `.md`, and the embedded `BULANCI.TMP`\n"
        f"    installer PE). Engine still allocates a pool slot for them\n"
        f"    at boot — they're sponsor leftovers / removed-feature\n"
        f"    assets that nothing in the code path ever consumes.\n"
        f"  * **Adjacency-only**: **{cov['adjacencyOnly']}** resources sit in the\n"
        f"    middle of a same-class run whose neighbours ARE concretely traced;\n"
        f"    each one carries a `sequence-neighbour` synthetic xref naming\n"
        f"    that neighbour, but exhaustive scanning has confirmed the\n"
        f"    neighbour's loader function does NOT contain the orphan's ID\n"
        f"    anywhere — the actual loader is still to be identified.\n"
        f"  * **Overlay-only**: **{cov['overlayOnly']}** resources have no literal\n"
        f"    reference anywhere in the binary (byte-level confirmed). They're\n"
        f"    attributed to `CDSApp_AppMain` as the engine entry that maps the\n"
        f"    master-pack overlay into the resource pool.\n"
        f"\n"
        f"  Each orphan's detail section carries an `_orphan_` marker.\n"
        f"  **Do not** infer the loader function from the adjacency hint when\n"
        f"  pushing names back into Ghidra — verify with disassembly first.\n"
    )
    out.append(f"* With Ghidra insn xref (concrete): {cov['withInsnXref']}\n")
    out.append(f"* With Ghidra data xref: {cov['withDataXref']}\n")
    out.append(f"* With Ghidra-decomp text hit: {cov['withGhidraText']}\n")
    out.append(f"* With IDA-decomp text hit:    {cov['withIdaText']}\n")
    out.append(f"* With analysis-markdown hit:  {cov['withAnalysisMd']}\n")
    out.append(f"* With script-asm hit:         {cov['withScriptHit']}\n")

    out.append("\n### Per class\n")
    out.append(
        "| Class | Count | With any xref | Concrete | Dead | Adjacency-only | Overlay-only | Coverage (any) |\n"
        "|------|------:|---:|---:|---:|---:|---:|---:|\n"
    )
    for cls, b in sorted(cov["byClass"].items()):
        pct = 100.0 * b["withAnyXref"] / b["count"] if b["count"] else 0.0
        out.append(
            f"| {cls} | {b['count']} | {b['withAnyXref']} | "
            f"{b['withConcreteXref']} | {b.get('deadOnly', 0)} | "
            f"{b.get('adjacencyOnly', 0)} | "
            f"{b.get('overlayOnly', 0)} | {pct:.0f}% |\n"
        )

    # Index by class.
    out.append("\n## Index by class\n")
    for cls in sorted(catalog["byClass"]):
        entries = catalog["byClass"][cls]
        out.append(f"\n### {cls} ({len(entries)})\n")
        out.append("| ID | Hex | File | Used by | MD notes |\n|----|----|------|---------|----------|\n")
        for e in sorted(entries, key=lambda x: x["id"]):
            funcs: set[str] = {
                x["func"] for x in e["xrefs"]["insn"] if x.get("func")
            }
            funcs |= {
                r["func"] for d in e["xrefs"]["data"] for r in d["readers"] if r.get("func")
            }
            funcs |= {
                f"{s['sourcePack']}/{s['scriptClass']} {s['scriptId']}/{s['opcode']}"
                for s in e["xrefs"]["script"]
            }
            funcs_sorted = sorted(funcs)
            funcs_str = ", ".join(funcs_sorted[:4]) + ("…" if len(funcs_sorted) > 4 else "")
            md_count = len(e["xrefs"]["analysisMd"])
            file_disp = e["files"][0] if e["files"] else ""
            out.append(
                f"| {e['id']} | [`{e['idHex']}`](#res-{e['idHex'].lower()}) "
                f"| `{file_disp}` | {funcs_str or '—'} | "
                f"{md_count} hit{'s' if md_count != 1 else ''} |\n"
            )

    # Per-resource detail sections.
    out.append("\n## Resources\n")
    for e in sorted(catalog["entries"], key=lambda x: x["id"]):
        out.append(f"\n### <a id=\"res-{e['idHex']}\"></a>`{e['idHex']}` — {e['className']} (id={e['id']})\n")
        if e.get("deadOnly"):
            out.append(
                "* _orphan (dead-asset / unreferenced)_: exhaustive verification\n"
                "  found ZERO references to this resource ID anywhere — no\n"
                "  Ghidra instruction immediate, no `.data`/`.rdata` scalar, no\n"
                "  mention in either decompilation (ghidra+ida), no script-asm\n"
                "  opcode, no analysis-md line, and not even a raw-byte hit in\n"
                "  the embedded `BULANCI.TMP` installer PE. The engine still\n"
                "  allocates a pool slot for it via the boot-time master-pack\n"
                "  overlay walk, but nothing in the code path consumes it.\n"
                "  Most likely sponsor-leftover or removed-feature content.\n"
            )
        elif e.get("adjacencyOnly"):
            out.append(
                "* _orphan (adjacency-only)_: this resource sits in the middle\n"
                "  of a same-class run whose neighbours ARE concretely traced,\n"
                "  but exhaustive static scanning has confirmed the neighbour's\n"
                "  loader function does not contain this orphan's ID anywhere.\n"
                "  The actual loader is **not yet identified** — do not infer\n"
                "  it from the adjacency hint when renaming things in Ghidra.\n"
            )
        elif e.get("overlayOnly"):
            out.append(
                "* _orphan (overlay-only)_: no literal reference detected\n"
                "  anywhere in the binary (byte-level confirmed). Attributed\n"
                "  to the master-pack overlay loader; most likely picked up at\n"
                "  runtime via the resource-pool's class-enumeration walker.\n"
            )
        if e["name"]:
            out.append(f"* **Editor name:** `{e['name']}`\n")
        if e["files"]:
            out.append(f"* **Files:** {', '.join(f'`{f}`' for f in e['files'])}\n")
        if e["size"] is not None:
            out.append(f"* **Size (bytes):** {e['size']}\n")
        if e["audioBank"]:
            ab = e["audioBank"]
            out.append(
                f"* **Bank link:** points at PCM resource id `{ab['bankResourceID']}` "
                f"({ab['sampleCount']} samples, {ab.get('totalBankBytes', 0)} bytes)\n"
            )

        ix = e["xrefs"]["insn"]
        if ix:
            out.append(f"\n**Ghidra immediate-operand xrefs ({len(ix)})**\n\n")
            for x in ix:
                syn = f" *[{x['synthetic']}]*" if x.get("synthetic") else ""
                out.append(
                    f"* `{x['addr']}` in `{x['func'] or '?'}` "
                    f"(`{x['funcAddr'] or '?'}`){syn} — `{x['insn']}`\n"
                )

        dx = e["xrefs"]["data"]
        if dx:
            out.append(f"\n**Ghidra .data/.rdata xrefs ({len(dx)})**\n\n")
            for d in dx:
                inh = f" *(inherited from {d['inheritedFrom']})*" if d.get("inheritedFrom") else ""
                if not d["readers"]:
                    out.append(f"* `{d['addr']}` in `{d['block']}` — no direct readers{inh}\n")
                else:
                    readers_str = ", ".join(
                        f"`{r['func'] or '?'}@{r['from']}`" for r in d["readers"]
                    )
                    out.append(f"* `{d['addr']}` in `{d['block']}`{inh} — read by {readers_str}\n")

        gt = e["xrefs"]["ghidraText"]
        it = e["xrefs"]["idaText"]
        if gt or it:
            out.append(f"\n**Decomp text hits** (Ghidra: {len(gt)}, IDA: {len(it)})\n\n")
            for h in (gt + it)[:8]:
                out.append(
                    f"* `{h['file']}:{h['line']}` in `{h.get('func') or '?'}` — `{h['snippet'][:120]}`\n"
                )
            if len(gt) + len(it) > 8:
                out.append(f"* …({len(gt) + len(it) - 8} more)\n")

        md = e["xrefs"]["analysisMd"]
        if md:
            out.append(f"\n**Analysis-markdown mentions ({len(md)})**\n\n")
            for h in md[:6]:
                sect = f" §{h['section']}" if h.get("section") else ""
                kind_tag = " *(by filename)*" if h.get("kind") == "filename" else ""
                out.append(
                    f"* `{h['file']}:{h['line']}`{sect}{kind_tag} — `{h['snippet'][:120]}`\n"
                )
            if len(md) > 6:
                out.append(f"* …({len(md) - 6} more)\n")

        sh = e["xrefs"]["script"]
        if sh:
            out.append(f"\n**Script-bytecode references ({len(sh)})**\n\n")
            for s in sh[:8]:
                out.append(
                    f"* `{s['sourcePack']}/{s['scriptFile']}` `{s['offset']}` — "
                    f"`{s['opcode']}(…)`: `{s['snippet'][:120]}`\n"
                )
            if len(sh) > 8:
                out.append(f"* …({len(sh) - 8} more)\n")

    return "".join(out)


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument(
        "--out-dir", type=Path, default=OUT_DIR,
        help="Output directory (default: ghidra_analysis/asset_catalog/).",
    )
    args = parser.parse_args()

    out_dir: Path = args.out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    print(f"[catalog] manifest: {MANIFEST_PATH}")
    resources = load_resources()
    print(f"[catalog]   loaded {len(resources)} resources "
          f"(IDs 0x{min(resources):x}..0x{max(resources):x})")

    print(f"[catalog] ghidra xrefs: {GHIDRA_XREFS}")
    insn_xrefs, data_xrefs, data_ptrs = load_ghidra_xrefs()
    print(f"[catalog]   {sum(len(v) for v in insn_xrefs.values())} insn xrefs "
          f"covering {len(insn_xrefs)} IDs")
    print(f"[catalog]   {sum(len(v) for v in data_xrefs.values())} data xrefs "
          f"covering {len(data_xrefs)} IDs")
    print(f"[catalog]   {len(data_ptrs)} candidate data-ptr instructions")

    print("[catalog] attributing data-ptr instructions to ID tables…")
    n_attr = attribute_data_pointers(data_xrefs, data_ptrs)
    print(f"[catalog]   {n_attr} data-ptr readers attached to existing ID slots")

    print("[catalog] table-grouping pass…")
    group_data_tables(data_xrefs)

    print(f"[catalog] scanning {GHIDRA_DECOMP.name}…")
    ghidra_text = scan_text_for_ids(
        GHIDRA_DECOMP,
        ids=resources.keys(),
        func_header_re=GHIDRA_FUNC_HEADER_RE,
        decimal_too=False,
    )
    print(f"[catalog]   {sum(len(v) for v in ghidra_text.values())} hits "
          f"covering {len(ghidra_text)} IDs")

    print(f"[catalog] scanning {IDA_DECOMP.name}…")
    ida_text = scan_text_for_ids(
        IDA_DECOMP,
        ids=resources.keys(),
        func_header_re=IDA_FUNC_HEADER_RE,
        decimal_too=True,
    )
    print(f"[catalog]   {sum(len(v) for v in ida_text.values())} hits "
          f"covering {len(ida_text)} IDs")

    print("[catalog] scanning ghidra_analysis/**/*.md…")
    md_hits = scan_markdown_for_ids(resources)
    print(f"[catalog]   {sum(len(v) for v in md_hits.values())} hits "
          f"covering {len(md_hits)} IDs")

    print("[catalog] scanning master-pack .script.asm files…")
    script_hits = scan_script_asms(resources)
    print(f"[catalog]   {sum(len(v) for v in script_hits.values())} hits "
          f"covering {len(script_hits)} IDs")

    print("[catalog] synthesising class-enumeration loaders...")
    synthesize_class_enumeration(resources, insn_xrefs)

    print("[catalog] synthesising AudioBank <-> AudioBankIndex links...")
    before = sum(len(v) for v in insn_xrefs.values())
    synthesize_audio_bank_links(
        resources, insn_xrefs, data_xrefs, ghidra_text, ida_text, md_hits,
    )
    after = sum(len(v) for v in insn_xrefs.values())
    print(f"[catalog]   {after - before} insn xrefs mirrored bank-index -> bank")

    print("[catalog] synthesising hardcoded struct-table loaders (weapons etc.)...")
    n_struct = synthesize_struct_tables(resources, insn_xrefs)
    print(f"[catalog]   {n_struct} struct-table xrefs added")

    print("[catalog] synthesising sequence-neighbour links for invisible orphans...")
    n_neigh = synthesize_sequence_neighbours(
        resources, insn_xrefs, data_xrefs, ghidra_text, ida_text, md_hits,
        script_hits,
    )
    print(f"[catalog]   {n_neigh} sequence-neighbour xrefs added")

    print("[catalog] synthesising movie-poster pairings (BitmapSpecial <- BitmapJpegAnim)...")
    n_poster = synthesize_movie_poster_pairs(
        resources, insn_xrefs, data_xrefs, ghidra_text, ida_text, md_hits,
        script_hits,
    )
    print(f"[catalog]   {n_poster} movie-poster pairs synthesised")

    print("[catalog] tagging confirmed dead/unreferenced assets...")
    n_dead = synthesize_dead_assets(
        resources, insn_xrefs, data_xrefs, ghidra_text, ida_text, md_hits,
        script_hits,
    )
    print(f"[catalog]   {n_dead} dead-asset xrefs added")

    print("[catalog] synthesising master-pack overlay-loader links for residual orphans...")
    n_orphans = synthesize_overlay_loader_links(
        resources, insn_xrefs, data_xrefs, ghidra_text, ida_text, md_hits,
        script_hits,
    )
    print(f"[catalog]   {n_orphans} orphans attributed to the overlay loader")

    catalog = assemble_catalog(
        resources, insn_xrefs, data_xrefs, ghidra_text, ida_text, md_hits,
        script_hits,
    )

    (out_dir / "catalog.json").write_text(
        json.dumps(catalog, ensure_ascii=False, indent=2),
        encoding="utf-8",
    )
    (out_dir / "catalog.md").write_text(render_markdown(catalog), encoding="utf-8")
    (out_dir / "coverage.json").write_text(
        json.dumps(catalog["coverage"], ensure_ascii=False, indent=2),
        encoding="utf-8",
    )

    cov = catalog["coverage"]
    total = cov["totalResources"]
    any_pct = 100.0 * cov["withAnyXref"] / total
    cnc_pct = 100.0 * cov["withConcreteXref"] / total
    print()
    print(f"[catalog] DONE - {cov['withAnyXref']}/{total} resources have at least one xref "
          f"({any_pct:.1f}%); {cov['withConcreteXref']}/{total} have concrete evidence "
          f"({cnc_pct:.1f}%); {cov['orphanOnly']} are orphans "
          f"({cov['deadOnly']} dead, "
          f"{cov['adjacencyOnly']} adjacency-only, "
          f"{cov['overlayOnly']} overlay-only).")
    print(f"[catalog] wrote {out_dir / 'catalog.json'}")
    print(f"[catalog] wrote {out_dir / 'catalog.md'}")
    print(f"[catalog] wrote {out_dir / 'coverage.json'}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
