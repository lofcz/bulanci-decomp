"""FFT-based spectrum comparison of two WASAPI captures of the hover SFX.

Picks the loudest contiguous 256 ms slice in each recording (where the
hover SFX dominates over any BG music tail), windows it, runs an FFT,
and prints the top bins for both files. If our rodio playback and
retail's DirectSound playback are at the same pitch, the dominant bins
should overlap closely.
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


def loudest_window(samp, sr, window_ms=256):
    win = int(sr * window_ms / 1000)
    if len(samp) <= win:
        return samp, 0
    # Find offset maximizing RMS over a window
    pow2 = samp * samp
    csum = np.concatenate([[0.0], np.cumsum(pow2)])
    energies = csum[win:] - csum[:-win]
    best = int(np.argmax(energies))
    return samp[best : best + win], best


def main():
    us_path, retail_path = sys.argv[1], sys.argv[2]
    us, us_sr = read_wav_mono(us_path)
    re, re_sr = read_wav_mono(retail_path)

    us_slice, us_off = loudest_window(us, us_sr)
    re_slice, re_off = loudest_window(re, re_sr)

    print(f"us    : sr={us_sr}  loudest 256ms window at {us_off/us_sr*1000:.1f} ms")
    print(f"retail: sr={re_sr}  loudest 256ms window at {re_off/re_sr*1000:.1f} ms")
    print()

    # Hann window + FFT.
    def spectrum(x, sr):
        w = np.hanning(len(x))
        X = np.fft.rfft(x * w)
        mag = np.abs(X)
        freqs = np.fft.rfftfreq(len(x), 1.0 / sr)
        return freqs, mag

    fU, mU = spectrum(us_slice, us_sr)
    fR, mR = spectrum(re_slice, re_sr)
    mU /= mU.max() + 1e-12
    mR /= mR.max() + 1e-12

    print("Top 12 spectral peaks (0..10 kHz, normalized to per-file peak):")
    print(f"  {'us Hz':>8} {'us mag':>8}    {'retail Hz':>10} {'retail mag':>10}")

    def top_peaks(freqs, mag, k=12, fmax=10000.0):
        keep = freqs <= fmax
        f = freqs[keep]
        m = mag[keep]
        # naive local max: bigger than left/right neighbor
        peaks = []
        for i in range(2, len(m) - 2):
            if m[i] >= m[i - 1] and m[i] >= m[i + 1] and m[i] > 0.05:
                peaks.append((m[i], f[i]))
        peaks.sort(reverse=True)
        return peaks[:k]

    pu = top_peaks(fU, mU)
    pr = top_peaks(fR, mR)
    for i in range(max(len(pu), len(pr))):
        if i < len(pu):
            mu, fu = pu[i]
            su = f"{fu:>8.0f} {mu:>8.3f}"
        else:
            su = " " * 17
        if i < len(pr):
            mri, fri = pr[i]
            sr_ = f"{fri:>10.0f} {mri:>10.3f}"
        else:
            sr_ = " " * 21
        print(f"  {su}    {sr_}")

    # Centroid for quick "is pitch shifted?" check.
    cent_u = (fU[fU <= 8000] * mU[fU <= 8000]).sum() / (mU[fU <= 8000].sum() + 1e-12)
    cent_r = (fR[fR <= 8000] * mR[fR <= 8000]).sum() / (mR[fR <= 8000].sum() + 1e-12)
    print(f"\nSpectral centroid below 8 kHz:")
    print(f"  us    : {cent_u:.0f} Hz")
    print(f"  retail: {cent_r:.0f} Hz")
    print(f"  ratio us/retail = {cent_u/cent_r:.3f} (1.0 => same pitch, 2.0 => 1 octave higher)")


if __name__ == "__main__":
    main()
