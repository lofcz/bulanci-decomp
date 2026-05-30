# Combat, Projectiles & Environmental Subsystems

A reverse-engineering analysis of the weapon, projectile, explosion, power-up effects, and teleportation systems in *Bulánci* (`bulanci.exe`).

---

## 1. Subsystem Architecture Overview

This subsystem is responsible for handling all offensive player actions, projectable and static environmental threats, and area-of-effect (AOE) damage computations. It consists of the following tightly coupled classes:

| Class | Size (Allocation) | Base Class | Primary Responsibility |
|---|---|---|---|
| `CWeapon` | — | None | Managed firing controller for players (`CBulanek`). Orchestrates ammo consumption, firing rates, and projectile spawning. |
| `CShot` | **`0xb0` bytes** (`OperatorNewWithBadAlloc`) | `CDSView` + `CDSUpdatedItem` @ `+0x88` | Projectile entity (single bullet or 5-pellet spread). Sub-frame trace collision; net spawn opcode **`0x0F`**. |
| `CMina` | `0x118` bytes | `CDSView` | Deployable landmines. Overlapped by players to trigger chain-reaction-capable explosions. |
| `CExplosion` | `0xf4` bytes | `CDSView` | Explosion controller. Orchestrates radial area-of-effect (AOE) raycasting, damage allocation, and chain-detonating of neighbor mines. |
| `CSpells` | `0x80` bytes | `ODSImage` | Visual power-up indicators (e.g., Shields, Invisibility bubbles) attached to player characters. |
| `CTeleportPoint`| — | `CDSView` | Map-embedded portals that transport players instantly between defined destination coordinates. |

---

## 2. Component Analysis

### A. `CWeapon` (Weapon / Firing Controller)

`CWeapon` acts as the coordinator between the player input and the instantiation of projectiles or environmental hazards.

#### Core Trigger Logic: `CWeapon::Fire` (Address: `0x004212b0`)
When a player pulls the trigger, `CWeapon::Fire` processes the action based on the weapon type variable at `this[0x60]` and trigger state `param_2`:

```cpp
void __thiscall CWeapon::Fire(CWeapon *this, undefined4 param_1, ushort trigger_state);
```

1. **Mine Deployment (Type `0x02`):**
   - If `trigger_state == 0` (trigger pulled), allocates a landmine entity of size `0x118` (`_Globals::FUN_00447c42(0x118)`).
   - Calls the `CMina` constructor (`FUN_0041cb70`) passing the owner `CBulanek` pointer (`*(int *)(this + 0x50)`).
   - Adds the mine entity to the game world via `CBulanek::AddEntity` (`FUN_0041a390`).
   - Plays firing audio and updates the ammo count.
   
2. **Standard & Scatter Projectiles (Type `0x04`):**
   - Calculates bullet spawn offsets based on the shooter's current facing direction (`uVar9 = shooter->direction`).
   - Spawns a `CShot` projectile entity of size `0xb0` using `CGameView::SpawnShot` (`FUN_0041edf0`).
   - Registers the projectile into the world entity manager.
   - Plays weapon-specific firing sound (e.g., pistol shot, shotgun blast).

3. **Magical Spells (Type `0x05`):**
   - Triggers spell capabilities (such as deploying shield/barrier or initiating invisibility timer).
   - Decrements spell-specific charge.

---

### B. `CShot` (Projectile Entity)

MSVC **multiple-inheritance** object: primary `CDSView` vtables at `+0`, `+4`, `+0x10`, `+0x18`; embedded **`CDSUpdatedItem`** at **`+0x88`** (scheduler / timer). Allocation size **`0xb0`**.

#### Object layout (`sizeof == 0xb0`)

| Offset | Size | Field | Notes |
|--------|------|-------|-------|
| `+0x00` | 4 | `vftable` (primary) | → `0x00481cbc` |
| `+0x04` | 4 | `vftable` (secondary) | → `0x00481ca0` |
| `+0x10` | 4 | `vftable` (tertiary) | → `0x00481c88` |
| `+0x18` | 4 | `vftable` (quaternary) | → `0x00481c74` |
| `+0x20` | 16 | `worldRect` | `left, top, right, bottom` — logical position |
| `+0x30` | 16 | `screenRect` | Used by `CShot_Draw` for single-pellet blit |
| `+0x44` … `+0x87` | — | `CDSView` / entity base | See `widgets.md` (`+0x4c` parent, flags at `+0x68`…) |
| `+0x68` | 1 | `movementEnabled` | Set by `CGameEntity_SetEntityType` (`0x00418fe0`) |
| `+0x6c` | 4 | `entityTypeId` | **`0x0F`** for bullets (`CShot_Ctor`) |
| `+0x84` | 4 | `pCGaming` | Owning `CGaming*`; set in `CBulanek::AddEntity` (`0x0041a390`) |
| `+0x88` | 0x18+ | `CDSUpdatedItem` | `Scheduler_RegisterEventSlot(this+0x88, 0, 0x32, 6)` |
| `+0x88` | 4 | `vftable` (updated item) | → `0x00481c5c` |
| `+0xa0` | 4 | `pBulletFrames` | From `CGameView::FUN_00417f40` (sprite strip) |
| `+0xa4` | 1 | `direction` | Facing byte (0–3 cardinal) |
| `+0xa5` | 1 | `ownerSlotId` | Shooter slot; `0xFF` = neutral |
| `+0xa6` | 1 | `weaponStrength` | `0`/`>1` → single pellet path in `Update`; **`1` or `2`** → 5-pellet loop |
| `+0xa7` | 1 | `pelletMask` | Bit *i* = pellet *i* alive; init **`0x1F`**; cleared on hit |
| `+0xa8` | 1 | `expired` | `CGaming_CleanupInactiveBullets` tests `[0x2a]` (= `+0xa8`) |
| `+0xac` | 4 | `scatterJitter` | Only if `weaponStrength > 2` after clamp: `rand()*9/32768 - 4` |

Shared **`CDSView`** fields used by all game entities: `+0x20` rect, `+0x84` world pointer, `+0x68`/`+0x6c` type flags.

#### Core Constructor: `CShot_Ctor` (`0x0041edf0`)

```cpp
CShot* __thiscall CShot_Ctor(
    CShot *this,
    int *spawnRect,      // param_2 → copies to +0x20, sizes from frame strip
    CGameView *mapView,  // param_3 — spatial queries + frame lookup
    byte direction,      // +0xa4
    byte ownerSlotId,    // +0xa5
    byte weaponStrength, // +0xa6 (clamped to 3)
    int frameVariant);   // param_7 → FUN_00417f40
```

Flow:

1. `CDSChained_ctor` → temporary `CGameView::vftable` → `CDSUpdatedItem_ctor` @ `+0x88` → final **`CShot`** vtables.
2. `CGameEntity_SetEntityType(this, 0x0F)` — entity class id for scene graph.
3. `Scheduler_RegisterEventSlot(this+0x88, 0, 0x32, 6)` — 50 ms tick slot.
4. Spawn overlap test: `SpatialQuery` on `+0x20`; hit → `CShot_ResolveHit`, `expired=1`, hide + arm scheduler.
5. If `weaponStrength > 2`: write `scatterJitter` at `+0xac` (pellet mask still `0x1F`).

#### Spawn pipeline: `CGaming_SpawnBulletAndPlaySound` (`0x0041f230`)

| Step | Behavior |
|------|----------|
| Post message | `CDSView__PostMessage(this+0x10, 0x200, 0xF4, …)` |
| Allocate | `OperatorNewWithBadAlloc(0xB0)` + `CShot_Ctor` when `(param_5 & 3) != 0` |
| `weaponStrength` arg | `(param_5 & 3) - 1` → `0`, `1`, or `2` |
| List insert | `CIntListInsertSortedOrAppend(this+0x2C8, shot, …)` — **`CGaming` bullet list** |
| Register | `CBulanek::AddEntity(gaming, shot, 1)` → sets `shot+0x84` |
| SFX | `param_5 & 0xF0`: `0x10`→sample 21, `0x20`→13, `0x40`→19 |

#### Hit resolution: `CShot_ResolveHit` (`0x0041dd70`)

- `weaponStrength == 2` (`+0xa6`): spawns **`CExplosion`** (`0xF4`) at `this+0x20` (rocket / explosive shot).
- Else: `CGaming_TryGetPlayerCoords` → **`CGaming_OnSlotPlacementEvent`** (`0x00417e80`) with `param_5=1` → **`CGame_NetSendShotSpawn_t0f`** (opcode **`0x0F`**, 8 bytes) when victim slot occupied.

#### `CGaming_OnSlotPlacementEvent` (`0x00417e80`)

| `param_5` | Effect |
|-----------|--------|
| `!= 0` | If shooter slot live → **`NetSendShotSpawn_t0f`** (sync spawn) |
| `== 0` | Teleport victim to impact coords; script events **`0xD7`** (damage) / **`0xD8`** (score) |

#### Core Update: `CShot::Update` (`0x0041df60`)

Calls `CGameView::FUN_00419010` (rect move + grid cell update via vtable `+0x74`).

| `weaponStrength` (`+0xa6`) | Collision path |
|----------------------------|----------------|
| `0` or `> 1` | One `TraceCollision(newRect, oldRect@+0x20)`; hit → `expired=1` |
| `1` or `2` | For each set bit in `pelletMask` (`+0xa7`): `CShot_ComputePelletRect` ×2, `TraceCollision`; clear bit on hit; mask `== 0` → `expired=1` |

#### Collision tracing: `CShot::TraceCollision` (`0x0041de60`)

Sub-step loop **`i = 1..4`**: lerp `(new-old)*i/4`, `FUN_00433200` (rect normalize), `SpatialQuery(pCGaming@+0x84, …)`. Hit → `CShot_ResolveHit`. If no hit, accept when rect inside **`[0..800)×[0..0x204)`**.

#### Draw: `CShot_Draw` (`0x00417bd0`)

| `weaponStrength` (`+0xa6`) | Blit |
|----------------------------|------|
| **`1` only** (`!= 0` and `!= 2`) | Up to 5 pellets via `pelletMask` + `+0x20` |
| **`0` or `2`** | Single blit from **`+0x30`** (`screenRect`) |

Note: **`Update`** treats both **`1` and `2`** as multi-pellet collision; **`Draw`** only multi-blits for type **`1`** (type **`2`** is explosive — see `CShot_ResolveHit`).

#### Pellet geometry: `CShot_ComputePelletRect` (`0x00417b30`)

Offsets pellet *index* `param_3` (0–4) along the axis perpendicular to `direction` (`+0xa4`), using frame width/height from `pBulletFrames+4/+8`.

#### Vtable map (primary `CDSView` facet @ `0x00481cbc`)

| Slot | Address | Symbol (Ghidra) |
|------|---------|-----------------|
| 0 | `0x004170e0` | `CShot::FUN_004170e0` — RTTI / class id (`DAT_004b37bc`) |
| 1 | `0x00417190` | scalar deleting dtor |
| 2–10 | `0x004245c0` … `0x0042c580` | shared `CDSView` plumbing |
| **11** | **`0x0041df60`** | **`CShot::Update`** |
| 12–13 | `0x0042c430`, `0x0042c3e0` | layout / focus |
| **14** | **`0x00417bd0`** | **`CShot_Draw`** |
| 19 | `0x0042cf50` | `CDSView_OnLButtonDownAcquireFocus` |
| 22–24 | `0x0042c0c0` … `0x0042c100` | show/hide/end-modal |
| **27** | **`0x0041acf0`** | **`CGameView` event dispatch** (inherits) |
| **30** | **`0x0041b1d0`** | scheduler hook (reads `this+0x88` slot 0) |

**`CDSUpdatedItem` facet** @ `0x00481c5c` (`this+0x88`): slots include `0x0041a980`, `0x00418ed0`, `0x00417170`, `0x00419b40` (timer / track callbacks).

**Contrast — `CGameView` gameplay facet** @ `0x00481b14`: slot **11** = `0x00419010` (generic entity move) instead of `CShot::Update`.

#### Bullet list on `CGaming`

| Offset | Field |
|--------|-------|
| `+0x2C8` | `CIntList` head / vector for active `CShot*` |
| `+0x2D0` | element count (cleanup walks backward) |

`CGaming_CleanupInactiveBullets` (`0x0041b?` region): removes shots with **`+0xa8 != 0`**, calls dtor vtable slot 2.

---

### C. `CMina` (Landmine Entity)

The landmine is a stationary, deployable trap that triggers an explosive shockwave when stepped on by any player.

#### Core Constructor: `CMina::CMina` (Address: `0x0041cce0`)
Initializes the mine entity, binds the sprite anim bank, sets the owner pointer at `this[0x42]` (`this + 0x108`), and sets the armed state `this[0x114] = 0` (starts unarmed, armed after a brief landing animation delay).

#### Event/Message Callback: `CMina::OnEvent` (Address: `0x0041efb0`)
```cpp
void __thiscall CMina::OnEvent(CMina *this, short eventId, undefined4 param_2, undefined4 *param_3);
```
- Listens for environment/physics events.
- If `eventId == 0xf2` (Player Step-On/Trigger Event):
  - Invokes `_Globals::ExplodeMine` (`FUN_0041e070`).

#### Detonation: `CMina::Explode` (Address: `0x0041e070`)
```cpp
void __fastcall CMina::Explode(CMina *mineObj);
```
1. Verifies if the mine is armed (`mineObj[0x114] != 0`). If unarmed, ignores the step event.
2. Sets armed flag `mineObj[0x114] = 0` to prevent duplicate trigger conditions.
3. Unregisters the mine from the active scene rendering and deletes it (`_Globals::FUN_004165b0`).
4. Resolves the owner player's ID slot from the shooter pointer:
   `ownerId = mineObj->ownerBulanek ? mineObj->ownerBulanek->slotId : 0xff`
5. Allocates a `CExplosion` object (`FUN_00447c42(0xf4)`), constructs it at the mine's center coordinates (`CExplosion::FUN_0041ce30`), and assigns the `ownerId` to credit subsequent blast kills properly.
6. Spawns the explosion into the main world entity list (`CBulanek::AddEntity`).

---

### D. `CExplosion` (Explosion & AOE Damage System)

The explosion system handles both visual FX animation and complex, multi-directional raycast area-of-effect (AOE) damage mechanics.

#### Constructor: `CExplosion::CExplosion` (Address: `0x0041ce30`)
Registers explosion vtables, loads the frame list, sets the owner's player slot ID at `this[0xf0]`, and triggers the explosion sound effect.

#### Event Dispatcher: `CExplosion::OnEvent` (Address: `0x0041efe0`)
Processes explosion state phases:
- `param_2 == 0` (Animation Start): Calls `CExplosion::ApplyAreaDamage` (`FUN_0041e140`). This ensures damage is processed on the exact frame the explosion begins.
- `param_2 == -1` (Animation End): Calls clean-up routines to unregister the visual FX from the renderer.

#### Area Damage & Raycast: `CExplosion::ApplyAreaDamage` (Address: `0x0041e140`)
This function contains the core damage engine of *Bulánci*:
1. **Blast Zone Definition:**
   Expands the explosion's core bounding box by 60 pixels on all sides:
   $$\text{BlastX}_{\text{min}} = X_{\text{min}} - 60 \qquad \text{BlastX}_{\text{max}} = X_{\text{max}} + 60$$
   $$\text{BlastY}_{\text{min}} = Y_{\text{min}} - 60 \qquad \text{BlastY}_{\text{max}} = Y_{\text{max}} + 60$$
2. **Entity Queries:**
   Queries all entities residing inside this expanded area using a spatial query (`FUN_004183d0`) and stores the results in a local stack buffer.
3. **Epicenter Damage:**
   Applies full damage to any entity lying directly at the epicenter using `CExplosion::DamageAtPoint` (`FUN_0041b250`).
4. **8-Directional Raycast Sweep:**
   To simulate physical shockwave blocking (walls/obstacles protecting a player from a nearby blast), the engine performs a raycast sweep:
   - Loops through 8 directions using step increments defined in the static table `UNK_004828c0`.
   - For each direction, it steps outwards up to 15 times.
   - At each step, it runs `CExplosion::DamageAtPoint` (`FUN_0041b250`).
   - If `DamageAtPoint` detects an obstacle (such as a solid wall), **the raycast in that direction terminates immediately**, shielding any entities sitting further down that trajectory!
5. **Chain Detonation of Mines:**
   Queries all other landmines within the blast radius using `FUN_0041a2f0`.
   - For each found landmine, it immediately calls `_Globals::ExplodeMine` (`FUN_0041e070`).
   - This creates rapid, highly destructive landmine chain-reactions.

---

### E. `CSpells` (Powerup Visual Effects)

The `CSpells` class handles drawing visual effects (e.g., active barrier shields, transparency invisibility) overlaying player characters.

- **Status Bitmask (`this + 0x78`):**
  Uses events `0xee` (powerup started) and `0xef` (powerup ended) to toggle active spell status bits:
  - Bit 0: Transparent/Invisibility status.
  - Bit 1: Shield / Protective barrier status.
- **Dynamic Blitting (`CSpells::Draw` @ `0x004278c0`):**
  If any spell state is active, it tracks the player's primary screen coordinates (`this + 0x30`, `this + 0x34`), reads the corresponding circular particle or glowing border frames, and blits them dynamically around the player.

---

### F. `CTeleportPoint` (Teleport Gate)

Portals/teleporters placed in maps are represented by `CTeleportPoint`.

- **Portal Trigger Callback (`CTeleportPoint::OnEvent` @ `0x0041fed0`):**
  When a player collides with a teleport gate:
  - Fetches the overlapping player character (`CBulanek`) pointer at `this + 100`.
  - Determines destination coordinates from its sister portal node.
  - Instantly teleports the player entity to the target gate coordinates using `CBulanek::SetPosition` (`FUN_0042cc80`):
    ```cpp
    CBulanek::SetPosition(targetPlayer, destX, destY);
    ```
  - Triggers a teleportation visual effect and plays the teleport audio cue (`FUN_0041fca0`).

---

## 3. Discovered Global Constants & Data Pools

- `UNK_004828c0` / `UNK_004828c4`: Step delta coordinates array defining 8 radial search directions (X, Y) used during explosion AOE raycasting.
- `DAT_004b3730`: Global parameter specifying explosion visual assets / audio sequence resource indices.
- `0xf2`: Physics trigger event (e.g. stepping on a mine).
- `0xee` / `0xef`: Message IDs indicating start and end states of magical spells/powerups.
- `0xd7` / `0xd8`: Game script events used to report bullet impact damage and player score/kill modifications respectively.
