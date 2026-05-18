#!/usr/bin/env python3
"""Bulanci unpacker.

Decodes the custom "GZIP" container used by `bulanci.exe`'s PE overlay,
`.eap` (compiled level bundle), and `.eapres` (editor level XML).

Container format and resource layout were recovered from the decompiled
`Editor.exe` (see `editor_il_spy/Editor/GZipStream.cs`,
`editor_il_spy/Editor/Level.cs`, `editor_il_spy/Editor/ResourceItem.cs`,
`editor_il_spy/Editor.ResourceItems/*.cs`,
`editor_il_spy/Editor.Scripts/Script.cs`,
`editor_il_spy/Editor.Scripts/Opcode.cs`).
"""

from __future__ import annotations

import argparse
import json
import struct
import sys
import zlib
from dataclasses import asdict, dataclass, field
from pathlib import Path
from typing import List, Optional, Tuple


CONTAINER_MAGIC = 0x50495A47  # ASCII 'GZIP' as little-endian uint32
CHUNK_SIZE = 32768
# Second int32 in a bundle. Editor.exe always writes 8 (Level.Compile); the
# native game's master pack writes 32 instead. Treat it as an opaque format
# tag and only enforce the per-record marker.
FORMAT_TAG_EDITOR = 8
FORMAT_TAG_OVERLAY = 32
MARKER_HEADER = 90
# ResourceItem.WriteHeader: uint32 ID + uint32 ClassID + int64 Position + uint32 Size = 20 bytes.
HEADER_RECORD_SIZE = 20
# Each header is preceded on disk by an int32 == 90 (Level.Compile).
HEADER_RECORD_STRIDE = 4 + HEADER_RECORD_SIZE  # 24 bytes per resource header on disk

CLASS_BITMAP_JPEG = 21
CLASS_BITMAP_BMP = 22
CLASS_BITMAP_SPECIAL = 28
CLASS_MP3 = 48
CLASS_SIGN = 94
CLASS_SCRIPT = 2026

CLASS_NAMES = {
    CLASS_BITMAP_JPEG: "BitmapJPEG",
    CLASS_BITMAP_BMP: "BitmapBMP",
    CLASS_BITMAP_SPECIAL: "BitmapSpecial",
    CLASS_MP3: "Mp3",
    CLASS_SIGN: "Sign",
    CLASS_SCRIPT: "Script",
}


# --------------------------------------------------------------------------------------
# Data classes
# --------------------------------------------------------------------------------------


@dataclass
class ResourceHeader:
    """One entry in a `.eap` bundle's header table."""

    id: int
    class_id: int
    position: int
    size: int


@dataclass
class ContainerInfo:
    """Decoded `.eap`/.eapres/overlay container header."""

    dir_off: int
    uncompressed_size: int
    n_chunks: int
    index_len: int
    container_start: int
    container_total_bytes: int


@dataclass
class Manifest:
    """JSON dump that goes alongside extracted resources."""

    source: str
    source_type: str
    container_start: int
    container_total_bytes: int
    container_dir_off: int
    container_chunks: int
    container_uncompressed_size: int
    payload_kind: str
    eap_count: int = 0
    eap_format_tag: int = 0
    resources: List[dict] = field(default_factory=list)
    unknown_class_ids: List[int] = field(default_factory=list)
    notes: List[str] = field(default_factory=list)


# --------------------------------------------------------------------------------------
# GZIP container decoder
# --------------------------------------------------------------------------------------


def decompress_container(buf: bytes, start: int) -> Tuple[bytes, ContainerInfo]:
    """Decompress a single GZIP container located at `start` within `buf`.

    Returns the fully-inflated payload and a `ContainerInfo` describing the
    container header. Raises `ValueError` if anything fails plausibility
    checks (we'd rather refuse than guess silently).
    """
    if len(buf) - start < 12:
        raise ValueError("Buffer too short to contain GZIP header")

    magic = struct.unpack_from("<I", buf, start)[0]
    if magic != CONTAINER_MAGIC:
        raise ValueError(
            f"Bad magic at 0x{start:X}: 0x{magic:08X} (expected 'GZIP' / 0x{CONTAINER_MAGIC:08X})"
        )

    dir_off = struct.unpack_from("<q", buf, start + 4)[0]
    if dir_off < 12 or start + dir_off + 12 > len(buf):
        raise ValueError(f"Implausible dirOff {dir_off}")

    uncompressed_size = struct.unpack_from("<q", buf, start + dir_off)[0]
    index_len = struct.unpack_from("<i", buf, start + dir_off + 8)[0]
    if uncompressed_size < 0:
        raise ValueError(f"Negative uncompressed size {uncompressed_size}")
    if index_len <= 0 or start + dir_off + 12 + index_len > len(buf):
        raise ValueError(f"Bad index length {index_len}")

    index_blob = bytes(buf[start + dir_off + 12 : start + dir_off + 12 + index_len])

    n_chunks = (uncompressed_size + CHUNK_SIZE - 1) // CHUNK_SIZE
    # Per Editor.GZipStream.CloseInt(): writer writes m_chunks (Count == n_chunks + 1) entries.
    # The reader expects num2 - 1 = n_chunks + 1 entries (it then duplicates the last as a sentinel).
    expected_entries = n_chunks + 1

    raw_dir = zlib.decompress(index_blob)
    needed_bytes = 8 * expected_entries
    if len(raw_dir) < needed_bytes:
        raise ValueError(
            f"Decompressed directory too small: {len(raw_dir)} < {needed_bytes}"
        )
    chunk_offsets = list(struct.unpack(f"<{expected_entries}q", raw_dir[:needed_bytes]))

    # The last chunk offset must equal dir_off (writer captures position right
    # before writing the directory).
    if chunk_offsets[-1] != dir_off:
        raise ValueError(
            f"Last chunk offset {chunk_offsets[-1]} != dirOff {dir_off}"
        )
    if chunk_offsets[0] != 12:
        raise ValueError(
            f"First chunk offset {chunk_offsets[0]} != 12 (expected end of header)"
        )

    out = bytearray()
    for n in range(n_chunks):
        a = chunk_offsets[n]
        b = chunk_offsets[n + 1]
        size = b - a
        if size <= 0 or size > CHUNK_SIZE:
            raise ValueError(f"Bad chunk[{n}] size {size}")
        chunk_bytes = bytes(buf[start + a : start + b])
        if size < CHUNK_SIZE:
            try:
                inflated = zlib.decompress(chunk_bytes)
            except zlib.error as exc:
                raise ValueError(f"Inflate failed at chunk {n}: {exc}") from exc
        else:
            inflated = chunk_bytes
        out.extend(inflated)

    if len(out) != uncompressed_size:
        raise ValueError(
            f"Inflated payload size {len(out)} != recorded {uncompressed_size}"
        )

    info = ContainerInfo(
        dir_off=dir_off,
        uncompressed_size=uncompressed_size,
        n_chunks=n_chunks,
        index_len=index_len,
        container_start=start,
        container_total_bytes=dir_off + 12 + index_len,
    )
    return bytes(out), info


# --------------------------------------------------------------------------------------
# PE overlay locator
# --------------------------------------------------------------------------------------


def find_pe_overlay_start(buf: bytes) -> int:
    """Return the file offset at which the PE overlay (data after the last section) begins."""
    if buf[:2] != b"MZ":
        raise ValueError("Not a PE/MZ file")
    e_lfanew = struct.unpack_from("<I", buf, 0x3C)[0]
    coff = e_lfanew + 4
    num_sections = struct.unpack_from("<H", buf, coff + 2)[0]
    opt_size = struct.unpack_from("<H", buf, coff + 16)[0]
    sec = coff + 20 + opt_size
    max_end = 0
    for i in range(num_sections):
        base = sec + i * 40
        raw_size = struct.unpack_from("<I", buf, base + 16)[0]
        raw_ptr = struct.unpack_from("<I", buf, base + 20)[0]
        end = raw_ptr + raw_size
        if end > max_end:
            max_end = end
    return max_end


# --------------------------------------------------------------------------------------
# Payload sniffing
# --------------------------------------------------------------------------------------


def sniff_payload(payload: bytes) -> str:
    """Classify the decompressed payload as 'xml', 'eap-bundle', or 'unknown'."""
    head = payload[:64].lstrip()
    if head.startswith(b"<?xml") or head.startswith(b"<Level") or head.startswith(b"\xef\xbb\xbf<"):
        return "xml"
    if len(payload) < 12:
        return "unknown"
    count = struct.unpack_from("<i", payload, 0)[0]
    # We don't constrain the second int32 (format tag) - editor-compiled .eap
    # writes 8, the game's master pack writes 32; both share the same per-
    # record layout. The strong signal is the marker 90 at offset 8.
    if count <= 0 or count > 1_000_000:
        return "unknown"
    need = 8 + count * HEADER_RECORD_STRIDE
    if need > len(payload):
        return "unknown"
    first_record_marker = struct.unpack_from("<i", payload, 8)[0]
    if first_record_marker != MARKER_HEADER:
        return "unknown"
    return "eap-bundle"


# --------------------------------------------------------------------------------------
# `.eap` bundle parser
# --------------------------------------------------------------------------------------


def parse_eap_bundle(payload: bytes) -> Tuple[List[ResourceHeader], int, int]:
    """Parse the (count, format-tag, headers, data) layout of a compiled bundle.

    Returns `(headers, data_section_start, format_tag)`. `format_tag` is the
    second int32: `8` for editor-compiled .eap (Level.Compile), `32` for the
    native game's master pack inside bulanci.exe's overlay.
    """
    if len(payload) < 8:
        raise ValueError(".eap payload too short for header")
    count = struct.unpack_from("<i", payload, 0)[0]
    format_tag = struct.unpack_from("<i", payload, 4)[0]
    if count < 0 or count > 10_000_000:
        raise ValueError(f"Implausible resource count {count}")

    headers: List[ResourceHeader] = []
    off = 8
    for i in range(count):
        if off + HEADER_RECORD_STRIDE > len(payload):
            raise ValueError(f"Header {i}/{count} runs past end of payload")
        m = struct.unpack_from("<i", payload, off)[0]
        if m != MARKER_HEADER:
            raise ValueError(f"Expected marker {MARKER_HEADER} at offset {off}, got {m}")
        rid, classid, position, size = struct.unpack_from("<IIqI", payload, off + 4)
        headers.append(ResourceHeader(rid, classid, position, size))
        off += HEADER_RECORD_STRIDE
    return headers, off, format_tag


# --------------------------------------------------------------------------------------
# Resource extractors
# --------------------------------------------------------------------------------------


def _slice_resource(rh: ResourceHeader, data_section: bytes) -> bytes:
    if rh.position < 0 or rh.position + 4 + rh.size > len(data_section):
        raise ValueError(
            f"Resource {rh.id} (pos={rh.position} size={rh.size}) past data section ({len(data_section)} bytes)"
        )
    classid_check = struct.unpack_from("<I", data_section, rh.position)[0]
    if classid_check != rh.class_id:
        raise ValueError(
            f"Resource {rh.id}: header ClassID {rh.class_id} != data ClassID {classid_check} at pos {rh.position}"
        )
    return bytes(data_section[rh.position + 4 : rh.position + 4 + rh.size])


def _save_jpeg(raw: bytes, base: Path) -> dict:
    out = base.with_suffix(".jpg")
    out.write_bytes(raw)
    return {"friendlyFile": out.name}


def _save_bmp(raw: bytes, base: Path) -> dict:
    out = base.with_suffix(".bmp")
    out.write_bytes(raw)
    return {"friendlyFile": out.name}


def _save_special(raw: bytes, base: Path) -> dict:
    # Editor.ResourceItems.BitmapResourceItem.WriteBitmap, Special branch:
    #   int32 width, int32 height, int32 = 5, int32 stride,
    #   uint32 transparent_rgb (R<<16 | G<<8 | B; 0xFFFFFFFF if no transparency),
    #   byte 0xFF, int32 = 16777216, byte 0,
    #   byte[height*stride] of 24bpp BGR pixel data
    meta: dict = {"friendlyFile": None}
    if len(raw) < 26:
        meta["warning"] = "Bitmap-Special: header truncated"
        return meta
    width, height, marker5, stride = struct.unpack_from("<iiii", raw, 0)
    trans_rgb = struct.unpack_from("<I", raw, 16)[0]
    pad_a = raw[20]
    tag24 = struct.unpack_from("<i", raw, 21)[0]
    pad_b = raw[25]
    pixel_bytes = len(raw) - 26
    meta["special"] = {
        "width": width,
        "height": height,
        "marker5": marker5,
        "stride": stride,
        "transparentRGB": f"0x{trans_rgb:08X}",
        "padA": pad_a,
        "tag24bit": tag24,
        "padB": pad_b,
        "pixelBytes": pixel_bytes,
    }
    # Phase 2 will convert this to PNG; for Phase 1, the raw bytes are already in res_*.bin
    return meta


def _save_mp3(raw: bytes, base: Path) -> dict:
    # Editor.ResourceItems.Mp3ResourceItem.Write:
    #   int32 dataLen, uint32 size, ushort channels, ushort bits, uint32 freq, byte[dataLen]
    meta: dict = {"friendlyFile": None}
    if len(raw) < 16:
        meta["warning"] = "Mp3: header truncated"
        return meta
    data_len = struct.unpack_from("<i", raw, 0)[0]
    size, channels, bits, freq = struct.unpack_from("<IHHI", raw, 4)
    if data_len < 0 or 16 + data_len > len(raw):
        meta["warning"] = f"Mp3: implausible dataLen {data_len}"
        return meta
    out = base.with_suffix(".mp3")
    out.write_bytes(raw[16 : 16 + data_len])
    meta["friendlyFile"] = out.name
    meta["audio"] = {
        "dataLen": data_len,
        "reportedSize": size,
        "channels": channels,
        "bits": bits,
        "freq": freq,
    }
    return meta


def _save_sign(raw: bytes, base: Path) -> dict:
    # Editor.ResourceItems.ResourceSign.Write:
    #   uint32 packed_date ((year<<11)|(month<<6)|day),
    #   int32 len + UTF-16LE chars (content),
    #   int32 len + UTF-16LE chars (copyright),
    #   byte 0
    meta: dict = {"friendlyFile": None}
    try:
        packed = struct.unpack_from("<I", raw, 0)[0]
        year = packed >> 11
        month = (packed >> 6) & 0x1F
        day = packed & 0x3F
        off = 4
        clen = struct.unpack_from("<i", raw, off)[0]
        off += 4
        content = raw[off : off + clen * 2].decode("utf-16-le")
        off += clen * 2
        kclen = struct.unpack_from("<i", raw, off)[0]
        off += 4
        cright = raw[off : off + kclen * 2].decode("utf-16-le")
        sign = {"date": f"{year:04d}-{month:02d}-{day:02d}", "content": content, "copyright": cright}
        out = base.with_suffix(".sign.json")
        out.write_text(json.dumps(sign, ensure_ascii=False, indent=2), encoding="utf-8")
        meta["sign"] = sign
        meta["friendlyFile"] = out.name
    except Exception as exc:  # noqa: BLE001
        meta["warning"] = f"Sign decode failed: {exc}"
    return meta


def _save_script(raw: bytes, base: Path) -> dict:
    # Editor.Scripts.Script.Write:
    #   int32 codeLen, int32 nExports, int32 nVars,
    #   byte[codeLen] bytecode,
    #   int32[nExports] entry offsets
    meta: dict = {"friendlyFile": None}
    if len(raw) < 12:
        meta["warning"] = "Script: header truncated"
        return meta
    code_len, n_exports, n_vars = struct.unpack_from("<iii", raw, 0)
    meta["script"] = {"codeLen": code_len, "nExports": n_exports, "nVars": n_vars}
    if code_len < 0 or 12 + code_len + 4 * n_exports > len(raw):
        meta["warning"] = "Script: implausible sizes"
        return meta
    code = raw[12 : 12 + code_len]
    exports_off = 12 + code_len
    exports = list(
        struct.unpack_from(f"<{n_exports}i", raw, exports_off)
    ) if n_exports > 0 else []
    meta["script"]["entryPoints"] = exports
    out = base.with_suffix(".script.bin")
    out.write_bytes(code)
    meta["friendlyFile"] = out.name
    return meta


_EXTRACTORS = {
    CLASS_BITMAP_JPEG: _save_jpeg,
    CLASS_BITMAP_BMP: _save_bmp,
    CLASS_BITMAP_SPECIAL: _save_special,
    CLASS_MP3: _save_mp3,
    CLASS_SIGN: _save_sign,
    CLASS_SCRIPT: _save_script,
}


def save_resource(rh: ResourceHeader, raw: bytes, out_dir: Path) -> dict:
    """Write `res_<ID>_<ClassID>_<Name>.bin` + any friendly companion file."""
    class_label = CLASS_NAMES.get(rh.class_id, f"cls{rh.class_id}")
    base = out_dir / f"res_{rh.id:010d}_{rh.class_id}_{class_label}"
    raw_path = base.with_suffix(".bin")
    raw_path.write_bytes(raw)

    meta: dict = {
        "id": rh.id,
        "classId": rh.class_id,
        "className": CLASS_NAMES.get(rh.class_id, "Unknown"),
        "position": rh.position,
        "size": rh.size,
        "rawFile": raw_path.name,
    }
    extractor = _EXTRACTORS.get(rh.class_id)
    if extractor is not None:
        try:
            meta.update(extractor(raw, base))
        except Exception as exc:  # noqa: BLE001
            meta["warning"] = f"Extractor failed: {exc}"
    return meta


# --------------------------------------------------------------------------------------
# Pipeline
# --------------------------------------------------------------------------------------


def unpack(buf: bytes, container_start: int, out_dir: Path, source: str, source_type: str) -> Manifest:
    out_dir.mkdir(parents=True, exist_ok=True)

    payload, info = decompress_container(buf, container_start)
    (out_dir / "_raw.bin").write_bytes(payload)

    kind = sniff_payload(payload)
    mf = Manifest(
        source=source,
        source_type=source_type,
        container_start=info.container_start,
        container_total_bytes=info.container_total_bytes,
        container_dir_off=info.dir_off,
        container_chunks=info.n_chunks,
        container_uncompressed_size=info.uncompressed_size,
        payload_kind=kind,
    )

    if kind == "xml":
        xml_path = out_dir / "_payload.xml"
        xml_path.write_bytes(payload)
        mf.notes.append("Payload is XML (Editor.Level serialization).")
        # Cheap structural verification: ensure root element is <Level>.
        try:
            import xml.etree.ElementTree as ET

            root = ET.fromstring(payload.decode("utf-8", errors="replace"))
            mf.notes.append(
                f"XML root: <{root.tag}> with {len(list(root))} top-level children"
            )
        except Exception as exc:  # noqa: BLE001
            mf.notes.append(f"XML parse warning: {exc}")
    elif kind == "eap-bundle":
        headers, data_start, format_tag = parse_eap_bundle(payload)
        data_section = payload[data_start:]
        mf.eap_count = len(headers)
        mf.eap_format_tag = format_tag
        tag_note = f"format tag = {format_tag}"
        if format_tag == FORMAT_TAG_EDITOR:
            tag_note += " (editor-compiled .eap)"
        elif format_tag == FORMAT_TAG_OVERLAY:
            tag_note += " (native game master pack)"
        else:
            tag_note += " (unknown variant)"
        mf.notes.append(tag_note)
        for rh in headers:
            try:
                raw = _slice_resource(rh, data_section)
            except Exception as exc:  # noqa: BLE001
                mf.resources.append(
                    {
                        "id": rh.id,
                        "classId": rh.class_id,
                        "className": CLASS_NAMES.get(rh.class_id, "Unknown"),
                        "position": rh.position,
                        "size": rh.size,
                        "error": str(exc),
                    }
                )
                continue
            meta = save_resource(rh, raw, out_dir)
            mf.resources.append(meta)
            if rh.class_id not in CLASS_NAMES and rh.class_id not in mf.unknown_class_ids:
                mf.unknown_class_ids.append(rh.class_id)
        mf.notes.append(
            f"Parsed {len(headers)} .eap resources; data section begins at payload offset {data_start}"
        )
    else:
        head_hex = " ".join(f"{b:02X}" for b in payload[:64])
        mf.notes.append("Payload kind is unknown; only _raw.bin was written.")
        mf.notes.append(f"First 64 bytes: {head_hex}")

    (out_dir / "_manifest.json").write_text(
        json.dumps(asdict(mf), ensure_ascii=False, indent=2), encoding="utf-8"
    )
    return mf


# --------------------------------------------------------------------------------------
# CLI
# --------------------------------------------------------------------------------------


def _read_bytes(path: str) -> bytes:
    return Path(path).read_bytes()


def cmd_overlay(args: argparse.Namespace) -> Manifest:
    buf = _read_bytes(args.input)
    start = find_pe_overlay_start(buf)
    print(
        f"PE overlay starts at file offset 0x{start:X} (file size 0x{len(buf):X}); "
        f"overlay length = {len(buf) - start} bytes"
    )
    return unpack(buf, start, Path(args.output), source=args.input, source_type="overlay")


def cmd_eap(args: argparse.Namespace) -> Manifest:
    buf = _read_bytes(args.input)
    return unpack(buf, 0, Path(args.output), source=args.input, source_type="eap")


def cmd_eapres(args: argparse.Namespace) -> Manifest:
    buf = _read_bytes(args.input)
    return unpack(buf, 0, Path(args.output), source=args.input, source_type="eapres")


def cmd_auto(args: argparse.Namespace) -> Manifest:
    p = Path(args.input)
    name = p.name.lower()
    if name.endswith(".exe"):
        return cmd_overlay(args)
    if name.endswith(".eapres"):
        return cmd_eapres(args)
    if name.endswith(".eap"):
        return cmd_eap(args)
    # Last resort: assume the file IS a container starting at offset 0.
    buf = _read_bytes(args.input)
    return unpack(buf, 0, Path(args.output), source=args.input, source_type="auto")


def _print_summary(mf: Manifest) -> None:
    print()
    print(f"== Unpack summary: {mf.source} ==")
    print(f"  type             : {mf.source_type}")
    print(f"  container start  : 0x{mf.container_start:X}")
    print(f"  container bytes  : {mf.container_total_bytes}")
    print(f"  dirOff           : 0x{mf.container_dir_off:X}")
    print(f"  chunks           : {mf.container_chunks}")
    print(f"  uncompressedSize : {mf.container_uncompressed_size} bytes")
    print(f"  payload kind     : {mf.payload_kind}")
    if mf.eap_count:
        print(f"  resource count   : {mf.eap_count}")
        print(f"  format tag       : {mf.eap_format_tag}")
        by_class: dict = {}
        for r in mf.resources:
            by_class[r["className"]] = by_class.get(r["className"], 0) + 1
        for k, v in sorted(by_class.items()):
            print(f"    {k:15s}: {v}")
        if mf.unknown_class_ids:
            print(f"  unknown ClassIDs : {mf.unknown_class_ids}")
    for n in mf.notes:
        print(f"  note: {n}")


def main(argv: Optional[List[str]] = None) -> int:
    parser = argparse.ArgumentParser(
        description=__doc__.splitlines()[0],
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    sub = parser.add_subparsers(dest="cmd", required=True)

    p_overlay = sub.add_parser("overlay", help="Unpack the PE overlay of bulanci.exe")
    p_overlay.add_argument("input", help="Path to bulanci.exe")
    p_overlay.add_argument("-o", "--output", required=True, help="Output directory")
    p_overlay.set_defaults(func=cmd_overlay)

    p_eap = sub.add_parser("eap", help="Unpack a compiled .eap level bundle")
    p_eap.add_argument("input")
    p_eap.add_argument("-o", "--output", required=True)
    p_eap.set_defaults(func=cmd_eap)

    p_eapres = sub.add_parser("eapres", help="Decompress a .eapres editor-level XML")
    p_eapres.add_argument("input")
    p_eapres.add_argument("-o", "--output", required=True)
    p_eapres.set_defaults(func=cmd_eapres)

    p_auto = sub.add_parser("auto", help="Auto-detect source type from file extension")
    p_auto.add_argument("input")
    p_auto.add_argument("-o", "--output", required=True)
    p_auto.set_defaults(func=cmd_auto)

    args = parser.parse_args(argv)
    try:
        mf = args.func(args)
    except Exception as exc:  # noqa: BLE001
        print(f"ERROR: {exc}", file=sys.stderr)
        return 2
    _print_summary(mf)
    return 0


if __name__ == "__main__":
    sys.exit(main())
