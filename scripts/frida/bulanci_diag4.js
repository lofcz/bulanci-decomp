// bulanci_diag4.js — hook the static srand/rand functions at 0x4477df/0x4477ec
'use strict';

setTimeout(function () {
    const log = function () {
        var args = Array.prototype.slice.call(arguments);
        args.unshift('[diag4]');
        console.log.apply(console, args);
    };

    const bulMod = Process.findModuleByName('bulanci.exe');
    if (!bulMod) { log('bulanci not found'); return; }
    log('bulanci.exe base=0x' + bulMod.base.toString(16) + ' size=0x' + bulMod.size.toString(16));

    // The EXE has its own static srand and rand. Per the disassembly at
    // 0x402b3c (e8 9e 4c 04 00) the call target is 0x4477df, and per
    // 0x438cc8 (e8 1f eb 00 00) the call target is 0x4477ec.
    const srandFn = bulMod.base.add(0x4477df - 0x400000);
    const randFn  = bulMod.base.add(0x4477ec - 0x400000);
    log('static srand entry: 0x' + srandFn.toString(16));
    log('static rand  entry: 0x' + randFn.toString(16));

    // First 16 bytes of each so we can see they look like real functions
    log('--- srand first 16 bytes ---');
    log(hexdump(srandFn.readByteArray(16), { length: 16, ansi: true }));
    log('--- rand first 16 bytes ---');
    log(hexdump(randFn.readByteArray(16), { length: 16, ansi: true }));

    try {
        Interceptor.attach(srandFn, {
            onEnter: function (args) { log('HOOK FIRE: srand(0x' + args[0].toString(16) + ')'); }
        });
        log('attached to srand');
    } catch (e) { log('srand attach failed: ' + e); }

    try {
        Interceptor.attach(randFn, {
            onLeave: function (retval) { log('HOOK FIRE: rand() -> 0x' + retval.toString(16)); }
        });
        log('attached to rand');
    } catch (e) { log('rand attach failed: ' + e); }

    log('--- waiting 12s. Pick "Na dobrou noc" in the menu during this time! ---');
    setTimeout(function () { log('=== done ==='); }, 12000);
}, 1500);
