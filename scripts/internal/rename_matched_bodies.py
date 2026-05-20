#! /usr/bin/env python3
"""Rewrite the function definition line of every matched body in
`src/bulanci/*.cpp` so the C++ name matches what Ghidra now claims.

`sync_units.py` preserves matched bodies verbatim, which is the right
default - but when Ghidra renames a function (or moves it to a
different class/namespace), the .h declaration is regenerated while
the .cpp definition keeps the old qualified name.  MSVC then errors
out with "is not a member of 'ClassName'".

This script:
  1. Loads `mapping.csv` to get addr -> (return_type, qualified_name,
     args) for every function.
  2. For each `// !FUNC 0xXXXX BEGIN` block:
       a. Skip stubs (they're regenerated anyway).
       b. Find the first line that looks like a definition and rewrite
          only the qualified name portion, keeping the body intact.

Safe to run repeatedly: when the line already matches, nothing happens.
"""

import re
import sys
from pathlib import Path

WORKSPACE = Path(__file__).parent.parent.parent
sys.path.insert(0, str(WORKSPACE / "scripts"))

from project import DecompUnit  # noqa: E402
from generate_sources import _sanitize_symbol_name  # noqa: E402

FUNC_BEGIN = re.compile(r"^// !FUNC (0x[0-9a-fA-F]+) BEGIN\s*$")
FUNC_END = re.compile(r"^// !FUNC (0x[0-9a-fA-F]+) END\s*$")
STUB_MARKER = "STUB_BODY("

# Match the qualified name in a function definition. The leading portion may
# include `__declspec(...)` attributes, return type qualifiers (const, etc.),
# pointer/reference markers, and class qualifiers.  We grab everything from
# start-of-line up to and including the last whitespace before the qualified
# name (the function name).  Capture group `qual` is the qualified function
# name itself; we replace it.
QUAL_NAME_RE = re.compile(
    r"^(?P<lead>(?:__declspec\s*\([^)]*\)\s+)?[A-Za-z_][\w:\s\*&]*?\s)"
    r"(?P<qual>[A-Za-z_][\w]*(?:::[A-Za-z_][\w]*)*)"
    r"(?P<rest>\s*\()"
)

# Old-style `FUN_HHHHHHHH` placeholder name; we use this to rewrite call
# sites inside matched bodies once we know the new qualified name.
FUN_PLACEHOLDER_RE = re.compile(r"\bFUN_([0-9a-fA-F]{8})\b")


def build_addr_map(unit: DecompUnit) -> tuple[dict[int, str], dict[int, str]]:
    """Return (addr -> qualified_name, addr -> leaf_name).

    `qualified_name` is `Namespace::Name` for class members (used for the
    definition line) while `leaf_name` is the unqualified function name
    (used to rewrite call sites where the original code had a bare
    `FUN_HHHHHHHH` placeholder)."""
    qual_map: dict[int, str] = {}
    leaf_map: dict[int, str] = {}
    for ns, funs in unit.mappings.items():
        for fun in funs:
            # Apply the same sanitization the header emitter uses so the
            # signature line in the .cpp lines up with the .h declaration.
            name = _sanitize_symbol_name(fun.name)
            # `_Globals` IS a real class in the generated header (just like
            # any other unit) — the only namespace we treat as a true
            # global is the top-level `Global` one.
            if ns and ns != "Global":
                qual_map[fun.start] = f"{ns}::{name}"
            else:
                qual_map[fun.start] = name
            leaf_map[fun.start] = name
    return qual_map, leaf_map


def rewrite_block(
    addr: int,
    lines: list[str],
    qualified_name: str,
    leaf_map: dict[int, str],
) -> list[str]:
    """Rewrite the first line that has `Type Some::Name(args)` to use
    `qualified_name` for the function name part, plus any `FUN_HHHHHHHH`
    call site inside the body to use the new leaf name.  Mutates a
    copy.
    """
    new_lines = list(lines)

    # 1) Rewrite the signature line.
    for i, line in enumerate(new_lines):
        m = QUAL_NAME_RE.match(line)
        if not m:
            continue
        cur_qual = m.group("qual")
        if cur_qual != qualified_name:
            new_lines[i] = (
                f"{m.group('lead')}{qualified_name}{m.group('rest')}{line[m.end():]}"
            )
        break

    # 2) Rewrite call sites that still use the `FUN_HHHHHHHH` placeholder.
    def _sub(m: re.Match) -> str:
        ref_addr = int(m.group(1), 16)
        new_leaf = leaf_map.get(ref_addr)
        if not new_leaf or new_leaf.startswith("FUN_"):
            return m.group(0)
        return new_leaf

    for i, line in enumerate(new_lines):
        new_line = FUN_PLACEHOLDER_RE.sub(_sub, line)
        if new_line != line:
            new_lines[i] = new_line

    return new_lines


def process_file(
    cpp: Path,
    qual_map: dict[int, str],
    leaf_map: dict[int, str],
) -> bool:
    text = cpp.read_text(encoding="utf-8", errors="replace")
    out_lines: list[str] = []
    in_block = False
    cur_addr: int | None = None
    cur_buf: list[str] = []
    changed = False

    for line in text.splitlines():
        m_b = FUNC_BEGIN.match(line)
        if m_b:
            out_lines.append(line)
            in_block = True
            cur_addr = int(m_b.group(1), 16)
            cur_buf = []
            continue
        m_e = FUNC_END.match(line)
        if m_e and in_block:
            in_block = False
            block_text = "\n".join(cur_buf)
            if STUB_MARKER not in block_text:
                qualified = qual_map.get(cur_addr)
                if qualified:
                    new_buf = rewrite_block(cur_addr, cur_buf, qualified, leaf_map)
                    if new_buf != cur_buf:
                        changed = True
                        cur_buf = new_buf
            out_lines.extend(cur_buf)
            out_lines.append(line)
            cur_addr = None
            cur_buf = []
            continue
        if in_block:
            cur_buf.append(line)
        else:
            out_lines.append(line)

    if changed:
        cpp.write_text("\n".join(out_lines) + "\n", encoding="utf-8", newline="")
    return changed


def main():
    unit = DecompUnit("Game code", "bulanci", "bulanci.exe")
    qual_map, leaf_map = build_addr_map(unit)
    src_dir = WORKSPACE / "src" / "bulanci"
    total_changed = 0
    for cpp in sorted(src_dir.glob("*.cpp")):
        if process_file(cpp, qual_map, leaf_map):
            total_changed += 1
            print(f"  updated {cpp.name}")
    print(f"rename_matched_bodies: {total_changed} file(s) updated")


if __name__ == "__main__":
    main()
