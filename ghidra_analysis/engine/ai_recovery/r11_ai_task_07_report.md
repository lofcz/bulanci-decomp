# Round 11 AI — Task 07 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 7 |
| **round** | 11 |
| **worker** | 07 / 30 |
| **archetype** | taxonomy |
| **seed_address** | `0x00416720` |
| **addresses** | `0x00416720`, `0x0041e4b0` |
| **acceptance** | Prove ctor writes `bIsAiVariant` (`slotKind>3`), `bSlotKind` masking; every `IsHumanPlayer` caller in AI path. |

## Status

**DONE** — Ghidra MCP live decompile + xref closure on `bulanci.exe` (Jun 2026). Ctor field writes, spawn-path `slotKind` inputs, and all 15 `CBulanek_IsHumanPlayer` call sites catalogued; AI-path subset proven with caller context.

## AI archetype

**taxonomy** — shared detection layer for all `CBulanek` player-like entities (humans `0x00..0x03`, co-op bots `0x20..0x23`, practice dummies `0x24..0x27`, campaign AI `0x7d..0x7f`). See [map_slots_spawner.md](../../gameplay/map_slots_spawner.md).

Two orthogonal flags:

| Field | Offset | Set by | Meaning |
|-------|--------|--------|---------|
| `bPlayerSlot` | `+0x70` | `CGaming_RegisterObjectAtSlot` | Entity index in `m_apEntitySlots[128]` |
| `bSlotKind` | `+0x124` | `CBulanekCtor` | Normalized role index `0..3` for tables / quips / walk variants |
| `bIsAiVariant` | `+0x199` | `CBulanekCtor` + script events | Non-human gameplay path (AI track holders, scheduler extras, ammo rules) |
| `IsHumanPlayer()` | — | runtime | **Network authority gate** — “may this entity originate slot-owned net msgs on this machine?” |

`bIsAiVariant` is **not** consulted inside `IsHumanPlayer`. AI bots on the host still pass `IsHumanPlayer` when `CGame+0x36 == 0` (host/admin).

---

## Algorithm

### `CBulanek_IsHumanPlayer` @ `0x00416720`

```c
// uint __fastcall CBulanek_IsHumanPlayer(CBulanek *this)
uint CBulanek_IsHumanPlayer(CBulanek *this) {
    if (this->bPlayerSlot < 4) {                          // +0x70
        return CGame_IsLocalPlayerSlot(this->pGame,       // +0xF4
                                       this->bPlayerSlot);
    }
  // AI / non-primary slots: host-only net authority
    return (*(char *)((char *)this->pGame + 0x36) == 0); // CGame+0x36 host/admin
}
```

`CGame_IsLocalPlayerSlot` @ `0x004128f0`:

```c
// true when lobby binding row for slot matches local DPID @ CGame+0x31
return **(int **)(CGame + 0xE2 + slot * 0x23) == *(int *)(CGame + 0x31);
```

Cross-ref: [gameplay_impact.md](../../netcode/gameplay_impact.md) — `CGame+0x36 == 0` = host/admin.

### `CBulanekCtor` — `bIsAiVariant` and `bSlotKind` @ `0x0041e4b0`

Ctor prototype (Ghidra):  
`CBulanek * __thiscall CBulanekCtor(CBulanek *this, CGame *pGame, uchar slotKind, void *pGamingHostScratch, int initialTrack, void *pTeamColorOverride, int nLives, int nSpeedParam, int skinPaletteId)`

```c
bool isCoopSlot  = (uint8)(slotKind - 0x20) < 4;   // entity slots 0x20..0x23
bool isDummySlot = (uint8)(slotKind - 0x24) < 4;   // entity slots 0x24..0x27

// --- bIsAiVariant @ +0x199 ---
this->bIsAiVariant = (slotKind > 3);

if (slotKind > 3) {
    // apAiTrackHolders[4] @ +0x178: CGaming_AppendDangerZoneNode × 4
    for (i = 0; i < 4; i++)
        this->apAiTrackHolders[i] = CGaming_AppendDangerZoneNode(pGamingHostScratch, i, ...);
}

// --- bSlotKind @ +0x124 (masking) ---
this->bSlotKind = slotKind;
if (slotKind > 3) {
    this->bSlotKind = slotKind & 3;
    if ((slotKind & 3) == 0)
        this->bSlotKind = 3;   // e.g. practice dummy slot 0x24 → bSlotKind=3
}

// Co-op overrides (isCoopSlot): nSpeedParam=0x28, nLives=1, bTeamColor=7, special walk anims
// Dummy team color: if isDummySlot && pTeamColorOverride < 8 → bTeamColor = (byte)pTeamColorOverride
// Human color: slotKind < 4 → CGameGetPlayerColorByte(pGame, slotKind)
// AI color: slotKind > 3 → _rand scaled to 0..7

// Scheduler extras (eventKind=7) after slot 0 always registered:
if (isCoopSlot)  { RegisterEventSlot(scheduler, 3, 0, 7); RegisterEventSlot(..., 4, 0, 7); RegisterEventSlot(..., 5, 100, 7); }
else if (isDummySlot) { RegisterEventSlot(scheduler, 3, 0, 7); RegisterEventSlot(..., 4, 0, 7); }
else if (slotKind >= 4) { RegisterEventSlot(scheduler, 2, 0, 7); }  // campaign / generic AI
// humans (slotKind < 4): only slot 0 registered, then early return
```

### Spawn path → `slotKind` argument

| Spawn function | Address | Entity slot (`bPlayerSlot`) | `slotKind` to ctor | `bIsAiVariant` | `bSlotKind` after mask |
|----------------|---------|----------------------------|--------------------|----------------|------------------------|
| `CGaming_SpawnPlayerAtSlot` | `0x00420530` | `0..bTotalSlots-1` | same as slot (`0..3`) | `0` | `0..3` |
| `CGaming_SpawnCoopPartnerSlots` | `0x00420550` | `0x20..0x23` | `0x20..0x23` | `1` | `0..3` (`slot & 3`) |
| `CGaming_SpawnPracticeDummy` | `0x0041f5d0` | `count+0x24` → `0x24..0x27` | `0x24..0x27` | `1` | `3,1,2,3` for slots `0x24..0x27` |
| `CBulanek_SpawnPlayerAndCampaignSlots` (solo) | `0x004205a0` | `0x7d..0x7f` | `0x7d..0x7f` | `1` | `1,2,3` |

Wrapper: `CGaming_SpawnAndInitializePlayer` @ `0x0041f526` passes `param_1` (= entity slot) as ctor `slotKind`.

### Runtime `bIsAiVariant` toggles (script)

| Event | Subparam | Effect |
|-------|----------|--------|
| `0xEE` | `(lParam>>8)&0xFF == 2`, slot match | `bIsAiVariant = 1` |
| `0xEF` | `(lParam>>8)&0xFF == 2`, slot match | `bIsAiVariant = 0` |

`CBulanek_OnEvent` @ `0x00420d40`.

### `bIsAiVariant` consumer (ammo, not IsHumanPlayer)

`CBulanek_DecrementWeaponAmmo` @ `0x004167xx` — decrements ammo only when `bIsAiVariant == 0` **and** `bPlayerSlot < 4`. AI variants skip human ammo accounting.

---

## Functions table

| Address | Symbol | Role in AI vs human taxonomy |
|---------|--------|------------------------------|
| `0x00416720` | `CBulanek_IsHumanPlayer` | Net-authority predicate (local human OR host for slot ≥ 4) |
| `0x004128f0` | `CGame_IsLocalPlayerSlot` | Sub-check for slots `0..3` |
| `0x0041e4b0` | `CBulanekCtor` | Writes `bIsAiVariant`, `bSlotKind`; AI track holders; scheduler topology |
| `0x0041f526` | `CGaming_SpawnAndInitializePlayer` | Alloc `0x19c`, ctor, register slot |
| `0x00420530` | `CGaming_SpawnPlayerAtSlot` | Humans + campaign AI (`nLives=1` hardcoded) |
| `0x00420550` | `CGaming_SpawnCoopPartnerSlots` | Co-op vampires `0x20..0x23` |
| `0x0041f5d0` | `CGaming_SpawnPracticeDummy` | Dummies `0x24..0x27`, script `nLives` |
| `0x004205a0` | `CBulanek_SpawnPlayerAndCampaignSlots` | Humans + solo campaign `0x7d..0x7f` |
| `0x00420a90` | `CBulanek_TryBotRandomAction` | **AI path** — gated on `IsHumanPlayer` |
| `0x00420b30` | `CBulanek_WeaponSchedulerCallback` | **AI path** — case 0 die net, case 3 → `TryBotRandomAction` |
| `0x004214f7` | `CWeapon_Fire` | **AI path** — `0xFFFF` branch → `TryBotRandomAction` (coop/dummy) |
| `0x004166b0` | `CBulanek_GetHitQuipSlot` | Uses `bPlayerSlot` for coop range; indexes quip table by `bSlotKind` |

### All `CBulanek_IsHumanPlayer` xrefs (15 call sites)

| Caller | Address | AI path? | Behavior when true |
|--------|---------|----------|-------------------|
| `CBulanek_ApplyAction` | `0x0042093d` | indirect | `CGame_NetSendPlayerState_t0d` (idle snap) |
| `CBulanek_ApplyAction` | `0x0042097c` | indirect | `CGame_NetSendPlayerState_t0d` (facing actions) |
| `CBulanek_SetFacingTrack` | `0x00419832` | indirect | `CGame_NetSendPlayerState_t0d` (facing \| 8) |
| `CBulanek_ApplyPickupEffect` | `0x0041eb2f` | indirect | `CGame_NetSendDamage_t0c` after weapon swap |
| `CBulanek_TriggerPrimaryActionAndBroadcast` | `0x004208ec` | indirect | `CGame_NetSendPlayerEvent1_t0e` on fire |
| `CBulanek_Draw` | `0x0041765b` | indirect | Allow muted gray draw for host-visible AI |
| `CDeath_OnCorpseHideRequestRespawn` | `0x00417ab1` | no | Post respawn message `0xF6` |
| `CGaming_TryGetPlayerCoords` | `0x0041a10c` | no | Return coords only for local/host entity |
| `CGaming_TickPlayerCollisions` | `0x0041f0da` | indirect | Pickup overlap scan slots `101..107` |
| `CGame_ApplyPickup` | `0x0041ebdf` | indirect | Net tri-byte / player event before ammo |
| `CGame_ApplyPickup` | `0x0041ec23` | indirect | `CBulanek_ApplyPickupEffect` |
| `CBulanek_OnDeath` | `0x0041fb1c` | indirect | Strip weapon via `ApplyPickupEffect(0)` |
| **`CBulanek_TryBotRandomAction`** | **`0x00420a93`** | **yes** | Entire bot idle/fire RNG skipped if false |
| `CWeapon_Fire` | `0x004214f7` | **yes** | Out-of-ammo net `0xD9`; enables `TryBotRandomAction` |
| **`CBulanek_WeaponSchedulerCallback`** | **`0x00420b6a`** | **yes** | Case 0: `CGame_NetSendPlayerDie_t11` on host |

**AI-path closure:** Host machine runs bot logic (`TryBotRandomAction`, scheduler case 3 knockdown, `CWeapon_Fire` `param_2==0xFFFF`) only when `IsHumanPlayer` is true — i.e. campaign/co-op/dummy entities on **clients** do not self-drive or emit net from those paths.

---

## Struct fields

| Offset | Name | Type | Writer | Readers (AI-relevant) |
|--------|------|------|--------|------------------------|
| `+0x70` | `bPlayerSlot` | `byte` | `CGaming_RegisterObjectAtSlot` | `IsHumanPlayer`, spawn range checks in `CWeapon_Fire`, `ApplyPickupEffect` |
| `+0x124` | `bSlotKind` | `byte` | `CBulanekCtor` (masked) | `CBulanek_GetHitQuipSlot`, walk/weapon tables (`DAT_004aef88`) |
| `+0x199` | `bIsAiVariant` | `byte` | `CBulanekCtor`; `OnEvent` `0xEE`/`0xEF` | `CBulanek_DecrementWeaponAmmo`; ctor AI holder alloc |
| `+0x178` | `apAiTrackHolders[4]` | `void*[4]` | ctor when `slotKind > 3` | `OnEvent` `0xF2` pickup facing |
| `+0xF4` | `pGame` | `CGame*` | ctor | `IsHumanPlayer` → `+0x36` host flag |

---

## Ghidra deltas

**none** — symbols `CBulanek_IsHumanPlayer`, `CBulanekCtor`, spawn wrappers, and struct fields already applied in prior rounds. Live MCP decompile confirms typed `CBulanek *` ctor and named fields (`bIsAiVariant`, `bSlotKind`, `scheduler`, `videoTrackManager`).

## Decomp fixes

**none required** — `CBulanek_WeaponSchedulerCallback` decompile still shows offset arithmetic via `this[-1].pReserved_preAmmo` (Ghidra this-type quirk on scheduler callback `this`); disasm target addresses (`0x00420b6a` xref) are correct.

## Frida

**none** — static xref + decompile sufficient for acceptance.

## Remaining UNK

- Exact lobby semantics of `CGame+0x31` vs `+0xE2+slot*0x23` binding rows (known pattern from `CGame_IsLocalPlayerSlot`; full roster lifecycle is netcode scope).
- Whether script `0xEE`/`0xEF` subparam `2` is emitted by any shipped level script (toggle path proven in `OnEvent` only).

---

## Evidence

| Claim | Source |
|-------|--------|
| `IsHumanPlayer` branches | MCP `decompile_function@0x00416720` |
| Ctor `bIsAiVariant` / `bSlotKind` | MCP `decompile_function@0x0041e4b0` |
| 15 xrefs | MCP `get_xrefs_to@0x00416720` |
| Slot taxonomy | [map_slots_spawner.md](../../gameplay/map_slots_spawner.md) |
| Struct offsets | [CBulanek.md](../struct_recovery/CBulanek.md) |
| Net gate `CGame+0x36` | [gameplay_impact.md](../../netcode/gameplay_impact.md) |
