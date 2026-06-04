// ============================================================================
// bulanci_diag.js — diagnose why the srand/rand hook doesn't fire
// ============================================================================
// Dumps:
//   - bulanci.exe base address
//   - bulanci.exe IAT entries named srand, rand, _srand, _rand
//   - all modules containing srand/rand exports and their addresses
//   - all modules containing srand/rand imports and their slots
// Tries hooking:
//   - srand/rand via findGlobalExportByName (already known to install)
//   - each import slot in bulanci.exe that names srand/rand
// Logs every fired onEnter on srand for 15s.
// ============================================================================

'use strict';

const log = function () {
    var args = Array.prototype.slice.call(arguments);
    args.unshift('[diag]');
    console.log.apply(console, args);
};

log('=== bulanci PRNG hook diagnostic (deferred 3s) ===');

// Wait 3s for the EXE to settle (loader, delay-load DLLs, main thread startup).
setTimeout(function () {
    runDiagnostic();
}, 3000);

function runDiagnostic() {
log('--- 3s elapsed, running diagnostic ---');

const mod = Process.findModuleByName('bulanci.exe');
if (mod === null) {
    log('bulanci.exe module not found?');
} else {
    log('bulanci.exe base=0x' + mod.base.toString(16) + ' size=0x' + mod.size.toString(16) + ' path=' + mod.path);
}

// Enumerate imports of bulanci.exe
if (mod) {
    log('--- bulanci.exe IAT entries named srand/rand/_srand/_rand/__srand/__rand ---');
    const targetNames = ['srand', 'rand', '_srand', '_rand', '__srand', '__rand'];
    const found = [];
    mod.enumerateImports().forEach(function (imp) {
        if (targetNames.indexOf(imp.name) !== -1) {
            log('  IAT: ' + imp.module + '!' + imp.name + ' slot=0x' + imp.slot.toString(16) +
                ' -> current value 0x' + imp.address.toString(16));
            found.push(imp);
        }
    });
    if (found.length === 0) {
        log('  NO srand/rand imports in bulanci.exe IAT — EXE is statically linked to CRT');
    }

    // Try hooking each IAT slot directly (if any)
    found.forEach(function (imp) {
        try {
            Interceptor.attach(imp.address, {
                onEnter: function (args) {
                    log('HOOK FIRE: ' + imp.name + ' called with 0x' + args[0].toString(16));
                }
            });
            log('attached to IAT-resolved ' + imp.name + ' at 0x' + imp.address.toString(16));
        } catch (e) {
            log('failed to attach to IAT-resolved ' + imp.name + ': ' + e);
        }
    });
}

// Also try findGlobalExportByName (the way the real script hooks) and check
// if that address is in bulanci.exe's address range (static CRT) or outside
// (MSVCRT or another module).
try {
    const s = Module.findGlobalExportByName('srand');
    const r = Module.findGlobalExportByName('rand');
    log('findGlobalExportByName srand=' + (s ? '0x' + s.toString(16) : 'null') +
        ' rand=' + (r ? '0x' + r.toString(16) : 'null'));
    if (s && mod) {
        const inBulanci = s.compare(mod.base) >= 0 && s.compare(mod.base.add(mod.size)) < 0;
        log('srand 0x' + s.toString(16) + ' is in bulanci.exe: ' + inBulanci);
    }
    if (s) {
        Interceptor.attach(s, {
            onEnter: function (args) {
                log('HOOK FIRE: srand(0x' + args[0].toString(16) + ')');
            }
        });
        log('attached hook to srand at 0x' + s.toString(16));
    }
    if (r) {
        Interceptor.attach(r, {
            onLeave: function (retval) {
                log('HOOK FIRE: rand() -> 0x' + retval.toString(16));
            }
        });
        log('attached hook to rand at 0x' + r.toString(16));
    }
} catch (e) {
    log('findGlobalExportByName failed: ' + e);
}

// List all modules with srand/rand exports
log('--- modules exporting srand/rand ---');
let modCount = 0;
Process.enumerateModules().forEach(function (m) {
    modCount++;
});
log('total modules loaded: ' + modCount);
Process.enumerateModules().forEach(function (m) {
    try {
        const s = Module.findExportByName(m.name, 'srand');
        const r = Module.findExportByName(m.name, 'rand');
        if (s || r) {
            log('  ' + m.name + ' (base=0x' + m.base.toString(16) + ' size=0x' + m.size.toString(16) + '): srand=' + (s ? '0x' + s.toString(16) : '-') +
                ' rand=' + (r ? '0x' + r.toString(16) : '-'));
        }
    } catch (_e) {}
});

// List all modules importing srand/rand
log('--- modules importing srand/rand ---');
Process.enumerateModules().forEach(function (m) {
    if (m.name === 'bulanci.exe') return;
    try {
        m.enumerateImports().forEach(function (imp) {
            if (imp.name === 'srand' || imp.name === 'rand') {
                log('  ' + m.name + ' imports ' + imp.name + ' from ' + imp.module +
                    ' slot=0x' + imp.slot.toString(16) + ' -> 0x' + imp.address.toString(16));
            }
        });
    } catch (_e) {}
});

log('=== diagnostic complete — waiting 15s for any hook fires ===');

setTimeout(function () {
    log('=== done. quitting. ===');
}, 15000);
}  // end runDiagnostic
