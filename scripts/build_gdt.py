"""Build a Ghidra `.gdt` data-type archive for one of bulanci.exe's
SDKs.  Generic driver that supports multiple per-toolchain targets.

Currently supported `--target` values:

    dx       DirectX SDK (February 2007) - DirectDraw, DirectSound,
             DirectInput.  Output: tools/dxsdk_feb2007/gdt/directx_feb2007.gdt
    win32    Microsoft Windows SDK shipped with VS 2005 (the same
             one vendored under tools/msvc8/PlatformSDK).  Covers
             windows.h + objbase.h transitive closure.
             Output: tools/msvc8/gdt/win32_msvc8.gdt
    all      Builds every target.

Pipeline per target:

    <wrapper>.c  --cl /E /FI <prelude>-->  <stage>.i  --regex clean-->
    <category-friendly>.i  --Ghidra CParser-->  <out>.gdt

The Ghidra step runs through a one-shot temp project so it doesn't
race against the live bulanci.gpr.

Phases are independent and re-runnable; everything is staged in
`build/gdt/<target>/` so failed parses can be inspected.
"""
from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
import tempfile
from dataclasses import dataclass, field
from pathlib import Path
from typing import List

REPO_ROOT = Path(__file__).resolve().parents[1]
BUILD_ROOT = REPO_ROOT / "build" / "gdt"

MSVC_ROOT = REPO_ROOT / "tools" / "msvc8"
DX_SDK_ROOT = REPO_ROOT / "tools" / "dxsdk_feb2007"
MSVC_INC = MSVC_ROOT / "Include"
PSDK_INC = MSVC_ROOT / "PlatformSDK" / "Include"
DX_INC = DX_SDK_ROOT / "Include"
CL_EXE = MSVC_ROOT / "Bin" / "cl.exe"


@dataclass
class Target:
    name: str
    main_c: Path            # the wrapper .c file (single TU)
    prelude_h: Path         # /FI-included prelude with macro overrides
    out_gdt: Path           # final .gdt destination
    friendly_basename: str  # used as the Ghidra category-path root
    defines: List[str] = field(default_factory=list)
    includes: List[Path] = field(default_factory=list)
    canaries: List[str] = field(default_factory=list)


SHARED_PRELUDE = DX_SDK_ROOT / "build" / "dx_prelude.h"

TARGETS = {
    "dx": Target(
        name="dx",
        main_c=DX_SDK_ROOT / "build" / "dx_main.c",
        prelude_h=SHARED_PRELUDE,
        out_gdt=DX_SDK_ROOT / "gdt" / "directx_feb2007.gdt",
        friendly_basename="directx_feb2007",
        defines=[
            "_X86_=1",
            "_WIN32_WINNT=0x0501",
            "WINVER=0x0501",
            "CINTERFACE",
            "COBJMACROS",
            "NONAMELESSUNION",
            "DIRECTINPUT_VERSION=0x0800",
            "DIRECT3D_VERSION=0x0900",
        ],
        includes=[DX_INC, MSVC_INC, PSDK_INC],
        canaries=[
            "IDirectDraw7Vtbl",
            "IDirectDrawSurface7Vtbl",
            "IDirectSoundVtbl",
            "IDirectSoundBuffer8Vtbl",
            "IDirectInput8AVtbl",
            "IDirectInputDevice8AVtbl",
            "WAVEFORMATEX",
            "DDSURFACEDESC2",
            "DSBUFFERDESC",
        ],
    ),
    "win32": Target(
        name="win32",
        main_c=MSVC_ROOT / "build" / "win32_main.c",
        prelude_h=SHARED_PRELUDE,
        out_gdt=MSVC_ROOT / "gdt" / "win32_msvc8.gdt",
        friendly_basename="win32_msvc8",
        defines=[
            "_X86_=1",
            "_WIN32_WINNT=0x0501",
            "WINVER=0x0501",
            "NONAMELESSUNION",
        ],
        includes=[MSVC_INC, PSDK_INC],
        canaries=[
            "STARTUPINFOA",
            "PROCESS_INFORMATION",
            "WIN32_FIND_DATAA",
            "_OVERLAPPED",
            "_SECURITY_ATTRIBUTES",
            "tagWNDCLASSEXA",
            "tagMSG",
            "tagBITMAPINFOHEADER",
            "tagLOGFONTA",
            "_FILETIME",
        ],
    ),
}


# Phase 1: preprocess via cl.exe.
def run_preprocess(t: Target, stage_dir: Path) -> Path:
    stage_dir.mkdir(parents=True, exist_ok=True)
    out_i = stage_dir / f"{t.name}_main.i"

    if not CL_EXE.exists():
        sys.exit(f"cl.exe missing: {CL_EXE}")
    for p in (*t.includes, t.prelude_h, t.main_c):
        if not p.exists():
            sys.exit(f"missing input for target '{t.name}': {p}")

    env = os.environ.copy()
    env["INCLUDE"] = ";".join(str(p) for p in t.includes)
    env["PATH"] = str(MSVC_ROOT / "Bin") + ";" + env.get("PATH", "")
    env["LIB"] = ";".join(str(p) for p in (MSVC_ROOT / "Lib",
                                            PSDK_INC.parent / "Lib"))

    cmd = [str(CL_EXE), "/nologo", "/E", "/TC"]
    cmd.extend(f"/D{d}" for d in t.defines)
    cmd.append(f"/FI{t.prelude_h}")
    cmd.append(str(t.main_c))

    print(f"[1/3] preprocess via cl.exe (target={t.name}) ...")
    print("    " + " ".join(cmd))
    with out_i.open("w", encoding="utf-8", newline="\n") as fp:
        proc = subprocess.run(cmd, env=env, stdout=fp,
                              stderr=subprocess.PIPE, text=True, check=False)
    if proc.returncode != 0:
        head = "\n".join(proc.stderr.splitlines()[:50])
        sys.exit(f"cl.exe /E failed (exit {proc.returncode}):\n{head}")
    if proc.stderr.strip():
        print(proc.stderr.strip().splitlines()[0])
    size_kb = out_i.stat().st_size // 1024
    print(f"    -> {out_i.relative_to(REPO_ROOT)} ({size_kb} KB)")
    return out_i


# Phase 2: regex cleanup.

_KEYWORD_STRIPPERS = [
    (re.compile(r"__declspec\s*\((?:[^()]+|\([^()]*\))*\)"), ""),
    (re.compile(r"__pragma\s*\((?:[^()]+|\([^()]*\))*\)"), ""),
    (re.compile(r"__asm\s*\{[^{}]*\}"), ";"),
    (re.compile(r"\b__asm\b[^;{}\n]*"), ""),
    (re.compile(r"\b__forceinline\b"), "inline"),
    (re.compile(r"\b__inline\b"), "inline"),
    (re.compile(r"\b_inline\b"), "inline"),
    (re.compile(r"\b__cdecl\b"), ""),
    (re.compile(r"\b__stdcall\b"), ""),
    (re.compile(r"\b__fastcall\b"), ""),
    (re.compile(r"\b__thiscall\b"), ""),
    (re.compile(r"\b__clrcall\b"), ""),
    (re.compile(r"\b__vectorcall\b"), ""),
    (re.compile(r"\b__unaligned\b"), ""),
    (re.compile(r"\b__w64\b"), ""),
    (re.compile(r"\b__ptr32\b"), ""),
    (re.compile(r"\b__ptr64\b"), ""),
    (re.compile(r"\b__sptr\b"), ""),
    (re.compile(r"\b__uptr\b"), ""),
    (re.compile(r"\b__restrict\b"), ""),
    (re.compile(r"\b__interface\b"), "struct"),
    (re.compile(r"\b__based\s*\((?:[^()]+|\([^()]*\))*\)"), ""),
]

_DROP_LINE_RX = re.compile(r"^\s*#\s*(line|pragma)\b.*$", re.MULTILINE)


def _strip_inline_function_bodies(src: str) -> str:
    """Replace `inline RETTYPE NAME(ARGS) { BODY }` with `... ;`.

    Tracks balanced parens then balanced braces.  Plain composite
    initialisers (no preceding `(...)`) are left untouched, so struct
    / union / enum bodies stay intact.
    """
    out: List[str] = []
    i = 0
    n = len(src)
    while i < n:
        m = re.search(r"\binline\b", src[i:])
        if not m:
            out.append(src[i:])
            break
        start = i + m.start()
        out.append(src[i:start])
        paren_open = src.find("(", start)
        if paren_open < 0:
            out.append(src[start:])
            break
        depth = 1
        j = paren_open + 1
        while j < n and depth > 0:
            c = src[j]
            if c == "(":
                depth += 1
            elif c == ")":
                depth -= 1
            j += 1
        if depth != 0:
            out.append(src[start:])
            break
        k = j
        while k < n and src[k] in " \t\r\n":
            k += 1
        if k >= n or src[k] != "{":
            out.append(src[start:k])
            i = k
            continue
        depth = 1
        body_open = k
        k += 1
        while k < n and depth > 0:
            c = src[k]
            if c == "{":
                depth += 1
            elif c == "}":
                depth -= 1
            k += 1
        if depth != 0:
            out.append(src[start:])
            break
        out.append(src[start:body_open])
        out.append(";")
        i = k
    return "".join(out)


def run_clean(t: Target, raw_i: Path) -> Path:
    print(f"[2/3] clean .i (target={t.name}) ...")
    raw = raw_i.read_text(encoding="utf-8", errors="replace")
    cleaned = _DROP_LINE_RX.sub("", raw)
    for rx, repl in _KEYWORD_STRIPPERS:
        cleaned = rx.sub(repl, cleaned)
    cleaned = _strip_inline_function_bodies(cleaned)
    cleaned = re.sub(r"\n{3,}", "\n\n", cleaned)
    out_path = raw_i.parent / f"{t.friendly_basename}.i"
    out_path.write_text(cleaned, encoding="utf-8")
    size_kb = out_path.stat().st_size // 1024
    print(f"    -> {out_path.relative_to(REPO_ROOT)} ({size_kb} KB)")
    return out_path


# Phase 3: Ghidra headless CParser.
def _ghidra_headless() -> Path:
    ghidra_home = os.environ.get("GHIDRA_HOME") or "C:\\tools\\ghidra"
    analyze = Path(ghidra_home) / "support" / "analyzeHeadless.bat"
    if not analyze.exists():
        sys.exit(f"analyzeHeadless.bat missing: {analyze}")
    return analyze


def run_parse(t: Target, clean_i: Path) -> Path:
    print(f"[3/3] parse with Ghidra's CParser (target={t.name}) ...")
    analyze = _ghidra_headless()
    script_dir = REPO_ROOT / "scripts" / "ghidra"
    t.out_gdt.parent.mkdir(parents=True, exist_ok=True)

    with tempfile.TemporaryDirectory(prefix=f"gdt_{t.name}_") as tmp:
        stub = Path(tmp) / "empty.bin"
        stub.write_bytes(b"\x00" * 16)
        cmd = [
            str(analyze), tmp, f"gdt_{t.name}",
            "-import", str(stub),
            "-loader", "BinaryLoader",
            "-processor", "x86:LE:32:default",
            "-cspec", "windows",
            "-scriptPath", str(script_dir),
            "-preScript", "ParseHeadersToGdt.java",
            str(t.out_gdt), str(clean_i),
            "-noanalysis", "-readOnly", "-deleteProject",
        ]
        print("    " + " ".join(cmd))
        proc = subprocess.run(cmd, text=True, capture_output=True, check=False)
    if proc.stdout:
        sys.stdout.write(proc.stdout)
    if proc.stderr:
        sys.stderr.write(proc.stderr)
    if proc.returncode != 0:
        sys.exit(f"Ghidra headless exited with {proc.returncode}")
    if not t.out_gdt.exists():
        sys.exit(f"expected {t.out_gdt} to exist after parse")
    size_kb = t.out_gdt.stat().st_size // 1024
    print(f"    -> {t.out_gdt.relative_to(REPO_ROOT)} ({size_kb} KB)")
    return t.out_gdt


def run_verify(t: Target) -> None:
    if not t.canaries:
        print(f"[verify] target={t.name} has no canaries, skipping.")
        return
    print(f"[verify] target={t.name} - checking .gdt for canary types ...")
    analyze = _ghidra_headless()
    script_dir = REPO_ROOT / "scripts" / "ghidra"
    with tempfile.TemporaryDirectory(prefix=f"gdt_{t.name}_v_") as tmp:
        stub = Path(tmp) / "empty.bin"
        stub.write_bytes(b"\x00" * 16)
        cmd = [
            str(analyze), tmp, f"gdtv_{t.name}",
            "-import", str(stub),
            "-loader", "BinaryLoader",
            "-processor", "x86:LE:32:default",
            "-cspec", "windows",
            "-scriptPath", str(script_dir),
            "-preScript", "DumpGdtTypes.java", str(t.out_gdt), *t.canaries,
            "-noanalysis", "-readOnly", "-deleteProject",
        ]
        proc = subprocess.run(cmd, text=True, capture_output=True, check=False)

    for line in proc.stdout.splitlines():
        if any(tok in line for tok in ("DumpGdtTypes", "filter[")):
            print("    " + line.strip())

    rx = re.compile(r"filter\[(?P<name>[^\]]+)\] -> (?P<n>\d+) match")
    counts = {m.group("name"): int(m.group("n")) for m in rx.finditer(proc.stdout)}
    missing = [c for c in t.canaries if counts.get(c, 0) == 0]
    if missing:
        print("    WARNING: missing canary types: " + ", ".join(missing))
        sys.exit(2)
    print("    OK: all canary types present.")


def build(t: Target, phases: List[str]) -> None:
    stage_dir = BUILD_ROOT / t.name
    if "preprocess" in phases:
        run_preprocess(t, stage_dir)
    if "clean" in phases:
        run_clean(t, stage_dir / f"{t.name}_main.i")
    if "parse" in phases:
        run_parse(t, stage_dir / f"{t.friendly_basename}.i")
    if "verify" in phases:
        run_verify(t)


def main(argv: List[str]) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument(
        "--target", choices=("dx", "win32", "all"), default="all",
        help="which target archive to build (default: all)",
    )
    parser.add_argument(
        "--phase",
        choices=("all", "preprocess", "clean", "parse", "verify"),
        default="all",
    )
    args = parser.parse_args(argv)

    phase_seq = (["preprocess", "clean", "parse", "verify"]
                 if args.phase == "all" else [args.phase])

    targets = list(TARGETS.values()) if args.target == "all" else [TARGETS[args.target]]
    for t in targets:
        print(f"=== target: {t.name} ===")
        build(t, phase_seq)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
