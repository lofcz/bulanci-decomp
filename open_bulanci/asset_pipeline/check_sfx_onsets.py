"""Tiny diagnostic: print the first 20 samples and onset stats for menu SFX.

Used to investigate whether sfx_hover.wav has a sharp-step onset that
DirectSound smooths but rodio/cpal lets through audibly.
"""

import os
import struct
from pathlib import Path

REPO_ROOT = Path(__file__).parent.parent.parent

SAMPLES = [
    "sfx_hover.wav",
    "sfx_radio_click.wav",
    "sfx_history.wav",
    "sfx_quit.wav",
    "sfx_start.wav",
    "sfx_force_exit.wav",
    "sfx_alt_exit.wav",
]


def main() -> None:
    print(f"{'sample':<22} sr   ch  first 10 samples (mono)   peak50  step5")
    print("-" * 100)
    for name in SAMPLES:
        path = str(REPO_ROOT / "open_bulanci" / "assets" / "audio" / name)
        if not os.path.exists(path):
            print(f"{name} MISSING")
            continue
        with open(path, "rb") as f:
            data = f.read()
        ch = struct.unpack_from("<H", data, 22)[0]
        sr = struct.unpack_from("<I", data, 24)[0]
        bits = struct.unpack_from("<H", data, 34)[0]
        if bits != 16:
            print(f"{name} bits={bits} unsupported")
            continue
        pcm = data[44:]
        n = len(pcm) // 2
        samp = struct.unpack("<" + "h" * n, pcm)
        mono = samp[::ch] if ch > 1 else samp
        first10 = mono[:10]
        first50 = mono[:50]
        peak50 = max(abs(x) for x in first50) if first50 else 0
        # Largest one-sample step in the first 5 transitions.
        step5 = max(abs(first10[i] - first10[i - 1]) for i in range(1, min(6, len(first10))))
        print(f"{name:<22} {sr:<5} {ch:<3} {list(first10)}  peak50={peak50}  step5={step5}")


if __name__ == "__main__":
    main()
