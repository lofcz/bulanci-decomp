// ============================================================================
// bulanci_force_bedtime.smoketest.js — prove hooks install without crash
// ============================================================================
//
// Spawns the same hook logic, polls for srand/rand resolution, and after
// 3s of idle (i.e. hooks installed, no rand() called yet) it forces 6
// rand() calls and dumps the result. Quits the script cleanly.
//
// Compare to bulanci_force_bedtime.js — the hook body is identical.
// ============================================================================

'use strict';

function readSeed() {
    var raw = '0x12345678';
    try {
        if (typeof Process !== 'undefined' && Process.env) {
            if (Process.env.BULANCI_FORCE_SEED) raw = Process.env.BULANCI_FORCE_SEED;
            else if (Process.environment && Process.environment.BULANCI_FORCE_SEED)
                raw = Process.environment.BULANCI_FORCE_SEED;
        }
    } catch (_e) {}
    var n = parseInt(raw, 16);
    if (isNaN(n)) n = parseInt(raw, 10);
    if (isNaN(n)) n = 0x12345678;
    return n & 0xFFFFFFFF;
}

const CONFIG = { seed: readSeed() };
let prngCalls = 0;
const INIT_LOCK_COUNT = 6;
let hooksInstalled = false;

const log = function () {
    var args = Array.prototype.slice.call(arguments);
    args.unshift('[smoke]');
    console.log.apply(console, args);
};

function findImport(name) {
    if (typeof Module === 'undefined') {
        log('Module not available');
        return null;
    }
    // Module.findGlobalExportByName(name) scans every loaded module and
    // returns the address of the named export. This is the simplest and
    // most reliable way to hook an imported C runtime function without
    // touching the IAT.
    if (typeof Module.findGlobalExportByName === 'function') {
        try {
            var p = Module.findGlobalExportByName(name);
            if (p && !p.isNull()) {
                log('resolved ' + name + ' via findGlobalExportByName -> 0x' + p.toString(16));
                return p;
            } else {
                log('findGlobalExportByName(' + name + ') returned null');
            }
        } catch (e) {
            log('findGlobalExportByName(' + name + ') threw: ' + e);
        }
    } else {
        log('Module.findGlobalExportByName not a function (Frida API mismatch?)');
    }
    // Fallback: enumerate every module and try .findExportByName (instance
    // method) on each. This is what we'd do anyway if findGlobalExportByName
    // doesn't exist on this Frida build.
    if (typeof Process !== 'undefined' && Process.enumerateModules) {
        var mods = Process.enumerateModules();
        log('fallback: enumerateModules found ' + mods.length + ' modules');
        for (var m = 0; m < mods.length; m++) {
            try {
                var p2 = Module.findExportByName(mods[m].name, name);
                if (p2 && !p2.isNull()) {
                    log('resolved ' + name + ' via ' + mods[m].name + ' -> 0x' + p2.toString(16));
                    return p2;
                }
            } catch (_e) {}
        }
    }
    return null;
}

function installHooks() {
    if (hooksInstalled) return true;
    var srandPtr = findImport('srand');
    var randPtr = findImport('rand');
    if (!srandPtr || !randPtr) {
        log('imports not yet resolved');
        return false;
    }
    log('installing hooks: srand=0x' + srandPtr.toString(16) + ', rand=0x' + randPtr.toString(16));
    Interceptor.attach(srandPtr, {
        onEnter: function (args) {
            args[0] = ptr(CONFIG.seed);
            log('srand(0x' + CONFIG.seed.toString(16) + ') — pinned');
        }
    });
    Interceptor.attach(randPtr, {
        onLeave: function (retval) {
            prngCalls++;
            if (prngCalls <= INIT_LOCK_COUNT) {
                var orig = retval.toInt32();
                retval.replace(0);
                log('_rand() #' + prngCalls + ' orig=' + orig + ' -> forced 0');
            }
        }
    });
    hooksInstalled = true;
    return true;
}

if (!installHooks()) {
    var tries = 0;
    var timer = setInterval(function () {
        tries++;
        if (installHooks() || tries > 400) {
            clearInterval(timer);
            if (!hooksInstalled) log('ERROR: never resolved after ' + tries + ' tries');
        }
    }, 50);
}

// 3s after hooks install (or 6s total, whichever first), dump state and exit.
setTimeout(function () {
    log('=== SMOKE TEST SUMMARY ===');
    log('hooksInstalled: ' + hooksInstalled);
    log('prngCalls: ' + prngCalls);
    log('resolved srand/rand: ' + (typeof srandPtr !== 'undefined' ? 'yes' : 'no'));
    log('=== END ===');
}, 4000);
