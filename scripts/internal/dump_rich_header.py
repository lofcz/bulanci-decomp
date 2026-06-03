"""Parse the PE Rich Header of a Windows binary.

The Rich Header is an undocumented blob Microsoft toolchains splice between
the DOS stub and the PE header. Each record is `(comp_id, count)` where the
high 16 bits of `comp_id` is the product type (pelib index) and the low 16 bits
is the build number (`mCV`). Product names follow avast/pelib
``RichHeader.cpp`` ``productNames[]`` (index == product id).

Usage:
    python scripts/internal/dump_rich_header.py orig/bulanci.exe
"""
from __future__ import annotations

import struct
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from pelib_rich_product_names import PELIB_PRODUCT_NAMES  # noqa: E402

# OptionalHeader.MajorLinkerVersion -> VS marketing name.
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

LINKER_BUILD_NOTES: dict[int, str] = {
    4035: "7.10.4035 (VS2003 toolset objects in a VS2005-linked binary)",
    50327: "8.00.50327 (VS2005 beta)",
    50727: "8.00.50727 (VS2005 RTM) — also 11.00.50727 (VS2012 RTM); use PE linker major to disambiguate",
    40219: "10.00.40219 (VS2010 SP1)",
    30319: "10.00.30319 (VS2010 RTM)",
    21022: "9.00.21022 (VS2008 RTM)",
    30729: "9.00.30729 (VS2008 SP1)",
    51106: "11.00.51106 (VS2012 Update 4)",
}


def product_name(product_id: int) -> str:
    if product_id < len(PELIB_PRODUCT_NAMES):
        return PELIB_PRODUCT_NAMES[product_id]
    return f"(unknown product 0x{product_id:04X})"


def parse_rich_header(data: bytes) -> list[tuple[int, int, int]]:
    """Return a list of (product_id, build_no, count) tuples."""
    rich_idx = data.find(b"Rich")
    if rich_idx < 0:
        raise ValueError("'Rich' marker not found")
    key = struct.unpack_from("<I", data, rich_idx + 4)[0]

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
    e_lfanew = struct.unpack_from("<I", data, 0x3C)[0]
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
    by_name: dict[str, int] = defaultdict(int)
    for product, build, count in records:
        name = product_name(product)
        by_name[name] += count
        if name.startswith("Linker") or name.startswith("Cvtres"):
            linker_builds.append(build)
        print(f"0x{product:04X}    {build:>8}  {count:>6}  {name}")

    if linker_builds:
        highest = max(linker_builds)
        note = LINKER_BUILD_NOTES.get(highest, "")
        print()
        print(f"Highest linker/cvtres build number: {highest} (0x{highest:04X})")
        if note:
            print(f"  -> {note}")

    print()
    print("Aggregated object counts (pelib product names):")
    for name, total in sorted(by_name.items(), key=lambda kv: (-kv[1], kv[0])):
        if total >= 5:
            print(f"  {total:>5}  {name}")

    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
