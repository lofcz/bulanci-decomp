/* eslint-disable */
"use strict";
//
// Bulanci main-menu CSwitch hover-audio tracer.
//
// Answers exactly:
//   1. Does CSwitch_PlayHoverTrack fire on every hover, or only when the
//      switch is in the idle state (CSwitch.state_0xc4 == 0)?
//   2. With what volumePercent (+0x54) is the hover sample initialised?
//   3. At which engine-clock millisecond (g_dwElapsedMs) does each
//      enter / leave / hover-track / sample-trigger event occur?
//   4. Does retail STOP an in-flight hover sample when a new hover
//      starts (voice-stealing), or does it genuinely let multiple
//      hover voices mix? -> hook CDSAudioPlayer_Play / _Stop and check
//      whether Stop is ever called on a tracked player with
//      `fromInsidePlay == false` AND `sampleCursor_0x28 < byteLen`.
//      Such an event would be a smoking gun for voice-stealing; its
//      absence proves there isn't any.
//   5. Exactly when does the per-button slot-0 bit get armed / acked?
//      -> filtered Scheduler_ArmSlot / Scheduler_AckSlot hooks emit
//      ONLY when ECX matches one of the three menu CSwitch schedulers
//      (discovered at runtime from CSwitch_OnMouseEnter).
//
// Run alongside the working orig\patch_window.js so you actually have
// a draggable window to mouse-hover in:
//
//   frida -l orig\patch_window.js ^
//         -l scripts\frida\menu_hover_audio_trace.js ^
//         -f orig\bulanci.exe ^
//         -o hover_audio.jsonl
//
// Type %resume in the Frida prompt if it does not auto-resume.
//
// Design notes (this file mirrors patch_window.js patterns):
//   * `this.context.ecx` is only read inside onEnter and stashed on
//     `this.*`. onLeave never re-reads ECX (after the call ECX holds
//     whatever the callee wrote, NOT the original `this`).
//   * Every hook body is wrapped in try / catch so a struct-read error
//     never escapes into Frida's dispatch.
//   * A Process.setExceptionHandler dumps full register / disasm /
//     stack context if anything still crashes -- so a future failure
//     is diagnosable instead of a silent "Process terminated".
//   * Function addresses below were verified against Ghidra at runtime
//     via the ghidra-mcp server before this commit. The previous
//     iteration of this script hooked CSwitch_PlayHoverTrack at
//     0x00423f50 -- which is 0x30 bytes INTO the function (the entry
//     is 0x00423f20). Frida's 5-byte inline trampoline overwrote
//     instructions in the middle of the function, so the first call
//     into PlayHoverTrack jumped to garbage and the process died. The
//     fixed entry below is the function's actual start.
//
// ─────────────────────────────────────────── module discovery
const IMAGE_BASE_PE = 0x00400000;
function findGameModule() {
    const names = [
        "Bulanci.exe", "bulanci.exe", "BULANCI.EXE",
        "Bulanci_insturmented.exe", "bulanci_insturmented.exe",
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
    console.error("[-] Could not locate Bulanci module - aborting tracer");
    throw new Error("Bulanci module not found");
}
const MODULE_BASE = MOD.base;
const SLIDE = MODULE_BASE.sub(IMAGE_BASE_PE);
function abs(va) { return ptr(va).add(SLIDE); }

// ─────────────────────────────────────────── verified addresses
// All addresses double-checked via `ghidra-mcp.get_function_by_address`.
//   CSwitch_OnMouseEnter   @ 0x00424cf0  (16-byte tail-call thunk)
//   CSwitch_OnMouseLeave   @ 0x00424d10  (16-byte tail-call thunk)
//   CSwitch_PlayIdleTrack  @ 0x00423ef0  (46-byte function)
//   CSwitch_PlayHoverTrack @ 0x00423f20  (77-byte function)  <-- WAS WRONG ONCE
//   TriggerBankSample      @ 0x00422430
//   CDSAudioPlayer_Init    @ 0x0043a760
//   CDSAudioPlayer_Play    @ 0x0043a9d0
//   CDSAudioPlayer_Stop    @ 0x0043a4a0
//   Scheduler_ArmSlot      @ 0x0042f300  (was FUN_0042f300)
//   Scheduler_AckSlot      @ 0x0042f330  (was FUN_0042f330)
const ADDR = {
    CSwitch_OnMouseEnter:   abs(0x00424cf0),
    CSwitch_OnMouseLeave:   abs(0x00424d10),
    CSwitch_PlayIdleTrack:  abs(0x00423ef0),
    CSwitch_PlayHoverTrack: abs(0x00423f20),
    TriggerBankSample:      abs(0x00422430),
    CDSAudioPlayer_Init:    abs(0x0043a760),
    CDSAudioPlayer_Play:    abs(0x0043a9d0),
    CDSAudioPlayer_Stop:    abs(0x0043a4a0),
    Scheduler_ArmSlot:      abs(0x0042f300),
    Scheduler_AckSlot:      abs(0x0042f330),
    g_dwElapsedMs:          abs(0x004b3bd8),
};

console.error(`[+] menu_hover_audio_trace.js loaded: ${MOD.name} @ ${MODULE_BASE} (slide=${SLIDE.toInt32()})`);
console.error(`[+] CSwitch_OnMouseEnter      @ ${ADDR.CSwitch_OnMouseEnter}`);
console.error(`[+] CSwitch_OnMouseLeave      @ ${ADDR.CSwitch_OnMouseLeave}`);
console.error(`[+] CSwitch_PlayIdleTrack     @ ${ADDR.CSwitch_PlayIdleTrack}`);
console.error(`[+] CSwitch_PlayHoverTrack    @ ${ADDR.CSwitch_PlayHoverTrack}`);
console.error(`[+] TriggerBankSample         @ ${ADDR.TriggerBankSample}`);
console.error(`[+] CDSAudioPlayer_Init       @ ${ADDR.CDSAudioPlayer_Init}`);
console.error(`[+] CDSAudioPlayer_Play       @ ${ADDR.CDSAudioPlayer_Play}`);
console.error(`[+] CDSAudioPlayer_Stop       @ ${ADDR.CDSAudioPlayer_Stop}`);
console.error(`[+] Scheduler_ArmSlot         @ ${ADDR.Scheduler_ArmSlot}`);
console.error(`[+] Scheduler_AckSlot         @ ${ADDR.Scheduler_AckSlot}`);

// Menu-button slots in the global AudioBank (slot 0 = hover voice cue).
// Other slots are voice clips fired by Cmd_Dispatch on click; we filter
// to these to keep noise out of the trace.
const TRACE_SLOTS = { 0: "hover", 0x18: "history", 0x19: "quit", 0x1a: "x_exit", 0x1b: "start", 0x1c: "f12" };

// ─────────────────────────────────────────── helpers
let seq = 0;
let pendingTriggerSlot = null;

// Map<playerPtrString, {kind, slot, createdAt}> -- every player that
// originated from a TriggerBankSample for a slot we care about.
// Populated in CDSAudioPlayer_Init.onEnter, consulted from Play/Stop
// hooks to decide whether to emit and what label to attach.
const trackedPlayers = new Map();

// Map<switchPtrString, {x, y, cmd, label}> -- learned at runtime from
// CSwitch_OnMouseEnter. Lets Arm/Ack hooks filter to the three menu
// switches' schedulers (each scheduler lives at switchPtr+0x80, but
// the function-call `this` is that scheduler ptr -- so we compute the
// CSwitch ptr by subtracting 0x80 in the hook).
const knownSwitches = new Map();
function rememberSwitch(swPtr, info) {
    const key = ptr(swPtr).toString();
    if (!knownSwitches.has(key)) knownSwitches.set(key, info);
}
function switchForScheduler(schedThisPtr) {
    // CSwitch's scheduler lives at CSwitch+0x80 (verified from
    // ConstructTrackManager + CSwitch_PlayHoverTrack offsets). The
    // Scheduler functions receive ECX = (CSwitch+0x80).
    const swCandidate = ptr(schedThisPtr).sub(0x80);
    const key = swCandidate.toString();
    if (knownSwitches.has(key)) {
        return { sw: key, info: knownSwitches.get(key) };
    }
    return null;
}

function nowMs() { try { return ADDR.g_dwElapsedMs.readU32(); } catch (e) { return null; } }
function hex(p)  { try { return ptr(p).toString(); } catch (e) { return String(p); } }
function safe(fn, fb) { try { return fn(); } catch (e) { return fb; } }
function readS32(p, o) { return safe(() => ptr(p).add(o).readS32(), null); }
function readU32(p, o) { return safe(() => ptr(p).add(o).readU32(), null); }
function readU16(p, o) { return safe(() => ptr(p).add(o).readU16(), null); }
function readU8(p, o)  { return safe(() => ptr(p).add(o).readU8(),  null); }

function emit(ev, payload) {
    seq += 1;
    const rec = { n: seq, t: nowMs(), ev };
    if (payload) for (const k of Object.keys(payload)) rec[k] = payload[k];
    try { console.log(JSON.stringify(rec)); } catch (e) { console.log(`${ev} (unserializable)`); }
}

function switchInfo(sw) {
    const p = ptr(sw);
    return {
        ptr: hex(p),
        x:           readS32(p, 0x20),
        y:           readS32(p, 0x24),
        flags1:      readU32(p, 0x44),
        aux0_0xa8:   readS32(p, 0xa8),
        aux1_0xb4:   readS32(p, 0xb4),
        state_0xc4:  readU8(p,  0xc4),
        cmd_0xc6:    readU16(p, 0xc6),
    };
}

// __fastcall / __thiscall (single arg): `this` in ECX, no stack args.
// We snapshot ECX *only* inside onEnter; never re-read it in onLeave.
function hookFastcallThis(address, name, callback) {
    Interceptor.attach(address, {
        onEnter() {
            try {
                callback(ptr(this.context.ecx));
            } catch (e) {
                emit("hook_error", { hook: name, error: String(e) });
            }
        },
    });
}

// ─────────────────────────────────────────── exception handler
// If anything below still misbehaves, this dumps a structured record
// so we get a real bug report instead of a bare "Process terminated".
function describeAddress(addr) {
    const p = ptr(addr);
    const inMod = p.compare(MODULE_BASE) >= 0 && p.compare(MODULE_BASE.add(MOD.size)) < 0;
    return { addr: hex(p), inGameModule: inMod, rva: inMod ? p.sub(MODULE_BASE).toString() : null };
}
function dumpBytes(p, len) {
    try {
        const buf = ptr(p).readByteArray(len);
        return Array.from(new Uint8Array(buf)).map(b => b.toString(16).padStart(2, "0")).join(" ");
    } catch (e) { return null; }
}
Process.setExceptionHandler(function (details) {
    try {
        const ctx = details.context;
        emit("EXCEPTION", {
            type: details.type,
            message: details.message,
            memoryOperation: details.memory ? details.memory.operation : null,
            memoryAddress:   details.memory ? hex(details.memory.address) : null,
            faultAt:         describeAddress(details.address),
            eax: hex(ctx.eax), ebx: hex(ctx.ebx), ecx: hex(ctx.ecx), edx: hex(ctx.edx),
            esi: hex(ctx.esi), edi: hex(ctx.edi), ebp: hex(ctx.ebp), esp: hex(ctx.esp),
            eip: hex(ctx.eip),
            faultBytes: dumpBytes(details.address, 32),
        });
    } catch (e) {}
    return false; // let the OS terminate as usual after we've recorded.
});

// ─────────────────────────────────────────── CSwitch hooks
//
// CSwitch_OnMouseEnter (0x00424cf0):
//   if (state_0xc4 == 0) CSwitch_PlayHoverTrack(this);
//   -> fires on EVERY mouse-enter; the state==0 check happens inside.
//
// CSwitch_PlayHoverTrack (0x00423f20):
//   Builds a fresh CDSAudioPlayer for AudioBank slot 0, plays-and-releases.
//   -> Only reached when OnMouseEnter or OnAnimEnd decides the switch is
//      eligible (idle state == 0). If we see this event, retail decided
//      to play the hover sample.
//
// CSwitch_PlayIdleTrack (0x00423ef0):
//   Counterpart to PlayHoverTrack; mouse-leave path.
//
// CSwitch_OnMouseLeave (0x00424d10):
//   Decides whether to invoke PlayIdleTrack.
hookFastcallThis(ADDR.CSwitch_OnMouseEnter, "CSwitch_OnMouseEnter", function (sw) {
    const info = switchInfo(sw);
    // Tag this switch ptr so Scheduler_ArmSlot / Scheduler_AckSlot
    // hooks can filter to the three menu CSwitches at runtime
    // (their CSwitch addresses are heap-allocated and not stable
    // across runs).
    const cmd = info.cmd_0xc6;
    const label =
        cmd === 0xc9 ? "START" :
        cmd === 0xca ? "HISTORY" :
        cmd === 0xcb ? "QUIT" :
        `cmd_${cmd}`;
    rememberSwitch(sw, { x: info.x, y: info.y, cmd, label });
    emit("CSwitch_OnMouseEnter", { label, sw: info });
});
hookFastcallThis(ADDR.CSwitch_OnMouseLeave, "CSwitch_OnMouseLeave", function (sw) {
    emit("CSwitch_OnMouseLeave", { sw: switchInfo(sw) });
});
hookFastcallThis(ADDR.CSwitch_PlayHoverTrack, "CSwitch_PlayHoverTrack", function (sw) {
    emit("CSwitch_PlayHoverTrack", { sw: switchInfo(sw) });
});
hookFastcallThis(ADDR.CSwitch_PlayIdleTrack, "CSwitch_PlayIdleTrack", function (sw) {
    emit("CSwitch_PlayIdleTrack", { sw: switchInfo(sw) });
});

// ─────────────────────────────────────────── TriggerBankSample
//
// __cdecl TriggerBankSample(flags, AudioBankIndex* bank, slot,
//                           preDelay, eventTarget, looping)
//   slot 0    -> hover voice cue
//   slot 0x1b -> Start click voice cue
//   slot 0x18 -> History click voice cue
//   slot 0x19 -> Quit click voice cue
//   slot 0x1a -> X-button exit (DispatchHotkey)
//   slot 0x1c -> F12 (DispatchHotkey)
//
// pendingTriggerSlot links this call to the immediately nested
// CDSAudioPlayer_Init hook below; the chain is single-threaded so the
// state machine is race-free.
Interceptor.attach(ADDR.TriggerBankSample, {
    onEnter(args) {
        try {
            const slot = args[2].toInt32();
            if (!Object.prototype.hasOwnProperty.call(TRACE_SLOTS, slot)) {
                this.trace = false;
                return;
            }
            this.trace = true;
            pendingTriggerSlot = slot;
            emit("TriggerBankSample", {
                kind:    TRACE_SLOTS[slot],
                slot,
                flags:   args[0].toInt32(),
                bank:    hex(args[1]),
                preDelay: args[3].toInt32(),
                eventTarget: hex(args[4]),
                looping: args[5].toInt32() & 0xff,
            });
        } catch (e) {
            this.trace = false;
            emit("hook_error", { hook: "TriggerBankSample.onEnter", error: String(e) });
        }
    },
    onLeave() {
        if (this.trace) pendingTriggerSlot = null;
    },
});

// ─────────────────────────────────────────── CDSAudioPlayer_Init
//
// __thiscall CDSAudioPlayer_Init(this, sample, flags, eventTarget,
//                                categoryOrTarget)
//   - Read default volumePercent at +0x54 (retail engine units: 0..100).
//   - Read category pointer at +0x4c.
//   - Read PCM sample header at sample+0x04..+0x0e.
//
// We only emit for Init calls that came from a TriggerBankSample we
// care about (pendingTriggerSlot != null), so background-music Init
// calls never touch our reads.
Interceptor.attach(ADDR.CDSAudioPlayer_Init, {
    onEnter(args) {
        try {
            if (pendingTriggerSlot === null) { this.trace = false; return; }
            this.trace = true;
            this.player = ptr(this.context.ecx);
            this.slot = pendingTriggerSlot;
            this.sample = args[0];
            this.flags = args[1].toUInt32();
            this.eventTarget = ptr(args[2]);
        } catch (e) {
            this.trace = false;
            emit("hook_error", { hook: "CDSAudioPlayer_Init.onEnter", error: String(e) });
        }
    },
    onLeave() {
        if (!this.trace) return;
        try {
            const p = this.player;
            const sample = this.sample;
            const sampleInfo = sample.isNull() ? null : {
                ptr:        hex(sample),
                byteLen:    readU32(sample, 0x04),
                channels:   readU16(sample, 0x08),
                bits:       readU16(sample, 0x0a),
                sampleRate: readU32(sample, 0x0c),
            };
            // Tag this player so the Play / Stop hooks below can
            // filter to menu-originated samples and avoid emitting
            // events for the background music / other systems.
            trackedPlayers.set(p.toString(), {
                kind:      TRACE_SLOTS[this.slot],
                slot:      this.slot,
                createdAt: nowMs(),
            });
            emit("CDSAudioPlayer_Init", {
                kind:                       TRACE_SLOTS[this.slot],
                slot:                       this.slot,
                player:                     hex(p),
                sample:                     sampleInfo,
                flags:                      this.flags,
                eventTarget:                hex(this.eventTarget),
                volumePercent_0x54:         readS32(p, 0x54),
                cachedEffectiveVolume_0x50: readS32(p, 0x50),
                category_0x4c:              hex(safe(() => p.add(0x4c).readPointer(), ptr(0))),
                liveTrackedCount:           trackedPlayers.size,
            });
        } catch (e) {
            emit("hook_error", { hook: "CDSAudioPlayer_Init.onLeave", error: String(e) });
        }
    },
});

// ─────────────────────────────────────────── CDSAudioPlayer_Play
//
// __thiscall, ECX = this. param_1 is the play-flags byte. We emit
// only for players we tagged in Init above. The first thing inside
// Play is a `Stop(this, 1)` -- harmless for freshly created players
// because their +0x22 "active" byte is 0; we identify it from the
// Stop hook by checking the return address (it lands inside Play).
Interceptor.attach(ADDR.CDSAudioPlayer_Play, {
    onEnter() {
        try {
            const self = ptr(this.context.ecx);
            const tag = trackedPlayers.get(self.toString());
            if (!tag) { this.trace = false; return; }
            this.trace = true;
            this.self = self;
            this.tag = tag;
            emit("CDSAudioPlayer_Play.enter", {
                kind:                       tag.kind,
                slot:                       tag.slot,
                player:                     hex(self),
                volumePercent_0x54:         readS32(self, 0x54),
                cachedEffectiveVolume_0x50: readS32(self, 0x50),
                liveTrackedCount:           trackedPlayers.size,
            });
        } catch (e) {
            this.trace = false;
            emit("hook_error", { hook: "CDSAudioPlayer_Play.onEnter", error: String(e) });
        }
    },
    onLeave() {
        if (!this.trace) return;
        try {
            emit("CDSAudioPlayer_Play.leave", {
                kind:                       this.tag.kind,
                player:                     hex(this.self),
                active_0x22:                readU8(this.self, 0x22),
                dsPlaying_0x21:             readU8(this.self, 0x21),
                cachedEffectiveVolume_0x50: readS32(this.self, 0x50),
            });
        } catch (e) {
            emit("hook_error", { hook: "CDSAudioPlayer_Play.onLeave", error: String(e) });
        }
    },
});

// ─────────────────────────────────────────── CDSAudioPlayer_Stop
//
// __thiscall, ECX = this. We emit ONLY when:
//   (a) `this` is a tracked menu player AND
//   (b) the call did not originate inside CDSAudioPlayer_Play's first
//       instruction (that Stop is a no-op cleanup for freshly created
//       players and would just flood the log).
//
// The smoking-gun event the user asked about: if a tracked player
// receives a Stop while another tracked player from a *different*
// CSwitch is currently active, retail is doing voice-stealing.
// If the only Stop calls we see are post-Init from inside Play
// itself, retail genuinely allows overlap (no stealing).
const PLAY_BODY_START = ADDR.CDSAudioPlayer_Play;
const PLAY_BODY_END   = ADDR.CDSAudioPlayer_Play.add(0x93); // body 0x0043a9d0..0x0043aa62
Interceptor.attach(ADDR.CDSAudioPlayer_Stop, {
    onEnter(args) {
        try {
            const self = ptr(this.context.ecx);
            const tag = trackedPlayers.get(self.toString());
            if (!tag) { this.trace = false; return; }
            const retAddr = this.returnAddress;
            const fromInsidePlay =
                retAddr.compare(PLAY_BODY_START) >= 0 && retAddr.compare(PLAY_BODY_END) < 0;
            this.trace = true;
            this.self = self;
            this.tag = tag;
            emit("CDSAudioPlayer_Stop.enter", {
                kind:                this.tag.kind,
                player:              hex(self),
                drain:               args[0].toInt32() & 0xff,
                returnAddress:       hex(retAddr),
                fromInsidePlay,
                active_0x22:         readU8(self, 0x22),
                dsPlaying_0x21:      readU8(self, 0x21),
                sampleCursor_0x28:   readU32(self, 0x28),
            });
        } catch (e) {
            this.trace = false;
            emit("hook_error", { hook: "CDSAudioPlayer_Stop.onEnter", error: String(e) });
        }
    },
    onLeave() {
        if (!this.trace) return;
        try {
            emit("CDSAudioPlayer_Stop.leave", {
                kind:           this.tag.kind,
                player:         hex(this.self),
                active_0x22:    readU8(this.self, 0x22),
                dsPlaying_0x21: readU8(this.self, 0x21),
            });
        } catch (e) {
            emit("hook_error", { hook: "CDSAudioPlayer_Stop.onLeave", error: String(e) });
        }
    },
});

// ─────────────────────────────────────────── Scheduler_ArmSlot / AckSlot
//
// Filter to slot==0 calls on a CSwitch's scheduler. This is THE bit
// (slot[+8] & 1) that gates PlayHoverTrack -- seeing every arm/ack on
// it lets us reconstruct the per-button cooldown mechanism from first
// principles instead of guessing.
function hookSchedulerOp(addr, evName) {
    Interceptor.attach(addr, {
        onEnter(args) {
            try {
                const sched = ptr(this.context.ecx);
                const slotIdx = args[0].toInt32();
                if (slotIdx !== 0) { this.trace = false; return; }
                const matched = switchForScheduler(sched);
                if (!matched) { this.trace = false; return; }
                this.trace = true;
                emit(evName, {
                    label:    matched.info.label,
                    sched:    hex(sched),
                    sw:       matched.sw,
                    slotIdx,
                });
            } catch (e) {
                this.trace = false;
                emit("hook_error", { hook: evName, error: String(e) });
            }
        },
    });
}
hookSchedulerOp(ADDR.Scheduler_ArmSlot, "Scheduler_ArmSlot");
hookSchedulerOp(ADDR.Scheduler_AckSlot, "Scheduler_AckSlot");

emit("trace_start", {
    module:     MOD.name,
    moduleBase: hex(MODULE_BASE),
    slide:      SLIDE.toInt32(),
    note:       [
        "Full menu-hover audio tracer.",
        "Events: CSwitch enter/leave/hover/idle, TriggerBankSample (menu slots),",
        "CDSAudioPlayer Init/Play/Stop (only for menu-originated players,",
        "with returnAddress.fromInsidePlay flag), Scheduler ArmSlot/AckSlot",
        "(only for slot 0 on the three menu CSwitch schedulers).",
        "Smoking gun: a Stop.enter event with fromInsidePlay=false on a",
        "tracked player AND with sampleCursor_0x28 less than the sample byteLen",
        "would prove retail kills hover voices mid-play.",
    ].join(" "),
});

console.error("[+] All menu-hover hooks installed - waiting for events");
