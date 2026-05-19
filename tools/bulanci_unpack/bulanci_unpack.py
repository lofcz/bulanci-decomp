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
# ClassIDs additionally used by the native game's master pack (bulanci.exe
# overlay). Names recovered from the game's RTTI / behavior; see the
# README for the analysis notes.
CLASS_AUDIO_BANK = 43            # CDSAudioBank PCM data blob
CLASS_MP3 = 48
CLASS_BITMAP_SPRITE = 52         # CDSBitmap animation/sprite atlas
CLASS_MOUSE_CURSOR = 54          # CDSImageMouse / CDSMouse cursor data
CLASS_DSM_INNER = 58             # embedded sub-archive (BULANCI.TMP)
CLASS_AUDIO_BANK_INDEX = 67      # CDSAudioBankSample index into ClassID 43
CLASS_BITMAP_JPEG_ANIM = 76      # animated/JPEG sprite (mixed header + JPEG)
CLASS_SIGN = 94
CLASS_SCRIPT = 2026
CLASS_TEXT_BLOCK = 2043          # CDSStaticTexts: u32 charCount + UTF-16 chars
CLASS_TEXT_TABLE = 2050          # CDSStaticTexts (multi-entry?) localised text table

CLASS_NAMES = {
    CLASS_BITMAP_JPEG: "BitmapJPEG",
    CLASS_BITMAP_BMP: "BitmapBMP",
    CLASS_BITMAP_SPECIAL: "BitmapSpecial",
    CLASS_AUDIO_BANK: "AudioBank",
    CLASS_MP3: "Mp3",
    CLASS_BITMAP_SPRITE: "BitmapSprite",
    CLASS_MOUSE_CURSOR: "MouseCursor",
    CLASS_DSM_INNER: "DsmInner",
    CLASS_AUDIO_BANK_INDEX: "AudioBankIndex",
    CLASS_BITMAP_JPEG_ANIM: "BitmapJpegAnim",
    CLASS_SIGN: "Sign",
    CLASS_SCRIPT: "Script",
    CLASS_TEXT_BLOCK: "TextBlock",
    CLASS_TEXT_TABLE: "TextTable",
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


_MARKER_BPP = {0: 1, 1: 2, 2: 4, 3: 8, 4: 16, 5: 24, 6: 32}


def _save_special(raw: bytes, base: Path) -> dict:
    """ClassID 28 - CDSImage / CDSBmpImage 'Special' bitmap.

    The 26-byte header is identical across all marker variants; only the
    trailing pixel payload differs. The layout was recovered from
    `CDSImage::Read` at `0x00437160` in `bulanci.exe` (which calls the byte
    reader 7 times for exactly 26 bytes) and the bpp lookup in
    `FUN_00435c70`:

        int32 width
        int32 height
        int32 marker          # bpp tier: 0=1bpp 1=2bpp 2=4bpp 3=8bpp
                              #           4=16bpp 5=24bpp BGR 6=32bpp BGRA
        int32 stride          # bytes per row of the packed pixel buffer
        int32 field4          # markers 0..3 with palette : transparent index
                              # marker 5: transparent RGB (R<<16 | G<<8 | B)
                              # 0xFFFFFFFF means "no transparency"
        byte  0xFF            # constant alpha sentinel (sometimes 0x80, 0xB4)
        int32 paletteCount    # markers 0..3: 4-byte palette entries that follow
                              # marker 5: editor always writes 0x01000000 here
        byte  hasUnpacked     # if 1, a width*height "unpacked" buffer follows
                              # the packed one (cached BGRA indices used by
                              # the game's blitter); 0 = packed-only.

    The editor only ever emits marker == 5 with hasUnpacked == 0 and the
    sentinel paletteCount == 0x01000000 (which the runtime ignores because
    marker >= 4). The native master pack uses every variant, including
    "monochrome with implicit grayscale palette" (`paletteCount == 0`).
    """
    meta: dict = {"friendlyFile": None}
    if len(raw) < 26:
        meta["warning"] = "Bitmap-Special: header truncated"
        return meta
    width, height, marker5, stride = struct.unpack_from("<iiii", raw, 0)
    field4 = struct.unpack_from("<I", raw, 16)[0]
    pad_a = raw[20]
    palette_count_field = struct.unpack_from("<i", raw, 21)[0]
    pad_b = raw[25]
    pixel_bytes = len(raw) - 26
    meta["special"] = {
        "width": width,
        "height": height,
        "marker5": marker5,
        "stride": stride,
        # Keep the old keys for backwards compatibility with manifests
        # that were generated before we understood markers 0..3.
        "transparentRGB": f"0x{field4:08X}",
        "padA": pad_a,
        "tag24bit": palette_count_field,
        "padB": pad_b,
        "pixelBytes": pixel_bytes,
    }
    if marker5 in _MARKER_BPP:
        meta["special"]["bpp"] = _MARKER_BPP[marker5]
    meta["special"]["hasUnpacked"] = bool(pad_b)

    if width <= 0 or height <= 0 or stride <= 0:
        meta["warning"] = "BitmapSpecial: nonsensical dimensions"
        return meta

    if marker5 == 5:
        # Editor-emitted 24bpp BGR. `field4` is an RGB sentinel
        # (R<<16 | G<<8 | B); 0xFFFFFFFF means fully opaque.
        if stride < width * 3 or 26 + height * stride != len(raw):
            meta["warning"] = (
                f"BitmapSpecial marker=5: pixel layout mismatch "
                f"(stride={stride}, expected payload {height*stride} "
                f"vs available {len(raw)-26})"
            )
            return meta
        try:
            png_bytes = _bgr24_to_png(
                raw[26:],
                width,
                height,
                stride,
                transparent_rgb=field4 if field4 != 0xFFFFFFFF else None,
            )
            out = base.with_suffix(".png")
            out.write_bytes(png_bytes)
            meta["friendlyFile"] = out.name
        except Exception as exc:  # noqa: BLE001
            meta["warning"] = f"BitmapSpecial marker=5: PNG encode failed: {exc}"
        return meta

    if marker5 in (0, 1, 2, 3):
        bpp = _MARKER_BPP[marker5]
        pal_count = palette_count_field
        if pal_count < 0 or pal_count > 256:
            meta["warning"] = (
                f"BitmapSpecial marker={marker5}: implausible "
                f"paletteCount={pal_count}"
            )
            return meta
        pal_bytes = pal_count * 4
        pixels_off = 26 + pal_bytes
        packed_payload = height * stride
        unpacked_payload = width * height if pad_b else 0
        expected = pal_bytes + packed_payload + unpacked_payload
        if expected != len(raw) - 26:
            meta["warning"] = (
                f"BitmapSpecial marker={marker5}: payload mismatch "
                f"(palette {pal_bytes} + packed {packed_payload} + "
                f"unpacked {unpacked_payload} != available {len(raw)-26})"
            )
            return meta

        transparent_index = field4 if field4 != 0xFFFFFFFF else None
        if pal_count > 0:
            palette = raw[26:pixels_off]
        else:
            # No palette in the file -> use an implicit identity grayscale
            # palette (idx i -> RGB(i,i,i)). For 1/2/4-bpp images with no
            # palette this still produces a sensible monochrome render even
            # if the original game would have re-coloured it at runtime.
            pal_count = 1 << bpp if bpp < 8 else 256
            buf = bytearray(pal_count * 4)
            for i in range(pal_count):
                v = (i * 255) // (pal_count - 1) if pal_count > 1 else 0
                buf[i * 4 + 0] = v  # B
                buf[i * 4 + 1] = v  # G
                buf[i * 4 + 2] = v  # R
                buf[i * 4 + 3] = 0
            palette = bytes(buf)
        try:
            png_bytes = _indexed_to_png(
                pixels=raw[pixels_off : pixels_off + packed_payload],
                palette=palette,
                width=width,
                height=height,
                stride=stride,
                bpp=bpp,
                transparent_index=transparent_index,
            )
            out = base.with_suffix(".png")
            out.write_bytes(png_bytes)
            meta["friendlyFile"] = out.name
            meta["special"]["paletteCount"] = palette_count_field
            if transparent_index is not None:
                meta["special"]["transparentIndex"] = transparent_index
        except Exception as exc:  # noqa: BLE001
            meta["warning"] = f"BitmapSpecial marker={marker5}: PNG encode failed: {exc}"
        return meta

    meta["warning"] = (
        f"BitmapSpecial marker={marker5}: variant not implemented "
        f"(bpp={_MARKER_BPP.get(marker5, '?')})"
    )
    return meta


def _bgr24_to_png(
    pixels: bytes,
    width: int,
    height: int,
    stride: int,
    transparent_rgb: Optional[int] = None,
) -> bytes:
    """Encode a top-down 24bpp BGR buffer as PNG (RGBA when transparent_rgb is given).

    Implemented in pure-Python to avoid pulling in `Pillow`. PNG is small
    enough that doing this by hand is cheap and dependency-free.
    """
    import struct as _struct
    import zlib as _zlib

    use_alpha = transparent_rgb is not None
    if use_alpha:
        tr = (transparent_rgb >> 16) & 0xFF
        tg = (transparent_rgb >> 8) & 0xFF
        tb = transparent_rgb & 0xFF
    out_rows = bytearray()
    for y in range(height):
        row_start = y * stride
        out_rows.append(0)
        if use_alpha:
            buf = bytearray(width * 4)
            for x in range(width):
                px = row_start + x * 3
                b = pixels[px]
                g = pixels[px + 1]
                r = pixels[px + 2]
                a = 0 if (r == tr and g == tg and b == tb) else 255
                ox = x * 4
                buf[ox] = r
                buf[ox + 1] = g
                buf[ox + 2] = b
                buf[ox + 3] = a
            out_rows.extend(buf)
        else:
            buf = bytearray(width * 3)
            for x in range(width):
                px = row_start + x * 3
                ox = x * 3
                buf[ox] = pixels[px + 2]
                buf[ox + 1] = pixels[px + 1]
                buf[ox + 2] = pixels[px]
            out_rows.extend(buf)

    color_type = 6 if use_alpha else 2  # 6=RGBA, 2=RGB
    idat = _zlib.compress(bytes(out_rows), 9)

    def chunk(tag: bytes, data: bytes) -> bytes:
        crc = _zlib.crc32(tag + data) & 0xFFFFFFFF
        return _struct.pack(">I", len(data)) + tag + data + _struct.pack(">I", crc)

    sig = b"\x89PNG\r\n\x1a\n"
    ihdr = _struct.pack(">IIBBBBB", width, height, 8, color_type, 0, 0, 0)
    return sig + chunk(b"IHDR", ihdr) + chunk(b"IDAT", idat) + chunk(b"IEND", b"")


def _indexed_to_png(
    pixels: bytes,
    palette: bytes,
    width: int,
    height: int,
    stride: int,
    bpp: int,
    transparent_index: Optional[int] = None,
) -> bytes:
    """Encode an 8bpp or 4bpp indexed bitmap as a PNG using a PLTE+tRNS palette.

    The native game's `BitmapSpecial` markers 2 (4bpp) and 3 (8bpp) store
    palette entries as B,G,R,reserved=0 quadruples. PNG palette chunks want
    RGB triples, so we swap the channels here. When a transparent palette
    index is provided we emit a `tRNS` chunk so PNG-aware viewers honor the
    transparency without us having to upgrade the image to RGBA.
    """
    import struct as _struct
    import zlib as _zlib

    if bpp not in (1, 2, 4, 8):
        raise ValueError(f"unsupported indexed bpp: {bpp}")
    pal_count = len(palette) // 4
    if pal_count == 0 or pal_count > 256:
        raise ValueError(f"palette count out of range: {pal_count}")

    plte = bytearray(pal_count * 3)
    for i in range(pal_count):
        # source: B G R reserved -> PNG wants R G B
        b = palette[i * 4]
        g = palette[i * 4 + 1]
        r = palette[i * 4 + 2]
        plte[i * 3 + 0] = r
        plte[i * 3 + 1] = g
        plte[i * 3 + 2] = b

    # Native packing matches PNG packing for indexed bitmaps at all of
    # 1/2/4/8 bpp: MSBs-first inside each byte, no padding between rows
    # (the source stride already covers that). The minimum number of
    # bytes per PNG row is `ceil(width * bpp / 8)`; the source may have
    # extra trailing bytes per row (`stride > row_bytes_packed`) that we
    # skip.
    out_rows = bytearray()
    row_bytes_packed = (width * bpp + 7) // 8
    for y in range(height):
        out_rows.append(0)  # filter = None
        src = y * stride
        out_rows.extend(pixels[src : src + row_bytes_packed])

    idat = _zlib.compress(bytes(out_rows), 9)

    def chunk(tag: bytes, data: bytes) -> bytes:
        crc = _zlib.crc32(tag + data) & 0xFFFFFFFF
        return _struct.pack(">I", len(data)) + tag + data + _struct.pack(">I", crc)

    sig = b"\x89PNG\r\n\x1a\n"
    ihdr = _struct.pack(
        ">IIBBBBB",
        width,
        height,
        bpp,
        3,  # color_type = 3 (indexed)
        0,
        0,
        0,
    )
    out = bytearray()
    out += sig
    out += chunk(b"IHDR", ihdr)
    out += chunk(b"PLTE", bytes(plte))
    if transparent_index is not None and 0 <= transparent_index < pal_count:
        # tRNS for indexed PNGs is an array of alpha bytes (one per palette
        # entry up to the highest defined entry). Mark the single transparent
        # index as 0 and everything before it as 255.
        trns = bytearray(0xFF for _ in range(transparent_index + 1))
        trns[transparent_index] = 0
        out += chunk(b"tRNS", bytes(trns))
    out += chunk(b"IDAT", idat)
    out += chunk(b"IEND", b"")
    return bytes(out)


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
    (base.with_suffix(".script.bin")).write_bytes(code)

    # Disassemble bytecode using the opcode table from
    # editor_il_spy/Editor.Scripts/Opcode.cs.
    try:
        asm = _disassemble_script(code, exports)
        out = base.with_suffix(".script.asm")
        out.write_text(asm, encoding="utf-8")
        meta["friendlyFile"] = out.name
    except Exception as exc:  # noqa: BLE001
        meta["warning"] = f"Script: disassembly failed: {exc}"
        meta["friendlyFile"] = base.with_suffix(".script.bin").name
    return meta


# --------------------------------------------------------------------------------------
# Script bytecode disassembler
# --------------------------------------------------------------------------------------

# (mnemonic, argument_spec). argument_spec items:
#   "i32"    -> read int32 from the bytecode stream
#   "u8"     -> read uint8 from the bytecode stream
#   "sub"    -> recursively read a nested Command (calls the dispatcher)
# Specials:
#   "_call"        -> Call: i32 target + u8 paramCount + paramCount sub args
#   "_switch"      -> Switch: sub expr + u8 caseCount + caseCount * (i32 val, i32 label)
#   "_strconst"    -> StrConst: UTF-16LE codepoints until a NUL terminator
#   "_create_anim" -> CreateAnim (opcode 45): 2 sub + u8 + u8 frameCount + frameCount * i32
#
# Editor-emitted opcodes are named per `editor_il_spy/Editor.Scripts/Opcode.cs`.
# Game-only opcodes (those that appear in the master pack's bytecode but never
# in editor-produced scripts) were recovered by disassembling each handler in
# `bulanci.exe` and counting calls to the well-known argument readers:
#
#   * `FUN_004384c0` -> evaluates a sub-expression (`sub`)
#   * `FUN_00438350` -> reads an `i32` from the bytecode stream
#   * `FUN_00438360` -> reads a `u16`
#   * `FUN_00438380` -> reads a `u8`
#
# See `ghidra_analysis/script_dispatch_table.md` for the per-opcode handler
# addresses and the base-vs-extension table layout that the native runtime
# constructs at `this+0x2c` (45 base entries at 0x004b0118 + 58 CLevelScript
# extension entries at 0x004af018 = opcodes 0..102 inclusive).
_OPCODES = {
    # --- CDSScript base opcodes (0..44, table at 0x004b0118) -----------------
    0:   ("IntConst",         ["i32"]),
    1:   ("Rand",             ["sub", "sub"]),
    2:   ("GetGlobalVar",     ["u8"]),
    3:   ("GetLocalVar",      ["u8"]),
    4:   ("Add",              ["sub", "sub"]),
    5:   ("Sub",              ["sub", "sub"]),
    6:   ("Mul",              ["sub", "sub"]),
    7:   ("Div",              ["sub", "sub"]),
    8:   ("Clamp",            ["sub", "sub", "sub"]),  # max(b, min(a, c))
    9:   ("Min",              ["sub", "sub"]),
    10:  ("Max",              ["sub", "sub"]),
    11:  ("SetGlobalVar",     ["u8", "sub"]),
    12:  ("SetLocalVar",      ["u8", "sub"]),
    13:  ("Goto",             ["i32"]),
    14:  ("Call",             "_call"),
    15:  ("ThisId",           []),  # returns this+0x42c->[0x8] (no arg getter)
    16:  ("GetLocalVarIdx",   ["sub"]),  # var[ECX + sub]
    17:  ("Return",           ["sub"]),
    18:  ("IfEqual",          ["sub", "sub", "i32"]),
    19:  ("IfLess",           ["sub", "sub", "i32"]),
    20:  ("IfGreater",        ["sub", "sub", "i32"]),
    21:  ("IfLessEq",         ["sub", "sub", "i32"]),
    22:  ("IfGreaterEq",      ["sub", "sub", "i32"]),
    23:  ("IfNotEqual",       ["sub", "sub", "i32"]),
    24:  ("Switch",           "_switch"),
    25:  ("Select",           "_switch"),  # like Switch but the labels are i32 *values*
    26:  ("StrDup",           ["sub"]),    # malloc + wcscpy a UTF-16 string
    27:  ("StrConst",         "_strconst"),
    28:  ("StrFree",          ["sub"]),
    29:  ("StrLen",           ["sub"]),
    30:  ("StrCharAt",        ["sub", "sub"]),
    31:  ("ShiftRight",       ["sub", "sub"]),
    32:  ("ShiftLeft",        ["sub", "sub"]),
    33:  ("And",              ["sub", "sub"]),
    34:  ("Or",               ["sub", "sub"]),
    35:  ("Not",              ["sub"]),
    36:  ("Negate",           ["sub"]),
    37:  ("StrmCreateMem",    ["sub", "sub"]),
    38:  ("StrmDestroy",      ["sub"]),
    39:  ("StrmWrite",        ["sub", "sub", "sub"]),
    40:  ("StrmRead",         ["sub", "sub"]),
    41:  ("StrmSeek",         ["sub", "sub"]),
    42:  ("StrmSetSize",      ["sub", "sub"]),
    43:  ("StrmGetPos",       ["sub"]),
    44:  ("StrmGetSize",      ["sub"]),
    # --- CLevelScript extension opcodes (45..102, table at 0x004af018) ------
    # Every entry in this block has been verified by hand-disassembling the
    # handler in `bulanci.exe`; opcodes whose name comes from
    # `Editor.Scripts.Opcode.cs` are kept as-is, the rest are named `opNN`
    # until their semantic role can be pinned down. See
    # `ghidra_analysis/script_dispatch_table.md` for per-handler addresses.
    45:  ("CreateAnim",       "_create_anim"),  # 2 sub + u8 + u8 count + count*i32
    46:  ("CreateImage",      ["sub", "sub", "i32"]),  # x, y, imageID
    47:  ("CreateObstacle",   ["sub", "sub", "sub", "sub"]),
    48:  ("SetObstacleBounds", ["sub", "sub", "sub", "sub", "sub"]),
    49:  ("SetOrderAxis",     ["sub", "sub"]),
    50:  ("InsertBulanci",    []),
    51:  ("InsertView",       ["sub"]),
    52:  ("SetInsertMode",    ["sub"]),
    53:  ("LoadPreface",      ["i32"]),
    54:  ("SetMusic",         ["i32", "i32"]),
    55:  ("op55",             ["sub", "sub"]),
    56:  ("op56",             ["sub", "sub"]),
    57:  ("op57",             ["sub"]),
    58:  ("op58",             ["sub", "sub", "sub"]),
    59:  ("op59",             ["sub"]),
    60:  ("op60",             ["sub"]),
    61:  ("op61",             ["sub", "sub", "sub"]),
    62:  ("op62",             ["sub"]),
    63:  ("op63",             ["sub", "sub", "sub"]),
    64:  ("op64",             ["sub"]),
    65:  ("op65",             ["sub", "sub"]),
    66:  ("op66",             ["sub", "sub"]),
    67:  ("op67",             ["sub", "sub", "sub", "sub", "sub"]),
    68:  ("op68",             ["sub", "sub", "sub"]),
    69:  ("op69",             ["sub"]),
    70:  ("op70",             ["sub"]),
    71:  ("op71",             ["sub"]),
    72:  ("op72",             ["sub", "sub"]),
    73:  ("op73",             ["sub", "sub", "sub", "sub", "sub"]),
    74:  ("IsServer",         []),
    75:  ("StrmSend",         ["sub"]),
    76:  ("SetCommStrm",      ["sub"]),
    77:  ("IsNet",            []),
    78:  ("op78",             ["sub", "sub"]),
    79:  ("op79",             ["sub", "i32"]),
    80:  ("op80",             ["sub", "sub"]),
    81:  ("MapSet",           ["sub", "sub"]),    # map[key].@0x18 = value
    82:  ("MapGet",           ["sub"]),           # returns map[key].@0x18
    83:  ("DefineTraceArea",  ["sub", "sub", "sub", "sub", "sub", "sub"]),
    84:  ("op84",             ["sub"]),
    85:  ("TeleportPlayerTo", ["sub", "sub", "sub", "sub"]),
    86:  ("op86",             ["sub", "sub"]),
    87:  ("op87",             ["sub"]),
    88:  ("op88",             ["sub", "sub"]),
    89:  ("op89",             []),               # allocates a fresh "list" object
    90:  ("op90",             ["sub"]),
    91:  ("op91",             ["sub", "sub", "sub"]),
    92:  ("ArrayGet",         ["sub", "sub"]),    # arr.@0x8[idx]
    93:  ("ArraySet",         ["sub", "sub", "sub"]),  # arr.@0x8[idx] = value
    94:  ("op94",             ["sub", "sub", "sub", "sub"]),
    95:  ("op95",             ["sub", "sub", "sub"]),
    96:  ("op96",             ["sub"]),
    97:  ("op97",             ["i32"]),          # CMenu image lookup
    98:  ("op98",             ["sub", "sub", "sub", "sub", "sub", "sub"]),
    99:  ("EnableFireThrough", ["sub", "sub"]),
    100: ("InsertVampires",   []),
    101: ("InsertOpponent",   ["sub", "sub", "sub", "sub"]),
    102: ("CreateMine",       ["sub", "sub"]),
}


def _disassemble_script(code: bytes, exports: List[int]) -> str:
    """Render the bytecode as a human-readable listing.

    Functions are anchored at the export offsets. The first byte at every
    function entry is `byte varCount`. We then iterate top-level commands;
    nested args within a command appear inline (so `SetGlobalVar 1 IntConst 1000`
    renders compactly).
    """
    lines: List[str] = []
    func_starts = sorted(set(int(e) for e in exports))
    fn_index = {addr: i for i, addr in enumerate(func_starts)}

    def export_label(off: int) -> str:
        i = fn_index.get(off)
        return f"export#{i}" if i is not None else f"fn@{off:#x}"

    def read_command(off: int) -> Tuple[int, str]:
        if off >= len(code):
            raise ValueError(f"command at {off:#x}: past end of code")
        op = code[off]
        end = off + 1
        info = _OPCODES.get(op)
        if info is None:
            return end, f"<UNKNOWN op={op}>"
        name, spec = info

        # Specials
        if spec == "_call":
            target = struct.unpack_from("<i", code, end)[0]
            end += 4
            param_count = code[end]
            end += 1
            arg_strs = []
            for _ in range(param_count):
                end, s = read_command(end)
                arg_strs.append(s)
            return end, f"{name}({export_label(target)}, {', '.join(arg_strs)})"

        if spec == "_switch":
            end, expr = read_command(end)
            count = code[end]
            end += 1
            cases = []
            for _ in range(count):
                val = struct.unpack_from("<i", code, end)[0]
                end += 4
                lbl = struct.unpack_from("<i", code, end)[0]
                end += 4
                cases.append(f"{val}=>{lbl:#x}")
            return end, f"{name}({expr}; {', '.join(cases)})"

        if spec == "_create_anim":
            # CreateAnim (CLevelScript opcode 45, handler 0x0041d6e0):
            #   sub x, sub y, u8 frameDelay, u8 frameCount, i32 imageId[frameCount]
            # Allocates a CAnim at +0xF0 and pushes `frameCount` frame images
            # (each looked up via CMenu::FUN_00413b20(i32)).
            end, x = read_command(end)
            end, y = read_command(end)
            delay = code[end]; end += 1
            n_frames = code[end]; end += 1
            frame_ids = []
            for _ in range(n_frames):
                fid = struct.unpack_from("<i", code, end)[0]
                end += 4
                frame_ids.append(fid)
            ids_str = ", ".join(str(f) for f in frame_ids)
            return end, f"{name}({x}, {y}, delay={delay}, frames=[{ids_str}])"

        if spec == "_strconst":
            # UTF-16LE characters until a null terminator. The compiler may
            # omit the terminator; cap at remaining code so we never overrun.
            chars: List[str] = []
            while end + 1 < len(code):
                cu = struct.unpack_from("<H", code, end)[0]
                end += 2
                if cu == 0:
                    break
                chars.append(chr(cu))
            return end, f"{name}({json.dumps(''.join(chars), ensure_ascii=False)})"

        arg_strs = []
        for token in spec:  # type: ignore[union-attr]
            if token == "i32":
                v = struct.unpack_from("<i", code, end)[0]
                end += 4
                arg_strs.append(str(v))
            elif token == "u8":
                arg_strs.append(str(code[end]))
                end += 1
            elif token == "sub":
                end, s = read_command(end)
                arg_strs.append(s)
            else:
                raise ValueError(f"unknown token {token!r}")
        return end, f"{name}({', '.join(arg_strs)})"

    def disasm_function(start: int, end_hint: int) -> None:
        if start >= len(code):
            lines.append(f"  ; (empty: past end)")
            return
        var_count = code[start]
        lines.append(f"  ; varCount={var_count}")
        off = start + 1
        guard = 0
        # Tracks whether we've seen control flow inside this function and
        # whether we just emitted a top-level Return. A function whose only
        # terminator is its first Return is straight-line prologue+exit; the
        # bytes that follow are an unreachable encoder tail (or an inline
        # helper that no export points at), and trying to parse them just
        # produces garbage. A function with branches/Goto/Switch typically
        # has multiple branch tails ending in Return, so we keep walking.
        # When we DO walk past a Return, the first UNKNOWN opcode we hit
        # also means we've fallen off the back of the real code -- stop
        # rather than continue printing bogus mnemonics.
        seen_branch = False
        while off < len(code) and off < end_hint:
            guard += 1
            if guard > 8192:
                lines.append(f"  ; ABORT: too many commands; bailing")
                break
            opc = code[off]
            if opc not in _OPCODES:
                # An unknown top-level opcode means we've fallen off the
                # back of the real script: encoder padding, an inline
                # helper that isn't pointed at by the export table, or
                # data baked into the code segment. Truncate cleanly.
                lines.append(
                    f"    ; ({end_hint - off} byte(s) of unreachable tail/inline helper "
                    f"starting with op={opc})"
                )
                break
            try:
                new_off, s = read_command(off)
            except Exception as exc:  # noqa: BLE001
                lines.append(f"    @{off:#06x}  ; PARSE ERROR: {exc}")
                break
            lines.append(f"    @{off:#06x}  {s}")
            off = new_off
            if not seen_branch and opc == 17:  # Return
                if off < end_hint:
                    lines.append(
                        f"    ; ({end_hint - off} byte(s) of unreachable tail/inline helper)"
                    )
                break
            # Mnemonics 13 (Goto), 18..23 (If*), 24 (Switch), 25 (Select)
            # introduce additional reachable basic blocks beyond the next
            # Return. After the first such opcode we walk to `end_hint`.
            if opc == 13 or 18 <= opc <= 25:
                seen_branch = True

    if not func_starts:
        lines.append("; (no exports; full code dump)")
        disasm_function(0, len(code))
    else:
        # First function may start before the first export (rare). The
        # editor-generated script always has at least one exported function
        # so this typically aligns.
        for i, fs in enumerate(func_starts):
            fe = func_starts[i + 1] if i + 1 < len(func_starts) else len(code)
            lines.append("")
            lines.append(f"fn export#{i} @ {fs:#06x}:")
            disasm_function(fs, fe)

    return "\n".join(lines) + "\n"


# --------------------------------------------------------------------------------------
# Native game (CDS*) resource extractors
# --------------------------------------------------------------------------------------


def _save_audio_bank(raw: bytes, base: Path) -> dict:
    """ClassID 43 - CDSAudioBank: raw PCM blob.

    Format:
        u32 dataLen        # bytes of PCM data that follow the header
        u16 channels
        u16 bits
        u32 freq           # samples per second
        byte[dataLen] PCM  # concatenated little-endian PCM samples
    """
    meta: dict = {"friendlyFile": None}
    if len(raw) < 12:
        meta["warning"] = "AudioBank: header truncated"
        return meta
    data_len, channels, bits, freq = struct.unpack_from("<IHHI", raw, 0)
    if data_len < 0 or 12 + data_len > len(raw):
        meta["warning"] = f"AudioBank: implausible dataLen {data_len}"
        return meta
    pcm = raw[12 : 12 + data_len]
    meta["audio"] = {
        "dataLen": data_len,
        "channels": channels,
        "bits": bits,
        "freq": freq,
        "pcmBytes": len(pcm),
    }
    # Wrap PCM in a minimal WAV so it can be played back directly.
    try:
        wav = _wrap_pcm_in_wav(pcm, channels=channels, bits=bits, freq=freq)
        out = base.with_suffix(".wav")
        out.write_bytes(wav)
        meta["friendlyFile"] = out.name
    except Exception as exc:  # noqa: BLE001
        meta["warning"] = f"AudioBank: WAV wrap failed: {exc}"
    return meta


def _wrap_pcm_in_wav(pcm: bytes, channels: int, bits: int, freq: int) -> bytes:
    """Minimal RIFF/WAVE wrapper around raw little-endian PCM."""
    byte_rate = freq * channels * (bits // 8)
    block_align = channels * (bits // 8)
    fmt = struct.pack(
        "<4sIHHIIHH",
        b"fmt ", 16, 1, channels, freq, byte_rate, block_align, bits,
    )
    data = b"data" + struct.pack("<I", len(pcm)) + pcm
    body = b"WAVE" + fmt + data
    return b"RIFF" + struct.pack("<I", len(body)) + body


def _save_audio_bank_index(raw: bytes, base: Path) -> dict:
    """ClassID 67 - CDSAudioBankSample: index into a CDSAudioBank.

    Format:
        u32 bankResourceID   # the matching ClassID-43 resource carrying the PCM
        u32 reserved         # observed 0 (likely loop point / flags)
        u32 sampleCount
        u32[sampleCount] byteLengths  # per-sample byte length inside the bank
    """
    meta: dict = {"friendlyFile": None}
    if len(raw) < 12:
        meta["warning"] = "AudioBankIndex: header truncated"
        return meta
    bank_id, reserved, count = struct.unpack_from("<III", raw, 0)
    needed = 12 + count * 4
    if count < 0 or count > 4096 or needed > len(raw):
        meta["warning"] = f"AudioBankIndex: implausible count {count}"
        return meta
    sizes = list(struct.unpack_from(f"<{count}I", raw, 12))
    cursor = 0
    samples = []
    for i, sz in enumerate(sizes):
        samples.append({"index": i, "offsetInBank": cursor, "byteLen": sz})
        cursor += sz
    meta["audioBank"] = {
        "bankResourceID": bank_id,
        "reserved": reserved,
        "sampleCount": count,
        "totalBankBytes": cursor,
        "samples": samples,
    }
    out = base.with_suffix(".bank.json")
    out.write_text(json.dumps(meta["audioBank"], ensure_ascii=False, indent=2), encoding="utf-8")
    meta["friendlyFile"] = out.name
    return meta


def _save_text_block(raw: bytes, base: Path) -> dict:
    """ClassID 2043 - UTF-16LE text block.

    Format: u32 charCount, char[charCount] UTF-16LE chars. The first char
    is often a marker (0x0001 / 0x0002) used as a section / language flag.
    """
    meta: dict = {"friendlyFile": None}
    if len(raw) < 4:
        meta["warning"] = "TextBlock: too short"
        return meta
    char_count = struct.unpack_from("<I", raw, 0)[0]
    if char_count < 0 or 4 + char_count * 2 > len(raw):
        meta["warning"] = f"TextBlock: implausible charCount {char_count}"
        return meta
    chars = raw[4 : 4 + char_count * 2].decode("utf-16-le", errors="replace")
    meta["text"] = {
        "charCount": char_count,
        "preview": chars[:200],
    }
    out = base.with_suffix(".txt")
    out.write_text(chars, encoding="utf-8")
    meta["friendlyFile"] = out.name
    return meta


def _save_text_table(raw: bytes, base: Path) -> dict:
    """ClassID 2050 - structured text/data table.

    Layout (best-effort, recovered empirically):
        u32 totalSize     # full resource size in bytes (=len(raw)+16-ish)
        u32 entryCount    # observed 1 in tutorial samples
        u32 reserved
        u32 reserved
        ... payload mixing fixed-size records and UTF-16LE strings.

    Phase 1 dumps a hex preview + extracts every embedded UTF-16LE string
    that's >= 2 characters and surrounded by null sentinels.
    """
    meta: dict = {"friendlyFile": None}
    if len(raw) < 16:
        meta["warning"] = "TextTable: too short"
        return meta
    total_size, count, r1, r2 = struct.unpack_from("<IIII", raw, 0)
    strings = _extract_utf16_strings(raw)
    meta["textTable"] = {
        "totalSize": total_size,
        "count": count,
        "reserved": [r1, r2],
        "extractedStrings": strings,
    }
    out = base.with_suffix(".table.json")
    out.write_text(json.dumps(meta["textTable"], ensure_ascii=False, indent=2), encoding="utf-8")
    meta["friendlyFile"] = out.name
    return meta


def _extract_utf16_strings(buf: bytes, min_chars: int = 2) -> List[str]:
    """Scan `buf` for printable-looking UTF-16LE substrings (>= `min_chars`).

    Skips surrogate pairs (0xD800 - 0xDFFF) so the result is always safely
    JSON-serialisable as UTF-8.
    """
    results: List[str] = []
    n = len(buf) // 2
    cu = struct.unpack(f"<{n}H", buf[: n * 2])
    cur: List[str] = []
    for v in cu:
        is_ascii = 32 <= v < 127
        is_unicode = 0x00A0 <= v <= 0xFFFD and not (0xD800 <= v <= 0xDFFF)
        if is_ascii or is_unicode:
            cur.append(chr(v))
        else:
            if len(cur) >= min_chars:
                results.append("".join(cur))
            cur = []
    if len(cur) >= min_chars:
        results.append("".join(cur))
    return results


def _save_dsm_inner(raw: bytes, base: Path) -> dict:
    """ClassID 58 - embedded binary file (CDSDsmFile inner record).

    Format:
        u32 nameLen, byte[nameLen] filename (ASCII),
        u64 fileTime (Windows FILETIME 100ns ticks since 1601),
        u32 fileLen, byte[fileLen] file contents.
    """
    meta: dict = {"friendlyFile": None}
    if len(raw) < 4:
        meta["warning"] = "DsmInner: too short"
        return meta
    off = 0
    name_len = struct.unpack_from("<I", raw, off)[0]
    off += 4
    if name_len < 0 or name_len > 260 or off + name_len > len(raw):
        meta["warning"] = f"DsmInner: implausible nameLen {name_len}"
        return meta
    name = raw[off : off + name_len].decode("ascii", errors="replace")
    off += name_len
    if off + 12 > len(raw):
        meta["warning"] = "DsmInner: truncated after name"
        return meta
    filetime = struct.unpack_from("<Q", raw, off)[0]
    off += 8
    file_len = struct.unpack_from("<I", raw, off)[0]
    off += 4
    if file_len < 0 or off + file_len > len(raw):
        meta["warning"] = f"DsmInner: implausible fileLen {file_len}"
        return meta
    content = raw[off : off + file_len]
    safe_name = name.replace("\\", "_").replace("/", "_")
    out = base.with_suffix("").parent / f"{base.name}_{safe_name}"
    out.write_bytes(content)
    meta["dsmInner"] = {
        "filename": name,
        "filetime": filetime,
        "fileLen": file_len,
    }
    meta["friendlyFile"] = out.name
    return meta


# BitmapSprite (ClassID 52) header constants. Header is exactly 11
# little-endian u32s (= 0x2c bytes) followed by the RLE/LC payload that
# `CBulPicture` decodes at runtime.
BITMAP_SPRITE_HEADER_SIZE = 0x2c
BITMAP_SPRITE_FIELDS = 11
# `channels` is 3 on every sample in the master pack (24bpp BGR pixels).
BITMAP_SPRITE_EXPECTED_CHANNELS = 3
# `inMemSize` (header[7]) commonly equals the `CBulPicture` allocation
# size (0x470 bytes from `CBulPicture::FUN_0040eb30`). Other observed
# values are extra runtime buffers above the base struct.
BITMAP_SPRITE_CBULPICTURE_SIZE = 0x470


def _save_bitmap_sprite(raw: bytes, base: Path) -> dict:
    """ClassID 52 - native sprite/animation container.

    The factory for ClassID 52 lives at `0x00432a50` in `bulanci.exe` and
    allocates an 80-byte handle whose vtable pointer chain matches the
    ``CDSMpx``/``CDSFlx`` family. The handle doesn't directly contain pixel
    data; instead it holds a reference to a ``CDSEasyMemStream`` of the raw
    payload, and the actual sprite is lazily decoded the first time a
    ``CBulPicture`` (1136 bytes, sibling factory at `0x0040eb30`) is
    instantiated to draw it. The CBulPicture layout (see
    `tools/bulanci_unpack/ghidra_analysis/sprite_container.md`) confirms
    the runtime ends up with:
        +0x68            uchar* pixel data
        +0x6c..+0x46b    256-entry RGBA palette (1024 bytes)
        +0x46d           transparent color index (0xFF = no transparency)

    The payload header parsed below is the bytes the engine reads out of
    the stream to populate the lazy decoder. Field semantics were recovered
    empirically by clustering sprites with identical metadata bytes:

        +0x00 u32 totalSize       == len(raw)
        +0x04 u32 encodedSize     bytes of "primary" encoded buffer
        +0x08 u32 encodedSize2    duplicate of encodedSize (often equal)
        +0x0c u32 width
        +0x10 u32 height
        +0x14 u32 frameCount      18..84 in the master pack
        +0x18 u32 channels        3 for every observed file (BGR)
        +0x1c u32 inMemSize       runtime CBulPicture size (often 0x470)
        +0x20 u32 flags           small (8..20); compression/codec selector
        +0x24 u32 encodedSize3    third copy of encodedSize; sometimes ==
                                  encodedSize, sometimes a per-frame slice
        +0x28 u32 packed          24-bit packed flags / palette pointer
        +0x2c ... RLE/LC-style frame stream (decoder still TODO)

    Files that share width/height/frameCount/encodedSize/flags but differ
    in `totalSize` are different recolours of the same base animation —
    a strong hint that the per-frame data is a delta encoding relative to
    a shared template.

    The parser emits soft `sanityWarnings` rather than refusing extraction
    so that future format variants are easy to spot from the manifest
    without breaking the rest of the pipeline.
    """
    meta: dict = {"friendlyFile": None}
    if len(raw) < BITMAP_SPRITE_HEADER_SIZE:
        meta["warning"] = (
            f"BitmapSprite: header truncated (have {len(raw)} bytes, "
            f"need {BITMAP_SPRITE_HEADER_SIZE})"
        )
        return meta

    fields = struct.unpack_from(f"<{BITMAP_SPRITE_FIELDS}I", raw, 0)
    sprite: dict = {
        "totalSize": fields[0],
        "encodedSize": fields[1],
        "encodedSize2": fields[2],
        "width": fields[3],
        "height": fields[4],
        "frameCount": fields[5],
        "channels": fields[6],
        "inMemSize": fields[7],
        "flags": fields[8],
        "encodedSize3": fields[9],
        # Rendered as hex so the 24-bit packed sentinel reads correctly
        # (e.g. 0x0005BF01) instead of as an opaque base-10 integer.
        "packed": f"0x{fields[10]:08X}",
        "payloadBytes": len(raw) - BITMAP_SPRITE_HEADER_SIZE,
    }
    meta["sprite"] = sprite

    # --- sanity checks ----------------------------------------------------
    # All checks are soft: every master-pack sample we have passes them,
    # so any future violation is genuinely interesting (new format
    # variant, corruption, or a sprite from a different game build).
    warnings: list = []
    total_size, _enc_a, enc_b, w, h, frames, channels, _in_mem, _flags, enc_c, _packed = fields
    if total_size != len(raw):
        warnings.append(
            f"totalSize {total_size} != len(raw) {len(raw)}"
        )
    if channels != BITMAP_SPRITE_EXPECTED_CHANNELS:
        warnings.append(
            f"channels {channels} != {BITMAP_SPRITE_EXPECTED_CHANNELS} (BGR)"
        )
    # The "third" encoded-size mirror always coincides with the second
    # one across every observed sample. Divergence between encodedSize
    # and encodedSize2 IS observed in a few rare multi-stream sprites,
    # so we only flag the (encodedSize2 vs encodedSize3) split.
    if enc_c != enc_b:
        warnings.append(
            f"encodedSize3 {enc_c} != encodedSize2 {enc_b}"
        )
    # Dimensions / frame counts. Two special values appear in the master
    # pack and are NOT corruption:
    #   * `width == 0`           — compound-atlas sprite (per-frame headers
    #                              decide the geometry). 8 samples.
    #   * `width == 0xFFFFFFFF`  — "fit to source" sentinel; same -1
    #                              convention BitmapSpecial uses elsewhere.
    #                              2 samples.
    # Anything else outside [0, 4096] indicates a genuine format break.
    if w not in (0, 0xFFFFFFFF) and not (0 < w <= 4096):
        warnings.append(f"implausible width {w}")
    if not (0 <= h <= 4096):
        warnings.append(f"implausible height {h}")
    if not (0 <= frames <= 1024):
        warnings.append(f"implausible frameCount {frames}")
    # Surface the sentinels as notes (not warnings) so consumers of the
    # manifest can tell at a glance which sprites use them.
    notes: list = []
    if w == 0xFFFFFFFF:
        notes.append("width == 0xFFFFFFFF (fit-to-source sentinel)")
    elif w == 0:
        notes.append("width == 0 (compound atlas; per-frame geometry)")
    if enc_b != fields[1]:
        # Already used elsewhere in this codebase to spot multi-stream
        # variants; surfacing it as a note keeps the spec self-documenting.
        notes.append("encodedSize2 != encodedSize (multi-stream variant)")
    if notes:
        sprite["notes"] = notes

    if warnings:
        sprite["sanityWarnings"] = warnings
    return meta


def _save_mouse_cursor(raw: bytes, base: Path) -> dict:
    """ClassID 54 - CDSImageMouse cursor data.

    Format (recovered empirically):
        u32 totalSize
        u32 frameCount   # observed 16
        u32 reserved
        u32 pixelBytes?  # observed 429 for 8KB cursor
        u32 transparent  # 0xFFFFFFFF
        u32 dataLen      # observed 1279
        u32 reserved
        u32 colorKey     # 0xFFFF0000
        byte[...]        # BGR triplet table then pixel data
    """
    meta: dict = {"friendlyFile": None}
    if len(raw) < 32:
        meta["warning"] = "MouseCursor: too short"
        return meta
    fields = struct.unpack_from("<8I", raw, 0)
    meta["cursor"] = {
        "totalSize": fields[0],
        "frameCount": fields[1],
        "reserved0": fields[2],
        "pixelBytes": fields[3],
        "transparent": f"0x{fields[4]:08X}",
        "dataLen": fields[5],
        "reserved1": fields[6],
        "colorKey": f"0x{fields[7]:08X}",
    }
    return meta


def _save_bitmap_jpeg_anim(raw: bytes, base: Path) -> dict:
    """ClassID 76 - composite bitmap with embedded JPEG frames + audio.

    Phase 1 records the descriptor and extracts every JPEG SOI..EOI run
    plus any obvious DSM-style PCM block found inside.
    """
    meta: dict = {"friendlyFile": None}
    if len(raw) < 24:
        meta["warning"] = "BitmapJpegAnim: too short"
        return meta
    fields = struct.unpack_from("<6I", raw, 0)
    meta["jpegAnim"] = {
        "totalSize": fields[0],
        "field1": fields[1],
        "field2": fields[2],
        "field3": fields[3],
        "field4": fields[4],
        "field5": fields[5],
    }
    # Find and dump embedded JPEGs (SOI marker 0xFFD8 followed by APP/DB/DA
    # marker, then EOI 0xFFD9). We require at least a sensible JFIF/JFXX/DB
    # second marker so spurious 0xFFD8 0xFF runs inside the audio payload
    # don't get extracted as fake "frames".
    jpegs = []
    i = 0
    idx = 0
    valid_seg_markers = {0xE0, 0xE1, 0xE2, 0xE3, 0xDB, 0xC0, 0xC4, 0xDA, 0xC2}
    while True:
        soi = raw.find(b"\xff\xd8\xff", i)
        if soi < 0:
            break
        if soi + 3 >= len(raw):
            break
        next_marker = raw[soi + 3]
        if next_marker not in valid_seg_markers:
            i = soi + 2
            continue
        eoi = raw.find(b"\xff\xd9", soi + 4)
        if eoi < 0:
            break
        blob = raw[soi : eoi + 2]
        # Sanity floor; real JPEGs almost never come in below ~512 bytes.
        if len(blob) < 256:
            i = eoi + 2
            continue
        out = base.with_suffix(f".frame{idx:03d}.jpg")
        out.write_bytes(blob)
        jpegs.append({"offset": soi, "length": len(blob), "file": out.name})
        idx += 1
        i = eoi + 2
    if jpegs:
        meta["jpegAnim"]["jpegFrames"] = jpegs
        meta["friendlyFile"] = jpegs[0]["file"]
    return meta


_EXTRACTORS = {
    CLASS_BITMAP_JPEG: _save_jpeg,
    CLASS_BITMAP_BMP: _save_bmp,
    CLASS_BITMAP_SPECIAL: _save_special,
    CLASS_AUDIO_BANK: _save_audio_bank,
    CLASS_MP3: _save_mp3,
    CLASS_BITMAP_SPRITE: _save_bitmap_sprite,
    CLASS_MOUSE_CURSOR: _save_mouse_cursor,
    CLASS_DSM_INNER: _save_dsm_inner,
    CLASS_AUDIO_BANK_INDEX: _save_audio_bank_index,
    CLASS_BITMAP_JPEG_ANIM: _save_bitmap_jpeg_anim,
    CLASS_SIGN: _save_sign,
    CLASS_SCRIPT: _save_script,
    CLASS_TEXT_BLOCK: _save_text_block,
    CLASS_TEXT_TABLE: _save_text_table,
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


def _build_id_name_map(eapres_path: Optional[Path]) -> dict:
    """If `eapres_path` exists, parse its XML payload and return {resourceID: name}.

    The `.eap` bundle only stores numeric IDs; the matching `.eapres` (editor
    project XML) carries the human-readable resource names that the user
    typed in the editor (Background, levels names, sound names, ...). We
    pair them up so each manifest entry can carry a friendly `name` field.

    Note: `.eapres` does not always store IDs inline (the editor assigns
    IDs at compile time). When IDs are missing we fall back to matching by
    order of appearance under the `<Resources>` element.
    """
    if eapres_path is None or not eapres_path.exists():
        return {}
    try:
        import xml.etree.ElementTree as ET

        data = eapres_path.read_bytes()
        # The file is wrapped in a GZIP container if it starts with 'GZIP'.
        if data[:4] == b"GZIP":
            try:
                payload, _ = decompress_container(data, 0)
                data = payload
            except Exception:  # noqa: BLE001
                return {}
        try:
            root = ET.fromstring(data.decode("utf-8", errors="replace"))
        except ET.ParseError:
            return {}
        names: dict = {}
        resources_node = root.find("Resources")
        if resources_node is None:
            return {}
        # Each <Resource> child carries a `Name` attribute. The editor
        # assigns IDs starting at 100000 in `Level.Compile()`.
        order: List[str] = []
        for child in resources_node:
            name = child.attrib.get("Name") or ""
            order.append(name)
        # Without an explicit ID field we use the compile-order ID base.
        for i, name in enumerate(order):
            names[100000 + i] = name
        return names
    except Exception:  # noqa: BLE001
        return {}


def unpack(
    buf: bytes,
    container_start: int,
    out_dir: Path,
    source: str,
    source_type: str,
    id_name_map: Optional[dict] = None,
) -> Manifest:
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
            if id_name_map and rh.id in id_name_map:
                meta["name"] = id_name_map[rh.id]
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
    # If a sibling .eapres exists, harvest its resource names so the
    # manifest entries get the editor's human-readable labels.
    inp = Path(args.input)
    sibling = inp.with_suffix(".eapres")
    if getattr(args, "names_from", None):
        sibling = Path(args.names_from)
    id_map = _build_id_name_map(sibling)
    return unpack(
        buf, 0, Path(args.output),
        source=args.input, source_type="eap",
        id_name_map=id_map,
    )


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
    p_eap.add_argument(
        "--names-from",
        help="Path to the matching .eapres for resource-name resolution "
        "(defaults to the sibling file with the same basename)",
    )
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
