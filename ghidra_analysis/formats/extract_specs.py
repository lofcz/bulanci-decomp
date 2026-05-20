"""One-shot helper: for each CDSScript / CLevelScript opcode handler we know,
disassemble a chunk and count CALL targets so we can derive arg layouts:

  CALL 0x004384c0  -> reads one sub-command
  CALL 0x00438350  -> reads one i32
  CALL 0x00438380  -> reads one u8
  CALL 0x00438360  -> reads one u16

The script does NOT depend on Ghidra; it consumes the JSON files already saved
under `tools/bulanci_unpack/ghidra_analysis/handlers/`.

Usage (offline): pipe the decompiled assembly snippets from Ghidra MCP into a
plain-text file per handler, then this script will read them.
"""
from __future__ import annotations
import re
import sys
from pathlib import Path

DISP = "0x004384c0"
READ_I32 = "0x00438350"
READ_U16 = "0x00438360"
READ_U8 = "0x00438380"

PATTERNS = [
    (DISP, "sub"),
    (READ_I32, "i32"),
    (READ_U16, "u16"),
    (READ_U8, "u8"),
]


def analyze(text: str) -> list[str]:
    spec: list[str] = []
    for line in text.splitlines():
        m = re.search(r"CALL\s+(0x[0-9a-fA-F]+)", line)
        if not m:
            continue
        tgt = m.group(1).lower()
        for pat, tag in PATTERNS:
            if tgt == pat:
                spec.append(tag)
    return spec


def main(argv: list[str]) -> int:
    folder = Path(argv[1])
    for p in sorted(folder.glob("*.asm")):
        s = analyze(p.read_text())
        print(f"{p.stem}: {s}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
