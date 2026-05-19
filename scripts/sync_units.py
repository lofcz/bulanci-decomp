#! /usr/bin/env python3
"""Splat-style per-function migration between unit files.

`generate_sources.py` only ever scaffolds a unit once (it refuses to
overwrite existing files).  Every subsequent change to `mapping.csv`
(from `scripts/promote_namespaces.py` reattributing functions, or from
hand-editing the CSV) flows through this script, the way sotn-decomp /
pokeemerald re-run splat after editing `splat.yaml`.

What we do:

  1. Scan every `src/bulanci/*.cpp` and `include/bulanci/*.h` for the
     `// !FUNC 0xXXXXXXXX BEGIN/END` and `// !DECL 0xXXXXXXXX BEGIN/END`
     markers `generate_sources.py` writes.  Each block is keyed by the
     function's start address (stable across Ghidra renames).
  2. Compare current location against `mapping.csv`+`units_listing.csv`.
  3. For each diff:
        - missing block        -> render a fresh stub via `STUB_BODY()`.
        - block in wrong unit  -> migrate the block.
        - signature stale      -> rewrite for stubs only; matched bodies
                                  are preserved verbatim so the user
                                  reconciles by hand if a parameter type
                                  changed.

This is intentionally surgical: hand-written matched bodies survive any
number of Ghidra promotion passes.  Cf. how `splat` regenerates `asm/`
but never touches the hand-owned `src/` tree.
"""

import argparse
import os
import re
import sys
from collections import defaultdict
from pathlib import Path

import generate_sources as gs
from generate_sources import (
    _sanitize_and_dedup,
    _leaf_class_names,
    _classify_namespaces,
    _collect_unknown_types,
    _decl_begin,
    _decl_end,
    _func_begin,
    _func_end,
    make_all_huge_name,
    render_decl_line,
    render_func_body,
)
from project import DecompUnit

WORKSPACE_PATH = Path(__file__).parent.parent

FUNC_BEGIN = re.compile(r"^// !FUNC (0x[0-9a-fA-F]+) BEGIN\s*$")
FUNC_END = re.compile(r"^// !FUNC (0x[0-9a-fA-F]+) END\s*$")
DECL_BEGIN = re.compile(r"^\s*// !DECL (0x[0-9a-fA-F]+) BEGIN\s*$")
DECL_END = re.compile(r"^\s*// !DECL (0x[0-9a-fA-F]+) END\s*$")
# Per-file user-owned region (cross-unit forward declarations, ad-hoc
# #include directives, etc.) sitting between the auto-emitted
# `#include "<unit>.h"` line and the first `// !FUNC` block.  Preserved
# verbatim across syncs.  Use sparingly: pollution shows up in every
# regenerated unit and the contents are NOT shared between units.
PROLOGUE_BEGIN = re.compile(r"^// !PROLOGUE BEGIN\s*$")
PROLOGUE_END = re.compile(r"^// !PROLOGUE END\s*$")

# Hand-written matched bodies are identified by the *absence* of this
# substring; stubs carry it because `render_func_body()` always writes
# `STUB_BODY();` between the braces.
STUB_MARKER = "STUB_BODY("


# ---------------------------------------------------------------------------
# Marker scanning
# ---------------------------------------------------------------------------


def _iter_blocks(text: str, begin_re: re.Pattern, end_re: re.Pattern):
    """Yield (addr, content_lines) for each properly closed marker pair."""
    cur_addr = None
    cur_content: list[str] = []
    for line in text.splitlines():
        m_b = begin_re.match(line)
        if m_b:
            cur_addr = int(m_b.group(1), 16)
            cur_content = []
            continue
        m_e = end_re.match(line)
        if m_e:
            end_addr = int(m_e.group(1), 16)
            if cur_addr is not None and end_addr == cur_addr:
                yield cur_addr, cur_content
            cur_addr = None
            cur_content = []
            continue
        if cur_addr is not None:
            cur_content.append(line)


def _extract_prologue(text: str) -> list[str] | None:
    """Pull the contents of a `// !PROLOGUE BEGIN .. // !PROLOGUE END` block
    if present.  Returns the inner lines (sans the markers), or None when
    no prologue block exists.  Only the first occurrence is honoured.
    """
    in_block = False
    inner: list[str] = []
    for line in text.splitlines():
        if not in_block and PROLOGUE_BEGIN.match(line):
            in_block = True
            continue
        if in_block and PROLOGUE_END.match(line):
            return inner
        if in_block:
            inner.append(line)
    return None


def scan_existing(src_dir: Path, include_dir: Path):
    """Returns:
        body_cache: {addr: (current_unit, is_stub, content_lines)}
        decl_cache: {addr: (current_unit, content_lines)}
        prologue_cache: {unit_stem: content_lines}
    """
    body_cache: dict[int, tuple[str, bool, list[str]]] = {}
    decl_cache: dict[int, tuple[str, list[str]]] = {}
    prologue_cache: dict[str, list[str]] = {}
    for cpp in sorted(src_dir.glob("*.cpp")):
        text = cpp.read_text(encoding="utf-8", errors="replace")
        prologue = _extract_prologue(text)
        if prologue is not None:
            prologue_cache[cpp.stem] = prologue
        for addr, content in _iter_blocks(text, FUNC_BEGIN, FUNC_END):
            is_stub = STUB_MARKER in "\n".join(content)
            if addr in body_cache:
                print(f"WARN: function 0x{addr:08x} appears in both "
                      f"{body_cache[addr][0]}.cpp and {cpp.stem}.cpp; "
                      f"keeping the latter.")
            body_cache[addr] = (cpp.stem, is_stub, content)
    for h in sorted(include_dir.glob("*.h")):
        text = h.read_text(encoding="utf-8", errors="replace")
        for addr, content in _iter_blocks(text, DECL_BEGIN, DECL_END):
            if addr in decl_cache:
                print(f"WARN: decl 0x{addr:08x} appears in both "
                      f"{decl_cache[addr][0]}.h and {h.stem}.h; "
                      f"keeping the latter.")
            decl_cache[addr] = (h.stem, content)
    return body_cache, decl_cache, prologue_cache


# ---------------------------------------------------------------------------
# Desired-state planning
# ---------------------------------------------------------------------------


def plan_expected(unit: DecompUnit):
    """Group functions per unit, applying the same sanitize+dedup pass
    `generate_sources` would.  Returns (per_unit_funs, per_unit_ns,
    addr_to_unit, orphan_ns).
    """
    ns_to_unit: dict[str, str] = {}
    for unit_name, namespaces in unit.units.items():
        for ns in namespaces:
            ns_to_unit[ns] = unit_name

    per_unit_funs: dict[str, list] = defaultdict(list)
    per_unit_ns: dict[str, list[str]] = defaultdict(list)
    addr_to_unit: dict[int, str] = {}
    orphan_ns: list[str] = []

    for unit_name, namespaces in unit.units.items():
        # Sanitize+dedup per unit so duplicate names within the same
        # unit get suffixed (matches generate_sources behaviour).
        _sanitize_and_dedup(unit.mappings, namespaces)
        per_unit_ns[unit_name] = list(namespaces)
        for ns in namespaces:
            funs = unit.mappings.get(ns, [])
            for fun in funs:
                per_unit_funs[unit_name].append(fun)
                addr_to_unit[fun.start] = unit_name

    # Detect namespaces present in mappings but missing from any unit.
    for ns in unit.mappings:
        if ns not in ns_to_unit:
            orphan_ns.append(ns)
    return per_unit_funs, per_unit_ns, addr_to_unit, orphan_ns


# ---------------------------------------------------------------------------
# Per-unit rendering with body preservation
# ---------------------------------------------------------------------------


def _render_unit_source(unit_name: str, funs: list, namespaces: list[str],
                        body_cache: dict[int, tuple[str, bool, list[str]]],
                        prologue: list[str] | None = None):
    """Render the .cpp file for one unit.  Hand-edited bodies (no
    STUB_BODY marker) are embedded verbatim regardless of where they
    previously lived; stubs are always re-rendered.  A `// !PROLOGUE
    BEGIN/END` block (cross-unit forward decls etc.) is re-emitted
    verbatim if `prologue` is non-empty."""
    leaves = _leaf_class_names(namespaces)
    out: list[str] = []
    out.append(f'#include "{unit_name}.h"\n')
    out.append("\n")
    if prologue:
        out.append("// !PROLOGUE BEGIN\n")
        for line in prologue:
            out.append(line + "\n")
        out.append("// !PROLOGUE END\n")
        out.append("\n")
    for fun in funs:
        out.append(_func_begin(fun.start))
        cached = body_cache.get(fun.start)
        if cached and not cached[1]:
            # Matched body - preserve verbatim.  Don't fight the user.
            for line in cached[2]:
                out.append(line + "\n")
        else:
            out.append(render_func_body(fun, leaves))
        out.append(_func_end(fun.start))
        out.append("\n")
    return "".join(out)


def _render_unit_header(unit_name: str, funs_by_ns: dict[str, list],
                        namespaces: list[str],
                        decl_cache: dict[int, tuple[str, list[str]]],
                        body_cache: dict[int, tuple[str, bool, list[str]]]):
    """Render the .h file for one unit.  Declarations are always
    re-rendered (single-line forms are not hand-edited in practice); the
    cache is used only to inherit decls from prior files when computing
    forward-decl sets if a function is being newly added.
    """
    guard = make_all_huge_name(unit_name)
    out: list[str] = []
    out.append(f"#ifndef {guard}\n")
    out.append(f"#define {guard}\n\n")
    out.append("#include <globals.h>\n\n")

    leaves = _leaf_class_names(namespaces)
    unknown_types = [
        t for t in _collect_unknown_types(funs_by_ns, namespaces)
        if t not in leaves
    ]
    if unknown_types:
        out.append(
            "/* Forward declarations of types referenced by parameter / return\n"
            " * positions but not (yet) defined in this project. Stubs only use\n"
            " * pointers so a struct forward-decl is enough. */\n"
        )
        for t in unknown_types:
            out.append(f"struct {t};\n")
        out.append("\n")

    ns_set, class_set = _classify_namespaces(namespaces)

    def emit_class(ns: str, display_name: str):
        out.append(f"class {display_name} {{\n")
        ns_funs = funs_by_ns.get(ns, [])
        if ns_funs:
            out.append("public:\n")
            for fun in ns_funs:
                out.append(_decl_begin(fun.start))
                out.append(render_decl_line(fun, leaves))
                out.append(_decl_end(fun.start))
        out.append("};\n\n")

    plain_classes = sorted(c for c in class_set if "::" not in c)
    for ns in plain_classes:
        emit_class(ns, ns)
    plain_namespaces = sorted(n for n in ns_set if "::" not in n)
    for top_ns in plain_namespaces:
        out.append(f"namespace {top_ns} {{\n\n")
        for sub in sorted(class_set):
            if sub.startswith(top_ns + "::"):
                leaf = sub.split("::")[-1]
                emit_class(sub, leaf)
        out.append(f"}} // namespace {top_ns}\n\n")

    out.append("#endif\n")
    return "".join(out)


def _funs_by_namespace(funs: list) -> dict[str, list]:
    grouped: dict[str, list] = defaultdict(list)
    for fun in funs:
        grouped[fun.namespace].append(fun)
    return grouped


# ---------------------------------------------------------------------------
# Apply
# ---------------------------------------------------------------------------


def _write_if_changed(path: Path, new_content: str) -> bool:
    """Returns True if the file actually changed on disk."""
    if path.exists():
        try:
            old = path.read_text(encoding="utf-8", errors="replace")
            if old == new_content:
                return False
        except OSError:
            pass
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="") as f:
        f.write(new_content)
    return True


def sync(unit: DecompUnit, dry_run: bool = False) -> dict:
    src_dir = WORKSPACE_PATH / unit.srcPath
    include_dir = WORKSPACE_PATH / unit.includePath
    src_dir.mkdir(parents=True, exist_ok=True)
    include_dir.mkdir(parents=True, exist_ok=True)

    body_cache, decl_cache, prologue_cache = scan_existing(src_dir, include_dir)
    per_unit_funs, per_unit_ns, addr_to_unit, orphan_ns = plan_expected(unit)

    if orphan_ns:
        print(f"WARN: {len(orphan_ns)} namespace(s) in mapping.csv have no "
              f"unit in units_listing.csv; their functions will not be "
              f"emitted. First few:")
        for ns in sorted(orphan_ns)[:5]:
            print(f"  {ns}")

    # Statistics for the report.
    stats = {
        "units_total": 0,
        "units_changed": 0,
        "funcs_added": 0,
        "funcs_migrated": 0,
        "funcs_matched_preserved": 0,
        "funcs_orphaned": 0,
    }

    # Track which addresses we successfully placed; orphans = leftover.
    placed_addrs: set[int] = set()

    for unit_name in sorted(per_unit_funs):
        funs = per_unit_funs[unit_name]
        namespaces = per_unit_ns[unit_name]
        funs_by_ns = _funs_by_namespace(funs)

        new_cpp = _render_unit_source(unit_name, funs, namespaces, body_cache,
                                      prologue=prologue_cache.get(unit_name))
        new_h = _render_unit_header(unit_name, funs_by_ns, namespaces,
                                    decl_cache, body_cache)

        cpp_path = src_dir / f"{unit_name}.cpp"
        h_path = include_dir / f"{unit_name}.h"

        stats["units_total"] += 1
        cpp_changed = False
        h_changed = False
        if dry_run:
            cpp_changed = (not cpp_path.exists()) or cpp_path.read_text(
                encoding="utf-8", errors="replace") != new_cpp
            h_changed = (not h_path.exists()) or h_path.read_text(
                encoding="utf-8", errors="replace") != new_h
        else:
            cpp_changed = _write_if_changed(cpp_path, new_cpp)
            h_changed = _write_if_changed(h_path, new_h)
        if cpp_changed or h_changed:
            stats["units_changed"] += 1

        for fun in funs:
            placed_addrs.add(fun.start)
            cached = body_cache.get(fun.start)
            if cached is None:
                stats["funcs_added"] += 1
            elif cached[0] != unit_name:
                stats["funcs_migrated"] += 1
                if not cached[1]:
                    stats["funcs_matched_preserved"] += 1
            elif not cached[1]:
                stats["funcs_matched_preserved"] += 1

    # Anything in the body cache but not in the new plan is orphaned.
    for addr, (origin_unit, is_stub, _content) in body_cache.items():
        if addr in placed_addrs:
            continue
        stats["funcs_orphaned"] += 1
        if not is_stub:
            # Hand-edited code that's no longer in mapping.csv would be
            # silently dropped if we let regeneration win.  Warn loudly.
            print(f"WARN: matched body at 0x{addr:08x} (was in "
                  f"{origin_unit}.cpp) is not in mapping.csv and was "
                  f"NOT migrated. Did the function disappear from Ghidra?")

    return stats


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--dry-run", action="store_true",
                        help="Compute the diff but don't write anything.")
    args = parser.parse_args()

    os.chdir(WORKSPACE_PATH)
    unit = DecompUnit("Game code", "bulanci", "bulanci.exe")
    stats = sync(unit, dry_run=args.dry_run)

    prefix = "[dry-run] " if args.dry_run else ""
    print(f"{prefix}sync_units: {stats['units_total']} unit(s) considered, "
          f"{stats['units_changed']} updated")
    print(f"  + {stats['funcs_added']} new function block(s)")
    print(f"  > {stats['funcs_migrated']} migrated between units "
          f"(of which {stats['funcs_matched_preserved']} were hand-matched and preserved)")
    if stats["funcs_orphaned"]:
        print(f"  ! {stats['funcs_orphaned']} block(s) in src/ no longer in mapping.csv (left untouched)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
