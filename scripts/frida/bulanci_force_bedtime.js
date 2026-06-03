// ============================================================================
// bulanci_force_bedtime.js — Force "Na dobrou noc" / "Bedtime story" layout
// ============================================================================
//
// Usage:
//   frida -l bulanci_force_bedtime.js -f bulanci.exe --no-pause
//   (or attach: frida -l bulanci_force_bedtime.js -p <pid>)
//
// What it does:
//   1. Pins the PRNG seed via hook on srand. Same seed -> same layout.
//   2. Hooks rand and forces the first 6 calls to return 0:
//        - OnInit:variant = 0  (Variant A, right-heavy furniture layout)
//        - fn_0x01e7 (butterfly):  1/8  -> 0 -> present
//        - fn_0x0286 (bunny):      1/10 -> 0 -> present
//        - fn_0x0394 (gun-mouse):  1/8  -> 0 -> present
//        - fn_0x04e1 (bird):       1/7  -> 0 -> present
//        - fn_0x05c9 (bush):       1/6  -> 0 -> present
//   3. After the 6 init-phase calls, rand is left alone so per-click
//      anim frames and the mimic delay still feel random.
//
// Strategy (current attempt):
//   Use Frida's Module.findExportByName to locate srand / rand in the
//   actual loaded MSVCRT (msvcrt, msvcrXX, ucrtbase) — no IAT-slot
//   arithmetic, no base.add(), no readPointer. If findExportByName
//   returns null, log it loudly and keep polling.
//
// Ghidra anchors (bulanci.exe, image base 0x00400000):
//   - srand is called from CBulanci_OnCreate (0x402b3c) and CGaming_ctor.
//   - rand  is called from CDSScript_Rand                 (0x438cc8).
//   - the IAT slot offsets ghidra shows (0x4477df / 0x4477ec) are NOT
//     used by this script — they're data pointers, not code, and
//     Interceptor.attach on them access-violates.
// ============================================================================

'use strict';

// ---- config ----
function readSeed() {
    var raw = '0x12345678';
    try {
        if (typeof Process !== 'undefined' && Process.env) {
            if (Process.env.BULANCI_FORCE_SEED) {
                raw = Process.env.BULANCI_FORCE_SEED;
            } else if (Process.environment && Process.environment.BULANCI_FORCE_SEED) {
                raw = Process.environment.BULANCI_FORCE_SEED;
            }
        }
    } catch (_e) { /* env read best-effort */ }
    var n = parseInt(raw, 16);
    if (isNaN(n)) n = parseInt(raw, 10);
    if (isNaN(n)) n = 0x12345678;
    return n & 0xFFFFFFFF;
}

const CONFIG = {
    seed: readSeed(),
    verbose: true,
};

let prngCalls = 0;
const INIT_LOCK_COUNT = 6;
let hooksInstalled = false;

const log = function () {
    if (!CONFIG.verbose) return;
    var args = Array.prototype.slice.call(arguments);
    args.unshift('[bedtime]');
    console.log.apply(console, args);
};

// ---- import discovery ----
// In Frida 17, Module.findExportByName(name) searches every loaded module
// for the named export and returns the function address. This is the
// idiomatic way to hook an imported function without touching the IAT.
function findImport(name) {
    if (typeof Module === 'undefined' || !Module.findExportByName) {
        log('Module.findExportByName not available; aborting');
        return null;
    }
    // Prefer the common Windows MSVCRT variants. msvcrt is the legacy
    // 32-bit; msvcr70/80/90/100/110/120 are VC redist; ucrtbase is the
    // modern Universal CRT.
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
        } catch (_e) { /* module not loaded; try next */ }
    }
    // Last-resort: scan every module's exports ourselves. findExportByName
    // is supposed to do this, but if a name lookup returns null because
    // the symbol isn't yet enumerated, walking the load list directly is
    // a useful fallback.
    if (typeof Process !== 'undefined' && Process.enumerateModules) {
        var mods = Process.enumerateModules();
        for (var m = 0; m < mods.length; m++) {
            try {
                var p2 = Module.findExportByName(mods[m].name, name);
                if (p2 && !p2.isNull()) {
                    log('resolved ' + name + ' via ' + mods[m].name + ' -> 0x' + p2.toString(16));
                    return p2;
                }
            } catch (_e) { /* skip */ }
        }
    }
    return null;
}

function installHooks() {
    if (hooksInstalled) return true;

    var srandPtr = findImport('srand');
    var randPtr  = findImport('rand');
    if (!srandPtr || !randPtr) {
        log('imports not yet resolved (srand=' + (srandPtr ? '0x' + srandPtr.toString(16) : 'null') +
            ', rand=' + (randPtr ? '0x' + randPtr.toString(16) : 'null') + ') — will retry');
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
                retval.replace(0);
                log('_rand() #' + prngCalls + ' -> forced 0 (locked ' + prngCalls + '/' + INIT_LOCK_COUNT + ')');
            }
        }
    });

    hooksInstalled = true;
    log('hooks installed — start the game, pick "Na dobrou noc", play.');
    return true;
}

// ---- bootstrap ----
if (!installHooks()) {
    log('imports not yet resolved — polling until srand/rand export resolves...');
    var tries = 0;
    var timer = setInterval(function () {
        tries++;
        if (installHooks() || tries > 400) {   // 20s of attempts at 50ms
            clearInterval(timer);
            if (!hooksInstalled) {
                log('ERROR: srand/rand never resolved after ' + tries + ' tries');
                log('  possible causes:');
                log('  - bulanci.exe imports a renamed MSVCRT (try `dump_table` in ghidra)');
                log('  - the EXE statically links srand/rand (then use a different strategy)');
                log('  - the function is called via thunks in the EXE itself');
            }
        }
    }, 50);
}
