/*
 * dx_main.c - single translation unit that drags in the DirectX
 * headers we need types from.  After running through `cl /E /FI
 * dx_prelude.h` this expands into one flattened .i file that the
 * Python orchestrator post-processes and feeds to Ghidra's CParser.
 *
 * Headers in scope:
 *   - ddraw.h    DirectDraw (used by Engine.DS.Graphics)
 *   - dsound.h   DirectSound (used by Engine.DS.Audio)
 *   - dinput.h   DirectInput (used by Engine.DS.Input)
 *
 * Owners: scripts/build_dx_gdt.py
 */
#include <windows.h>
#include <objbase.h>

#include <ddraw.h>
#include <dsound.h>
#include <dinput.h>
