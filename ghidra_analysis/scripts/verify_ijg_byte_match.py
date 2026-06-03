#!/usr/bin/env python3
"""Byte-match bulanci.exe IJG functions against MSVC8 builds of libjpeg v6/v6a/v6b.

Uses tools/msvc8 (VC8 14.00.50727.42) and kornelski/libjpeg commits.
"""
from __future__ import annotations

import re
import struct
import subprocess
import sys
import tarfile
import io
from pathlib import Path

BULANCI = Path(__file__).resolve().parents[2]
MSVC = BULANCI / "tools" / "msvc8"
LIBJPEG = Path(r"C:\Users\mstagl-dev\Documents\GitHub\libjpeg")
PE_PATH = BULANCI / "orig" / "bulanci.exe"
WORK = BULANCI / "ghidra_analysis" / "build" / "ijg_verify"

COMMITS = {
    "v6": "0baf670330cc17ab3d59bd5a4d3ae675d1c05026",
    "v6a": "16c97143f3db4360230bcdc4d7f804d685b3c953",
    "v6b": "41f55c093a551ff5df0431155de978618862bbc9",
}

# Ghidra VA -> (source file, COFF symbol stem without leading _)
# Stems verified against cl /O2 /c per-file objects.
PROBES: list[tuple[str, int, str, str]] = [
    ("jpeg_CreateDecompress", 0x0045E6A0, "jdapimin.c", "_jpeg_CreateDecompress"),
    ("jpeg_CreateCompress", 0x0045ECC0, "jcapimin.c", "_jpeg_CreateCompress"),
    ("pre_process_data", 0x0046A870, "jcprepct.c", "_pre_process_data"),
    ("sep_downsample", 0x0046ADF0, "jcsample.c", "_sep_downsample"),
    ("h2v1_downsample", 0x0046B000, "jcsample.c", "_h2v1_downsample"),
    ("h2v2_downsample", 0x0046B0B0, "jcsample.c", "_h2v2_downsample"),
    ("int_downsample", 0x0046AE80, "jcsample.c", "_int_downsample"),
    ("encode_one_block_baseline", 0x00468200, "jchuff.c", "_encode_one_block"),
    ("finish_pass_gather_phuff", 0x004696A0, "jchuff.c", "_finish_pass_gather"),
    ("jinit_phuff_encoder", 0x00469900, "jcphuff.c", "_jinit_phuff_encoder"),
    ("expand_bottom_edge_homolog", 0x0046A840, "jcprepct.c", "_expand_bottom_edge"),
]

# Files whose .text differs between v6a and v6b (from git diff stat).
DISCRIMINATING_SOURCES = {"jchuff.c", "jcmarker.c", "jdmarker.c", "jcapimin.c", "jdapimin.c"}


def env_msvc() -> dict[str, str]:
    import os

    e = os.environ.copy()
    e["PATH"] = f"{MSVC / 'Bin'};{e.get('PATH', '')}"
    e["INCLUDE"] = f"{MSVC / 'Include'};{MSVC / 'PlatformSDK' / 'Include'}"
    e["LIB"] = f"{MSVC / 'Lib'};{MSVC / 'PlatformSDK' / 'Lib'}"
    return e


def archive_commit(commit: str, dest: Path) -> None:
    dest.mkdir(parents=True, exist_ok=True)
    proc = subprocess.run(
        ["git", "-C", str(LIBJPEG), "archive", commit],
        capture_output=True,
        check=True,
    )
    with tarfile.open(fileobj=io.BytesIO(proc.stdout), mode="r:*") as tar:
        tar.extractall(dest)


def compile_file(src_dir: Path, c_file: str) -> Path:
    obj = src_dir / (Path(c_file).stem + ".obj")
    if obj.exists() and obj.stat().st_mtime > (src_dir / c_file).stat().st_mtime:
        return obj
    jcfg = src_dir / "jconfig.vc"
    if not jcfg.is_file():
        jcfg = WORK / "v6b" / "jconfig.vc"
    (src_dir / "jconfig.h").write_bytes(jcfg.read_bytes())
    cl = MSVC / "Bin" / "cl.exe"
    cmd = [
        str(cl),
        "/nologo",
        "/c",
        "/O2",
        "/I.",
        c_file,
        f"/Fo{obj.name}",
    ]
    subprocess.run(cmd, cwd=src_dir, env=env_msvc(), check=True, capture_output=True)
    return obj


def pe_function_bytes(pe_path: Path, va: int) -> bytes:
    import pefile  # type: ignore

    pe = pefile.PE(str(pe_path))
    base = pe.OPTIONAL_HEADER.ImageBase
    off = pe.get_offset_from_rva(va - base)
    raw = pe.__data__[off : off + 4096]
    for i in range(16, len(raw)):
        if raw[i] == 0xC3:
            return raw[: i + 1]
        if raw[i] == 0xC2 and i + 3 <= len(raw):
            return raw[: i + 3]
    return raw[:512]


def coff_text_symbols(obj_path: Path) -> dict[str, bytes]:
    sys.path.insert(0, str(BULANCI / "scripts" / "internal"))
    from near_miss import coff_function_bytes

    return coff_function_bytes(obj_path)


def byte_match_report(pe: bytes, obj: bytes) -> tuple[str, int, float]:
    """Return bucket, first_diff_offset, equal_prefix_ratio."""
    if pe == obj:
        return "EXACT", -1, 1.0
    n = min(len(pe), len(obj))
    prefix = 0
    for i in range(n):
        if pe[i] != obj[i]:
            prefix = i
            break
    else:
        prefix = n
    ratio = prefix / max(len(pe), len(obj), 1)
    if len(pe) == len(obj) and ratio >= 0.9:
        return "CLOSE", prefix, ratio
    if abs(len(pe) - len(obj)) <= 4 and ratio >= 0.85:
        return "NEAR", prefix, ratio
    return "OFF", prefix, ratio


def main() -> int:
    if not PE_PATH.is_file():
        print(f"Missing PE: {PE_PATH}", file=sys.stderr)
        return 1
    if not (MSVC / "Bin" / "cl.exe").is_file():
        print(f"Missing MSVC8: {MSVC / 'Bin' / 'cl.exe'}", file=sys.stderr)
        return 1
    if not LIBJPEG.is_dir():
        print(f"Missing libjpeg repo: {LIBJPEG}", file=sys.stderr)
        return 1

    WORK.mkdir(parents=True, exist_ok=True)

    print("=== Setup: extract + compile IJG sources (MSVC8 /O2) ===\n")
    objs: dict[str, dict[str, Path]] = {v: {} for v in COMMITS}
    for ver, commit in [("v6b", COMMITS["v6b"]), ("v6a", COMMITS["v6a"]), ("v6", COMMITS["v6"])]:
        src_dir = WORK / ver
        print(f"  {ver} ({commit[:7]}) ...")
        archive_commit(commit, src_dir)
        needed = {p[2] for p in PROBES}
        for c in sorted(needed):
            compile_file(src_dir, c)
            objs[ver][c] = src_dir / f"{Path(c).stem}.obj"

    print("\n=== Per-function byte match (PE vs compiled .obj) ===\n")
    print(f"{'function':<28} {'PE':>4}  " + "  ".join(f"{v:>12}" for v in COMMITS))
    print("-" * 72)

    scores = {v: {"EXACT": 0, "CLOSE": 0, "NEAR": 0, "OFF": 0, "MISS": 0} for v in COMMITS}

    for label, va, c_file, sym in PROBES:
        pe_b = pe_function_bytes(PE_PATH, va)
        row = [f"{label:<28} {len(pe_b):>4} "]
        for ver in COMMITS:
            obj_path = objs[ver][c_file]
            syms = coff_text_symbols(obj_path)
            if sym not in syms:
                # static METHODDEF may mangle differently — fuzzy search
                alt = [k for k in syms if sym.strip("_") in k]
                if len(alt) == 1:
                    ob = syms[alt[0]]
                else:
                    scores[ver]["MISS"] += 1
                    row.append(f"{'MISS':>12}")
                    continue
            else:
                ob = syms[sym]
            bucket, _, ratio = byte_match_report(pe_b, ob)
            scores[ver][bucket] += 1
            row.append(f"{bucket}@{ratio:.0%}({len(ob)})".rjust(12))
        print("".join(row))

    print("\n=== Aggregate ===\n")
    for ver in COMMITS:
        s = scores[ver]
        total = sum(s.values())
        print(
            f"  {ver}: EXACT={s['EXACT']} CLOSE={s['CLOSE']} NEAR={s['NEAR']} "
            f"OFF={s['OFF']} MISS={s['MISS']}  (of {total})"
        )

    # Natural order rodata
    print("\n=== jpeg_natural_order .rdata (identical all 3 sources) ===")
    pe_no = pe_function_bytes(PE_PATH, 0x0049DB50)  # wrong — read fixed 320
    import pefile

    pe = pefile.PE(str(PE_PATH))
    off = pe.get_offset_from_rva(0x0049DB50 - pe.OPTIONAL_HEADER.ImageBase)
    pe_no = pe.__data__[off : off + 320]

    for ver, commit in [("v6b", COMMITS["v6b"]), ("v6a", COMMITS["v6a"]), ("v6", COMMITS["v6"])]:
        ju = (WORK / ver / "jutils.c").read_text(encoding="utf-8", errors="replace")
        m = re.search(
            r"const\s+int\s+jpeg_natural_order\s*\[\s*DCTSIZE2\s*\+\s*16\s*\]\s*=\s*\{([^}]+)\}",
            ju,
            re.S,
        )
        nums = [int(x) for x in re.findall(r"\d+", m.group(1))] if m else []
        blob = struct.pack("<" + "I" * len(nums), *nums)
        print(f"  {ver}: {'MATCH' if blob == pe_no else 'DIFF'} ({len(nums)} ints)")

    print("\n=== Interpretation ===")
    best = max(COMMITS, key=lambda v: scores[v]["EXACT"] + 0.5 * scores[v]["CLOSE"])
    print(
        f"  Best aggregate object match: **{best}** "
        f"(EXACT+CLOSE weighted; PE is SleepTeam-linked VC8, not necessarily identical /O2 to nmake defaults)."
    )
    print(
        "  v6 uses jpeg_create_* API — expect universal MISS/OFF on Create* even if sources compile."
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
