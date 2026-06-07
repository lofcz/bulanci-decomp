# Round 11 — AI Task 03 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 3 |
| **title** | `CBulanek_TryBotRandomAction` — random idle vs action 0..3 |
| **archetype** | `moving_bot` |
| **seed_address** | `0x00420a90` |
| **addresses** | `0x00420a90`, `0x004477ec` |
| **acceptance** | Prove `_rand` scaling formulas; `IsHumanPlayer` gate; fire-delay slot bit checks; `CanDispatchPlayerAction` before `ApplyAction` |

## Status

**DONE** — Live Ghidra MCP (`bulanci.exe`) + disasm correlation with export `bulanci.ghidra.exe.c`. Full control flow, both `_rand` formulas, scheduler slot bit gates, and three caller sites proven. No new Ghidra mutations required (R5 worker 03 comments @ `0x00420ab6` / `0x00420af2` still accurate).

## AI archetype

**Moving bot / co-op ally / practice dummy** — post-fire or knockdown-recovery random behavior for **non-campaign** AI slots `0x20..0x27`. Campaign AI (`0x7d..0x7f`) uses a separate `RandInRange` self-damage path in `CWeapon_Fire` and does **not** call this function.

| Slot range | Role | `TryBotRandomAction` caller |
|------------|------|-----------------------------|
| `0x20..0x23` | Co-op / vampire allies | `CWeapon_Fire` @ `param_2==0xFFFF` |
| `0x24..0x27` | Solo practice dummies | same |
| `0x7d..0x7f` | Campaign AI | **not called** — `RandInRange(0,99)<40` → `OnTakeDamage` instead |

## Algorithm

Pseudocode with proven constants from disasm @ `0x00420a90`–`0x00420b23`:

```
// _rand @ 0x004477ec → int in [0, 32767]  (MSVC LCG, return (seed>>16)&0x7fff)
// Fixed-point divide-by-32768:  (n * K + (n*K>>31 & 0x7fff)) >> 15

void CBulanek_TryBotRandomAction(CBulanek *this)   // __fastcall, ECX=this, size 0x94
{
    if (!CBulanek_IsHumanPlayer(this))          // CALL 0x00416720 @ 0x00420a93
        return;

    // Gate: weapon embedded scheduler slot 0 must be ARMED (descriptor+8 bit0 set)
  weaponSched = Scheduler_GetEventSlot(this->pWeapon + 0x0C, 0);  // pWeapon@+0xF8
    if (!(weaponSched[8] & 1))                  // TEST [EAX+8],1 @ 0x00420ab0
        return;

    r = _rand();
    idleRoll = (r * 0x0D) >> 15;                // IMUL 0xD; SAR 15 @ 0x00420abb–0x00420ac7
    // idleRoll == 0  ⇔  r ∈ [0, 2520]  (~7.69% = 2521/32768)

    videoSched = Scheduler_GetEventSlot(&this->videoTrackManager.scheduler, 0);
    // videoTrackManager@+0xA8; scheduler embed @ +0xAC in disasm (manager+4)

    if (idleRoll == 0 && !(videoSched[8] & 1)) {
        // Idle: continue current facing track, human-style dispatch flag
        CBulanek_ApplyAction(this, this->videoTrackManager.nCurrentTrackIdx, 0);
        // nCurrentTrackIdx @ CBulanek+0xD4; ApplyAction @ 0x00420910, PUSH 0 @ 0x00420ae5
        return;
    }

    r = _rand();
    action = (r * 4) >> 15;                   // IMUL via ADD EAX,EAX ×2 @ 0x00420af7–0x00420b08
    // action ∈ {0,1,2,3} uniform (8192 values each)

    if (CBulanek_CanDispatchPlayerAction(this, action, 1))  // CALL 0x004174a0
        CBulanek_ApplyAction(this, action, 1);
}
```

### `_rand` formulas (disasm-proven)

| Branch | Site | Assembly | Formula | Range / probability |
|--------|------|----------|---------|---------------------|
| **Idle roll** | `0x00420ab6`–`0x00420aca` | `CALL _rand`; `IMUL EAX,0xD`; `CDQ`; `AND EDX,0x7FFF`; `ADD`; `SAR 15`; `JNZ` | `idle = (r × 13) ÷ 32768` (signed fixed-point) | `idle == 0` when `r ≤ 2520` → **2521/32768 ≈ 7.690%** |
| **Action index** | `0x00420af2`–`0x00420b08` | `CALL _rand`; `ADD EAX,EAX` ×2; same ÷32768 | `action = (r × 4) ÷ 32768` | **Uniform 0..3** (8192 values per bucket) |

### Scheduler slot bit checks

| Scheduler object | CBulanek path | Slot index | Bit test | Meaning in this function |
|------------------|---------------|------------|----------|--------------------------|
| **Weapon** `CDSUpdatedItem` @ `pWeapon+0x0C` | `[ESI+0xF8]+0x0C` | **0** | `[slot+8] & 1` **must be 1** | Post-fire weapon cooldown armed — outer gate @ `0x00420ab0` |
| **Video track mgr** `CDSUpdatedItem` @ `videoTrackManager+0x04` | `[ESI+0xAC]` | **0** | `[slot+8] & 1` **must be 0** for idle | Walk/anim slot 0 free — idle branch @ `0x00420ad9` |
| Player body `scheduler` @ `+0x88` slot **2** | — | — | — | **Not read here**; armed by `CBulanek_ArmFireDelayScheduler` for campaign AI fire delay (task 5) |

Descriptor `+8` bit 0 = scheduler event slot **armed/live** (consistent with `CShot_SchedulerTick`, `CBulanek_StepMovementAndCollision`).

### `CBulanek_IsHumanPlayer` gate @ `0x00416720`

Disasm:

```
MOV AL, [ECX+0x70]       ; bPlayerSlot
CMP AL, 4
JNC  branch_ai
; slot < 4: CGame_IsLocalPlayerSlot(pGame@+0xF4, slot)
branch_ai:
; slot >= 4: return (CGame+0x36 == 0)   ; host/non-client session
```

| `bPlayerSlot@+0x70` | Return true when | Effect on `TryBotRandomAction` |
|---------------------|------------------|--------------------------------|
| `0..3` | `CGame_IsLocalPlayerSlot(pGame, slot)` | Local human seat only (not remote peers) |
| `≥ 4` | `*(byte*)(pGame + 0x36) == 0` | AI entities in host/simulation mode |

Despite the name, for AI slots this is **not** “is human” — it gates **local simulation authority** (host + local player), blocking client-only sessions.

### `CBulanek_CanDispatchPlayerAction` @ `0x004174a0`

Called with `param_2 = 1` (AI dispatch) and `param_1 ∈ {0,1,2,3}`:

1. **`view_flags@+0x44` bit 0** must be set (`TEST` in CanDispatch) — entity active in world.
2. For actions **0..3** with `param_2 != 0`: returns **1** immediately (no fire-delay re-check on this path).
3. Actions **4/5** with `param_2 != 0` additionally require weapon scheduler slot 0 armed (not used by this function).

### `CBulanek_ApplyAction` outcomes @ `0x00420910`

| Call site | `action` | `isAi` (`param_2`) | Behavior |
|-----------|----------|-------------------|----------|
| Idle @ `0x00420aea` | `nCurrentTrackIdx@+0xD4` | `0` | `SnapPositionToFacingAxis` + `BeginCurrentTrackPlayback`; net send `state = action+4` if `IsHumanPlayer` |
| Random @ `0x00420b1c` | `_rand` → `0..3` | `1` | `SetFacingTrack(action, 1)`; net `state = action` if `IsHumanPlayer` |

Action indices match human controls: **0=up, 1=right, 2=down, 3=left** (facing tracks; see `player_controls.md`).

## Functions table

| Address | Symbol | Role | Evidence |
|---------|--------|------|----------|
| `0x00420a90` | `CBulanek_TryBotRandomAction` | **Seed** — idle vs facing random | `__fastcall`; `PUSH ESI` / `MOV ESI,ECX`; size `0x94`; mapping.csv |
| `0x004477ec` | `_rand` | MSVC LCG → `[0,32767]` | Two `CALL` sites in seed |
| `0x00416720` | `CBulanek_IsHumanPlayer` | Entry gate | `CALL` @ `0x00420a93` |
| `0x004174a0` | `CBulanek_CanDispatchPlayerAction` | Action 0..3 gate | `CALL` @ `0x00420b0e`; `PUSH 1` @ `0x00420b06` |
| `0x00420910` | `CBulanek_ApplyAction` | Idle / facing dispatch | `CALL` @ `0x00420aea`, `0x00420b1c` |
| `0x0042f1e0` | `Scheduler_GetEventSlot` | Slot descriptor lookup | `CALL` @ `0x00420aab`, `0x00420ad4` |
| `0x00420b30` | `CBulanek_WeaponSchedulerCallback` | Caller case 3 knockdown | `TryBotRandomAction` + `ArmTournamentSchedulerDelays` |
| `0x00420d40` | `CBulanek_OnEvent` | Caller event `0x102` | Direct call |
| `0x004212b0` | `CWeapon_Fire` | Caller `param_2==0xFFFF` branch | Slots `0x20..0x27` only |

### Callers (`get_function_callers@0x00420a90`)

| Caller | Address | Trigger |
|--------|---------|---------|
| `CBulanek_WeaponSchedulerCallback` | `0x00420b30` | **Case 3:** player scheduler slot 3 armed + `IsInKnockdownAnimBand` → random action then `ArmTournamentSchedulerDelays` |
| `CBulanek_OnEvent` | `0x00420d40` | **Event `0x102`** — script/synthetic bot tick |
| `CWeapon_Fire` | `0x004212b0` | **`param_2 == 0xFFFF`** (ammo-empty frame hint) after fire; `bPlayerSlot > 3` and slot in `0x20..0x23` or `0x24..0x27` |

## Struct fields

| Offset | Field | Use in seed |
|--------|-------|-------------|
| `+0x44` | `view_flags` | `CanDispatchPlayerAction` bit 0 |
| `+0x70` | `bPlayerSlot` | `IsHumanPlayer` branch; `CWeapon_Fire` slot filter |
| `+0x88` | `scheduler` | Not read in seed (case 3 caller uses slot 3) |
| `+0xA8` | `videoTrackManager` | Idle scheduler slot 0 + `nCurrentTrackIdx` |
| `+0xAC` | `videoTrackManager.scheduler` | Disasm `LEA ECX,[ESI+0xAC]` |
| `+0xD4` | `videoTrackManager.nCurrentTrackIdx` | Idle `ApplyAction` argument |
| `+0xF4` | `pGame` | `IsHumanPlayer` → `CGame+0x36` |
| `+0xF8` | `pWeapon` | Weapon scheduler @ `pWeapon+0x0C`, slot 0 |

## Ghidra deltas

None this round. Prior R5 worker 03:

| Action | Target |
|--------|--------|
| `set_decompiler_comment` | `0x00420ab6`, `0x00420af2` |
| `set_plate_comment` | `_rand@0x004477ec` |

## Decomp fixes

None required — live decompile matches disasm. Ghidra correctly types `pWeapon->trackManager.scheduler` and `videoTrackManager.scheduler`.

## Frida

Not run — static proof sufficient for all branches.

## Remaining UNK

| Item | Notes |
|------|-------|
| Event `0x102` producer chain | Who posts `0x102` to `CBulanek_OnEvent` — defer to script/scheduler batch |
| Weapon scheduler slot 0 arm site | Confirmed consumed here and in `TriggerPrimaryActionAndBroadcast`; exact armer is `CWeapon` fire path (task 4/5 overlap) |
| `CGame+0x36` symbol | Used as client/host gate in `IsHumanPlayer`; correlate with `gameplay_impact.md` admin flag |
