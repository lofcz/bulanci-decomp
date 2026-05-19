"""Walk every `build/orig/bulanci/*.obj` and list all symbol-table
entries, classifying each as:

  defined-text  - defined in a .text section of some .obj (a function
                  body or label; provided by another COFF in our set)
  defined-other - defined in a non-text section of some .obj (rare; we
                  don't emit data sections, so this should be empty)
  external      - never defined in any .obj => an external reference
                  the final linker would have to resolve.  These are
                  the data symbols (DAT_xxx, LAB_xxx, PTR_xxx, ...)
                  that we need to declare as `extern` in our build to
                  produce matching relocations.

Pure dump for inspection.  `scripts/internal/extract_externs.py` uses
the same parsing helpers to actually emit the header.
"""

import argparse
import struct
import sys
from collections import defaultdict
from pathlib import Path


def parse_obj(path: Path):
    data = path.read_bytes()
    n_sections = struct.unpack_from("<H", data, 2)[0]
    sym_tbl_off = struct.unpack_from("<I", data, 8)[0]
    n_syms = struct.unpack_from("<I", data, 12)[0]
    opthdr = struct.unpack_from("<H", data, 16)[0]
    sec_tbl = 20 + opthdr
    section_kinds = []
    for i in range(n_sections):
        s_off = sec_tbl + 40 * i
        name = data[s_off:s_off + 8].rstrip(b"\x00").decode("latin1", "replace")
        section_kinds.append(name)
    strtbl = sym_tbl_off + 18 * n_syms
    out = []
    i = 0
    while i < n_syms:
        off = sym_tbl_off + 18 * i
        if data[off:off + 4] == b"\x00\x00\x00\x00":
            name_off = struct.unpack_from("<I", data, off + 4)[0]
            end = data.index(b"\x00", strtbl + name_off)
            name = data[strtbl + name_off:end].decode("latin1", "replace")
        else:
            name = data[off:off + 8].rstrip(b"\x00").decode("latin1", "replace")
        section_num = struct.unpack_from("<h", data, off + 12)[0]
        storage_cls = data[off + 16]
        n_aux = data[off + 17]
        out.append((name, section_num, storage_cls, section_kinds))
        i += 1 + n_aux
    return out


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--dir", default="build/orig/bulanci",
                        help="Directory of target .obj files.")
    parser.add_argument("--limit", type=int, default=40)
    args = parser.parse_args()
    obj_dir = Path(args.dir)
    defined: dict[str, str] = {}      # symbol -> "text" / "other"
    referenced: dict[str, set] = defaultdict(set)  # symbol -> {obj names}
    for p in sorted(obj_dir.glob("*.obj")):
        for name, sec_num, sc, kinds in parse_obj(p):
            if sec_num > 0 and sec_num <= len(kinds):
                k = kinds[sec_num - 1]
                if k.startswith(".text"):
                    defined.setdefault(name, "text")
                else:
                    defined.setdefault(name, "other")
            elif sec_num == 0 and sc == 2:  # external reference
                referenced[name].add(p.stem)
    externs = {n: srcs for n, srcs in referenced.items() if n not in defined}

    print(f"defined symbols (across all .obj): {len(defined)}")
    print(f"externally referenced symbols     : {len(referenced)}")
    print(f"  of which never defined          : {len(externs)}")

    # Bucket the externs by Ghidra-style prefix so we can decide which
    # ones to declare and which to skip (e.g. _MSVCRT helpers).
    prefixes = defaultdict(list)
    for n in externs:
        if n.startswith("DAT_"):
            prefixes["DAT_"].append(n)
        elif n.startswith("PTR_"):
            prefixes["PTR_"].append(n)
        elif n.startswith("LAB_"):
            prefixes["LAB_"].append(n)
        elif "::vftable" in n:
            prefixes["::vftable"].append(n)
        elif "::" in n:
            prefixes["::other"].append(n)
        elif n.startswith("FUN_"):
            prefixes["FUN_"].append(n)
        else:
            prefixes["misc"].append(n)
    print()
    print("=== externs grouped by prefix ===")
    for k in sorted(prefixes):
        v = prefixes[k]
        print(f"  {k:14s} {len(v)}")
        for n in sorted(v)[:args.limit]:
            print(f"    {n}")
        if len(v) > args.limit:
            print(f"    ... ({len(v) - args.limit} more)")
        print()


if __name__ == "__main__":
    sys.exit(main())
