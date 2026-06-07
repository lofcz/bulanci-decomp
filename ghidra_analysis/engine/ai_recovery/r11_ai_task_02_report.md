# Round 11 — AI Task 02 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 2 |
| **worker** | 02 / 30 |
| **round** | 11 (WRITE MODE) |
| **title** | `CBulanek_WeaponSchedulerCallback` — slot switch 0..5+ |
| **archetype** | `core_scheduler` |
| **seed_address** | `0x00420b30` |
| **addresses** | `0x00420b30`, `0x00420a90`, `0x004172d0` |

## Status

**DONE** — Full `switch(slotIndex)` for slots **0..5** proven (live Ghidra decompile + disasm). Default path for `slotIndex > 5` is no-op return. `CBulanek_TryBotRandomAction` and `CBulanek_ArmTournamentSchedulerDelays` algorithms documented with disasm-backed `_rand` formulas and caller xrefs. No Ghidra mutations this session.

## AI archetype

All paths run on **`CBulanek.scheduler`** (`CDSUpdatedItem` @ `+0x88`, IDSUpdated vtable `0x00481df4` slot **4**). This is the per-entity weapon/combat scheduler facet — shared by human slots, coop vampires (`0x20..0x23`), practice dummies (`0x24..0x27`), and campaign AI (`0x7d..0x7f`). Slot registration at ctor (`CBulanekCtor@0x0041e4b0`) gates which indices are live per archetype; this callback fires when armed slots expire.

### Ctor slot registration (context)

| `slotKind` band | Slots registered | Initial delay |
|-----------------|------------------|---------------|
| All | **0** | `150` ms if `pGame->bTotalSlots==1`, else `500` ms |
| `0x20..0x23` (coop) | **3**, **4**, **5** | `0`, `0`, `100` ms |
| `0x24..0x27` (practice) | **3**, **4** | `0`, `0` |
| `slotKind >= 4` (campaign AI) | **2** | `0` |

Event kind passed to `Scheduler_RegisterEventSlot` is **7** (weapon scheduler family).

## Algorithm

### Entry / `this` typing

```
// Called as IDSUpdated vtable method on CBulanek+0x88
void __thiscall CBulanek_WeaponSchedulerCallback(CDSUpdatedItem* sched, uint slotIndex)

CBulanek* self = (CBulanek*)((char*)sched - 0x88);  // disasm: LEA EDI,[ESI+0xffffff78]
```

**Disasm:** `CMP slotIndex, 5` / `JA` default; `JMP [EAX*4 + 0x420d24]` jump table for cases 0..5.

### Case 0 — death countdown / reload die / HUD refresh

```
// Internal tick byte @ sched+0x12 (CBulanek+0x9A) — not a named struct field yet
byte* pTick = (byte*)sched + 0x12;
(*pTick)++;

if (*pTick == 0x10) {                    // 16 scheduler firings
    Scheduler_ArmSlot(sched, 0);         // re-arm slot 0
    if (CBulanek_IsHumanPlayer(self)) {
        CBulanek_ResetAmmoAndPlayReload(self);
        CGame_NetSendPlayerDie_t11(self->pGame, self->bPlayerSlot);
        return;
    }
} else if (self->pWeapon->field_0x64 == 0) {   // weapon+0x64 disasm
    CBulanek_RefreshHudWeaponStrip(self);
    return;
}
// else fall through (no-op)
```

**Evidence:** `INC [ESI+0x9a]` / `CMP 0x10` @ `0x00420b4b`; `CALL 0x00416720` / `0x00417570` / `0x00412c40`; alternate `CMP [ECX+0x64],0` @ `0x00420b96` → `0x00416700`.

Matches `damage_pipeline.md`: post-death respawn timer → net **`0x11`** when local human.

### Case 1 — primary fire

```
CBulanek_TriggerPrimaryActionAndBroadcast(self);
return;
```

**Evidence:** `CALL 0x004208c0` @ `0x00420bb9`. Requires `view_flags@+0x44 & 1` and weapon track-manager slot 0 armed inside `TriggerPrimaryActionAndBroadcast`.

### Case 2 — damage tick (campaign AI fire-delay slot)

```
slot = Scheduler_GetEventSlot(&self->pWeapon->trackManager.scheduler, 0);
if (slot && (slot[+8] & 1)) {   // armed
    CBulanek_OnTakeDamage(self, -1, -1, 0);
    return;
}
```

**Evidence:** `LEA ECX,[EAX+0xc]` (weapon scheduler) @ `0x00420bcb`; `TEST [EAX+8],1` @ `0x00420bd3`; `CALL 0x0041db00`.

Pairs with `CBulanek_ArmFireDelayScheduler@0x00417260` which arms scheduler **slot 2** with `_rand`-scaled **5..15 s** delay for campaign slots (outside `0x00..3`, `0x20..27` bands).

### Case 3 — knockdown bot action + tournament re-arm

```
slot = Scheduler_GetEventSlot(&self->pWeapon->trackManager.scheduler, 0);
if (slot && (slot[+8] & 1)) {
    if (CBulanek_IsInKnockdownAnimBand(self)) {
        CBulanek_TryBotRandomAction(self);
        CBulanek_ArmTournamentSchedulerDelays(self);
        return;
    }
}
```

**`CBulanek_IsInKnockdownAnimBand@0x00416490`:** returns true iff `bPlayerSlot` ∈ `0x20..0x23` **or** `0x24..0x27` (coop vampires + practice dummies). Campaign AI (`0x7d+`) returns false — they never enter this branch.

**Evidence:** `CALL 0x00416490` @ `0x00420c11`; `CALL 0x00420a90` @ `0x00420c1c`; `CALL 0x004172d0` @ `0x00420c23`.

### Case 4 — AI pickup scan → facing → arm slot 1

```
if (self->apAiTrackHolders[0] == NULL) break;

for (i = 0; i < 4; i++) {
    holder = self->apAiTrackHolders[i];
    mask = holder->field_0x04 & 0x0F;          // disasm: TEST [EDX+4],0xf
    if (mask == 0) continue;

    if (CBulanek_TryApplyPickupMask(self, mask, holder->pVptr_IDSReferenced)) {
        trackIdx = *(int*)holder;              // first dword of holder node
        if (!CBulanek_SetFacingTrack(self, trackIdx, 0))
            return;

        delayMs = round(200.0 / (self->nSpeedParam / 100.0));
        // constants: g_kSpeedFormulaNeutral_100p0 @ 0x00482910, double 200.0 @ 0x004829a8
        Scheduler_RegisterEventSlot(sched, 1, delayMs, 2);
        return;
    }
}
```

**Speed formula (disasm @ `0x00420c94`..`0x00420ccf`):**

\[
\text{delayMs} = \mathrm{round}\left(\frac{200.0}{\text{nSpeedParam}/100.0}\right) = \mathrm{round}\left(\frac{20000}{\text{nSpeedParam}}\right)
\]

Examples: speed `100` → `200` ms; `200` → `100` ms; `40` → `500` ms.

**Evidence:** loop `CMP EBP,4` @ `0x00420c6b`; `CALL 0x004194b0` / `0x004197b0` / `0x0042f210`.

### Case 5 — respawn alpha fade + position sync

```
self->nDrawAlphaPercent += 10;               // CBulanek+0x140 (ESI+0xb8 from sched facet)
if (self->nDrawAlphaPercent > 100) {
    self->nDrawAlphaPercent = 100;
    Scheduler_ArmSlot(sched, 5);
}
self->vftable_primary[9](self, 0, 0);        // vtable+0x24 — movement fallback sync
return;
```

**Evidence:** `ADD [ESI+0xb8],0xa` / `CMP 0x64` @ `0x00420ce1`; `CALL [EAX+0x24]` @ `0x00420d17`. Same `+0x24` fallback documented in `round6_logic_task_02_report.md` (`FUN_0041af70` collision path).

### Default (`slotIndex > 5`)

Fall through to `RET` — no operation.

---

## `CBulanek_TryBotRandomAction` @ `0x00420a90`

```
void __fastcall CBulanek_TryBotRandomAction(CBulanek* self)
{
    if (!CBulanek_IsHumanPlayer(self)) return;

    // Gate: weapon track-manager scheduler slot 0 must be armed
    wSlot = Scheduler_GetEventSlot(&self->pWeapon->trackManager.scheduler, 0);
    if (!(wSlot[+8] & 1)) return;

    r = _rand();
    // idle branch ~7.7%: (r * 13) >> 15 == 0
    if (((r * 13 + sign) >> 15) == 0) {
        vSlot = Scheduler_GetEventSlot(&self->videoTrackManager.scheduler, 0);
        if (vSlot[+8] & 1) goto action_branch;   // walk TM busy → skip idle

        CBulanek_ApplyAction(self, self->videoTrackManager.nCurrentTrackIdx, 0);
        return;
    }

action_branch:
    r = _rand();
    action = (r * 4 + sign) >> 15;             // uniform 0..3
    if (CBulanek_CanDispatchPlayerAction(self, action, 1))
        CBulanek_ApplyAction(self, action, 1);
}
```

**`_rand` scaling:** MSVC return `0..0x7FFF`; formula `(r * N + (r*N>>31 & 0x7FFF)) >> 15` ≡ `floor(r * N / 32768)` for non-negative products.

| Branch | Formula | Outcome |
|--------|---------|---------|
| Idle | `(r*13)>>15 == 0` | `ApplyAction(nCurrentTrackIdx, ai=0)` if walk TM slot 0 **not** armed |
| Action | `(r*4)>>15` → `0..3` | `CanDispatchPlayerAction` then `ApplyAction(action, ai=1)` |

**Callers (xrefs):**

| Caller | Site |
|--------|------|
| `CBulanek_WeaponSchedulerCallback` | `0x00420c1c` (case 3) |
| `CWeapon_Fire` | `0x0042155f` |
| `CBulanek_OnEvent` | `0x00421160` |

## `CBulanek_ArmTournamentSchedulerDelays` @ `0x004172d0`

```
void __fastcall CBulanek_ArmTournamentSchedulerDelays(CBulanek* self)
{
    slot = self->bPlayerSlot;
    if (!((0x20 <= slot && slot < 0x24) || (0x24 <= slot && slot < 0x28)))
        return;

    sched = &self->scheduler;

    r = _rand();
    delay3_ms = (((r * 10 + sign) >> 15) + 1) * 1000;   // 1000..10000 ms step 1000
    Scheduler_SetEventDelayMs(sched, 3, delay3_ms);
    Scheduler_SetEventLastFireMs(sched, 3, -1);
    if (slot3 armed) Scheduler_AckSlot(sched, 3, -1);

    Scheduler_SetEventDelayMs(sched, 4, 2000);            // fixed 2000 ms
    Scheduler_SetEventLastFireMs(sched, 4, -1);
    if (slot4 armed) Scheduler_AckSlot(sched, 4, -1);
}
```

**Slot band proof (disasm):** `CMP AL,0x20` / `CMP AL,0x24` / `CMP AL,0x28` @ `0x004172d3`..`0x004172e5`.

**Callers (xrefs):**

| Caller | Site |
|--------|------|
| `CBulanek_WeaponSchedulerCallback` | `0x00420c23` (case 3) |
| `CBulanek_OnShow` | `0x004173d8` |

## Functions table

| Address | Symbol | Role |
|---------|--------|------|
| `0x00420b30` | `CBulanek_WeaponSchedulerCallback` | IDSUpdated vtable slot 4; `switch(slotIndex)` cases 0..5 |
| `0x00420a90` | `CBulanek_TryBotRandomAction` | Random idle walk vs action 0..3 for tournament/co-op AI |
| `0x004172d0` | `CBulanek_ArmTournamentSchedulerDelays` | Re-arm scheduler slots 3/4 with `_rand` + fixed delays |
| `0x004208c0` | `CBulanek_TriggerPrimaryActionAndBroadcast` | Case 1 callee |
| `0x00417570` | `CBulanek_ResetAmmoAndPlayReload` | Case 0 human path |
| `0x00416720` | `CBulanek_IsHumanPlayer` | Case 0 gate; TryBot entry gate |
| `0x00416490` | `CBulanek_IsInKnockdownAnimBand` | Case 3 slot-band gate |
| `0x004194b0` | `CBulanek_TryApplyPickupMask` | Case 4 pickup collision mask |
| `0x004197b0` | `CBulanek_SetFacingTrack` | Case 4 facing after pickup |
| `0x00417260` | `CBulanek_ArmFireDelayScheduler` | Arms slot **2** (pairs with case 2) — related, not in task slice |
| `0x0041aed0` | `CBulanek_SchedulerTick` | Pre-tick enqueue of sched / weapon / walk TM facets |

## Struct fields touched

| Offset | Name | Cases / helpers |
|--------|------|-----------------|
| `+0x88` | `scheduler` | Callback `this`; `ArmTournament` target |
| `+0x9A` | `scheduler.slot0TickByte` (UNK name) | Case 0 increment to `0x10` |
| `+0x44` | `view_flags` | Case 1 via `TriggerPrimaryAction` (`& 1`) |
| `+0x70` | `bPlayerSlot` | Case 0 net die; knockdown band; tournament arm band |
| `+0xAC` | `videoTrackManager.scheduler` | TryBot idle gate |
| `+0xD4` | `videoTrackManager.nCurrentTrackIdx` | TryBot idle `ApplyAction` arg |
| `+0xF4` | `pGame` | Case 0 `NetSendPlayerDie_t11` |
| `+0xF8` | `pWeapon` | Cases 0/2/3 weapon scheduler; TryBot gate |
| `+0xF8+0x64` | `CWeapon::field_0x64` | Case 0 HUD refresh when zero |
| `+0x140` | `nDrawAlphaPercent` | Case 5 fade counter |
| `+0x178` | `apAiTrackHolders[4]` | Case 4 pickup loop |
| `+0x194` | `nSpeedParam` | Case 4 fire re-arm delay |

## Ghidra deltas

**none** — read-only analysis session.

## Decomp fixes

| Issue | Fix |
|-------|-----|
| `CBulanek_WeaponSchedulerCallback` decompile uses `this[-1].pReserved_preAmmo + 0x10` | **`this` is `CDSUpdatedItem*` @ `CBulanek+0x88`**, not `CBulanek*`. Correct: `(CBulanek*)((char*)this - 0x88)`. Disasm `LEA EDI,[ESI+0xffffff78]` @ `0x00420b62`. |
| Case 5 decompile names `(videoTrackManager).scheduler.pEventSlots` @ `+0xb8` | Field is **`nDrawAlphaPercent@+0x140`** (`ESI+0xb8` from sched base `+0x88`). |
| Case 4 decompile `pActiveAnim` loop | Loop iterates **`apAiTrackHolders@+0x178`** (`LEA EDI,[ESI+0xf0]`). |

Recommended (deferred): `set_function_this_type(CDSUpdatedItem *)` on callback + plate comment documenting `CBulanek*` recovery.

## Frida

**none** — all branches closed via disasm + live decompile.

## Remaining UNK

- Exact naming of **`scheduler+0x12`** tick byte used in case 0 (maps to `CBulanek+0x9A`).
- **`CWeapon+0x64`** semantic name (gates HUD refresh in case 0 sub-path).
- **`Scheduler_RegisterEventSlot(..., eventKind=2)`** in case 4 vs ctor `eventKind=7` — confirm event-kind taxonomy in `CDSUpdatedItem` batch.
- Case 5 **`vtable+0x24`** symbol name on primary `CBulanek` vtable (movement fallback; likely `CGameView` position sync).
