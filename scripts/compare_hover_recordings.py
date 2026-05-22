"""Compare two WASAPI recordings of the hover SFX side by side.

Usage:
    python scripts/compare_hover_recordings.py <us.wav> <retail.wav>

Reports format, RMS envelope per ~5 ms window, broad-band peak, and
attempts to time-align the two recordings via cross-correlation on the
amplitude envelope. The goal is to make any spectral/onset differences
between our rodio playback and retail's DirectSound playback visible
without ever loading the files into a DAW.
"""

import struct
import sys
import math
from pathlib import Path


def read_wav(path):
    """Tiny WAV reader: handles standard PCM 8/16/24/32 mono or stereo."""
    raw = Path(path).read_bytes()
    if raw[:4] != b"RIFF" or raw[8:12] != b"WAVE":
        raise RuntimeError(f"{path}: not a RIFF/WAVE file")
    i = 12
    fmt = None
    data = None
    while i + 8 <= len(raw):
        chunk_id = raw[i : i + 4]
        chunk_sz = struct.unpack_from("<I", raw, i + 4)[0]
        body = raw[i + 8 : i + 8 + chunk_sz]
        if chunk_id == b"fmt ":
            fmt_tag, ch, sr, _br, _ba, bits = struct.unpack_from("<HHIIHH", body, 0)
            fmt = (fmt_tag, ch, sr, bits)
        elif chunk_id == b"data":
            data = body
        i += 8 + chunk_sz + (chunk_sz & 1)
    if fmt is None or data is None:
        raise RuntimeError(f"{path}: missing fmt or data chunk")
    fmt_tag, ch, sr, bits = fmt
    if fmt_tag not in (1, 3):
        raise RuntimeError(f"{path}: unsupported fmt_tag={fmt_tag}")
    if bits == 16 and fmt_tag == 1:
        n = len(data) // 2
        samp = struct.unpack("<" + "h" * n, data)
        flt = [s / 32768.0 for s in samp]
    elif bits == 32 and fmt_tag == 3:
        n = len(data) // 4
        flt = list(struct.unpack("<" + "f" * n, data))
    elif bits == 24 and fmt_tag == 1:
        n = len(data) // 3
        out = []
        for k in range(n):
            b0, b1, b2 = data[3 * k], data[3 * k + 1], data[3 * k + 2]
            v = b0 | (b1 << 8) | (b2 << 16)
            if v >= (1 << 23):
                v -= 1 << 24
            out.append(v / float(1 << 23))
        flt = out
    elif bits == 32 and fmt_tag == 1:
        n = len(data) // 4
        samp = struct.unpack("<" + "i" * n, data)
        flt = [s / 2147483648.0 for s in samp]
    else:
        raise RuntimeError(f"{path}: unsupported bits={bits} fmt_tag={fmt_tag}")
    if ch == 2:
        mono = [(flt[2 * k] + flt[2 * k + 1]) * 0.5 for k in range(len(flt) // 2)]
    else:
        mono = flt
    return mono, sr, ch, bits


def envelope(mono, sr, window_ms):
    win = max(1, int(sr * window_ms / 1000))
    out = []
    k = 0
    while k < len(mono):
        chunk = mono[k : k + win]
        peak = max((abs(x) for x in chunk), default=0.0)
        rms = math.sqrt(sum(x * x for x in chunk) / max(1, len(chunk)))
        out.append((peak, rms))
        k += win
    return out, win


def find_burst_start(mono, sr, noise_floor_db=-60.0, hold_ms=5.0):
    """Index of the first sample sustaining `hold_ms` of audio above the
    given dB threshold (relative to peak)."""
    if not mono:
        return 0
    peak = max(abs(x) for x in mono) or 1e-9
    thresh = peak * (10.0 ** (noise_floor_db / 20.0))
    hold = max(1, int(sr * hold_ms / 1000))
    run = 0
    for i, x in enumerate(mono):
        if abs(x) >= thresh:
            run += 1
            if run >= hold:
                return i - hold + 1
        else:
            run = 0
    return 0


def main():
    if len(sys.argv) != 3:
        print(__doc__)
        sys.exit(1)
    us_path, retail_path = sys.argv[1], sys.argv[2]
    us, us_sr, us_ch, us_bits = read_wav(us_path)
    re, re_sr, re_ch, re_bits = read_wav(retail_path)

    print("== format ==")
    print(f"  us    : sr={us_sr} ch={us_ch} bits={us_bits} samples={len(us)} "
          f"duration={len(us)/us_sr*1000:.1f} ms")
    print(f"  retail: sr={re_sr} ch={re_ch} bits={re_bits} samples={len(re)} "
          f"duration={len(re)/re_sr*1000:.1f} ms")
    print()

    if us_sr != re_sr:
        print("WARNING: sample rate mismatch; downstream timings still use each file's sr.\n")

    us_peak = max(abs(x) for x in us) or 1e-9
    re_peak = max(abs(x) for x in re) or 1e-9
    print("== peak amplitude (full-scale = 1.0) ==")
    print(f"  us    : {us_peak:.4f}  ({20*math.log10(us_peak):+.2f} dBFS)")
    print(f"  retail: {re_peak:.4f}  ({20*math.log10(re_peak):+.2f} dBFS)")
    print()

    # Align bursts so envelopes are directly comparable.
    us_start = find_burst_start(us, us_sr)
    re_start = find_burst_start(re, re_sr)
    print("== first audible onset (-60 dB rel. peak, 5 ms hold) ==")
    print(f"  us    : sample {us_start} ({us_start/us_sr*1000:.1f} ms)")
    print(f"  retail: sample {re_start} ({re_start/re_sr*1000:.1f} ms)")
    print()

    # Trim leading silence to the onset for both, normalize amplitudes to
    # peak=1 so we compare shape, and show 5 ms RMS windows.
    us_align = [x / us_peak for x in us[us_start:]]
    re_align = [x / re_peak for x in re[re_start:]]

    print("== normalized 5 ms windowed envelope (peak | rms), us vs retail ==")
    us_env, _ = envelope(us_align, us_sr, 5.0)
    re_env, _ = envelope(re_align, re_sr, 5.0)
    n = min(len(us_env), len(re_env), 80)
    print(f"  t(ms) |     us peak  rms     |   retail peak  rms   | dpeak  drms")
    for w in range(n):
        up, ur = us_env[w]
        rp, rr = re_env[w]
        dp = up - rp
        dr = ur - rr
        print(f"  {w*5:5d} | {up:8.3f}  {ur:8.3f}   | {rp:8.3f}  {rr:8.3f}  | "
              f"{dp:+6.3f} {dr:+6.3f}")

    # Sub-sample look at the absolute first samples after onset, to see
    # any sharp-step transient.
    print()
    print("== first 32 normalized samples after burst onset ==")
    print("  us    :", " ".join(f"{us_align[i]:+.3f}" for i in range(min(32, len(us_align)))))
    print("  retail:", " ".join(f"{re_align[i]:+.3f}" for i in range(min(32, len(re_align)))))


if __name__ == "__main__":
    main()
