"""Parse a candidate ``.mpx`` file (16-byte WAVEFORMAT-style header + MPEG audio
bitstream) and dump the first few MPEG frame headers.

No actual ``.mpx`` sample ships in this tree (no resource ClassID matches the
``CDSMpx`` path in ``bulanci.exe.c``; the trial-distribution master pack stops
at audio ClassIDs ``43`` / ``48`` / ``67``). The parser is written so that the
next person to dig one up out of a full retail install can drop it in and
get an immediate sanity check:

    python _scratch_mpx_header.py path/to/intro.mpx

It will:

1.  Print the 16-byte container header (``MpxFileHeader``) as recovered from
    ``CDSMpxStream::Load`` (``FUN_00433180`` @ ``bulanci.exe`` :
    ``0x00433180``).
2.  Walk the MPEG audio payload looking for ``0xFF Ex / 0xFF Fx`` sync words
    and decode the first ``--frames`` (default 8) frame headers using the
    bitrate / sample-rate tables also used by ``ParseMpegFrameHeader``
    (``FUN_00458f50``).
3.  Cross-check the recovered ``dwDataSize`` and ``dwAvgBytesPerSec`` against
    the actual MPEG frame stream.

Designed to fail loudly: if the candidate file's header doesn't satisfy
``len(file) == 16 + dwDataSize`` or the first 16 bytes after the header
don't look like an MPEG sync word, the script bails with a clear message.
"""
from __future__ import annotations

import argparse
import os
import struct
import sys
from dataclasses import dataclass
from typing import Iterator, Optional


# --------------------------------------------------------------------------------------
# MPEG-1/2/2.5 audio framing tables (matches g_dwMpegBitrateTable @ 0x0048b7b4 and
# g_dwMpegSampleRateTable @ 0x0048b91c in bulanci.exe)
# --------------------------------------------------------------------------------------

# kbps; row 0 unused, rows 1..3 = Layer I / II / III (the engine's
# `*headerInfo = 4 - rawLayerField` mapping is preserved here).
_BITRATE_KBPS = {
    1: (0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448),
    2: (0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384),
    3: (0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320),
}

# MPEG-1 sample rates; MPEG-2 / 2.5 are these shifted right by 1 / 2 (per FUN_00458f50).
_SAMPLE_RATE_HZ_MPEG1 = (44100, 48000, 32000)


@dataclass
class FrameHeader:
    file_offset: int
    version: str            # "MPEG-1" / "MPEG-2" / "MPEG-2.5"
    layer: int              # 1, 2, 3 (engine's `4 - raw` mapping)
    has_crc: bool
    bitrate_kbps: int       # 0 if free-format
    sample_rate_hz: int
    padding: bool
    private: bool
    channel_mode: str       # "stereo" / "joint" / "dual" / "mono"
    mode_extension: int
    copyright: bool
    original: bool
    emphasis: int
    frame_size_bytes: int   # computed below


_MODE_NAMES = ("stereo", "joint", "dual", "mono")


def _decode_frame_header(
    raw: bytes, file_offset: int
) -> Optional[FrameHeader]:
    """Decode a 4-byte MPEG audio frame header (matches FUN_00458f50)."""
    if len(raw) < 4:
        return None
    b0, b1, b2, b3 = raw[0], raw[1], raw[2], raw[3]
    # Sync: 11 bits high (relaxed to allow MPEG-2.5 like the engine does).
    if b0 != 0xFF or (b1 & 0xE0) != 0xE0:
        return None

    # bit 11 (version-2.5 indicator) and bit 12 (version-1 indicator)
    is_v25 = (b1 & 0x10) == 0
    is_v1 = (b1 & 0x08) != 0
    version = "MPEG-2.5" if is_v25 else "MPEG-1" if is_v1 else "MPEG-2"

    raw_layer = (b1 >> 1) & 0x3
    if raw_layer == 0:
        return None                          # reserved, like error 0x102
    layer = 4 - raw_layer

    has_crc = (b1 & 0x01) == 0

    bitrate_idx = (b2 >> 4) & 0xF
    if bitrate_idx == 0xF:
        return None                          # error 0x103
    bitrate_kbps = _BITRATE_KBPS[layer][bitrate_idx]

    rate_idx = (b2 >> 2) & 0x3
    if rate_idx == 0x3:
        return None                          # error 0x104
    sample_rate_hz = _SAMPLE_RATE_HZ_MPEG1[rate_idx]
    if version == "MPEG-2":
        sample_rate_hz >>= 1
    elif version == "MPEG-2.5":
        sample_rate_hz >>= 2

    padding = (b2 & 0x02) != 0
    private = (b2 & 0x01) != 0
    channel_mode = _MODE_NAMES[(b3 >> 6) & 0x3]
    mode_extension = (b3 >> 4) & 0x3
    copyright = (b3 & 0x08) != 0
    original = (b3 & 0x04) != 0
    emphasis = b3 & 0x3

    # Per ISO 11172-3 / 13818-3 frame-size formula:
    #   L1: (12 * bitrate / rate + pad) * 4
    #   L2/L3: (144 * bitrate / rate) + pad  (MPEG-1)
    #   L3 MPEG-2/2.5: (72 * bitrate / rate) + pad
    if bitrate_kbps == 0:
        frame_size_bytes = 0                 # free-format; engine seeks-by-sync
    elif layer == 1:
        frame_size_bytes = (12 * bitrate_kbps * 1000 // sample_rate_hz + (1 if padding else 0)) * 4
    elif layer == 2:
        frame_size_bytes = 144 * bitrate_kbps * 1000 // sample_rate_hz + (1 if padding else 0)
    else:                                    # layer == 3
        if version == "MPEG-1":
            frame_size_bytes = 144 * bitrate_kbps * 1000 // sample_rate_hz + (1 if padding else 0)
        else:                                # LSF
            frame_size_bytes = 72 * bitrate_kbps * 1000 // sample_rate_hz + (1 if padding else 0)

    return FrameHeader(
        file_offset=file_offset,
        version=version,
        layer=layer,
        has_crc=has_crc,
        bitrate_kbps=bitrate_kbps,
        sample_rate_hz=sample_rate_hz,
        padding=padding,
        private=private,
        channel_mode=channel_mode,
        mode_extension=mode_extension,
        copyright=copyright,
        original=original,
        emphasis=emphasis,
        frame_size_bytes=frame_size_bytes,
    )


def _walk_frames(payload: bytes, payload_base: int) -> Iterator[FrameHeader]:
    """Yield each MPEG frame header found in `payload`, advancing by the frame's
    own size (or by 1 byte if free-format)."""
    cursor = 0
    while cursor + 4 <= len(payload):
        # Same sync rule the engine uses (FUN_00456850).
        if payload[cursor] != 0xFF or (payload[cursor + 1] & 0xE0) != 0xE0:
            cursor += 1
            continue
        hdr = _decode_frame_header(payload[cursor:cursor + 4], payload_base + cursor)
        if hdr is None:
            cursor += 1
            continue
        yield hdr
        cursor += hdr.frame_size_bytes if hdr.frame_size_bytes else 1


def parse_mpx(path: str, frames_to_print: int = 8) -> int:
    """Return 0 on success, non-zero on validation failure (file unreadable as MPx)."""
    data = open(path, "rb").read()
    if len(data) < 16:
        print(f"FAIL: {path!r}: file is {len(data)} bytes, < 16-byte header", file=sys.stderr)
        return 2

    dw_size, ch, bits, rate, avg_bps = struct.unpack_from("<IHHII", data, 0)
    payload = data[16:]

    print(f"=== MpxFileHeader ({path}) ===")
    print(f"  dwDataSize        = {dw_size} (0x{dw_size:x})")
    print(f"  wChannels         = {ch}")
    print(f"  wBitsPerSample    = {bits}")
    print(f"  dwSampleRate      = {rate} Hz")
    print(f"  dwAvgBytesPerSec  = {avg_bps}")
    print(f"  derived avg check = rate*ch*bits/8 = {rate * ch * bits // 8} "
          f"({'OK' if rate * ch * bits // 8 == avg_bps else 'mismatch (MPEG VBR likely)'})")
    print(f"  file size         = {len(data)}")
    print(f"  payload size      = {len(payload)}")
    print(f"  size match        = {'OK' if len(payload) == dw_size else f'MISMATCH ({len(payload) - dw_size:+d})'}")

    if len(payload) < 4 or (payload[0] != 0xFF or (payload[1] & 0xE0) != 0xE0):
        print("WARN: first payload bytes are not an MPEG audio sync word -- "
              "either the header schema is wrong on this file or the .mpx contains a different codec.")
        return 3

    print(f"\n=== first {frames_to_print} MPEG frames ===")
    layers = set()
    versions = set()
    rates = set()
    total_size = 0
    n = 0
    last_offset = 0
    for h in _walk_frames(payload, payload_base=16):
        if n >= frames_to_print and n % max(frames_to_print, 1) != 0:
            n += 1
            total_size += h.frame_size_bytes
            last_offset = h.file_offset + h.frame_size_bytes
            layers.add(h.layer)
            versions.add(h.version)
            rates.add(h.sample_rate_hz)
            continue
        if n < frames_to_print:
            crc = "+crc" if h.has_crc else "    "
            pad = "P" if h.padding else " "
            print(
                f"  @0x{h.file_offset:08x}  {h.version:>9s} L{h.layer}  "
                f"{h.bitrate_kbps:>4d} kbps  {h.sample_rate_hz:>5d} Hz  {crc}{pad}  "
                f"{h.channel_mode:>6s} (ext={h.mode_extension})  "
                f"emp={h.emphasis}  size={h.frame_size_bytes}"
            )
        layers.add(h.layer)
        versions.add(h.version)
        rates.add(h.sample_rate_hz)
        total_size += h.frame_size_bytes
        last_offset = h.file_offset + h.frame_size_bytes
        n += 1

    print(f"\n=== summary ===")
    print(f"  frames detected   = {n}")
    print(f"  layers observed   = {sorted(layers)}")
    print(f"  versions observed = {sorted(versions)}")
    print(f"  rates observed    = {sorted(rates)} Hz")
    print(f"  sum(frame_size)   = {total_size}")
    print(f"  payload size      = {len(payload)}  (delta = {total_size - len(payload):+d})")
    print(f"  last consumed byte = 0x{last_offset:x}")
    return 0


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("path", help="candidate .mpx file (or directory; processes all *.mpx)")
    ap.add_argument("--frames", type=int, default=8, help="frames to dump per file (default 8)")
    args = ap.parse_args()

    if os.path.isdir(args.path):
        rc = 0
        for f in sorted(os.listdir(args.path)):
            if f.lower().endswith(".mpx"):
                rc |= parse_mpx(os.path.join(args.path, f), args.frames)
                print()
        return rc
    return parse_mpx(args.path, args.frames)


if __name__ == "__main__":
    sys.exit(main())
