# Win32 `.gdt` data-type archive

`win32_msvc8.gdt` contains every public C struct / union / enum /
typedef pulled out of the Microsoft Platform SDK headers shipped with
Visual Studio 2005 (vendored under `tools/msvc8/PlatformSDK/Include/`),
plus `mmsystem.h` and the COM bits from `objbase.h`.

It exists to give Ghidra's decompiler full type fidelity for the Win32
API surface bulanci.exe imports (kernel32 / user32 / advapi32 / ole32 /
gdi32 / winmm) - so things like

```c
STARTUPINFOA si;
GetStartupInfoA(&si);
CreateProcessA(NULL, cmd, NULL, NULL, FALSE, 0,
               NULL, NULL, &si, &pi);
```

decompile recognisably instead of devolving into anonymous struct
field-offset arithmetic.

## Rebuild

```pwsh
python scripts\build_gdt.py --target win32
python scripts\apply_gdt.py                  # applies every committed .gdt
```

`build_gdt.py` is phased - `--phase {preprocess,clean,parse,verify,all}` -
so iterating on cleanup regexes doesn't require re-running `cl /E`.

## What's in here

- ~2000 composites (`tagSTARTUPINFOA`, `_WIN32_FIND_DATAA`, ...)
- ~150 enums
- ~4500 typedefs
- ~2 MB on disk

Canary types covered by the verify pass:
`STARTUPINFOA`, `PROCESS_INFORMATION`, `WIN32_FIND_DATAA`,
`_OVERLAPPED`, `_SECURITY_ATTRIBUTES`, `tagWNDCLASSEXA`, `tagMSG`,
`tagBITMAPINFOHEADER`, `tagLOGFONTA`, `_FILETIME`.

## Coverage overlap with DirectX .gdt

The DirectX `.gdt` under `tools/dxsdk_feb2007/gdt/` already drags in a
subset of these via `windows.h` (~50 winnt.h, ~25 winbase.h types).
`apply_gdt.py` uses `DataTypeConflictHandler.KEEP_HANDLER` so applying
both archives is safe: the first one in wins for any overlapping
type-name, and the second one fills in the gaps.

## Why commit the .gdt?

Same reasoning as the DirectX archive: it's a one-shot derivative of
the vendored SDK that takes ~20s to rebuild and pins exactly to the
toolchain bulanci.exe was originally built with.  Committing saves
every fresh clone that build step.
