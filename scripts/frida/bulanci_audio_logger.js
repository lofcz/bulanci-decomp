/* eslint-disable */
"use strict";
//
// bulanci_audio_logger.js
//
// Verifies the RE findings about Bulanci hit / kill sounds.
//
// Findings under test (from Ghidra decomp + unpacked/overlay analysis):
//
//   * Default in-game audio bank is resource 0x10152 (65874), backed by
//     `unpacked/overlay/res_0000065874_43_AudioBank.bin` (41 samples).
//
//   * When a player is hit by a bullet, `CBulanek_OnTakeDamage` fires
//     `_Globals::TriggerBankSample(0x1, 0, 0x24, attenDb, NULL, 1)` — i.e.
//     slot 0x24 of the default bank. That slot is the *thud* sample
//     (offset 0x0012_9094 inside the bank, ~3.8 KB).
//
//   * The "voice quip" that follows is played later from
//     `CBulanek_OnEvent` case 0xdb. The slot is chosen by
//     `CBulanek_GetHitQuipSlot(this)`:
//        - returns 0x27 if `this->bSlotKind` is in [0x20..0x23] (special
//          player slots)
//        - else returns `table[+0x124]` where +0x124 is the player's
//          CURRENT kill count (`dwScriptKillScore`).
//
//   * TriggerBankSample also fires from menu code (slots 0/0x18-0x1c) and
//     pickup code. The script logs those too, but tagged with their slot
//     name so the kill / hit ones are easy to filter.
//
//   * Conditions for the hit thud to actually play (in OnTakeDamage):
//        - pCorpseAnim == NULL              (player alive)
//        - bHitStun == 0                    (not already in stun)
//        - pGamingHostScratch != NULL       (match is active)
//        - (gaming host) match not paused
//        - dwView_flags & 1                 (player is live)
//
// This script hooks every entry point of that pipeline and prints a
// JSONL record per event with enough context (kill count, lives, slot
// kind, hit-stun flag, bank id, slot index, slot wav name) that you can
// reproduce the entire chain from a recorded run.
//
// ──────────────────────────────────────────────── usage
//
//   frida -l scripts\frida\bulanci_audio_logger.js            ^
//         -l scripts\frida\menu_hover_audio_trace.js          ^
//         -f orig\bulanci.exe                                 ^
//         -o hit_audio.jsonl
//
// Chaining is purely additive: the menu-hover script keeps emitting its
// own JSONL records, this script emits its own. If you only want hit
// sounds, drop the menu line. The same script works on
// `bulanci_insturmented.exe` — function addresses are identical because
// the instrumented build is a recompile of the same source.
//
// During play, %resume if Frida doesn't auto-resume. Type
// `rpc.exports.dumpLive()` for a one-shot snapshot of every live player.
// Type `rpc.exports.die()` to detach cleanly.
//
// ──────────────────────────────────────────────── output schema
//
// Every line is a JSON object with at least { n, t, ev }:
//   n  — monotonic sequence
//   t  — game-clock millisecond (g_dwElapsedMs) when available
//   ev — event name
//
// Per-event extra fields:
//   "ready"            : { module, base, slide, bankHint }
//   "Bank_Deserialize" : { bankPtr, parent, stream }
//   "SetMusicAndSound" : { mp3Res, bankIndexRes, bankPtr, prevBankPtr }
//                        bankIndexRes is the key fact — it tells us
//                        which .bin file the bank samples live in.
//   "Bank_Loaded"      : { bankPtr, id, slotCount, name, file }
//                        merged by later events from the same bankPtr.
//   "Shot_ResolveHit"  : { shot: { weaponStrength, ownerSlot, direction },
//                        victim: <bulanek dump> }
//   "OnTakeDamage"     : { arg1, arg2, arg3, retval, pre, post,
//                        conditionsMet }
//                        `conditionsMet` summarises the 5 guards so you
//                        can see at a glance why a hit did or didn't
//                        fire the thud.
//   "GetHitQuipSlot"   : { bulanek, slot, slotName, slotWav,
//                        reason: "table_lookup" | "special_slot" }
//   "TriggerBankSample": { category, bankPtr, bankId, slot, slotName,
//                        slotWav, attenDb, evtTgt, loopArm,
//                        triggerKind: "thud"|"quip"|"menu"|"pickup"|... }
//
// The `bulanek dump` shape (from dumpBulanek):
//   { ptr, slotKind, lives, teamColor, killCount, deathCount,
//     hitStun, pCorpseAnim, pGamingHost, pHitQuipPlayer, pDeathVoicePlayer }
//
// ──────────────────────────────────────────────── verified addresses
// All cross-checked via ghidra-mcp.get_function_by_address:
//
//   CBulanek_OnTakeDamage        @ 0x0041db00
//   CBulanek_GetHitQuipSlot      @ 0x004166b0
//   CShot_ResolveHit             @ 0x0041dd70
//   _Globals::TriggerBankSample  @ 0x00422430
//   _Globals::CDSAudioPlayer_Create       @ 0x00422310
//   _Globals::CDSAudioPlayer_PlayAndRelease @ 0x004223c0
//   CDSAudioBank_Deserialize     @ 0x00429600   (facet; bank = facet - 0x14)
//   CGaming_SetMusicAndSoundBank @ 0x0041ba60
//   g_dwElapsedMs                @ 0x004b3bd8
//
(function () {
    "use strict";

    console.error("[+] bulanci_audio_logger.js loaded");

    // ───────────────────────────── module discovery
    const IMAGE_BASE_PE = 0x00400000;

    function findGameModule() {
        const names = [
            "bulanci_insturmented.exe", "Bulanci_insturmented.exe",
            "bulanci.exe", "Bulanci.exe", "BULANCI.EXE",
        ];
        for (const n of names) {
            try {
                const m = Process.getModuleByName(n);
                if (m) return m;
            } catch (e) { /* keep trying */ }
        }
        for (const m of Process.enumerateModules()) {
            if (m.name.toLowerCase().indexOf("bulanci") !== -1) return m;
        }
        return null;
    }

    const MOD = findGameModule();
    if (!MOD) {
        console.error("[-] Could not locate Bulanci module - aborting");
        throw new Error("Bulanci module not found");
    }
    const MODULE_BASE = MOD.base;
    const SLIDE = MODULE_BASE.sub(IMAGE_BASE_PE);
    function abs(va) { return ptr(va).add(SLIDE); }
    function hex(p)  { try { return ptr(p).toString(); } catch (e) { return String(p); } }
    function safe(fn, fb) { try { return fn(); } catch (e) { return fb; } }
    function rS32(p, o) { return safe(() => ptr(p).add(o).readS32(), null); }
    function rU32(p, o) { return safe(() => ptr(p).add(o).readU32(), null); }
    function rS16(p, o) { return safe(() => ptr(p).add(o).readS16(), null); }
    function rU16(p, o) { return safe(() => ptr(p).add(o).readU16(), null); }
    function rU8(p, o)  { return safe(() => ptr(p).add(o).readU8(),  null); }
    function rPtr(p, o) { return safe(() => ptr(p).add(o).readPointer(), null); }

    const ADDR = {
        OnTakeDamage:             abs(0x0041db00),
        GetHitQuipSlot:           abs(0x004166b0),
        ShotResolveHit:           abs(0x0041dd70),
        TriggerBankSample:        abs(0x00422430),
        CDSAudioPlayerCreate:     abs(0x00422310),
        CDSAudioPlayerPlayRel:    abs(0x004223c0),
        BankDeserialize:          abs(0x00429600),
        SetMusicAndSoundBank:     abs(0x0041ba60),
        CBulanekCtor:             abs(0x0041e4b0),
        CGaming_SpawnPlayerAtSlot:abs(0x00420530),
        _rand:                    abs(0x004477ec),
        _srand:                   abs(0x004477df),
        CBulanci_OnCreate:        abs(0x00402b20),
        CGaming_ctor:             abs(0x0041ff90),
        CMenu_OpenNetworkSession: abs(0x00414dd0),
        CGame_StartGame:          abs(0x00413ce0),
        CGame_ProcessNetMessage:  abs(0x00415290),  // actual function address
        g_dwElapsedMs:            abs(0x004b3bd8),
    };

    // Hardcoded bulanci.exe globals verified via ghidra-mcp disassembly:
    //
    //   * g_pApp is at 0x004b3b88 — the prologue of TriggerBankSample
    //     loads it with `MOV EAX,[0x004b3b88]` then adds 0x4c0 to read
    //     the default AudioBank pointer.
    //
    //   * dwView_flags is at offset +0x44 (NOT +0x158 — that was a
    //     decomp artefact).  Same offset is used for both the gaming
    //     host's paused bit ((flags>>4)&1) and the player's live bit
    //     (flags&1), as confirmed in the OnTakeDamage disassembly.
    //
    //   * pCorpseAnim is at offset +0xFC.
    //
    //   * nOrigin_x is at +0x20 (so ComputeSpatialAttenuationDb100 takes
    //     &(this+0x20) as its sole arg).
    //
    // The runtime scan for g_pApp is kept as a sanity check; if it
    // returns a different address, we log a warning so the user notices
    // a binary mismatch (the script supports both bulanci.exe and
    // bulanci_insturmented.exe).
    const G_APP_ADDR_HARDCODED = ptr(0x004b3b88);

    console.error(`[+] module=${MOD.name} base=${MODULE_BASE} slide=${SLIDE.toInt32()}`);
    for (const k of Object.keys(ADDR)) {
        console.error(`[+]   ${k.padEnd(28)} @ ${ADDR[k]}`);
    }
    console.error(`[+]   g_pApp (hardcoded)        @ ${G_APP_ADDR_HARDCODED}  (verified via ghidra-mcp)`);
    console.error(`[+]   CBulanek offsets:        pCorpseAnim=+0xFC, dwView_flags=+0x44, nOrigin_x=+0x20`);

    // ───────────────────────────── bank / slot catalog
    // Mirrors unpacked/overlay/extracted_all_samples/bank_65874/
    // (sample_00.wav .. sample_40.wav). Updated at runtime whenever a
    // bank is deserialized so dynamic banks (65870..65873, etc.) light up
    // automatically. Resource id 65537 (0x10001, "bedtime" menu bank,
    // 5 slots) was confirmed live via CGaming_SetMusicAndSound.
    const KNOWN_BANKS = {
        65537: { name: "menu_bedtime",   file: "res_0000065870_43_AudioBank.bin", wavDir: "extracted_all_samples/bank_65870" },
        65538: { name: "bank_8bit",      file: "res_0000065871_43_AudioBank.bin", wavDir: "extracted_all_samples/bank_65871" },
        65539: { name: "bank_steel",     file: "res_0000065872_43_AudioBank.bin", wavDir: "extracted_all_samples/bank_65872" },
        65540: { name: "default_ingame", file: "res_0000065873_43_AudioBank.bin", wavDir: "extracted_all_samples/bank_65873" },
        65870: { name: "bank_65870",     file: "res_0000065870_43_AudioBank.bin", wavDir: "extracted_all_samples/bank_65870" },
        65871: { name: "bank_65871",     file: "res_0000065871_43_AudioBank.bin", wavDir: "extracted_all_samples/bank_65871" },
        65872: { name: "bank_65872",     file: "res_0000065872_43_AudioBank.bin", wavDir: "extracted_all_samples/bank_65872" },
        65873: { name: "bank_65873",     file: "res_0000065873_43_AudioBank.bin", wavDir: "extracted_all_samples/bank_65873" },
        65874: { name: "default_ingame", file: "res_0000065874_43_AudioBank.bin", wavDir: "extracted_all_samples/bank_65874" },
    };

    // 41-sample banks light up as "default_ingame" candidates.
    const IN_GAME_BANK_CAPACITY = 41;

    // Slot → friendly label (across all banks; the slot *index* is global
    // per-bank, but the meanings are stable because every bank uses the
    // same 41-slot layout — see unpacked/overlay/.../bank_65874/sample_*).
    const SLOT_LABEL = {
        // Hit / quip (verified via decomp)
        0x24: "HIT_THUD",
        0x27: "HIT_QUIP_SPECIAL",
        // Menu UI (also TriggerBankSample callers — see menu_hover_audio_trace.js)
        0x00: "MENU_HOVER",
        0x18: "MENU_HISTORY",
        0x19: "MENU_QUIT",
        0x1a: "MENU_X_EXIT",
        0x1b: "MENU_START",
        0x1c: "MENU_F12",
    };

    // bSlotKind → quip slot.  The decompiled GetHitQuipSlot does
    // `*(undefined4 *)(&DAT_004aef88 + (uint)*(byte *)(+0x124) * 4)`,
    // but the byte at +0x124 is bSlotKind (always 0..3 — humans are
    // 0..3, AI bots get `slotKind & 3`), not a kill counter.  So only
    // the first 4 entries of the 12-entry table are functionally
    // reachable.  We still read all 12 entries because the data is
    // already there and it makes the script tolerant if the byte
    // value ever exceeds 3 (e.g. due to a struct-layout drift).
    const QUIP_TABLE_ADDR = abs(0x004aef88);
    const QUIP_TABLE_ENTRIES = 12;     // 4 functionally reachable, 8 dead
    const QUIP_SLOT_BY_KILLS = Object.create(null);
    function learnQuipTable() {
        if (Object.keys(QUIP_SLOT_BY_KILLS).length > 0) return;
        try {
            const buf = QUIP_TABLE_ADDR.readByteArray(QUIP_TABLE_ENTRIES * 4);
            const b   = new Uint8Array(buf);
            for (let i = 0; i < QUIP_TABLE_ENTRIES; i++) {
                const v = (b[i*4]   | (b[i*4+1] << 8) |
                          (b[i*4+2] << 16) | (b[i*4+3] << 24)) >>> 0;
                if (v >= 0 && v <= 40) {
                    QUIP_SLOT_BY_KILLS[i] = v;
                }
            }
        } catch (e) { /* ignore */ }
    }

    // ───────────────────────────── per-player score tracker
    // nLivesRemaining is set from the CBulanekCtor's nLives param, which
    // is per-game-mode (deathmatch=1, tournament=3, etc.).  Decremented in
    // CBulanek_OnEvent case 0xd7 (a "knockdown" event).  So a "1" in the
    // trace means "1 life in whatever mode you're playing", not "1 life
    // universally".  See CBulanekCtor asm at 0x41e775-0x41e7a2.
    //
    // We track per-player: kills (dwScriptKillScore +0x128), deaths
    // (dwDeathCount +0x13C), lives (nLivesRemaining +0x18C), bSlotKind
    // (+0x124) and bTeamColor (+0x123).  Score = kills - deaths.
    const playerScores = new Map();
    function fmtDelta(n) { return (n >= 0 ? "+" : "") + n; }
    function snapshotPlayer(bulanekPtr) {
        if (!bulanekPtr) return null;
        const x = ptr(bulanekPtr);
        if (x.isNull()) return null;
        try {
            return {
                ptr:        x.toString(),
                bSlotKind:  rU8(x, 0x124),
                bTeamColor: rU8(x, 0x123),
                kills:      rU32(x, 0x128),
                deaths:     rU32(x, 0x13C),
                lives:      rS16_(x, 0x18C),
            };
        } catch (e) { return null; }
    }
    function maybeEmitScore(p) {
        if (!p) return;
        const key   = p.ptr;
        const prev  = playerScores.get(key);
        const score = p.kills - p.deaths;     // net kills
        if (prev &&
            prev.kills === p.kills &&
            prev.deaths === p.deaths &&
            prev.lives  === p.lives) {
            return;                            // no change -> no spam
        }
        const isNew = !prev;
        const delta = prev ? {
            dKills:  p.kills  - prev.kills,
            dDeaths: p.deaths - prev.deaths,
            dLives:  p.lives  - prev.lives,
            dScore:  score - (prev.kills - prev.deaths),
        } : null;
        playerScores.set(key, { ...p, score });

        emit("score", {
            ptr:            p.ptr,
            bSlotKind:      p.bSlotKind,
            bTeamColor:     p.bTeamColor,
            kills:          p.kills,
            deaths:         p.deaths,
            score:          score,
            nLivesRemaining: p.lives,
            isNew:          isNew,
            change:         delta,
            livesNote:      "game-mode dependent; set by CBulanekCtor from nLives param",
        });

        // Yellow-colored terminal display.  Runs in TTY for human eyes;
        // if the user redirected with `frida -o file.jsonl`, the ANSI
        // escapes are mixed into the file -- filter with `grep '^{'`
        // to get just the JSONL events.
        const YEL = "\x1b[33m";
        const DIM = "\x1b[2m";
        const RST = "\x1b[0m";
        const team = (p.bTeamColor === 0) ? "" : (DIM + "team=" + p.bTeamColor + RST + " ");
        const dStr = delta
            ? ` ${DIM}(Δk=${fmtDelta(delta.dKills)} Δd=${fmtDelta(delta.dDeaths)} Δl=${fmtDelta(delta.dLives)} Δs=${fmtDelta(delta.dScore)})${RST}`
            : "";
        console.log(
            `${YEL}[SCORE] P${p.bSlotKind}${team}@ ${p.ptr} : ` +
            `score=${score}  (${p.kills}k / ${p.deaths}d)  lives=${p.lives}${dStr}${RST}`
        );
    }
    function dumpPlayerScores() {
        const out = [];
        for (const [k, v] of playerScores.entries()) {
            out.push({
                ptr: k, bSlotKind: v.bSlotKind,
                kills: v.kills, deaths: v.deaths, lives: v.lives,
                score: (v.kills|0) - (v.deaths|0),
            });
        }
        return out;
    }

    // ───────────────────────────── nLives override (Option A)
    // CBulanekCtor signature (verified via ghidra decompile):
    //   CBulanek* __thiscall CBulanekCtor(
    //       CBulanek* this,                  // ECX
    //       CGame* pGame,                    // args[0]
    //       uchar slotKind,                  // args[1]
    //       void* pGamingHostScratch,        // args[2]
    //       int initialTrack,                // args[3]   (0)
    //       void* pTeamColorOverride,        // args[4]
    //       int nLives,                      // args[5]   <-- THE ONE
    //       int nSpeedParam,                 // args[6]
    //       int skinPaletteId);              // args[7]
    //
    // In Frida, thiscall skips ECX, so nLives is args[5].
    //
    // The level's call chain CGaming_SpawnPlayerAtSlot @ 0x420530 →
    // CGaming_SpawnAndInitializePlayer hardcodes nLives=1 (deathmatch).
    // A tournament/league caller would set nLives=3 or 5.  We override
    // args[5] BEFORE the ctor body runs, so nLivesRemaining @ +0x18C
    // ends up with the patched value (ctor writes it at 0x41e7a2).
    let nLivesOverride = null;   // null = no override; number = force
    let nLivesOverrideSlot = null; // null = all slots; number = only this slotKind (0..3)
    const nLivesTrace = { total: 0, overridden: 0, bySlot: Object.create(null) };

    // Game modes are bound to the level; user-facing UI is the "Typ hry"
    // modal in the main menu (click the "Typ hry:" label).  Confirmed
    // string set (UTF-16LE in .rdata):
    //
    //   0x08614C "na cas"         (radio button)   <-- selected by default
    //   0x0860F0 "Délka hry ve vterinách:"         (input: time, default 180s)
    //   0x0861DC "Hra koncí po %d vterinách."     (description)
    //
    //   0x08615C "na zásahy"      (radio button)
    //   0x086120 "Pocet zásahu k vyhre:"           (input: kill count to win)
    //   0x086218 "Kdo %d-krát zasáhne soupere, vyhrál."
    //
    //   0x085754 "na zivoty"      (radio button)
    //   0x085768 "Pocet zivotu:"                  (input: lives per player)
    //   0x085710 "Kdo bude %d-krát zabit, skoncil."
    //
    // That's it — 3 modes total, no others in the binary.
    // At level load, the ctor uses whatever the level's script
    // (InsertOpponent) or the hardcoded path (SpawnPlayerAtSlot, nLives=1)
    // supplies.  For dynamic testing across modes, use setNLives RPC below.
    const KNOWN_GAME_MODES = [
        { id: "time",  radio: "na cas",    input: "Délka hry ve vterinách:",
          inputDefault: 180,                desc: "Hra koncí po %d vterinách.",
          radioAddr: 0x08614C, inputAddr: 0x0860F0, descAddr: 0x0861DC },
        { id: "kill",  radio: "na zásahy", input: "Pocet zásahu k vyhre:",
          inputDefault: null,               desc: "Kdo %d-krát zasáhne soupere, vyhrál.",
          radioAddr: 0x08615C, inputAddr: 0x086120, descAddr: 0x086218 },
        { id: "lives", radio: "na zivoty", input: "Pocet zivotu:",
          inputDefault: null,               desc: "Kdo bude %d-krát zabit, skoncil.",
          radioAddr: 0x085754, inputAddr: 0x085768, descAddr: 0x085710 },
    ];

    function slotName(slot) {
        if (SLOT_LABEL[slot]) return SLOT_LABEL[slot];
        if (slot >= 0 && slot <= 40) return `BANK_SLOT_${slot}`;
        return `BANK_SLOT_0x${slot.toString(16)}`;
    }
    function wavForSlot(bankId, slot) {
        const bank = KNOWN_BANKS[bankId];
        if (!bank) return null;
        return `${bank.wavDir}/sample_${String(slot).padStart(2, "0")}.wav`;
    }
    function bankIdHint(bankPtr) {
        // The bank doesn't store its own resource id; the loader keeps it
        // in CGaming. We just match by pointer if we've seen a SetMusic
        // announce the same bankPtr. (Filled in by SetMusicAndSoundBank
        // hook below.)
        return bankPtrToId.get(bankPtr.toString()) || null;
    }

    // bankPtr -> { id, capacity, file, wavDir, name }   (filled at runtime)
    const bankPtrToId      = new Map();
    const bankPtrToMeta    = new Map();
    // Most recent bank with capacity==41 (i.e. the default in-game bank).
    // Used as a fallback when we can't read g_pApp+0x4c0.
    let lastBigBankPtr   = null;
    let lastBigBankId    = null;

    // Default-bank pointer: the value of *(g_pApp + 0x4c0).  The
    // decomp'd TriggerBankSample says: "if param_2 == 0, param_2 =
    // *(g_pApp + 0x4c0)".  The hardcoded g_pAppAddr (0x004b3b88) is
    // verified via ghidra-mcp disassembly; we also do a runtime sanity
    // scan of the prologue and warn if it disagrees.
    let g_pAppAddr = G_APP_ADDR_HARDCODED;
    let g_pAppDefaultBankPtr = null;
    let g_pAppSanityWarning = null;

    function resolveDefaultBankPtr() {
        if (!g_pAppAddr) return null;
        try {
            // TriggerBankSample's prologue is:
            //   MOV EAX, [0x004b3b88]     ; EAX = g_pApp (the pointer itself)
            //   MOV EAX, [EAX + 0x4c0]    ; EAX = *(g_pApp + 0x4c0) = default bank
            // So this is a TWO-STAGE deref: read the global to get g_pApp,
            // then add 0x4c0 and read again.  An earlier version of this
            // script did `ptr(g_pAppAddr).add(0x4c0).readPointer()`, which
            // reads at the wrong address and returns garbage.
            const g_pApp = ptr(g_pAppAddr).readPointer();
            if (!g_pApp || g_pApp.isNull()) return null;
            return g_pApp.add(0x4c0).readPointer();
        }
        catch (e) { return null; }
    }

    function findGAppAddrByScan() {
        // The prologue of TriggerBankSample loads g_pApp from a global.
        // Common x86 patterns:
        //   A1 <addr>            mov eax, [moffs32]      (5 bytes)
        //   8B 0D <addr>         mov ecx, [moffs32]      (6 bytes)
        //   8B 15 <addr>         mov edx, [moffs32]      (6 bytes)
        //   8B 3D <addr>         mov edi, [moffs32]      (6 bytes)
        // Plus SIB variants: 8B 04 25 <addr>  (mov eax, [moffs32])
        // Plus PUSH/CMOV variants — keep it simple for now.
        try {
            const code = ADDR.TriggerBankSample.readByteArray(160);
            const b = new Uint8Array(code);
            for (let i = 0; i + 5 < b.length; i++) {
                const op = b[i];
                let addr = null;
                let consumed = 0;
                if (op === 0xA1) {
                    addr = (b[i+1] | (b[i+2] << 8) | (b[i+3] << 16) | (b[i+4] << 24)) >>> 0;
                    consumed = 4;
                } else if (op === 0x8B && (b[i+1] === 0x0D || b[i+1] === 0x15 || b[i+1] === 0x1D ||
                                            b[i+1] === 0x2D || b[i+1] === 0x35 || b[i+1] === 0x3D)) {
                    addr = (b[i+2] | (b[i+3] << 8) | (b[i+4] << 16) | (b[i+5] << 24)) >>> 0;
                    consumed = 5;
                } else if (op === 0x8B && b[i+1] === 0x04 && b[i+2] === 0x25) {
                    addr = (b[i+3] | (b[i+4] << 8) | (b[i+5] << 16) | (b[i+6] << 24)) >>> 0;
                    consumed = 6;
                }
                if (addr !== null && addr >= IMAGE_BASE_PE && addr < IMAGE_BASE_PE + 0x00800000) {
                    return ptr(addr);
                }
                if (consumed) i += consumed;
            }
        } catch (e) { /* fall through */ }
        return null;
    }

    // Validate the hardcoded g_pApp address with a runtime scan.
    // If they disagree, log a one-time warning so the user knows.
    try {
        const scanned = findGAppAddrByScan();
        if (scanned && !scanned.equals(g_pAppAddr)) {
            g_pAppSanityWarning = `hardcoded g_pApp=${g_pAppAddr} != scanned ${scanned}`;
            console.error(`[!] ${g_pAppSanityWarning} — using hardcoded`);
        }
        if (g_pAppAddr) g_pAppDefaultBankPtr = resolveDefaultBankPtr();
    } catch (e) { /* ignore */ }

    // ───────────────────────────── bulanek struct dump
    // Offsets verified against the ghidra CBulanek struct (412 bytes,
    // 0x19C) by reading back the ctor asm + comparing to the ghidra
    // analyzer's struct layout.  Earlier version of this script used
    // the decompiler's offset *annotations* (e.g. +0x124 for
    // dwScriptKillScore), which are stale: the actual dwScriptKillScore
    // is at +0x128, and the byte at +0x124 is bSlotKind (the field the
    // game actually uses to index the quip-slot table at 0x4aef88).
    //
    // Offsets:
    //   +0x20  nOrigin_x        (used by ComputeSpatialAttenuationDb100)
    //   +0x24  nOrigin_y
    //   +0x44  dwView_flags     (paused = (flags>>4)&1, live = flags&1)
    //   +0x84  pGamingHostScratch
    //   +0xA8  videoTrackManager  (CDSVideoPlayer, 72 bytes)
    //   +0xFC  pCorpseAnim      (the "alive" guard)
    //   +0x11C bAmmoKind0
    //   +0x11D bAmmoKind1
    //   +0x123 bTeamColor
    //   +0x124 bSlotKind        (1 byte; for AI bots, set to slotKind & 3)
    //   +0x128 dwScriptKillScore
    //   +0x13C dwDeathCount
    //   +0x16A bHitStun
    //   +0x16C pDeathVoicePlayer
    //   +0x170 pHitQuipPlayer
    //   +0x174 pDelayedQuipPlayer
    //   +0x18C nLivesRemaining
    //   +0x190 nLivesSnapshot
    //   +0x194 nSpeedParam
    function rS8_(p, o) { return safe(() => ptr(p).add(o).readS8(), null); }
    function rS16_(p, o) { return safe(() => ptr(p).add(o).readS16(), null); }
    function isAlive(x) { try { return x.add(0xFC).readPointer().isNull(); } catch (e) { return null; } }

    function dumpBulanek(p) {
        if (p === null || p === undefined || p.isNull()) return null;
        const x = ptr(p);
        return {
            ptr:                 hex(x),
            nOrigin:             { x: rS32(x, 0x20), y: rS32(x, 0x24) },
            dwView_flags:        rU32(x, 0x44),
            pCorpseAnim:         hex(rPtr(x, 0xFC)),
            pGamingHost:         hex(rPtr(x, 0x84)),
            ammo0:               rU8(x, 0x11c),
            ammo1:               rU8(x, 0x11d),
            bTeamColor:          rU8(x, 0x123),
            bSlotKind:           rU8(x, 0x124),
            dwScriptKillScore:   rU32(x, 0x128),
            dwDeathCount:        rU32(x, 0x13C),
            bHitStun:            rU8(x, 0x16A),
            pDeathVoicePlayer:   hex(rPtr(x, 0x16c)),
            pHitQuipPlayer:      hex(rPtr(x, 0x170)),
            pDelayedQuipPlayer:  hex(rPtr(x, 0x174)),
            nLivesRemaining:     rS16_(x, 0x18C),
            nLivesSnapshot:      rS16_(x, 0x190),
            nSpeedParam:         rU16(x, 0x194),
            alive:               isAlive(x),
        };
    }

    // Compact form for hot path
    function dumpCompact(p) {
        if (p === null || p === undefined || p.isNull()) return null;
        const x = ptr(p);
        return {
            ptr:                 hex(x),
            bSlotKind:           rU8(x, 0x124),
            nLivesRemaining:     rS16_(x, 0x18C),
            bTeamColor:          rU8(x, 0x123),
            dwScriptKillScore:   rU32(x, 0x128),
            dwDeathCount:        rU32(x, 0x13C),
            bHitStun:            rU8(x, 0x16A),
            alive:               isAlive(x),
        };
    }

    // ───────────────────────────── emit
    let seq = 0;
    function nowMs() { try { return ADDR.g_dwElapsedMs.readU32(); } catch (e) { return null; } }
    function emit(ev, payload) {
        seq += 1;
        const rec = { n: seq, t: nowMs(), ev };
        if (payload) for (const k of Object.keys(payload)) rec[k] = payload[k];
        try {
            console.log(JSON.stringify(rec));
        } catch (e) {
            console.log(JSON.stringify({ n: seq, t: nowMs(), ev, _serialise_err: String(e) }));
        }
    }

    // ───────────────────────────── hooks

    // ── CGaming_SetMusicAndSoundBank @ 0x0041ba60
    // signature: void __thiscall CGaming_SetMusicAndSoundBank(this, mp3Res, bankIndexRes)
    //   `this` is in ECX (MSVC thiscall).  mp3Res: 0 means keep current mp3.
    //   bankIndexRes != 0: load the AudioBankIndex resource, then re-bind
    //   `gaming->bankPtr` (a CDSAudioBank*) to it.
    {
        Interceptor.attach(ADDR.SetMusicAndSoundBank, {
            onEnter(args) {
                try {
                    this.this_   = ptr(this.context.ecx);
                    this.mp3Res  = args[0].toInt32();
                    this.bankRes = args[1].toInt32();
                    this.prevPtr = rPtr(this.this_, 0x80);  // CGaming.bankPtr
                } catch (e) {
                    this.this_ = null;
                }
            },
            onLeave(retval) {
                try {
                    const newPtr = rPtr(this.this_, 0x80);
                    if (newPtr && this.bankRes) {
                        bankPtrToId.set(newPtr.toString(), this.bankRes);
                        const meta = KNOWN_BANKS[this.bankRes];
                        if (meta) bankPtrToMeta.set(newPtr.toString(), meta);
                    }
                    emit("SetMusicAndSound", {
                        mp3Res:       this.mp3Res,
                        bankIndexRes: this.bankRes,
                        bankId:       this.bankRes,
                        bankName:     (KNOWN_BANKS[this.bankRes] || {}).name || null,
                        bankFile:     (KNOWN_BANKS[this.bankRes] || {}).file || null,
                        bankPtr:      hex(newPtr),
                        prevBankPtr:  hex(this.prevPtr),
                    });
                } catch (e) {
                    emit("SetMusicAndSound", { err: String(e) });
                }
            },
        });
    }

    // ── CDSAudioBank_Deserialize @ 0x00429600
    // signature: void __thiscall(CDSAudioBank_BankDeserializeFacet* this, int* pStream)
    //   `this` is the deserialize facet; real CDSAudioBank* is at this-0x14.
    //   The bank stores its slotVector (CDSPtrSlotVec) at facet+0x04, i.e.
    //   bank+0x18.  CDSPtrSlotVec observed at runtime:
    //     +0x00 = pSlots  (array of CDSAudioBankSample*)
    //     +0x04 = capacity
    //   (no count field — capacity is the truth, and the first null pSlot
    //    in [0..capacity) marks the end.)
    {
        Interceptor.attach(ADDR.BankDeserialize, {
            onEnter(args) {
                try {
                    this.facet   = ptr(this.context.ecx);
                    this.bank    = this.facet.sub(0x14);
                    this.pStream = args[0];
                    this.parentRef = rPtr(this.bank, 0x14);
                } catch (e) { this.bank = null; }
            },
            onLeave(retval) {
                try {
                    if (!this.bank) return;
                    const slotVecBase = ptr(this.bank).add(0x18);
                    const pSlots    = rPtr(slotVecBase, 0x00);
                    const capacity  = rU32(slotVecBase, 0x04);
                    // Track the most recent "big" (41-slot) bank — that's
                    // the default in-game one when g_pApp+0x4c0 can't be
                    // resolved.  We prefer the first one we see (audio
                    // engine deserialises it at startup); the menu bank
                    // (5 slots) is loaded later via SetMusicAndSound.
                    if (capacity === IN_GAME_BANK_CAPACITY && !lastBigBankPtr) {
                        lastBigBankPtr = this.bank;
                        lastBigBankId  = 65874;     // canonical id from unpacked/overlay
                    }
                    emit("Bank_Deserialize", {
                        bankPtr:    hex(this.bank),
                        facetPtr:   hex(this.facet),
                        parentRef:  hex(this.parentRef),
                        pSlots:     hex(pSlots),
                        capacity:   capacity,
                    });
                } catch (e) {
                    emit("Bank_Deserialize", { err: String(e) });
                }
            },
        });
    }

    // ── _Globals::TriggerBankSample @ 0x00422430
    // signature: void __cdecl(uchar category, int bankPtr, int slot,
    //                         int attenDb, void* evtTgt, char loopArm)
    //   bankPtr==0 means "use default bank = *(g_pApp + 0x4c0)".
    //
    // g_pAppAddr is set from G_APP_ADDR_HARDCODED above; we don't re-scan
    // inside the hook (the scan ran once at startup and either confirmed
    // the hardcode or logged a warning).
    {
        // No-op: g_pAppAddr already set in the validation block above.

        Interceptor.attach(ADDR.TriggerBankSample, {
            onEnter(args) {
                try {
                    this.category = args[0].toInt32();
                    this.bank     = args[1];
                    this.slot     = args[2].toInt32();
                    this.attenDb  = args[3].toInt32();
                    this.evtTgt   = args[4];
                    this.loopArm  = args[5].toInt32();

                    // Resolve bankPtr=0 to the live default bank pointer.
                    // Two strategies, in order:
                    //   1. Direct: read *(g_pApp + 0x4c0) if we found g_pApp.
                    //   2. Fallback: use the first 41-slot bank we saw in
                    //      Bank_Deserialize (the audio engine materialises
                    //      it at startup, before the menu bank is loaded).
                    let effectiveBank = this.bank;
                    let resolvedVia = "as-passed";
                    if (effectiveBank.isNull()) {
                        if (g_pAppAddr) {
                            const liveDefault = resolveDefaultBankPtr();
                            if (liveDefault) {
                                effectiveBank = liveDefault;
                                resolvedVia = "g_pApp+0x4c0";
                            }
                        }
                        if (effectiveBank.isNull() && lastBigBankPtr) {
                            effectiveBank = lastBigBankPtr;
                            resolvedVia = "lastBigBankFallback";
                        }
                    }
                    const bankId = bankIdHint(effectiveBank);
                    const meta   = bankId ? KNOWN_BANKS[bankId] : null;
                    // For the big 41-slot bank we never have an explicit
                    // bankId, but we know it's the default_ingame bank.
                    let slotWav = null;
                    let slotNameOut = slotName(this.slot);
                    let bankNameOut = meta ? meta.name : null;
                    if (meta) {
                        slotWav = meta.wavDir + "/sample_" +
                                  String(this.slot).padStart(2, "0") + ".wav";
                    } else if (effectiveBank.equals(lastBigBankPtr)) {
                        // Untagged 41-slot bank — assume it's 65874.
                        bankNameOut = "default_ingame";
                        slotWav = "extracted_all_samples/bank_65874/sample_" +
                                  String(this.slot).padStart(2, "0") + ".wav";
                    }

                    emit("TriggerBankSample", {
                        category:    this.category,
                        bankPtr:     hex(this.bank),
                        bankPtrEff:  hex(effectiveBank),
                        bankPtrRes:  resolvedVia,
                        bankId:      bankId,
                        bankName:    bankNameOut,
                        slot:        this.slot,
                        slotName:    slotNameOut,
                        slotWav:     slotWav,
                        attenDb:     this.attenDb,
                        evtTgt:      hex(this.evtTgt),
                        loopArm:     this.loopArm,
                        triggerKind: classifyTrigger(this),
                    });
                } catch (e) {
                    emit("TriggerBankSample", { err: String(e) });
                }
            },
        });
    }

    function classifyTrigger(t) {
        // Cheap heuristic — refine by caller in OnEnter of higher hooks.
        const s = t.slot;
        if (s === 0x24) return "thud";
        if (s === 0x27) return "quip_special";
        if (s >= 1 && s <= 12) return "quip_table";
        if (s === 0x00 || (s >= 0x18 && s <= 0x1c)) return "menu";
        if (s >= 0 && s <= 40) return "pickup_or_other";
        return "unknown";
    }

    // ── CShot_ResolveHit @ 0x0041dd70
    // signature: void __thiscall(CShot* this, int hitEntity, void* pGaming)
    //   `this` is in ECX.  hitEntity is the bulanek* that took the hit
    //   (or 0xffffffff for world geometry / the special owner-self case).
    {
        Interceptor.attach(ADDR.ShotResolveHit, {
            onEnter(args) {
                try {
                    this.shot   = ptr(this.context.ecx);
                    this.hit    = args[0];
                    this.gaming = args[1];
                    // CShot fields per decomp:
                    //   +0xa4 bDirection, +0xa5 bOwnerSlotId, +0xa6 bWeaponStrength
                    //   +0x70 bHitStun (shot's own stun)
                    //   +0x84 owner entity pointer
                    const s = this.shot;
                    emit("Shot_ResolveHit", {
                        shot: {
                            ptr:             hex(s),
                            direction:       rU8(s, 0xa4),
                            ownerSlot:       rU8(s, 0xa5),
                            weaponStrength:  rU8(s, 0xa6),
                            hitStun:         rU8(s, 0x70),
                            owner:           hex(rPtr(s, 0x84)),
                        },
                        victim: dumpCompact(this.hit),
                    });
                } catch (e) {
                    emit("Shot_ResolveHit", { err: String(e) });
                }
            },
        });
    }

    // ── CBulanek_OnTakeDamage @ 0x0041db00
    // signature: uint __thiscall(CBulanek* this, int, int, char)
    //   `this` is in ECX.  The third stack arg is a single `char`, not an
    //   int (read as U8).
    {
        Interceptor.attach(ADDR.OnTakeDamage, {
            onEnter(args) {
                try {
                    this.this_ = ptr(this.context.ecx);
                    this.a1    = args[0].toInt32();
                    this.a2    = args[1].toInt32();
                    this.a3    = args[2].toUInt32() & 0xff;   // char, value lives in the arg slot
                    this.pre   = dumpCompact(this.this_);
                    // The 5 guards, re-derived from the OnTakeDamage asm:
                    //   CMP dword [ESI+0xFC], 0    ; pCorpseAnim  (must be NULL)
                    //   CMP byte  [ESI+0x16A], 0    ; bHitStun     (must be 0)
                    //   MOV ECX, [ESI+0x84]; TEST ECX, ECX; JZ skip
                    //                                  pGamingHost (must be non-NULL)
                    //   MOV AL, [ECX+0x44]; NOT AL; SHR AL, 4; TEST AL, 1
                    //                                  gamingHost.dwView_flags
                    //                                  paused == (((~flags)>>4)&1)
                    //                                  == (flags & 0x10) == 0
                    //                                  i.e. paused when BIT 4 is CLEAR
                    //   TEST byte [ESI+0x44], 1      ; this.dwView_flags
                    //                                  live bit = flags & 1
                    //
                    // Previous version of the script had two bugs here:
                    //   1. matchNotPaused was checked as `bit 4 == 0` (paused)
                    //      instead of `bit 4 == 1` (not paused) — this made
                    //      every real running match look paused.
                    //   2. hasGamingHost was checked against the hex-string
                    //      pGamingHost in the compact dump, which was always
                    //      a non-null string, so the check was always true.
                    //      Now we read the raw pointer and check `.isNull()`.
                    const corpsePtr = rPtr(this.this_, 0xFC);
                    this.guardAlive     = !!(corpsePtr) && corpsePtr.isNull();   // true if pCorpseAnim is NULL
                    const hitStun       = rU8(this.this_, 0x16A);
                    this.guardNotStun   = (hitStun === 0);
                    const hostPtr       = rPtr(this.this_, 0x84);
                    this.guardHasHost   = !!(hostPtr) && !hostPtr.isNull();
                    this.guardHostNotPaused = null;
                    if (this.guardHasHost) {
                        const flags = rU32(hostPtr, 0x44);
                        this.guardHostNotPaused = (flags === null) ? null : ((flags & 0x10) !== 0);
                    }
                    this.guardLiveFlag  = null;
                    if (this.this_) {
                        const own = rU32(this.this_, 0x44);
                        this.guardLiveFlag = (own === null) ? null : ((own & 1) === 1);
                    }
                } catch (e) {
                    this.this_ = null;
                    this.pre   = { err: String(e) };
                }
            },
            onLeave(retval) {
                try {
                    const post = this.this_ ? dumpCompact(this.this_) : null;
                    const conditionsMet =
                        this.guardAlive === true &&
                        this.guardNotStun === true &&
                        this.guardHasHost === true &&
                        this.guardHostNotPaused === true &&
                        this.guardLiveFlag === true;
                    emit("OnTakeDamage", {
                        arg1: this.a1, arg2: this.a2, arg3: this.a3,
                        retval: retval.toInt32(),
                        pre: this.pre,
                        post: post,
                        conditionsMet: conditionsMet,
                        guards: {
                            alive:           this.guardAlive,
                            notInStun:       this.guardNotStun,
                            hasGamingHost:   this.guardHasHost,
                            matchNotPaused:  this.guardHostNotPaused,
                            playerLiveFlag:  this.guardLiveFlag,
                        },
                    });
                    // Score tracking — emit on change, with yellow terminal line.
                    // nLivesRemaining is game-mode dependent (see CBulanekCtor
                    // asm at 0x41e775-0x41e7a2: lives=1 deathmatch, =3 tournament, etc.)
                    maybeEmitScore(snapshotPlayer(this.this_));
                } catch (e) {
                    emit("OnTakeDamage", { err: String(e) });
                }
            },
        });
    }

    // ── CBulanek_GetHitQuipSlot @ 0x004166b0
    // signature: int __fastcall(CBulanek* this) — this in ECX
    {
        Interceptor.attach(ADDR.GetHitQuipSlot, {
            onEnter(args) {
                try {
                    this.this_   = ptr(this.context.ecx);
                    this.bulanek = dumpCompact(this.this_);
                } catch (e) { this.bulanek = { err: String(e) }; }
            },
            onLeave(retval) {
                try {
                    const slot = retval.toInt32();
                    let reason = "table_lookup";
                    if (this.bulanek && this.bulanek.slot !== null) {
                        const sk = this.bulanek.slot;
                        if (sk >= 0x20 && sk <= 0x23) reason = "special_slot";
                    }
                    // Learn the table on first use (so we don't try to
                    // read 0x004aef88 before the binary's been mapped in
                    // some odd startup cases).
                    if (Object.keys(QUIP_SLOT_BY_KILLS).length === 0) {
                        learnQuipTable();
                    }
                    // The asm reads *(byte *)(+0x124), which is bSlotKind
                    // (per the ghidra CBulanek struct).  For human players
                    // bSlotKind = slotKind (0..3); for AI bots the ctor
                    // does `bSlotKind = slotKind & 3` (and forces 0 -> 3).
                    // So the byte is always 0..3, and only the first
                    // 4 entries of the table at 0x4aef88 are reachable
                    // in practice (entries 4..11 are dead code from the
                    // data section that follows).
                    const slotKind = safe(() => ptr(this.this_).add(0x124).readU8(), null);
                    const expectedBySlot = (slotKind !== null && slotKind < QUIP_TABLE_ENTRIES)
                        ? (QUIP_SLOT_BY_KILLS[slotKind] ?? null)
                        : null;
                    emit("GetHitQuipSlot", {
                        bulanek: this.bulanek,
                        bSlotKind: slotKind,
                        slot: slot,
                        slotName: slotName(slot),
                        slotWav:  lastBigBankPtr
                            ? "extracted_all_samples/bank_65874/sample_" +
                              String(slot).padStart(2, "0") + ".wav"
                            : null,
                        reason: reason,
                        expectedBySlot: expectedBySlot,
                        tableMatch: (expectedBySlot !== null) ? (expectedBySlot === slot) : null,
                        tableLearned: Object.keys(QUIP_SLOT_BY_KILLS).length,
                    });
                    // Update the per-player scoreboard too (in case the kill
                    // landed between OnTakeDamage and this quip-table lookup,
                    // and we didn't see the score-change event yet).
                    maybeEmitScore(snapshotPlayer(this.this_));
                } catch (e) {
                    emit("GetHitQuipSlot", { err: String(e) });
                }
            },
        });
    }

    // ───────────────────────────── exception handler
    // If a hook ever lands on a stale offset and we crash, this turns it
    // into a JSONL record instead of a bare "Process terminated".
    Process.setExceptionHandler(function (details) {
        emit("exception", {
            type: details.type,
            address: hex(details.address),
            context: {
                eip: hex(details.context.eip || ptr(0)),
                esp: hex(details.context.esp || ptr(0)),
                eax: hex(details.context.eax || ptr(0)),
                ecx: hex(details.context.ecx || ptr(0)),
                edx: hex(details.context.edx || ptr(0)),
            },
        });
    });

    // ───────────────────────────── RPC exports
    // Useful for `frida -H ... --eval`. Lets you probe live state from
    // the host without restarting the game.
    rpc.exports = {
        dumpLive: function () {
            // Without a reliable way to enumerate all live players from
            // here, this returns the bank registry + the elapsed-ms
            // counter, which is enough to confirm timing.
            const out = {
                t: nowMs(),
                banks: Array.from(bankPtrToId.entries()).map(([ptr, id]) => ({
                    ptr, id, name: (KNOWN_BANKS[id] || {}).name || null,
                    file: (KNOWN_BANKS[id] || {}).file || null,
                })),
                scoreboard: dumpPlayerScores(),
            };
            return JSON.stringify(out, null, 2);
        },
        scoreboard: function () {
            // Return every tracked player's current score, sorted by
            // score (kills - deaths) descending.
            const rows = dumpPlayerScores().map(r => ({
                ...r,
                score: (r.kills|0) - (r.deaths|0),
            }));
            rows.sort((a, b) => b.score - a.score);
            return JSON.stringify(rows, null, 2);
        },
        banks: function () { return JSON.stringify(KNOWN_BANKS, null, 2); },
        quipTable: function () { return JSON.stringify(QUIP_SLOT_BY_KILLS, null, 2); },
        setNLives: function (n, slot) {
            // n   : number  -> force nLives to this value (e.g. 1=deathmatch,
            //                  3=tournament, 5=long tournament)
            //        null    -> disable override (use game default)
            // slot: optional 0..3 to scope the override to one player;
            //        null/undefined = all players
            if (n === null || n === undefined || n === "null") {
                nLivesOverride = null;
                nLivesOverrideSlot = null;
                return JSON.stringify({ active: false });
            }
            const nNum = Number(n);
            if (!Number.isFinite(nNum)) {
                return JSON.stringify({ error: "n must be a number or null" });
            }
            nLivesOverride = nNum;
            nLivesOverrideSlot = (slot === null || slot === undefined)
                ? null : Number(slot);
            return JSON.stringify({
                active: true,
                nLives: nLivesOverride,
                slot:  nLivesOverrideSlot,
                appliedSoFar: nLivesTrace.overridden,
            });
        },
        nLivesStatus: function () {
            return JSON.stringify({
                override:        nLivesOverride,
                overrideSlot:    nLivesOverrideSlot,
                totalCtorCalls:  nLivesTrace.total,
                overriddenCalls: nLivesTrace.overridden,
                bySlot:          nLivesTrace.bySlot,
            }, null, 2);
        },
        gameModes: function () {
            // Return the catalog of the 3 game modes the binary supports.
            // (No scan — these are static from .rdata string offsets.)
            return JSON.stringify(KNOWN_GAME_MODES, null, 2);
        },
        die: function () { Interceptor.detachAll(); Process.exit(0); },
    };

    // ── CBulanekCtor @ 0x0041e4b0  (Option A)
    // OnEnter: optionally rewrite args[5] (nLives) before the ctor body
    //          stores it into this->nLivesRemaining @ +0x18C.
    // onLeave : emit a `ctor` event with the nLives the ctor actually
    //           committed, so the trace shows whether the override hit.
    {
        Interceptor.attach(ADDR.CBulanekCtor, {
            onEnter(args) {
                this.this_      = this.context.ecx;
                this.slotKind   = safe(() => ptr(this.this_).add(0x124).readU8(), null);
                this.nLivesOrig = safe(() => args[5].toInt32(), null);
                this.nLivesUsed = this.nLivesOrig;
                nLivesTrace.total++;
                nLivesTrace.bySlot[this.slotKind] =
                    (nLivesTrace.bySlot[this.slotKind] || 0) + 1;

                // Apply override if active and (slot matches or no slot filter)
                const slotFilterOk = (nLivesOverrideSlot === null) ||
                                     (this.slotKind === nLivesOverrideSlot);
                if (nLivesOverride !== null && slotFilterOk && this.nLivesOrig !== null) {
                    args[5] = ptr(nLivesOverride);
                    this.nLivesUsed = nLivesOverride;
                    nLivesTrace.overridden++;
                }
            },
            onLeave(retval) {
                try {
                    const committed = safe(() => ptr(this.this_).add(0x18C).readS16(), null);
                    emit("ctor", {
                        ptr:          this.this_ ? this.this_.toString() : null,
                        slotKind:     this.slotKind,
                        nLivesArg:    this.nLivesOrig,
                        nLivesUsed:   this.nLivesUsed,
                        nLivesCommit: committed,    // 0x18C after ctor body
                        override:     nLivesOverride,
                        overrideHit:  this.nLivesUsed !== this.nLivesOrig,
                    });
                } catch (e) {
                    emit("ctor", { err: String(e) });
                }
            },
        });
    }

    // ───────────────────────────── PRNG seed tracking (Option D in PRNG analysis)
    //
    // The game's PRNG is seeded twice:
    //   1) App launch (CBulanci_OnCreate @ 0x402b20): srand(PackedTimeToMs(BuildLocalDateTime()))
    //      = srand(ms_since_midnight_localtime)
    //   2) Match start (CGaming_ctor @ 0x41ff90): srand(*(ownerGame + 0xd4))
    //      = srand(CGame.chain.bPad_end)
    //
    // The +0xd4 field is set to g_dwElapsedMs at CMenu_OpenNetworkSession @ 0x414f31
    // (the "Start Game" button). For multiplayer, it's synced from the host's
    // message offset 0x1B in CGame_ProcessNetMessage.
    //
    // We hook all three to log and compare.

    function readElapsedMs() {
        try { return ADDR.g_dwElapsedMs.readU32(); } catch (e) { return null; }
    }
    function readTimeGetTime() {
        // timeGetTime is a WINMM.dll export, returns ms since system boot
        try {
            const w = Process.findModuleByName("winmm.dll");
            if (!w) return null;
            const f = w.findExportByName("timeGetTime");
            if (!f) return null;
            const fn = new NativeFunction(f, "uint32", []);
            return fn();
        } catch (e) { return null; }
    }
    function readValueAt0xd4(ptr) {
        try { return ptr.add(0xd4).readU32(); } catch (e) { return null; }
    }
    const seedTrace = { events: [] };

    // ── _srand @ 0x4477df — log every seed
    {
        Interceptor.attach(ADDR._srand, {
            onEnter(args) {
                const seed = args[0].toUInt32();
                const elapsedMs = readElapsedMs();
                const tgm = readTimeGetTime();
                const evt = {
                    t:         nowMs(),
                    where:     "_srand",
                    seed:      seed,
                    elapsedMs: elapsedMs,                      // g_dwElapsedMs
                    timeGetTime:tgm,                          // Windows system uptime ms
                    tgmMod24h: tgm !== null ? (tgm % 86400000) : null,  // ms since midnight
                    tgmAsUptimeMod24h: tgm !== null ? formatUptime(tgm) : null,
                };
                seedTrace.events.push(evt);
                emit("seed", evt);
                const YEL = "\x1b[33m", RST = "\x1b[0m";
                console.log(`${YEL}[SEED] _srand(0x${seed.toString(16).padStart(8, "0")})  ` +
                        `g_dwElapsedMs=${elapsedMs}  timeGetTime=${tgm} (mod24h=${evt.tgmMod24h}, ${evt.tgmAsUptimeMod24h})${RST}`);
            },
        });
    }

    // ── CBulanci_OnCreate @ 0x402b20 — log app-launch seed site
    {
        Interceptor.attach(ADDR.CBulanci_OnCreate, {
            onEnter(args) {
                const elapsedMs = readElapsedMs();
                const tgm = readTimeGetTime();
                const evt = {
                    t:         nowMs(),
                    where:     "CBulanci_OnCreate.before_srand",
                    elapsedMs: elapsedMs,
                    timeGetTime: tgm,
                    tgmMod24h: tgm !== null ? (tgm % 86400000) : null,
                    tgmAsUptimeMod24h: tgm !== null ? formatUptime(tgm) : null,
                };
                seedTrace.events.push(evt);
                emit("seed", evt);
                const YEL = "\x1b[33m", RST = "\x1b[0m";
                console.log(`${YEL}[SEED] app-launch path about to call srand  ` +
                        `g_dwElapsedMs=${elapsedMs}  timeGetTime=${tgm} (${evt.tgmAsUptimeMod24h})${RST}`);
            },
        });
    }

    // ── CGaming_ctor @ 0x41ff90 — log match-start seed site, read CGame+0xd4
    //
    // The srand call inside this function reads from [this+0x84]+0xd4, where `this`
    // is in ECX (thiscall convention) and this+0x84 is a field holding the owner
    // game pointer.  In Frida, `args[]` is the stack-only — `this` lives in
    // `this.context.ecx`, NOT in args[0].  Bug-fix: use ecx for `this`, args[0]
    // for the explicit `ownerGame` parameter.
    {
        Interceptor.attach(ADDR.CGaming_ctor, {
            onEnter(args) {
                this.this_      = this.context.ecx;   // CGaming* (the one being constructed)
                this.ownerGame  = args[0];            // CGame* (first stack arg = "ownerGame" param)
                // The asm reads from [this+0x84] which is an inner pointer to the
                // owner game.  Read BOTH: the inner ptr field and its +0xd4, AND
                // the direct ownerGame param + 0xd4, in case either is the source.
                const ownerGameViaInnerPtr = safe(() =>
                    this.this_.add(0x84).readPointer(), null);
                const seedFromInnerPtr      = safe(() =>
                    ownerGameViaInnerPtr.add(0xd4).readU32(), null);
                const seedFromOwnerArg       = readValueAt0xd4(this.ownerGame);
                const seedFromThisDirect     = safe(() =>
                    this.this_.add(0xd4).readU32(), null);
                const elapsedMs = readElapsedMs();
                const tgm       = readTimeGetTime();
                const evt = {
                    t:         nowMs(),
                    where:     "CGaming_ctor.before_srand",
                    thisPtr:        this.this_ ? this.this_.toString() : null,
                    ownerArgPtr:    this.ownerGame ? this.ownerGame.toString() : null,
                    ownerGameViaInnerPtr: ownerGameViaInnerPtr ? ownerGameViaInnerPtr.toString() : null,
                    seedFromInnerPtr:     seedFromInnerPtr,
                    seedFromOwnerArg:      seedFromOwnerArg,
                    seedFromThisDirect:    seedFromThisDirect,
                    elapsedMs: elapsedMs,
                    timeGetTime: tgm,
                };
                seedTrace.events.push(evt);
                emit("seed", evt);
                const YEL = "\x1b[33m", RST = "\x1b[0m";
                console.log(`${YEL}[SEED] match-start path  ` +
                    `[this+0x84]+0xd4=0x${(seedFromInnerPtr||0).toString(16).padStart(8,"0")}  ` +
                    `args[0]+0xd4=0x${(seedFromOwnerArg||0).toString(16).padStart(8,"0")}  ` +
                    `this+0xd4=0x${(seedFromThisDirect||0).toString(16).padStart(8,"0")}  ` +
                    `g_dwElapsedMs=${elapsedMs}  timeGetTime=${tgm}${RST}`);
            },
        });
    }

    // ── CMenu_OpenNetworkSession @ 0x414f31 — log "Start Game" +0xd4 write
    //
    // __fastcall with `this` in ECX.  args[0] is NOT this.
    {
        Interceptor.attach(ADDR.CMenu_OpenNetworkSession, {
            onEnter(args) {
                this.this_ = this.context.ecx;   // CGame* (this is in ECX for __fastcall)
                this.before = readValueAt0xd4(this.this_);
                this.beforeElapsedMs = readElapsedMs();
                this.beforeTgm = readTimeGetTime();
            },
            onLeave(retval) {
                try {
                    const after = readValueAt0xd4(this.this_);
                    const afterElapsedMs = readElapsedMs();
                    const afterTgm = readTimeGetTime();
                    const evt = {
                        t:        nowMs(),
                        where:    "CMenu_OpenNetworkSession",
                        thisPtr:  this.this_ ? this.this_.toString() : null,
                        before0xd4: this.before,
                        after0xd4:  after,
                        delta:      (after !== null && this.before !== null) ? (after - this.before) : null,
                        beforeElapsedMs: this.beforeElapsedMs,
                        afterElapsedMs:  afterElapsedMs,
                        elapsedMsDelta:  (afterElapsedMs !== null && this.beforeElapsedMs !== null)
                                          ? (afterElapsedMs - this.beforeElapsedMs) : null,
                        beforeTgm: this.beforeTgm,
                        afterTgm:  afterTgm,
                        // The seed is captured at function ENTRY: g_dwElapsedMs is
                        // read at the top of CMenu_OpenNetworkSession and stored
                        // to +0xd4 before the function does any other work.
                        // So the correct comparison is: after0xd4 === beforeElapsedMs.
                        matchesG_dwElapsedMs: (after === this.beforeElapsedMs),
                    };
                    seedTrace.events.push(evt);
                    emit("seed", evt);
                    const YEL = "\x1b[33m", RST = "\x1b[0m";
                    console.log(`${YEL}[SEED] CMenu_OpenNetworkSession  this+0xd4: 0x${(this.before||0).toString(16)} -> 0x${(after||0).toString(16)}  ` +
                        `seed=0x${(after||0).toString(16)} (==g_dwElapsedMs@entry=0x${(this.beforeElapsedMs||0).toString(16)})  ` +
                        `match?${evt.matchesG_dwElapsedMs}${RST}`);
                } catch (e) {
                    emit("seed", { where: "CMenu_OpenNetworkSession", err: String(e) });
                }
            },
        });
    }

    function formatUptime(tgm) {
        if (tgm === null) return null;
        const ms   = tgm % 86400000;
        const h    = Math.floor(ms / 3600000);
        const m    = Math.floor((ms % 3600000) / 60000);
        const s    = Math.floor((ms % 60000) / 1000);
        const mil  = ms % 1000;
        return `${String(h).padStart(2,"0")}:${String(m).padStart(2,"0")}:${String(s).padStart(2,"0")}.${String(mil).padStart(3,"0")}`;
    }

    // ───────────────────────────── MemoryAccessMonitor on CGame+0xd4
    //
    // We need to find the single-player +0xd4 writer.  Strategy:
    //   1. Hook CGame_StartGame onEnter → grab the CGame* (the param/this).
    //   2. Install MemoryAccessMonitor on (CGame* + 0xd4, 4 bytes), write-only.
    //   3. The monitor fires on EVERY write to +0xd4; we log details.from
    //      (writing instruction's address) and the value being written.
    //   4. Hook CGaming_ctor onEnter → disable the monitor (we already saw the seed).
    //   5. Backstop: if the CGame* is null, also try the g_pApp+0x284 location
    //      (the CGame embedded in the main CBulanci).
    //
    // `details.from` is the return address from the writing instruction,
    // so we can look that up in ghidra to find the calling function.
    let seedMonitor = null;  // {monitor, pGame, pTarget}
    function startSeedMonitor(pGame) {
        if (!pGame || pGame.isNull()) return;
        const pTarget = pGame.add(0xd4);
        try {
            // Frida 17.x: MemoryAccessMonitor may take a plain function as the
            // third arg instead of an object with onAccess.  Try the function form
            // first; fall back to the object form on type error.
            const cb = function (details) {
                if (details.operation !== 'write') return;
                const val = safe(() => details.address.readU32(), null);
                const fromAddr = details.from
                    ? "0x" + details.from.toString(16).padStart(8, "0")
                    : "unknown";
                const targetAddr = "0x" + details.address.toString(16).padStart(8, "0");
                const pGameAddr = "0x" + pGame.toString(16).padStart(8, "0");
                const evt = {
                    t:         nowMs(),
                    where:     "MemAccessMonitor",
                    pGame:     pGameAddr,
                    pTarget:   targetAddr,
                    offset:    "0xd4",
                    from:      fromAddr,
                    value:     val,
                    elapsedMs: readElapsedMs(),
                };
                seedTrace.events.push(evt);
                emit("seed", evt);
                const YEL = "\x1b[33m", RST = "\x1b[0m";
                console.log(`${YEL}[SEED-WRITE] pGame=${pGameAddr} ${targetAddr} (+0xd4) <- 0x${(val||0).toString(16).padStart(8,"0")}  ` +
                    `from=${fromAddr}  g_dwElapsedMs=${evt.elapsedMs}${RST}`);
            };
            let mon;
            try {
                mon = MemoryAccessMonitor.enable(
                    { base: pTarget, size: 4 },
                    { write: true },
                    cb
                );
            } catch (e1) {
                // Try the object form as a fallback
                mon = MemoryAccessMonitor.enable(
                    { base: pTarget, size: 4 },
                    { write: true },
                    { onAccess: cb }
                );
            }
            seedMonitor = { monitor: mon, pGame: pGame, pTarget: pTarget };
            const YEL = "\x1b[33m", RST = "\x1b[0m";
            console.log(`${YEL}[SEED-MONITOR] installed on pGame=${pGame} +0xd4 (4 bytes)${RST}`);
        } catch (e) {
            console.log(`[SEED-MONITOR] install failed: ${e}`);
            console.log("[SEED-MONITOR] fallback: hooking System breakpoint — write Stalker if Frida MAM unavailable");
        }
    }
    function stopSeedMonitor() {
        if (!seedMonitor) return;
        try {
            MemoryAccessMonitor.disable(seedMonitor.monitor);
            const YEL = "\x1b[33m", RST = "\x1b[0m";
            console.log(`${YEL}[SEED-MONITOR] disabled (saw CGame+0xd4 = 0x${safe(() => seedMonitor.pTarget.readU32().toString(16), "???")})${RST}`);
        } catch (e) {
            console.log(`[SEED-MONITOR] disable failed: ${e}`);
        }
        seedMonitor = null;
    }

    // ── CGame_StartGame @ 0x413ce0 — install seed monitor on entry, uninstall
    //    at CGaming_ctor so we catch only the writes between "Start" and match load.
    {
        Interceptor.attach(ADDR.CGame_StartGame, {
            onEnter(args) {
                // __thiscall: `this` is in ECX, args[0..] are stack args.
                // CGame_StartGame has signature void(CGame *this) — no other args.
                const pThis = this.context.ecx;   // CGame*
                this.pThis = pThis;
                const beforeVal = safe(() => pThis.add(0xd4).readU32(), null);
                const beforeElapsedMs = readElapsedMs();
                const YEL = "\x1b[33m", RST = "\x1b[0m";
                console.log(`${YEL}[SEED] CGame_StartGame ENTER  this=${pThis}  ` +
                    `this+0xd4(before)=0x${(beforeVal||0).toString(16).padStart(8,"0")}  ` +
                    `g_dwElapsedMs=${beforeElapsedMs}${RST}`);
                emit("seed", {
                    t:        nowMs(),
                    where:    "CGame_StartGame.enter",
                    pThis:    pThis ? pThis.toString() : null,
                    beforeVal: beforeVal,
                    elapsedMs: beforeElapsedMs,
                });
                // Install the monitor — fires on any write to this+0xd4.
                startSeedMonitor(pThis);
            },
            onLeave(retval) {
                // After CGame_StartGame returns, the CGaming_ctor has been called
                // and the seed has been read.  Disable the monitor so we don't
                // get spammed with per-frame writes to other +0xd4 fields.
                stopSeedMonitor();
            },
        });
    }

    emit("ready", {
        module: MOD.name,
        base:   hex(MOD.base),
        slide:  SLIDE.toInt32(),
        bankCatalog: Object.keys(KNOWN_BANKS).length,
        knownSlots:  Object.keys(SLOT_LABEL).length,
    });
})();
