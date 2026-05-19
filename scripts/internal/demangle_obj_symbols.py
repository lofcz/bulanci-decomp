#! /usr/bin/env python3
"""Rewrite MSVC-mangled symbols in a COFF .obj down to `Class::Method`
form so they pair with the names ExportDelinker writes into the target
.obj.

Background:
  * `tools/msvc8/Bin/cl.exe` emits the standard MSVC mangling for every
    function it compiles - e.g. `?FUN_00467430@CDSApp@@QAEXXZ`.
  * `scripts/ghidra/ExportDelinker.java` writes the demangled qualified
    name into the original-bytes COFF target - `CDSApp::FUN_00467430`.
  * `objdiff` pairs symbols by exact raw-name equality. Its internal
    MSVC demangler runs for *display* but the report.json generator
    does not use it for pairing (encounter/objdiff#279), so the
    project measures stay at 0.00% even when the bytes match.

This script flattens our compiled .obj's symbols to match ExportDelinker
exactly. Run it on every `build/Src/bulanci/*.obj` after `cl /c`.

Symbols are demangled via the Windows `UnDecorateSymbolName` API with
`UNDNAME_NAME_ONLY`, which strips `public:`/`__thiscall`/`(void)`
adornments and yields just `Namespace::Name`.

Idempotent: re-running on an already-rewritten .obj is a no-op (the
names no longer start with `?` so they're left alone).
"""

import argparse
import ctypes
import re
import struct
import sys
from ctypes import wintypes
from pathlib import Path

dbghelp = ctypes.WinDLL("dbghelp")
UnDecorateSymbolName = dbghelp.UnDecorateSymbolName
UnDecorateSymbolName.argtypes = [wintypes.LPCSTR, ctypes.c_char_p,
                                 wintypes.DWORD, wintypes.DWORD]
UnDecorateSymbolName.restype = wintypes.DWORD

UNDNAME_NAME_ONLY = 0x1000  # strip type/decoration info entirely

# Ghidra-auto data-symbol patterns produced by `extract_externs.py`.
# cl.exe prefixes every C-linkage data symbol with `_` in 32-bit mode;
# the matching ExportDelinker symbol has no prefix.  Whitelist the
# patterns we own so we don't accidentally rename real MSVCRT helpers
# like `_main`, `_atexit`, `__except_handler4`, etc.
DATA_UNDERSCORE_PATTERNS = re.compile(
    r"^_("
    r"DAT_[0-9a-fA-F]{8}|"
    r"LAB_[0-9a-fA-F]{8}|"
    r"PTR_.+_[0-9a-fA-F]{8}|"
    r"s_.+_[0-9a-fA-F]{8}|"
    r"IMAGE_DOS_HEADER_[0-9a-fA-F]{8}|"
    r"switchD_[0-9a-fA-F]{8}|"
    r"switchdataD_[0-9a-fA-F]{8}|"
    r"thunk_FUN_[0-9a-fA-F]{8}"
    r")$"
)


def demangle(name: str) -> str:
    if name.startswith("?"):
        # MSVC-mangled C++ symbol - run through UnDecorateSymbolName
        # with NAME_ONLY so we land on `CClass::Method` (matches
        # ExportDelinker's output exactly).
        buf = ctypes.create_string_buffer(4096)
        n = UnDecorateSymbolName(name.encode("latin1"), buf, 4096,
                                 UNDNAME_NAME_ONLY)
        if n == 0:
            return name
        return buf.value.decode("latin1")
    if DATA_UNDERSCORE_PATTERNS.match(name):
        # cl.exe's C-linkage underscore for one of our extern data
        # symbols.  Strip it so the COFF symbol pairs against the
        # target object that uses the bare Ghidra name.
        return name[1:]
    return name


def rewrite_obj(path: Path) -> tuple[int, int]:
    """Returns (changed_count, total_symbols)."""
    data = bytearray(path.read_bytes())
    sym_tbl_off = struct.unpack_from("<I", data, 8)[0]
    n_syms = struct.unpack_from("<I", data, 12)[0]
    if sym_tbl_off == 0 or n_syms == 0:
        return 0, 0

    strtbl_off = sym_tbl_off + 18 * n_syms
    strtbl_size = struct.unpack_from("<I", data, strtbl_off)[0]
    # Snapshot the existing string table; we append new strings to a
    # fresh tail buffer so existing offsets stay valid.
    new_strings = bytearray()

    changed = 0
    i = 0
    while i < n_syms:
        rec_off = sym_tbl_off + 18 * i
        zero = data[rec_off:rec_off + 4]
        if zero == b"\x00\x00\x00\x00":
            name_off = struct.unpack_from("<I", data, rec_off + 4)[0]
            end = data.index(b"\x00", strtbl_off + name_off)
            name = data[strtbl_off + name_off:end].decode("latin1", "replace")
            inline = False
        else:
            raw = bytes(data[rec_off:rec_off + 8])
            name = raw.rstrip(b"\x00").decode("latin1", "replace")
            inline = True

        new_name = demangle(name)
        n_aux = data[rec_off + 17]

        if new_name != name:
            changed += 1
            encoded = new_name.encode("latin1") + b"\x00"
            # Append to the new-strings tail and reference the resulting
            # offset.  We never reuse the inline 8-byte slot because the
            # rewritten name is almost always longer than the original.
            tail_off = strtbl_size + len(new_strings)
            new_strings += encoded
            data[rec_off:rec_off + 4] = b"\x00\x00\x00\x00"
            struct.pack_into("<I", data, rec_off + 4, tail_off)

        i += 1 + n_aux

    if new_strings:
        # Splice the appended strings in *before* whatever follows the
        # string table.  For COFF .obj files nothing follows: the string
        # table is the very last region.  We still locate end-of-file
        # so we don't assume.
        # Update strtbl size field.
        new_size = strtbl_size + len(new_strings)
        struct.pack_into("<I", data, strtbl_off, new_size)
        # Append at end of file (= end of string table).
        data.extend(new_strings)
        path.write_bytes(data)
    return changed, n_syms


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("paths", nargs="+", type=Path,
                        help="One or more .obj files to rewrite in place.")
    parser.add_argument("--quiet", action="store_true",
                        help="Suppress per-file logging.")
    args = parser.parse_args()
    total_changed = 0
    total_syms = 0
    for p in args.paths:
        c, n = rewrite_obj(p)
        total_changed += c
        total_syms += n
        if not args.quiet:
            print(f"  {p.name}: {c}/{n} symbols rewritten")
    print(f"demangle_obj_symbols: {total_changed} symbol(s) rewritten "
          f"across {len(args.paths)} object(s)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
