# Round 11 — AI Task 23 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 23 |
| **title** | CWeapon::Fire — AI mine self-hit and bot branches |
| **archetype** | `shooting_ai` |
| **seed_address** | `0x004212b0` |
| **addresses** | `0x004212b0`, `0x0041cb70` |
| **acceptance** | kind dispatch; `u16==0xFFFF` ammo-empty bot damage; `CMina` spawn |

## Status

**DONE** — Full `CWeapon::Fire` dispatch matrix, AI mine self-hit, empty-ammo human/bot branches, and `CMina_Ctor` spawn path documented from live Ghidra decompile + IDA `sub_4212B0` / `sub_41CB70` correlation. Plate comments applied on mine spawn, self-hit, and bot branches.

## AI archetype

**Shooting AI** — all non-human `CBulanek` slots (`bPlayerSlot > 3`) hit special cases inside `CWeapon::Fire` when deploying mines or firing on empty ammo (`FrameTimeHint u16 == 0xFFFF`). Co-op vampires (`0x20..0x23`) and practice dummies (`0x24..0x27`) diverge from campaign AI (`0x7d..0x7f`) and other bot slots.

## Algorithm

### Entry / parameters

`CWeapon::Fire(CWeapon *this, uint /*unused*/, ushort param_2)` — **IDSChained `vfn[4]`** subscriber for FLX opcode `0x0C` (`FrameTimeHint`). `param_2` is **not** frame delay ms.

| Field | Offset (canonical `CWeapon*`) | Role in `Fire` |
|-------|------------------------------|----------------|
| `pTrackHolder` | `+0x50` | Aiming / ammo `CBulanek*` (`DecrementWeaponAmmo`, spawn offsets, bot slot tests) |
| `pOwner` | `+0x54` | Passed to `CGaming_AddEntity` / `CShot_Ctor` map view (IDA: owner `CBulanek*`) |
| `pWorld` | `+0x58` | `CGaming*` from owner `+0x84` at ctor (special-pickup path uses `+0x54` in binary — see Decomp fixes) |
| `bWeaponKind` | `+0x64` | Dispatch byte `0..5` (mine=`2`, scatter family=`4`, pistol=`5`) |

**MI note:** Ghidra decompile names `dwParamB` @ `+0x60` for kind tests; IDA `sub_4212B0` reads `*(BYTE*)(this+96)` while ctor stores kind @ `this+100` — consistent when `Fire` receives **`this` adjusted by `+4`** (secondary `IDSChained` vtable facet @ `CWeapon+0x04`). Reimplementation should use canonical `bWeaponKind@+0x64` on unadjusted `CWeapon*`.

### Dispatch matrix (`param_2` × `bWeaponKind`)

| `bWeaponKind` | `param_2` | Action |
|---------------|-----------|--------|
| `2` (mine) | `0` | Spawn `CMina`; decrement ammo; **AI self-hit** if `bPlayerSlot > 3` |
| `4` (shotgun / MG / rocket overlay) | `!= 0xFFFF` | `CShot` with `weaponStrength = param_2 + 3`; `SetAmmo(4 - (param_2 < 3))` |
| `5` (pistol) | `0` | Decrement ammo; `CShot` with `weaponStrength = 2`; `SetAmmo(7)`; **return** |
| any | `0xFFFF` | Empty-ammo / bot branch (below) |
| other combos | — | No-op fall-through |

### Branch A — mine deploy (`kind==2`, `param_2==0`) @ `0x004212d8`

```text
mine = OperatorNew(0x118)
CMina_Ctor(mine, pTrackHolder)          // owner + spawn offset from holder facing
CGaming_AddEntity(pOwner, mine, 1)
CBulanek_DecrementWeaponAmmo(pTrackHolder)
if (pTrackHolder.bPlayerSlot > 3) {     // slots 4..127 — all AI / non-local-player
    CBulanek_OnTakeDamage(pTrackHolder, -1, -1, 0)
    mineSelfHit = true                    // stack flag; suppresses Branch C random harm
}
```

**AI mine self-hit:** Every AI-placed mine immediately damages the placer (stun / respawn gate path via `OnTakeDamage`, not script `0xD7`). Humans (`bPlayerSlot <= 3`) skip this call.

### Branch B — scatter / explosive shot (`kind==4`, `param_2 != 0xFFFF`) @ `0x0042134d`

```text
track = pTrackHolder.videoTrackManager.nCurrentTrackIdx & 0xFF
spawnX = pTrackHolder.nOrigin_x + gShotSpawnDeltaX[track]
spawnY = pTrackHolder.nOrigin_y + gShotSpawnDeltaY[track]   // DAT_004829d0/d4
shot = OperatorNew(0xB0)
CShot_Ctor(shot, &spawn, pOwner, track, pTrackHolder.bPlayerSlot, param_2 + 3, 0)
CGaming_AddEntity(pOwner, shot, 1)
SetAmmo(4 - (param_2 < 3))
```

### Branch C — pistol (`kind==5`, `param_2==0`) @ `0x0042140c`

Same spawn pattern with `DAT_004829b0/b4` offsets, `weaponStrength=2`, `SetAmmo(7)`, early return.

### Branch D — empty ammo / bot (`param_2==0xFFFF`) @ `0x004214cb`

```text
if (bWeaponKind != 0) {
    if (GetAmmoForKind(pTrackHolder, bWeaponKind) == 0) {
        if ((~CGaming.dwView_flags >> 4 & 1) == 0) {   // match not paused (bit 4 set on host)
            if (IsHumanPlayer(pTrackHolder))
                PostMessage(holder+0x10, 0x200, 0xD9, bWeaponKind, 0)   // out-of-ammo event
            if (pTrackHolder.bShotgunPickupGuard) {
                CGaming_SpawnSpecialPickupIfAllowed(pOwner, 0)
                pTrackHolder.bShotgunPickupGuard = 0
            }
        }
    }
}
if (pTrackHolder.bPlayerSlot > 3 && !mineSelfHit) {
    slot = pTrackHolder.bPlayerSlot
    if ((slot > 0x1F && slot < 0x24) || (slot > 0x23 && slot < 0x28)) {
        CBulanek_TryBotRandomAction(pTrackHolder)    // coop 0x20..0x23 OR dummies 0x24..0x27
        return
    }
    if (RandInRange(0, 99) < 0x28)                  // 40% (40/100)
        CBulanek_OnTakeDamage(pTrackHolder, -1, -1, 0)
}
```

| Slot range | Role | `0xFFFF` bot behavior |
|------------|------|------------------------|
| `0..3` | Humans | Out-of-ammo message only; no self-damage |
| `4..0x1F`, `0x28..0x7C`, `0x80+` | Other AI / script slots | **40%** random `OnTakeDamage` |
| `0x20..0x23` | Co-op vampires | `TryBotRandomAction` (idle / move / fire lottery) |
| `0x24..0x27` | Practice dummies | Same `TryBotRandomAction` path |
| `0x7D..0x7F` | Campaign AI | **40%** random self-harm (not vampire/dummy ranges) |

`TryBotRandomAction@0x00420a90` (R11 task 3): gated by `IsHumanPlayer` + weapon scheduler slot-0 armed bit; `_rand` idle walk vs `_rand*4` action `0..3` through `CanDispatchPlayerAction` → `ApplyAction`.

### `CMina_Ctor` weapon spawn (`0x0041cb70`)

Called only from mine branch with `param_2 = pTrackHolder` (`CBulanek*`):

```text
CAnim subobject + CDSUpdatedItem @ +0xF0
pOwnerBulanek = param_2
bArmed = 0
nDeployOrOwnerCtx = *(param_2 + 0x84)     // CGaming host scratch
spawn origin = holder.nOrigin + gMineDeployDelta[facing @ holder+0xD4]
entity type 0xFFFFFFF2; TM bind mine anim RES 0x100B1
```

Map-placed mines use `InitMine@0x0041cce0` instead (documented in `combat_projectiles.md`).

## Functions table

| Address | Symbol | Role | Evidence |
|---------|--------|------|----------|
| `0x004212b0` | `CWeapon::Fire` | Weapon overlay dispatch; AI mine self-hit + bot empty-fire | Live decompile; IDA `sub_4212B0`; body `0x212b0..0x215ac` |
| `0x0041cb70` | `CMina::CMina_Ctor` | Weapon-deployed mine ctor | Called @ `0x00421310`; IDA `sub_41CB70` |
| `0x0041a390` | `CGaming_AddEntity` | Register mine/shot in world | CALL @ `0x00421327`, `0x004213eb`, `0x004214a7` |
| `0x00417500` | `CBulanek_DecrementWeaponAmmo` | Mine/pistol ammo decrement | CALL @ `0x0042132f`, `0x00421420` |
| `0x0041db00` | `CBulanek_OnTakeDamage` | AI self-hit (mine + 40% empty-fire) | CALL @ `0x00421343`, `0x00421593` |
| `0x00416640` | `CBulanek_GetAmmoForKind` | Empty-ammo test | CALL @ `0x004214de` |
| `0x00416720` | `CBulanek_IsHumanPlayer` | Human-only `0xD9` post | CALL @ `0x004214f7` |
| `0x0042f390` | `CDSView__PostMessage` | Queue `0xD9` out-of-ammo | CALL @ `0x00421517` |
| `0x0041f1d0` | `CGaming_SpawnSpecialPickupIfAllowed` | Shotgun-training pickup respawn | CALL @ `0x0042152d` |
| `0x00420a90` | `CBulanek_TryBotRandomAction` | Vampire/dummy empty-fire AI | CALL @ `0x0042155f` |
| `0x0040ace0` | `RandInRange` | `0..99` inclusive; `<40` → self-harm | CALL @ `0x0042157d` |
| `0x0041edf0` | `CShot::CShot_Ctor` | Bullet spawn | Shot branches |
| `0x004179a0` | `CWeapon::SetAmmo` | HUD ammo / sound | After shot branches |

## Struct fields

| Object | Offset | Name | `Fire` use |
|--------|--------|------|------------|
| `CWeapon` | `+0x50` | `pTrackHolder` | Aim, ammo, bot slot, damage target |
| `CWeapon` | `+0x54` | `pOwner` | `AddEntity` / `CShot` map view host |
| `CWeapon` | `+0x64` | `bWeaponKind` | Dispatch byte (`2`/`4`/`5`) |
| `CBulanek` | `+0x70` | `bPlayerSlot` | `>3` AI gate; vampire/dummy range tests |
| `CBulanek` | `+0x16B` | `bShotgunPickupGuard` | Special pickup respawn on empty ammo |
| `CBulanek` | `+0x11C+kind` | ammo bytes | `GetAmmoForKind` |
| `CGaming` | `+0x44` | `dwView_flags` | Pause bit 4 — empty-ammo FX gate |

## Ghidra deltas

| Action | Target |
|--------|--------|
| `set_decompiler_comment` | `0x004212d8` — mine spawn `kind==2`, `param_2==0` |
| `set_decompiler_comment` | `0x00421337` — AI mine self-hit `bPlayerSlot>3` |
| `set_decompiler_comment` | `0x0042153f` — `0xFFFF` bot branch slot ranges |
| `save_program` | `bulanci.exe` |

## Decomp fixes

| Issue | Correction |
|-------|------------|
| Kind tests on `dwParamB@+0x60` | Canonical field is `bWeaponKind@+0x64`; `Fire` entry uses MI-adjusted `this+4` (IDA `this+96` ≡ `+0x64`) |
| `CGaming_AddEntity((CGaming*)this->pOwner)` | IDA passes `CWeapon+0x54` (`CBulanek*` owner); treat as map/entity host — same pointer used for `CShot_Ctor` `CGameView*` |
| `SetAmmo((CWeapon*)&this[-1].pHudIconB, …)` | Bogus pointer arithmetic; real target is `CWeapon*` (`this` adjusted back one facet) |
| `SpawnSpecialPickupIfAllowed(this->pOwner)` | IDA uses `this+84` (owner slot); function expects `CGaming*` — use `pWorld@+0x58` in clean reimplementation |

## Frida

Not required — branch predicates and slot constants closed via disasm + IDA export.

## Remaining UNK

- Whether campaign slots `0x7D..0x7F` intentionally use 40% self-harm vs a dedicated AI idle hook (static proof only shows range exclusion from `TryBotRandomAction`).
- Exact `gMineDeployDelta` table (`DAT_004827d4/d8`) per facing byte — spawn math delegated to `CMina_Ctor` (task 26 danger zones).
- Net replication of AI self-hit damage (local `OnTakeDamage` only; no `NetSend` in `Fire` body).

## Doc cross-refs

- `ghidra_analysis/gameplay/combat_projectiles.md` — `CWeapon::Fire` overview, `CMina` chain
- `ghidra_analysis/gameplay/damage_pipeline.md` — `OnTakeDamage` caller row, `0xD9` event
- `ghidra_analysis/engine/struct_recovery/round4_task_31_report.md` — `param_2` / `0xFFFF` semantics
- `ghidra_analysis/gameplay/map_slots_spawner.md` — slot taxonomy `0x20..0x27`
