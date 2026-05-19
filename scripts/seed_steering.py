"""Seed bulanci-specific guidance into decomp-goal-harness steering leads.

The harness injects recent steering leads into every prompt generated
by ``decomp-goal goal`` / ``decomp-goal codex``. We use that to push
project-specific rules (compiler, typedefs, stub layout, Ghidra MCP
URL, etc.) into the agent's context without forking the harness.

Run once after cloning the repo (idempotent: the harness keeps every
lead, but duplicates do no harm beyond cluttering history). Re-run if
the rules below change.

This script does NOT require Ghidra to be open; it only writes the
.git/decomp-goal/steering/ directory under this repo.
"""
from __future__ import annotations

import os
import shutil
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(os.path.realpath(__file__)).resolve().parents[1]

# Each tuple is (source-tag, lead-text). The source-tag must be a
# short identifier (`human`, `ghidra`, `tooling`, ...). The text is
# what gets quoted verbatim into the goal prompt.
LEADS: list[tuple[str, str]] = [
    (
        "tooling",
        "Compiler is MSVC 2005 RTM (cl 14.00.50727, link 8.00.50727). "
        "Stubs are built with tools/msvc8/Bin/cl.exe at /Zi /O2 /GR /EHsc. "
        "Do not assume modern C++; std::nullptr_t, range-for, etc. are unavailable. "
        "Header dirs: include/, include/bulanci, tools/msvc8/Include, "
        "tools/msvc8/PlatformSDK/Include, tools/dxsdk_feb2007/Include.",
    ),
    (
        "tooling",
        "Each translation unit lives at src/bulanci/<unit>.cpp with a header at "
        "include/bulanci/<unit>.h. Shared typedefs (uchar, uint, BOOL, NULL, ...) "
        "are in include/globals.h. Do not redefine those.",
    ),
    (
        "tooling",
        "Build any source change with `tools\\ninja\\ninja.exe` from the repo root; "
        "compare via `decomp-goal run --repo . --unit <U>` (it shells out to "
        "scripts/oracle.py which regenerates report.json via objdiff-cli).",
    ),
    (
        "ghidra",
        "Ground-truth comes from Ghidra. The MCP plugin exposes HTTP endpoints on "
        "http://localhost:8089 (decompile, list_symbols, get_function_callees, etc.). "
        "Read the target function's signature, callers, and callees there BEFORE "
        "guessing types from the asm.",
    ),
    (
        "human",
        "bulanci.exe is a Czech indie 2D shooter on DirectX 7 wrappers (DDraw, "
        "DInput, DSound, DirectPlay). UI strings are Czech (Pauza, Konec hry, "
        "Typ hry). Class prefix `CDS` marks the engine library (CDSFileStream, "
        "CDSAudioBank, ...); plain `C` is game logic (CBulanci, CGame, ...).",
    ),
    (
        "human",
        "Avoid touching _Globals when a function clearly belongs to a class. "
        "Prefer moving the function in Ghidra first (via promote_namespaces.py "
        "after manual rename), then matching its source in the proper class unit.",
    ),
    (
        "objdiff",
        "objdiff reports per-function `matching_prefix_bytes` and "
        "`first_mismatch_offset`. Treat those as primary last-mile signals once "
        "fuzzy score is above ~90%; raw matched_code can be misleading.",
    ),
    (
        "tooling",
        "Function bodies in stubs are placeholders. Implementing a function is "
        "fine; renaming the function symbol is NOT - the address suffix "
        "(`FUN_004032c0`, `_004032c0`) must survive until the match is achieved, "
        "otherwise objdiff loses pairing. Rename via Ghidra MCP instead so the "
        "whole pipeline regenerates consistent names.",
    ),
]

GLOBAL_UNIT = "_Globals"  # leads filed against this unit are read by every prompt
DECOMP_GOAL_BIN = (
    Path(r"C:\Users\mstagl-dev\Documents\GitHub\decomp-goal-harness\.venv\Scripts\decomp-goal.exe")
    if os.name == "nt"
    else Path("decomp-goal")
)


def _resolve_binary() -> Path | str:
    if DECOMP_GOAL_BIN.exists():
        return DECOMP_GOAL_BIN
    found = shutil.which("decomp-goal")
    if found:
        return Path(found)
    raise SystemExit(
        "could not find `decomp-goal` on PATH or at the default venv path. "
        "Run `uv sync` in ../decomp-goal-harness, or pass --bin."
    )


def main() -> int:
    bin_path = _resolve_binary()
    print(f"using {bin_path}")
    for source, text in LEADS:
        cmd = [
            str(bin_path),
            "steer",
            "--repo", str(REPO_ROOT),
            "--source", source,
            "--text", text,
        ]
        print(f"+ steer source={source}")
        subprocess.run(cmd, check=True, cwd=str(REPO_ROOT))
    print(
        "\nSeeded {n} lead(s). Every subsequent `decomp-goal goal` / "
        "`decomp-goal codex` prompt will include the most recent ones.".format(
            n=len(LEADS)
        )
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
