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
    if (typeof Module === 'undefined' || !Module.findExportByName) {
        log('Module.findExportByName not available');
        return null;
    }
    var candidates = [
        'msvcrt', 'msvcr80', 'msvcr90', 'msvcr100', 'msvcr110', 'msvcr120',
        'ucrtbase', 'MSVCRT', 'msvcrt.dll',
    ];
    for (var i = 0; i < candidates.length; i++) {
        try {
            var p = Module.findExportByName(candidates[i], name);
            if (p && !p.isNull()) {
                log('resolved ' + name + ' via ' + candidates[i] + ' -> 0x' + p.toString(16));
                return p;
            }
        } catch (_e) {}
    }
    if (typeof Process !== 'undefined' && Process.enumerateModules) {
        var mods = Process.enumerateModules();
        log('enumerateModules found ' + mods.length + ' modules:');
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
