"""Compare the FFT spectra of the first 100 ms of two WASAPI captures.

The user reports a distinct *high-pitched* component "added" to the
beginning of our SFX that retail does not have. We isolate the first
100 ms after each recording's burst onset, run an FFT, and look for
bins where our magnitude exceeds retail's by a meaningful margin.

Anything ours has that retail does NOT, especially above the source's
sample-rate Nyquist of 11025 Hz, is the artifact we are looking for.
"""

import struct
import sys
import numpy as np


def read_wav_mono(path):
    raw = open(path, "rb").read()
    fmt = data = None
    i = 12
    while i + 8 <= len(raw):
        cid = raw[i : i + 4]
        sz = struct.unpack_from("<I", raw, i + 4)[0]
        body = raw[i + 8 : i + 8 + sz]
        if cid == b"fmt ":
            fmt = struct.unpack_from("<HHIIHH", body, 0)
        elif cid == b"data":
            data = body
        i += 8 + sz + (sz & 1)
    _ft, ch, sr, _br, _ba, _bits = fmt
    samp = np.frombuffer(data, dtype="<i2").astype(np.float32) / 32768.0
    if ch == 2:
        samp = (samp[::2] + samp[1::2]) * 0.5
    return samp, sr


def burst_start(x, sr, db=-50.0, hold_ms=5.0):
    peak = float(np.max(np.abs(x))) or 1e-9
    thresh = peak * 10.0 ** (db / 20.0)
    hold = max(1, int(sr * hold_ms / 1000))
    above = np.abs(x) >= thresh
    # find first index where 'hold' consecutive samples are above threshold
    csum = np.concatenate([[0], np.cumsum(above.astype(np.int32))])
    for i in range(0, len(x) - hold):
        if csum[i + hold] - csum[i] == hold:
            return i
    return 0


def spectrum(x, sr):
    w = np.hanning(len(x))
    X = np.fft.rfft(x * w)
    return np.fft.rfftfreq(len(x), 1.0 / sr), np.abs(X)


def main():
    us_path, retail_path = sys.argv[1], sys.argv[2]
    intro_ms = int(sys.argv[3]) if len(sys.argv) > 3 else 100
    us, us_sr = read_wav_mono(us_path)
    re, re_sr = read_wav_mono(retail_path)

    us_start = burst_start(us, us_sr)
    re_start = burst_start(re, re_sr)
    n_us = int(us_sr * intro_ms / 1000)
    n_re = int(re_sr * intro_ms / 1000)
    us_clip = us[us_start : us_start + n_us]
    re_clip = re[re_start : re_start + n_re]
    print(
        f"us    : sr={us_sr}, onset={us_start/us_sr*1000:.1f} ms, "
        f"intro = {len(us_clip)/us_sr*1000:.1f} ms ({len(us_clip)} samples)"
    )
    print(
        f"retail: sr={re_sr}, onset={re_start/re_sr*1000:.1f} ms, "
        f"intro = {len(re_clip)/re_sr*1000:.1f} ms ({len(re_clip)} samples)"
    )

    fU, mU = spectrum(us_clip, us_sr)
    fR, mR = spectrum(re_clip, re_sr)

    # Align bin grids (both files are 48 kHz / same intro length => same bins).
    n_bins = min(len(fU), len(fR))
    fU, mU, fR, mR = fU[:n_bins], mU[:n_bins], fR[:n_bins], mR[:n_bins]

    # Show the spectrum in 500 Hz bands.
    band_hz = 500.0
    nyquist = us_sr / 2.0
    n_bands = int(nyquist // band_hz)
    print(
        f"\n{'band (Hz)':<14}  {'us power':>12}  {'retail power':>14}  "
        f"{'us / retail':>14}  pct of total"
    )
    us_total = float((mU * mU).sum()) or 1e-12
    re_total = float((mR * mR).sum()) or 1e-12
    for b in range(n_bands):
        lo, hi = b * band_hz, (b + 1) * band_hz
        mask = (fU >= lo) & (fU < hi)
        if not mask.any():
            continue
        u_pow = float((mU[mask] * mU[mask]).sum())
        r_pow = float((mR[mask] * mR[mask]).sum())
        ratio = u_pow / max(r_pow, 1e-12)
        u_pct = u_pow / us_total * 100.0
        r_pct = r_pow / re_total * 100.0
        marker = ""
        if hi > 11025 and u_pow > r_pow * 4.0:
            marker = " <-- US has >>4x retail above source Nyquist"
        elif u_pow > r_pow * 4.0 and u_pct > 1.0:
            marker = " <-- US has >>4x retail energy"
        elif r_pow > u_pow * 4.0 and r_pct > 1.0:
            marker = " <-- retail has >>4x us energy"
        print(
            f"{int(lo):>5}-{int(hi):<7}  {u_pow:>12.4f}  {r_pow:>14.4f}  "
            f"{ratio:>14.2f}  us={u_pct:5.2f}% re={r_pct:5.2f}%{marker}"
        )


if __name__ == "__main__":
    main()
