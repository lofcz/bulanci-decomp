"""High-quality polyphase resampling for SFX WAVs.

Why this exists
---------------
`rodio 0.17.3`'s built-in `SampleRateConverter` does **linear interpolation
between integer-sample pairs**.  Linear interpolation has the spectral
response of a triangular window, which is not band-limited — every
input transient (sharp onset, percussive attack) produces aliasing
in the upper half of the output spectrum.

For `sfx_hover.wav` the source starts mid-oscillation at
`-27, -206, -381, -428` (sample 0..4), which has dominant content
around 2 kHz with rich harmonics out to the source's Nyquist (11 kHz).
Linear-interpolating that up to 48 kHz creates audible ~5 kHz noise
during the first ~100 ms of every playback.  WASAPI / Windows Media
Player don't exhibit this because the Windows shared-mode mixer's
resampler is a high-order polyphase filter — but cpal/rodio doesn't
hand the source rate to WASAPI; it resamples first, then feeds the
already-corrupted stream.

The cleanest fix is to **store the SFX assets at the runtime's actual
output rate** (48 kHz on virtually every modern Windows system) so
rodio's `SampleRateConverter` sees `from == to` and short-circuits
the linear interpolation entirely (see
`rodio/src/conversions/sample_rate.rs` lines 70-72 and 129-132).

scipy's `resample_poly` implements a windowed-sinc (Kaiser window)
polyphase filter that is band-limited at the destination Nyquist —
exactly what Windows itself does, and what retail's DirectSound
shared-mode path also gets.

Usage
-----
    python scripts/resample_sfx.py input.wav output.wav [--rate 48000]
"""

from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path

import numpy as np
from scipy.signal import resample_poly


def read_wav_mono16(path: Path) -> tuple[np.ndarray, int]:
    """Read a 16-bit mono PCM WAV. Raises if format is unsupported."""
    data = path.read_bytes()
    if data[:4] != b"RIFF" or data[8:12] != b"WAVE":
        raise ValueError(f"{path} is not a RIFF/WAVE file")

    # Walk chunks until we find fmt + data
    pos = 12
    fmt = None
    pcm = None
    while pos + 8 <= len(data):
        chunk_id = data[pos:pos + 4]
        chunk_size = struct.unpack_from("<I", data, pos + 4)[0]
        body = data[pos + 8 : pos + 8 + chunk_size]
        if chunk_id == b"fmt ":
            fmt = body
        elif chunk_id == b"data":
            pcm = body
        pos += 8 + chunk_size + (chunk_size & 1)  # word-aligned

    if fmt is None or pcm is None:
        raise ValueError(f"{path} is missing fmt or data chunk")

    audio_format, channels, sample_rate, _byte_rate, _block_align, bits = struct.unpack_from("<HHIIHH", fmt, 0)
    if audio_format != 1 or bits != 16 or channels != 1:
        raise ValueError(
            f"{path} is not 16-bit mono PCM (audio_format={audio_format}, channels={channels}, bits={bits})"
        )

    samples = np.frombuffer(pcm, dtype="<i2")
    return samples, sample_rate


def write_wav_mono16(path: Path, samples: np.ndarray, sample_rate: int) -> None:
    """Write 16-bit mono PCM WAV with a minimal RIFF header."""
    if samples.dtype != np.int16:
        samples = np.clip(samples, -32768, 32767).astype(np.int16)

    data_bytes = samples.tobytes()
    fmt_chunk = struct.pack("<HHIIHH", 1, 1, sample_rate, sample_rate * 2, 2, 16)
    riff = (
        b"RIFF"
        + struct.pack("<I", 36 + len(data_bytes))
        + b"WAVE"
        + b"fmt " + struct.pack("<I", len(fmt_chunk)) + fmt_chunk
        + b"data" + struct.pack("<I", len(data_bytes)) + data_bytes
    )
    path.write_bytes(riff)


def resample_int16(samples: np.ndarray, src_rate: int, dst_rate: int) -> np.ndarray:
    """Polyphase resample with a Kaiser-windowed sinc kernel."""
    if src_rate == dst_rate:
        return samples.copy()

    # Reduce to coprime ratio so the polyphase factor stays small
    from math import gcd
    g = gcd(src_rate, dst_rate)
    up = dst_rate // g
    down = src_rate // g

    # Default Kaiser window with high stopband attenuation (~100 dB).
    # The default filter length is automatically chosen by scipy based on
    # `up`/`down`. We don't override it — scipy.signal.resample_poly's
    # default already meets audio quality.
    resampled = resample_poly(samples.astype(np.float64), up, down)
    return np.clip(resampled, -32768, 32767).round().astype(np.int16)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=Path, help="source WAV (16-bit mono PCM)")
    parser.add_argument("output", type=Path, help="destination WAV (same format, new rate)")
    parser.add_argument("--rate", type=int, default=48000, help="target sample rate (default 48000)")
    args = parser.parse_args()

    samples, src_rate = read_wav_mono16(args.input)
    print(f"  {args.input.name}: {len(samples)} samples @ {src_rate} Hz ({len(samples) * 1000 / src_rate:.1f} ms)")
    resampled = resample_int16(samples, src_rate, args.rate)
    write_wav_mono16(args.output, resampled, args.rate)
    print(f"→ {args.output.name}: {len(resampled)} samples @ {args.rate} Hz ({len(resampled) * 1000 / args.rate:.1f} ms)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
