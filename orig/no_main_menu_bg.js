/* eslint-disable */
"use strict";

(function () {
"use strict";

// Mute only the retail main-menu background music buffers.
//
// This deliberately leaves CDSAudioPlayer_SetVolumePercent and the menu fade
// scheduler untouched: retail still computes and stores the real 70..100/100..70
// percent curve, while WASAPI receives silence for the ambient bed. Menu SFX,
// hover barks, and exit voice cues remain audible.

console.log("[+] no_main_menu_bg.js loaded");

const IMAGE_BASE_PE = 0x00400000;

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
    console.log("[-] Could not locate Bulanci module - aborting main-menu BG mute");
    throw new Error("Bulanci module not found");
}

const SLIDE = MOD.base.sub(IMAGE_BASE_PE);
function va(addr) { return ptr(addr).add(SLIDE); }
function hex(p) { try { return ptr(p).toString(); } catch (e) { return String(p); } }
function key(p) { return hex(p); }
function readPtr(p, offset) {
    try { return ptr(p).add(offset).readPointer(); } catch (e) { return ptr(0); }
}

const ADDR = {
    CDSAudioPlayer_CreateFromResource: va(0x00422550),
    CDSAudioPlayer_Play: va(0x0043a9d0),
    CDSAudioPlayer_SetVolumePercent: va(0x0043a0d0),
    CDSAudioPlayer_ApplyEffectiveVolume: va(0x0043a060),
};

const MENU_BG_RESOURCES = {
    0x10149: "menu_day_ambient",
    0x1014a: "menu_night_ambient",
};

const DSBVOLUME_MIN = -10000;
const mutedPlayers = new Map();
const mutedBuffers = new Set();
const hookedSetVolumeTargets = new Set();
const nativeSetVolumeByTarget = new Map();

function isMenuBgResource(resourceId) {
    return Object.prototype.hasOwnProperty.call(MENU_BG_RESOURCES, resourceId);
}

function hookDirectSoundSetVolumeForBuffer(buffer) {
    const b = ptr(buffer);
    if (b.isNull()) return;

    const vtbl = readPtr(b, 0);
    if (vtbl.isNull()) return;

    // IDirectSoundBuffer::SetVolume(LONG lVolume) is vtable slot 15.
    const setVolume = readPtr(vtbl, 0x3c);
    if (setVolume.isNull()) return;

    const fnKey = key(setVolume);
    if (!nativeSetVolumeByTarget.has(fnKey)) {
        nativeSetVolumeByTarget.set(
            fnKey,
            new NativeFunction(setVolume, "int", ["pointer", "int"], "stdcall")
        );
    }

    if (hookedSetVolumeTargets.has(fnKey)) return;
    hookedSetVolumeTargets.add(fnKey);

    Interceptor.attach(setVolume, {
        onEnter(args) {
            const bufferPtr = ptr(args[0]);
            if (!mutedBuffers.has(key(bufferPtr))) return;

            const requested = args[1].toInt32();
            if (requested !== DSBVOLUME_MIN) {
                args[1] = ptr(DSBVOLUME_MIN);
                console.log(`[no-main-menu-bg] DirectSound_SetVolume ${hex(bufferPtr)} ${requested} -> ${DSBVOLUME_MIN}`);
            }
        },
    });

    console.log(`[+] Hooked IDirectSoundBuffer::SetVolume @ ${setVolume}`);
}

function forceMuteBuffer(buffer, why) {
    const b = ptr(buffer);
    if (b.isNull()) return false;

    mutedBuffers.add(key(b));
    hookDirectSoundSetVolumeForBuffer(b);

    const vtbl = readPtr(b, 0);
    const setVolume = vtbl.isNull() ? ptr(0) : readPtr(vtbl, 0x3c);
    const fn = setVolume.isNull() ? null : nativeSetVolumeByTarget.get(key(setVolume));
    if (fn) {
        try {
            fn(b, DSBVOLUME_MIN);
            console.log(`[no-main-menu-bg] forced ${why}: buffer=${b} volume=${DSBVOLUME_MIN}`);
        } catch (e) {
            console.log(`[no-main-menu-bg] force mute failed (${why}) buffer=${b}: ${e.message}`);
        }
    }
    return true;
}

function forceMutePlayer(player, why) {
    const p = ptr(player);
    if (p.isNull()) return false;
    const meta = mutedPlayers.get(key(p));
    if (!meta) return false;

    const buffer = readPtr(p, 0x14);
    if (buffer.isNull()) {
        console.log(`[no-main-menu-bg] ${meta.name} player=${p} still has no DS buffer at ${why}`);
        return false;
    }

    return forceMuteBuffer(buffer, `${why}/${meta.name}`);
}

function attachThiscall(address, name, handlers) {
    Interceptor.attach(address, {
        onEnter(args) {
            this.self = ptr(this.context.ecx);
            try { if (handlers.onEnter) handlers.onEnter.call(this, args); }
            catch (e) { console.log(`[no-main-menu-bg] ${name}.onEnter error: ${e.message}`); }
        },
        onLeave(retval) {
            try { if (handlers.onLeave) handlers.onLeave.call(this, retval); }
            catch (e) { console.log(`[no-main-menu-bg] ${name}.onLeave error: ${e.message}`); }
        },
    });
}

Interceptor.attach(ADDR.CDSAudioPlayer_CreateFromResource, {
    onEnter(args) {
        this.category = args[0].toInt32();
        this.resourceId = args[1].toUInt32();
        this.isMenuBg = this.category === 2 && isMenuBgResource(this.resourceId);
    },
    onLeave(retval) {
        if (!this.isMenuBg) return;

        const player = ptr(retval);
        if (player.isNull()) return;

        mutedPlayers.set(key(player), {
            resourceId: this.resourceId,
            name: MENU_BG_RESOURCES[this.resourceId],
        });
        console.log(`[no-main-menu-bg] tracking ${MENU_BG_RESOURCES[this.resourceId]} resource=0x${this.resourceId.toString(16)} player=${player}`);
        forceMutePlayer(player, "CreateFromResource.leave");
    },
});

attachThiscall(ADDR.CDSAudioPlayer_Play, "CDSAudioPlayer_Play", {
    onEnter() {
        forceMutePlayer(this.self, "Play.enter");
    },
    onLeave() {
        forceMutePlayer(this.self, "Play.leave");
    },
});

attachThiscall(ADDR.CDSAudioPlayer_SetVolumePercent, "CDSAudioPlayer_SetVolumePercent", {
    onEnter() {
        forceMutePlayer(this.self, "SetVolumePercent.enter");
    },
    onLeave() {
        forceMutePlayer(this.self, "SetVolumePercent.leave");
    },
});

attachThiscall(ADDR.CDSAudioPlayer_ApplyEffectiveVolume, "CDSAudioPlayer_ApplyEffectiveVolume", {
    onEnter() {
        forceMutePlayer(this.self, "ApplyEffectiveVolume.enter");
    },
    onLeave() {
        forceMutePlayer(this.self, "ApplyEffectiveVolume.leave");
    },
});

console.log("[+] Main-menu background mute hooks installed");
})();
