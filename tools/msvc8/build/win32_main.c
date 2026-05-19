/*
 * win32_main.c - single translation unit that drags in the Windows
 * Platform SDK headers we want types from.  After running through
 * `cl /E /FI dx_prelude.h` this expands into a flattened .i file that
 * scripts/build_gdt.py post-processes and feeds to Ghidra's CParser.
 *
 * The DX prelude is reused because its keyword-overrides and
 * NONAMELESSUNION knob apply equally to plain Win32; the
 * DirectX-specific CINTERFACE / COBJMACROS toggles are harmless when
 * no COM interface is in scope.
 *
 * Headers in scope:
 *   - windows.h    drags in winnt, winbase, winuser, wingdi, winreg,
 *                  shellapi, commdlg, dde, lzexpand, wincon, ...
 *   - objbase.h    COM types (IUnknown vtable etc.), referenced by
 *                  bulanci through ole32 imports (CoCreateInstance,
 *                  CoInitialize).
 *   - mmsystem.h   waveform / timer types reachable through winmm
 *                  (timeGetTime, etc.).
 *
 * Owners: scripts/build_gdt.py
 */
#include <windows.h>
#include <objbase.h>
#include <mmsystem.h>
