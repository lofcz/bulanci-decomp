# `bulanci_audio_logger.js` — Frida tracer for Bulanci hit / kill sounds

This script verifies the reverse-engineering findings about how
Bulanci plays hit / kill samples from a soundbank. It hooks every entry
point of the audio pipeline and prints a JSONL record per event with
enough context (player state, slot index, bank id, wav filename) to
reproduce the entire chain from a recorded run.

## What it proves

| RE finding (from `unpacked/overlay` + Ghidra decomp) | What the script shows |
|---|---|
| Default in-game bank is `res_0000065874_43_AudioBank.bin` (resource 0x10152) | `SetMusicAndSound { bankIndexRes: 65874, bankFile: "res_0000065874_43_AudioBank.bin" }` followed by `Bank_Deserialize { bankPtr, slotCount: 41 }` |
| Bullet hit always plays slot **0x24** (the thud, `sample_36.wav`) | `Shot_ResolveHit` → `OnTakeDamage { conditionsMet: true }` → `TriggerBankSample { slot: 36, slotName: "HIT_THUD", slotWav: ".../sample_36.wav" }` |
| Hit quip slot is `0x27` for special player slots `[0x20..0x23]`, else `table[killCount]` from `0x004aef88` | `GetHitQuipSlot { reason: "special_slot" \| "table_lookup", slot, tableMatch: true/false }` |
| Five guards gate the thud (`pCorpseAnim==NULL`, `bHitStun==0`, `pGamingHost!=NULL`, host not paused, player live flag) | `OnTakeDamage.guards: { alive, notInStun, hasGamingHost, matchNotPaused, playerLiveFlag }` and `conditionsMet: bool` |
| Quip is fired later from `CBulanek_OnEvent` case `0xdb` | Look for `TriggerBankSample { triggerKind: "quip_table" \| "quip_special", evtTgt: non-null }` after a prior `OnTakeDamage` event for the same victim |

The script does **not** modify game state — it's a passive observer.
You can chain it with the existing `menu_hover_audio_trace.js` or
`patch_window.js` for full coverage.

## Running

### One-liner, PowerShell

```powershell
# orig build, no output file (Frida CLI prints to stdout)
.\scripts\frida\run_audio_logger.ps1

# instrumented build, redirected JSONL
.\scripts\frida\run_audio_logger.ps1 -Target instrumented -OutputFile hit.jsonl

# chain with menu hover trace
.\scripts\frida\run_audio_logger.ps1 `
    -Target orig `
    -OutputFile hit.jsonl `
    -l .\scripts\frida\menu_hover_audio_trace.js
```

### One-liner, cmd

```bat
scripts\frida\run_audio_logger.cmd
scripts\frida\run_audio_logger.cmd instrumented hit.jsonl
scripts\frida\run_audio_logger.cmd orig hit.jsonl -l scripts\frida\menu_hover_audio_trace.js
```

### Manual `frida` invocation

If you'd rather wire it yourself:

```powershell
frida -l scripts\frida\bulanci_audio_logger.js `
      -f orig\bulanci.exe `
      -o hit_audio.jsonl
```

If Frida doesn't auto-resume, type `%resume` at the prompt. To detach
cleanly mid-session, send `%` and the JSONL stream will flush.

### RPC exports (interactive)

At the Frida REPL, type:

```
rpc.exports.dumpLive()    # show bank registry + game clock
rpc.exports.banks()       # the bank catalog the script knows
rpc.exports.quipTable()   # the killCount → quipSlot table
rpc.exports.die()         # detach + exit the game
```

## Output schema

Every line is a self-contained JSON object. Common fields:

- `n`  — monotonic sequence
- `t`  — game-clock millisecond (from `g_dwElapsedMs` at `0x004b3bd8`); `null` if the global isn't readable yet
- `ev` — event name

Per-event payload (most relevant fields):

| Event | Key fields |
|---|---|
| `ready` | `module`, `base`, `slide`, `bankCatalog` |
| `SetMusicAndSound` | `mp3Res`, `bankIndexRes`, `bankId`, `bankName`, `bankFile`, `bankPtr`, `prevBankPtr` |
| `Bank_Deserialize` | `bankPtr`, `parentRef`, `slotVec`, `slotCount` |
| `Shot_ResolveHit` | `shot { weaponStrength, ownerSlot, direction, hitStun, owner }`, `victim { ptr, slot, lives, kills, deaths, hitStun, alive }` |
| `OnTakeDamage` | `arg1..arg3`, `retval`, `pre`, `post`, `guards { alive, notInStun, hasGamingHost, matchNotPaused, playerLiveFlag }`, `conditionsMet` |
| `GetHitQuipSlot` | `bulanek`, `slot`, `slotName`, `reason: "special_slot"\|"table_lookup"`, `expectedByKills`, `tableMatch` |
| `TriggerBankSample` | `category`, `bankPtr`, `bankId`, `bankName`, `slot`, `slotName`, `slotWav`, `attenDb`, `evtTgt`, `loopArm`, `triggerKind: "thud"\|"quip_special"\|"quip_table"\|"menu"\|"pickup_or_other"\|"unknown"` |
| `exception` | `type`, `address`, `context { eip, esp, eax, ecx, edx }` |

## Verification checklist

Run through a few rounds in single-player (or with a friend), then
`grep` the JSONL to confirm each finding:

```bash
# 1. The bank loads exactly once on match start.
grep -c '"ev":"SetMusicAndSound"' hit.jsonl
grep -c '"ev":"Bank_Deserialize"' hit.jsonl

# 2. Every OnTakeDamage that's expected to fire a thud has conditionsMet=true.
grep '"ev":"OnTakeDamage"' hit.jsonl | jq 'select(.conditionsMet==false)'

# 3. Every OnTakeDamage is followed by a HIT_THUD TriggerBankSample with
#    slot=36 and slotWav=.../sample_36.wav.
grep '"ev":"TriggerBankSample"' hit.jsonl | jq 'select(.slotName=="HIT_THUD")'

# 4. The quip slot matches the killCount table.
grep '"ev":"GetHitQuipSlot"' hit.jsonl | jq 'select(.tableMatch==false)'

# 5. Special-slot players (0x20..0x23) always get HIT_QUIP_SPECIAL.
grep '"ev":"GetHitQuipSlot"' hit.jsonl | jq 'select(.reason=="special_slot")'
```

## Notes

- Function addresses are image-relative to `0x00400000` and
  `SLIDE = MOD.base - 0x00400000` is applied at attach time. The same
  script works on both `bulanci.exe` and `bulanci_insturmented.exe`
  because the instrumented build is a recompile of the same source.

- All struct reads are wrapped in `safe(fn, fb)` so a hook never
  crashes the process because of a stale pointer. If a hook *does*
  crash, `Process.setExceptionHandler` emits a structured `exception`
  record instead of a bare "Process terminated".

- The script doesn't need any extra Windows setup beyond
  `python -m pip install --user frida-tools` (see the parent
  `README.md`). Frida injects via the standard `CreateRemoteThread`
  path, so admin isn't required.

- The bank catalog in the script is a static seed. As new bank ids
  appear in `SetMusicAndSound` (e.g. if a level script overrides the
  default bank via `SetMusic mp3, bankIndex`), the catalog grows
  automatically — the script just learns the new `bankPtr → id`
  mapping and prints the resource id and file name in every subsequent
  `TriggerBankSample` event.

## Layout

```
scripts/frida/
  README.md                    ← this file
  bulanci_audio_logger.js      ← the tracer
  run_audio_logger.cmd         ← cmd launcher
  run_audio_logger.ps1         ← PowerShell launcher
```
