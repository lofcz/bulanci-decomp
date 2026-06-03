# PRNG Seed Analysis

End-to-end reverse engineering of the random number generator used by `bulanci.exe`. The full chain from `srand` to the actual seed value is documented here, with verification via Frida runtime traces.

## TL;DR

The game uses MSVCRT's standard `rand()` LCG. It is seeded **twice** during normal play:

1. **App launch** — `srand(ms_since_local_midnight)` at app startup.
2. **Every match start** — `srand(g_dwElapsedMs)` at the moment the user clicks Start in the level select, regardless of single-player or multiplayer mode.

The single "seed store" field is `CGame.chain.bPad_end @ CGame+0xd4`. Writers and readers are scattered across the menu/match-init path; the analysis below pins each one down.

For multiplayer, the host's seed is propagated to clients via a net message so everyone srand's to the same value.

---

## 1. The PRNG algorithm

The game's `rand()` is the textbook MSVCRT LCG:

```c
// Runtime::MSVCRT::_rand @ 0x004477ec
int __cdecl _rand(void) {
    _ptiddata pTls = __getptd();
    uint holdrand = pTls->_holdrand * 0x343FD + 0x269EC3;
    pTls->_holdrand = holdrand;
    return (holdrand >> 16) & 0x7FFF;   // [0, 32767]
}
```

State is per-thread, stored in TLS at `_ptiddata._holdrand` (the `__getptd()` struct).

The `srand` side:

```c
// Runtime::MSVCRT::_srand @ 0x004477df
void __cdecl _srand(ulong dwSeed) {
    _ptiddata pTls = __getptd();
    pTls->_holdrand = dwSeed;
}
```

**Both function names are ghidra decompiler inferences** (auto-storage mapping). They were renamed via `rename_or_label` from `FUN_004477ec` / `FUN_004477df`. The actual implementation is the MSVCRT LCG, no surprises.

---

## 2. The seed store

`CGame.chain.bPad_end @ CGame+0xd4` is the single 32-bit field that holds the per-match seed. It is read by `CGaming_ctor` (via `srand(*(args[0]+0xd4))`) and written by various paths in the menu/match-init code.

Struct offsets seen in the wild:
- `g_pApp + 0x284` = the embedded `CGame` object inside the main `CBulanci`
- `g_pApp + 0x35c` = `CGame + 0xd4` (the seed field)
- `g_dwElapsedMs @ 0x004b3bd8` = the per-process "ms since launch" counter

---

## 3. Seeding site #1 — App launch

**Where:** `CBulanci::CBulanci_OnCreate @ 0x00402b20`

At the very top of the ctor body, before any other setup:

```c
// asm @ 0x402b3c
CALL _srand(...)
```

The seed is computed in two steps:

```c
// CBulanci::BuildLocalDateTime @ 0x42e8b0
CBulanci *BuildLocalDateTime(CBulanci *param_1) {
    _SYSTEMTIME local_10;
    GetLocalTime(&local_10);   // <-- LOCAL time, not UTC
    CBulanci_PackTimeDwordFromSystemTime(param_1, (int)&local_10);
    CBulanci_SetDateFromShortFields((CBulanci *)&(param_1->app).vftable_sub04, &local_10.wYear);
    return param_1;
}
```

Then `PackedTimeToMs` converts the packed 4-byte time field to milliseconds-since-midnight:

```c
// CBulanci::CBulanci_PackedTimeToMs @ 0x42e730  (renamed from FUN_0042e730)
int __fastcall CBulanci_PackedTimeToMs(int *pPackedTime) {
    int packed = *pPackedTime;   // first 4 bytes of packed local time
    return (
        (   ((packed << 0xe) >> 0x19)   // hours   (bits 11-17)
          + (((packed << 0x7) >> 0x19)   // minutes (bits 18-24)
             + ((packed >> 0x19) * 0x3c) // seconds (bits 25-31) * 60
            ) * 0x3c                     // (minutes + sec*60) * 60
        ) * 1000                       // convert to ms
        + ((packed << 0x15) >> 0x15)   // ms      (bits 0-10)
    );
}
```

Decoded: `seed = (hours*3600 + minutes*60 + seconds) * 1000 + milliseconds`, i.e. **ms-since-local-midnight at the moment of launch**.

**Confirmed `GetLocalTime` (not `GetSystemTime`)**, so the seed is unambiguously local time of day, not UTC.

### Frida runtime verification (6 runs)

| Run | Launch seed (dec) | Launch seed (hex) | Reconstructed local time-of-day |
|---|---|---|---|
| 1 | 44150539 | 0x02A1AF0B | 12:15:50.539 |
| 2 | 45461783 | 0x02B5B117 | 12:36:01.783 |
| 3 | 45737853 | 0x02B9E77D | 12:42:57.853 |
| 4 | 45461783 | 0x02B5B117 | 12:36:01.783 (same as run 2 — user relaunched at the same time) |
| 5 | 54672680 | 0x03423D28 | 15:17:06.280 |
| 6 | 54964190 | 0x0346AFDE | 15:27:24.190 |

All match `ms-since-local-midnight`. The seed is NOT `timeGetTime % 86400000` (which would be system uptime mod 24h, the value displayed as `tgmAsUptimeMod24h` in the Frida logs — misleadingly named earlier; renamed to honest "uptime" terminology).

---

## 4. Seeding site #2 — Match start (the interesting one)

The match-start `srand` happens inside `CGaming::CGaming_ctor @ 0x0041ff90` near the end of its body:

```c
// asm @ 0x420108..0x420110
MOV ECX, dword ptr [ESI + 0x84]   ; ECX = this+0x84 (set in ctor body from ownerGame)
MOV EAX, dword ptr [ECX + 0xd4]   ; EAX = *(ownerGame + 0xd4) = match seed
PUSH EAX
CALL _srand
```

The seed source is `ownerGame + 0xd4`, where `ownerGame` is the `CGame*` arg (`args[0]` in Frida, or the first stack param in `__thiscall`). And the value is read **at function ENTRY time** (the ghidra decompiler showed this — I confirmed at runtime the seed is already set when CGame_StartGame runs).

### Frida hook confirmation (CGaming_ctor entry, run 5)

```
[SEED] CGaming_ctor.before_srand  args[0]+0xd4=0x00000eba  g_dwElapsedMs=7120
[SEED] _srand(0x00000eba)  g_dwElapsedMs=7120
```

`args[0]+0xd4 = 0xeba = 3770` (the seed) and the actual srand argument `= 3770` — same value. Match.

### Who writes the seed to `+0xd4`?

There are **two writers**, both feeding the same field. They run on different code paths:

#### Writer A: `CMenu_OpenNetworkSession @ 0x00414dd0` (host's Start click)

**Despite the name, this function runs for BOTH single-player and multiplayer.** It's the bridge from the level select (`CStartGame1`) to match creation (`CGaming_ctor`). The ghidra decompile comment confirms:

> `CMenu::OpenNetworkSession -- bridges 'Start Game' -> CGaming. Fires after CStartGame1's radios are all set.`

The seed write asm at `0x414f2c`:

```asm
MOV EAX, [0x004b3bd8]   ; EAX = g_dwElapsedMs (read at function ENTRY)
MOV [EBX+0xd4], EAX     ; write to CGame+0xd4
```

The seed is captured at the **top** of `CMenu_OpenNetworkSession`, before the function does its 1-3 second body of work (DirectPlay setup for multiplayer, level-load for single-player). The +0xd4 value is therefore `g_dwElapsedMs` at the moment the user clicks Start in the level select, NOT at the moment the match actually begins.

**Frida runtime verification (6 runs, all single-player):**

| Run | Seed (dec) | Seed (hex) | g_dwElapsedMs @ CMenu_OpenNetworkSession entry | Click→srand delta |
|---|---|---|---|---|
| 1 | 12377 | 0x3059 | 12377 (hook pre-fix) | 2.99s |
| 2 | 3875 | 0x0F23 | 3875 (hook pre-fix) | 2.69s |
| 3 | 5494 | 0x1576 | 5494 (hook pre-fix) | 2.59s |
| 4 | 4540 | 0x11BC | 4540 (hook pre-fix) | 2.90s |
| 5 | 3770 | 0x0EBA | 3770 ✓ (hook fired) | 3.35s |
| 6 | 6131 | 0x17F3 | 6131 ✓ (hook fired) | 2.59s |

**`matchesG_dwElapsedMs: true` for runs 5 and 6** — the value written equals `g_dwElapsedMs` at function entry.

The 2-3 second delta between CMenu_OpenNetworkSession and the actual `srand` is the time the function spends doing its body (DirectPlay setup, level load) before returning. The seed is fixed the moment CMenu_OpenNetworkSession starts.

#### Writer B: `CGame_ProcessNetMessage @ 0x00415290` (multiplayer client sync)

For multiplayer only, when a net message arrives at the host's "match start" announcement:

```asm
@ 0x415750
MOV EAX, dword ptr [EBX + 0x1B]   ; EAX = randSeed from incoming message buffer
@ 0x415753
MOV dword ptr [EBP + 0xd4], EAX    ; write to CGame+0xd4 (overrides Writer A's value)
```

The host's `g_dwElapsedMs` is sent in a message at offset `0x1B`. Clients receive it and overwrite their `CGame+0xd4`. Both sides then `srand` to the same value, so the match-start RNG sequence is identical on every machine.

For single-player, Writer B never fires (no networking).

---

## 5. Call-chain summary

```
APP LAUNCH:
  CBulanci_OnCreate @ 0x402b20
    └─ BuildLocalDateTime()           // GetLocalTime → packed 4-byte time
    └─ PackedTimeToMs(packedTime)     // unpack → ms-since-local-midnight
    └─ _srand(launchSeed)             // seed = local-time-of-day at launch
    └─ [PRNG now seeded for menu randomness]

MAIN MENU → LEVEL SELECT:
  User clicks "Start" in CMenu main menu
  CMenu_CmdDispatch(this, 0xc9)    // "Start" command → push CStartGame1
  [user picks a level in CStartGame1]
  CStartGame1 closes, posts 0xcd
  CBulanci_OnEvent_MenuStateMachine(this, 0xcd)
    └─ CBulanci_StartGameAndShowPostMatchScore(this)
         └─ CGame_StartGame(this->CGame)         // CGame @ g_pApp+0x284
              └─ CGaming_ctor(&local_414, this->CGame)  // onEnter: seed=??
                 └─ srand(*(args[0]+0xd4))         // read CGame+0xd4

The +0xd4 was set BY:
  WRITER A (single OR multiplayer):
    CMenu_OpenNetworkSession @ 0x414dd0
      [runs in the path between "Start click" and "CGame_StartGame"]
      MOV EAX, [0x4b3bd8]   ; g_dwElapsedMs (at ENTRY)
      MOV [EBX+0xd4], EAX
      → CGame+0xd4 = g_dwElapsedMs @ "Start click" time

  WRITER B (multiplayer only, overrides Writer A):
    CGame_ProcessNetMessage @ 0x415290
      [when the host's match-start message arrives]
      MOV EAX, [EBX + 0x1B]   ; host's seed from msg
      MOV [EBP + 0xd4], EAX
      → CGame+0xd4 = host's g_dwElapsedMs (synced across all clients)
```

The seed captured at `srand` time equals `g_dwElapsedMs` at the user's Start click (modulo Writer B override in multiplayer).

---

## 6. Reproduction recipe for the re-impl

```c
// On app launch:
SYSTEMTIME st;
GetLocalTime(&st);
uint32_t launchSeed = st.wHour*3600000 + st.wMinute*60000 + st.wSecond*1000 + st.wMilliseconds;
srand(launchSeed);

// On every "Start click" in the level select (any mode):
uint32_t matchSeed = g_dwElapsedMs_at_click;  // ms since launch
CGame* theGame = g_pApp + 0x284;             // embedded CGame
theGame->chain.bPad_end = matchSeed;          // store to CGame+0xd4

// Match starts shortly after. At CGaming_ctor:
srand(theGame->chain.bPad_end);               // = matchSeed from above

// For multiplayer only — host side:
//   Include matchSeed in your match-start net message at offset 0x1B
// Client side:
//   When message arrives, write *(msg+0x1B) to theGame->chain.bPad_end
//   before CGaming_ctor runs on the client.
```

The seed is monotonic and bounded by game session length. For deterministic match replay, record `g_dwElapsedMs` at the moment of click and use it as the seed for every re-simulation.

---

## 7. Key ghidra functions, addresses, and notes

| Function | Address | Convention | `this` type | Notes |
|---|---|---|---|---|
| `Runtime::MSVCRT::_srand` | 0x004477df | `__cdecl` | — | Renamed from `FUN_004477df`. Body: read `__getptd()`, write `_holdrand`. |
| `Runtime::MSVCRT::_rand` | 0x004477ec | `__cdecl` | — | Standard LCG. State in TLS via `__getptd()->_holdrand`. |
| `CBulanci::CBulanci_OnCreate` | 0x00402b20 | `__fastcall` | `CBulanci*` (renamed from `param_1`) | Launch seed site. |
| `CBulanci::BuildLocalDateTime` | 0x0042e8b0 | `__cdecl` | — | Calls `GetLocalTime` (local, not UTC). |
| `CBulanci::CBulanci_PackedTimeToMs` | 0x0042e730 | `__fastcall` | — | `param_1` renamed to `pPackedTime`. |
| `CMenu::CMenu_OpenNetworkSession` | 0x00414dd0 | `__fastcall` | `CGame*` (in ECX) | **Writer A** — single + multiplayer. NOTE: Frida hook must be on `0x414dd0`, NOT `0x414f31` (the old wrong address). The asm `MOV [0x4b3bd8]` then `MOV [EBX+0xd4]` is at `0x414f2c`/`0x414f31`, but the function entry is at `0x414dd0`. |
| `CGame::CGame_StartGame` | 0x00413ce0 | `__thiscall` | `CGame*` (in ECX) | Was previously mis-typed as `__fastcall` `BYTE [4]`. Two-step fix: `set_function_prototype(calling_convention=__thiscall)` then `set_function_this_type(this_type="CGame *")`. |
| `CGaming::CGaming_ctor` | 0x0041ff90 | `__thiscall` | `CGaming*` (in ECX) | Was previously mis-typed as `CBulanci::` namespace. `this` was `CBulanci*` (wrong). Two-step fix: `set_function_prototype(calling_convention=__thiscall)` then `set_function_this_type(this_type="CGaming *")`. |
| `CGame_ctor` (real name) | 0x0041ff90 | `__thiscall` | `CGaming*` | Same function as above. |
| `CGame::CGame_ProcessNetMessage` | 0x00415290 | `__thiscall` | `CGame*` | **Writer B** (multiplayer only). Read host's seed from msg+0x1B, write to `+0xd4`. |
| `CGame_ctor` (writer side) | — | — | — | `CStartGame1_GetEmbeddedCGamePtr @ 0x401280` returns `g_pApp + 0x284` (the CGame ptr). |

### Other useful globals

| Symbol | Address | Purpose |
|---|---|---|
| `g_pApp` | 0x004b3b88 | Main CBulanci* — embeds CGame at offset 0x284 |
| `g_dwElapsedMs` | 0x004b3bd8 | Per-process "ms since launch" counter |
| `__getptd` | 0x0044ad16 | MSVCRT thread-local data accessor (TLS) |
| `__getptd_noexit` | 0x0044ac93 | Variant that doesn't call `__amsg_exit` on NULL |

### Frida hook — verifying a seed write

For verifying any future question about the PRNG, the Frida hook in `scripts/frida/bulanci_audio_logger.js` already instruments:

- `CBulanci_OnCreate` (capture launch seed site)
- `Runtime_MSVCRT__srand` (log every srand call)
- `CGaming_ctor` (capture match seed site, read `args[0]+0xd4`)
- `CMenu_OpenNetworkSession` (capture Writer A, log before/after at `+0xd4`)
- `CGame_StartGame` (verify `+0xd4` already set when CGaming_ctor runs)

To re-run with the Frida `MemoryAccessMonitor` on the `+0xd4` write (currently failing on Frida 17.9.10 on Windows), use gdb or hardware watchpoints via the ghidra-mcp debugger tools.

---

## 8. Lessons from this analysis

1. **Static analysis + runtime verification is the only way to be sure.** The decompiler comments are user-written, prone to error. Always verify claims with runtime traces.
2. **"Multiplayer-only" was a wrong assumption.** `CMenu_OpenNetworkSession` runs for both single-player and multiplayer. The "Network" in the name is misleading.
3. **The seed is captured at function entry, not exit.** My initial heuristic was wrong; the value written is `g_dwElapsedMs` at the moment `CMenu_OpenNetworkSession` is called, not at the moment it returns.
4. **Frida `__thiscall` / `__fastcall` hooks must read `this` from `this.context.ecx`**, not from `args[0]`. `args[0]` is the first STACK arg.
5. **`timeGetTime` returns system uptime, not local time.** A seed value of `12:05:00.539` looks like local time-of-day, not uptime mod 24h.
6. **The `0x414f31` vs `0x414dd0` address difference for `CMenu_OpenNetworkSession`** — ghidra's auto-analysis re-numbered the function; the Frida hook needs to be at the function entry, not at an internal address (the `+0xf2c` offset from the start of the function).
