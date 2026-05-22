/* eslint-disable */
"use strict";

console.log("[+] Bulanci windowed-mode Frida patch loaded");

// ─────────────────────────────────────────── locate game module
function findGameModule() {
    const names = [
        "bulanci_insturmented.exe",
        "Bulanci_insturmented.exe",
        "bulanci.exe",
        "Bulanci.exe",
        "BULANCI.EXE",
    ];
    for (const n of names) {
        try {
            const m = Process.getModuleByName(n);
            if (m) return m;
        } catch (e) {}
    }
    for (const m of Process.enumerateModules()) {
        if (m.name.toLowerCase().indexOf("bulanci") !== -1) return m;
    }
    return null;
}

const MOD = findGameModule();
if (!MOD) {
    console.log("[-] Could not locate Bulanci module — aborting patch");
    throw new Error("Bulanci module not found");
}
console.log(`[+] Found game module: ${MOD.name} @ ${MOD.base}`);

// Bulanci.exe is a 32-bit MSVC8 PE with no /DYNAMICBASE, so the preferred
// base 0x00400000 is always honoured. Compute slide defensively anyway.
const IMAGE_BASE_PE = 0x00400000;
const SLIDE = MOD.base.sub(IMAGE_BASE_PE);
function va(addr) { return ptr(addr).add(SLIDE); }

// ─────────────────────────────────────────── helpers
function readUtf16StringSafe(p) {
    try {
        if (p.isNull()) return null;
        let s = "";
        for (let i = 0; i < 500; i++) {
            const c = p.add(i * 2).readU16();
            if (c === 0) break;
            s += String.fromCharCode(c);
        }
        return s;
    } catch (e) { return null; }
}

// ════════════════════════════════════════════════════════════════════════
// Patch 1: Force CBulanci::m_bWindowed (this+0xe4) = 1 on every entry to
// CBulanci::CDSApp_InitDirectDraw (0x00429990).
//
// This is THE single source of truth that the engine uses to switch
// between exclusive fullscreen and windowed rendering. From the
// decompilation at 0x00429990:
//
//   if (this[0xe4] != 0) {            // ← windowed branch
//       SetWindowPos(...);            //   centers the window
//   }
//   DirectDrawCreate(...);
//   DVar4 = (this[0xe4] == 0) ? 0x11  //   DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN
//                             : 0x08; //   DDSCL_NORMAL
//   SetCooperativeLevel(piVar1, hwnd, DVar4);
//
// And the same flag drives downstream surface creation, blit/flip
// selection, AdaptDisplaySize, Alt+Tab handling, etc. The original game
// reads it from HKLM\Software\SleepTeam\Bulanci\Windowed in CDSApp_ctor;
// when that key is absent (the common case on modern Windows) the flag
// stays 0 and the engine drives DDSCL_EXCLUSIVE|FULLSCREEN. Forcing it
// to 1 the instant control enters InitDirectDraw makes the windowed
// branch active for both the cooperative-level call AND every
// subsequent surface/blit decision, in one coherent flip.
//
// __thiscall: `this` is delivered in ECX.
const InitDirectDrawAddr = va(0x00429990);
console.log(`[+] CBulanci::CDSApp_InitDirectDraw @ ${InitDirectDrawAddr}`);
Interceptor.attach(InitDirectDrawAddr, {
    onEnter: function () {
        try {
            const thisPtr = this.context.ecx;
            const before = thisPtr.add(0xe4).readU8();
            if (before === 0) {
                thisPtr.add(0xe4).writeU8(1);
                console.log(`[+] InitDirectDraw: m_bWindowed (this+0xe4) 0 -> 1 (entire engine now in windowed mode)`);
            } else {
                console.log(`[+] InitDirectDraw: m_bWindowed already ${before} (no change)`);
            }
        } catch (e) {
            console.log(`[-] InitDirectDraw onEnter error: ${e.message}`);
        }
    }
});

// ════════════════════════════════════════════════════════════════════════
// Patch 2: SetWindowPos in the centering branch of InitDirectDraw uses
// the engine-stored client rect (this+0x20..0x2c → 800x600). Because
// our CreateWindowExW hook below grows the OUTER window to host a real
// title bar, we must keep that adjusted outer size when the engine
// centers the window. Simply OR SWP_NOSIZE into the flags so cx/cy
// from the engine are ignored.
//
// The centering call lives at exactly one site inside InitDirectDraw
// (size 0xad bytes); we identify it by checking the return address.
const SetWindowPos = Module.findGlobalExportByName('SetWindowPos');
if (SetWindowPos) {
    const InitDDStart = InitDirectDrawAddr;
    const InitDDEnd   = InitDirectDrawAddr.add(0xad);
    Interceptor.attach(SetWindowPos, {
        onEnter: function (args) {
            try {
                const ret = this.returnAddress;
                if (ret.compare(InitDDStart) >= 0 && ret.compare(InitDDEnd) < 0) {
                    const flags = args[6].toInt32();
                    args[6] = ptr(flags | 0x0001); // SWP_NOSIZE
                    console.log(`[+] SetWindowPos@InitDirectDraw: flags 0x${flags.toString(16)} | SWP_NOSIZE`);
                }
            } catch (e) {}
        }
    });
}

// ════════════════════════════════════════════════════════════════════════
// Patch 3: CreateWindowExW in CDSApp_OnCreate (0x0042a210) is hard-coded
// to WS_POPUP (0x80000000) — a borderless full-window. Convert it to a
// standard overlapped window with caption, system menu, and minimize
// box, then grow the outer w/h via AdjustWindowRect so the engine's
// internal 800x600 client area stays pixel-exact.
const AdjustWindowRectAddr = Module.findGlobalExportByName('AdjustWindowRect');
const AdjustWindowRect = AdjustWindowRectAddr
    ? new NativeFunction(AdjustWindowRectAddr, 'int', ['pointer', 'uint32', 'int'])
    : null;

function patchCreateWindowStyle(args) {
    try {
        const className  = readUtf16StringSafe(args[1]);
        const windowName = readUtf16StringSafe(args[2]);
        const hit = ((className  || "").toLowerCase().indexOf("bulanci") !== -1)
                 || ((windowName || "").toLowerCase().indexOf("bulanci") !== -1);
        if (!hit) return;

        const oldStyle = args[3].toInt32() >>> 0;
        if ((oldStyle & 0x80000000) === 0) return;

        const newStyle = 0x00CA0000; // WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX
        args[3] = ptr(newStyle);

        if (AdjustWindowRect) {
            const r = Memory.alloc(16);
            r.writeS32(0);
            r.add(4).writeS32(0);
            r.add(8).writeS32(args[6].toInt32());
            r.add(12).writeS32(args[7].toInt32());
            if (AdjustWindowRect(r, newStyle, 0)) {
                const w = r.add(8).readS32() - r.readS32();
                const h = r.add(12).readS32() - r.add(4).readS32();
                args[6] = ptr(w);
                args[7] = ptr(h);
                console.log(`[+] CreateWindowEx '${windowName || className}': WS_POPUP -> 0x${newStyle.toString(16)}, outer ${w}x${h}`);
                return;
            }
        }
        console.log(`[+] CreateWindowEx '${windowName || className}': WS_POPUP -> 0x${newStyle.toString(16)} (no AdjustWindowRect)`);
    } catch (e) {
        console.log(`[-] CreateWindowEx hook error: ${e.message}`);
    }
}

const CreateWindowExW = Module.findGlobalExportByName('CreateWindowExW');
if (CreateWindowExW) Interceptor.attach(CreateWindowExW, { onEnter: patchCreateWindowStyle });
const CreateWindowExA = Module.findGlobalExportByName('CreateWindowExA');
if (CreateWindowExA) Interceptor.attach(CreateWindowExA, { onEnter: patchCreateWindowStyle });

// ════════════════════════════════════════════════════════════════════════
// Background: stale screen-space blit destination rect.
//
// The engine caches the blit-destination rect at `this+0xcc..+0xdc` as
// GetClientRect → ClientToScreen×2, and only recomputes it through
// FUN_00429a80 @ 0x00429a80 (the windowed branch of that helper).
// FUN_00429a80 has exactly one caller in the original binary —
// FUN_0042a330, the size adapter — which itself only runs on init and
// Alt+Enter. The original game uses WS_POPUP (no caption, immovable)
// so the engine never had to refresh the rect mid-session.
//
// Patch 3 swaps the style to WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU,
// making the window draggable. Now any user drag leaves the engine
// blitting to the init-time screen position regardless of where the
// window actually is — content stays anchored at the screen-center
// (the launch position), the window paints black, and the visible
// symptom is the duplicated / shifted rendering the user reported.
//
// We need to call FUN_00429a80(g_pApp) any time the window moves.
//
// Calling convention: FUN_00429a80 is MSVC __fastcall with a single
// `int` param, which the MSVC ABI lowers to "arg in ECX, no stack
// args, callee `ret`-without-operand". That register layout is
// identical to MSVC __thiscall single-arg, so we wrap it with
// Frida's 'thiscall' ABI — works for both __fastcall(1 arg) and
// __thiscall on x86. ('mscdecl' would put the arg on the stack
// instead, which would corrupt the call.)
const FUN_00429A80_ADDR = va(0x00429A80);
const G_PAPP_ADDR       = va(0x004B3B88);
const G_PHWND_ADDR      = va(0x004B3B1C);

let refreshScreenSpaceDestRect = null;
try {
    refreshScreenSpaceDestRect = new NativeFunction(
        FUN_00429A80_ADDR,
        'void',
        ['pointer'],
        'thiscall'
    );
    console.log(`[+] FUN_00429a80 (refresh screen-space dest rect) @ ${FUN_00429A80_ADDR}`);
} catch (e) {
    console.log(`[-] Cannot wrap FUN_00429a80 as NativeFunction: ${e.message}`);
}

// Shared helper used by both patches below. The two guards are cheap
// and necessary: skipping when m_bWindowed is still 0 prevents
// FUN_00429a80's fullscreen branch from overwriting the screen-space
// rect with stale `this+0x20..0x2c` values during early init, before
// Patch 1 has flipped the flag.
function refreshEngineRectSafely() {
    if (!refreshScreenSpaceDestRect) return;
    try {
        const gpApp = G_PAPP_ADDR.readPointer();
        if (gpApp.isNull()) return;
        if (gpApp.add(0xe4).readU8() === 0) return;
        refreshScreenSpaceDestRect(gpApp);
    } catch (e) {
        // Silent — failing here would spam during every refresh attempt.
    }
}

// ════════════════════════════════════════════════════════════════════════
// Patch 4: refresh after every successful user32::SetWindowPos for our
// HWND.
//
// This catches programmatic moves (e.g. our own SetWindowPos centering
// call in InitDirectDraw, the engine's own SC_RESTORE handling, third-
// party window-management tools, etc.) cheaply at the source of the
// move. It does NOT catch Windows 10/11 title-bar drags reliably —
// DefWindowProc's SC_MOVE modal loop ends up in win32u syscalls that
// don't always re-enter user32::SetWindowPos — which is exactly the
// case Patch 5 below covers.
if (SetWindowPos && refreshScreenSpaceDestRect) {
    Interceptor.attach(SetWindowPos, {
        onEnter: function (args) {
            try { this.targetHwnd = args[0]; } catch (e) {}
        },
        onLeave: function (retval) {
            if (retval.toInt32() === 0) return;
            if (!this.targetHwnd) return;
            try {
                const gpHwnd = G_PHWND_ADDR.readPointer();
                if (gpHwnd.isNull()) return;
                if (!this.targetHwnd.equals(gpHwnd)) return;
                refreshEngineRectSafely();
            } catch (e) {}
        }
    });
    console.log(`[+] SetWindowPos onLeave -> refresh engine screen-space dest rect`);
}

// ════════════════════════════════════════════════════════════════════════
// Patch 5: refresh the screen-space dest rect at the top of every
// frame (CDSApp_RenderFrame @ 0x0042bc00, the engine's per-frame draw
// entry point — see _Globals::CDSApp_FrameBody @ 0x0042bda0 which
// calls into it after the message-pump section).
//
// This is the bulletproof safety net for every move source we can't
// intercept upstream (Windows 10/11 DWM-driven drag, Win+arrow snap,
// taskbar-driven minimize/restore, multi-monitor relocation, …). The
// cost of one frame's refresh is exactly one `GetClientRect` + two
// `ClientToScreen` calls — three Win32 stubs, ~1 µs in total. At the
// engine's ~58-120 fps render rate that's well under 0.01% CPU.
//
// Important ordering: we refresh in `onEnter` so the rect is correct
// BEFORE the engine reads it during the frame's Blt/BltFast sequence.
// Patch 1's m_bWindowed=1 guarantee has already run by the first
// frame (InitDirectDraw is upstream of any render), so the helper's
// guard short-circuits to a no-op on the genuinely-fullscreen branch
// instead of writing stale values.
const RenderFrameAddr = va(0x0042BC00);
if (refreshScreenSpaceDestRect) {
    try {
        Interceptor.attach(RenderFrameAddr, {
            onEnter: function () {
                refreshEngineRectSafely();
            }
        });
        console.log(`[+] CDSApp_RenderFrame @ ${RenderFrameAddr} -> per-frame rect refresh`);
    } catch (e) {
        console.log(`[-] Cannot attach to CDSApp_RenderFrame: ${e.message}`);
    }
}

console.log("[+] All hooks installed - launching game in coherent windowed mode");
