// ============================================================================
// bulanci_force_bedtime_v2.js — deterministically choose the "Na dobrou noc" /
// "Bedtime story" layout by forcing the SCRIPT-level Rand *results*.
// ============================================================================
//
// Why a v2?
//   The original bulanci_force_bedtime.js hooks the low-level MSVCRT _rand()
//   (returns a raw 0..32767) and replaces its return with 0 or 1. But the
//   level script never uses the raw value directly — it runs it through the
//   `Rand(lo,hi)` opcode (CDSScript_Rand @ 0x438cb0):
//
//       result = lo + ((raw * (hi - lo + 1)) >> 15)        // floor, raw in 0..32767
//
//   Forcing raw=0 -> result=lo for every roll.
//   Forcing raw=1 -> (1*span)>>15 == 0 for every span<=32768 -> still lo.
//
//   So the old script collapsed EVERY roll to its minimum no matter what
//   layout string you passed: variant A, and bunny/mouse/bird/bush all
//   "== 0" -> ABSENT. That is exactly the "everything missing" layout you
//   always got, and "present"/"variant B" were simply unreachable.
//
//   v2 hooks CDSScript_Rand itself and replaces the *opcode result* (EAX),
//   so we can request any value in [lo,hi] precisely. Hooking the script
//   Rand (instead of raw _rand) also isolates us from engine randomness
//   (AI, anim frames, RandInRange @ 0x40ace0) entirely — those never go
//   through CDSScript_Rand.
//
// Verified bedtime OnInit PRNG budget (export #1, both net & non-net),
// in call order — these are the ONLY 5 script Rand calls during scene setup:
//   #0 variant  : Rand(0,1)        -> 0 = layout A, 1 = layout B   (picks the
//                                      whole A/B coordinate table; this is the
//                                      "placement a/b" — there is NO per-prop
//                                      position roll, just this one switch)
//   #1 bunny    : Rand(0,7) == 0   -> ABSENT, else present  (1/8 absent)
//   #2 mouse    : Rand(0,9) == 0   -> ABSENT, else present  (1/10 absent)
//   #3 bird     : Rand(0,7) == 0   -> ABSENT, else present  (1/8 absent)
//   #4 bush     : Rand(0,6) == 0   -> ABSENT, else present  (1/7 absent)
//   butterfly   : ALWAYS present (no roll).
//
//   (The per-function comments in res_..._Script.script.asm have their
//   probabilities rotated by one and are wrong; the opcodes above are the
//   ground truth, re-derived from the bytecode + CDSScript_Rand math.)
//
// Reset signal: every match start, CGaming_ctor @ 0x41ff90 calls the static
//   srand(*(ownerGame+0xd4)) and then immediately CallExport(script, 1) =
//   OnInit. So "first 5 CDSScript_Rand calls after an srand" == the 5 rolls
//   above. We reset the roll plan on srand entry.
//
// Config: same one-line file as v1, bulanci_force_bedtime.cfg, format
//   <variant><bunny><mouse><bird><bush>, e.g. "A1111" / "B0010".
//     variant: 'A' (result 0) or 'B' (result 1)
//     bits:    '1' = force present, '0' = force absent
//
// Static addresses (image base 0x00400000, single module bulanci.exe):
//   CDSScript_Rand          0x438cb0   (the Rand(lo,hi) opcode handler)
//   static srand            0x4477df   (reset trigger)
//   CGaming_ctor (optional) 0x41ff90   (extra reset belt-and-suspenders)
// ============================================================================

'use strict';

const IMAGE_BASE = 0x00400000;
const RVA_CDSSCRIPT_RAND = 0x438cb0 - IMAGE_BASE;
const RVA_SRAND          = 0x4477df - IMAGE_BASE;
const RVA_CGAMING_CTOR   = 0x41ff90 - IMAGE_BASE;

const DEFAULT_LAYOUT = 'A1111';

const log = function () {
    const a = Array.prototype.slice.call(arguments);
    a.unshift('[bedtime2]');
    console.log.apply(console, a);
};

function parseLayout(raw) {
    raw = String(raw).trim().toUpperCase();
    if (raw.length < 5) return null;
    if (raw[0] !== 'A' && raw[0] !== 'B') return null;
    const bits = raw.substring(1, 5).split('');
    for (let i = 0; i < 4; i++) {
        if (bits[i] !== '0' && bits[i] !== '1') return null;
        bits[i] = parseInt(bits[i], 10);
    }
    return { variant: raw[0] === 'B' ? 1 : 0, bunny: bits[0], mouse: bits[1], bird: bits[2], bush: bits[3] };
}

// Read the whole cfg as text (robust across Frida File API variants).
function readConfigText() {
    try {
        if (typeof File.readAllText === 'function') return File.readAllText('bulanci_force_bedtime.cfg');
    } catch (_e) { /* fall through */ }
    try {
        const f = new File('bulanci_force_bedtime.cfg', 'r');
        let data = (typeof f.readText === 'function') ? f.readText() : f.read();
        f.close();
        if (data instanceof ArrayBuffer) {
            const bytes = new Uint8Array(data);
            let s = '';
            for (let i = 0; i < bytes.length; i++) s += String.fromCharCode(bytes[i]);
            data = s;
        }
        return data;
    } catch (_e) { return null; }
}

// Scan all lines, skip blanks and comment lines (';' or '#'), return the
// first that parses as a layout. Lets the cfg carry a documentation header.
function parseConfigText(txt) {
    if (!txt) return null;
    const lines = String(txt).split(/\r?\n/);
    for (let i = 0; i < lines.length; i++) {
        const t = lines[i].trim();
        if (!t || t[0] === ';' || t[0] === '#') continue;
        const p = parseLayout(t);
        if (p) return p;
    }
    return null;
}

let layout = parseLayout(DEFAULT_LAYOUT);
const cfgRaw = readConfigText();
if (cfgRaw) {
    const p = parseConfigText(cfgRaw);
    if (p) { layout = p; log('loaded layout from cfg'); }
    else log('WARN: no valid layout line in cfg, using default ' + DEFAULT_LAYOUT);
} else {
    log('no cfg, using default ' + DEFAULT_LAYOUT);
}

// The 5 forced RESULTS, in OnInit roll order. For presence rolls, "present"
// must be != 0 and within [lo,hi]; 1 satisfies every bound here. "absent" = 0.
function buildPlan(L) {
    return [
        L.variant ? 1 : 0,   // #0 variant: 0=A, 1=B
        L.bunny   ? 1 : 0,    // #1 bunny  : !=0 present, 0 absent
        L.mouse   ? 1 : 0,    // #2 mouse
        L.bird    ? 1 : 0,    // #3 bird
        L.bush    ? 1 : 0,    // #4 bush
    ];
}
const ROLL_NAMES = ['variant', 'bunny', 'mouse', 'bird', 'bush'];
let plan = buildPlan(layout);
log('plan = ' + JSON.stringify(plan) +
    '  (variant=' + (layout.variant ? 'B' : 'A') +
    ' bunny=' + layout.bunny + ' mouse=' + layout.mouse +
    ' bird=' + layout.bird + ' bush=' + layout.bush + ', butterfly=always)');

let rollIndex = 0;            // how many script-Rand calls since last reset
let hooksInstalled = false;

function resetPlan(why) {
    rollIndex = 0;
    log('reset roll plan (' + why + ')');
}

function installHooks() {
    if (hooksInstalled) return true;
    const mod = Process.findModuleByName('bulanci.exe');
    if (!mod) return false;

    const pScriptRand = mod.base.add(RVA_CDSSCRIPT_RAND);
    const pSrand      = mod.base.add(RVA_SRAND);
    const pGamingCtor = mod.base.add(RVA_CGAMING_CTOR);

    log('bulanci.exe base = 0x' + mod.base.toString(16));
    log('  CDSScript_Rand @ 0x' + pScriptRand.toString(16));
    log('  srand          @ 0x' + pSrand.toString(16));

    // Reset the roll plan whenever the match PRNG is reseeded (CGaming_ctor),
    // which happens right before the level OnInit runs.
    Interceptor.attach(pSrand, {
        onEnter: function (args) { resetPlan('srand(0x' + args[0].toString(16) + ')'); }
    });
    // Belt-and-suspenders: also reset on CGaming_ctor entry in case a build
    // reorders the srand. Harmless if it double-resets (OnInit hasn't rolled yet).
    Interceptor.attach(pGamingCtor, {
        onEnter: function () { resetPlan('CGaming_ctor entry'); }
    });

    // Force the first 5 script Rand RESULTS after a reset; pass the rest through.
    Interceptor.attach(pScriptRand, {
        onLeave: function (retval) {
            if (rollIndex < plan.length) {
                const want = plan[rollIndex];
                const orig = retval.toInt32();
                retval.replace(ptr(want));
                log('CDSScript_Rand #' + rollIndex + ' (' + ROLL_NAMES[rollIndex] +
                    ') result ' + orig + ' -> forced ' + want);
            }
            rollIndex++;
        }
    });

    hooksInstalled = true;
    log('hooks installed. Pick "Na dobrou noc" in the menu.');
    return true;
}

let tries = 0;
const timer = setInterval(function () {
    if (installHooks() || ++tries > 100) {
        clearInterval(timer);
        if (!hooksInstalled) log('ERROR: failed to install hooks after ' + tries + ' tries');
    }
}, 50);
