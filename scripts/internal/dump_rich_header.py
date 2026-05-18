"""Parse the PE Rich Header of a Windows binary.

The Rich Header is an undocumented blob Microsoft toolchains splice between
the DOS stub and the PE header. Each record is `(comp_id, count)` where the
high 16 bits of `comp_id` is `mCV` (build/version tag) and the low 16 bits is
a product type. The set of tuples (and especially the highest linker tag)
is the most reliable way to pin down the original MSVC build.

Usage:
    python scripts/internal/dump_rich_header.py orig/bulanci.exe
"""
from __future__ import annotations

import struct
import sys
from pathlib import Path

# Subset of well-known VC8 / VS2005 @comp.id product IDs. The full table is
# version-specific; we keep it minimal because the OptionalHeader's
# MajorLinkerVersion is a stronger fingerprint anyway.
PRODUCT_NAMES: dict[int, str] = {
    0x0001: "Import (linker-tagged)",
    0x005C: "Masm800 (VS2005 assembler)",
    0x005D: "Cvtres800 (VS2005 resource converter)",
    0x005F: "Utc1310_C (VS2003 compiler)",
    0x0060: "Utc1310_CPP (VS2003 compiler)",
    0x006D: "Cvtomf800 / VC8 object",
    0x006E: "Linker800 (VS2005 linker)",
    0x0078: "Utc1400_C (VS2005 compiler)",
    0x007C: "Utc1400_CPP (VS2005 compiler)",
    0x007D: "Linker800 / VC8 LTCG",
    0x0083: "Cvtres900 (VS2008)",
    0x0084: "Linker900 (VS2008)",
    0x0085: "Masm900 (VS2008)",
    0x009A: "Utc1900_C (VS2010)",
    0x00CF: "Utc1700_C (VS2012)",
    0x00D9: "Linker1100 (VS2012)",
}

# OptionalHeader.MajorLinkerVersion -> VS marketing name. The PE-level linker
# version is the strongest single fingerprint and does not require keeping a
# fragile @comp.id build table in sync.
LINKER_MAJOR: dict[int, str] = {
    5: "Visual Studio 97 (VC5)",
    6: "Visual Studio 6.0 (VC6)",
    7: "Visual Studio .NET 2002/2003 (VC7/VC7.1)",
    8: "Visual Studio 2005 (VC8)",
    9: "Visual Studio 2008 (VC9)",
    10: "Visual Studio 2010 (VC10)",
    11: "Visual Studio 2012 (VC11)",
    12: "Visual Studio 2013 (VC12)",
    14: "Visual Studio 2015/2017/2019/2022 (VC14.x)",
}

# Linker-self build numbers that are well-publicised; useful as a secondary
# signal alongside MajorLinkerVersion.
LINKER_BUILD_NOTES: dict[int, str] = {
    50327: "8.00.50327 (VS2005 beta)",
    50727: "8.00.50727 (VS2005 RTM) OR 11.00.50727 (VS2012 RTM)",
    40219: "10.00.40219 (VS2010 SP1)",
    30319: "10.00.30319 (VS2010 RTM)",
    21022: "9.00.21022 (VS2008 RTM)",
    30729: "9.00.30729 (VS2008 SP1)",
    51106: "11.00.51106 (VS2012 Update 4)",
}


def parse_rich_header(data: bytes) -> list[tuple[int, int, int]]:
    """Return a list of (product_id, build_no, count) tuples."""
    rich_idx = data.find(b"Rich")
    if rich_idx < 0:
        raise ValueError("'Rich' marker not found")
    # XOR key is the dword immediately after 'Rich'.
    key = struct.unpack_from("<I", data, rich_idx + 4)[0]

    # Walk backwards from 'Rich' in 4-byte steps until we find the obfuscated
    # 'DanS' marker (the prologue), XOR-decrypting along the way.
    decoded: list[int] = []
    cursor = rich_idx - 4
    while cursor >= 0x80:
        raw = struct.unpack_from("<I", data, cursor)[0]
        plain = raw ^ key
        if plain == 0x536E6144:  # 'DanS'
            break
        decoded.append(plain)
        cursor -= 4
    else:
        raise ValueError("'DanS' prologue not found while walking back")

    decoded.reverse()
    # Skip the three padding zero dwords that follow 'DanS'.
    while decoded and decoded[0] == 0:
        decoded.pop(0)

    records: list[tuple[int, int, int]] = []
    for i in range(0, len(decoded) - 1, 2):
        comp_id = decoded[i]
        count = decoded[i + 1]
        product = (comp_id >> 16) & 0xFFFF
        build = comp_id & 0xFFFF
        records.append((product, build, count))
    return records


def _read_linker_version(data: bytes) -> tuple[int, int]:
    """Return (Major, Minor) from the PE OptionalHeader without pefile."""
    e_lfanew = struct.unpack_from("<I", data, 0x3C)[0]
    # PE signature ('PE\0\0') + COFF header (20 bytes) precede OptionalHeader.
    opt_off = e_lfanew + 4 + 20
    major = data[opt_off + 2]
    minor = data[opt_off + 3]
    return major, minor


def main(argv: list[str]) -> int:
    if len(argv) < 2:
        print("usage: dump_rich_header.py <pe-file>", file=sys.stderr)
        return 2
    path = Path(argv[1])
    if not path.exists():
        print(f"missing: {path}", file=sys.stderr)
        return 1
    data = path.read_bytes()
    try:
        records = parse_rich_header(data)
    except ValueError as exc:
        print(f"failed to parse Rich Header: {exc}", file=sys.stderr)
        return 1

    major, minor = _read_linker_version(data)
    linker_marketing = LINKER_MAJOR.get(major, f"unknown major {major}")
    print(f"Rich Header for {path} ({len(records)} records)")
    print(
        f"PE OptionalHeader linker version: {major}.{minor:02d} -> {linker_marketing}"
    )
    print()
    print(f"{'product':<10} {'build':>8}  {'count':>6}  description")
    print("-" * 70)
    linker_builds: list[int] = []
    for product, build, count in records:
        name = PRODUCT_NAMES.get(product, f"product 0x{product:04X}")
        if "Linker" in name or "Cvtres" in name:
            linker_builds.append(build)
        print(f"0x{product:04X}    {build:>8}  {count:>6}  {name}")
    if linker_builds:
        highest = max(linker_builds)
        note = LINKER_BUILD_NOTES.get(highest, "")
        print()
        print(f"Highest linker/cvtres build number: {highest} (0x{highest:04X})")
        if note:
            print(f"  -> {note}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
