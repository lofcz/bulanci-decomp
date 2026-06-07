# Round 11 AI — Task 15 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 15 |
| **round** | 11 |
| **worker** | 15 / 30 |
| **title** | Practice dummies — `SpawnPracticeDummy` (slots `0x24..0x27`) |
| **archetype** | `stationary_target` |
| **seed_address** | `0x0041f5d0` |
| **addresses** | `0x0041f5d0`, `0x0041f230` |
| **acceptance** | Prove stationary (no TryBot?); shoot-only interaction; cloneFlag/register mode |

## Status

**DONE** — Live Ghidra MCP (`connect_instance("bulanci")`, `bulanci.exe`): spawn path, slot auto-alloc, `bEntityRegisterMode` preset, ctor scheduler differential vs co-op, and bullet hit pipeline disasm-proven. Cross-checked with R11 tasks 01/06/07/12/16.

## AI archetype

**`stationary_target`** — solo / tournament **pillow dummies** in entity slots **`0x24..0x27`** (decimal 36..39). Spawned only via `CGaming_SpawnPracticeDummy` (script opcode **101** `InsertOpponent`). Same `CBulanek` class (`0x7ec`) as players, but **no co-op walk pulse** (scheduler slot **5** omitted at ctor). Primary player interaction is **incoming projectile damage** (`CShot` → script `0xD7`); dummies do not run the human pickup-overlap loop in `CGaming_TickPlayerCollisions`.

| vs co-op `0x20..0x23` | Practice `0x24..0x27` |
|----------------------|------------------------|
| Ctor forces `nLives=1`, `nSpeed=0x28`, skin `4`, team `7` | Script `nLives` / `nSpeed` / `skinId` preserved |
| Scheduler slots **3, 4, 5** (slot 5 delay **100 ms** → periodic movement) | Scheduler slots **3, 4** only (delay **0**) |
| `gABulanekWalkAnimsSpecial` | `gABulanekWalkAnimsNormal` |
| Moving campaign ally | Stationary shoot target (tutorial / tournament) |

---

## Algorithm

### 1. Spawn gate — `CGaming_SpawnPracticeDummy` @ `0x0041f5d0`

```c
// void __thiscall CGaming_SpawnPracticeDummy(
//     CGaming *this,
//     void *teamColor, int nLives, int nSpeedParam, int skinPaletteId)

void CGaming_SpawnPracticeDummy(...) {
    int count = CGaming_CountOccupiedPlayerSlots(this);  // 0x00416970
    if (count >= 4)
        return;                                          // all four cells full

    CGaming_SetEntityRegisterMode(this, 1);                // bEntityRegisterMode@+0x341
    byte slotKind = (byte)(count + 0x24);                // ADD DL,0x24 @ 0x0041f5fa

    CGaming_SpawnAndInitializePlayer(this, slotKind,
        teamColor, nLives, nSpeedParam, skinPaletteId);  // 0x0041f500
}
```

**Disasm proof (`0x0041f5d0`–`0x0041f608`):**

| Address | Instruction | Meaning |
|---------|-------------|---------|
| `0x0041f5d3` | `CALL 0x00416970` | Occupancy count |
| `0x0041f5da` | `CMP EDX,0x4` / `JGE` | Fail when count ≥ 4 |
| `0x0041f5df` | `PUSH 1` / `CALL 0x004168c0` | `SetEntityRegisterMode(1)` |
| `0x0041f5fa` | `ADD DL,0x24` | `slotKind = count + 36` |
| `0x0041f602` | `CALL 0x0041f500` | Factory |

**Sole code xref:** `CLevelScriptOpExt_InsertOpponent` @ `0x0041f764` (opcode 101).

### 2. Slot auto-allocation — `CGaming_CountOccupiedPlayerSlots` @ `0x00416970`

```c
// Counts non-null pointers in apEntitySlots[0x24..0x27]
// CGaming+0xC8 + 0x24*4 == CGaming+0x158

int count = 0;
for (int *p = (int *)((char *)this + 0x158); loops < 4; p++)
    if (*p) count++;
return count;
```

First free slot index = `count + 0x24` → fills **`0x24`, `0x25`, `0x26`, `0x27`** in order.

### 3. Factory chain — `CGaming_SpawnAndInitializePlayer` @ `0x0041f500`

```
OperatorNew(0x19c)
  → CBulanekCtor(pGame, slotKind, pGamingHost=this, initialTrack=0,
                 teamColor, nLives, nSpeed, skinId)
  → CGaming_AddEntity(this, entity, 0xFF)     // does NOT rewrite +0x341 (mode stays 1)
  → CGaming_RegisterObjectAtSlot(this, entity, slotKind)  // bPlayerSlot@+0x70 = full slot
  → CGaming_RespawnPlayerAtSafeLocation
  → _rand() → CBulanek_SetFacingTrack(facing 0..3)
```

### 4. `cloneFlag` / register mode (`bEntityRegisterMode@CGaming+0x341`)

| Step | Effect |
|------|--------|
| `SetEntityRegisterMode(this, 1)` @ `0x0041f5e3` | `bEntityRegisterMode = 1` **before** entity alloc |
| `AddEntity(entity, 0xFF)` @ `0x0041f583` | `register_flag == -1` → **skips** mode write inside AddEntity |
| `AddEntity` mode **1** branch | `CDSView__AddChild` tail @ `+0x34C` + `CGaming_InsertEntityByDepth` (depth sibling insert) |

Historical name **cloneFlag** in R6 reports = **`bEntityRegisterMode`**, not a per-entity CBulanek field.

### 5. Stationary behavior (relative to co-op AI)

**Ctor scheduler registration** (`CBulanekCtor@0x0041e4b0`, disasm `0x0041ea1d`–`0x0041ea6a`):

```c
bool isCoop    = (slotKind - 0x20) <= 3;   // SUB AL,0x20; CMP AL,3 @ 0x0041e5e7
bool isPractice = (slotKind - 0x24) <= 3;  // SUB DL,0x24; CMP DL,3 @ 0x0041e5f2

// Always: Scheduler_RegisterEventSlot(&scheduler, 0, solo?0x96:0x1f4, 7)

if (isCoop) {
    RegisterEventSlot(3, 0, 7);
    RegisterEventSlot(4, 0, 7);
    RegisterEventSlot(5, 100, 7);          // 0x0041ea3b — co-op ONLY
} else if (isPractice) {
    RegisterEventSlot(3, 0, 7);            // 0x0041ea49
    RegisterEventSlot(4, 0, 7);            // 0x0041ea58 → RET (no slot 5)
} else if (slotKind > 3) {
    RegisterEventSlot(2, 0, 7);            // campaign AI 0x7d..0x7f
}
```

**`CBulanek_WeaponSchedulerCallback` case 5** (`0x00420b30`): periodic `vtable+0x24(0,0)` movement step — **only armed when ctor registered slot 5** (co-op). Practice dummies **never** register slot 5 → **no autonomous walk pulse**.

**TryBot path (clarification):** `CBulanek_TryBotRandomAction` **can** still run for `0x24..0x27` when scheduler **case 3** fires (`IsInKnockdownAnimBand@0x00416490` is a misnomer — it returns **1** for any slot in `0x20..0x27`). That path re-arms tournament think delays (`ArmTournamentSchedulerDelays`) and may call `ApplyAction` (move/fire). **Stationary** in reimplementation terms = **omit slot 5**; do not assume zero `TryBot` calls.

**Fire-delay exclusion:** `CBulanek_ArmFireDelayScheduler@0x00417260` returns immediately when `bPlayerSlot` ∈ `0x20..0x27` — practice dummies never get the 5–16 s PvP fire-delay slot **2** used by campaign AI.

### 6. Shoot-only interaction — `CGaming_SpawnBulletAndPlaySound` @ `0x0041f230`

Player → dummy damage chain (seed address `0x0041f230`):

```
CWeapon_Fire / net recv
  → CGaming_SpawnBulletAndPlaySound(this, origin, dir, weaponFlags, ...)  // 0x0041f230
       → OperatorNew(0xB0) + CShot_Ctor
       → CIntListInsertSortedOrAppend(CGaming+0x2C8, shot)
       → CGaming_AddEntity(shot, register_mode=1)
  → CShot::Update → CShot::TraceCollision → SpatialQuery
  → CShot_ResolveHit @ 0x0041dd70
       → CGaming_TryGetPlayerCoords(gaming, victimSlot@+0x70)
       → CGaming_OnSlotPlacementEvent(gaming, victimSlot, shooterSlot, dir, xy, authoritative=1)
            → CBulanek_PostScriptEvent(victim, 0xD7, ...)   // damage / life
            → CBulanek_PostScriptEvent(shooter, 0xD8, ...)   // score (if hit registered)
```

**Incoming-only pickup loop:** `CGaming_TickPlayerCollisions@0x0041f0c0` runs map-pickup overlap (`0x64..0x6c`) **only when** `CBulanek_IsHumanPlayer(pEntity)` — slots `0x24..0x27` fail the `< 4` branch and are not local humans, so **dummies do not collect world pickups** via this tick. Players shoot dummies; dummies are valid `SpatialQuery` / `CShot` targets via `CGaming_IsPlayerSlotOccupied`.

**Outgoing fire (edge):** Tournament dummies with weapons could still fire via scheduler case **3** → `TryBotRandomAction` → `ApplyAction(4)` or case **4** pickup-mask → arm slot **1**; tutorial solo targets typically have no pickup attachments. Document as **secondary** path, not primary shoot-target behavior.

---

## Functions table

| Address | Symbol | Role | Evidence |
|---------|--------|------|----------|
| `0x0041f5d0` | `CGaming_SpawnPracticeDummy` | **Seed.** Guard + mode 1 + auto slot | Decompile; disasm above |
| `0x0041f730` | `CLevelScriptOpExt_InsertOpponent` | Opcode 101 → 4 bytecode args → spawn | Xref `0x0041f764` |
| `0x00416970` | `CGaming_CountOccupiedPlayerSlots` | Count `apEntitySlots[0x24..0x27]` @ `+0x158` | Decompile loop `+0x158`, 4 iter |
| `0x004168c0` | `CGaming_SetEntityRegisterMode` | Writer `bEntityRegisterMode@+0x341` | `PUSH 1` @ `0x0041f5df` |
| `0x0041f500` | `CGaming_SpawnAndInitializePlayer` | `CBulanek` factory + register + respawn | Callee @ `0x0041f602` |
| `0x0041a390` | `CGaming_AddEntity` | View attach; mode from `+0x341` when flag ≠ `0xFF` | `AddEntity(...,-1)` @ spawn |
| `0x00417fb0` | `CGaming_RegisterObjectAtSlot` | `apEntitySlots[slot]@+0xC8`; `entity+0x70=slot` | After ctor |
| `0x0041e4b0` | `CBulanekCtor` | `isPractice` branch: sched 3/4, no slot 5 | Disasm `0x0041ea43`–`0x0041ea5a` |
| `0x00416490` | `CBulanek_IsInKnockdownAnimBand` | Gate `0x20..0x27` (incl. practice) for case 3 | Decompile |
| `0x00420b30` | `CBulanek_WeaponSchedulerCallback` | Case 5 = movement pulse (co-op only) | Case 5 @ `0x00420c30` region |
| `0x0041f230` | `CGaming_SpawnBulletAndPlaySound` | **Seed.** Bullet alloc + `AddEntity` + SFX | Decompile; [combat_projectiles.md](../../gameplay/combat_projectiles.md) |
| `0x0041dd70` | `CShot_ResolveHit` | Hit → `OnSlotPlacementEvent` | Victim `+0x70` |
| `0x00417e80` | `CGaming_OnSlotPlacementEvent` | Script `0xD7`/`0xD8` on hit | `param_5==0` damage path |
| `0x0041f0c0` | `CGaming_TickPlayerCollisions` | Pickup scan gated on `IsHumanPlayer` | Decompile |
| `0x00416720` | `CBulanek_IsHumanPlayer` | Net authority; not pickup gate for dummies | Task 07 |

---

## Struct fields

| Location | Field | Practice-dummy use |
|----------|-------|-------------------|
| `CGaming+0x341` | `bEntityRegisterMode` | Set to **1** before spawn (`depth-insert` AddEntity path) |
| `CGaming+0xC8` | `apEntitySlots[128]` | Slots `0x24..0x27` hold `CBulanek*` |
| `CGaming+0x158` | `apEntitySlots[0x24..0x27]` mirror | `CountOccupiedPlayerSlots` scan target |
| `CBulanek+0x70` | `bPlayerSlot` | Full slot `0x24..0x27` after `RegisterObjectAtSlot` |
| `CBulanek+0x124` | `bSlotKind` | Remapped `slotKind & 3` (0→3) for tables |
| `CBulanek+0x199` | `bIsAiVariant` | `1` (`slotKind > 3`) |
| `CBulanek+0x18C` | `nLivesRemaining` | From script (`InsertOpponent` arg 2) |
| `CBulanek+0x123` | `bTeamColor` | From script team arg if `< 8` |
| `CBulanek+0x88` | `scheduler` | Slots **3, 4** only at spawn (no **5**) |

---

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0041ea43` | Practice `0x24..0x27`: sched slots 3+4 only; no slot 5 walk pulse |
| `save_program` | `bulanci.exe` | Saved |

Prior rounds already documented `SpawnPracticeDummy` @ `0x0041f5fa` (`slot = count + 0x24`) and `SetEntityRegisterMode(1)` @ `0x0041f5e3`.

---

## Decomp fixes

| Issue | Ground truth | Notes |
|-------|--------------|-------|
| `extraout_DL` in `SpawnPracticeDummy` | `EDX = count`; `ADD DL,0x24` | Disasm @ `0x0041f5d8`–`0x0041f5fa` |
| `CountOccupiedPlayerSlots` name | Counts **`0x24..0x27`**, not humans `0..3` | `+0x158` = `+0xC8 + 0x90` |
| `cloneFlag` | `CGaming+0x341` `bEntityRegisterMode` | Not a CBulanek byte |
| `IsInKnockdownAnimBand` | Slot-range test `0x20..0x27` | Misleading name; true for practice slots |

---

## Frida

**none** — static disasm + xref closure sufficient.

---

## Remaining UNK

| Item | Notes |
|------|-------|
| `CGaming+0x158` struct field name | Proven mirror of `apEntitySlots[0x24..0x27]`; not yet in `CGaming.md` |
| Tournament dummy outbound fire | Case 3/4 can arm `ApplyAction` / slot 1; frequency in shipped levels not statically closed |
| Push / bump between human and dummy | No dedicated slot filter found; collision uses shared `CBulanek` rects |

---

## Cross-references

- [map_slots_spawner.md](../../gameplay/map_slots_spawner.md) — slot `36..39`
- [combat_projectiles.md](../../gameplay/combat_projectiles.md) — `0x0041f230` / `ResolveHit`
- [r11_ai_task_12_report.md](./r11_ai_task_12_report.md) — ctor band predicates
- [r11_ai_task_06_report.md](./r11_ai_task_06_report.md) — scheduler slots 3/4 re-arm
- [r11_ai_task_16_report.md](./r11_ai_task_16_report.md) — `InsertOpponent` bytecode (overlapping spawn entry)
