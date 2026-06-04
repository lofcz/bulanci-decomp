// ============================================================================
// bulanci_force_bedtime.js — Force any of the 32 layouts on "Na dobrou noc"
// ============================================================================
//
// Usage:
//   frida -l bulanci_force_bedtime.js -f bulanci.exe
//   (or attach: frida -l bulanci_force_bedtime.js -p <pid>)
//
// What it does:
//   Resolves the static srand/rand inside bulanci.exe (statically linked,
//   not the IAT) by reading the 5-byte x86 relative CALL at known sites,
//   installs Interceptor hooks on those function entries, and forces the
//   first 5 rand() returns (the only 5 consumed during the level's
//   OnInit) to match a user-configured layout.
//
//   After the 5 init-phase calls, rand() is left alone so per-event
//   rolls (anim frames, mimic delay, mimic side, trace-area trigger
//   rolls) still feel random.
//
// Configuration:
//   The script reads a one-line text file `bulanci_force_bedtime.cfg`
//   from the same directory as the .js (or, on failure, from CWD). The
//   line is "A1111" / "A0000" / "B1010" etc.:
//
//     <variant><bunny><mouse><bird><bush>
//     variant: 'A' or 'B'
//     bits:    '1' = force present, '0' = force absent
//
//   The butterfly is ALWAYS present (no roll in fn@0x1e7), so the
//   layout spec is 1+4 = 5 chars.
//
//   If the config file is missing, the default A1111 is used. To
//   change the layout, edit the file or have the .cmd launcher write
//   it. (Frida 17.10.1 has no Process.env in the JS API, so env vars
//   don't reach the script — config file is the reliable path.)
//
//   You can also hardcode CONFIG.layout at the top of this file.
//
// Level init's PRNG budget (the only 5 forced calls), in order:
//   1. variant roll: Rand(0,1)        -> 0=A, 1=B
//   2. bunny presence: Rand(0,7)==0   -> absent  (else present)
//   3. mouse presence: Rand(0,9)==0   -> absent
//   4. bird presence:  Rand(0,7)==0   -> absent
//   5. bush presence:  Rand(0,6)==0   -> absent
// (The butterfly is unconditional; fn@0x1e7 has no Rand call.)
//
// Counter reset signals (both reset prngCalls -> 0 and re-arm forcing):
//   - srand call (the EXE reseeds at level construction; we resolve its
//     address via the 5-byte CALL at 0x402b3c inside CBulanci_OnCreate).
//   - CBulanci_OnCreate entry (0x402b20). Catches the case where the
//     level is constructed without an srand re-seed (e.g. only the
//     process-start srand was called, and the next 6 startup Rands
//     already exhausted the 5-call budget before the level is picked).
//
// Known call sites (RVA from image base 0x00400000):
//   - srand            0x402b3c  (e8 ?? ?? ?? ??)  -> static srand inside EXE
//   - rand             0x438cc8  (e8 ?? ?? ?? ??)  -> static rand  inside EXE
//   - CBulanci_OnCreate 0x402b20  (function entry, hook by absolute ptr)
//
// Per-entity geometry (hardcoded in OnInit, NOT affected by the PRNG):
//   Variant A:
//     butterfly (417,113)  bunny (568,360)  mouse (227,103, danger_w=800)
//     bird (154,306)       bush (61,127)
//   Variant B:
//     butterfly (309,50)   bunny (195,364)  mouse (124,283, danger_w=580)
//     bird (576,245)       bush (67,85)
// ============================================================================

'use strict';

// ---- config ----
const DEFAULT_LAYOUT = 'A1111';
const DEFAULT_SEED = 0x12345678;

function parseLayout(raw) {
    raw = String(raw).trim().toUpperCase();
    if (raw.length < 5) return null;
    if (raw[0] !== 'A' && raw[0] !== 'B') return null;
    var bits = raw.substring(1, 5).split('');
    for (var i = 0; i < 4; i++) {
        if (bits[i] !== '0' && bits[i] !== '1') return null;
        bits[i] = parseInt(bits[i], 10);
    }
    return {
        variant: raw[0] === 'B' ? 1 : 0,  // 0=A, 1=B
        bunny:   bits[0],
        mouse:   bits[1],
        bird:    bits[2],
        bush:    bits[3],
    };
}

// Try a few candidate paths for the config file. Frida exposes file IO
// via `new File()`.
function readConfigFile() {
    var candidates = [
        'bulanci_force_bedtime.cfg',  // cwd
        // scriptDir is approximated by trying the .js path next to known
        // process cwd; on most user setups, cwd is the bulanci repo root
        // or the scripts/frida dir, so the bare name resolves either way.
    ];
    for (var i = 0; i < candidates.length; i++) {
        try {
            var f = new File(candidates[i], 'r');
            var line = f.read(64);
            f.close();
            if (line) return line;
        } catch (_e) { /* try next */ }
    }
    return null;
}

const CONFIG = {
    seed:   DEFAULT_SEED,
    layout: parseLayout(DEFAULT_LAYOUT),
};

const log = function () {
    var args = Array.prototype.slice.call(arguments);
    args.unshift('[bedtime]');
    console.log.apply(console, args);
};

var _cfgRaw = readConfigFile();
if (_cfgRaw) {
    var _parsed = parseLayout(_cfgRaw);
    if (_parsed) {
        CONFIG.layout = _parsed;
        log('loaded layout from bulanci_force_bedtime.cfg: ' + _cfgRaw.trim());
    } else {
        log('WARN: bulanci_force_bedtime.cfg contents not a valid layout (' +
            JSON.stringify(_cfgRaw) + '), using default ' + DEFAULT_LAYOUT);
    }
} else {
    log('no bulanci_force_bedtime.cfg found, using default ' + DEFAULT_LAYOUT);
}

log('config: seed=0x' + CONFIG.seed.toString(16) +
    ' layout=' + (CONFIG.layout.variant ? 'B' : 'A') +
                CONFIG.layout.bunny +
                CONFIG.layout.mouse +
                CONFIG.layout.bird +
                CONFIG.layout.bush);

// prngCalls counts every rand() seen since the last reset. The first 5
// calls after a reset are forced; the 6th+ pass through to the real PRNG
// so per-event Rands (anim frames, mimic delay) stay random.
let prngCalls = 0;
const INIT_BUDGET = 5;
let hooksInstalled = false;

function resetCounter() {
    prngCalls = 0;
}

// For the i-th forced call (0-indexed), return the value the script wants.
// Layout's "1" = present, "0" = absent. Presence-compare is "==0 then absent",
// so for present we return 1 (any non-zero would do, but 1 is clean).
function forcedValueFor(i) {
    const L = CONFIG.layout;
    if (i === 0) return L.variant;     // 0 = A, 1 = B
    if (i === 1) return L.bunny ? 1 : 0;
    if (i === 2) return L.mouse ? 1 : 0;
    if (i === 3) return L.bird  ? 1 : 0;
    if (i === 4) return L.bush  ? 1 : 0;
    return -1; // shouldn't happen
}

// ---- call-site resolution ----
// Read a 5-byte x86 relative CALL at absolute VA `absVA` inside `mod`
// and return the absolute target address. Throws if not a CALL.
function resolveCallTarget(mod, absVA) {
    const imageBase = ptr('0x00400000');
    const rva = absVA - imageBase.toInt32();
    const ip = mod.base.add(rva);
    const b0 = ip.readU8();
    if (b0 !== 0xE8) {
        throw new Error('expected 0xE8 (CALL) at VA 0x' + absVA.toString(16) +
                        ' (RVA 0x' + rva.toString(16) + '), got 0x' + b0.toString(16));
    }
    const rel = ip.add(1).readS32();
    return ip.add(5).add(rel);
}

function installHooks() {
    if (hooksInstalled) return true;

    const mod = Process.findModuleByName('bulanci.exe');
    if (!mod) {
        log('bulanci.exe module not loaded yet — will retry');
        return false;
    }

    let srandPtr, randPtr, cBulanciOnCreate;
    try { srandPtr = resolveCallTarget(mod, 0x00402b3c); }
    catch (e) { log('failed to resolve srand call site: ' + e); return false; }
    try { randPtr  = resolveCallTarget(mod, 0x00438cc8); }
    catch (e) { log('failed to resolve rand call site: ' + e); return false; }
    cBulanciOnCreate = mod.base.add(0x0002b20);  // RVA = 0x402b20 - 0x400000

    log('bulanci.exe base=0x' + mod.base.toString(16));
    log('  srand call target:           0x' + srandPtr.toString(16));
    log('  rand  call target:           0x' + randPtr.toString(16));
    log('  CBulanci_OnCreate entry:     0x' + cBulanciOnCreate.toString(16));

    // Hook srand: pin seed + reset budget.
    Interceptor.attach(srandPtr, {
        onEnter: function (args) {
            args[0] = ptr(CONFIG.seed);
            resetCounter();
            log('srand(0x' + CONFIG.seed.toString(16) + ') — pinned; budget reset');
        }
    });

    // Hook CBulanci_OnCreate: reset budget (catches the case where srand
    // isn't re-called at level init and the process-startup 6 Rands
    // already exhausted the 5-call budget).
    Interceptor.attach(cBulanciOnCreate, {
        onEnter: function (args) {
            resetCounter();
            log('CBulanci_OnCreate entered — budget reset');
        }
    });

    // Hook rand: force the first 5 calls, then pass through.
    Interceptor.attach(randPtr, {
        onLeave: function (retval) {
            prngCalls++;
            if (prngCalls <= INIT_BUDGET) {
                const forced = forcedValueFor(prngCalls - 1);
                const orig = retval.toInt32();
                retval.replace(forced);
                log('_rand() #' + prngCalls + ' orig=' + orig + ' -> forced ' + forced +
                    ' (budget ' + prngCalls + '/' + INIT_BUDGET + ')');
            }
            // prngCalls > INIT_BUDGET -> pass through (real PRNG result)
        }
    });

    hooksInstalled = true;
    log('hooks installed — pick "Na dobrou noc" in the menu.');
    log('level init will consume exactly 5 rand() calls (variant + 4 entity presence).');
    log('butterfly is always present (no roll).');
    return true;
}

var tries = 0;
var timer = setInterval(function () {
    tries++;
    if (installHooks() || tries > 100) {
        clearInterval(timer);
        if (!hooksInstalled) {
            log('ERROR: failed to install hooks after ' + tries + ' tries');
        }
    }
}, 50);

