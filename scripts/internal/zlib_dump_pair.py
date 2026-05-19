"""Side-by-side hex dump for matched-engine helpers to see what /O2 keeps
vs the original. Use to drive the next round of matching tweaks."""
import sys
sys.path.insert(0, "scripts/internal")
from near_miss import coff_function_bytes
from pathlib import Path

t = coff_function_bytes(Path("build/orig/bulanci/_Globals.obj"))
b = coff_function_bytes(Path("build/Src/bulanci/_Globals.obj"))

def hex_lines(label, blob):
    print(f"--- {label} ({len(blob)} bytes) ---")
    for i in range(0, len(blob), 16):
        chunk = blob[i:i+16]
        hexstr = " ".join(f"{x:02x}" for x in chunk)
        print(f"  {i:04x}  {hexstr}")

target = sys.argv[1] if len(sys.argv) > 1 else "_Globals::FUN_00434e30"
hex_lines(f"TARGET {target}", t[target])
hex_lines(f"BASE   {target}", b[target])
