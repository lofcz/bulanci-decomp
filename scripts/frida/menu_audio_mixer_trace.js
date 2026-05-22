/* eslint-disable */
"use strict";
//
// Bulanci main-menu audio mixer/attenuation tracer.
//
// This complements menu_hover_audio_trace.js. The hover trace answers when
// CSwitch emits SFX; this one answers how retail maps music/SFX volume through
// CDSAudioPlayer and DirectSound.
//
// Run with a window patch so the menu is usable:
//
//   frida -l orig\patch_window.js ^
//         -l scripts\frida\menu_audio_mixer_trace.js ^
//         -f orig\bulanci.exe ^
//         -o menu_audio_mixer.jsonl
//
// Type %resume in the Frida prompt if it does not auto-resume.
//

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

const ADDR = {
    CMenu_LoadBackgroundMusic:           abs(0x004252a0),
    CMenu_SetDayNightBg:                 abs(0x004252f0),
    CMenu_EnableBackgroundState:         abs(0x00424010),
    CMenu_OnMusicFadeTick:               abs(0x00424080),
    CDSAudioPlayer_CreateFromResource:   abs(0x00422550),
    CDSAudioPlayer_Init:                 abs(0x0043a760),
    CDSAudioPlayer_Play:                 abs(0x0043a9d0),
    CDSAudioPlayer_PlayAndRelease:       abs(0x004223c0),
    CDSAudioPlayer_Stop:                 abs(0x0043a4a0),
    CDSAudioPlayer_SetVolumePercent:     abs(0x0043a0d0),
    CDSAudioPlayer_ApplyEffectiveVolume: abs(0x0043a060),
    TriggerBankSample:                   abs(0x00422430),
    g_dwElapsedMs:                       abs(0x004b3bd8),
    g_volumeBusDbTable:                  abs(0x004b8370),
};

const RESOURCE_NAMES = {
    0x10149: "menu_day_ambient",
    0x1014a: "menu_night_ambient",
};

const MENU_BANK_SLOTS = {
    0: "hover",
    0x18: "history",
    0x19: "quit",
    0x1a: "x_exit",
    0x1b: "start",
    0x1c: "f12",
};

let seq = 0;
let pendingCreate = null;
let pendingBankSlot = null;
const playerMeta = new Map();
const bufferToPlayer = new Map();
const hookedSetVolumeTargets = new Set();

function nowMs() { try { return ADDR.g_dwElapsedMs.readU32(); } catch (e) { return null; } }
function hex(p)  { try { return ptr(p).toString(); } catch (e) { return String(p); } }
function key(p)  { return hex(p); }
function safe(fn, fb) { try { return fn(); } catch (e) { return fb; } }
function readS32(p, o) { return safe(() => ptr(p).add(o).readS32(), null); }
function readU32(p, o) { return safe(() => ptr(p).add(o).readU32(), null); }
function readU16(p, o) { return safe(() => ptr(p).add(o).readU16(), null); }
function readU8(p, o)  { return safe(() => ptr(p).add(o).readU8(), null); }
function readPtr(p, o) { return safe(() => ptr(p).add(o).readPointer(), ptr(0)); }

function emit(ev, payload) {
    seq += 1;
    const rec = { n: seq, t: nowMs(), ev };
    if (payload) for (const k of Object.keys(payload)) rec[k] = payload[k];
    try { console.log(JSON.stringify(rec)); } catch (e) { console.log(`${ev} (unserializable)`); }
}

function dsHundredthDbToGain(db100) {
    if (db100 === null) return null;
    return Math.pow(10, db100 / 2000.0);
}

function volumeBusDb(category) {
    if (category === null || category <= 0) return 0;
    return safe(() => ADDR.g_volumeBusDbTable.add(category * 4).readS32(), null);
}

function computeEffectiveDb(category, volumePercent) {
    const busDb = volumeBusDb(category);
    if (busDb === null || volumePercent === null) return null;
    // Matches CDSAudioPlayer_ApplyEffectiveVolume:
    // ((busDb + 10000) * volumePercent) / 100 - 10000
    return Math.trunc(((busDb + 10000) * volumePercent) / 100) - 10000;
}

function playerSnapshot(player) {
    const p = ptr(player);
    const category = readS32(p, 0x4c);
    const volumePercent = readS32(p, 0x54);
    const expectedDb = computeEffectiveDb(category, volumePercent);
    const buffer = readPtr(p, 0x14);
    return {
        player: hex(p),
        source_0x0c: hex(readPtr(p, 0x0c)),
        dsBuffer_0x14: hex(buffer),
        streaming_0x1c: readU8(p, 0x1c),
        looping_0x1d: readU8(p, 0x1d),
        hasData_0x1e: readU8(p, 0x1e),
        playing_0x22: readU8(p, 0x22),
        category_0x4c: category,
        busDb_0x4b8370: volumeBusDb(category),
        cachedEffectiveDb_0x50: readS32(p, 0x50),
        volumePercent_0x54: volumePercent,
        expectedEffectiveDb: expectedDb,
        expectedLinearGain: dsHundredthDbToGain(expectedDb),
    };
}

function sampleSnapshot(sample) {
    const s = ptr(sample);
    if (s.isNull()) return null;
    return {
        ptr: hex(s),
        byteLen: readU32(s, 0x04),
        channels: readU16(s, 0x08),
        bits: readU16(s, 0x0a),
        sampleRate: readU32(s, 0x0c),
        avgBytesPerSec: readU32(s, 0x10),
    };
}

function rememberPlayer(player, patch) {
    const k = key(player);
    const meta = Object.assign(playerMeta.get(k) || {}, patch || {});
    playerMeta.set(k, meta);
    const buffer = readPtr(player, 0x14);
    if (!buffer.isNull()) bufferToPlayer.set(key(buffer), k);
    return meta;
}

function playerContext(player) {
    return playerMeta.get(key(player)) || {};
}

function hookDirectSoundSetVolumeForBuffer(buffer) {
    const b = ptr(buffer);
    if (b.isNull()) return;
    const vtbl = readPtr(b, 0);
    if (vtbl.isNull()) return;
    const fn = readPtr(vtbl, 0x3c); // IDirectSoundBuffer::SetVolume
    if (fn.isNull()) return;
    const fnKey = key(fn);
    if (hookedSetVolumeTargets.has(fnKey)) return;
    hookedSetVolumeTargets.add(fnKey);
    emit("DirectSound_SetVolume_hooked", {
        target: hex(fn),
        vtbl: hex(vtbl),
        exampleBuffer: hex(b),
    });
    Interceptor.attach(fn, {
        onEnter(args) {
            try {
                const bufferPtr = ptr(args[0]);
                const attenuationDb100 = args[1].toInt32();
                const playerKey = bufferToPlayer.get(key(bufferPtr));
                const ctx = playerKey ? (playerMeta.get(playerKey) || {}) : {};
                emit("DirectSound_SetVolume", {
                    dsBuffer: hex(bufferPtr),
                    attenuationDb100,
                    linearGain: dsHundredthDbToGain(attenuationDb100),
                    player: playerKey || null,
                    playerKind: ctx.kind || null,
                    resourceId: ctx.resourceId || null,
                    resourceName: ctx.resourceName || null,
                    bankSlot: ctx.slot === undefined ? null : ctx.slot,
                    bankKind: ctx.bankKind || null,
                });
            } catch (e) {
                emit("hook_error", { hook: "DirectSound_SetVolume.onEnter", error: String(e) });
            }
        },
    });
}

function attachThiscall(address, name, handlers) {
    Interceptor.attach(address, {
        onEnter(args) {
            this.self = ptr(this.context.ecx);
            try { if (handlers.onEnter) handlers.onEnter.call(this, args); }
            catch (e) { emit("hook_error", { hook: `${name}.onEnter`, error: String(e) }); }
        },
        onLeave(retval) {
            try { if (handlers.onLeave) handlers.onLeave.call(this, retval); }
            catch (e) { emit("hook_error", { hook: `${name}.onLeave`, error: String(e) }); }
        },
    });
}

function attachFastcallThis(address, name, callback) {
    Interceptor.attach(address, {
        onEnter() {
            try { callback.call(this, ptr(this.context.ecx)); }
            catch (e) { emit("hook_error", { hook: `${name}.onEnter`, error: String(e) }); }
        },
    });
}

Process.setExceptionHandler(function (details) {
    try {
        const ctx = details.context;
        emit("EXCEPTION", {
            type: details.type,
            message: details.message,
            memoryOperation: details.memory ? details.memory.operation : null,
            memoryAddress: details.memory ? hex(details.memory.address) : null,
            address: hex(details.address),
            eax: hex(ctx.eax), ebx: hex(ctx.ebx), ecx: hex(ctx.ecx), edx: hex(ctx.edx),
            esi: hex(ctx.esi), edi: hex(ctx.edi), ebp: hex(ctx.ebp), esp: hex(ctx.esp),
            eip: hex(ctx.eip),
        });
    } catch (e) {}
    return false;
});

attachThiscall(ADDR.CMenu_LoadBackgroundMusic, "CMenu_LoadBackgroundMusic", {
    onEnter(args) {
        const resourceId = args[0].toUInt32();
        pendingCreate = {
            kind: "menu_bg",
            resourceId,
            resourceName: RESOURCE_NAMES[resourceId] || null,
        };
        emit("CMenu_LoadBackgroundMusic.enter", {
            menu: hex(this.self),
            resourceId,
            resourceName: RESOURCE_NAMES[resourceId] || null,
            oldPlayer_0xc0: hex(readPtr(this.self, 0xc0)),
        });
    },
    onLeave() {
        emit("CMenu_LoadBackgroundMusic.leave", {
            menu: hex(this.self),
            player_0xc0: hex(readPtr(this.self, 0xc0)),
            player: playerSnapshot(readPtr(this.self, 0xc0)),
        });
        pendingCreate = null;
    },
});

attachThiscall(ADDR.CMenu_SetDayNightBg, "CMenu_SetDayNightBg", {
    onEnter(args) {
        const isNight = args[0].toInt32() & 0xff;
        emit("CMenu_SetDayNightBg", {
            menu: hex(this.self),
            isNight,
            expectedResourceId: isNight ? 0x1014a : 0x10149,
            expectedResourceName: isNight ? "menu_night_ambient" : "menu_day_ambient",
        });
    },
});

attachThiscall(ADDR.CMenu_EnableBackgroundState, "CMenu_EnableBackgroundState", {
    onEnter(args) {
        emit("CMenu_EnableBackgroundState", {
            menu: hex(this.self),
            enable: args[0].toInt32() & 0xff,
            player_0xc0: hex(readPtr(this.self, 0xc0)),
            active_0xe2_before: readU8(this.self, 0xe2),
        });
    },
});

attachFastcallThis(ADDR.CMenu_OnMusicFadeTick, "CMenu_OnMusicFadeTick", function (slotOrMenuPart) {
    const player = readPtr(slotOrMenuPart, 0x58);
    emit("CMenu_OnMusicFadeTick.enter", {
        slotOrMenuPart: hex(slotOrMenuPart),
        fadeInFlag_0x7a: readU8(slotOrMenuPart, 0x7a),
        player: playerSnapshot(player),
    });
});

Interceptor.attach(ADDR.CDSAudioPlayer_CreateFromResource, {
    onEnter(args) {
        try {
            this.ownsPendingCreate = pendingCreate === null;
            this.category = args[0].toInt32();
            this.resourceId = args[1].toUInt32();
            this.param3 = args[2].toInt32();
            this.param4 = args[3].toUInt32();
            this.loopFlag = args[4].toInt32() & 0xff;
            pendingCreate = pendingCreate || {
                kind: "resource",
                resourceId: this.resourceId,
                resourceName: RESOURCE_NAMES[this.resourceId] || null,
            };
            emit("CDSAudioPlayer_CreateFromResource.enter", {
                category: this.category,
                resourceId: this.resourceId,
                resourceName: RESOURCE_NAMES[this.resourceId] || null,
                param3: this.param3,
                param4: this.param4,
                loopFlag: this.loopFlag,
                pendingKind: pendingCreate.kind,
            });
        } catch (e) {
            emit("hook_error", { hook: "CDSAudioPlayer_CreateFromResource.onEnter", error: String(e) });
        }
    },
    onLeave(retval) {
        try {
            const player = ptr(retval);
            if (!player.isNull()) {
                rememberPlayer(player, Object.assign({}, pendingCreate || {}, {
                    createCategory: this.category,
                    createLoopFlag: this.loopFlag,
                }));
            }
            emit("CDSAudioPlayer_CreateFromResource.leave", {
                player: hex(player),
                playerState: player.isNull() ? null : playerSnapshot(player),
                meta: player.isNull() ? null : playerContext(player),
            });
        } catch (e) {
            emit("hook_error", { hook: "CDSAudioPlayer_CreateFromResource.onLeave", error: String(e) });
        } finally {
            if (this.ownsPendingCreate) pendingCreate = null;
        }
    },
});

Interceptor.attach(ADDR.TriggerBankSample, {
    onEnter(args) {
        try {
            const slot = args[2].toInt32();
            const bankKind = MENU_BANK_SLOTS[slot] || null;
            this.trace = bankKind !== null;
            if (!this.trace) return;
            pendingBankSlot = {
                kind: "bank_sfx",
                slot,
                bankKind,
                flags: args[0].toInt32(),
                looping: args[5].toInt32() & 0xff,
            };
            emit("TriggerBankSample.enter", {
                bankKind,
                slot,
                flags: args[0].toInt32(),
                bank: hex(args[1]),
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
        if (this.trace) pendingBankSlot = null;
    },
});

attachThiscall(ADDR.CDSAudioPlayer_Init, "CDSAudioPlayer_Init", {
    onEnter(args) {
        this.sample = args[0];
        this.flags = args[1].toUInt32();
        this.eventTarget = ptr(args[2]);
        this.categoryArg = args[3].toInt32();
        const meta = pendingBankSlot || pendingCreate || { kind: "unknown" };
        rememberPlayer(this.self, Object.assign({}, meta, {
            initFlags: this.flags,
            initCategoryArg: this.categoryArg,
        }));
        emit("CDSAudioPlayer_Init.enter", {
            player: hex(this.self),
            meta: playerContext(this.self),
            sample: sampleSnapshot(this.sample),
            flags: this.flags,
            eventTarget: hex(this.eventTarget),
            categoryArg: this.categoryArg,
        });
    },
    onLeave() {
        rememberPlayer(this.self, {});
        hookDirectSoundSetVolumeForBuffer(readPtr(this.self, 0x14));
        emit("CDSAudioPlayer_Init.leave", {
            player: playerSnapshot(this.self),
            meta: playerContext(this.self),
        });
    },
});

attachThiscall(ADDR.CDSAudioPlayer_SetVolumePercent, "CDSAudioPlayer_SetVolumePercent", {
    onEnter(args) {
        this.percent = args[0].toInt32();
        emit("CDSAudioPlayer_SetVolumePercent.enter", {
            requestedPercent: this.percent,
            before: playerSnapshot(this.self),
            meta: playerContext(this.self),
        });
    },
    onLeave() {
        emit("CDSAudioPlayer_SetVolumePercent.leave", {
            requestedPercent: this.percent,
            after: playerSnapshot(this.self),
            meta: playerContext(this.self),
        });
    },
});

attachThiscall(ADDR.CDSAudioPlayer_ApplyEffectiveVolume, "CDSAudioPlayer_ApplyEffectiveVolume", {
    onEnter(args) {
        this.force = args[0].toInt32() & 0xff;
        hookDirectSoundSetVolumeForBuffer(readPtr(this.self, 0x14));
        emit("CDSAudioPlayer_ApplyEffectiveVolume.enter", {
            force: this.force,
            before: playerSnapshot(this.self),
            meta: playerContext(this.self),
        });
    },
    onLeave() {
        emit("CDSAudioPlayer_ApplyEffectiveVolume.leave", {
            force: this.force,
            after: playerSnapshot(this.self),
            meta: playerContext(this.self),
        });
    },
});

attachThiscall(ADDR.CDSAudioPlayer_Play, "CDSAudioPlayer_Play", {
    onEnter(args) {
        emit("CDSAudioPlayer_Play.enter", {
            loopArg: args[0].toInt32() & 0xff,
            before: playerSnapshot(this.self),
            meta: playerContext(this.self),
        });
    },
    onLeave() {
        emit("CDSAudioPlayer_Play.leave", {
            after: playerSnapshot(this.self),
            meta: playerContext(this.self),
        });
    },
});

Interceptor.attach(ADDR.CDSAudioPlayer_PlayAndRelease, {
    onEnter(args) {
        try {
            this.player = ptr(args[0]);
            emit("CDSAudioPlayer_PlayAndRelease.enter", {
                loopArg: args[1].toInt32() & 0xff,
                player: playerSnapshot(this.player),
                meta: playerContext(this.player),
            });
        } catch (e) {
            emit("hook_error", { hook: "CDSAudioPlayer_PlayAndRelease.onEnter", error: String(e) });
        }
    },
});

attachThiscall(ADDR.CDSAudioPlayer_Stop, "CDSAudioPlayer_Stop", {
    onEnter(args) {
        emit("CDSAudioPlayer_Stop.enter", {
            preserveFlag: args[0].toInt32() & 0xff,
            before: playerSnapshot(this.self),
            meta: playerContext(this.self),
        });
    },
    onLeave() {
        emit("CDSAudioPlayer_Stop.leave", {
            after: playerSnapshot(this.self),
            meta: playerContext(this.self),
        });
    },
});

emit("trace_start", {
    module: MOD.name,
    moduleBase: hex(MODULE_BASE),
    slide: SLIDE.toInt32(),
    note: "Main-menu CDSAudioPlayer volume/fade/DirectSound SetVolume trace.",
    addresses: Object.keys(ADDR).reduce((out, k) => {
        out[k] = hex(ADDR[k]);
        return out;
    }, {}),
});

console.error(`[+] menu_audio_mixer_trace.js loaded: ${MOD.name} @ ${MODULE_BASE} (slide=${SLIDE.toInt32()})`);
console.error("[+] Audio mixer hooks installed - waiting for events");
