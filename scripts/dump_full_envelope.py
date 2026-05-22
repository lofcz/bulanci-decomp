"""Dump full 10 ms peak envelope of two WASAPI captures, side by side."""

import struct
import sys


def read_wav(path):
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
    n = len(data) // 2
    samp = struct.unpack("<" + "h" * n, data)
    flt = [s / 32768.0 for s in samp]
    mono = [(flt[2 * k] + flt[2 * k + 1]) * 0.5 for k in range(len(flt) // 2)] if ch == 2 else flt
    return mono, sr


def env(x, sr, ms=10):
    w = int(sr * ms / 1000)
    out = []
    k = 0
    while k < len(x):
        c = x[k : k + w]
        out.append(max(abs(v) for v in c) if c else 0)
        k += w
    return out


def main():
    us, us_sr = read_wav(sys.argv[1])
    re, re_sr = read_wav(sys.argv[2])
    ue = env(us, us_sr)
    re_e = env(re, re_sr)
    print("=== full 10ms peak envelope ===")
    print(f"{'t(ms)':>6} {'us':>8} {'retail':>8}")
    n = max(len(ue), len(re_e))
    for w in range(n):
        u = ue[w] if w < len(ue) else 0
        r = re_e[w] if w < len(re_e) else 0
        marker = ""
        if u > 0.5 or r > 0.5:
            marker = " <- big"
        elif u > 0.1 or r > 0.1:
            marker = " <- med"
        print(f"{w*10:>6} {u:>8.3f} {r:>8.3f}{marker}")


if __name__ == "__main__":
    main()
