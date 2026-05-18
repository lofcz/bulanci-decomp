# `tools/msvc8/` — Visual Studio 2005 toolchain (VC8)

`bulanci.exe` was linked with `link.exe 8.00.50727` (Visual Studio 2005 RTM,
internal name VC8). This was determined by:

* PE OptionalHeader `MajorLinkerVersion = 8`, `MinorLinkerVersion = 0`
* Rich Header linker build `50727` (`0xC627`) — see
  [`scripts/internal/dump_rich_header.py`](../../scripts/internal/dump_rich_header.py)
* Compiler tags `Utc1400_C` / `Utc1400_CPP` (VC8 `cl 14.00.50727`)
* TimeDateStamp `0x44A4D775` = 2006-06-02, i.e. between VS2005 RTM (Nov 2005)
  and VS2005 SP1 (Dec 2006), so **no service pack**

For matching decompilation we need the exact RTM toolchain. SP1 changes
codegen subtly and will not match.

## Expected layout

`scripts/generate_ninja.py` defaults to `tools/msvc8/Bin/cl.exe`. The full
expected layout is:

```
tools/msvc8/
  Bin/
    cl.exe          ; 14.00.50727.42 (RTM)
    link.exe        ; 8.00.50727.42  (RTM)
    lib.exe
    c1.dll, c1xx.dll, c2.dll, mspdb80.dll, msvcr80.dll, ...
  Include/          ; CRT + C++ headers
  Lib/              ; libcmt.lib, libcpmt.lib, oldnames.lib, ...
  PlatformSDK/
    Include/        ; Windows headers (windows.h, ddraw.h, dinput.h, ...)
    Lib/            ; user32.lib, gdi32.lib, kernel32.lib, ...
```

Override the path with `BULANCI_CL=path\to\cl.exe` if you put the toolchain
elsewhere; the rest of the install must still be reachable from `cl.exe`'s
sibling directories (`Bin\..\Include`, `Bin\..\Lib`, `Bin\..\PlatformSDK`).

## How to obtain VS2005 RTM

VS2005 has been out of mainstream support since 2011. Microsoft no longer
distributes the installer, but the redistributable build tools shipped in
the Windows Server 2003 SP1 Platform SDK and several free SDKs of the era
contain a usable `cl 14.00.50727` and `link 8.00.50727`:

1. **Windows Server 2003 R2 Platform SDK** (Feb 2003 release).
   Ships VC++ 7.1 headers/libs but also contains a `Bin\Win64` cl/link.
   Not RTM 8.00 — skip.

2. **Visual C++ 2005 Express Edition (recommended).**
   Free, downloadable from Microsoft's legacy archive. Ships
   `Microsoft Visual Studio 8/VC/bin/cl.exe` at exactly `14.00.50727.42`.
   Combine with the Platform SDK below for `windows.h`.

3. **Microsoft Platform SDK for Windows Server 2003 R2** (PSDK-x86.exe)
   for `windows.h`, `ddraw.h`, `dinput.h`, and the `.lib`s that VC8
   Express does not ship.

After installing Express + PSDK, copy the relevant files into the layout
above. A minimal install is ~80 MB.

## Verifying the toolchain

```cmd
tools\msvc8\Bin\cl.exe /Bv
```

should print:

```
Microsoft (R) C/C++ Optimizing Compiler Version 14.00.50727.42 for 80x86
```

If the version is **not** `14.00.50727.42` you have the wrong build.
50727**.x** for any other `.x` (e.g. `.762` from SP1) will produce
non-matching code.

## Once vendored

After the toolchain is in place, regenerate the build configuration and
ninja will start emitting build edges for the stub C++ in `src/bulanci/`.
`scripts/generate_objdiff.py` then auto-flips `build_base` to `true`:

```cmd
python scripts\configure.py --skip-ghidra
ninja
progress_update.bat
```

`build/Src/bulanci/_Globals.obj` will appear and `objdiff` will start
reporting a non-zero `matched_code` once any stub function happens to
match — the very first matches will be tiny CRT thunks / `__chkstk`-style
helpers that the linker pulled in from `libcmt.lib`.
