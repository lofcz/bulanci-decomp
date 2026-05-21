/* eslint-disable */
//
// Bulanci CGunMouse end-to-end Frida tracer.
//
// What this script captures live, from inside the running Bulanci.exe:
//
//   * Every WM_MOUSEMOVE that the engine ingests
//     (CDSApp::MouseQueue @ 0x0042a5c0). This is the *only* path by
//     which Win32 mouse motion reaches the game — we record the packed
//     LPARAM, both raw signed s16 fields, and the four candidate
//     "current mouse" globals on CBulanci (+0xe0/+0xe4 from the last
//     dispatched input event; +0xf0/+0xf4 = what CGunMouse::Draw
//     actually reads; +0xf8/+0xfc = scaled output of MouseQueue).
//
//   * Every CGunMouse::Draw call (@ 0x00424610). Per the Ghidra trail
//     this is the function that
//       1. snapshots g_pApp[+0xf0]/[+0xf4] into [this+0x190]/[+0x194],
//       2. push-gates by `[+0x1f8] + 10 <= g_dwElapsedMs`
//          (pushes (m_mouseX, m_mouseY) into the ring at [this+0x208]),
//       3. when [this+0x210] (size) == 0x1e, pops the head into
//          [this+0x198]/[+0x19c] and sets [+0x206] = 1,
//       4. blits the dot at (target_X + dot_offset_X - sw/2,
//                            target_Y + dot_offset_Y - sh/2).
//     We capture the full struct slice from +0x190…+0x20c AND the
//     entire raw 30-slot ring buffer both BEFORE the push/pop and
//     AFTER it, so we can correlate exactly which input produced
//     which dot screen position.
//
//   * Every CGunMouse::OnMouseMove call (@ 0x00423900). Observer
//     callback registered against some IDSAnim source. The asm we've
//     pinned down writes to [this+0x19c] and [this+0x1a0] (NOT the
//     adjacent (x, y) pair you'd expect — that's the data point we
//     need to confirm/dispel with live runtime data). We log the
//     deltas it was passed, a short backtrace (so we can tell idle
//     track callbacks apart from any other invoker), and the full
//     before/after CGunMouse state.
//
//   * CGunMouse::Activate (@ 0x00423b50) and CGunMouse::Erase
//     (@ 0x004249b0) — included so we can correlate the
//     dot_base→dot_offset promotion in Erase and the Activate-time
//     seed (2, 3) with the live data stream.
//
// ─────────────────────────────────────────────────────────────────────
// Usage:
//   pip install frida-tools
//   # Either launch the game under Frida:
//   frida -l tools/frida/cursor_trace.js -f path\to\Bulanci.exe --no-pause > trace.jsonl
//   # Or attach to an already-running instance (use exact image name):
//   frida -l tools/frida/cursor_trace.js Bulanci.exe > trace.jsonl
//
// Each emitted record is one line of JSON. Pipe to a .jsonl file and
// analyse offline (python -c "import json,sys;[print(json.loads(l)) for l in sys.stdin]" etc.).
// ─────────────────────────────────────────────────────────────────────

"use strict";

// Original Bulanci.exe is a 32-bit MSVC8 PE with no /DYNAMICBASE flag,
// so in practice it loads at its preferred ImageBase. We compute the
// runtime slide anyway so the script also works if you ever rebase
// the binary (or run under EMET / a Detours-style loader).
const IMAGE_BASE_PE = 0x00400000;

function findGameModule() {
    const candidates = ["Bulanci.exe", "bulanci.exe", "BULANCI.EXE"];
    for (const name of candidates) {
        const m = Process.findModuleByName(name);
        if (m !== null) return m;
    }
    // Fall back to the main module. Filter out frida-agent.dll etc.
    const mods = Process.enumerateModules();
    for (const m of mods) {
        if (m.name.toLowerCase().indexOf("bulanci") !== -1) return m;
    }
    return mods[0];
}

const MOD = findGameModule();
const MODULE_BASE = MOD.base;
const SLIDE = MODULE_BASE.sub(IMAGE_BASE_PE);

console.log(
    `[+] Bulanci module loaded: ${MOD.name} @ ${MODULE_BASE} (slide=${SLIDE.toInt32()})`,
);

function abs(va) {
    return ptr(va).add(SLIDE);
}

// ────────────────────────────────── well-known offsets from Ghidra
const ADDR = {
    CGunMouse_Draw:        abs(0x00424610),
    CGunMouse_OnMouseMove: abs(0x00423900),
    CGunMouse_Activate:    abs(0x00423b50),
    CGunMouse_Erase:       abs(0x004249b0),
    CDSApp_MouseQueue:     abs(0x0042a5c0),
    // CDSApp::DispatchInputEvent — writes [g_pApp+0xe0]/[+0xe4] from the
    // event's payload before fanning the event out to the input chain.
    CDSApp_DispatchInputEvent: abs(0x00429db0),
    g_dwElapsedMs:         abs(0x004b3bd8),
};

// ────────────────────────────────── helpers
function nowMs() {
    return ADDR.g_dwElapsedMs.readU32();
}

let recordCounter = 0;

function emit(eventName, payload) {
    recordCounter += 1;
    const rec = { n: recordCounter, t: nowMs(), ev: eventName };
    for (const k of Object.keys(payload)) rec[k] = payload[k];
    console.log(JSON.stringify(rec));
}

// Read the full CGunMouse object slice we care about, including a
// raw walk over the 30-slot ring buffer in **chronological order**
// (oldest first). This is what makes the trace useful: we always
// see exactly what the engine has lined up to drive `target_*`
// next.
function safeReadS32(p, off) {
    try { return p.add(off).readS32(); } catch (e) { return null; }
}
function safeReadU32(p, off) {
    try { return p.add(off).readU32(); } catch (e) { return null; }
}
function safeReadU8(p, off) {
    try { return p.add(off).readU8(); } catch (e) { return null; }
}

function readGunMouse(thisPtr) {
    // Queue header is at +0x208; layout (from FUN_00424370 / FUN_004243b0):
    //   +0x00 data ptr   +0x04 capacity   +0x08 size   +0x0c head
    const qBase = thisPtr.add(0x208);
    const qDataRaw = safeReadU32(qBase, 0);
    const qCap = safeReadU32(qBase, 0x04);
    const qSize = safeReadU32(qBase, 0x08);
    const qHead = safeReadU32(qBase, 0x0c);

    const queue = [];
    // Defensively cap: a real CGunMouse queue is 30 entries; anything
    // beyond that is almost certainly a wrong-`this` mis-read.
    const safeCap = qCap !== null && qCap > 0 && qCap <= 64 ? qCap : 0;
    const safeSize = qSize !== null && qSize >= 0 && qSize <= safeCap ? qSize : 0;
    if (safeCap > 0 && qDataRaw !== null && qDataRaw !== 0) {
        const qDataPtr = ptr(qDataRaw);
        for (let i = 0; i < safeSize; i++) {
            const slot = (qHead + i) % safeCap;
            const a = safeReadS32(qDataPtr, slot * 8);
            const b = safeReadS32(qDataPtr, slot * 8 + 4);
            if (a === null || b === null) break;
            queue.push([a, b]);
        }
    }

    return {
        mouse:    [safeReadS32(thisPtr, 0x190), safeReadS32(thisPtr, 0x194)],
        target:   [safeReadS32(thisPtr, 0x198), safeReadS32(thisPtr, 0x19c)],
        dotOff:   [safeReadS32(thisPtr, 0x1a0), safeReadS32(thisPtr, 0x1a4)],
        dotBase:  [safeReadS32(thisPtr, 0x1a8), safeReadS32(thisPtr, 0x1ac)],
        lastTick:  safeReadU32(thisPtr, 0x1f8),
        flag1f9:   safeReadU8(thisPtr, 0x1f9),
        flag204:   safeReadU8(thisPtr, 0x204),
        flag205:   safeReadU8(thisPtr, 0x205),
        dotReady:  safeReadU8(thisPtr, 0x206),
        q: { cap: qCap, size: qSize, head: qHead, data: queue },
    };
}

function readApp(appPtr) {
    const i32 = (off) => appPtr.add(off).readS32();
    return {
        e0e4: [i32(0xe0), i32(0xe4)],
        f0f4: [i32(0xf0), i32(0xf4)],
        f8fc: [i32(0xf8), i32(0xfc)],
    };
}

// Short backtrace, only walking up frames inside the game module.
function shortBacktrace(ctx, depth) {
    try {
        const frames = Thread.backtrace(ctx, Backtracer.ACCURATE).slice(0, depth);
        return frames.map((f) => {
            const sym = DebugSymbol.fromAddress(f);
            const rva = f.sub(MODULE_BASE).toInt32();
            return `${f}=${MOD.name}+0x${rva.toString(16)} (${sym.name || "?"})`;
        });
    } catch (e) {
        return [`<backtrace failed: ${e.message}>`];
    }
}

// ──────────────────────────────────────────── CGunMouse::Draw (per-frame)
//
// __thiscall: ECX = this. No stack args. We snapshot the full struct
// twice — once on entry (BEFORE the per-frame Erase block has run)
// and once on exit (AFTER push/pop and after the dot has been blit).
// The on-exit `target` is the value the dot was actually drawn with.
Interceptor.attach(ADDR.CGunMouse_Draw, {
    onEnter() {
        this.gm = this.context.ecx;
        this.before = readGunMouse(this.gm);
    },
    onLeave() {
        const after = readGunMouse(this.gm);
        emit("Draw", {
            gm: this.gm.toString(),
            before: this.before,
            after: after,
        });
    },
});

// ────────────────────────────────────────── CGunMouse::OnMouseMove
//
// __thiscall(this=ECX, undefined4 param_1, int* param_2). Crucial
// detail from the live trace: ECX here is **NOT** the CGunMouse
// pointer — it's the **embedded observer sub-object at
// `CGunMouse + 0xc`** (vtable3, the third vtable the ctor wires up).
// We therefore subtract 0xc before any field read.
//
// With `gm := ECX - 0xc`, the asm offsets re-map to actual CGunMouse
// fields as follows:
//   [ECX + 0x194 / 0x198] = gm + 0x1a0 / 0x1a4 = dot_offset_X / Y
//   [ECX + 0x19c / 0x1a0] = gm + 0x1a8 / 0x1ac = dot_base_X / Y   (write target)
//   [ECX + 0x1f8]         = gm + 0x204         = dot_needs_update (LSB)
//
// So the function in CGunMouse semantics is exactly:
//   if (dot_needs_update) { dot_base += (dx, dy); }
//   else { dot_base = dot_offset + (dx, dy); dot_needs_update = 1; }
//
// The two deltas in param_2 come straight from a BitmapSprite
// opcode-0x0A frame — they are the (dx, dy) of a predefined,
// closed-loop motion path replayed via DecodeFrame → NotifyMove →
// observer.vtable[+4].
Interceptor.attach(ADDR.CGunMouse_OnMouseMove, {
    onEnter() {
        // Real CGunMouse* lives 12 bytes behind the observer.
        this.gm = this.context.ecx.sub(0xc);
        this.observer = this.context.ecx;
        const esp = this.context.esp;
        this.param1 = esp.add(0x04).readU32();
        const p2 = ptr(esp.add(0x08).readU32());
        this.deltas = [p2.readS32(), p2.add(4).readS32()];
        try {
            this.before = readGunMouse(this.gm);
        } catch (e) {
            this.before = { error: String(e) };
        }
        this.bt = shortBacktrace(this.context, 6);
    },
    onLeave() {
        let after;
        try {
            after = readGunMouse(this.gm);
        } catch (e) {
            after = { error: String(e) };
        }
        emit("OnMouseMove", {
            gm: this.gm.toString(),
            observer: this.observer.toString(),
            param1: `0x${this.param1.toString(16)}`,
            deltas: this.deltas,
            before: this.before,
            after: after,
            backtrace: this.bt,
        });
    },
});

// ─────────────────────────────────────────────────── CGunMouse::Activate
Interceptor.attach(ADDR.CGunMouse_Activate, {
    onEnter() {
        this.gm = this.context.ecx;
    },
    onLeave() {
        emit("Activate", {
            gm: this.gm.toString(),
            after: readGunMouse(this.gm),
        });
    },
});

// ────────────────────────────────────────────────────── CGunMouse::Erase
Interceptor.attach(ADDR.CGunMouse_Erase, {
    onEnter() {
        this.gm = this.context.ecx;
        this.before = readGunMouse(this.gm);
    },
    onLeave() {
        emit("Erase", {
            gm: this.gm.toString(),
            before: this.before,
            after: readGunMouse(this.gm),
        });
    },
});

// ─────────────────────────────────────────── CDSApp::MouseQueue (WM_MOUSEMOVE)
//
// __thiscall:
//   ECX  = CBulanci *this    (also stored as g_pApp)
//   esp+4  = u16 param_1      (event-class flags)
//   esp+8  = u16 param_2      (event-id / sub-class)
//   esp+0xc = u32 param_3     (packed: low16 = X, high16 = Y) — sign-extended
//                              by the function before storing to +0xf8/+0xfc
//
// We also snapshot the four mouse-related fields on CBulanci so the
// post-condition can be cross-checked against the per-frame Draw.
Interceptor.attach(ADDR.CDSApp_MouseQueue, {
    onEnter() {
        this.app = this.context.ecx;
        const esp = this.context.esp;
        const p1 = esp.add(0x04).readU16();
        const p2 = esp.add(0x08).readU16();
        const packed = esp.add(0x0c).readU32();
        // Sign-extend the two halves of LPARAM-style packed XY.
        const xs = (packed & 0xffff) << 16 >> 16;
        const ys = ((packed >>> 16) & 0xffff) << 16 >> 16;
        this.payload = { p1, p2, packed: `0x${packed.toString(16)}`, raw: [xs, ys] };
        this.before = readApp(this.app);
    },
    onLeave() {
        emit("MouseQueue", {
            app: this.app.toString(),
            ...this.payload,
            before: this.before,
            after: readApp(this.app),
        });
    },
});

// ────────────────────────────────────── CDSApp::DispatchInputEvent
//
// __thiscall(this=ECX, CDSEvent* param_1). The function writes the
// event's payload (param_1+0xc / +0x10) into this+0xe0/+0xe4 — those
// two writes are what every subsequent OS-mouse-aware widget reads.
// We emit one record per dispatched event with the event class flags
// at +4 so we can isolate true mouse-move events from key/button.
Interceptor.attach(ADDR.CDSApp_DispatchInputEvent, {
    onEnter() {
        this.app = this.context.ecx;
        const ev = ptr(this.context.esp.add(0x04).readU32());
        const cls = ev.add(0x04).readU16();
        const x = ev.add(0x0c).readS32();
        const y = ev.add(0x10).readS32();
        // NOTE: name this `eventPtr`, not `ev` — `ev` is the
        // emit() event-name field and would otherwise overwrite
        // "DispatchInputEvent" with this pointer string.
        this.payload = {
            eventPtr: ev.toString(),
            classFlags: `0x${cls.toString(16)}`,
            xy: [x, y],
        };
        this.before = readApp(this.app);
    },
    onLeave() {
        emit("DispatchInputEvent", {
            app: this.app.toString(),
            ...this.payload,
            before: this.before,
            after: readApp(this.app),
        });
    },
});

console.log("[+] CGunMouse + CDSApp instrumentation installed. Move the mouse.");
