"""Cross-verify the recovered DSM header schema across every ClassID 76 file."""
import glob
import os
import struct


def main() -> None:
    files = sorted(glob.glob(r"unpacked\overlay\res_*_76_BitmapJpegAnim.bin"))
    print(f"Total: {len(files)} BitmapJpegAnim files\n")

    formats = set()
    bdw1_vals = set()
    rates = set()
    mismatches = []
    for f in files:
        data = open(f, "rb").read()
        end = struct.unpack_from("<I", data, 0)[0]
        w, h, fmt, dur_ms, frames = struct.unpack_from("<5I", data, 4)
        audio_bytes, packed, rate = struct.unpack_from("<3I", data, 24)
        channels = packed & 0xFFFF
        bits = (packed >> 16) & 0xFFFF
        expected_audio = (dur_ms * rate * channels * bits) // 8 // 1000
        end_ok = end == len(data)
        fps = frames * 1000 / dur_ms if dur_ms else 0
        print(
            f"{os.path.basename(f):>50s}  "
            f"size={len(data):>8d}  endOk={end_ok}  "
            f"W={w:>4d} H={h:>4d} fmt={fmt}  "
            f"dur={dur_ms:>6d}ms N={frames:>4d}  fps={fps:>5.2f}  "
            f"audio={audio_bytes:>8d} (calc={expected_audio}, d={audio_bytes - expected_audio:+d})  "
            f"ch={channels} bits={bits} rate={rate}"
        )
        formats.add(fmt)
        bdw1_vals.add(packed)
        rates.add(rate)
        if abs(audio_bytes - expected_audio) > 8 or not end_ok:
            mismatches.append((os.path.basename(f), audio_bytes - expected_audio, end_ok))

    print()
    print(f"Formats observed: {sorted(formats)}")
    print(f"B_dw1 (channels|bits) observed: {[hex(v) for v in sorted(bdw1_vals)]}")
    print(f"B_dw2 (rate) observed: {sorted(rates)}")
    print(f"Mismatches (file, audio_diff, end_ok): {mismatches}")


if __name__ == "__main__":
    main()
