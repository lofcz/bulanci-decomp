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
import shutil
import struct
import sys
import zlib
from dataclasses import asdict, dataclass, field
from pathlib import Path
from collections import Counter
from typing import Iterable, List, Optional, Tuple


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
CLASS_FONT = 54                  # CDSFont pre-rendered font metadata + glyph sheet
CLASS_DSM_INNER = 58             # embedded sub-archive (BULANCI.TMP)
CLASS_AUDIO_BANK_INDEX = 67      # CDSAudioBankSample index into ClassID 43
CLASS_BITMAP_JPEG_ANIM = 76      # animated/JPEG sprite (mixed header + JPEG)
CLASS_SIGN = 94
CLASS_SCRIPT = 2026              # CDSScript / CLevelScript level script
CLASS_POEM = 2043                # CPoem: u32 charCount + UTF-16LE chars (Czech poem)
CLASS_HISTORY_SCRIPT = 2050      # CHistoryScript: CDSScript subclass for the history dialog
CLASS_HELP_SCRIPT = 2076         # CHelpScript: CDSScript subclass for the help dialog

# Note: the engine-side per-language string pool (`CDSStaticTexts`) does NOT
# live in any overlay/.eap resource -- it's baked into `bulanci.exe`'s .data
# section as a single static singleton. See
# `../../ghidra_analysis/static_texts.md` and `../../ghidra_analysis/static_texts.py`
# for the layout and extractor.

CLASS_NAMES = {
    CLASS_BITMAP_JPEG: "BitmapJPEG",
    CLASS_BITMAP_BMP: "BitmapBMP",
    CLASS_BITMAP_SPECIAL: "BitmapSpecial",
    CLASS_AUDIO_BANK: "AudioBank",
    CLASS_MP3: "Mp3",
    CLASS_BITMAP_SPRITE: "BitmapSprite",
    CLASS_FONT: "Font",
    CLASS_DSM_INNER: "DsmInner",
    CLASS_AUDIO_BANK_INDEX: "AudioBankIndex",
    CLASS_BITMAP_JPEG_ANIM: "BitmapJpegAnim",
    CLASS_SIGN: "Sign",
    CLASS_SCRIPT: "Script",
    CLASS_POEM: "Poem",
    CLASS_HISTORY_SCRIPT: "HistoryScript",
    CLASS_HELP_SCRIPT: "HelpScript",
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


def _save_script(raw: bytes, base: Path, *, kind: str = "level_script") -> dict:
    """Save a serialized `CDSScript` (or subclass).

    On disk every script subclass uses the same wire format -- the engine's
    deserialiser is on `CDSScript`, the subclass only differs in which
    runtime opcode-extension table it installs. So this single handler
    serves classIds 2026 (`CLevelScript`), 2050 (`CHistoryScript`) and
    2076 (`CHelpScript`). `kind` selects which extension table the
    disassembler should use; see `_disassemble_script`.

    Wire format (Editor.Scripts.Script.Write):
        int32 codeLen, int32 nExports, int32 nVars,
        byte[codeLen] bytecode,
        int32[nExports] entry offsets
    """
    meta: dict = {"friendlyFile": None}
    if len(raw) < 12:
        meta["warning"] = "Script: header truncated"
        return meta
    code_len, n_exports, n_vars = struct.unpack_from("<iii", raw, 0)
    meta["script"] = {
        "kind": kind,
        "codeLen": code_len,
        "nExports": n_exports,
        "nVars": n_vars,
    }
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

    # Disassemble bytecode. The opcode table is chosen by `kind`:
    #   level_script   -> full coverage (base 0..44 + CLevelScript 45..102)
    #   help_script    -> base only; 45..52 render as <UNKNOWN> until traced
    #   history_script -> ditto
    try:
        asm = _disassemble_script(code, exports, kind=kind)
        out = base.with_suffix(".script.asm")
        out.write_text(asm, encoding="utf-8")
        meta["friendlyFile"] = out.name
    except Exception as exc:  # noqa: BLE001
        meta["warning"] = f"Script: disassembly failed: {exc}"
        meta["friendlyFile"] = base.with_suffix(".script.bin").name
    return meta


def _save_history_script(raw: bytes, base: Path) -> dict:
    return _save_script(raw, base, kind="history_script")


def _save_help_script(raw: bytes, base: Path) -> dict:
    return _save_script(raw, base, kind="help_script")


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
# See `../../ghidra_analysis/script_dispatch_table.md` for the per-opcode handler
# addresses and the base-vs-extension table layout that the native runtime
# constructs at `this+0x2c` (45 base entries at 0x004b0118 + 58 CLevelScript
# extension entries at 0x004af018 = opcodes 0..102 inclusive).
_BASE_OPCODES: dict = {
    # --- CDSScript base opcodes (0..44, table at 0x004b0118) -----------------
    # Shared by every script subclass (`CLevelScript`, `CHelpScript`,
    # `CHistoryScript`). Built by `CDSScript::CDSScript` via
    # `FUN_00438310(this, 0, &PTR_LAB_004b0118, 0x2d)`.
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
}


_LEVELSCRIPT_EXT_OPCODES: dict = {
    # --- CLevelScript extension opcodes (45..102, table at 0x004af018) ------
    # Installed by `CLevelScript::CLevelScript` via
    # `FUN_00438310(this, 0x2d, &PTR_FUN_004af018, 0x3a)`.
    # Every entry has been verified against `bulanci.exe`'s handler. The
    # names come from one of three sources, in decreasing order of
    # confidence:
    #   1. The editor's `Editor.Scripts.Opcode.cs` enum (45..54, 74..77,
    #      83, 85, 99..102; the editor only emits these opcodes itself).
    #   2. A directly-named runtime helper the handler forwards to
    #      (e.g. opcode 65 calls a "spawn projectile at view" helper, so
    #      the script-side name is `SpawnAtView`).
    #   3. A best-effort name extracted from the handler's observable
    #      effect on the engine state (object slot table, animation
    #      component, collection methods, etc.).
    # See `../../ghidra_analysis/script_dispatch_table.md` for per-handler
    # addresses and the underlying engine call.
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
    55:  ("SetActive",        ["sub", "sub"]),    # view.@0x69 = (level > 0)
    56:  ("BindToSlot",       ["sub", "sub"]),    # CGaming.views[slot] = view
    57:  ("ResortDepth",      ["sub"]),           # CGaming::ReorderByDepth(view)
    58:  ("TranslateTo",      ["sub", "sub", "sub"]),  # CBulanek translate-to (x, y)
    59:  ("HideView",         ["sub"]),           # set bit 0 in flags + freeze
    60:  ("ShowView",         ["sub"]),           # clear bit 0 in flags + wake
    61:  ("EvalSeq3",         ["sub", "sub", "sub"]),  # evaluate 3 sub-exprs, return first
    62:  ("GetSlot",          ["sub"]),           # CGaming.views[slot]
    63:  ("SetAnim",          ["sub", "sub", "sub"]),  # view+0x98: anim id + flags
    64:  ("AnimResume",       ["sub"]),           # restart paused animation
    65:  ("SpawnAtView",      ["sub", "sub"]),    # create projectile at view, kind
    66:  ("SetAnimFrame",     ["sub", "sub"]),    # view+0x98: anim.frameIndex = f
    67:  ("SpawnEnemyAt",     ["sub", "sub", "sub", "sub", "sub"]),  # 5-arg spawn (view+rect+state)
    68:  ("RegisterTimer",    ["sub", "sub", "sub"]),  # this+0x440 timer slot
    69:  ("TimerStop",        ["sub"]),           # decrement timer count
    70:  ("TimerStart",       ["sub"]),           # set timer running bit
    71:  ("TimerRelease",     ["sub"]),           # free the timer slot
    72:  ("TimerSetData",     ["sub", "sub"]),    # store payload at slot.@4
    73:  ("DefineDangerZone", ["sub", "sub", "sub", "sub", "sub"]),  # kind + (x1,y1,x2,y2)
    74:  ("IsServer",         []),
    75:  ("StrmSend",         ["sub"]),
    76:  ("SetCommStrm",      ["sub"]),
    77:  ("IsNet",            []),
    78:  ("SetAnimDirection", ["sub", "sub"]),    # view.anim.@0x18 = direction
    79:  ("SetViewImage",     ["sub", "i32"]),    # view.@0x94+offset = CMenu.image[id]
    80:  ("PlayAnim",         ["sub", "sub"]),    # view+0x98: run + (loop?)
    81:  ("MapSet",           ["sub", "sub"]),    # map[key].@0x18 = value
    82:  ("MapGet",           ["sub"]),           # returns map[key].@0x18
    83:  ("DefineTraceArea",  ["sub", "sub", "sub", "sub", "sub", "sub"]),
    84:  ("KillObject",       ["sub"]),           # CBulanek::Damage on a GameView
    85:  ("TeleportPlayerTo", ["sub", "sub", "sub", "sub"]),
    86:  ("RemoveView",       ["sub", "sub"]),    # unbind from CGaming + optional release
    87:  ("IsViewKind",       ["sub"]),           # IsKindOf(view, CLevelScript-class-2031)
    88:  ("SeekAnim",         ["sub", "sub"]),    # advance anim frame counter
    89:  ("NewCollection",    []),                # allocate a fresh CDSCollection
    90:  ("GetField0C",       ["sub"]),           # returns view.@0xC (anim flags?)
    91:  ("CollResize",       ["sub", "sub", "sub"]),     # CDSCollection::Resize
    92:  ("ArrayGet",         ["sub", "sub"]),    # ((u32*)arr.@8)[idx]
    93:  ("ArraySet",         ["sub", "sub", "sub"]),     # ((u32*)arr.@8)[idx] = v
    94:  ("CollRemove",       ["sub", "sub", "sub", "sub"]),  # CDSCollection::Remove
    95:  ("CollInsert",       ["sub", "sub", "sub"]),     # CDSCollection::Insert
    96:  ("FreeObject",       ["sub"]),           # call vtable[2] (Release)
    97:  ("GetImage",         ["i32"]),           # CMenu image lookup by ID
    98:  ("SpawnOpponentEx",  ["sub", "sub", "sub", "sub", "sub", "sub"]),  # 6-arg InsertOpponent
    99:  ("EnableFireThrough", ["sub", "sub"]),
    100: ("InsertVampires",   []),
    101: ("InsertOpponent",   ["sub", "sub", "sub", "sub"]),
    102: ("CreateMine",       ["sub", "sub"]),
}


# CHelpScript / CHistoryScript install their *own* 8-entry extension tables
# at offset 45..52, NOT the 58-entry CLevelScript extension. Handler
# addresses (from `bulanci.exe`):
#
#   CHistoryScript table @ 0x004af7ac (CHistoryScript::CHistoryScript, 0x004226c0):
#     45 -> 0x00422b00   46 -> 0x00422bc0   47 -> 0x00423530   48 -> 0x00421750
#     49 -> 0x004215c0   50 -> 0x00422810   51 -> 0x004217e0   52 -> 0x00423820
#
#   CHelpScript table    @ 0x004af2fc (CHelpScript::CHelpScript,       0x004215e0):
#     45 -> 0x00421940   46 -> 0x00421a00   47 -> 0x004221a0   48 -> 0x00421750
#     49 -> 0x004215c0   50 -> 0x00422810   51 -> 0x004217e0   52 -> 0x00421af0
#
# Slots 48..51 are shared (identical handler addresses between Help and
# History). The other four slots (45/46/47/52) come in two flavours:
#
#   * 45..47 are functionally identical between Help and History (each
#     pair instantiates the same widget class with the same arg layout;
#     only the surrounding error-handling block differs). 47 in
#     particular is THE loader that turns the inline `i32 imageId` into
#     a CDSBitmap via `g_pApp.menu->FUN_00413b20(id)` -- it is the only
#     reference path that ties HistoryScript-screen image IDs to the
#     binary.
#
#   * 52 differs by type: CHistoryScript spawns a CMovieView
#     (loading a BitmapJpegAnim by inline i32) while CHelpScript spawns
#     a CButton (no i32, just sub-exprs).
#
# Naming convention below: shared opcodes are prefixed with `Hh` (Help/
# history); the divergent ones are named for the widget they create.
_SHARED_HELPHISTORY_OPCODES = {
    45: ("HhBuildStaticTextAuto", ["sub", "sub", "sub", "sub", "i32"]),  # x, y, text, w, fontId
    46: ("HhBuildStaticText",     ["sub", "sub", "sub", "sub", "sub", "sub", "i32", "sub"]),  # x, y, w, h, text, font, ?, ?
    47: ("HhCreateBitmap",        ["sub", "sub", "i32"]),  # x, y, imageId  <-- LOADS bitmaps
    48: ("HhSetStaticTextStyle",  "_textstyle"),  # widget + u8 count + count*sub
    49: ("HhPassFirst",           ["sub", "sub"]),  # returns 1st arg; 2nd consumed but unused
    50: ("HhSetByteField",        ["sub", "sub"]),  # *(widget+0x74+0x18) = byte(arg2)
    51: ("HhAddChild",            ["sub"]),
}
_HELPSCRIPT_EXT_OPCODES: dict = {
    **_SHARED_HELPHISTORY_OPCODES,
    52: ("HCreateButton", ["sub", "sub", "sub", "sub"]),  # ?, ?, text, kind
}
_HISTORYSCRIPT_EXT_OPCODES: dict = {
    **_SHARED_HELPHISTORY_OPCODES,
    52: ("HCreateMovie",  ["sub", "sub", "i32", "sub", "sub"]),  # x, y, movieId, ?, ?  <-- LOADS movies
}


_SCRIPT_OPCODES = {
    "level_script":   {**_BASE_OPCODES, **_LEVELSCRIPT_EXT_OPCODES},
    "help_script":    {**_BASE_OPCODES, **_HELPSCRIPT_EXT_OPCODES},
    "history_script": {**_BASE_OPCODES, **_HISTORYSCRIPT_EXT_OPCODES},
}


# Per-export lifecycle annotation for `CLevelScript`-shaped scripts. The
# engine invokes export#N for fixed lifecycle events (see
# `../../ghidra_analysis/script_lifecycle.md`). For scripts that follow that
# contract (every level script in the master pack does), the annotation
# makes the disassembly much easier to read. Scripts with fewer exports
# (e.g. help-page or history-page scripts only define export#0) just get
# the export#0 label and the rest are left bare.
# Each entry is (name, description, signature). `signature` is appended
# after the name in the disassembly label so the argument shape the engine
# guarantees is always visible. Mapping verified against every xref to
# `CDSScript::CallExport` (`FUN_00438c40`) in `bulanci.exe`; see
# `../../ghidra_analysis/script_lifecycle.md` for the full evidence chain.
_LEVEL_SCRIPT_EXPORT_NAMES = {
    0:  ("GetInfo",     "writes globals 0/1/2 = name/type/GUID for the level/help/history picker",
         "(language)"),
    1:  ("OnInit",      "fires once during CBulanci construction; level scene setup happens here",
         "()"),
    2:  ("OnDeinit",    "fires once during CGaming destruction; resource teardown",
         "()"),
    3:  ("OnBitmapEvt", "fires when a CBitmap sub-view emits an event (animation frame end, "
                        "click/hit). slot = view.@0x70 = its CGaming slot, evt = the event code",
         "(slot, evt)"),
    4:  ("OnSlotPlaced", "fires when CExplosion / net-msg-0x0f spawns or moves a slot via "
                         "`FUN_00417e80` and msg 0xd7 reaches it. slot = destination slot, "
                         "msgHi/msgLo = the upper/lower halves of the spawn parameter",
         "(slot, msgHi, msgLoBits)"),
    5:  ("OnSlotDisplaced", "companion to OnSlotPlaced: fires on a *second* slot when its "
                            "occupant gets displaced by a OnSlotPlaced event (msg 0xd8). "
                            "slot = the displaced slot, byParam = the slot that displaced it",
         "(slot, byParam)"),
    6:  ("OnTimer",     "fires when a `RegisterTimer(slotId, delay, flags)` countdown (opcodes "
                        "68..72; timer table at `this+0x440`) expires; the slot id passed back "
                        "is the same slotId originally registered",
         "(slotId)"),
    7:  ("OnEnter",     "fires when a player/entity *enters* a `DefineTraceArea` rectangle. "
                        "traceId = the first arg passed to DefineTraceArea, entitySlot = the "
                        "0..3 player slot (or 0x88-N for slots 4..7) that crossed the boundary",
         "(traceId, entitySlot)"),
    8:  ("OnLeave",     "fires when a player/entity *leaves* a `DefineTraceArea` rectangle "
                        "(symmetric to OnEnter, same argument shape)",
         "(traceId, entitySlot)"),
    9:  ("OnNetCustom", "fires from `CGame::ProcessNetMessage` case 0x15 with a stream handle. "
                        "Scripts typically `StrmRead(handle, 1)` the leading opcode byte and "
                        "switch on it to dispatch their own RPC sub-protocol",
         "(streamHandle)"),
    10: ("OnGameStart", "fires from `CGaming::FUN_0041c140(true)` whenever the level "
                        "transitions from paused/loaded to running (level start, post-pause "
                        "resume). Music is started and engine-side timer slots 1/2 are armed "
                        "right after this returns",
         "()"),
}


def _disassemble_script(
    code: bytes,
    exports: List[int],
    kind: str = "level_script",
) -> str:
    """Render the bytecode as a human-readable listing.

    `kind` selects which extension opcode table (45..) to merge in:

    * ``"level_script"`` – the canonical 58-entry `CLevelScript` extension
      at `0x004af018`. Fully named.
    * ``"history_script"`` – the 8-entry `CHistoryScript` extension at
      `0x004af7ac`. Handler addresses are recorded in
      `_HISTORYSCRIPT_EXT_OPCODES`'s preamble; not yet named, so opcodes
      45..52 render as ``<UNKNOWN op=N>``.
    * ``"help_script"`` – ditto for `CHelpScript` (table at `0x004af2fc`).

    Functions are anchored at the export offsets. The first byte at every
    function entry is `byte varCount`. We then iterate top-level commands;
    nested args within a command appear inline (so `SetGlobalVar 1 IntConst 1000`
    renders compactly).

    IMPORTANT: the exports table is **positional, not address-sorted**. The
    engine indexes it by lifecycle-event slot (`exports[0]` is always
    `GetInfo`, `exports[1]` is always `OnInit`, etc. — see
    `../../ghidra_analysis/script_lifecycle.md`). Several scripts in the master
    pack place lifecycle exports out of address order (e.g.
    `res_0000065859`'s `OnGameStart` lives at offset 1178, *before*
    `OnBitmapEvt` at 1212), so sorting the table by address scrambles the
    `export#N` labels. Walk the exports list in its original order and
    use a separate sorted-address pass purely to find function ends.
    """
    opcodes = _SCRIPT_OPCODES.get(kind)
    if opcodes is None:
        raise ValueError(f"unknown script kind {kind!r}; expected one of {list(_SCRIPT_OPCODES)}")
    lines: List[str] = []
    exports_int = [int(e) for e in exports]
    # Sorted distinct addresses → used only to find each function's `end_hint`.
    sorted_addrs = sorted(set(exports_int))
    next_addr_after = {
        a: (sorted_addrs[i + 1] if i + 1 < len(sorted_addrs) else len(code))
        for i, a in enumerate(sorted_addrs)
    }
    # Positional index → display label. When multiple exports share the same
    # bytecode offset (common for "unused" lifecycle slots that point at the
    # same `Return(0)` stub), every reference resolves to the lowest-index
    # owner so the disassembly listing stays unambiguous.
    fn_index: dict = {}
    for i, addr in enumerate(exports_int):
        fn_index.setdefault(addr, i)

    # Helper-function discovery: every Call(fn@target, ...) we see during
    # disassembly is appended to `call_targets`. After the export pass we
    # walk every still-unseen target so the listing covers the "inline
    # helper" tail that the old code dropped on the floor (and which
    # often contains CreateAnim / CreateImage opcodes loading sprite IDs
    # that nothing else in the binary references literally).
    call_targets: set[int] = set()

    def export_label(off: int) -> str:
        i = fn_index.get(off)
        return f"export#{i}" if i is not None else f"fn@{off:#x}"

    def read_command(off: int) -> Tuple[int, str]:
        if off >= len(code):
            raise ValueError(f"command at {off:#x}: past end of code")
        op = code[off]
        end = off + 1
        info = opcodes.get(op)
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
            # Track non-export call targets so we disassemble them as
            # separate functions after the main pass. Only consider
            # offsets inside the code blob; negative/huge targets are
            # almost always sign-extended `i32` literals interpreted as
            # call addresses (a corrupted byte stream).
            if (
                target not in fn_index
                and 0 <= target < len(code)
            ):
                call_targets.add(target)
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

        if spec == "_textstyle":
            # CHelpHistoryScript::ext_op48_shared (handler 0x00421750):
            #   sub widget, u8 count, count*sub style-attr
            # Each sub is an alloca-pushed style attribute eventually
            # passed to CStaticText::SetStyle(widget, attrs[], count).
            end, widget = read_command(end)
            count = code[end]; end += 1
            attrs = []
            for _ in range(count):
                end, s = read_command(end)
                attrs.append(s)
            return end, f"{name}({widget}, count={count}, [{', '.join(attrs)}])"

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
            if opc not in opcodes:
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

    if not exports_int:
        lines.append("; (no exports; full code dump)")
        disasm_function(0, len(code))
    else:
        # A CLevelScript-shaped script has exactly 11 exports (indices
        # 0..10, see `../../ghidra_analysis/script_lifecycle.md`). When that
        # signature matches, label each export with its lifecycle role so
        # the disassembly reads as event handlers rather than anonymous
        # functions. Scripts with a different shape (help/history pages
        # only use export#0) get the bare `export#N` label for indices
        # we don't have a contract for.
        looks_like_level_script = len(exports_int) == 11
        for i, fs in enumerate(exports_int):
            fe = next_addr_after.get(fs, len(code))
            lines.append("")
            role = _LEVEL_SCRIPT_EXPORT_NAMES.get(i) if looks_like_level_script else None
            if role is None and i == 0:
                # Every script uses export#0 as `GetInfo(language)`, so
                # label it even when the script doesn't look like a level.
                role = _LEVEL_SCRIPT_EXPORT_NAMES[0]
            if role is not None:
                name, desc, sig = role
                lines.append(f"fn export#{i} @ {fs:#06x}  ; {name}{sig}  -- {desc}")
            else:
                lines.append(f"fn export#{i} @ {fs:#06x}:")
            disasm_function(fs, fe)

        # Helper-function pass: every script in the master pack has
        # inline-helper functions called via Call(fn@addr, args) that
        # the export walker skips (they sit in the "unreachable tail"
        # past each export's first Return). These helpers regularly
        # contain CreateAnim / CreateImage opcodes whose i32 frame IDs
        # are the ONLY literal reference to certain master-pack assets
        # (e.g. all of band 8 in "Na dobrou noc" = res 65856 lives in
        # such a helper). Walk every discovered call target and
        # disassemble it as `fn@OFFSET`.
        #
        # End-of-helper detection: we use the next sorted helper / export
        # address as the upper bound so we don't accidentally consume the
        # following helper's bytes as one giant function.
        worked: set[int] = set()
        # Worklist that grows as we discover nested calls inside helpers.
        pending = sorted(call_targets - set(exports_int))
        while pending:
            target = pending.pop(0)
            if target in worked or target in fn_index:
                continue
            worked.add(target)
            # Compute end_hint: next known boundary (export or already-
            # disassembled helper or end-of-code), taking only addresses
            # strictly greater than this one.
            boundaries = sorted({
                *exports_int,
                *worked,
                *call_targets,
                len(code),
            } - {target})
            end_hint = len(code)
            for b in boundaries:
                if b > target:
                    end_hint = b
                    break
            lines.append("")
            lines.append(f"fn @ {target:#06x}  ; helper (call-target)")
            before = len(call_targets)
            disasm_function(target, end_hint)
            # If disassembling this helper revealed more call targets,
            # add them to the worklist.
            new_targets = call_targets - worked - set(exports_int) - set(pending)
            if new_targets:
                pending.extend(sorted(new_targets))
                pending.sort()

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


def _save_poem(raw: bytes, base: Path) -> dict:
    """ClassID 2043 - `CPoem`: UTF-16LE text block consumed by the menu's
    poem scroller (`CPoemScroller`).

    Recovered via factory→vftable→RTTI: classID 2043's factory at
    `0x00409ab0` builds a 28-byte object whose primary vftable's
    TypeDescriptor decodes to `.?AVCPoem@@`. So this resource is a
    self-contained Czech poem (or chapter title) -- not a "static text
    table". The engine-side localised string pool is a separate
    singleton inside `bulanci.exe`; see
    `../../ghidra_analysis/static_texts.md`.

    Wire format::

        u32 charCount, char[charCount] UTF-16LE chars

    Inline markup language (per-line, parsed by
    `TextShaper_LayOutAndRender @ 0x004375e0`; full description in
    `../../ghidra_analysis/main_menu.md` §9.3):

    - `\\x0a` (`\\n`): line break. There is no auto-wrap -- every visual
      line is delimited by an explicit `\\n`.
    - `\\x01` at column 0: render the rest of the line CENTER aligned.
    - `\\x02` at column 0: render the rest of the line RIGHT aligned.
    - No prefix: default LEFT alignment.
    - `\\x09` (`\\t`): horizontal tab, expanded to 4 spaces.

    A typical poem therefore reads::

        \\1Title line\\n               <- centered
        Verse line\\n                  <- left
        ... more verses ...
        \\2by Author\\n                <- right-aligned signature
        \\0                            <- (optional) terminator

    The companion `<base>.bin` is the byte-faithful wire form (keep it
    if you need a round-trip source). The `<base>.txt` we emit here is
    the human-readable rendering: each `\\x01`/`\\x02` line is prefixed
    with `[CENTER]` / `[RIGHT]`, tabs are expanded to four spaces, and
    `\\x00` terminators are stripped. The manifest gets a structured
    `lines: [{align, text}]` array plus a `markers` tally.
    """
    meta: dict = {"friendlyFile": None}
    if len(raw) < 4:
        meta["warning"] = "Poem: too short"
        return meta
    char_count = struct.unpack_from("<I", raw, 0)[0]
    if char_count < 0 or 4 + char_count * 2 > len(raw):
        meta["warning"] = f"Poem: implausible charCount {char_count}"
        return meta
    chars = raw[4 : 4 + char_count * 2].decode("utf-16-le", errors="replace")

    raw_text = chars.rstrip("\x00")
    parsed_lines: List[dict] = []
    markers = {"center": 0, "right": 0, "tab": 0, "newline": 0}
    for line in raw_text.split("\n"):
        align = "left"
        body = line
        if body.startswith("\x01"):
            align = "center"
            body = body[1:]
            markers["center"] += 1
        elif body.startswith("\x02"):
            align = "right"
            body = body[1:]
            markers["right"] += 1
        if "\t" in body:
            markers["tab"] += body.count("\t")
        body_expanded = body.replace("\t", "    ")
        parsed_lines.append({"align": align, "text": body_expanded})
    markers["newline"] = max(0, len(parsed_lines) - 1)

    meta["text"] = {
        "charCount": char_count,
        "lineCount": len(parsed_lines),
        "markers": markers,
        "lines": parsed_lines,
    }

    pretty_lines = []
    for entry in parsed_lines:
        text = entry["text"]
        tag = entry["align"]
        if tag == "center":
            pretty_lines.append(f"[CENTER] {text}".rstrip())
        elif tag == "right":
            pretty_lines.append(f"[RIGHT]  {text}".rstrip())
        else:
            pretty_lines.append(text.rstrip())
    out_txt = base.with_suffix(".txt")
    out_txt.write_text("\n".join(pretty_lines).rstrip() + "\n", encoding="utf-8")
    meta["friendlyFile"] = out_txt.name

    return meta


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
# little-endian u32s (= 0x2c bytes). The header bytes at +0x24..+0x2c
# double as the first frame's 5-byte header — see _walk_bitmap_sprite_frames.
BITMAP_SPRITE_HEADER_SIZE = 0x2c
BITMAP_SPRITE_FIELDS = 11
# `channels` is 3 on every sample in the master pack (BGR palette triplets
# arrive via inner opcode 0x09; pixels are 8bpp palette indices).
BITMAP_SPRITE_EXPECTED_CHANNELS = 3
# `inMemSize` (header[7]) commonly equals the `CBulPicture` allocation
# size (0x470 bytes from `CBulPicture::CBulPicture_Create` @ 0x0040eb30).
# Other observed values are extra runtime buffers above the base struct.
BITMAP_SPRITE_CBULPICTURE_SIZE = 0x470
# Each outer-FRAME header is `u32 frameSize, u8 numInnerChunks`. The first
# frame's 5-byte header starts at file offset 0x24 (i.e. the last 4 bytes
# of `encodedSize3` + the low byte of `packed`); this overlap makes the
# decoder reuse the same 9 file-header bytes for two distinct purposes.
BITMAP_SPRITE_FIRST_CHUNK_OFFSET = 0x24
BITMAP_SPRITE_CHUNK_HEADER_SIZE = 5
# `flags` (header[8]) is `len(frames) - 1` on every observed sample.
BITMAP_SPRITE_FLAGS_IS_CHUNK_COUNT_MINUS_1 = True
# Empty terminator frames always have frameSize == 5 and numInnerChunks == 0.
BITMAP_SPRITE_EMPTY_CHUNK_TAG = 0

# Inner-chunk opcodes — the dispatch table inside
# ``CDSFlxFile::FUN_00432c60`` (the per-frame switch). Empirically these
# are the only opcodes the master-pack ever emits.
BITMAP_SPRITE_INNER_OP_RLE = 0x00          # FUN_00432740: index RLE into pixel buf
BITMAP_SPRITE_INNER_OP_DELTA = 0x04        # FUN_00432780: skip-delta RLE
BITMAP_SPRITE_INNER_OP_MEMCPY = 0x08       # FUN_004327e0: raw memcpy
BITMAP_SPRITE_INNER_OP_PALETTE = 0x09      # FUN_00432800: BGR palette update
BITMAP_SPRITE_INNER_OP_RECT = 0x0A         # FUN_00436e80(x:i16, y:i16) — frame rect
BITMAP_SPRITE_INNER_OP_REGIONS = 0x0B      # FUN_00432850: <=32 sub-rect table
BITMAP_SPRITE_INNER_OP_TIME = 0x0C         # NotifyFrameTime(u16) @ 0x00436ef0 — frame timing
BITMAP_SPRITE_INNER_OP_TRANSPARENT = 0x0D  # ctx[+0x18] = u8 transparent index
BITMAP_SPRITE_INNER_OP_RLE_ALT = 0x0E      # FUN_00432740 into alt/mask buffer
BITMAP_SPRITE_INNER_OP_DELTA_ALT = 0x0F    # FUN_00432780 into alt/mask buffer

BITMAP_SPRITE_INNER_OP_NAMES = {
    0x00: "rle",
    0x04: "delta",
    0x08: "memcpy",
    0x09: "palette",
    0x0A: "rect",
    0x0B: "regions",
    0x0C: "time",
    0x0D: "transparent",
    0x0E: "rleAlt",
    0x0F: "deltaAlt",
}


def _walk_bitmap_sprite_frames(raw: bytes) -> tuple[list[dict], Optional[str]]:
    """Walk the BitmapSprite outer-frame chain (Ghidra-verified).

    The outer chunks are *animation frames*: a 5-byte header
    ``(u32 frameSize, u8 numInnerChunks)`` followed by ``numInnerChunks``
    back-to-back inner chunks. Each inner chunk has its own 5-byte header
    ``(u32 chunkSize, u8 opcode)`` and a body of ``chunkSize - 5`` bytes.

    Returns ``(frames, error)``. Each ``frames[i]`` dict carries:

    * ``offset``        : file offset of the outer 5-byte header
    * ``frameSize``     : total bytes including the outer 5-byte header
    * ``numInner``      : number of inner chunks (= the legacy ``tag`` byte)
    * ``innerOpcodes``  : list of opcode-byte ints, in order
    * ``innerChunks``   : list of ``{offset, size, opcode, dataSize}``

    Structural invariants (verified across all 130 master-pack samples):

    * The first frame's 5-byte header starts at offset 0x24 of the resource
      (the last 4 bytes of ``encodedSize3`` + the low byte of ``packed``).
    * Frames pack back-to-back and the chain terminates exactly at EOF.
    * ``flags + 1`` equals the total frame count on every sample.
    * Empty frames (``numInner == 0``) always have ``frameSize == 5`` and
      mean "reuse the previous frame's bitmap" in the animation timeline.
    * The dispatch on inner opcodes goes through ``CDSFlxFile::FUN_00432c60``.
    """
    frames: list[dict] = []
    pos = BITMAP_SPRITE_FIRST_CHUNK_OFFSET
    end = len(raw)
    while pos + BITMAP_SPRITE_CHUNK_HEADER_SIZE <= end:
        fs = struct.unpack_from("<I", raw, pos)[0]
        num_inner = raw[pos + 4]
        if fs < BITMAP_SPRITE_CHUNK_HEADER_SIZE:
            return frames, f"frame at 0x{pos:x} has size {fs} (< {BITMAP_SPRITE_CHUNK_HEADER_SIZE})"
        if pos + fs > end:
            return frames, f"frame at 0x{pos:x} overruns EOF (fs={fs}, remaining={end - pos})"
        inner_chunks: list[dict] = []
        inner_pos = pos + BITMAP_SPRITE_CHUNK_HEADER_SIZE
        frame_end = pos + fs
        for _ in range(num_inner):
            if inner_pos + BITMAP_SPRITE_CHUNK_HEADER_SIZE > frame_end:
                return frames, (
                    f"inner-chunk header at 0x{inner_pos:x} overruns frame end"
                )
            ics = struct.unpack_from("<I", raw, inner_pos)[0]
            iop = raw[inner_pos + 4]
            if ics < BITMAP_SPRITE_CHUNK_HEADER_SIZE or inner_pos + ics > frame_end:
                return frames, (
                    f"inner chunk at 0x{inner_pos:x} has bad size {ics}"
                )
            inner_chunks.append({
                "offset": inner_pos,
                "size": ics,
                "opcode": iop,
                "dataSize": ics - BITMAP_SPRITE_CHUNK_HEADER_SIZE,
            })
            inner_pos += ics
        if inner_pos != frame_end and num_inner > 0:
            return frames, (
                f"inner chunks of frame at 0x{pos:x} end at 0x{inner_pos:x} "
                f"but frame ends at 0x{frame_end:x}"
            )
        frames.append({
            "offset": pos,
            # Keep the legacy field names so existing manifest consumers
            # (and the chunkTags histogram) continue to work — these names
            # are also retained inside ``chunks`` for backwards compat.
            "chunkSize": fs,
            "tag": num_inner,
            "dataSize": fs - BITMAP_SPRITE_CHUNK_HEADER_SIZE,
            "numInner": num_inner,
            "innerOpcodes": [c["opcode"] for c in inner_chunks],
            "innerChunks": inner_chunks,
        })
        pos += fs
        if pos == end:
            return frames, None
    if pos != end:
        return frames, f"chain stopped mid-stream at 0x{pos:x} (end=0x{end:x})"
    return frames, None


# Backwards-compat alias — older call sites still ask for "chunks".
_walk_bitmap_sprite_chunks = _walk_bitmap_sprite_frames


# ----------------------------------------------------------------------
# CDSFlxFile inner-opcode decoders (port of ``CDSFlxFile::FUN_00432740``,
# ``FUN_00432780``, ``FUN_00432800`` from bulanci.exe).
# ----------------------------------------------------------------------

def _flxrle_decode(body: bytes) -> bytes:
    """Decode a CDSFlxFile RLE stream (port of FUN_00432740 @ 0x00432740).

    Opcode set, single-byte ``N`` read at the head of each packet:

      * ``N == 0x00`` (4-byte opcode):
          read ``u16 count``, ``u8 value``; emit ``(count + 1)`` copies of value.
      * ``N == 0x01..0x7f`` (2-byte opcode):
          read ``u8 value``; emit ``(N + 1)`` copies of value.
      * ``N == 0x80..0xff`` (``1 + L`` byte opcode):
          ``L = 256 - N`` literal bytes follow; copy them verbatim.

    The native function reads until ``source == end``; we mimic by
    consuming the whole input buffer. Used for inner opcode 0x00 (and the
    0x0e variant that writes into the alt/mask buffer).
    """
    out = bytearray()
    pos = 0
    end = len(body)
    while pos < end:
        n = body[pos]
        pos += 1
        if n == 0:
            if pos + 3 > end:
                raise ValueError("flxrle: long-run header underflow")
            count = body[pos] | (body[pos + 1] << 8)
            value = body[pos + 2]
            pos += 3
            out.extend([value] * (count + 1))
        elif n < 0x80:
            if pos >= end:
                raise ValueError("flxrle: short-run value underflow")
            value = body[pos]
            pos += 1
            out.extend([value] * (n + 1))
        else:
            cnt = 256 - n
            if pos + cnt > end:
                raise ValueError("flxrle: literal underflow")
            out.extend(body[pos : pos + cnt])
            pos += cnt
    return bytes(out)


def _flxdelta_apply(body: bytes, working: bytearray, start_pos: int = 0) -> int:
    """Apply a CDSFlxFile delta stream to ``working`` starting at byte
    ``start_pos`` (port of ``FUN_00432780`` @ 0x00432780).

    Packet structure (read repeatedly until input is consumed):

      * Skip prefix:
          ``skip_byte = body[pos]``; advance ``pos``. If ``skip_byte == 0xff``
          then read ``u16 long_skip`` from the next 2 bytes and use that
          as the skip count (and advance ``pos`` by 2 more).
          The destination pointer advances by ``skip`` bytes (existing
          pixels are kept).
      * Op byte:
          - ``op == 0x00``                    → no data; loop back to read
                                                the next skip+op pair.
          - ``op == 0x01..0x7f``              → 2 more bytes (``op + 1``
                                                copies of next value byte).
          - ``op == 0x80`` + ``i16``          → if ``i16 >= 0``: long run
                                                ``(i16 + 1)`` copies of next
                                                value byte; if ``i16 < 0``:
                                                literal of ``-i16`` bytes.
          - ``op == 0x81..0xff``              → literal of ``(256 - op)``
                                                bytes.

    Returns the new destination cursor (= ``start_pos`` + accumulated
    skips and emits). Mutates ``working`` in place.
    """
    pos = 0
    dst = start_pos
    end = len(body)
    cap = len(working)
    while pos < end:
        # ---- skip prefix ------------------------------------------------
        skip_byte = body[pos]
        pos += 1
        if skip_byte == 0xFF:
            if pos + 2 > end:
                raise ValueError("flxdelta: long-skip header underflow")
            skip = body[pos] | (body[pos + 1] << 8)
            pos += 2
        else:
            skip = skip_byte
        dst += skip
        if pos >= end:
            break
        # ---- op byte ---------------------------------------------------
        op = body[pos]
        pos += 1
        if op == 0x00:
            # No data; loop back for the next skip+op pair.
            continue
        if op == 0x80:
            if pos + 2 > end:
                raise ValueError("flxdelta: long-op header underflow")
            raw_u16 = body[pos] | (body[pos + 1] << 8)
            pos += 2
            if raw_u16 < 0x8000:
                # Positive long count -> long RUN of (raw_u16 + 1) of next value.
                if pos >= end:
                    raise ValueError("flxdelta: long-run value underflow")
                value = body[pos]
                pos += 1
                count = raw_u16 + 1
                if dst + count > cap:
                    raise ValueError(
                        f"flxdelta: long-run overruns buffer "
                        f"({dst}+{count} > {cap})"
                    )
                for i in range(count):
                    working[dst + i] = value
                dst += count
            else:
                # Negative i16 -> literal of -i16 bytes (no value byte).
                count = (~raw_u16 + 1) & 0xFFFF
                if pos + count > end:
                    raise ValueError("flxdelta: long literal underflow")
                if dst + count > cap:
                    raise ValueError(
                        f"flxdelta: long literal overruns buffer "
                        f"({dst}+{count} > {cap})"
                    )
                working[dst : dst + count] = body[pos : pos + count]
                pos += count
                dst += count
            continue
        if op < 0x80:
            if pos >= end:
                raise ValueError("flxdelta: short-run value underflow")
            value = body[pos]
            pos += 1
            count = op + 1
            if dst + count > cap:
                raise ValueError(
                    f"flxdelta: short-run overruns buffer "
                    f"({dst}+{count} > {cap})"
                )
            for i in range(count):
                working[dst + i] = value
            dst += count
        else:
            count = 256 - op
            if pos + count > end:
                raise ValueError("flxdelta: short literal underflow")
            if dst + count > cap:
                raise ValueError(
                    f"flxdelta: short literal overruns buffer "
                    f"({dst}+{count} > {cap})"
                )
            working[dst : dst + count] = body[pos : pos + count]
            pos += count
            dst += count
    return dst


def _flxpalette_apply(body: bytes, palette: bytearray) -> int:
    """Apply a CDSFlxFile palette-update stream (port of
    ``CDSFlxFile::DecodePaletteRgb @ 0x00432800``).

    This is the **classic FLI/FLC COLOR_256 chunk shape** as used by
    Bulanci (FLX opcode 0x09). Packets repeat until ``body`` is fully
    consumed; the consumer maintains a cursor into the 256-entry
    palette buffer that PERSISTS across packets:

        u8 skip          ; advance cursor by `skip` entries
        u8 count_minus_1 ; write `count_minus_1 + 1` entries from cursor
        repeat (count_minus_1 + 1) times:
            u8 R
            u8 G
            u8 B          ; entry written to palette[cursor], cursor += 1

    Ghidra disassembly (``param_3`` is a ``u16*`` so ``+ skip * 2``
    advances by ``skip * 4`` bytes, i.e. ``skip`` BGRA entries):

        param_3 = param_3 + (uint)*param_2 * 2;   // cursor += skip
        param_2 = param_2 + 2;
        iVar2 = *pbVar1 + 1;                       // count = body[1]+1
        do {
            *param_3 = *(undefined2 *)param_2;     // dst[0..1] = src[0..1]
            *(byte *)(param_3 + 1) = param_2[2];   // dst[2]    = src[2]
            param_2 = param_2 + 3;
            param_3 = param_3 + 2;                 // cursor += 1 entry
        } while (--iVar2 != 0);

    The buffer is BGRA in memory but the FLX stream stores RGB in
    file order (per the FLI/FLC inheritance) and the decompile writes
    the bytes straight through, so on disk the byte at +0 of an entry
    is the **R** channel, +1 is **G**, +2 is **B** (despite the BGRA
    layout name). The PNG encoder accounts for that — see
    ``_encode_png_indexed``.

    ``palette`` is a 1024-byte buffer (256 entries * 4 bytes each).
    Returns the number of palette entries updated.

    Per the master-pack survey, the cursor is reset to 0 at the start
    of every palette write (each opcode 0x09 chunk gets a fresh cursor).
    Within a single chunk multiple packets accumulate via ``skip``.
    """
    pos = 0
    end = len(body)
    n_entries = 0
    cursor = 0
    while pos + 2 <= end:
        skip = body[pos]
        cnt = body[pos + 1] + 1
        pos += 2
        cursor += skip
        if pos + cnt * 3 > end:
            raise ValueError(
                f"flxpalette: triplets underflow "
                f"(pos={pos}, cnt={cnt}, end={end})"
            )
        for _ in range(cnt):
            if cursor >= 256:
                raise ValueError(
                    f"flxpalette: cursor {cursor} >= 256 (skip={skip}, cnt={cnt})"
                )
            base = cursor * 4
            palette[base + 0] = body[pos + 0]
            palette[base + 1] = body[pos + 1]
            palette[base + 2] = body[pos + 2]
            # palette[base + 3] (A) intentionally left unchanged
            pos += 3
            cursor += 1
            n_entries += 1
    return n_entries


# ----------------------------------------------------------------------
# Tiny self-contained PNG encoder (uses only zlib from stdlib). We emit
# 8bpp palette PNGs (PLTE + tRNS) — the natural representation for the
# native sprites since they're palette-indexed.
# ----------------------------------------------------------------------

def _png_chunk(name: bytes, data: bytes) -> bytes:
    crc = zlib.crc32(name + data) & 0xFFFFFFFF
    return struct.pack(">I", len(data)) + name + data + struct.pack(">I", crc)


def _encode_png_indexed(
    width: int,
    height: int,
    pixels: bytes,
    palette_bgra: bytes,
    transparent_indices: Optional[Iterable[int]],
) -> bytes:
    """Write an 8bpp indexed PNG. Palette is BGRA (1024 bytes) — we
    transpose to RGB for PNG's PLTE chunk and emit a tRNS chunk that
    flags every entry in ``transparent_indices`` as fully transparent.

    Accepts a single int, a set/list of ints, or None.
    """
    assert len(pixels) == width * height, "pixel count mismatch"
    assert len(palette_bgra) == 1024, "palette must be 256 BGRA entries"
    sig = b"\x89PNG\r\n\x1a\n"
    ihdr = struct.pack(">IIBBBBB", width, height, 8, 3, 0, 0, 0)  # 8bpp palette
    rgb = bytearray(256 * 3)
    for i in range(256):
        b = palette_bgra[i * 4 + 0]
        g = palette_bgra[i * 4 + 1]
        r = palette_bgra[i * 4 + 2]
        rgb[i * 3 + 0] = r
        rgb[i * 3 + 1] = g
        rgb[i * 3 + 2] = b
    # tRNS: alpha for each palette entry; default opaque, transparent
    # indices zero-alpha. We always emit length 256 to be safe.
    if transparent_indices is None:
        idx_iter: Iterable[int] = ()
    elif isinstance(transparent_indices, int):
        idx_iter = (transparent_indices,)
    else:
        idx_iter = transparent_indices
    trns_bytes = bytearray([0xFF] * 256)
    has_any = False
    for idx in idx_iter:
        if 0 <= idx < 256:
            trns_bytes[idx] = 0x00
            has_any = True
    trns_chunk = _png_chunk(b"tRNS", bytes(trns_bytes)) if has_any else b""
    # Filter byte 0x00 per scanline, then deflate.
    rows = bytearray()
    for y in range(height):
        rows.append(0x00)
        rows.extend(pixels[y * width : (y + 1) * width])
    idat = zlib.compress(bytes(rows), level=6)
    return (
        sig
        + _png_chunk(b"IHDR", ihdr)
        + _png_chunk(b"PLTE", bytes(rgb))
        + trns_chunk
        + _png_chunk(b"IDAT", idat)
        + _png_chunk(b"IEND", b"")
    )


def _encode_png_rgba(width: int, height: int, rgba: bytes) -> bytes:
    """Write a 32bpp RGBA PNG (color type 6). ``rgba`` is exactly
    ``width * height * 4`` bytes in R,G,B,A order, row-major.

    Used for the atlas image so each frame can be rendered with its
    own per-frame palette state — required because 71/130 master-pack
    BitmapSprites cycle their palette mid-animation.
    """
    assert len(rgba) == width * height * 4, "RGBA buffer size mismatch"
    sig = b"\x89PNG\r\n\x1a\n"
    ihdr = struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)
    rows = bytearray()
    stride = width * 4
    for y in range(height):
        rows.append(0x00)
        rows.extend(rgba[y * stride : (y + 1) * stride])
    idat = zlib.compress(bytes(rows), level=6)
    return (
        sig
        + _png_chunk(b"IHDR", ihdr)
        + _png_chunk(b"IDAT", idat)
        + _png_chunk(b"IEND", b"")
    )


def _lzw_encode_gif(data: bytes, min_code_size: int) -> bytes:
    """GIF-flavour LZW encode ``data`` into a stream of length-prefixed
    sub-blocks (no trailing 0x00 terminator — the caller appends it).

    Implements the textbook GIF89a variant:

    * Initial dictionary holds ``2^min_code_size`` single-byte codes
      0..N-1. Reserved codes ``clear = N`` and ``eoi = N + 1`` follow,
      so the first user-assigned code is ``N + 2``.
    * Initial output code width is ``min_code_size + 1`` bits; codes
      are packed LSB-first into the byte stream.
    * When a new dictionary entry would exceed the current code width
      capacity (and the width is still < 12), the width is bumped by
      one bit *before* emitting the next code.
    * When the dictionary reaches 4096 entries it is reset; a CLEAR
      code is emitted at the current width and parsing continues with
      a fresh dictionary and width = ``min_code_size + 1``.
    * Output bytes are wrapped in ≤255-byte sub-blocks; the caller
      appends the GIF block terminator (``0x00``).
    """
    clear_code = 1 << min_code_size
    eoi_code = clear_code + 1
    max_code = 1 << 12

    bit_buf = 0
    bit_len = 0
    raw = bytearray()

    def _emit(code: int, width: int) -> None:
        nonlocal bit_buf, bit_len
        bit_buf |= code << bit_len
        bit_len += width
        while bit_len >= 8:
            raw.append(bit_buf & 0xFF)
            bit_buf >>= 8
            bit_len -= 8

    dictionary: dict[bytes, int] = {bytes([i]): i for i in range(clear_code)}
    next_code = eoi_code + 1
    code_width = min_code_size + 1

    _emit(clear_code, code_width)

    prefix = b""
    for byte in data:
        candidate = prefix + bytes([byte])
        if candidate in dictionary:
            prefix = candidate
            continue
        _emit(dictionary[prefix], code_width)
        if next_code < max_code:
            dictionary[candidate] = next_code
            next_code += 1
            if next_code > (1 << code_width) and code_width < 12:
                code_width += 1
        else:
            _emit(clear_code, code_width)
            dictionary = {bytes([i]): i for i in range(clear_code)}
            next_code = eoi_code + 1
            code_width = min_code_size + 1
        prefix = bytes([byte])

    if prefix:
        _emit(dictionary[prefix], code_width)
    _emit(eoi_code, code_width)
    if bit_len > 0:
        raw.append(bit_buf & 0xFF)

    blocks = bytearray()
    for i in range(0, len(raw), 255):
        chunk = raw[i : i + 255]
        blocks.append(len(chunk))
        blocks.extend(chunk)
    return bytes(blocks)


def _encode_gif_animated(
    width: int,
    height: int,
    snapshots: list[bytes],
    palette_snapshots: list[bytes],
    transparent_index: Optional[int],
    delay_centisecs: int = 10,
) -> bytes:
    """Encode a looping animated GIF89a from indexed-color frames.

    Each frame ships with its own 256-entry local color table — the
    cheapest way to render the 71/130 master-pack sprites that cycle
    their palette mid-animation correctly. ``transparent_index``, when
    set, is marked transparent in every frame's Graphic Control
    Extension; per-frame disposal is set to *restore-to-background* so
    the empty pixels of one frame don't bleed into the next.

    The Netscape ``NETSCAPE2.0`` looping extension is emitted with a
    zero loop count (infinite loop).
    """
    out = bytearray()
    out += b"GIF89a"
    out += struct.pack("<HHBBB", width, height, 0x00, 0, 0)
    out += (
        b"\x21\xFF\x0BNETSCAPE2.0\x03\x01"
        + struct.pack("<H", 0)
        + b"\x00"
    )

    has_alpha = transparent_index is not None and 0 <= transparent_index < 256
    t_idx = transparent_index if has_alpha else 0
    # GCE packed byte: reserved(3) | disposal(3)=2 | user_input(1)=0 | transparent(1)
    gce_packed = (2 << 2) | (0x01 if has_alpha else 0x00)

    for snap, pal_bgra in zip(snapshots, palette_snapshots):
        out += b"\x21\xF9\x04"
        out += struct.pack("<BHBB", gce_packed, max(1, delay_centisecs), t_idx, 0x00)
        out += b"\x2C"
        out += struct.pack("<HHHH", 0, 0, width, height)
        out += bytes([0x80 | 0x07])
        lct = bytearray(768)
        for i in range(256):
            b = pal_bgra[i * 4 + 0]
            g = pal_bgra[i * 4 + 1]
            r = pal_bgra[i * 4 + 2]
            lct[i * 3 + 0] = r
            lct[i * 3 + 1] = g
            lct[i * 3 + 2] = b
        out += bytes(lct)
        out += b"\x08"
        out += _lzw_encode_gif(bytes(snap), 8)
        out += b"\x00"

    out += b"\x3B"
    return bytes(out)


def _palette_indexed_to_rgba(
    pixels: bytes,
    palette_bgra: bytes,
    transparent_indices: Iterable[int],
) -> bytes:
    """Apply a 256-entry BGRA palette to a buffer of indexed bytes,
    producing R,G,B,A bytes (alpha 0 for ``transparent_indices``)."""
    alpha = bytearray([0xFF] * 256)
    for idx in transparent_indices:
        if 0 <= idx < 256:
            alpha[idx] = 0x00
    out = bytearray(len(pixels) * 4)
    for i, p in enumerate(pixels):
        base = p * 4
        out[i * 4 + 0] = palette_bgra[base + 2]  # R
        out[i * 4 + 1] = palette_bgra[base + 1]  # G
        out[i * 4 + 2] = palette_bgra[base + 0]  # B
        out[i * 4 + 3] = alpha[p]
    return bytes(out)


def _save_bitmap_sprite(
    raw: bytes,
    base: Path,
    inherited_palette: Optional[bytes] = None,
    inherited_palette_source_id: Optional[int] = None,
    sprite_state: Optional[dict] = None,
) -> dict:
    """ClassID 52 - native sprite/animation container.

    Container layout (header + frame stream + per-opcode pixel decoders),
    Ghidra-verified against the ClassID 52 factory at ``0x00432a50``, the
    ``CDSFlxFile`` dispatcher at ``0x00432c60``, and the per-opcode
    decoders at ``0x00432740`` / ``0x00432780`` / ``0x00432800``:

    * 11 little-endian u32s (= 0x2c bytes) of metadata, fields below.
    * A **frame stream** spanning the rest of the file. The first
      frame's 5-byte header ``(u32 frameSize, u8 numInner)`` overlaps the
      file header at offsets 0x24..0x28: ``encodedSize3`` IS the frame
      size and the low byte of ``packed`` IS the per-frame inner-chunk
      count. Subsequent frames have plain headers. Frames pack
      back-to-back and the chain terminates exactly at end-of-file.
      Frames with ``numInner == 0`` are always 5 bytes long ("reuse
      previous frame" hints); frames with ``numInner >= 1`` carry that
      many ``(u32 chunkSize, u8 opcode, byte[chunkSize-5] body)`` inner
      chunks whose opcodes are dispatched per
      ``../../ghidra_analysis/flx_file_format.md``.

    The 80-byte ClassID 52 handle returned by the factory doesn't itself
    hold pixel data — it's a stream descriptor. The actual pixels live in
    a separately-allocated ``CBulPicture`` (1136 bytes, sibling factory at
    ``0x0040eb30``) that is bound to the handle the first time the sprite
    is drawn. The CBulPicture layout (see
    `../../ghidra_analysis/sprite_container.md`) confirms
    the runtime ends up with:
        +0x68            uchar* pixel data
        +0x6c..+0x46b    256-entry RGBA palette (1024 bytes)
        +0x46d           transparent color index (0xFF = no transparency)

    Header fields (legacy names — empirically the "height" field is the
    bitmap *width* and the "frameCount" field is the bitmap *height*;
    see ``_decode_bitmap_sprite_frames`` for the verification):

        +0x00 u32 totalSize       == len(raw)
        +0x04 u32 encodedSize     bytes of "primary" encoded buffer
                                  (= length of the first chunk's body when
                                  measured from offset 0x29)
        +0x08 u32 encodedSize2    duplicate of encodedSize on every sample
                                  except a handful of compound atlases
        +0x0c u32 width           0 = compound atlas, 0xFFFFFFFF = "fit"
        +0x10 u32 height          **(bitmap width in pixels)**
        +0x14 u32 frameCount      **(bitmap height in pixels)**
        +0x18 u32 channels        3 for every observed file (BGR palette)
        +0x1c u32 inMemSize       runtime allocation hint (264..4473)
        +0x20 u32 flags           **= frameCount - 1 on every sample.**
                                  Number of animation frames minus one.
        +0x24 u32 encodedSize3    chunk-0 size (overlaps with first chunk
                                  header; always == encodedSize)
        +0x28 u32 packed          (numInner<<0) | (firstThreeBytesOfData<<8).
                                  Low byte = first frame's numInner (1..3);
                                  high 3 bytes = first 3 bytes of inner
                                  chunk 0's body.
        +0x2c ...                 frame-0 body + frame-1, frame-2, ...
                                  back-to-back to end-of-file.

    Files that share width/height/frameCount/encodedSize/flags but differ
    in `totalSize` are different recolours of the same base animation; the
    bytes that differ across recolour groups are isolated palette indices
    inside chunk bodies — confirming the encoded payload is a byte-level
    RLE rather than an entropy-coded stream. The 41 master-pack recolour
    variants that don't carry an inline palette inherit it from the
    most-recently-loaded inline-palette sibling — the engine keeps the
    FLX palette buffer live across consecutive sprite loads in the same
    render context, and the unpacker mirrors that lookup via the
    ``sprite_state`` ambient-palette plumbed in from
    :func:`save_resource`. The atlas sidecar records the provenance in
    ``paletteSource = "inherited"`` and ``inheritedFrom = <sibling ID>``;
    only sprites with neither an inline palette nor any sibling to
    inherit from fall back to a 256-step grayscale ramp.

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

    # --- frame stream inventory ------------------------------------------
    # Walk the outer frame chain starting from the embedded first-frame
    # header at offset 0x24. Cheap (no pixel decode) and gives the
    # manifest a precise structural fingerprint that survives recolour
    # edits.
    frames, frame_error = _walk_bitmap_sprite_frames(raw)
    if frames:
        tag_histogram: dict = {}
        inner_op_histogram: dict = {}
        for fr in frames:
            tag_histogram[fr["tag"]] = tag_histogram.get(fr["tag"], 0) + 1
            for op in fr["innerOpcodes"]:
                inner_op_histogram[op] = inner_op_histogram.get(op, 0) + 1
        sprite["chunkCount"] = len(frames)
        sprite["chunkTags"] = {str(t): n for t, n in sorted(tag_histogram.items())}
        sprite["chunks"] = frames
        sprite["innerOpcodes"] = {
            f"0x{op:02x}": n for op, n in sorted(inner_op_histogram.items())
        }
        # `flags == chunkCount - 1` is a Ghidra/empirical invariant on
        # every 130/130 master-pack samples. Surfacing the violation here
        # makes new format variants immediately visible in the manifest.
        if BITMAP_SPRITE_FLAGS_IS_CHUNK_COUNT_MINUS_1 and fields[8] + 1 != len(frames):
            warnings.append(
                f"flags {fields[8]} + 1 != chunkCount {len(frames)}"
            )
    if frame_error is not None:
        warnings.append(f"frame-walk: {frame_error}")

    # --- pixel decoding --------------------------------------------------
    # Apply the per-opcode decoders we ported from CDSFlxFile and emit
    # one PNG per non-empty animation frame, plus a horizontally-stacked
    # atlas. The runtime bitmap layout is **row-major** with rows of
    # ``fields[4]`` bytes and ``fields[5]`` rows total — this was
    # verified across the entire master pack by:
    #
    #   1. Every keyframe RLE (inner opcode 0x00) decodes to exactly
    #      ``fields[4] * fields[5]`` bytes (538/538 samples).
    #   2. Rendering the resulting buffer as `fields[4] wide × fields[5]
    #      tall` yields recognisable game sprites (characters, vehicles,
    #      weapons). Both other orientations produce noise or rotated
    #      content.
    #
    # The original "height"/"frameCount" header labels turn out to be
    # the bitmap's width and height respectively — we keep the legacy
    # names in the manifest (for stability) but use the empirical
    # mapping when actually drawing pixels.
    bitmap_w = fields[4]
    bitmap_h = fields[5]
    if (
        frames
        and frame_error is None
        and 0 < bitmap_w <= 4096
        and 0 < bitmap_h <= 4096
        and bitmap_w * bitmap_h <= 1 << 22
    ):
        decoded = _decode_bitmap_sprite_frames(
            raw,
            frames,
            bitmap_w,
            bitmap_h,
            base,
            warnings,
            inherited_palette=inherited_palette,
            inherited_palette_source_id=inherited_palette_source_id,
        )
        if decoded:
            sprite["decoded"] = decoded
            # Propagate the final palette upstream so the next sibling
            # sprite without its own opcode-0x09 palette can inherit
            # it. The engine keeps the palette buffer live across
            # consecutive sprite loads in the same render context;
            # ``sprite_state`` mirrors that ambient store.
            if sprite_state is not None:
                final_pal = decoded.pop("_finalPalette", None)
                if final_pal is not None and decoded.get(
                    "paletteSource"
                ) == "inline":
                    sprite_state["ambient"] = bytes(final_pal)
                    sprite_state["sourceId"] = decoded.get("_sourceId")
                # The ``_*`` private keys never go into the manifest.
                decoded.pop("_sourceId", None)

    if warnings:
        sprite["sanityWarnings"] = warnings
    return meta


def _summarise_motion_path(
    per_frame: list[dict],
    ring_frame_index: Optional[int],
    frames_stored: int,
) -> dict:
    """Roll up the per-frame ``move`` deltas (FLX opcode 0x0A, fed to
    :func:`CDSFlxFile::NotifyMove` @ ``0x00436e80``) into a single
    summary block consumers can use without re-walking the per-frame
    array.

    The CGunMouse idle paths (BitmapSprite resources ``65801``,
    ``65802``, ``65803``) are the marquee users of opcode 0x0A — the
    cursor's "inertia" is in fact a sequence of these (Δx, Δy) deltas
    looped back to (0, 0) by a closing ring frame. Surfacing the
    cumulative trajectory makes it trivial for the client to validate
    closure, render the path for debugging, and embed the table as a
    Rust ``const`` without re-doing the arithmetic.

    Returned shape::

        {
            "enabled": bool,                  # True if any frame moves
            "frameCountWithMove": int,
            "framesWithMove": [<frame index>, ...],
            "perFrameAbsolutePosition": [
                {"index": int, "dx": int, "dy": int, "x": int, "y": int},
                ...
            ],
            "netDelta": [sum_dx, sum_dy],     # both must be 0 for a loop
            "closedLoop": bool,
            "boundingBox": {"minX","minY","maxX","maxY"},
            "totalManhattanLength": int,      # sum |dx| + |dy|
            "ringFrameIncluded": bool,        # True if ring contributes
        }

    The cumulative trajectory walks **all** stored frames (atlas frames
    + ring frame, if present) in playback order. Frames without a
    ``move`` op contribute (0, 0) — that matches the engine, where
    ``NotifyMove`` is only fired when opcode 0x0A is present and the
    consumer's accumulator otherwise stays put. The starting position
    is always (0, 0): the deltas are relative, not absolute.
    """
    enabled = any("move" in slot for slot in per_frame)
    perf: list[dict] = []
    x = 0
    y = 0
    sum_dx = 0
    sum_dy = 0
    min_x = 0
    min_y = 0
    max_x = 0
    max_y = 0
    manhattan = 0
    frames_with_move: list[int] = []
    ring_contributes = False
    # Iterate every stored frame (including the ring frame) so the
    # path closes back to start exactly the way the runtime sees it.
    # ``per_frame`` is indexed by stored-frame index so we can read
    # it directly without juggling atlas vs. ring partitions.
    for fi in range(frames_stored):
        slot = per_frame[fi] if fi < len(per_frame) else {}
        mv = slot.get("move")
        if mv is not None:
            dx, dy = int(mv[0]), int(mv[1])
            frames_with_move.append(fi)
            if ring_frame_index is not None and fi == ring_frame_index:
                ring_contributes = True
        else:
            dx, dy = 0, 0
        x += dx
        y += dy
        sum_dx += dx
        sum_dy += dy
        manhattan += abs(dx) + abs(dy)
        if x < min_x:
            min_x = x
        if y < min_y:
            min_y = y
        if x > max_x:
            max_x = x
        if y > max_y:
            max_y = y
        perf.append(
            {
                "index": fi,
                "dx": dx,
                "dy": dy,
                "x": x,
                "y": y,
            }
        )
    return {
        "enabled": enabled,
        "frameCountWithMove": len(frames_with_move),
        "framesWithMove": frames_with_move,
        "perFrameAbsolutePosition": perf,
        "netDelta": [sum_dx, sum_dy],
        "closedLoop": enabled and sum_dx == 0 and sum_dy == 0,
        "boundingBox": {
            "minX": min_x,
            "minY": min_y,
            "maxX": max_x,
            "maxY": max_y,
        },
        "totalManhattanLength": manhattan,
        "ringFrameIncluded": ring_contributes,
    }


def _decode_bitmap_sprite_frames(
    raw: bytes,
    frames: list[dict],
    bitmap_w: int,
    bitmap_h: int,
    base: Path,
    warnings: list,
    inherited_palette: Optional[bytes] = None,
    inherited_palette_source_id: Optional[int] = None,
) -> Optional[dict]:
    """Apply all inner-opcode decoders frame by frame and emit a single
    horizontally-stacked atlas PNG plus a ``.atlas.json`` sidecar
    describing the frame strip (frame size, count, per-frame
    NotifyMove/duration/regions/etc.).

    ``inherited_palette`` (optional) — a 1024-byte BGRA palette
    snapshot left by the most-recently-loaded sibling sprite that
    carried an inline FLX opcode-0x09 palette. The engine keeps the
    palette buffer live across consecutive sprite loads in the same
    render context, so a sprite with no inline palette inherits the
    sibling's final palette. Passing it here lets the unpacker render
    the recolour-variant sprites in their authentic colours instead
    of falling back to a grayscale ramp.

    ``inherited_palette_source_id`` (optional) — the resource ID of
    the sibling whose palette is being inherited. Recorded in the
    atlas sidecar's ``inheritedFrom`` field for provenance.

    Side effects: writes files next to ``base`` and sweeps any stale
    ``<stem>.frame*.png`` from previous unpacker runs. Returns a
    metadata dict suitable for inclusion in ``sprite['decoded']``
    (palette path, atlas path, summary stats). Returns ``None`` if any
    decoder raises (in which case a soft warning is appended). The
    returned dict also carries a private ``_finalPalette`` field
    (stripped before manifest serialisation) so the caller can update
    its ambient palette state for the next sibling.
    """
    pixel_total = bitmap_w * bitmap_h
    pixels = bytearray(pixel_total)
    alt = bytearray(pixel_total)
    have_inherited = (
        inherited_palette is not None and len(inherited_palette) == 1024
    )
    if have_inherited:
        # Seed the palette buffer from the inheriting sibling. The
        # opcode-0x09 handler still mutates it normally below — the
        # engine permits a sprite to seed-then-modify the inherited
        # palette mid-stream, though no master-pack sprite exercises
        # both behaviours simultaneously.
        palette = bytearray(inherited_palette)
    else:
        # init to (255,255,255,255) like CBulPicture +0x6c..+0x46b
        palette = bytearray([0xFF] * 1024)
    transparent_index: Optional[int] = None
    n_palette_writes = 0
    # One metadata dict per stored frame, in order. Populated below as
    # we walk each frame's inner chunks; any NotifyMove / frame-timing /
    # transparent-index / region ops fold into the same dict so the
    # atlas sidecar has a single entry per frame.
    per_frame: list[dict] = [
        {"index": fi, "ops": []} for fi in range(len(frames))
    ]
    out_dir = base.parent
    stem = base.name
    keyframe_seen = False

    try:
        # First pass: apply all opcodes to learn the palette + per-frame
        # metadata (transparency index, regions, timing). We DON'T
        # snapshot the bitmap here — the second pass
        # (``_redecode_for_atlas`` below) emits the atlas image from a
        # clean replay.
        for fr_idx, fr in enumerate(frames):
            slot = per_frame[fr_idx]
            ops_applied = slot["ops"]
            for ic in fr["innerChunks"]:
                op = ic["opcode"]
                body = raw[ic["offset"] + 5 : ic["offset"] + ic["size"]]
                if op == BITMAP_SPRITE_INNER_OP_RLE:
                    decoded = _flxrle_decode(body)
                    if len(decoded) != pixel_total:
                        raise ValueError(
                            f"frame {fr_idx} op=0x00: "
                            f"got {len(decoded)} pixels, want {pixel_total}"
                        )
                    pixels[:] = decoded
                    keyframe_seen = True
                    ops_applied.append("rle")
                elif op == BITMAP_SPRITE_INNER_OP_DELTA:
                    if not keyframe_seen:
                        warnings.append(
                            f"frame {fr_idx}: delta opcode 0x04 before any keyframe"
                        )
                    _flxdelta_apply(body, pixels)
                    ops_applied.append("delta")
                elif op == BITMAP_SPRITE_INNER_OP_MEMCPY:
                    if len(body) > pixel_total:
                        raise ValueError(
                            f"frame {fr_idx} op=0x08: memcpy {len(body)} > buffer {pixel_total}"
                        )
                    pixels[: len(body)] = body
                    keyframe_seen = True
                    ops_applied.append("memcpy")
                elif op == BITMAP_SPRITE_INNER_OP_PALETTE:
                    n_palette_writes += _flxpalette_apply(body, palette)
                    ops_applied.append("palette")
                elif op == BITMAP_SPRITE_INNER_OP_RECT:
                    # FLX opcode 0x0A — NotifyMove(x, y). Per the engine
                    # trace this broadcasts a new (x, y) screen origin
                    # for the sprite at the start of this frame.
                    if len(body) >= 4:
                        mx, my = struct.unpack_from("<hh", body, 0)
                        slot["move"] = [int(mx), int(my)]
                    ops_applied.append("move")
                elif op == BITMAP_SPRITE_INNER_OP_REGIONS:
                    # FLX opcode 0x0B carries up to 32 5-byte records
                    # ``(u8 kind, i16 x, i16 y)``. The engine fans these
                    # out via NotifyRegionList (0x00436eb0); subscribers
                    # include the script VM, which raises
                    # ``OnBitmapEvt(slot, evt)`` with ``evt = kind``. In
                    # the 130-sprite master pack every record carries
                    # ``kind=0`` and the (x, y) varies per frame — i.e.
                    # a per-frame anchor/attachment point.
                    events: list[dict] = []
                    rec_count = len(body) // 5
                    for ri in range(rec_count):
                        kind, ex, ey = struct.unpack_from("<Bhh", body, ri * 5)
                        events.append(
                            {"kind": int(kind), "x": int(ex), "y": int(ey)}
                        )
                    if events:
                        slot["events"] = events
                    ops_applied.append("regions")
                elif op == BITMAP_SPRITE_INNER_OP_TIME:
                    # FLX opcode 0x0C carries a single ``u16`` value,
                    # broadcast via ``BroadcastFrameTimeHint @
                    # 0x00436ef0`` to the consumer's per-instance
                    # subscriber list (slot 4). **It is NOT a per-frame
                    # timing override** for the track manager — an
                    # exhaustive sweep of the binary's stores to
                    # ``track_manager+0x44`` shows exactly one writer
                    # (``TM_SetFrameDelayOverrideMs @ 0x00439720``,
                    # fed by the construction-time speed formula
                    # ``4725 / speed`` ms), and the
                    # ``BroadcastFrameTimeHint`` fan-out callgraph
                    # does NOT reach it. The raw u16 values in the
                    # 130-sprite master pack are mostly ``0`` and
                    # ``1`` — sub-perceptible as ms — corroborating
                    # that this opcode is a side-channel notification,
                    # likely a script-VM cadence hook whose listener
                    # has not yet been mapped. See ``anim_runtime.md``
                    # ("Frame timing and the clock").
                    if len(body) >= 2:
                        (t,) = struct.unpack_from("<H", body, 0)
                        slot["durationTicks"] = int(t)
                    ops_applied.append("time")
                elif op == BITMAP_SPRITE_INNER_OP_TRANSPARENT:
                    if body:
                        transparent_index = body[0]
                        slot["setTransparent"] = int(body[0])
                    ops_applied.append("transparent")
                elif op == BITMAP_SPRITE_INNER_OP_RLE_ALT:
                    decoded = _flxrle_decode(body)
                    if len(decoded) != pixel_total:
                        raise ValueError(
                            f"frame {fr_idx} op=0x0e: alt got {len(decoded)} bytes, "
                            f"want {pixel_total}"
                        )
                    alt[:] = decoded
                    ops_applied.append("rleAlt")
                elif op == BITMAP_SPRITE_INNER_OP_DELTA_ALT:
                    _flxdelta_apply(body, alt)
                    ops_applied.append("deltaAlt")
                else:
                    ops_applied.append(f"unknown(0x{op:02x})")
                    warnings.append(
                        f"frame {fr_idx}: unknown inner opcode 0x{op:02x}"
                    )
            if fr["numInner"] == 0:
                ops_applied.append("reuse")
    except Exception as exc:  # decoder failure shouldn't sink extraction
        warnings.append(f"decode: {exc}")
        return None

    # Animation-timing summary. The engine clock is real wall-clock
    # milliseconds (``g_dwElapsedMs = timeGetTime() - g_dwStartMs``
    # updated each frame in ``CDSApp_UpdateClock @ 0x0042e790``), and
    # the CDSObject scheduler that drives ``TM_AdvanceFrame @
    # 0x004399b0`` fires events when ``g_dwElapsedMs >= lastFire +
    # delay`` — so every delay value the track manager arms is in ms.
    # See ``anim_runtime.md`` "Frame timing and the clock" for the
    # full chain.
    #
    # The actual per-frame delay at runtime depends on the spawner:
    #   * If the construction site (e.g. ``CBulanek::FUN_0041e4b0``)
    #     passes a non-100 speed, ``TM_SetFrameDelayOverrideMs``
    #     writes ``trackMgr+0x44 = round(4725 / speed)`` ms, used
    #     for every frame of the sequence.
    #   * Otherwise (default speed=100) ``trackMgr+0x44 == -1`` and
    #     each frame is held for the Bresenham slice
    #     ``((f+1)*seq[0x10])/seq[0x14] - (f*seq[0x10])/seq[0x14]``
    #     ms, where ``seq[0x10]`` is the sequence's declared total
    #     duration and ``seq[0x14]`` is its frame count.
    #
    # FLX opcode 0x0C is a SIDE-CHANNEL notification, not a timing
    # override (see the opcode handler comment above and
    # ``anim_runtime.md``). We still surface the parsed u16 values
    # per frame so consumers can investigate, but we deliberately do
    # NOT roll them into an "effective duration" claim that would
    # mislead about playback cadence.
    explicit_time_frames = 0
    first_time_frame: Optional[int] = None
    for slot in per_frame:
        if "durationTicks" in slot:
            explicit_time_frames += 1
            if first_time_frame is None:
                first_time_frame = slot["index"]
            slot["explicitTime"] = True
    timing_block: dict = {
        "engineClockUnit": "ms",
        "engineClockSource": (
            "timeGetTime() polled by CDSApp_UpdateClock @ 0x0042e790; "
            "CDSObject scheduler fires events when "
            "g_dwElapsedMs >= lastFire + delay (see anim_runtime.md)."
        ),
        "speedFormulaMs": {
            "formula": "round(4725.0 / speed)",
            "neutralSpeed": 100,
            "numerator": 47.25,
            "denominatorReference": 100.0,
            "applies": (
                "When the spawner passes speed != 100 to the construction "
                "site (e.g. CBulanek::FUN_0041e4b0). At speed == 100 the "
                "formula is skipped and trackMgr+0x44 stays at -1, "
                "leaving the sequence-default cadence in effect."
            ),
        },
        "sequenceDefaultCadenceMs": {
            "formula": (
                "delay_per_frame = ((f+1)*seq[0x10])/seq[0x14] "
                "- (f*seq[0x10])/seq[0x14]"
            ),
            "totalDurationMs": None,
            "note": (
                "seq[0x14] = the sequence's frame count (see top-level "
                "'frameCount'). seq[0x10] = total clip duration in ms, "
                "set by the resource-pool wrapper at load time and NOT "
                "present in any natural alignment of the on-disk "
                "BitmapSprite header. Pinning it requires further "
                "Ghidra work on the resource-pool side."
            ),
        },
        "flxOpcode0x0c": {
            "purpose": "side-channel notification",
            "description": (
                "u16 broadcast via BroadcastFrameTimeHint @ "
                "0x00436ef0 to the consumer's per-instance subscriber "
                "list (slot 4). Does NOT drive frameDelayOverrideMs. "
                "Listener semantics not yet pinned; the master pack's "
                "values (mostly 0/1) are too small to be ms delays."
            ),
            "explicitDurationFrames": explicit_time_frames,
            "firstFrameWithOpcode": first_time_frame,
            "anyPresent": explicit_time_frames > 0,
        },
    }

    # Palette-source decision. The engine's runtime keeps a single
    # BGRA palette buffer (CBulPicture +0x6c..+0x46b) and FLX
    # opcode-0x09 either writes into it directly (89/130 master-pack
    # sprites) or — when absent — leaves whatever the previously
    # loaded sibling left behind. The unpacker mirrors that lookup
    # via ``inherited_palette``. Three terminal cases:
    #
    #   * ``inline``              — this sprite carried at least one
    #                                opcode-0x09 packet.
    #   * ``inherited``           — no opcode-0x09 was seen, but the
    #                                caller supplied a sibling palette
    #                                we can use authoritatively.
    #   * ``grayscale-fallback``  — no opcode-0x09 AND no inherited
    #                                palette (e.g. the first sprite in
    #                                a pack or a one-off without a
    #                                sibling). Falls back to a 256-step
    #                                grayscale ramp so the structure is
    #                                still visible.
    if n_palette_writes > 0:
        palette_source = "inline"
    elif have_inherited:
        palette_source = "inherited"
    else:
        palette_source = "grayscale-fallback"
    if palette_source == "grayscale-fallback":
        for i in range(256):
            palette[i * 4 + 0] = i
            palette[i * 4 + 1] = i
            palette[i * 4 + 2] = i
            palette[i * 4 + 3] = 0xFF

    # Re-decode every frame from scratch so we can take a clean snapshot
    # of the pixel buffer, the optional mask plane, AND the palette
    # state after each frame's ops are applied. Per-frame palette is
    # needed because 71/130 master-pack BitmapSprites update their
    # palette mid-animation (color-cycle/glow effects); using the final
    # cumulative palette for every frame would render frame 0..(k-1)
    # with colours that don't exist yet at that frame.
    # ``initial_palette`` seeds the per-frame snapshot buffer the same
    # way as the first pass — inherited sprites need it so their per-
    # frame palette snapshots reflect the sibling's colours.
    seed_palette: Optional[bytes] = (
        bytes(inherited_palette) if have_inherited else None
    )
    snapshots, mask_snapshots, palette_snapshots, mask_was_written = (
        _redecode_for_atlas(
            raw, frames, bitmap_w, bitmap_h, initial_palette=seed_palette
        )
    )
    # For sprites that landed on the grayscale-fallback branch the
    # per-frame palette snapshots will be all-white; overwrite each
    # with the grayscale ramp so the atlas RGBA emitter doesn't
    # render a solid white stripe.
    if palette_source == "grayscale-fallback":
        gray = bytearray(1024)
        for i in range(256):
            gray[i * 4 + 0] = i
            gray[i * 4 + 1] = i
            gray[i * 4 + 2] = i
            gray[i * 4 + 3] = 0xFF
        palette_snapshots = [bytearray(gray) for _ in palette_snapshots]

    # ---- Transparency resolution ---------------------------------------
    #
    # Per the FLX dispatcher (``CDSFlxFile::DecodeFrame @ 0x00432c60``)
    # the engine supports two distinct mechanisms:
    #
    # 1. **Mask plane** (opcodes 0x0E RLE-into-mask and 0x0F delta-into-
    #    mask, ``AllocMaskPlane @ 0x00436ff0`` — lazy w*h byte buffer at
    #    consumer +0x20). The masked blit kernels (dispatch table at
    #    ``BlitTable_Masked`` / ``DAT_004b0bc8``, e.g. ``BlitMasked`` @
    #    0x004414e0) gate per-pixel opacity on this mask. **39 of 130
    #    master-pack sprites use a mask plane** — that's the engine's
    #    authoritative per-pixel alpha.
    # 2. **Color-key index** (opcode 0x0D writes consumer +0x18; the
    #    sibling ``CBulPicture +0x46d`` byte feeds the color-key blit
    #    kernels at ``BlitTable_KeyAndMask`` / ``BlitTable_DestKey``).
    #    Only 3 of 130 master-pack sprites use this.
    #
    # The remaining ~88 sprites are rendered by the engine fully
    # opaque from the per-sprite blit's perspective; their visible
    # in-game transparency comes from a destination-side chroma key
    # (typical 8bpp DDraw pattern — the back-buffer the scene blits
    # into is created with a colour key, so opaque source blits
    # produce the right cut-out at flip time). We replicate that
    # behaviour synthetically by treating the per-sprite perimeter-RGB
    # palette index as a full chroma key (every matching pixel
    # transparent, NOT just edge-connected ones — that matches
    # ``BlitChromaKey``-style kernels @ 0x0043f470 in dispatch table
    # ``BlitTable_ChromaKey``). This correctly resolves "hollow
    # silhouette" encodings where the body fill reuses the same
    # palette index as the rectangular BG (sprite 65797 frames 0/1 etc.).
    bg_key_indices: set[int] = set()
    transparency_source = "none"
    reserved_alpha_idx: Optional[int] = None

    if mask_was_written:
        # Engine-authoritative per-pixel mask. Each mask snapshot holds
        # ``w*h`` bytes; the "transparent" value is whichever byte the
        # bitmap's corners agree on across frames (the BG is always
        # contiguous along the bitmap edge — the corners can't be
        # subject pixels without the engine's frame-overhang clipping
        # going wrong).
        reserved_alpha_idx = _apply_mask_plane_transparency(
            snapshots, mask_snapshots, bitmap_w, bitmap_h
        )
        transparency_source = (
            "mask-plane" if reserved_alpha_idx is not None else "none-no-free-slot"
        )
    else:
        bg_key_indices, transparency_source = _resolve_bg_key_indices(
            transparent_index, snapshots, bitmap_w, bitmap_h, palette
        )
        if bg_key_indices:
            reserved_alpha_idx = _apply_chromakey_transparency(
                snapshots, palette, bitmap_w, bitmap_h, bg_key_indices
            )
            if reserved_alpha_idx is None:
                transparency_source = "none-no-free-slot"

    transparent_indices: set[int] = (
        {reserved_alpha_idx} if reserved_alpha_idx is not None else set()
    )

    # ---- FLI/FLC "ring frame" detection ------------------------------
    #
    # Per ``CDSFlxFile::DecodeFrame @ 0x00432c60`` the engine treats the
    # frame stream as a circular buffer: when the cursor reaches
    # ``*(this + 0x40)`` (stream end) it wraps to ``*(this + 0x3c)``
    # (stream start). This means the last stored frame plays just like
    # any other, but per the FLI/FLC convention every master-pack
    # BitmapSprite (130 / 130) leaves that last frame's pixel buffer
    # equal to the first frame's. The "ring frame" is a re-keyframe (or
    # delta) whose sole purpose is to wrap the bitmap back to frame 0
    # so the loop is seamless.
    #
    # A true ring frame must reproduce **both** the color plane AND
    # the palette state of frame 0 — otherwise the rendered image
    # differs (e.g. palette-cycle sprites that finish their cycle on
    # the same color buffer as frame 0 but with a different palette
    # snapshot — that's a real visible final frame, not a ring).
    #
    # For visualisation the ring frame is a duplicate that adds
    # nothing — it makes the atlas read as if the animation has one
    # extra frame. We detect it so the atlas only shows the unique
    # part of the cycle; the sidecar still records the ring-frame
    # index + its op list under ``ringFrame`` so the data round-trips.
    ring_frame_index: Optional[int] = None
    if (
        len(snapshots) >= 2
        and bytes(snapshots[0]) == bytes(snapshots[-1])
        and bytes(palette_snapshots[0]) == bytes(palette_snapshots[-1])
    ):
        ring_frame_index = len(snapshots) - 1

    # Sweep any stale per-frame PNGs from a previous unpacker run —
    # we no longer emit them (the atlas + json sidecar + animated GIF
    # replace them). This keeps ``unpacked/overlay`` from accumulating
    # thousands of tiny PNG files between unpacker iterations.
    for stale in out_dir.glob(f"{stem}.frame*.png"):
        try:
            stale.unlink()
        except OSError:
            pass

    # The atlas stacks frames horizontally. Different frames can carry
    # incompatible palettes (palette-cycle sprites), so the atlas is
    # emitted as true-colour RGBA — each stripe is materialised
    # through its own per-frame palette, so the animation reads
    # correctly even for glow / fade sprites. The ring frame (if any)
    # is skipped from the atlas image and called out in the JSON
    # sidecar instead.
    atlas_count = len(snapshots) - (1 if ring_frame_index is not None else 0)
    atlas_path: Optional[Path] = None
    gif_path: Optional[Path] = None
    # Always sweep a previous run's GIF — keeps things tidy when a
    # sprite's decoder result changes or the GIF is intentionally
    # suppressed (atlas_count == 0).
    for stale in out_dir.glob(f"{stem}.atlas.gif"):
        try:
            stale.unlink()
        except OSError:
            pass
    if atlas_count > 0:
        atlas_w = bitmap_w * atlas_count
        atlas_rgba = bytearray(atlas_w * bitmap_h * 4)
        for fi in range(atlas_count):
            snap = snapshots[fi]
            pal = palette_snapshots[fi]
            stripe = _palette_indexed_to_rgba(
                bytes(snap), bytes(pal), transparent_indices
            )
            stride = bitmap_w * 4
            for y in range(bitmap_h):
                src_off = y * stride
                dst_off = (y * atlas_w + fi * bitmap_w) * 4
                atlas_rgba[dst_off : dst_off + stride] = stripe[
                    src_off : src_off + stride
                ]
        atlas_path = out_dir / f"{stem}.atlas.png"
        atlas_path.write_bytes(
            _encode_png_rgba(atlas_w, bitmap_h, bytes(atlas_rgba))
        )

        # Animated GIF sidecar — pure visualisation aid. One frame per
        # entry in ``snapshots[:atlas_count]`` (the ring frame is
        # dropped, the Netscape looping extension handles wraparound).
        # Indexed snapshots + per-frame local colour tables let the
        # palette-cycle sprites render correctly.
        gif_transparent_idx: Optional[int] = (
            next(iter(transparent_indices)) if transparent_indices else None
        )
        gif_path = out_dir / f"{stem}.atlas.gif"
        gif_path.write_bytes(
            _encode_gif_animated(
                bitmap_w,
                bitmap_h,
                [bytes(snap) for snap in snapshots[:atlas_count]],
                [bytes(pal) for pal in palette_snapshots[:atlas_count]],
                gif_transparent_idx,
            )
        )

    # Build the atlas JSON sidecar. Frame size is uniform across the
    # whole sprite, so the sidecar is compact: top-level frame
    # dimensions + a per-frame array carrying anything that varies
    # (engine opcodes applied, NotifyMove origin (x, y), frame
    # duration in ticks, dirty-rect / region / transparency-index
    # markers).
    atlas_meta: Optional[dict] = None
    if atlas_path is not None:
        # Filter ring-frame metadata into a sibling field so the
        # primary ``frames`` array describes only what the atlas
        # actually shows.
        atlas_frames = per_frame[:atlas_count]
        motion_path = _summarise_motion_path(
            per_frame, ring_frame_index, len(snapshots)
        )
        atlas_meta = {
            "atlas": atlas_path.name,
            "gif": gif_path.name if gif_path is not None else None,
            "frameWidth": bitmap_w,
            "frameHeight": bitmap_h,
            "frameCount": atlas_count,
            "framesStored": len(snapshots),
            "paletteSource": palette_source,
            "transparencySource": transparency_source,
            "maskPlanePresent": mask_was_written,
            "timing": timing_block,
            "motionPath": motion_path,
            "frames": atlas_frames,
        }
        if palette_source == "inherited" and inherited_palette_source_id is not None:
            atlas_meta["inheritedFrom"] = inherited_palette_source_id
        if ring_frame_index is not None:
            atlas_meta["ringFrameIndex"] = ring_frame_index
            atlas_meta["ringFrame"] = per_frame[ring_frame_index]
        sidecar_path = out_dir / f"{stem}.atlas.json"
        sidecar_path.write_text(
            json.dumps(atlas_meta, indent=2, ensure_ascii=False),
            encoding="utf-8",
        )

    result: dict = {
        "bitmapWidth": bitmap_w,
        "bitmapHeight": bitmap_h,
        "paletteWrites": n_palette_writes,
        "paletteSource": palette_source,
        "transparentIndex": transparent_index,
        "bgKeyIndices": sorted(bg_key_indices),
        "transparentIndices": sorted(transparent_indices),
        "transparencySource": transparency_source,
        "maskPlanePresent": mask_was_written,
        "atlasPng": atlas_path.name if atlas_path else None,
        "atlasJson": (out_dir / f"{stem}.atlas.json").name if atlas_path else None,
        "atlasGif": gif_path.name if gif_path else None,
        "framesStored": len(snapshots),
        "framesAnimated": atlas_count,
        "ringFrameIndex": ring_frame_index,
    }
    if palette_source == "inherited" and inherited_palette_source_id is not None:
        result["inheritedFrom"] = inherited_palette_source_id
    # Private bridge fields consumed by ``_save_bitmap_sprite`` to
    # update the caller's ambient palette state; stripped before the
    # dict is folded into the manifest.
    if palette_source == "inline":
        result["_finalPalette"] = bytes(palette_snapshots[-1]) if palette_snapshots else bytes(palette)
        # Resource ID is encoded in ``base.name`` as
        # ``res_<10-digit ID>_52_BitmapSprite``; pull it back out so the
        # caller can stamp the next inheriting sprite's
        # ``inheritedFrom`` field.
        try:
            sid_str = base.name.split("_")[1]
            result["_sourceId"] = int(sid_str)
        except (IndexError, ValueError):
            pass
    return result


def _palette_siblings_of(palette: bytearray, rgb: Tuple[int, int, int]) -> set[int]:
    """Return the set of palette indices whose RGB triple equals ``rgb``."""
    return {
        i
        for i in range(256)
        if (palette[i * 4], palette[i * 4 + 1], palette[i * 4 + 2]) == rgb
    }


def _chromakey_bg_mask(
    snap: bytes, w: int, h: int, key_indices: set[int]
) -> bytearray:
    """Return a ``w*h`` mask where ``1`` means the pixel's palette index
    is in ``key_indices`` (= the chroma key set) and ``0`` means it's a
    subject pixel.

    Mirrors the engine's chroma-key blit kernels (dispatch table
    ``BlitTable_ChromaKey`` / ``DAT_004b0ac8``, kernels like
    ``BlitChromaKey`` @ 0x0043f470) which skip *every* source pixel
    matching the chroma byte irrespective of topology — the test is
    the simple equality ``if (src_pixel != key) draw_pixel``.

    Per-pixel transparency for engine-faithful sprite extraction:

    * The 39 sprites that emit opcode ``0x0E/0x0F`` use the
      authoritative mask plane and never reach this path.
    * The 3 sprites that emit ``0x0D`` set an explicit chroma byte
      and reach this path with that exact index.
    * For the 88 remaining sprites the engine itself runs the opaque
      blit kernel (``BlitTable_Opaque`` / ``DAT_004b08c8``); their
      visible in-game transparency is achieved upstream (DDraw
      destination-side color key on the back-buffer the scene blits
      into — see the engine trace in sprite_container.md). The
      perimeter-majority RGB index is the canonical "BG palette slot"
      they're authored against, and using it as a synthetic chroma
      key here produces the correct hollow silhouette that the game
      shows (verified on 65797 where every frame paints body pixels
      in the same palette index 104 as the rectangular BG, so the
      "filled green oval" only resolves to a gray-outlined creature
      when index 104 is fully chroma-keyed).
    """
    mask = bytearray(w * h)
    for i in range(len(snap)):
        if snap[i] in key_indices:
            mask[i] = 1
    return mask


def _resolve_bg_key_indices(
    explicit_index: Optional[int],
    snapshots: list[bytes],
    bitmap_w: int,
    bitmap_h: int,
    palette: bytearray,
) -> Tuple[set[int], str]:
    """Pick the set of palette indices that should be treated as
    background chroma for the synthetic key.

    Priority order:

    1. ``"explicit"`` — FLX opcode 0x0D set an explicit
       ``transparent_index``. We use ``palette[N]``'s RGB plus every
       palette entry sharing that RGB.
    2. ``"perimeter"`` — Tally the RGB of every pixel on the bitmap
       perimeter (top row + bottom row + left col + right col) across
       every emitted frame. If one RGB covers a clear majority of the
       perimeter, that's the rectangular BG colour.

       This generalises the previous "all four corners agree" rule:
       the engine's chroma is a game-wide constant (FLI-conventional
       magic green ``RGB(7,155,0)`` for this title), and many sprites
       have content pixels touching one or two corners while the
       remainder of the perimeter is still clean BG. A pure corner
       vote bails in those cases; a perimeter vote does not.

    3. ``"none"`` — no single RGB dominates the perimeter (the sprite
       fills the bitmap edge-to-edge with content): emit opaque, the
       engine would have relied on the destination-side chroma key
       anyway.
    """
    # Sanity-checked explicit path. Opcode 0x0D writes a single palette
    # index to consumer +0x18, which the engine's color-key blit kernels
    # compare against the source pixel value byte-for-byte
    # (``BlitChromaKey`` @ 0x0043f470 etc.). Some sprites — the CGunMouse
    # idle-twitch tracks 65801/65802/65803 are the canonical example —
    # write an opcode-0x0D index whose palette slot was never initialised
    # by an opcode-0x09 palette write, so it stays at the default
    # ``(255, 255, 255)`` from ``palette = bytearray([0xFF] * 1024)``.
    # The sprite's real BG ends up encoded at a *different* palette index
    # (palette[0] = (0, 0, 0) for the dot tracks), and a strict RGB
    # sibling lookup against palette[explicit_index] resolves to a set
    # that no pixel in the sprite ever references — the chroma key
    # rewrite becomes a no-op and the BG renders opaque.
    #
    # We accept the explicit path only when its sibling set actually
    # matches at least one painted pixel; otherwise we fall through to
    # the perimeter-vote heuristic (which finds palette[0] = BLACK for
    # the dot sprites).
    if explicit_index is not None and 0 <= explicit_index < 256:
        n = explicit_index
        rgb = (palette[n * 4 + 0], palette[n * 4 + 1], palette[n * 4 + 2])
        siblings = _palette_siblings_of(palette, rgb)
        siblings.add(n)
        explicit_hits_pixels = any(
            (p in siblings) for snap in snapshots for p in snap
        )
        if explicit_hits_pixels:
            return siblings, "explicit"
        # Fall through to perimeter vote below. The explicit index is
        # noted in the result via ``transparentIndex`` already.

    if not snapshots or bitmap_w < 3 or bitmap_h < 3:
        return set(), "none"

    # Tally the RGB at every perimeter pixel across every snapshot.
    # We vote on RGB rather than palette index so palette-cycle
    # sprites (whose BG re-indexes from frame to frame) still
    # accumulate to the same RGB bucket.
    perimeter_rgbs: Counter[Tuple[int, int, int]] = Counter()
    for snap in snapshots:
        for x in range(bitmap_w):
            for y in (0, bitmap_h - 1):
                idx = snap[y * bitmap_w + x]
                perimeter_rgbs[
                    (
                        palette[idx * 4 + 0],
                        palette[idx * 4 + 1],
                        palette[idx * 4 + 2],
                    )
                ] += 1
        for y in range(1, bitmap_h - 1):
            for x in (0, bitmap_w - 1):
                idx = snap[y * bitmap_w + x]
                perimeter_rgbs[
                    (
                        palette[idx * 4 + 0],
                        palette[idx * 4 + 1],
                        palette[idx * 4 + 2],
                    )
                ] += 1
    if not perimeter_rgbs:
        return set(), "none"
    (top_rgb, top_count), *_ = perimeter_rgbs.most_common(1)
    total = sum(perimeter_rgbs.values())
    # Majority threshold = 50% of perimeter pixels carry one RGB.
    # In practice the engine's BG covers 80–100% of the perimeter
    # whenever there *is* a BG; anything subject-edge-to-edge falls
    # well below this and is left opaque.
    if top_count * 2 <= total:
        return set(), "none"
    siblings = _palette_siblings_of(palette, top_rgb)
    if not siblings:
        return set(), "none"
    return siblings, "perimeter"


def _apply_mask_plane_transparency(
    snapshots: list[bytearray],
    mask_snapshots: list[Optional[bytearray]],
    bitmap_w: int,
    bitmap_h: int,
) -> Optional[int]:
    """Translate the engine's per-pixel mask plane (consumer +0x20,
    populated by FLX opcodes 0x0E / 0x0F) into PNG alpha by rewriting
    transparent pixels to a reserved palette index.

    Per the Ghidra trace (``CDSFlxFile::AllocMaskPlane @ 0x00436ff0``)
    the mask plane is byte-per-pixel, sized exactly w*h. The masked
    blit kernels at ``BlitTable_Masked`` / ``DAT_004b0bc8`` (e.g.
    ``BlitMasked`` @ 0x004414e0) then test each pixel's mask byte
    against a reference value to decide opacity.

    Empirically (verified across every mask-using sprite in the
    master pack), the engine's per-frame BG identification rule is:

        **A pixel is transparent iff its
        ``(color[i], mask[i])`` pair matches the bitmap's corner
        consensus ``(corner_color, corner_mask)`` pair.**

    This is a stricter version of the "BG sentinel" rule: both the
    color plane AND the mask plane must agree with the corners.
    Sprites where the color plane uses a uniform fill index but the
    mask plane carries the *actual* opacity (e.g. 65831's
    color=104 grayscale fallback overlayed with mask=255 sentinel)
    are handled correctly by the joint check, where a color-only or
    mask-only check would either leave the BG fully opaque or wipe
    the entire frame.

    Cross-checked against four sprites with very different shapes:

    * 65714 frame 0 (yellow explosion): corner pair (2, 2). BG =
      pixels where color=2 AND mask=2 = 8417 px; subject = 67 px.
    * 65791 frame 0 (wireframe cube): corner pair (0, 0). BG = 3724
      px; subject (cube edges + interior) = 4088 px.
    * 65831 frame 0 (gray sprite, grayscale-fallback palette): corner
      pair (104, 255). BG = 2801 px where color=104 AND mask=255;
      subject = 1654 px (the small figure on its gray BG).
    * 65754 frame 0 (init splash): corner pair (104, 104). 100% of
      pixels match — fully transparent initial frame.

    The corner consensus is the *majority* corner value within each
    frame (handles masks that don't perfectly reach every corner —
    very rare on the master pack but possible). Reserves an unused
    palette index for the alpha encoding (so we don't accidentally
    clobber a real colour), or returns ``None`` if no free slot
    exists in the palette.
    """
    used = bytearray(256)
    for snap in snapshots:
        for px in snap:
            used[px] = 1
    free_idx: Optional[int] = None
    for i in range(256):
        if not used[i]:
            free_idx = i
            break
    if free_idx is None:
        return None

    last_x = bitmap_w - 1
    last_row = (bitmap_h - 1) * bitmap_w
    for snap, mask in zip(snapshots, mask_snapshots):
        if mask is None:
            continue
        color_corners = [
            snap[0],
            snap[last_x],
            snap[last_row],
            snap[last_row + last_x],
        ]
        mask_corners = [
            mask[0],
            mask[last_x],
            mask[last_row],
            mask[last_row + last_x],
        ]
        ccounts: dict[int, int] = {}
        mcounts: dict[int, int] = {}
        for c in color_corners:
            ccounts[c] = ccounts.get(c, 0) + 1
        for m in mask_corners:
            mcounts[m] = mcounts.get(m, 0) + 1
        bg_color = max(ccounts, key=lambda c: ccounts[c])
        bg_mask = max(mcounts, key=lambda c: mcounts[c])
        for i in range(len(snap)):
            if snap[i] == bg_color and mask[i] == bg_mask:
                snap[i] = free_idx
    return free_idx


def _apply_chromakey_transparency(
    snapshots: list[bytes],
    palette: bytearray,
    bitmap_w: int,
    bitmap_h: int,
    key_indices: set[int],
) -> Optional[int]:
    """Rewrite each snapshot in place so that every pixel whose palette
    index is in ``key_indices`` is remapped to a single reserved
    "transparent" palette index. Returns the reserved index (so the
    caller can mark it transparent in the PNG ``tRNS`` chunk), or
    ``None`` if no free palette slot is available.

    This matches the engine's chroma-key blit semantics exactly:
    every source pixel matching the key is skipped (per
    ``BlitChromaKey`` @ 0x0043f470 and siblings in dispatch table
    ``BlitTable_ChromaKey`` / ``DAT_004b0ac8``).
    Sprites whose body and rectangular BG share the same palette
    index — a very common encoding for "hollow silhouette" content —
    therefore resolve to the correct cut-out shape rather than a
    filled blob.

    Picks a palette index that is currently unused by ANY snapshot so
    we don't accidentally turn an in-use index transparent. If every
    palette slot is occupied (extremely rare) the caller falls back
    to emitting the sprite opaque.
    """
    used = bytearray(256)
    for snap in snapshots:
        for px in snap:
            used[px] = 1
    free_idx: Optional[int] = None
    for i in range(256):
        if not used[i]:
            free_idx = i
            break
    if free_idx is None:
        return None
    for snap in snapshots:
        mask = _chromakey_bg_mask(snap, bitmap_w, bitmap_h, key_indices)
        for i, m in enumerate(mask):
            if m:
                snap[i] = free_idx
    return free_idx


def _redecode_for_atlas(
    raw: bytes,
    frames: list[dict],
    bitmap_w: int,
    bitmap_h: int,
    initial_palette: Optional[bytes] = None,
) -> Tuple[list[bytearray], list[Optional[bytearray]], list[bytearray], bool]:
    """Re-run the per-frame decode for atlas emission, capturing the
    color plane, the (lazy) mask plane, and the palette state after
    every emitted frame.

    Returns ``(color_snapshots, mask_snapshots, palette_snapshots,
    mask_was_ever_written)``.

    * ``color_snapshots`` — one mutable ``bytearray`` per emitted frame.
      Empty frames are NOT emitted (they inherit the previous frame).
    * ``mask_snapshots`` — same length; entry is ``None`` if no mask
      bytes have been written yet (the engine renders fully opaque
      until then), else a snapshot of the mask plane at this frame.
    * ``palette_snapshots`` — same length; each entry is a copy of the
      256-entry BGRA palette as-it-was at this frame's emit point.
      This is what the engine would render the frame with; required
      for sprites that update their palette mid-animation (71/130
      master-pack sprites — verified empirically).
    * ``mask_was_ever_written`` — True iff opcode 0x0e / 0x0f ever
      fired across the whole stream.

    ``initial_palette`` (optional) seeds the 256-entry BGRA palette
    buffer before the decode starts. The engine's render context
    keeps the palette buffer live across sibling sprites loaded in
    the same load batch, so a sprite without its own opcode-0x09
    palette write inherits whatever the previously-loaded inline
    sibling left behind. Callers pass that sibling's final palette
    here so the inheriting sprite renders in colour rather than
    falling back to a grayscale ramp. ``None`` keeps the engine's
    own default (``CBulPicture`` init writes ``0xFFFFFFFF`` into
    every slot).

    Mirrors ``CDSFlxFile::DecodeFrame`` (0x00432c60). Per Ghidra the
    mask plane is allocated lazily on the first 0x0e (``AllocMaskPlane
    @ 0x00436ff0``) sized exactly w*h bytes, and the masked blit
    kernels gate per-pixel opacity on the mask byte value.
    """
    pixel_total = bitmap_w * bitmap_h
    pixels = bytearray(pixel_total)
    mask = bytearray(pixel_total)
    mask_inited = False
    if initial_palette is not None and len(initial_palette) == 1024:
        palette = bytearray(initial_palette)
    else:
        palette = bytearray([0xFF] * 1024)
    color_snapshots: list[bytearray] = []
    mask_snapshots: list[Optional[bytearray]] = []
    palette_snapshots: list[bytearray] = []
    keyframe_seen = False
    for fr in frames:
        for ic in fr["innerChunks"]:
            op = ic["opcode"]
            body = raw[ic["offset"] + 5 : ic["offset"] + ic["size"]]
            if op == BITMAP_SPRITE_INNER_OP_RLE:
                pixels[:] = _flxrle_decode(body)
                keyframe_seen = True
            elif op == BITMAP_SPRITE_INNER_OP_DELTA:
                _flxdelta_apply(body, pixels)
            elif op == BITMAP_SPRITE_INNER_OP_MEMCPY:
                pixels[: len(body)] = body
                keyframe_seen = True
            elif op == BITMAP_SPRITE_INNER_OP_PALETTE:
                _flxpalette_apply(body, palette)
            elif op == BITMAP_SPRITE_INNER_OP_RLE_ALT:
                mask[:] = _flxrle_decode(body)
                mask_inited = True
            elif op == BITMAP_SPRITE_INNER_OP_DELTA_ALT:
                _flxdelta_apply(body, mask)
                mask_inited = True
        if keyframe_seen:
            color_snapshots.append(bytearray(pixels))
            mask_snapshots.append(bytearray(mask) if mask_inited else None)
            palette_snapshots.append(bytearray(palette))
    return color_snapshots, mask_snapshots, palette_snapshots, mask_inited


_FONT_GLYPH_ENCODING = "cp1250"
"""Code page the CDSFont glyph table is indexed by.

The engine renders Unicode (UTF-16-LE) poem/menu strings by mapping each
code point to its Windows-1250 byte and using that byte as the glyph
index. Empirical confirmation: the Czech poems at resources 65541..65553
roundtrip cleanly through ``str.encode('cp1250')`` and the glyph slot at
e.g. byte 0xEC (= ``ě``) holds a visibly correct caret-e glyph in the
extracted font sheet. CP-1250 (Central European) is the natural code
page for the Czech publisher's Win32 build.

Byte positions 0x81, 0x83, 0x88, 0x90, 0x98 are unassigned in CP-1250 and
therefore have ``unicodeChar = None`` in the manifest (glyphs at those
slots are unreachable from text input but are still extracted so the
sheet round-trips byte-for-byte).
"""


def _cp1250_char_for_glyph_index(i: int) -> Optional[str]:
    """Return the Unicode character that the engine renders via glyph
    slot ``i``, or ``None`` if the slot is unreachable from text input.

    * 0x00..0x1F (control chars) and 0x7F (DEL) → ``None`` even though
      a handful of master-pack fonts ship non-empty glyphs in those
      slots (likely UI icons or unused placeholders). The text path
      can't reach them via a normal CP-1250 byte.
    * 0x20..0x7E (printable ASCII) → ``chr(i)``.
    * 0x80..0xFF → decoded via the ``cp1250`` codec; slots that map to
      ``\\ufffd`` (Python's REPLACEMENT CHARACTER) are unassigned in
      CP-1250 and reported as ``None``.
    """
    if i < 0x20 or i == 0x7F:
        return None
    if 0x20 <= i <= 0x7E:
        return chr(i)
    try:
        ch = bytes([i]).decode(_FONT_GLYPH_ENCODING, errors="replace")
    except Exception:
        return None
    if not ch or ch == "\ufffd":
        return None
    return ch


def _save_font(raw: bytes, base: Path) -> dict:
    """ClassID 54 - CDSFont pre-rendered font.

    Format:
        - Embedded CDSImage (ClassID 28 BitmapSpecial style) at offset 0.
        - 8 bytes: default character width (int32), line height (int32).
        - 1280 bytes: character metrics table of 256 entries * 5 bytes each.
          Each entry: offset_x (uint16), width (uint8), offset_y (uint8), height (uint8).

    The 256-entry glyph table is indexed by a Windows-1250 byte (see
    :data:`_FONT_GLYPH_ENCODING`). The manifest surfaces three pieces of
    information consumers need to actually render strings:

    * ``encoding``       — the canonical name of the lookup code page.
    * ``characters[i].unicodeChar`` — the Unicode character that maps to
      glyph slot ``i`` (replaces the old ASCII-only ``char`` field).
    * ``unicodeToGlyph`` — a compact ``{ "<codepoint hex>": <index> }``
      lookup so consumers don't have to embed a CP-1250 table.
    """
    meta: dict = {"friendlyFile": None}
    if len(raw) < 26:
        meta["warning"] = "Font: too short for CDSImage header"
        return meta

    # Parse the CDSImage header to compute its exact size on disk
    width, height, marker5, stride = struct.unpack_from("<iiii", raw, 0)
    palette_count_field = struct.unpack_from("<i", raw, 21)[0]
    pad_b = raw[25]

    if marker5 in (0, 1, 2, 3):
        pal_bytes = palette_count_field * 4
    else:
        pal_bytes = 0

    packed_payload = height * stride
    unpacked_payload = width * height if pad_b else 0
    image_size = 26 + pal_bytes + packed_payload + unpacked_payload

    if len(raw) < image_size + 8 + 1280:
        meta["warning"] = f"Font: file too short ({len(raw)} bytes vs expected at least {image_size + 8 + 1280})"
        return meta

    # Slice the image portion and extract it as a standard BitmapSpecial image
    image_raw = raw[:image_size]
    try:
        # Call _save_special to decode and save the glyph sheet PNG
        image_meta = _save_special(image_raw, base)
        if "warning" in image_meta:
            meta["warning"] = f"Font image decode warning: {image_meta['warning']}"
        meta["friendlyFile"] = image_meta.get("friendlyFile")
    except Exception as exc:
        meta["warning"] = f"Font image decode failed: {exc}"
        image_meta = {}

    # Parse the 8 bytes of extra header
    extra_off = image_size
    default_width, line_height = struct.unpack_from("<ii", raw, extra_off)

    # Parse the 1280 bytes of character metrics table. ``char`` stays
    # ASCII-only for backwards compatibility; ``unicodeChar`` carries
    # the engine-authoritative CP-1250 mapping for the full 0..255
    # range, and ``unicodeCodepoint`` mirrors it as an integer so
    # consumers can pick whichever shape is easier to consume.
    table_off = extra_off + 8
    chars = []
    unicode_to_glyph: dict[str, int] = {}
    glyphs_with_pixels = 0
    for i in range(256):
        offset_x, char_width, offset_y, char_height = struct.unpack_from(
            "<HBBB", raw, table_off + i * 5
        )
        ascii_char = chr(i) if 32 <= i < 127 else None
        unicode_char = _cp1250_char_for_glyph_index(i)
        entry = {
            "index": i,
            "char": ascii_char,
            "unicodeChar": unicode_char,
            "unicodeCodepoint": ord(unicode_char) if unicode_char else None,
            "offsetX": offset_x,
            "width": char_width,
            "offsetY": offset_y,
            "height": char_height,
        }
        chars.append(entry)
        if char_width > 0 and char_height > 0:
            glyphs_with_pixels += 1
        if unicode_char is not None and char_width > 0:
            # Only register reachable glyphs in the reverse lookup —
            # empty/zero-width slots (e.g. line breaks) shouldn't show
            # up as renderable mappings.
            unicode_to_glyph[f"U+{ord(unicode_char):04X}"] = i

    # Save the parsed font metadata to a JSON file
    meta["font"] = {
        "width": width,
        "height": height,
        "marker": marker5,
        "stride": stride,
        "defaultWidth": default_width,
        "lineHeight": line_height,
        "encoding": _FONT_GLYPH_ENCODING,
        "encodingNote": (
            "Glyph table is indexed by Windows-1250 byte. The engine "
            "maps each input Unicode code point through cp1250 before "
            "looking up characters[byte]. Slots without a CP-1250 "
            "byte (e.g. control codes, unassigned positions) carry "
            "unicodeChar = null."
        ),
        "image": image_meta.get("special", {}),
        "characters": chars,
        "unicodeToGlyph": unicode_to_glyph,
        "glyphsWithPixels": glyphs_with_pixels,
        "reachableGlyphCount": len(unicode_to_glyph),
    }

    out_json = base.with_suffix(".font.json")
    try:
        out_json.write_text(json.dumps(meta["font"], indent=2), encoding="utf-8")
        if not meta["friendlyFile"]:
            meta["friendlyFile"] = out_json.name
    except Exception as exc:
        meta["warning"] = f"Font JSON write failed: {exc}"

    return meta


def _save_bitmap_jpeg_anim(raw: bytes, base: Path) -> dict:
    """ClassID 76 - ``CDSDsmFile`` synchronized MJPEG + 16-bit PCM movie.

    The on-disk format is a fixed 36-byte ``CDsmHeader`` followed by
    ``2 * dwFrameCount`` interleaved ``{u32 len, byte[len] body}`` chunks:
    chunk ``2k`` is a complete JPEG frame, chunk ``2k+1`` is the matching
    frame's raw little-endian PCM audio. See
    ``../../ghidra_analysis/dsm_file_format.md`` for the
    full reverse-engineering writeup of the format and the ``CDSDsmFile``
    class that consumes it.

    We dump:
      * one ``.frame<N>.jpg`` per video chunk (round-trippable to libjpeg),
      * one consolidated ``.audio.wav`` containing every audio chunk
        concatenated in order with a standard PCM WAVE header.
    """
    meta: dict = {"friendlyFile": None}
    if len(raw) < 36:
        meta["warning"] = "BitmapJpegAnim: too short for CDsmHeader"
        return meta
    end_offset, w, h, fmt, dur_ms, frames, audio_bytes, audio_packed, rate = (
        struct.unpack_from("<9I", raw, 0)
    )
    channels = audio_packed & 0xFFFF
    bits = (audio_packed >> 16) & 0xFFFF
    expected_audio = (dur_ms * rate * channels * bits) // 8 // 1000
    header = {
        "dwPayloadEndOffset": end_offset,
        "dwCanvasWidth": w,
        "dwCanvasHeight": h,
        "dwPixelFormat": fmt,
        "dwDurationMs": dur_ms,
        "dwFrameCount": frames,
        "dwAudioByteCount": audio_bytes,
        "dwAudioFormatPacked": f"0x{audio_packed:08x}",
        "dwAudioSampleRate": rate,
        "audioChannels": channels,
        "audioBitsPerSample": bits,
        "calculatedAudioBytes": expected_audio,
        "fps": round(frames * 1000 / dur_ms, 3) if dur_ms else None,
        "pixelFormatHint": {5: "24bpp BGR", 6: "32bpp BGRA"}.get(fmt, "unknown"),
    }
    warnings: list[str] = []
    if end_offset != len(raw):
        warnings.append(
            f"dwPayloadEndOffset={end_offset} differs from file size {len(raw)}"
        )
    if abs(audio_bytes - expected_audio) > 8:
        warnings.append(
            f"dwAudioByteCount={audio_bytes} differs from expected "
            f"{expected_audio} by {audio_bytes - expected_audio:+d}"
        )
    meta["jpegAnim"] = {"header": header}

    jpeg_frames: list[dict] = []
    audio_chunks: list[bytes] = []
    off = 36
    truncated = False
    while off + 4 <= len(raw):
        (chunk_len,) = struct.unpack_from("<I", raw, off)
        body_start = off + 4
        body_end = body_start + chunk_len
        if chunk_len == 0 or body_end > len(raw):
            truncated = True
            break
        body = raw[body_start:body_end]
        is_jpeg = body[:3] == b"\xff\xd8\xff"
        if is_jpeg:
            idx = len(jpeg_frames)
            out = base.with_suffix(f".frame{idx:03d}.jpg")
            out.write_bytes(body)
            jpeg_frames.append(
                {"offset": body_start, "length": chunk_len, "file": out.name}
            )
        else:
            audio_chunks.append(body)
        off = body_end

    if truncated:
        warnings.append(f"chunk walk stopped early at offset 0x{off:x}")
    if len(jpeg_frames) != frames:
        warnings.append(
            f"JPEG chunk count {len(jpeg_frames)} differs from "
            f"dwFrameCount {frames}"
        )

    if audio_chunks and channels in (1, 2) and bits == 16 and rate > 0:
        pcm = b"".join(audio_chunks)
        wav_path = base.with_suffix(".audio.wav")
        block_align = channels * bits // 8
        byte_rate = rate * block_align
        with wav_path.open("wb") as fh:
            fh.write(b"RIFF")
            fh.write(struct.pack("<I", 36 + len(pcm)))
            fh.write(b"WAVE")
            fh.write(b"fmt ")
            fh.write(struct.pack("<I", 16))
            fh.write(struct.pack("<H", 1))           # wFormatTag = PCM
            fh.write(struct.pack("<H", channels))
            fh.write(struct.pack("<I", rate))
            fh.write(struct.pack("<I", byte_rate))
            fh.write(struct.pack("<H", block_align))
            fh.write(struct.pack("<H", bits))
            fh.write(b"data")
            fh.write(struct.pack("<I", len(pcm)))
            fh.write(pcm)
        meta["jpegAnim"]["audioWav"] = {
            "file": wav_path.name,
            "pcmBytes": len(pcm),
            "expectedPcmBytes": audio_bytes,
        }
    elif audio_chunks:
        warnings.append(
            "audio chunks present but format not (16-bit mono/stereo PCM); "
            "wav export skipped"
        )

    if jpeg_frames:
        meta["jpegAnim"]["jpegFrames"] = jpeg_frames
        meta["friendlyFile"] = jpeg_frames[0]["file"]
    if warnings:
        meta["jpegAnim"]["warnings"] = warnings
    return meta


_EXTRACTORS = {
    CLASS_BITMAP_JPEG: _save_jpeg,
    CLASS_BITMAP_BMP: _save_bmp,
    CLASS_BITMAP_SPECIAL: _save_special,
    CLASS_AUDIO_BANK: _save_audio_bank,
    CLASS_MP3: _save_mp3,
    CLASS_BITMAP_SPRITE: _save_bitmap_sprite,
    CLASS_FONT: _save_font,
    CLASS_DSM_INNER: _save_dsm_inner,
    CLASS_AUDIO_BANK_INDEX: _save_audio_bank_index,
    CLASS_BITMAP_JPEG_ANIM: _save_bitmap_jpeg_anim,
    CLASS_SIGN: _save_sign,
    CLASS_SCRIPT: _save_script,
    CLASS_POEM: _save_poem,
    CLASS_HISTORY_SCRIPT: _save_history_script,
    CLASS_HELP_SCRIPT: _save_help_script,
}


def save_resource(
    rh: ResourceHeader,
    raw: bytes,
    out_dir: Path,
    sprite_state: Optional[dict] = None,
) -> dict:
    """Write `res_<ID>_<ClassID>_<Name>.bin` + any friendly companion file.

    ``sprite_state`` (optional) — caller-owned mutable dict carrying
    the ambient FLX palette across consecutive resources, mirroring
    the engine's render context. Keys: ``ambient`` (1024-byte BGRA
    palette of the most-recently loaded inline-palette sibling, or
    None) and ``sourceId`` (its resource ID). BitmapSprite is the
    only extractor that reads / writes it today; everything else
    ignores it.
    """
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
            if rh.class_id == CLASS_BITMAP_SPRITE and sprite_state is not None:
                meta.update(
                    _save_bitmap_sprite(
                        raw,
                        base,
                        inherited_palette=sprite_state.get("ambient"),
                        inherited_palette_source_id=sprite_state.get("sourceId"),
                        sprite_state=sprite_state,
                    )
                )
            else:
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
        # Ambient palette inheritance state for BitmapSprite resources.
        # Mirrors the engine's render context: the FLX palette buffer
        # in CBulPicture +0x6c..+0x46b is left live across consecutive
        # sprite loads in the same pack, so a sprite without an inline
        # FLX opcode-0x09 palette inherits whatever the previously
        # loaded sibling left behind. Headers are walked in load order
        # below, which matches the engine's resource-pool load order.
        sprite_state: dict = {"ambient": None, "sourceId": None}
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
            meta = save_resource(rh, raw, out_dir, sprite_state=sprite_state)
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


def _prepare_output_dir(output: Path, clean: bool) -> None:
    """Make sure ``output`` exists and (optionally) starts empty.

    The unpacker writes only ``res_*`` resource files, ``_manifest.json``,
    ``_raw.bin``, and class-specific subdirs (e.g. an embedded
    BitmapJpegAnim WAV). When the user re-runs the unpacker with a
    newer decoder table, stale artifacts from the previous run are
    NOT overwritten — different decoders pick different suffixes,
    name resources by their (now corrected) class name, or emit
    different sidecar files. Result: the output directory keeps a
    long tail of orphaned files (e.g. ``res_*_54_MouseCursor.bin``
    left over from when class 54 was misidentified as ``MouseCursor``
    instead of ``Font``).

    The default behaviour is therefore to wipe ``output`` first so
    every run produces a clean, self-consistent snapshot of what the
    *current* unpacker decodes. Pass ``clean=False`` to keep the
    existing files in place (useful for incremental debug iteration
    where regenerating every JPEG is wasteful).

    Safety rails: we never touch ``output`` if it equals ``/`` or the
    current working directory, and we refuse to wipe anything that
    resolves to a file (i.e. not an existing directory). All other
    failure modes fall through to ``shutil.rmtree`` which will raise
    a clear ``OSError`` the user can act on.
    """
    output = output.resolve()
    cwd = Path.cwd().resolve()
    if clean and output.exists():
        if output == cwd or output.parent == output:
            raise ValueError(
                f"refusing to clean output directory {output} "
                f"(equals cwd or filesystem root)"
            )
        if not output.is_dir():
            raise ValueError(
                f"output path {output} exists but is not a directory; "
                f"refusing to clean."
            )
        shutil.rmtree(output)
    output.mkdir(parents=True, exist_ok=True)


def cmd_overlay(args: argparse.Namespace) -> Manifest:
    _prepare_output_dir(Path(args.output), getattr(args, "clean", True))
    buf = _read_bytes(args.input)
    start = find_pe_overlay_start(buf)
    print(
        f"PE overlay starts at file offset 0x{start:X} (file size 0x{len(buf):X}); "
        f"overlay length = {len(buf) - start} bytes"
    )
    return unpack(buf, start, Path(args.output), source=args.input, source_type="overlay")


def cmd_eap(args: argparse.Namespace) -> Manifest:
    _prepare_output_dir(Path(args.output), getattr(args, "clean", True))
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
    _prepare_output_dir(Path(args.output), getattr(args, "clean", True))
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
    _prepare_output_dir(Path(args.output), getattr(args, "clean", True))
    buf = _read_bytes(args.input)
    return unpack(buf, 0, Path(args.output), source=args.input, source_type="auto")


# Skipped silently by `cmd_all`: companion DLLs / .NET editor binaries / book-
# keeping that live alongside bulanci.exe in `orig/` but aren't unpackable
# containers.
_ALL_SKIP_NAMES = frozenset({
    "editor.exe",
    "naudio.dll",
    "zlib.net.dll",
    "hashes",
    ".gitkeep",
    ".gitignore",
})


def cmd_all(args: argparse.Namespace) -> int:
    """Batch-unpack every supported file in `args.input_dir`.

    Routes by extension, mirroring the layout already used by the rest of
    the project:

        bulanci.exe         ->  <output>/overlay/
        <name>.eap          ->  <output>/<name>_eap/      (paired with sibling .eapres)
        <name>.eapres       ->  <output>/<name>_eapres/

    Returns a shell-style exit code: 0 if every supported input succeeded,
    otherwise the number of failed files.
    """
    input_dir = Path(args.input_dir)
    output_dir = Path(args.output_dir)
    if not input_dir.is_dir():
        print(f"ERROR: not a directory: {input_dir}", file=sys.stderr)
        return 2

    # In ``--clean`` mode we wipe the entire root output directory
    # before we start, so any orphan files from a previous run
    # (loose ``res_*.bin`` left over from an early manual
    # ``cmd overlay -o unpacked`` invocation, or a top-level
    # ``_manifest.json`` / ``_raw.bin`` snapshot) don't survive into
    # the new run. The per-target subdirs are then recreated by the
    # individual ``cmd_*`` callees, also under ``--clean`` semantics,
    # so the final tree is guaranteed to be a self-consistent
    # snapshot of what the current decoders produce.
    _prepare_output_dir(output_dir, args.clean)

    # (input_path, mode, output_subdir)
    plan: List[Tuple[Path, str, Path]] = []
    skipped: List[Path] = []
    for p in sorted(input_dir.iterdir()):
        if not p.is_file():
            continue
        name = p.name.lower()
        stem = p.stem
        if name == "bulanci.exe":
            plan.append((p, "overlay", output_dir / "overlay"))
        elif name.endswith(".eap"):
            plan.append((p, "eap", output_dir / f"{stem}_eap"))
        elif name.endswith(".eapres"):
            plan.append((p, "eapres", output_dir / f"{stem}_eapres"))
        elif name in _ALL_SKIP_NAMES:
            skipped.append(p)
        else:
            skipped.append(p)

    if not plan:
        print(f"No supported inputs under {input_dir} "
              f"(looked for bulanci.exe / *.eap / *.eapres)")
        return 0

    print(f"== Bulanci batch unpack ==")
    print(f"  input dir   : {input_dir}")
    print(f"  output dir  : {output_dir}")
    print(f"  clean mode  : {'wipe output root + target subdirs first' if args.clean else 'overwrite in place'}")
    print(f"  to process  : {len(plan)} file(s)")
    for src, mode, dest in plan:
        print(f"     {mode:7s} {src.name:<32s} -> {dest.name}/")
    if skipped:
        print(f"  skipping    : {', '.join(s.name for s in skipped)}")
    print()

    manifests: List[Manifest] = []
    failures: List[Tuple[Path, BaseException]] = []

    for src, mode, dest in plan:
        print()
        print(f"--- [{mode}] {src.name} -> {dest} ---")
        # Each callee runs ``_prepare_output_dir`` on its own subdir
        # so the per-target tree is guaranteed clean (the root-level
        # wipe above already covers the case where ``dest`` didn't
        # exist at all). Forward the same ``--clean`` flag so a
        # ``--no-clean`` batch run preserves existing subdir contents
        # for incremental iteration.
        sub_args = argparse.Namespace(
            input=str(src), output=str(dest), clean=args.clean
        )
        try:
            if mode == "overlay":
                mf = cmd_overlay(sub_args)
            elif mode == "eap":
                sibling = src.with_suffix(".eapres")
                sub_args.names_from = str(sibling) if sibling.exists() else None
                mf = cmd_eap(sub_args)
            elif mode == "eapres":
                mf = cmd_eapres(sub_args)
            else:
                continue
            manifests.append(mf)
            _print_summary(mf)
        except BaseException as exc:  # noqa: BLE001 - propagate everything as a failure record
            failures.append((src, exc))
            print(f"  ERROR: {exc}", file=sys.stderr)

    # ---- Aggregate roll-up ------------------------------------------------
    print()
    print("=" * 72)
    print(f"== Batch summary: {len(manifests)} succeeded, {len(failures)} failed ==")
    print()

    total_by_class: Counter = Counter()
    rows: List[Tuple[str, str, int, int]] = []  # (name, type, total, bmp)
    for mf in manifests:
        bc: Counter = Counter()
        for r in mf.resources:
            cls = r.get("className", "?")
            bc[cls] += 1
            total_by_class[cls] += 1
        rows.append((
            Path(mf.source).name,
            mf.source_type,
            len(mf.resources),
            bc.get("BitmapBMP", 0),
        ))

    print(f"  {'source':<32s} {'type':<8s} {'total':>6s} {'BMP':>4s}")
    print("  " + "-" * 60)
    for name, typ, total, bmp in rows:
        marker = "   <-- has BitmapBMP" if bmp > 0 else ""
        print(f"  {name:<32s} {typ:<8s} {total:>6d} {bmp:>4d}{marker}")
    if total_by_class:
        print()
        print("  Aggregate class distribution across all packs:")
        for k, v in total_by_class.most_common():
            print(f"    {k:<20s} {v}")

    if failures:
        print()
        print("Failures:")
        for src, exc in failures:
            print(f"  {src.name}: {exc}")
        return len(failures)
    return 0


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

    def _add_clean_flag(p: argparse.ArgumentParser) -> None:
        # Default to ``--clean`` so the output directory is a true
        # snapshot of what the current decoders produce. ``--no-clean``
        # opts into incremental iteration (overwrite-in-place) for the
        # cases where wiping a 1+ GB extraction tree just to add one
        # missing JSON is wasteful.
        p.add_argument(
            "--clean",
            dest="clean",
            action="store_true",
            default=True,
            help="Wipe the output directory before unpacking (default).",
        )
        p.add_argument(
            "--no-clean",
            dest="clean",
            action="store_false",
            help="Keep existing files in the output directory (overwrite in place).",
        )

    p_overlay = sub.add_parser("overlay", help="Unpack the PE overlay of bulanci.exe")
    p_overlay.add_argument("input", help="Path to bulanci.exe")
    p_overlay.add_argument("-o", "--output", required=True, help="Output directory")
    _add_clean_flag(p_overlay)
    p_overlay.set_defaults(func=cmd_overlay)

    p_eap = sub.add_parser("eap", help="Unpack a compiled .eap level bundle")
    p_eap.add_argument("input")
    p_eap.add_argument("-o", "--output", required=True)
    p_eap.add_argument(
        "--names-from",
        help="Path to the matching .eapres for resource-name resolution "
        "(defaults to the sibling file with the same basename)",
    )
    _add_clean_flag(p_eap)
    p_eap.set_defaults(func=cmd_eap)

    p_eapres = sub.add_parser("eapres", help="Decompress a .eapres editor-level XML")
    p_eapres.add_argument("input")
    p_eapres.add_argument("-o", "--output", required=True)
    _add_clean_flag(p_eapres)
    p_eapres.set_defaults(func=cmd_eapres)

    p_auto = sub.add_parser("auto", help="Auto-detect source type from file extension")
    p_auto.add_argument("input")
    p_auto.add_argument("-o", "--output", required=True)
    _add_clean_flag(p_auto)
    p_auto.set_defaults(func=cmd_auto)

    p_all = sub.add_parser(
        "all",
        help="Batch-unpack bulanci.exe + every *.eap / *.eapres under a directory",
        description=(
            "Walk an input directory and route each supported file through "
            "the matching unpack mode (overlay / eap / eapres). bulanci.exe "
            "lands in <output>/overlay/, each <name>.eap in <output>/<name>_eap/, "
            "each <name>.eapres in <output>/<name>_eapres/. .eap files are "
            "auto-paired with their sibling .eapres for resource-name "
            "resolution."
        ),
    )
    p_all.add_argument(
        "-i", "--input-dir", default="orig",
        help="Directory to scan for inputs (default: orig)",
    )
    p_all.add_argument(
        "-o", "--output-dir", default="unpacked",
        help="Root directory for per-pack output subdirs (default: unpacked)",
    )
    p_all.add_argument(
        "--clean", dest="clean", action="store_true", default=True,
        help="Wipe each target subdirectory before unpacking (default)",
    )
    p_all.add_argument(
        "--no-clean", dest="clean", action="store_false",
        help="Keep existing files in target subdirs (overwrite in place)",
    )
    p_all.set_defaults(func=cmd_all)

    args = parser.parse_args(argv)
    try:
        result = args.func(args)
    except Exception as exc:  # noqa: BLE001
        print(f"ERROR: {exc}", file=sys.stderr)
        return 2
    if isinstance(result, Manifest):
        _print_summary(result)
        return 0
    if isinstance(result, int):
        return result
    return 0


if __name__ == "__main__":
    sys.exit(main())
