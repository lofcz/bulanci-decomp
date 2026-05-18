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
    c1.dll, c1xx.dll, c2.dll, mspdb80.dll, msobj80.dll, ...
  Include/          ; CRT + C++ headers (~145 files)
  Lib/              ; libcmt.lib, libcpmt.lib, oldnames.lib, ...
  PlatformSDK/      ; Windows Server 2003 R2 Platform SDK
    Include/        ; windows.h, winuser.h, wingdi.h, ole2.h, ... (~1276 files)
    Lib/            ; kernel32.lib, user32.lib, gdi32.lib, ole32.lib,
                    ;   advapi32.lib, winmm.lib, ... (x86 only - AMD64
                    ;   and IA64 sub-trees are deliberately omitted)
```

`scripts/generate_ninja.py` auto-detects `PlatformSDK/Include/` and adds it
to the compiler include path when present, so once vendored, `#include
<windows.h>` resolves transparently.

Override the path with `BULANCI_CL=path\to\cl.exe` if you put the toolchain
elsewhere; the rest of the install must still be reachable from `cl.exe`'s
sibling directories (`Bin\..\Include`, `Bin\..\Lib`, `Bin\..\PlatformSDK`).

## How to obtain VS2005 RTM

VS2005 has been out of mainstream support since 2011. Microsoft no longer
distributes the installer, but the redistributable build tools shipped in
the Windows Server 2003 SP1 Platform SDK and several free SDKs of the era
contain a usable `cl 14.00.50727` and `link 8.00.50727`:

1. **Visual C++ 2005 Express Edition (required).**
   Free, downloadable from Microsoft's legacy archive. Ships
   `Microsoft Visual Studio 8/VC/bin/cl.exe` at exactly `14.00.50727.42`.

2. **Microsoft Platform SDK for Windows Server 2003 R2 (PSDK-x86.exe).**
   Ships `windows.h` and friends, plus all the user-mode import libs
   (kernel32, user32, gdi32, ole32, advapi32, winmm, ...) that VC8
   Express does not include. Default install path is
   `C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2`.

3. (DirectX headers live in a separate SDK; see
   [`tools/dxsdk_feb2007/`](../dxsdk_feb2007/README.md) for that.)

Total vendored footprint: ~62 MB (MSVC) + ~79 MB (PSDK x86 subset).

### Quick vendoring recipe

```powershell
$vs   = 'C:\Program Files (x86)\Microsoft Visual Studio 8'
$psdk = 'C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2'
$dst  = 'tools\msvc8'

# (1) MSVC compiler + CRT
robocopy "$vs\VC\bin"     "$dst\Bin"     /E
robocopy "$vs\VC\include" "$dst\Include" /E
robocopy "$vs\VC\lib"     "$dst\Lib"     /E
# cl.exe expects mspdb80.dll / mspdbcore.dll / mspdbsrv.exe / msobj80.dll
# alongside it. Express puts them under Common7\IDE; copy them into Bin
# so the vendored toolchain is self-contained.
@('mspdb80.dll', 'mspdbcore.dll', 'mspdbsrv.exe', 'msobj80.dll') |
    ForEach-Object { Copy-Item "$vs\Common7\IDE\$_" "$dst\Bin\" -Force }

# (2) Windows Platform SDK (x86 only - skip AMD64 and IA64 subtrees)
robocopy "$psdk\Include" "$dst\PlatformSDK\Include" /E
robocopy "$psdk\Lib"     "$dst\PlatformSDK\Lib"     /E /XD AMD64 IA64
```

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
compile the stubs. `scripts/generate_objdiff.py` auto-detects
`Bin\cl.exe` and flips `build_base` to `true` in `objdiff.json`:

```cmd
python scripts\configure.py --skip-ghidra
for %u in (ATL _LocaleUpdate exception std type_info _Globals) do ^
  tools\msvc8\Bin\cl.exe /nologo /Zi /O2 /GR /EHsc ^
    /I include\ /I include\bulanci\ /I tools\msvc8\Include /I tools\ ^
    /c src\bulanci\%u.cpp /Fobuild\Src\bulanci\%u.obj
progress_update.bat
```

After the first run you should see every entry in `objdiff.json` carry a
`base_path` and `build/Src/bulanci/_Globals.obj` weigh in around 1.5 MB.
`matched_code` will still be 0% — the stubs are empty placeholders — but
the pipeline is wired end-to-end. Real matches arrive as you rename
functions in Ghidra and reduce stub bodies to the actual disassembly.

The first matches will likely be tiny CRT thunks / `__chkstk`-style
helpers that the linker pulled in from `libcmt.lib`.
