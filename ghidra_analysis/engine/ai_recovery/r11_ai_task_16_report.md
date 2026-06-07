# Round 11 — AI Task 16 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 16 |
| **title** | InsertOpponent script spawn (tournament lives) |
| **archetype** | `script_spawn` |
| **seed_address** | `0x0041f730` |
| **addresses** | `0x0041f730`, `0x0041f500`, `0x0041f5d0`, `0x00416970` |
| **acceptance** | 4-arg bytecode → `CGaming_SpawnAndInitializePlayer`; slot `0x24..` auto alloc |
| **evidence** | [script_dispatch_table.md](../script_dispatch_table.md), tutorial/Valka/nebezpecna_zona scripts, `bulanci.ghidra.exe.c` |

## Status

**DONE** — Live Ghidra MCP (`bulanci.exe`, 2026-06-07): opcode **101** handler disasm-proven; four bytecode sub-expressions map to `CGaming_SpawnPracticeDummy` → `CGaming_SpawnAndInitializePlayer` with auto slot `CountOccupiedPlayerSlots + 0x24`. Script `nLives` preserved for slots `0x24..0x27` (tournament). `script_dispatch_table.md` updated; decompiler comment @ `0x0041f730`.

## AI archetype

**Script-spawn tournament opponent** — full `CBulanek` entities in slots **`0x24..0x27`**, not campaign slots `0x7d..0x7f`. Same physical spawn helper as solo practice targets (`CGaming_SpawnPracticeDummy`), but **`InsertOpponent` passes script-configurable `nLives`, `nSpeedParam`, and `skinPaletteId`** (co-op slots `0x20..0x23` force `nLives=1` in ctor; `0x24..0x27` do not).

After spawn: `bIsAiVariant=1`, four `apAiTrackHolders` danger zones, walk AI via `CBulanek_TryBotRandomAction` when `CBulanek_IsHumanPlayer` (solo: `CGame+0x36==0`), tournament scheduler re-arm via `CBulanek_ArmTournamentSchedulerDelays` on knockdown (slots `0x20..0x27`).

## Algorithm

### Opcode handler — `CLevelScriptOpExt_InsertOpponent` (`0x0041f730`)

```text
InsertOpponent(teamColor, nLives, nSpeed, skinId):
  gaming = scriptCtx->pGaming          // [ESI+0x458]
  teamColor  = ReadSubExpr(ctx)        // → EDI
  nLives     = ReadSubExpr(ctx)        // → EBX
  nSpeed     = ReadSubExpr(ctx)        // → EBP
  skinId     = ReadSubExpr(ctx)        // → EAX
  CGaming_SpawnPracticeDummy(gaming, teamColor, nLives, nSpeed, skinId)
  return 0
```

**Disasm proof** (`0x0041f730`–`0x0041f76f`): four `CALL 0x004384c0` (`CDSScript::ReadSubExpr`); `MOV ECX,[ESI+0x458]`; stack args `PUSH EAX, EBP, EBX, EDI`; `CALL 0x0041f5d0`.

Editor mirror (`Editor.Scripts.InsertOpponent`): `Color`, `Hits` (= nLives), `Speed`, `Weapon` (= skinPaletteId).

### Slot auto-allocation — `CGaming_SpawnPracticeDummy` (`0x0041f5d0`)

```text
CGaming_SpawnPracticeDummy(this, teamColor, nLives, nSpeed, skinId):
  count = CGaming_CountOccupiedPlayerSlots(this)   // CALL 0x00416970
  if count >= 4:
      return                                       // all four 0x24..0x27 cells full
  CGaming_SetEntityRegisterMode(this, 1)           // bEntityRegisterMode@+0x341
  slotKind = count + 0x24                          // ADD DL,0x24 @ 0x0041f5fa
  CGaming_SpawnAndInitializePlayer(this, slotKind, teamColor, nLives, nSpeed, skinId)
```

**`CGaming_CountOccupiedPlayerSlots` (`0x00416970`)** scans **`apEntitySlots[0x24..0x27]`** (`CGaming+0x158`, four dwords) — **not** human slots `0..3`. Name is legacy; behavior is “count occupied tournament/practice cells.”

### Factory chain — `CGaming_SpawnAndInitializePlayer` (`0x0041f500`)

```text
SpawnAndInitializePlayer(this, slotKind, pTeamColorOverride, nLives, nSpeed, skinPaletteId):
  p = OperatorNew(0x19c)
  CBulanekCtor(p, pGame, slotKind, this, initialTrack=0,
               pTeamColorOverride, nLives, nSpeed, skinPaletteId)
  CGaming_AddEntity(this, p, 0xFF)
  CGaming_RegisterObjectAtSlot(this, p, slotKind)    // apEntitySlots[slot]@+0xC8
  CGaming_RespawnPlayerAtSafeLocation(p)
  facing = _rand() % 4
  CBulanek_SetFacingTrack(p, facing, sendNet=0)
```

### `CBulanekCtor` slot-kind branches (relevant to `0x24..0x27`)

| Check | Slots | Effect |
|-------|-------|--------|
| `(slotKind - 0x20) < 4` | `0x20..0x23` co-op | Force `nSpeed=0x28`, `nLives=1`, `skin=4`, team color `7`, Special walk table |
| `(slotKind - 0x24) < 4` | `0x24..0x27` tournament | **Keep script `nLives`, `nSpeed`, `skinId`**; if `teamColor < 8` → `bTeamColor@+0x123` |
| `3 < slotKind` | all AI slots | `bIsAiVariant=1`; alloc 4× `CGaming_AppendDangerZoneNode` → `apAiTrackHolders` |
| `slotKind > 3` | | `bSlotKind = slotKind & 3`; if zero → `3` (maps `0x24→3`, `0x25→1`, …) |

`nLives` → `nLivesRemaining@+0x18C` and `nLivesSnapshot@+0x190` (tournament death / respawn scoring).

### Script usage (master pack)

| Level | Pattern |
|-------|---------|
| **Tutorial** (`res_0000100009`) | `InsertBulanci()` then `InsertOpponent(7,3,120,5)`, `(7,2,120,4)`, `(7,2,120,1)` — mixed 2–3 hit tournament |
| **Valka / nebezpecna_zona** | `InsertBulanci()` + `InsertOpponent(7,1,…)` — 1-hit deathmatch |
| **Overlay menu levels** (`65855..65860`) | **`InsertBulanci()` only** — zero `InsertOpponent` calls |

Typical OnInit sequence: **`InsertBulanci`** (opcode 50 @ `0x00420610` → `CBulanek_SpawnPlayerAndCampaignSlots`) spawns humans `0..3`, then **`InsertOpponent`** fills `0x24..` with AI opponents.

### Post-spawn AI tick path

```text
Scheduler tick → CBulanek_WeaponSchedulerCallback
  case 3 (knockdown): TryBotRandomAction + ArmTournamentSchedulerDelays
TryBotRandomAction:
  if IsHumanPlayer && fire-delay slot idle:
    _rand → ApplyAction(track, 0) idle walk OR action 0..3 via CanDispatch
ArmTournamentSchedulerDelays (slots 0x20..0x27):
  scheduler slot 3 delay = (_rand % 10 + 1) * 1000 ms
  scheduler slot 4 delay = 2000 ms
```

Collision inclusion: slots `0x24..0x27` are `CBulanek` (`0x7ec`); `CGaming_IsPlayerSlotOccupied` and round-scoring loops treat them as player-like entities.

## Functions table

| Address | Symbol | Role |
|---------|--------|------|
| `0x0041f730` | `CLevelScriptOpExt_InsertOpponent` | Opcode 101: 4× `ReadSubExpr` → `SpawnPracticeDummy` |
| `0x0041f5d0` | `CGaming_SpawnPracticeDummy` | Guard `count<4`; mode `1`; `slot=count+0x24` |
| `0x00416970` | `CGaming_CountOccupiedPlayerSlots` | Count non-null in `apEntitySlots[0x24..0x27]` |
| `0x004168c0` | `CGaming_SetEntityRegisterMode` | `bEntityRegisterMode@+0x341 = 1` before spawn |
| `0x0041f500` | `CGaming_SpawnAndInitializePlayer` | `CBulanek` factory + register + respawn + facing |
| `0x0041e4b0` | `CBulanekCtor` | Slot-kind overrides; `nLives` commit @ `+0x18C` |
| `0x00420610` | `CLevelScriptOpExt_InsertBulanci` | Opcode 50: spawn human slots before opponents |
| `0x00420a90` | `CBulanek_TryBotRandomAction` | Random idle / action dispatch for AI |
| `0x004172d0` | `CBulanek_ArmTournamentSchedulerDelays` | Re-arm scheduler 3/4 for slots `0x20..0x27` |
| `0x00416720` | `CBulanek_IsHumanPlayer` | Solo gate: slot≥4 → `CGame+0x36==0` |

## Struct fields

| Offset | Field | InsertOpponent source |
|--------|-------|----------------------|
| `+0x70` | `bPlayerSlot` | Auto: `0x24 + occupiedCount` |
| `+0x123` | `bTeamColor` | Arg1 `teamColor` when `< 8` |
| `+0x18C` | `nLivesRemaining` | Arg2 `nLives` (**tournament hits-to-kill**) |
| `+0x190` | `nLivesSnapshot` | Same as arg2 at spawn |
| `+0x194` | `nSpeedParam` | Arg3 `nSpeed` (clamped `[0x28,200]`) |
| `+0x199` | `bIsAiVariant` | `1` (slotKind > 3) |
| `+0x178` | `apAiTrackHolders[4]` | Allocated when slotKind > 3 |
| `CGaming+0x341` | `bEntityRegisterMode` | Set to `1` during spawn |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `decompile_function` | `0x0041f730`, `0x0041f5d0`, `0x0041f500`, `0x00416970` | OK |
| `disassemble_bytes` | `0x0041f730`, `0x0041f5d0` | 4-arg push order + `ADD DL,0x24` verified |
| `set_decompiler_comment` | `0x0041f730` | InsertOpponent → SpawnPracticeDummy chain |
| `save_program` | `bulanci.exe` | Saved |

Prior art (R10 task 03): comments @ `0x0041f5fa`, `SpawnPracticeDummy` / `SpawnAndInitializePlayer` plates already present.

## Decomp fixes

| Issue | Resolution |
|-------|------------|
| `extraout_EAX_*` in InsertOpponent | Cosmetic — disasm shows sequential `MOV EDI/EBX/EBP` from four `ReadSubExpr` |
| `extraout_DL` in SpawnPracticeDummy | Cosmetic — disasm: `MOV EDX,EAX` (count) then `ADD DL,0x24` |
| `CountOccupiedPlayerSlots` name | Misleading — counts **`0x24..0x27`** occupancy, not humans `0..3` |

## Frida

**none** — static disasm + script corpus + `scripts/frida/bulanci_audio_logger.js` documents `nLives` override hook on `CBulanekCtor` args[5] for runtime validation if needed.

## Remaining UNK

| Item | Status |
|------|--------|
| Exact `skinPaletteId` → weapon sprite table | Deferred to asset catalog; passed to weapon ctor unchanged |
| Stationary vs moving for same slot range | Task **15** (practice dummies) vs this task — shared helper, behavior delta may be `nLives`/scheduler only |
| `CGaming+0x158` field name in struct | Still unnamed quad mirror of slots `0x24..0x27` |

## Cross-links

- [script_dispatch_table.md](../script_dispatch_table.md) — opcode 101 row updated
- [r10_deep_task_03_report.md](../deep_recovery/r10_deep_task_03_report.md) — slot auto-pick disasm
- [map_slots_spawner.md](../../gameplay/map_slots_spawner.md) — slot `0x24..0x27` taxonomy
- [CBulanek.md](../struct_recovery/CBulanek.md) — `nLivesRemaining`, `bIsAiVariant`
