#! /usr/bin/env python3
"""Compile jpeg_CreateDecompress slice into build/Src/bulanci/_Globals.obj."""

from __future__ import annotations

import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SLICE = ROOT / "src" / "bulanci" / "_Globals_jpeg_slice.cpp"
OUT = ROOT / "build" / "Src" / "bulanci" / "_Globals.obj"
COMPILE_UNIT = ROOT / "scripts" / "internal" / "compile_unit.py"
CL = os.environ.get("BULANCI_CL", str(ROOT / "tools" / "msvc8" / "Bin" / "cl.exe"))


def main() -> int:
    msvc_inc = Path(CL).resolve().parent.parent / "Include"
    psdk_inc = Path(CL).resolve().parent.parent / "PlatformSDK" / "Include"
    flags = [
        "/Zi",
        "/O2",
        "/Oy",
        "/Oi-",
        "/GR",
        "/EHsc",
        "/I",
        str(ROOT / "include"),
        "/I",
        str(ROOT / "include" / "bulanci"),
        "/I",
        str(msvc_inc),
        "/I",
        str(ROOT / "tools"),
    ]
    if psdk_inc.exists():
        flags.extend(["/I", str(psdk_inc)])

    OUT.parent.mkdir(parents=True, exist_ok=True)
    cmd = [
        sys.executable,
        str(COMPILE_UNIT),
        "--out",
        str(OUT),
        "--",
        CL,
        "/nologo",
        *flags,
        "/c",
        str(SLICE),
        f"/Fd{OUT.with_suffix('.obj.pdb')}",
        f"/Fo{OUT}",
    ]
    print(" ".join(cmd))
    return subprocess.call(cmd)


if __name__ == "__main__":
    sys.exit(main())
