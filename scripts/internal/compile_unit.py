#! /usr/bin/env python3
"""ninja wrapper: run `cl /c ...` then demangle the produced .obj's
symbol table so it pairs with ExportDelinker's target object.

Usage:
    python scripts/internal/compile_unit.py --out PATH cl-arg [cl-arg...]

The `--out PATH` flag tells us which file to demangle after cl finishes.
We need it as a separate flag because the cl arg list mixes the .obj
path with other tokens (`/Fo<path>`, `/Fd<path>`) that aren't trivial
to parse here.
"""

import argparse
import subprocess
import sys
from pathlib import Path

THIS = Path(__file__).resolve()
DEMANGLE = THIS.parent / "demangle_obj_symbols.py"


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--out", required=True, type=Path,
                        help="The .obj path cl is going to write (=> demangled after).")
    parser.add_argument("cl_args", nargs=argparse.REMAINDER,
                        help="Arguments forwarded to cl.exe (do NOT prepend --).")
    args = parser.parse_args()

    cl_args = list(args.cl_args)
    if cl_args and cl_args[0] == "--":
        cl_args = cl_args[1:]

    rc = subprocess.call(cl_args)
    if rc != 0:
        return rc
    if not args.out.exists():
        print(f"compile_unit: cl reported success but {args.out} is missing",
              file=sys.stderr)
        return 1
    rc = subprocess.call([sys.executable, str(DEMANGLE), "--quiet", str(args.out)])
    return rc


if __name__ == "__main__":
    sys.exit(main())
