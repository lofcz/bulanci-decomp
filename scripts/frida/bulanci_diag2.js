// bulanci_diag2.js — find the module that owns 0x748da1b0 and dump nearby
// modules. Also look for srand/rand inside bulanci.exe itself.
'use strict';

setTimeout(function () {
    const log = function () {
        var args = Array.prototype.slice.call(arguments);
        args.unshift('[diag2]');
        console.log.apply(console, args);
    };

    log('--- 2s elapsed, running ---');

    // Which module contains 0x748da1b0 (the srand findGlobalExportByName returned)?
    const target = ptr('0x748da1b0');
    let owner = null;
    Process.enumerateModules().forEach(function (m) {
        if (target.compare(m.base) >= 0 && target.compare(m.base.add(m.size)) < 0) {
            owner = m;
        }
    });
    if (owner) {
        log('0x748da1b0 is in module: ' + owner.name + ' (base=0x' + owner.base.toString(16) +
            ' size=0x' + owner.size.toString(16) + ' path=' + owner.path + ')');
        const offset = target.sub(owner.base).toInt32();
        log('  offset within module: 0x' + offset.toString(16));
    } else {
        log('0x748da1b0 is in NO module — orphaned memory');
    }

    // List all modules whose name contains "msv" or "crt" or "ucrt"
    log('--- modules whose name matches /msv|crt|ucrt/i ---');
    Process.enumerateModules().forEach(function (m) {
        if (/msv|crt|ucrt|api-ms/i.test(m.name)) {
            log('  ' + m.name + ' (base=0x' + m.base.toString(16) + ' size=0x' + m.size.toString(16) + ')');
        }
    });

    // Now: does bulanci.exe itself contain an srand-like function?
    // Search its .text section for a known srand prologue. The MSVCRT srand
    // typically starts with a 1-byte or 2-byte instruction. Let's just
    // search the EXE for 5 different typical function prologues via
    // Memory.scan, and look for call sites at 0x402b3c and 0x438cc8.
    const bulMod = Process.findModuleByName('bulanci.exe');
    if (bulMod) {
        const callSite_srand = bulMod.base.add(0x402b3c - 0x400000);
        const callSite_rand  = bulMod.base.add(0x438cc8 - 0x400000);
        log('--- bytes at 0x402b3c (alleged srand call site) ---');
        try {
            const bytes = callSite_srand.readByteArray(16);
            log('  ' + hexdump(bytes, { length: 16, ansi: true }));
        } catch (e) { log('  read failed: ' + e); }

        log('--- bytes at 0x438cc8 (alleged rand call site) ---');
        try {
            const bytes = callSite_rand.readByteArray(16);
            log('  ' + hexdump(bytes, { length: 16, ansi: true }));
        } catch (e) { log('  read failed: ' + e); }

        // Disassemble the 20 instructions around 0x402b3c
        log('--- 20 instructions starting at 0x402b20 (CBulanci_OnCreate) ---');
        try {
            const codeStart = bulMod.base.add(0x402b20 - 0x400000);
            const code = codeStart.readByteArray(64);
            log('  ' + hexdump(code, { length: 64, ansi: true }));
        } catch (e) { log('  read failed: ' + e); }

        // Hook the srand call site and the rand call site
        log('--- hooking srand call site @ 0x402b3c ---');
        try {
            Interceptor.attach(callSite_srand, function (args) {
                log('HOOK FIRE: srand call site @ 0x402b3c entered');
            });
            log('  attached');
        } catch (e) { log('  attach failed: ' + e); }

        log('--- hooking rand call site @ 0x438cc8 ---');
        try {
            Interceptor.attach(callSite_rand, {
                onEnter: function (args) {
                    log('HOOK FIRE: rand call site @ 0x438cc8 entered');
                }
            });
            log('  attached');
        } catch (e) { log('  attach failed: ' + e); }

        // Also try hooking the full CDSScript_Rand function at 0x438cb0
        const cdScriptRand = bulMod.base.add(0x438cb0 - 0x400000);
        log('--- hooking CDSScript_Rand @ 0x438cb0 ---');
        try {
            Interceptor.attach(cdScriptRand, {
                onEnter: function (args) {
                    log('HOOK FIRE: CDSScript_Rand entered');
                }
            });
            log('  attached');
        } catch (e) { log('  attach failed: ' + e); }

        // And hook CBulanci_OnCreate at 0x402b20
        const cBulanciOnCreate = bulMod.base.add(0x402b20 - 0x400000);
        log('--- hooking CBulanci_OnCreate @ 0x402b20 ---');
        try {
            Interceptor.attach(cBulanciOnCreate, {
                onEnter: function (args) {
                    log('HOOK FIRE: CBulanci_OnCreate entered');
                }
            });
            log('  attached');
        } catch (e) { log('  attach failed: ' + e); }
    }

    log('--- waiting 12s for any hook fires ---');
    setTimeout(function () { log('=== done ==='); }, 12000);
}, 2000);
