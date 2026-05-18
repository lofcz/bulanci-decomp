# `tools/dxsdk_feb2007/` — DirectX SDK (February 2007) sliver

`bulanci.exe` is a 2006 DirectDraw + DirectInput game. Its DirectX usage,
as recovered from the PE import table, is:

```
DDRAW.dll!DirectDrawCreate      <- DirectDraw 1 entrypoint, returns IDirectDraw
DINPUT.dll!DirectInputCreateW   <- DirectInput 7 entrypoint, returns IDirectInputW
```

No DirectSound, no Direct3D 8/9, no DirectInput 8. Audio is via `WINMM.dll`
(`waveOut*` family).

## Why Feb 2007

The legacy DDraw 1 and DInput 7 API surfaces (function signatures, vtable
layouts, IID GUIDs) have been **frozen since DirectX 8 (2000)** and were
removed from the SDK after **August 2007**. The Feb 2007 SDK sits inside
that range, exactly 7 months after the binary's build date
(`TimeDateStamp = 0x44A4D775` = 2006-06-30) and still ships the legacy
headers intact. Any SDK in the range DX 8.0 - Aug 2007 produces byte-
identical call sites for the two entry points we care about; the SDK
version is therefore not a load-bearing matching constraint, only the
header presence is.

## What is vendored

```
tools/dxsdk_feb2007/
  Include/
    ddraw.h         ; DirectDraw 1-7 declarations + IIDs
    dinput.h        ; DirectInput 1-7 declarations + IIDs
    dinputd.h       ; DirectInput device driver IDs (pulled in by dinput.h)
    dsound.h        ; kept for completeness; not currently referenced
  Lib/x86/
    ddraw.lib       ; import stub for DirectDrawCreate*
    dinput.lib      ; import stub for DirectInputCreate*W
    dinput8.lib     ; not used by bulanci, kept for symmetry
    dsound.lib      ; kept for completeness
    dxguid.lib      ; static GUIDs (IID_IDirectDraw, IID_IDirectInputW, ...)
```

## How to obtain

The DirectX SDK Feb 2007 redistributable installer
(`dxsdk_feb2007.exe`, ~453 MB) was archived by Microsoft and is still
available from MS legacy archive and assorted mirrors. After installing
to its default location
(`C:\Program Files (x86)\Microsoft DirectX SDK (February 2007)\`), copy
the files above into this directory.

PowerShell one-shot:

```powershell
$src = 'C:\Program Files (x86)\Microsoft DirectX SDK (February 2007)'
$dst = 'tools\dxsdk_feb2007'
New-Item -ItemType Directory -Path $dst\Include, $dst\Lib\x86 -Force | Out-Null
'ddraw.h','dinput.h','dinputd.h','dsound.h' | % {
    Copy-Item "$src\Include\$_" "$dst\Include\" -Force
}
'ddraw.lib','dinput.lib','dinput8.lib','dsound.lib','dxguid.lib' | % {
    Copy-Item "$src\Lib\x86\$_" "$dst\Lib\x86\" -Force
}
```

## Wiring it into the build

The DX headers transitively `#include <windows.h>` / `<objbase.h>`, which
ship in the Windows Platform SDK (not yet vendored). As long as no
`.cpp` in `src/bulanci/` reaches for `<ddraw.h>` or `<dinput.h>` we do not
need the DX headers on the compiler include path - the current empty
stubs do not.

Once we begin restoring DirectDraw / DirectInput call sites for real,
add to `scripts/generate_ninja.py`:

```python
DX_INCLUDE = Path("tools/dxsdk_feb2007/Include")
DX_LIBS    = Path("tools/dxsdk_feb2007/Lib/x86")
```

and append `/I {DX_INCLUDE}` to `cl_flags`, plus a future link rule that
references `ddraw.lib dinput.lib dxguid.lib` from `DX_LIBS`. Also vendor
a Platform SDK at that point so `<windows.h>` resolves.
