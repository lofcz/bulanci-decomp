"""Emit per-unit COFF .obj targets for objdiff before Ghidra ExportDelinker
has been run.

Real, per-TU COFF targets come from
`scripts/ghidra/ExportDelinker.java` once Ghidra is installed and
`GHIDRA_HOME` is set. Until then, this script builds a stand-in COFF
that contains the actual function bytes from `orig/bulanci.exe`, with
one symbol per function pointing at its offset inside the synthetic
`.text` section. No relocations are emitted; that is what differentiates
this from the real Ghidra-delinker output.

Result: the first `report.json` honestly shows the unit's total code
size, zero matched bytes (because no base .obj exists yet), i.e. 0 %
progress. The placeholder is overwritten in place by ExportDelinker once
it can run.
"""
from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
DEFAULT_MAPPING = ROOT / "config" / "bulanci" / "mapping.csv"
DEFAULT_UNITS = ROOT / "config" / "bulanci" / "units_listing.csv"
DEFAULT_BINARY = ROOT / "orig" / "bulanci.exe"
DEFAULT_OUTPUT_DIR = ROOT / "build" / "orig" / "bulanci"

# COFF constants
IMAGE_FILE_MACHINE_I386 = 0x014C
IMAGE_FILE_32BIT_MACHINE = 0x0100
IMAGE_FILE_RELOCS_STRIPPED = 0x0001
IMAGE_SCN_CNT_CODE = 0x00000020
IMAGE_SCN_MEM_EXECUTE = 0x20000000
IMAGE_SCN_MEM_READ = 0x40000000
IMAGE_SYM_CLASS_EXTERNAL = 2
IMAGE_SYM_CLASS_STATIC = 3
IMAGE_SYM_TYPE_NULL = 0
IMAGE_SYM_DTYPE_FUNCTION = 0x20  # 2 << 4

SECTION_NUMBER_TEXT = 1
SYMBOL_SIZE = 18


# --- PE parsing helpers -----------------------------------------------------

class PE:
    def __init__(self, data: bytes):
        if data[:2] != b"MZ":
            raise ValueError("not a PE: missing MZ")
        e_lfanew = struct.unpack_from("<I", data, 0x3C)[0]
        if data[e_lfanew:e_lfanew + 4] != b"PE\x00\x00":
            raise ValueError("not a PE: missing PE signature")
        # IMAGE_FILE_HEADER
        coff = e_lfanew + 4
        (machine, num_sections, _ts, _ptr_sym, _num_sym,
         size_opt, _chars) = struct.unpack_from("<HHIIIHH", data, coff)
        if size_opt < 28:
            raise ValueError("optional header too small")
        opt = coff + 20
        # ImageBase lives at offset 28 for PE32, 24 for PE32+.
        magic = struct.unpack_from("<H", data, opt)[0]
        if magic == 0x10B:
            self.image_base = struct.unpack_from("<I", data, opt + 28)[0]
        elif magic == 0x20B:
            self.image_base = struct.unpack_from("<Q", data, opt + 24)[0]
        else:
            raise ValueError(f"unknown PE optional magic 0x{magic:04x}")
        # Section headers
        sec_off = opt + size_opt
        self.sections = []
        for i in range(num_sections):
            so = sec_off + 40 * i
            name = data[so:so + 8].rstrip(b"\x00").decode("ascii", errors="replace")
            (vsize, vaddr, raw_size, raw_addr,
             _ptr_reloc, _ptr_line, _n_reloc, _n_line, _chars) = struct.unpack_from(
                "<IIIIIIHHI", data, so + 8)
            self.sections.append({
                "name": name, "vsize": vsize, "vaddr": vaddr,
                "raw_size": raw_size, "raw_addr": raw_addr,
            })
        self.data = data

    def rva_to_offset(self, rva: int) -> int | None:
        for s in self.sections:
            if s["vaddr"] <= rva < s["vaddr"] + max(s["vsize"], s["raw_size"]):
                return s["raw_addr"] + (rva - s["vaddr"])
        return None

    def read_va(self, va: int, size: int) -> bytes:
        rva = va - self.image_base
        off = self.rva_to_offset(rva)
        if off is None:
            return b""
        return self.data[off:off + size]


# --- mapping.csv parsing ----------------------------------------------------

class FuncRow:
    __slots__ = ("namespace", "name", "addr", "size")

    def __init__(self, namespace: str, name: str, addr: int, size: int):
        self.namespace = namespace
        self.name = name
        self.addr = addr
        self.size = size


def load_mapping(path: Path) -> list[FuncRow]:
    out: list[FuncRow] = []
    for line in path.read_text(encoding="utf-8").splitlines():
        if not line.strip():
            continue
        parts = line.split(";")
        if len(parts) < 4:
            continue
        qualified = parts[1]
        addr = int(parts[2], 16)
        size = int(parts[3], 16)
        if size <= 0:
            continue
        if "::" in qualified:
            ns, _, name = qualified.rpartition("::")
        else:
            ns, name = "", qualified
        out.append(FuncRow(ns, name, addr, size))
    return out


def load_units(path: Path) -> dict[str, list[str]]:
    units: dict[str, list[str]] = {}
    for line in path.read_text(encoding="utf-8").splitlines():
        if not line.strip():
            continue
        parts = line.split(",")
        units[parts[0]] = parts[1:]
    return units


# --- COFF emission ----------------------------------------------------------

def _encode_symbol_name(name: str, string_table: bytearray) -> bytes:
    raw = name.encode("ascii", errors="replace")
    if len(raw) <= 8:
        return raw + b"\x00" * (8 - len(raw))
    # Long name: zero-int + offset into string table
    offset = len(string_table)
    string_table.extend(raw)
    string_table.append(0)
    return struct.pack("<II", 0, offset)


def build_unit_coff(funcs: list[FuncRow], pe: PE) -> bytes:
    text_data = bytearray()
    symbols: list[tuple[str, int]] = []  # (name, offset_within_text)
    for f in funcs:
        body = pe.read_va(f.addr, f.size)
        if len(body) < f.size:
            # function spans past the section -- pad with zeros so the
            # symbol table offsets stay honest
            body = body + b"\x00" * (f.size - len(body))
        symbols.append((f.name, len(text_data)))
        text_data.extend(body)

    if not text_data:
        # Mirror the simpler placeholder shape if there's nothing to emit.
        text_data.extend(b"\x90")

    section_name = b".text\x00\x00\x00"
    raw_data_offset = 20 + 40  # file header + 1 section header
    raw_data_size = len(text_data)

    section_header = struct.pack(
        "<8sIIIIIIHHI",
        section_name,
        0, 0,
        raw_data_size, raw_data_offset,
        0, 0,
        0, 0,
        IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ,
    )

    symbol_table_offset = raw_data_offset + raw_data_size
    string_table = bytearray()
    string_table.extend(b"\x00\x00\x00\x00")  # reserved 4-byte length, fixed up later

    symbol_records = bytearray()
    # Emit .text section symbol first (typical COFF convention)
    text_sym_name = _encode_symbol_name(".text", string_table)
    symbol_records.extend(text_sym_name)
    symbol_records.extend(struct.pack("<IhHBB", 0, SECTION_NUMBER_TEXT, 0, IMAGE_SYM_CLASS_STATIC, 0))
    num_symbols = 1

    for sym_name, offset in symbols:
        # Strip "@" and other characters COFF dislikes by quoting via long names.
        encoded = _encode_symbol_name(sym_name, string_table)
        symbol_records.extend(encoded)
        symbol_records.extend(struct.pack(
            "<IhHBB",
            offset,
            SECTION_NUMBER_TEXT,
            IMAGE_SYM_DTYPE_FUNCTION,
            IMAGE_SYM_CLASS_EXTERNAL,
            0,
        ))
        num_symbols += 1

    # Fix up string table length
    string_table[0:4] = struct.pack("<I", len(string_table))

    file_header = struct.pack(
        "<HHIIIHH",
        IMAGE_FILE_MACHINE_I386,
        1,                                         # NumberOfSections
        0,                                         # TimeDateStamp
        symbol_table_offset,                       # PointerToSymbolTable
        num_symbols,
        0,                                         # SizeOfOptionalHeader
        IMAGE_FILE_RELOCS_STRIPPED | IMAGE_FILE_32BIT_MACHINE,
    )

    return file_header + section_header + bytes(text_data) + bytes(symbol_records) + bytes(string_table)


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--mapping", default=str(DEFAULT_MAPPING))
    parser.add_argument("--units", default=str(DEFAULT_UNITS))
    parser.add_argument("--binary", default=str(DEFAULT_BINARY))
    parser.add_argument("--output-dir", default=str(DEFAULT_OUTPUT_DIR))
    parser.add_argument("--force", action="store_true",
                        help="Overwrite even when an existing .obj looks like a real Ghidra export")
    args = parser.parse_args(argv)

    mapping_path = Path(args.mapping)
    units_path = Path(args.units)
    binary_path = Path(args.binary)

    if not mapping_path.exists():
        raise SystemExit(f"{mapping_path} not found")
    if not units_path.exists():
        raise SystemExit(f"{units_path} not found")
    if not binary_path.exists():
        raise SystemExit(f"{binary_path} not found - drop bulanci.exe at orig/bulanci.exe")

    pe = PE(binary_path.read_bytes())
    all_funcs = load_mapping(mapping_path)
    units = load_units(units_path)

    by_namespace: dict[str, list[FuncRow]] = {}
    for f in all_funcs:
        by_namespace.setdefault(f.namespace, []).append(f)
    # sort each namespace by address for deterministic output
    for ns in by_namespace:
        by_namespace[ns].sort(key=lambda r: r.addr)

    output_dir = Path(args.output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    written = 0
    skipped = 0
    for unit_name, namespaces in units.items():
        funcs: list[FuncRow] = []
        for ns in namespaces:
            funcs.extend(by_namespace.get(ns, []))
        if not funcs:
            continue
        out = output_dir / f"{unit_name}.obj"
        if out.exists() and not args.force:
            if out.stat().st_size > 4 * 1024 * 1024:
                # If a Ghidra-derived .obj > 4 MiB already exists, leave it alone.
                skipped += 1
                continue
        out.write_bytes(build_unit_coff(funcs, pe))
        written += 1

    print(f"wrote {written} unit COFFs to {output_dir} (skipped {skipped})", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
