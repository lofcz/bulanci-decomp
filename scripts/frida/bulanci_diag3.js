// bulanci_diag3.js — read IAT slots 0x4477df and 0x4477ec, and try hooking
// whatever they point to. Also dump the import table from the on-disk PE.
'use strict';

setTimeout(function () {
    const log = function () {
        var args = Array.prototype.slice.call(arguments);
        args.unshift('[diag3]');
        console.log.apply(console, args);
    };

    const bulMod = Process.findModuleByName('bulanci.exe');
    if (!bulMod) { log('bulanci not found'); return; }
    log('bulanci.exe base=0x' + bulMod.base.toString(16));

    const iat_srand = bulMod.base.add(0x4477df - 0x400000);
    const iat_rand  = bulMod.base.add(0x4477ec - 0x400000);
    log('IAT slot srand @ 0x4477df:');
    try {
        const v = iat_srand.readPointer();
        log('  value (ptr): 0x' + v.toString(16));
        // find module containing that value
        let owner = null;
        Process.enumerateModules().forEach(function (m) {
            if (v.compare(m.base) >= 0 && v.compare(m.base.add(m.size)) < 0) owner = m;
        });
        log('  in module: ' + (owner ? owner.name + ' (base=0x' + owner.base.toString(16) + ')' : 'ORPHAN'));
    } catch (e) { log('  read failed: ' + e); }

    log('IAT slot rand @ 0x4477ec:');
    try {
        const v = iat_rand.readPointer();
        log('  value (ptr): 0x' + v.toString(16));
        let owner = null;
        Process.enumerateModules().forEach(function (m) {
            if (v.compare(m.base) >= 0 && v.compare(m.base.add(m.size)) < 0) owner = m;
        });
        log('  in module: ' + (owner ? owner.name + ' (base=0x' + owner.base.toString(16) + ')' : 'ORPHAN'));
    } catch (e) { log('  read failed: ' + e); }

    // Also: dump the entire IAT section's first 64 bytes for inspection
    log('--- first 64 bytes of the IAT region (0x447780-0x4477c0) ---');
    try {
        const iatBase = bulMod.base.add(0x447780 - 0x400000);
        const bytes = iatBase.readByteArray(128);
        log(hexdump(bytes, { length: 128, ansi: true }));
    } catch (e) { log('  read failed: ' + e); }

    // Try hooking the IAT-resolved function address for srand/rand
    try {
        const s = iat_srand.readPointer();
        const r = iat_rand.readPointer();
        log('--- hooking IAT-resolved srand @ 0x' + s.toString(16) + ' ---');
        Interceptor.attach(s, {
            onEnter: function (args) { log('HOOK FIRE: srand(0x' + args[0].toString(16) + ')'); }
        });
        log('  attached');
        log('--- hooking IAT-resolved rand @ 0x' + r.toString(16) + ' ---');
        Interceptor.attach(r, {
            onLeave: function (retval) { log('HOOK FIRE: rand() -> 0x' + retval.toString(16)); }
        });
        log('  attached');
    } catch (e) { log('  hook install failed: ' + e); }

    log('--- waiting 12s ---');
    setTimeout(function () { log('=== done ==='); }, 12000);
}, 2000);
