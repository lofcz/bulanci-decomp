# Combat, Projectiles & Environmental Subsystems

A reverse-engineering analysis of the weapon, projectile, explosion, power-up effects, and teleportation systems in *Bulánci* (`bulanci.exe`).

---

## 1. Subsystem Architecture Overview

This subsystem is responsible for handling all offensive player actions, projectable and static environmental threats, and area-of-effect (AOE) damage computations. It consists of the following tightly coupled classes:

| Class | Size (Allocation) | Base Class | Primary Responsibility |
|---|---|---|---|
| `CWeapon` | — | None | Managed firing controller for players (`CBulanek`). Orchestrates ammo consumption, firing rates, and projectile spawning. |
| `CShot` | `~0xac` bytes | `CDSView` | Projectile entity (single-bullet or multi-projectile scatter). Includes sub-frame path interpolation and collision detection. |
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

The `CShot` class handles moving projectiles, supporting both single direct bullets and multi-bullet shotgun-like spread patterns.

#### Core Constructor: `CShot::CShot` (Address: `0x0041edf0`)
```cpp
CShot* __thiscall CShot::CShot(
    CShot *this, 
    int *startPos, 
    void *gamingState, 
    byte direction, 
    CGameView param_4, 
    byte weaponStrength, 
    int param_6
);
```
- Sets up direction `this[0xa4] = direction`.
- Sets owner player ID `this[0xa5] = shooterId`.
- Sets weapon strength level `this[0xa6] = weaponStrength`.
- Loads corresponding bullet frame sequence from the sprite bank (`FUN_00417f40`).
- If `weaponStrength > 2` (scatter/spread weapons like Shotgun):
  - Sets sub-shots mask to `this[0xa7] = 0x1f` (`11111` in binary, designating 5 active sub-projectiles).
  - Multiplies trajectory path by a random scatter angle:
    $$\Delta \text{angle} = \frac{\text{rand()} \times 9}{32768} - 4 \quad \in [-4, 4] \text{ pixels}$$
- Executes an immediate collision check upon spawning via `_Globals::SpatialQuery` (`FUN_00418300`). If a target or wall is in zero-distance contact, it triggers immediate hit resolution and destroys the projectile.

#### Core Update/Tick: `CShot::Update` (Address: `0x0041df60`)
```cpp
void __thiscall CShot::Update(CShot *this, int *newPosRect);
```
Updates the projectile coordinates based on velocity. If the projectile has multiple sub-shots (`this[0xa6]` is scatter), it iterates over all active sub-shot bits (0 to 4), calculates each sub-projectile's local bounding box (`FUN_00417b30`), and executes trace collision.

#### Precise Collision Tracing: `CShot::TraceCollision` (Address: `0x0041de60`)
```cpp
uint __thiscall CShot::TraceCollision(CShot *this, int *newRect, int *oldRect, byte *outHitFlag);
```
To prevent bullets from skipping through targets at high velocities ("tunneling"), `CShot` implements a sub-frame interpolation trace loop:
1. Steps along the trajectory vector, interpolating the coordinate delta up to 4 sub-steps per tick.
2. At each sub-step, it generates a temporary sub-frame bounding box and runs a spatial obstacle/entity query (`_Globals::FUN_00418300`).
3. If an intersection is discovered, it breaks the interpolation early and executes `_Globals::ResolveHit` (`FUN_0041dd70`) at that specific intersection point.

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
