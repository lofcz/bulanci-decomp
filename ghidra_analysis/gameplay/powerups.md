# Power-ups & Item Spawner Subsystem

A reverse-engineering analysis of the dynamic pickup spawner, item collection, player inventory state, and active status/spell effects in *Bulánci* (`bulanci.exe`).

---

## 1. Subsystem Architecture Overview

The power-ups and item spawner subsystem is responsible for orchestrating the lifecycle of collectible pickups (weapons, ammo, and special items) dropped on the map. The system governs spawning, networking synchronization, item collection, and active status indicators.

### Primary Classes & Entities

| Class / Struct | Size | Base Class | Responsibility |
|---|---|---|---|
| `CGaming` | — | `CDSChained` | Matches-level driver. Coordinates the tick-based spawner and manages the world slot registrations for entities. |
| `CBulanek` | `~6.5 KB` | `CDSView` / `ODSImage` | Player entity. Holds ammo inventory array at `+0x11c` and pending respawn flags at `+0x16b`. |
| `CWeapon` | — | `CDSObject` | Weapon/projectile spawning controller. |
| `CSpells` | `0x80` bytes | `ODSImage` | Passive/active magic status visualizer (renders shields, invisibility bubbles, and clocks above the player). |

---

## 2. Spawning Mechanism

### A. The Random Spawner: `CGaming::CGaming_RandomPickupSpawner_Tick` (Address: `0x0041e350`)

This function ticks on the active host instance to determine if a new pickup should be placed in the game world.

```cpp
int __fastcall CGaming::CGaming_RandomPickupSpawner_Tick(void *param_1)
```

1. **Host-Only & State Gates:**
   - Obtains the pointer to `CGame` at `this + 0x84`.
   - Verifies if the match is active and not client-only/paused by checking `*(char *)(CGame + 0x36) == '\0'`. If client/paused, the spawner tick is bypassed.

2. **Random Kind Selection:**
   - Obtains the spawner upper bound `*(int *)(CGaming + 0x368)`.
   - Generates a random kind using the MSVCRT `rand()` function:
     - The range is normalized to $kind \in [1, \text{bound} - 1]$.
     - If the kind selection is greater than 2, it skips kind 3 (which is unused/skipped) and maps to $kind \in [4, 5]$ by doing `kind = cVar5 + 2`.
   - **Weapon Mode Boundaries:**
     - **Shotgun-only mode**: `*(int *)(CGaming + 0x368)` is initialized to `2`. Only $kind < 2$ can be generated, restricting spawning entirely to **Kind 1 (Special Pickup)**.
     - **All-weapons mode**: `*(int *)(CGaming + 0x368)` is initialized to `6`. All kinds can be generated, enabling **Kind 1 (Special)**, **Kind 2 (Mines)**, **Kind 4 (Machine Gun)**, and **Kind 5 (Rocket Launcher)**.

3. **Special Slot Verification:**
   - If `kind == 1` (Special pickup), the spawner checks if the reserved world slot `100` is already occupied via `CGaming_GetObjectAtSlotSafe(this, 100)`.
   - If slot `100` is already occupied, the spawner overrides the kind to `4` (Machine Gun) and confirms that total active items do not exceed maximum parameters before triggering.

4. **Instantiation & Broadcast:**
   - Triggers `CGaming_SpawnPickupAndBroadcast` (`0x0041e2c0`).

---

### B. Allocation & Registration: `CGaming_SpawnPickupAndBroadcast` (Address: `0x0041e2c0`)

```cpp
void __thiscall _Globals::CGaming_SpawnPickupAndBroadcast(void *this, uint param_1)
```

- **Special Pickup (Kind 1):** Spawns the pickup directly at reserved world slot `100`. Renders using sprite resource ID `0x000100B3` (loaded from `g_PickupKind_SpriteIdTable[1]`). Broadcasts the placement using DirectPlay message **`0x13`** (`CGame_OnNetMsg_t13_PlaceSpecialPickup` / `CGame_NetSendWorldEvent_t13`).
- **General Pickups (Kind 2..5):**
  - Searches iteratively for an empty map world slot in the range `[101..107]`.
  - Once an empty slot is located, allocates the pickup entity via `CGame_SpawnPickupObject` (`0x0041d240`) mapping to its kind-specific sprite:
    - Kind 2 (Mines): `0x0001010C`
    - Kind 4 (Machine Gun): `0x000100DE`
    - Kind 5 (Rocket Launcher): `0x000100DB`
  - Registers the entity to the active board collision list.
  - Broadcasts placement to all clients using DirectPlay message **`0x18`** (`CGame_OnNetMsg_t18_PlaceWorldPickup` / `CGame_NetSendPlaceObject_t18`).

---

## 3. Pickup Collection & Apply Logic

### `_Globals::CGame_ApplyPickup` (Address: `0x0041eba0`)

This function is triggered when a player (`CBulanek`) collides with a world pickup.

```cpp
void __thiscall _Globals::CGame_ApplyPickup(void *this, byte param_1, byte param_2)
```

1. **Entity Deletion:**
   - Resolves the pickup's world slot (`param_2`) to obtain the entity pointer.
   - De-registers the pickup from the board collision and invokes its virtual destructor to clean up the graphical/audio resources.

2. **Inventory Refills:**
   - State/ammo updates are written into the player's weapon slot array at `this + 0x11c + kind`.
   - **Kind 1 (Special Pickup / Hourglass):** Refills special inventory slot `this + 0x11d`. If shotgun-only mode is active (`*(CGaming + 0x368) == 2`), the player's guard flag `this[0x16b]` is set to `1`.
     - *Guard Flag Purpose:* Because only one special pickup spawns in shotgun-only mode, if the player dies (`CBulanek_OnDeath` @ `0x0041f900`) or depletes their ammunition (`CWeapon::Fire` @ `0x004212b0`), this flag guarantees a fresh special pickup spawns back on the map so the item is never permanently lost.
   - **Kind 2 (Mines):** Increments mine count `this + 0x11e` by 1 up to maximum capacity `4` (queried from `g_kMaxAmmoTable[2]`).
   - **Kind 4 & 5 (Machine Gun / Rocket Launcher):** Directly refills ammo slots `this + 0x120` and `this + 0x121` to their maximum capacities (4 and 2 respectively) as defined in `g_kMaxAmmoTable`.

---

## 4. Active Status & Spell Effects (`CSpells`)

Floating visual indicators (shield bubbles, clocks, invisibility eyes) shown above a player's head are managed by the `CSpells` subclass.

### A. Lifecycle State Machine: `CSpells::CSpells__OnEvent` (Address: `0x00426da0`)

This acts as the network and scheduler listener for spell activation and deactivation.
- **Spell Triggered (Event `0xee`):** Computes Bit-Shift $1 \ll \text{spell\_id}$. Enables the matching bit in the spell mask at `CSpells + 0x78`.
- **Spell Terminated (Event `0xef`):** Disables the matching bit in the spell mask: `mask = mask & ~(1 << spell_id)`.

### B. Status Render Layout: `CSpells::CSpells__Draw` (Address: `0x004278c0`)

The subsystem holds three pre-baked texture coordinates mapping to visual indicators:

| Bit | Active Spell / Power-up | Texture Source Rect | Representation |
|---|---|---|---|
| **Bit 0 (`1 << 0`)** | **Shield / Armor (Brnění)** | `(25, 0, 36, 14)` | Absorbs one shot of direct damage. |
| **Bit 1 (`1 << 1`)** | **Hourglass (Přesýpací Hodiny)**| `(0, 0, 11, 14)` | Freezes/slows opponent players. |
| **Bit 2 (`1 << 2`)** | **Invisibility Eye (Oko)** | `(13, 0, 23, 14)` | Blends player with the background. |

---

## 5. Reverse-Engineering Symbol Reference

### Analyzed & Renamed Functions

| Address | Original Symbol | Assigned Symbol | Function & Purpose |
|---|---|---|---|
| `0x0041e350` | `FUN_0041e350` | `CGaming_RandomPickupSpawner_Tick` | Periodic timer checking/resolving whether to spawn a random world pickup. |
| `0x0041e2c0` | `FUN_0041e2c0` | `CGaming_SpawnPickupAndBroadcast` | Places kind-specific pickup on map and broadcasts via DirectPlay messages. |
| `0x0041eba0` | `FUN_0041eba0` | `CGame_ApplyPickup` | Resolves collision, deletes map pickup, and updates player's inventory array. |
| `0x0041ea90` | `FUN_0041ea90` | `CBulanek_ApplyPickupEffect` | Triggers weapon change visual/audio feedback when player collects a pickup. |
| `0x0041f900` | `FUN_0041f900` | `CBulanek_OnDeath` | Handles inventory wipe, respawn timers, and guard flag checks for shotgun-only modes. |
| `0x00412490` | `FUN_00412490` | `GetMaxAmmoForKind` | Retrieves max ammo from the global read-only limits table `g_kMaxAmmoTable`. |
| `0x0041ed60` | `FUN_0041ed60` | `DetonatePlayerMines` | Remote-detonates active mines in the map world linked to the player's slot. |
| `0x004212b0` | `FUN_004212b0` | `CWeapon::CWeapon__Fire` | Orchestrates projectile spawning, ammo consumption, and shotgun training respawn triggers. |

### Global Data Structures

#### `g_kMaxAmmoTable` (Address: `0x00481a60`)
Byte array defining maximum ammo capacities for each entity kind index:
`05 04 04 01 04 02 00 00`
- Kind 0 (Pistol): `5` shots
- Kind 1 (Special): `4` charges/duration
- Kind 2 (Mines): `4` max mines
- Kind 4 (Machine Gun): `4` bursts
- Kind 5 (Rocket Launcher): `2` rockets

#### `g_PickupKind_SpriteIdTable` (Address: `0x004827f4`)
Dword array mapping entity index to overlay sprite resource ID:
- Index 1: `0x000100B3` (Special Pickup sprite)
- Index 2: `0x0001010C` (Mine Pickup sprite)
- Index 4: `0x000100DE` (Machine Gun Pickup sprite)
- Index 5: `0x000100DB` (Rocket Launcher Pickup sprite)
