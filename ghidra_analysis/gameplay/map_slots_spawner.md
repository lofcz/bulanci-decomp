# Map World Slots & Dynamic Pickup Spawner Deep-Dive

This document provides a comprehensive reverse-engineered specification of the map slots and spawning system in *Bulánci* (`bulanci.exe`). It details the memory structures, slot allocations for all 128 possible slots, spawning limits, and exact spawning mathematics required for byte-perfect or logic-compatible reimplementation.

---

## 1. The Global Entity Slot Registry (`m_apEntitySlots`)

At the core of game state orchestration is a fixed-size registry array of active entities managed by `CGaming`. This registry guarantees that entities (players, dummies, weapons, mines, or script widgets) can be queried stably and synchronized deterministically across the network or level scripts via single-byte identifiers (Slot IDs).

### Memory Structure

The slot registry is an array of 128 object pointers:

```cpp
CGameObject* m_apEntitySlots[128]; // Size: 128 * 4 = 512 bytes (0x200 bytes)
```

- **Location:** Inside the `CGaming` object (which is size `0x36c` bytes), starting at offset `+200` (decimal, or `0xc8` hex).
- **Initialization:** During `CGaming` construction (`CGaming_ctor` at `0x0041ff90`), the entire array is cleared to zero:
  ```cpp
  Runtime::MSVCRT::_memset(this + 200, 0, 0x200);
  ```

### Registration & Unregistration Contracts

#### Registration: `_Globals::CGaming_RegisterObjectAtSlot` (Address: `0x00417fb0`)
When an entity is spawned, it is bound to a slot:
- Writes the entity pointer to `m_apEntitySlots[slot_id]`.
- Enables the registration flag on the entity: `entity->flags |= 0x200` (offset `+0x14`).
- Records the slot ID inside the entity itself: `entity->slot_id = slot_id` (offset `+0x70`).

#### Unregistration: `CGaming::CGaming_UnregisterAndRemoveObject` (Address: `0x00419ca0`)
When an entity is collected, detonated, or destroyed, it is unbound:
- Passes a dummy slot index `0xff` (or any value $\ge \text{128}$) to the registrar.
- Clears the original slot array cell: `m_apEntitySlots[entity->slot_id] = nullptr`.
- Resets the entity's internal slot assignment: `entity->slot_id = 0xff`.
- Removes the entity from the graphical/logical hierarchy of child views.

---

## 2. Exhaustive Slot Allocations (All 128 Slots)

The 128 slots are divided into strict functional zones. Reimplementations and map scripts must respect these functional boundaries:

| Slot Range (Decimal) | Slot Range (Hex) | Assigned Class | Purpose / Role |
|---|---|---|---|
| **`0..3`** | `0x00..0x03` | `CBulanek` | **Primary Active Players:** Slots for human players (Players 1 through 4) in local or multiplayer matches. Spawned in `_Globals::FUN_004205a0` based on active player count. |
| **`4..31`** | `0x04..0x1f` | *Any (Script-Defined)* | **First Script Gap (28 Slots):** Free space left for level scripts to spawn scenery or script-controlled objects. Starts at 4 (after human players) and ends at 31 (before co-op allies). |
| **`32..35`** | `0x20..0x23` | `CBulanek` | **AI Bots / Co-op Partners:** Player-like slots used for computer-controlled allies or cooperative campaign partners. Spawned in `_Globals::FUN_00420550`. |
| **`36..39`** | `0x24..0x27` | `CBulanek` (Dummies) | **Solo Practice Targets:** The stationary green pillow target dummies spawned during solo training. Spawned in `_Globals::FUN_0041f5d0`. |
| **`40..99`** | `0x28..0x63` | *Any (Script-Defined)* | **Second Script Gap (60 Slots):** Large unreserved block for level scripts to place custom animated hazards, interactive props, or scenery. Starts at 40 (after training dummies) and ends at 99 (before the special pickup). |
| **`100`** | `0x64` | `CIcon` (Special) | **Special World Pickup:** Reserved strictly for the Hourglass / Clock special pickup. |
| **`101..107`** | `0x65..0x6b` | `CIcon` (Weapons/Mines) | **General World Pickups:** Reserved for map-spawned weapons (Machine Gun, Rocket Launcher) and mines. |
| **`108..124`** | `0x6c..0x7c` | *Any (Script-Defined)* | **Third Script Gap (17 Slots):** Open slots for custom level script entity bindings. Starts at 108 (after general pickups) and ends at 124 (before singleplayer campaign enemies). |
| **`125..127`** | `0x7d..0x7f` | `CBulanek` | **Singleplayer Campaign AI Enemies:** Campaign AI opponent slots spawned in `_Globals::FUN_004205a0` (slots `125..127` / `0x7d..0x7f`) when the active human player count is exactly 1. |

---

## 3. The Architecture of Script-Controlled Gaps

A crucial architectural question is: **Why are there three distinct open script gaps (`4..31`, `40..99`, and `108..124`), and how does the engine place elements into them?**

The answers lie in the layout design of the static and dynamic object-allocation loops in `bulanci.exe`:

### 1. The Gaps are "Leftover" Spaces
The three script ranges are not intentionally partitioned by design; rather, they are the **natural leftover gaps** inside the contiguous 128-element pointer array (`m_apEntitySlots`) left between strict, hardcoded engine-level allocations. 
- Because player-like characters, dummies, campaign AI enemies, and dynamic map weapon/special spawners are hardcoded to check specific slot boundaries, any index not claimed by an engine system was left open for level script-writers to use.

### 2. How are Elements Placed into these Gaps?
- **Hard-coded Spawners Bypass Gaps:** The engine's automatic pickup spawner (`CGaming::CGaming_RandomPickupSpawner_Tick`) and character-spawn loops *never* touch the script gaps. They are restricted to checking and writing to slots `0..3` (players), `32..35` (bots), `36..39` (dummies), `100` (hourglass), `101..107` (weapons/mines), and `125..127` (campaign enemies).
- **Explicit Script Allocation (`BindToSlot`):** Placing items in these gaps is done entirely via Level Scripts (`CLevelScript`) loaded from map payload resources. Level scripts execute compiling bytecode instructions such as `BindToSlot` (Opcode 56 / `_Globals::FUN_00418770`) to bind script-instantiated views (like a moving obstacle or static decor) to specific slots.

### 3. Level-Design Conventions
To maintain compatibility and prevent scripts from accidentally clobbering active players or weapons, level designers adhered to the following strict conventions when writing script files:
- **`4..31` (28 Slots):** Standard range for small maps or simple custom scripting triggers.
- **`40..99` (60 Slots):** The main "sandbox" range. This massive range is where highly interactive levels (such as the train map with moving carriages or maps with complex timing-based lasers/hazards) place all their active scenery, switches, and dangerous obstacles.
- **`108..124` (17 Slots):** Auxiliary scripting space, usually reserved for secondary interactive decorative widgets or scenery that need to be turned on/off programmatically.

---

## 4. Deep-Dive into Subsystem Behaviors

### Player and Bot Slots (`0..7` & `32..39`)
The game engine treats player characters (`CBulanek`, Class ID `2028` / `0x7ec`) with special priority. 
- During core physics ticks and collision evaluations, the engine performs fast checks using `CGaming_IsPlayerSlotOccupied` (`0x00417e40`), which validates if an entity resides at a slot index $< 128$ and checks if its Class ID equals `0x7ec`.
- Unchecked player-ranking loops in `CGaming_ProcessRoundStateAndScoring` (`0x0041f350`) read player metadata from the active player slots dynamically, finding the winner or determining if round ending criteria have been achieved.

### Dynamic World Pickups (`100` & `101..107`)
The spawner logic specifically splits world item pickups into separate logical slots:
- **Slot `100`** acts as a strict singleton slot for the Hourglass. The host's periodic spawner verifies slot occupancy *only* at index `100` to decide if a new Special pickup can be placed.
- **Slots `101..107`** are treated as an iterative buffer. General pickups are checked in a linear loop starting from slot `101` and proceeding upwards. The first vacant slot in this range is assigned the newly generated item. If no vacant slot is found, the spawn fails silently (preventing map cluttering beyond 7 concurrent items).

### Special Interactive Script Slots (`125..127`)
Unlike the general level scenery slots, slots `125`, `126`, and `127` are hard-coded into key engine-loop lifecycles to enable interactive level triggers:
1. **Resume Game Hook (`CGaming_OnResumeOrStartGame` @ `0x0041c140`):**
   When the match starts or resumes from a pause, the engine executes a safe lookup at slot `127` (`0x7f`). If slot `127` is occupied, it runs a downward loop through slots `127`, `126`, and `125`:
   ```cpp
   iVar2 = CGaming_GetObjectAtSlotSafe(this, 127);
   if (iVar2 != 0) {
       char slot = 3;
       do {
           piVar1 = (int*)CGaming_GetObjectAtSlotUnchecked(this, 128 - slot);
           CGaming_TickPlayerCollisions(this, piVar1);
           slot = slot - 1;
       } while (slot != 0);
   }
   ```
   This ensures that interactive switches or active triggers registered in these slots are woken up, ticked, and synchronized instantly.

2. **Trace Area & Collider Overlaps (`CMina::FUN_00419fd0`):**
   When resolving step-on triggers, collider overlaps, or proximity traps, the engine runs a dedicated loop over player slots and specifically includes slots `125..127` (since single-player opponents reside in those slots and need to trigger traps or step on mines).
   This allows campaign AI opponents placed in slots `125..127` to interact perfectly with mine and projectile physics natively.

---

## 6. Compiled Level Script & Editor Integration

The Level Editor (`Editor.exe`) compiles map geometry, resource links, and trigger bytecode into `.eap` bundles. Within these scripts, map designers manipulate world slots dynamically using specific bytecode commands:

### Binding Views: `BindToSlot` (Opcode 56 / Address `0x00418770`)
A script-created view (such as an animated decoration `CreateAnim` or a static sprite image `CreateImage`) is registered to a specific slot dynamically via Opcode 56:
```cpp
// Script bytecode translates to:
CGaming_RegisterObjectAtSlot(g_pGaming, view_pointer, slot_id);
```
Once bound, `view->slot_id` (offset `+0x70`) is set to the slot index.

### Lifecycle Callbacks: `OnBitmapEvt` (Message `0xD7` / Address `0x00418260`)
When a script-created image or animation bound to a slot triggers an internal frame boundary or animation marker event, it redirects the event directly to the running script:
- It fetches its bound slot index from offset `+0x70`.
- It invokes the script lifecycle slot 3 (`OnBitmapEvt`), passing:
  - `Local0 = slot_id`
  - `Local1 = event_code` (such as frame index achieved or playback completed).
This architecture allows level scripts to react dynamically to scenery events (e.g., when a custom gate animation completes opening, trigger a pathing refresh for bots).

---

## 5. Spawning Mathematics & Probabilities

Spawning is driven periodically by the host on match ticks via `CGaming::CGaming_RandomPickupSpawner_Tick` (Address: `0x0041e350`). 

### Random Generation Formula

Let $S$ be the match weapon spawner bound stored at `CGaming + 0x368`:
- $S = 2$ in **Shotgun-Only Mode** (Special Pickups Only).
- $S = 6$ in **All-Weapons Mode**.

The random value `rand()` (from MSVCRT, in range `[0..32767]`) is scaled and normalized as follows:

1. **Initial Range Scale:**
   $$V = \text{floor}\left( \frac{\text{rand()} \times (S \times 25 - 25)}{32768} \right)$$
   - If $S = 6$ (All-Weapons), $V \in [0..124]$.
   - If $S = 2$ (Shotgun-Only), $V \in [0..24]$.

2. **Uniform Division:**
   $$c = \text{floor}\left(\frac{V}{25}\right)$$
   - If $S = 6$, $c \in [0, 1, 2, 3, 4]$.
   - If $S = 2$, $c = 0$.

3. **Kind Selection ($b$):**
   - $b_{\text{initial}} = c + 1$.
   - If $b_{\text{initial}} > 2$, then $b = c + 2$ (this skips kind 3, which is unused/undefined in the engine).
   - Otherwise, $b = b_{\text{initial}}$.

This produces the following uniform attempt probabilities in **All-Weapons Mode ($S = 6$)**:

| Generated Value ($c$) | Computed Kind ($b$) | Target Entity | Attempt Probability | Sprite ID | Resource Table |
|---|---|---|---|---|---|
| `0` | **Kind 1** | Special Pickup (Hourglass) | **20%** | `0x000100b3` | `g_PickupKind_SpriteIdTable[1]` (`0x004827f8`) |
| `1` | **Kind 2** | Mine | **20%** | `0x0001010c` | `g_PickupKind_SpriteIdTable[2]` (`0x004827fc`) |
| `2` | **Kind 4** | Machine Gun | **20%** | `0x000100de` | `g_PickupKind_SpriteIdTable[4]` (`0x00482804`) |
| `3` | **Kind 5** | Rocket Launcher | **20%** | `0x000100db` | `g_PickupKind_SpriteIdTable[5]` (`0x00482808`) |
| `4` | **Kind 6** | *None* (Bypasses Spawner) | **20%** | — | — |

In **Shotgun-Only Mode ($S = 2$)**, $c$ is always `0`, yielding a **100%** attempt rate for **Kind 1** (Special Pickup).

*Note: Kind 3 is unused/undefined in the engine and is explicitly skipped by the spawner kind-selection step. It has a placeholder Sprite ID value of `-1` at `g_PickupKind_SpriteIdTable[3]` (`0x00482800`).*

---

### Saturation & Redirection Logic

When a spawn attempt is made, progressive map saturation alters the outcome dynamically:

#### Case A: Kind 1 (Special Pickup) is Selected
The spawner queries slot `100`:
1. **If Slot `100` is empty:** Spawns the Hourglass in slot `100`. (Spawn succeeds).
2. **If Slot `100` is occupied:** The kind is overridden and **redirected to Kind 4 (Machine Gun)**:
   - **In All-Weapons Mode ($S = 6$):** The redirected Kind 4 is checked against the boundary ($4 < 6$ is true). The spawner attempts to place a Machine Gun instead.
     - *Mathematical Effect:* The probability of a Machine Gun attempt increases from **20% to 40%** when the Hourglass is on the ground.
   - **In Shotgun-Only Mode ($S = 2$):** The redirected Kind 4 is checked against the boundary ($4 < 2$ is false). The spawn attempt is **disallowed and fails silently**.
     - *Mathematical Effect:* No weapon can ever spawn in Shotgun-Only mode. Thus, once the Hourglass is spawned, the actual successful spawn rate on all subsequent ticks drops to **0%** until the Hourglass is picked up.

#### Case B: Kind 2, 4, or 5 (General Pickups) is Selected
The spawner executes an iterative scan over slots `101..107` via `_Globals::CGaming_SpawnPickupAndBroadcast` (Address: `0x0041e2c0`):
- It tests slot `101`. If non-empty, it tests `102`, up to `107`.
- **If an empty slot is found:** Allocates the pickup, assigns it to the first found slot, and broadcasts the event over the network (DP msg `0x18`). (Spawn succeeds).
- **If all slots `101..107` are occupied:** The loop bounds check triggers (`slot_index > 107`), and the function returns immediately. The item is discarded. (Spawn fails silently).
  - *Mathematical Effect:* As the ground gets saturated with weapons and mines, the probability of spawning a new weapon/mine decays:
    - **0 to 6 items on ground:** Spawning success for weapons/mines is $100\%$ of attempts (net $60\%$ of total ticks, or $80\%$ if Hourglass is present).
    - **7 items on ground:** Spawning success for weapons/mines drops to **0%** of attempts. Only the Special pickup can still spawn (if slot 100 is empty, with a $20\%$ chance).

---

## 7. Summary for Reimplementation

To maintain exact behavioral parity with the MSVC C++ engine:
1. Maintain an explicit 128-pointer array, reserving `0..3` for players, `32..35` for bots/co-op, `36..39` for training targets, `100` for the hourglass, `101..107` for weapons and mines, and `125..127` for campaign AI enemies.
2. Support the script opcode `BindToSlot` (56) to allow level scripts to load custom dynamic/scenery entities into the three open script gaps (`4..31`, `40..99`, and `108..124`) dynamically.
3. Ensure campaign AI opponents placed in slots `125..127` interact perfectly with mine and projectile physics natively.
4. Maintain a strict cap of 7 simultaneous weapon/mine pickups.
5. Keep the Hourglass bound to a single instance at slot 100.
6. Implement the exact random distribution scaling by $S$ (spawner bound).
7. Ensure that if the Hourglass is on the ground, Hourglass spawn attempts are redirected to Machine Guns (all-weapons mode) or discarded (shotgun-only mode).
8. Implement the linear slot-search in `[101..107]` for general pickups, returning immediately without spawning once slot `107` is exceeded.
