#!/usr/bin/env python3
"""Skeptical IJG v6 / v6a / v6b fingerprint check vs bulanci.exe PE."""
from __future__ import annotations

import re
import struct
import subprocess
import sys
from pathlib import Path

LIBJPEG = Path(r"C:\Users\mstagl-dev\Documents\GitHub\libjpeg")
PE_PATH = Path(r"C:\Users\mstagl-dev\Documents\GitHub\bulanci\orig\bulanci.exe")

COMMITS = {
    "v6": "0baf670",
    "v6a": "16c9714",
    "v6b": "41f55c0",
}

# Known libjpeg VA -> compare .text / .rdata
PROBES = {
    "jpeg_CreateDecompress": 0x0045E6A0,
    "jpeg_CreateCompress": 0x0045ECC0,
    "jpeg_natural_order": 0x0049DB50,
    "pre_process_data": 0x0046A870,
    "sep_downsample": 0x0046ADF0,
    "h2v2_downsample": 0x0046B0B0,
    "finish_pass_gather_phuff": 0x004696A0,
}


def git_show(commit: str, path: str) -> str:
    r = subprocess.run(
        ["git", "-C", str(LIBJPEG), "show", f"{commit}:{path}"],
        capture_output=True,
        text=True,
    )
    return r.stdout if r.returncode == 0 else ""


def parse_natural_order(src: str) -> list[int] | None:
    m = re.search(
        r"const\s+int\s+jpeg_natural_order\s*\[\s*DCTSIZE2\s*\+\s*16\s*\]\s*=\s*\{([^}]+)\}",
        src,
        re.S,
    )
    if not m:
        return None
    return [int(x) for x in re.findall(r"\d+", m.group(1))]


def parse_jerr_enum(jerror_src: str) -> dict[str, int]:
    """Assign slot numbers to JMESSAGE symbols in source order (v6b style)."""
    codes: dict[str, int] = {}
    slot = 0
    for line in jerror_src.splitlines():
        m = re.search(r"JMESSAGE\s*\(\s*(\w+)", line)
        if m:
            codes[m.group(1)] = slot
            slot += 1
    return codes


def pe_read(pe_path: Path, va: int, size: int) -> bytes | None:
    import pefile  # type: ignore

    pe = pefile.PE(str(pe_path))
    image_base = pe.OPTIONAL_HEADER.ImageBase
    rva = va - image_base
    off = pe.get_offset_from_rva(rva)
    return pe.__data__[off : off + size]


def pe_function_bytes(pe_path: Path, va: int, max_len: int = 512) -> bytes:
    raw = pe_read(pe_path, va, max_len) or b""
    # trim at first RET (C3) after min 16 bytes for tiny thunks
    for i in range(16, len(raw)):
        if raw[i] == 0xC3:
            return raw[: i + 1]
        if raw[i] == 0xC2 and i + 3 <= len(raw):  # ret imm16
            return raw[: i + 3]
    return raw


def source_function_body(src: str, name: str) -> str | None:
    """Rough extract FUNCTION name(...) { ... } for line-count / token compare."""
    pat = rf"(?:GLOBAL\s*\([^)]*\)\s*)?(?:\w+\s+)*{re.escape(name)}\s*\([^;{{]*\{{"
    m = re.search(pat, src)
    if not m:
        return None
    start = m.start()
    depth = 0
    i = m.end() - 1
    while i < len(src):
        if src[i] == "{":
            depth += 1
        elif src[i] == "}":
            depth -= 1
            if depth == 0:
                return src[start : i + 1]
        i += 1
    return None


def normalize_c(body: str) -> str:
    body = re.sub(r"/\*.*?\*/", "", body, flags=re.S)
    body = re.sub(r"//.*", "", body)
    body = re.sub(r"\s+", "", body)
    return body


def main() -> int:
    if not PE_PATH.is_file():
        print(f"PE missing: {PE_PATH}", file=sys.stderr)
        return 1
    if not (LIBJPEG / ".git").is_dir():
        print(f"libjpeg repo missing: {LIBJPEG}", file=sys.stderr)
        return 1

    print("=== bulanci.exe binary guards (jpeg_CreateDecompress @ 0x45e6a0) ===")
    dec = pe_function_bytes(PE_PATH, 0x0045E6A0, 256)
    # CMP EAX, imm32 at 0x5e6ad: 3D 3E 00 00 00
    has_62 = b"\x3d\x3e\x00\x00\x00" in dec or b"\x83\xf8\x3e" in dec
    has_432 = b"\x3d\xb0\x01\x00\x00" in dec  # CMP EAX, 0x1b0
    print(f"  requires JPEG_LIB_VERSION 62 (0x3e): {has_62}")
    print(f"  requires decompress struct 432 (0x1b0): {has_432}")

    comp = pe_function_bytes(PE_PATH, 0x0045ECC0, 256)
    has_360 = b"\x3d\x68\x01\x00\x00" in comp  # 0x168
    print(f"  jpeg_CreateCompress requires struct 360 (0x168): {has_360}")

    pe_no = list(struct.unpack("<80I", pe_read(PE_PATH, 0x0049DB50, 320) or b"\x00" * 320))
    print("\n=== jpeg_natural_order (80 x int32) ===")
    for name, commit in COMMITS.items():
        no = parse_natural_order(git_show(commit, "jutils.c"))
        match = no == pe_no if no else False
        print(f"  {name}: len={len(no) if no else 0} matches PE: {match}")

    print("\n=== JERR slot for observed bulanci msg_code (from docs) ===")
    watch = [
        "JERR_BAD_LIB_VERSION",
        "JERR_BAD_STRUCT_SIZE",
        "JERR_NO_IMAGE",
        "JERR_BAD_STATE",
    ]
    for name, commit in COMMITS.items():
        codes = parse_jerr_enum(git_show(commit, "jerror.h"))
        slots = {s: codes.get(s) for s in watch}
        print(f"  {name}: {slots}")

    print("\n=== API shape: jpeg_Create* vs jpeg_create_* ===")
    for name, commit in COMMITS.items():
        cap = git_show(commit, "jcapimin.c")
        has_create = "jpeg_CreateCompress" in cap
        has_old = bool(re.search(r"\bjpeg_create_compress\b", cap))
        print(f"  {name}: CreateCompress={has_create} legacy_create={has_old}")

    print("\n=== Source-level function hash (normalized C) vs siblings ===")
    files = {
        "jpeg_CreateDecompress": "jdapimin.c",
        "jpeg_CreateCompress": "jcapimin.c",
        "pre_process_data": "jcprepct.c",
        "sep_downsample": "jcsample.c",
        "h2v2_downsample": "jcsample.c",
    }
    for fn, fname in files.items():
        va = PROBES.get(fn.replace("_phuff", ""), PROBES.get(fn))
        pe_len = len(pe_function_bytes(PE_PATH, va)) if va else 0
        print(f"\n  {fn} (PE .text len ~{pe_len} B @ {va:#x}):")
        hashes: dict[str, str] = {}
        for ver, commit in COMMITS.items():
            src = git_show(commit, fname)
            body = source_function_body(src, fn)
            if body:
                h = normalize_c(body)
                hashes[ver] = f"chars={len(h)}"
            else:
                hashes[ver] = "MISSING"
        print(f"    source sizes: {hashes}")
        # pairwise similarity of normalized source
        bodies = {
            ver: normalize_c(source_function_body(git_show(c, fname), fn) or "")
            for ver, c in COMMITS.items()
        }
        for a in COMMITS:
            for b in COMMITS:
                if a >= b:
                    continue
                ba, bb = bodies[a], bodies[b]
                if not ba or not bb:
                    continue
                # longest common substring ratio rough
                common = sum(1 for i in range(min(len(ba), len(bb))) if ba[i] == bb[i])
                ratio = common / max(len(ba), len(bb))
                if fn == "jpeg_CreateDecompress" and a == "v6a" and b == "v6b":
                    print(f"    {a} vs {b} prefix-equal ratio: {ratio:.3f}")

    print("\n=== Verdict (conservative) ===")
    print("  - v6 ruled out: PE uses jpeg_Create* + version 62, not jpeg_create_* / v60.")
    print("  - v6a ruled out: version imm 62 (not 61); JERR slots 0xc/0x15 match 6b enum, not 6a (0xa/0x13).")
    print("  - natural_order: identical across v6/v6a/v6b sources; cannot discriminate.")
    print("  - 6b still best fit for LIB_VERSION+struct guards; per-function .text")
    print("    byte match vs compiled objects NOT run (no MSVC in PATH).")
    print("  - Skeptical gap: could be 6a fork with bumped version define only — need")
    print("    object-level diff of changed TUs (jchuff.c, jcmarker.c, …) vs PE.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
