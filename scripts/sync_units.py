#! /usr/bin/env python3
"""Splat-style per-function migration between unit files.

`generate_sources.py` only ever scaffolds a unit once (it refuses to
overwrite existing files).  Every subsequent change to `mapping.csv`
(from `scripts/promote_namespaces.py` reattributing functions, or from
hand-editing the CSV) flows through this script, the way sotn-decomp /
pokeemerald re-run splat after editing `splat.yaml`.

The model is dead simple:

    The .cpp / .h file is plain text plus marker blocks
    (`// !FUNC 0xXXXX BEGIN/END` and `// !DECL 0xXXXX BEGIN/END`).
    *Only* marker-block contents are auto-managed.  *Everything* else
    in the file -- includes, class member vars, hand-written ctors,
    free-standing helpers, inheritance specs, `// !PROLOGUE` blocks,
    blank lines, comments -- is yours and survives verbatim.

What we do:

  1. Walk every `src/bulanci/*.cpp` and `include/bulanci/*.h`, locating
     each `// !FUNC` / `// !DECL` block keyed by the function's start
     address (stable across Ghidra renames).
  2. Compare current location against `mapping.csv`+`units_listing.csv`.
  3. For each diff:
        - missing block        -> append a fresh block at the right
                                  place (end of .cpp; just before the
                                  class's `};` for a .h).
        - block in wrong unit  -> drop it from the losing file and
                                  re-emit it in the winning file,
                                  preserving any matched body verbatim.
        - stub signature stale -> rewrite the stub line in place;
                                  matched bodies stay verbatim so the
                                  user reconciles by hand if a param
                                  type changed.
        - DECL signature stale -> rewrite the declaration but **replay
                                  any C++ modifiers** found in the
                                  existing block (`virtual`, `explicit`,
                                  `const`, `override`, ...).

This is intentionally surgical: hand-written matched bodies *and*
hand-edited declaration modifiers survive any number of Ghidra
promotion passes.  Cf. how `splat` regenerates `asm/` but never
touches the hand-owned `src/` tree.
"""

import argparse
import os
import re
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from _claim_guard import resolve_agent, guard_units  # noqa: E402

import generate_sources as gs
from generate_sources import (
    _sanitize_and_dedup,
    _leaf_class_names,
    _decl_begin,
    _decl_end,
    _func_begin,
    _func_end,
    render_decl_line,
    render_func_body,
)
from project import DecompUnit

WORKSPACE_PATH = Path(__file__).parent.parent

FUNC_BEGIN = re.compile(r"^// !FUNC (0x[0-9a-fA-F]+) BEGIN\s*$")
FUNC_END = re.compile(r"^// !FUNC (0x[0-9a-fA-F]+) END\s*$")
DECL_BEGIN = re.compile(r"^\s*// !DECL (0x[0-9a-fA-F]+) BEGIN\s*$")
DECL_END = re.compile(r"^\s*// !DECL (0x[0-9a-fA-F]+) END\s*$")

# Hand-written matched bodies are identified by the *absence* of this
# substring; stubs carry it because `render_func_body()` always writes
# `STUB_BODY();` between the braces.
STUB_MARKER = "STUB_BODY("

# C++ declaration modifiers that the auto-emitter does *not* know about
# (Ghidra prototypes don't carry virtuality, constness, override, etc.)
# but that the user routinely adds inside ``// !DECL`` blocks for
# v-table thunks, const accessors, and the like.  When we regenerate a
# DECL line we scan the existing block for these and splice them back
# in -- the user's hand-edit wins.
#
# ``static`` is intentionally excluded: it's already driven by
# ``FunctionType.STATIC`` in mapping.csv, so the auto-emitter manages
# it directly.
_USER_PRE_MODIFIERS = ("virtual", "explicit", "inline", "constexpr")
_USER_POST_MODIFIERS = ("const", "override", "final", "noexcept")


# ---------------------------------------------------------------------------
# File segmentation
#
# Every .cpp and .h we sync is parsed into a flat list of segments:
#
#     ("text", [lines])             -- user-owned, verbatim
#     ("func", addr, [block_lines]) -- marker-managed (cpp only)
#     ("decl", addr, [block_lines]) -- marker-managed (header only)
#
# ``block_lines`` includes the BEGIN and END marker lines themselves so
# we can preserve the user's leading-whitespace style on the markers
# (some files indent DECL markers with tabs, others don't).
# ---------------------------------------------------------------------------


def _segment_cpp(text: str) -> list[tuple]:
    """Split a .cpp into ``[('text', lines), ('func', addr, lines), ...]``.

    Anything outside a balanced ``// !FUNC X BEGIN .. END`` pair goes
    into a ``text`` segment; we *never* invent or drop content from a
    text segment.  An unterminated FUNC block is treated as text so
    we don't accidentally swallow a half-written hand edit.
    """
    segments: list[tuple] = []
    cur_text: list[str] = []
    cur_func_addr: int | None = None
    cur_func_lines: list[str] = []
    for line in text.splitlines():
        if cur_func_addr is None:
            m = FUNC_BEGIN.match(line)
            if m:
                if cur_text:
                    segments.append(("text", cur_text))
                    cur_text = []
                cur_func_addr = int(m.group(1), 16)
                cur_func_lines = [line]
                continue
            cur_text.append(line)
            continue
        cur_func_lines.append(line)
        m_end = FUNC_END.match(line)
        if m_end and int(m_end.group(1), 16) == cur_func_addr:
            segments.append(("func", cur_func_addr, cur_func_lines))
            cur_func_addr = None
            cur_func_lines = []
    if cur_func_addr is not None:
        # Unterminated FUNC block -- fold back into text so nothing
        # disappears under a half-edited file.
        cur_text.extend(cur_func_lines)
    if cur_text:
        segments.append(("text", cur_text))
    return segments


def _segment_header(text: str) -> list[tuple]:
    """Split a .h into ``[('text', lines), ('decl', addr, lines), ...]``.

    Class boundaries are *not* extracted into structured segments --
    they live inside the surrounding ``text`` segment.  When we need to
    insert a brand-new DECL we scan the text segments for the matching
    ``class X { ... };`` window; see ``_insert_new_decl_for_class``.
    """
    segments: list[tuple] = []
    cur_text: list[str] = []
    cur_decl_addr: int | None = None
    cur_decl_lines: list[str] = []
    for line in text.splitlines():
        if cur_decl_addr is None:
            m = DECL_BEGIN.match(line)
            if m:
                if cur_text:
                    segments.append(("text", cur_text))
                    cur_text = []
                cur_decl_addr = int(m.group(1), 16)
                cur_decl_lines = [line]
                continue
            cur_text.append(line)
            continue
        cur_decl_lines.append(line)
        m_end = DECL_END.match(line)
        if m_end and int(m_end.group(1), 16) == cur_decl_addr:
            segments.append(("decl", cur_decl_addr, cur_decl_lines))
            cur_decl_addr = None
            cur_decl_lines = []
    if cur_decl_addr is not None:
        cur_text.extend(cur_decl_lines)
    if cur_text:
        segments.append(("text", cur_text))
    return segments


# ---------------------------------------------------------------------------
# DECL modifier preservation
#
# Ghidra prototypes don't carry virtuality / constness / override etc.,
# but those keywords matter for v-table thunks and const accessors.
# When we regenerate the declaration line from mapping.csv we scan the
# *existing* DECL block for any user-added modifiers and re-insert them
# in the same syntactic position.  This is the *only* exception to
# "marker block contents are auto-managed."
# ---------------------------------------------------------------------------


_ADDR_COMMENT_RE = re.compile(r"^\s*/\*\s*[0-9A-Fa-f]+\s*\*/\s*")


def _decl_payload_line(block_lines: list[str]) -> str | None:
    """Return the actual declaration line from a DECL block (the one
    containing the ``/* ADDR */`` prefix), or ``None`` if none found."""
    for line in block_lines:
        if _ADDR_COMMENT_RE.match(line):
            return line
    return None


def _extract_decl_modifiers(block_lines: list[str]) -> tuple[list[str], list[str]]:
    """Pick out C++ declaration modifiers a user has added inside the
    existing DECL block.  Returns ``(pre_modifiers, post_modifiers)``.

    Pre-modifiers (``virtual`` etc.) appear before the return type;
    post-modifiers (``const`` etc.) appear after the closing ``)``
    before the trailing ``;``.  Order within each bucket follows the
    canonical C++ order, not the order the user typed them in -- this
    keeps idempotence simple (a re-sync produces the exact same line).
    """
    line = _decl_payload_line(block_lines)
    if not line:
        return [], []
    # Drop the ``/* ADDR */`` so keyword matchers don't false-fire on
    # anything inside the comment.
    payload = _ADDR_COMMENT_RE.sub("", line, count=1)
    # Pre-modifiers: tokens before the return type / function name.
    tokens = payload.split()
    pre: list[str] = []
    for tok in tokens:
        if tok in _USER_PRE_MODIFIERS:
            if tok not in pre:
                pre.append(tok)
        elif tok in ("static",):
            # Auto-managed -- skip but keep scanning further tokens.
            continue
        else:
            break
    # Post-modifiers: scan the substring between the closing ``)`` and
    # the terminating ``;``.
    m = re.search(r"\)\s*([^;]*);", payload)
    post: list[str] = []
    if m:
        for tok in m.group(1).split():
            if tok in _USER_POST_MODIFIERS and tok not in post:
                post.append(tok)
    # Sort each bucket into canonical order so the output is stable.
    pre.sort(key=_USER_PRE_MODIFIERS.index)
    post.sort(key=_USER_POST_MODIFIERS.index)
    return pre, post


def _apply_modifiers(decl_line: str,
                     pre_mods: list[str],
                     post_mods: list[str]) -> str:
    """Splice user modifiers into a freshly rendered ``render_decl_line``
    output.  ``decl_line`` has the shape

        \\t/* ADDR */ [static ]<ret-type> <name>(<args>);\\n

    Result with ``pre=['virtual']``, ``post=['const']``:

        \\t/* ADDR */ [static ]virtual <ret-type> <name>(<args>) const;\\n
    """
    if not pre_mods and not post_mods:
        return decl_line
    trailing = "\n" if decl_line.endswith("\n") else ""
    line = decl_line.rstrip("\n")
    m = re.match(r"^(?P<prefix>\s*/\*\s*[0-9A-Fa-f]+\s*\*/\s*)(?P<body>.*?)(?P<semi>;\s*)$", line)
    if not m:
        # Malformed -- bail out without making it worse.
        return decl_line
    prefix, body, semi = m.group("prefix"), m.group("body"), m.group("semi")
    # Step past any auto-emitted ``static`` already at the head so we
    # land the user mods *after* it but *before* the return type.
    body_tokens = body.split(" ")
    cut = 0
    while cut < len(body_tokens) and body_tokens[cut] in ("static",):
        cut += 1
    pre_part = " ".join(body_tokens[:cut])
    rest_part = " ".join(body_tokens[cut:])
    if pre_mods:
        injected = " ".join(pre_mods)
        if pre_part:
            new_body = f"{pre_part} {injected} {rest_part}"
        else:
            new_body = f"{injected} {rest_part}"
    else:
        new_body = body
    # Inject post-mods immediately after the final ``)``.
    if post_mods:
        last_paren = new_body.rfind(")")
        if last_paren >= 0:
            suffix = " " + " ".join(post_mods)
            new_body = new_body[: last_paren + 1] + suffix + new_body[last_paren + 1 :]
    return f"{prefix}{new_body}{semi}{trailing}"


# ---------------------------------------------------------------------------
# Block scanning helpers
# ---------------------------------------------------------------------------


def _iter_blocks(text: str, begin_re: re.Pattern, end_re: re.Pattern):
    """Yield (addr, content_lines) for each properly closed marker pair.
    ``content_lines`` excludes the BEGIN / END marker lines themselves.
    Used only for collecting matched-body content into ``body_cache``;
    in-place rewriting works off the segmenters above.
    """
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


_CLASS_OPEN_RE = re.compile(
    r"^(\s*)class\s+([A-Za-z_]\w*)(\s*:[^\{]*)?\s*\{\s*$"
)


def _find_class_close(lines: list[str], open_idx: int) -> int | None:
    """Return the index of the ``};`` line that closes the class opened
    at ``lines[open_idx]``.  Uses simple brace counting on each line so
    nested inline lambdas / initializers behave sensibly.  Returns
    ``None`` if no matching close exists.
    """
    depth = lines[open_idx].count("{") - lines[open_idx].count("}")
    for j in range(open_idx + 1, len(lines)):
        depth += lines[j].count("{") - lines[j].count("}")
        if depth <= 0 and lines[j].strip() == "};":
            return j
    return None


def _scan_bodies(src_dir: Path) -> dict[int, tuple[str, bool, list[str]]]:
    """Walk every ``src/bulanci/*.cpp`` and record where each function
    address currently lives.  Used to migrate matched bodies between
    units when ``mapping.csv`` changes which namespace a function
    belongs to.

    Returns ``{addr: (current_unit_stem, is_stub, content_lines)}``
    where ``content_lines`` are the lines **inside** the ``// !FUNC``
    markers (markers excluded).
    """
    body_cache: dict[int, tuple[str, bool, list[str]]] = {}
    for cpp in sorted(src_dir.glob("*.cpp")):
        text = cpp.read_text(encoding="utf-8", errors="replace")
        for addr, content in _iter_blocks(text, FUNC_BEGIN, FUNC_END):
            is_stub = STUB_MARKER in "\n".join(content)
            if addr in body_cache:
                print(f"WARN: function 0x{addr:08x} appears in both "
                      f"{body_cache[addr][0]}.cpp and {cpp.stem}.cpp; "
                      f"keeping the latter.")
            body_cache[addr] = (cpp.stem, is_stub, content)
    return body_cache


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
# In-place rewriting
#
# Every existing .cpp / .h is segmented into ``text`` + marker-block
# segments by ``_segment_cpp`` / ``_segment_header``.  We then rebuild
# the file by walking those segments and:
#
#   - ``text`` segments  -> emitted verbatim.
#   - ``func`` / ``decl`` segments whose address is in the unit's new
#     desired set     -> emitted with the marker pair preserved but the
#                        content regenerated (or, for matched bodies,
#                        the previous content kept verbatim).
#   - ``func`` / ``decl`` segments whose address is no longer in the
#     desired set     -> dropped (the function migrated elsewhere; its
#                        matched body will reappear in the winning
#                        file via ``body_cache``).
#
# Addresses that exist in the unit's desired set but appear in *no*
# segment of the file are appended:
#   - cpp: at the end of the file.
#   - .h:  just before the matching ``class X { ... };`` closing brace
#          (with a fresh ``class X { public: ... };`` block synthesized
#          at the end of the file if class X is brand new here).
# ---------------------------------------------------------------------------


def _render_func_block(fun, leaves: set[str],
                       matched_body: list[str] | None = None) -> list[str]:
    """Render a complete ``// !FUNC`` block as a list of lines (no
    trailing newlines).  If ``matched_body`` is given, it's used
    verbatim as the body content; otherwise a fresh stub is emitted.
    """
    block: list[str] = [_func_begin(fun.start).rstrip("\n")]
    if matched_body is not None:
        block.extend(matched_body)
    else:
        block.extend(render_func_body(fun, leaves).rstrip("\n").splitlines())
    block.append(_func_end(fun.start).rstrip("\n"))
    return block


def _render_decl_block(fun, leaves: set[str],
                       existing_block: list[str] | None = None) -> list[str]:
    """Render a complete ``// !DECL`` block as a list of lines (no
    trailing newlines).  ``existing_block``, when provided, is the
    previous block content -- we scan it for user-added modifiers
    (``virtual``, ``const``, ``override``, ...) and replay them on the
    freshly rendered declaration line.
    """
    pre_mods: list[str] = []
    post_mods: list[str] = []
    if existing_block:
        pre_mods, post_mods = _extract_decl_modifiers(existing_block)
    line = render_decl_line(fun, leaves)
    line = _apply_modifiers(line, pre_mods, post_mods).rstrip("\n")
    return [
        _decl_begin(fun.start).rstrip("\n"),
        line,
        _decl_end(fun.start).rstrip("\n"),
    ]


def _flatten_segments(segments: list[tuple]) -> list[str]:
    """Concatenate the ``lines`` lists of every segment into a single
    flat list of lines (each entry is a line *without* trailing
    newline; the caller joins with ``"\\n"`` and adds a trailing
    newline).
    """
    out: list[str] = []
    for seg in segments:
        out.extend(seg[-1] if seg[0] == "text" else seg[2])
    return out


def _rewrite_cpp(text: str,
                 desired: dict[int, "object"],
                 leaves: set[str],
                 body_cache: dict[int, tuple[str, bool, list[str]]],
                 unit_name: str) -> tuple[str, set[int]]:
    """In-place rewrite a .cpp file.  Returns ``(new_text, seen_addrs)``.

    Hand-written content outside ``// !FUNC`` markers is preserved
    verbatim.  Matched-body content is preserved verbatim too; only
    stubs are regenerated.  Functions that migrated to another unit
    are dropped (they reappear in the winning unit via ``body_cache``).
    New addresses are appended at the end of the file.
    """
    segments = _segment_cpp(text)
    new_segments: list[tuple] = []
    seen: set[int] = set()

    for seg in segments:
        if seg[0] == "text":
            new_segments.append(seg)
            continue
        # seg = ("func", addr, [block_lines])
        _, addr, _block_lines = seg
        if addr not in desired:
            # Migrated elsewhere or removed from mapping.csv.  Drop it
            # from this file; the winner reads the cached body.
            continue
        fun = desired[addr]
        cached = body_cache.get(addr)
        if cached and not cached[1]:
            # Matched body: preserve verbatim from wherever it lives.
            body_lines = list(cached[2])
        else:
            body_lines = None  # render a fresh stub
        new_segments.append(("func", addr, _render_func_block(fun, leaves, body_lines)))
        seen.add(addr)

    out_lines = _flatten_segments(new_segments)

    # Append blocks for desired addresses we didn't already place.
    new_addrs = sorted(set(desired) - seen)
    if new_addrs:
        # Make sure there's a blank line between the existing tail and
        # the appended blocks for readability.
        if out_lines and out_lines[-1].strip() != "":
            out_lines.append("")
        for addr in new_addrs:
            fun = desired[addr]
            cached = body_cache.get(addr)
            body_lines = list(cached[2]) if (cached and not cached[1]) else None
            out_lines.extend(_render_func_block(fun, leaves, body_lines))
            out_lines.append("")
            seen.add(addr)

    return "\n".join(out_lines) + "\n", seen


def _class_window(lines: list[str], class_name: str) -> tuple[int, int] | None:
    """Locate ``class <class_name> { ... };`` in ``lines``.  Returns
    ``(open_idx, close_idx)`` (inclusive of both ``{`` and ``};`` lines)
    or ``None`` if not found.  Used to pick the insertion point for a
    brand-new ``// !DECL`` block.
    """
    for i, line in enumerate(lines):
        m = _CLASS_OPEN_RE.match(line)
        if m and m.group(2) == class_name:
            close_idx = _find_class_close(lines, i)
            if close_idx is not None:
                return i, close_idx
    return None


def _rewrite_header(text: str,
                    funs_by_ns: dict[str, list],
                    leaves: set[str]) -> set[int]:
    """In-place rewrite a .h file.  Returns ``(new_text, seen_addrs)``.

    Same model as ``_rewrite_cpp`` but with header-specific insertion
    points: a new DECL goes just before the matching class's ``};``.
    If the class doesn't exist in the file yet (rare; only when a new
    namespace was added to ``units_listing.csv`` for an existing unit)
    a fresh ``class X { public: ... };`` block is appended before the
    closing ``#endif``.
    """
    desired: dict[int, "object"] = {
        fun.start: fun
        for funs in funs_by_ns.values()
        for fun in funs
    }
    segments = _segment_header(text)
    new_segments: list[tuple] = []
    seen: set[int] = set()

    for seg in segments:
        if seg[0] == "text":
            new_segments.append(seg)
            continue
        # seg = ("decl", addr, [block_lines])
        _, addr, block_lines = seg
        if addr not in desired:
            # Migrated to another unit / class.  Drop.
            continue
        fun = desired[addr]
        new_segments.append((
            "decl", addr,
            _render_decl_block(fun, leaves, existing_block=block_lines),
        ))
        seen.add(addr)

    out_lines = _flatten_segments(new_segments)

    # Append new DECLs at the right class boundary.
    new_addrs = sorted(set(desired) - seen)
    if not new_addrs:
        return "\n".join(out_lines) + "\n", seen

    # Group by leaf class so we can find one insertion point per
    # class.  ``fun.namespace`` ends in the leaf class name.
    new_by_class: dict[str, list] = defaultdict(list)
    for addr in new_addrs:
        fun = desired[addr]
        leaf = fun.namespace.split("::")[-1] if fun.namespace else ""
        new_by_class[leaf].append(fun)

    for class_name, funs in new_by_class.items():
        funs.sort(key=lambda f: f.start)
        window = _class_window(out_lines, class_name)
        if window is not None:
            _open_idx, close_idx = window
            # Insert just before the ``};`` line.  If the class body
            # currently has DECLs, drop a blank separator first.
            block: list[str] = []
            has_existing_decl = any(
                DECL_BEGIN.match(out_lines[k])
                for k in range(_open_idx, close_idx)
            )
            for fun in funs:
                block.extend(_render_decl_block(fun, leaves))
                seen.add(fun.start)
            if has_existing_decl and out_lines[close_idx - 1].strip() != "":
                block.insert(0, "")
            out_lines = out_lines[:close_idx] + block + out_lines[close_idx:]
        else:
            # No class block for this leaf yet; synthesize one before
            # ``#endif`` (or at EOF if there's no guard).
            insert_at = len(out_lines)
            for k in range(len(out_lines) - 1, -1, -1):
                if out_lines[k].strip() == "#endif":
                    insert_at = k
                    break
            block = [
                "",
                f"class {class_name} {{",
                "public:",
            ]
            for fun in funs:
                block.extend(_render_decl_block(fun, leaves))
                seen.add(fun.start)
            block.append("};")
            block.append("")
            out_lines = out_lines[:insert_at] + block + out_lines[insert_at:]

    return "\n".join(out_lines) + "\n", seen


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


def sync(
    unit: DecompUnit,
    dry_run: bool = False,
    *,
    agent: str | None = None,
    allow_unclaimed: bool = False,
) -> dict:
    src_dir = WORKSPACE_PATH / unit.srcPath
    include_dir = WORKSPACE_PATH / unit.includePath
    src_dir.mkdir(parents=True, exist_ok=True)
    include_dir.mkdir(parents=True, exist_ok=True)

    body_cache = _scan_bodies(src_dir)
    per_unit_funs, per_unit_ns, addr_to_unit, orphan_ns = plan_expected(unit)

    # Publish the project-wide leaf class set so ``generate_sources``
    # treats cross-unit by-value references (``CBulanci param_3`` in
    # ``CButton.h``) as known types instead of demoting them to ``int``.
    # See ``set_project_classes`` for the rationale.
    project_classes: set[str] = set()
    for namespaces in unit.units.values():
        for ns in namespaces:
            for part in ns.split("::"):
                if part:
                    project_classes.add(part)
    gs.set_project_classes(project_classes)

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

    # Apply the agent-claim filter (no-op when ``agent is None``).
    # ``allow_unclaimed`` defaults to False so an enforcing agent only
    # rewrites units it explicitly claimed; the refresher on main
    # (``agent is None``) sees every unit as before.
    candidate_units = sorted(per_unit_funs)
    permitted_units = guard_units(
        agent,
        candidate_units,
        action="sync_units",
        allow_unclaimed=allow_unclaimed,
    )
    permitted_set = set(permitted_units)
    skipped = [u for u in candidate_units if u not in permitted_set]
    if skipped and agent:
        stats["units_skipped_by_claim"] = len(skipped)
        # Pre-populate ``placed_addrs`` with the addresses for skipped
        # units so they don't trip the "orphaned in src/" warning at
        # the bottom of this function -- those functions are still in
        # mapping.csv, we just chose not to write them this run.
        for unit_name in skipped:
            for fun in per_unit_funs[unit_name]:
                placed_addrs.add(fun.start)

    for unit_name in permitted_units:
        funs = per_unit_funs[unit_name]
        namespaces = per_unit_ns[unit_name]
        funs_by_ns = _funs_by_namespace(funs)
        leaves = _leaf_class_names(namespaces)
        desired_by_addr = {f.start: f for f in funs}

        cpp_path = src_dir / f"{unit_name}.cpp"
        h_path = include_dir / f"{unit_name}.h"

        if not cpp_path.exists() or not h_path.exists():
            # Brand-new unit: ``generate_sources`` scaffolds these.
            # ``sync_units`` only rewrites existing units in-place; we
            # don't try to invent a from-scratch layout here.
            print(f"WARN: {unit_name}.cpp/.h missing; run scripts/configure.py to scaffold.")
            continue

        cpp_text = cpp_path.read_text(encoding="utf-8", errors="replace")
        h_text = h_path.read_text(encoding="utf-8", errors="replace")
        new_cpp, _ = _rewrite_cpp(cpp_text, desired_by_addr, leaves,
                                  body_cache, unit_name)
        new_h, _ = _rewrite_header(h_text, funs_by_ns, leaves)

        stats["units_total"] += 1
        if dry_run:
            cpp_changed = cpp_text != new_cpp
            h_changed = h_text != new_h
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
    parser.add_argument(
        "--agent",
        default=None,
        help=(
            "Restrict writes to units claimed by this agent id "
            "(see scripts/agent_coord.py). Defaults to $BULANCI_AGENT; "
            "leave unset on the main checkout / refresher role for "
            "permissive behaviour."
        ),
    )
    parser.add_argument(
        "--allow-unclaimed",
        action="store_true",
        help=(
            "When --agent is set, also allow writes to units that "
            "have NO live claim (rather than skipping them).  Useful "
            "for a first-time setup or when explicitly bootstrapping."
        ),
    )
    args = parser.parse_args()

    os.chdir(WORKSPACE_PATH)
    unit = DecompUnit("Game code", "bulanci", "bulanci.exe")
    agent = resolve_agent(args.agent)
    if agent:
        print(f"sync_units: enforcing claims for agent={agent}")
    stats = sync(
        unit,
        dry_run=args.dry_run,
        agent=agent,
        allow_unclaimed=args.allow_unclaimed,
    )

    prefix = "[dry-run] " if args.dry_run else ""
    print(f"{prefix}sync_units: {stats['units_total']} unit(s) considered, "
          f"{stats['units_changed']} updated")
    print(f"  + {stats['funcs_added']} new function block(s)")
    print(f"  > {stats['funcs_migrated']} migrated between units "
          f"(of which {stats['funcs_matched_preserved']} were hand-matched and preserved)")
    if stats["funcs_orphaned"]:
        print(f"  ! {stats['funcs_orphaned']} block(s) in src/ no longer in mapping.csv (left untouched)")
    if stats.get("units_skipped_by_claim"):
        print(f"  ~ {stats['units_skipped_by_claim']} unit(s) skipped: not claimed by {agent}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
