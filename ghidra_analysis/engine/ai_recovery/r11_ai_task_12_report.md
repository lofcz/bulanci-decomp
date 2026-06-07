# Round 11 — AI Task 12 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 12 |
| **title** | CGaming_SpawnAndInitializePlayer + CBulanekCtor slotKind |
| **archetype** | spawn |
| **seed_address** | `0x0041f500` |
| **addresses** | `0x0041f500`, `0x0041e4b0` |
| **acceptance** | All ctor branches for slotKind `0..3`, `0x20..0x27`, `0x7d..0x7f`; speed / nLives / team color params |

## Status

**DONE** — Live Ghidra MCP (`connect_instance("bulanci")`, `bulanci.exe`): full decompile + disasm for `CGaming_SpawnAndInitializePlayer` and `CBulanekCtor`; caller chain traced through `CGaming_SpawnPlayerAtSlot` and `CGaming_SpawnPracticeDummy`. All slotKind band predicates disasm-proven @ `0x0041e5e1`–`0x0041ea6f`. No Ghidra mutations required (comments/prototypes already applied by prior rounds).

## AI archetype

| slotKind (hex) | Band predicate | Spawn entry | ctor AI flag | Scheduler extras (beyond slot 0) |
|----------------|----------------|-------------|--------------|----------------------------------|
| `0x00..0x03` | `slotKind ≤ 3` | `CBulanek_SpawnPlayerAndCampaignSlots` → `CGaming_SpawnPlayerAtSlot` | `bIsAiVariant = 0` | **none** — early return after slot 0 only |
| `0x20..0x23` | `(slotKind - 0x20) ≤ 3` | `CGaming_SpawnCoopPartnerSlots` → `CGaming_SpawnPlayerAtSlot` | `bIsAiVariant = 1` | slots **3, 4, 5** (delays 0, 0, **100**) |
| `0x24..0x27` | `(slotKind - 0x24) ≤ 3` | `CGaming_SpawnPracticeDummy` → direct call | `bIsAiVariant = 1` | slots **3, 4** (delay 0) |
| `0x7d..0x7f` | `slotKind > 3` ∧ not coop ∧ not practice | `CBulanek_SpawnPlayerAndCampaignSlots` (solo only) → `CGaming_SpawnPlayerAtSlot` | `bIsAiVariant = 1` | slot **2** (delay 0) |

## Algorithm

### `CGaming_SpawnAndInitializePlayer` @ `0x0041f500`

**Signature (live Ghidra):**

```c
void __thiscall CGaming_SpawnAndInitializePlayer(
    CGaming *this,
    byte slotKind,
    void *pTeamColorOverride,
    int nLives,
    int nSpeedParam,
    int skinPaletteId);
```

**Pseudocode (disasm @ `0x0041f526`–`0x0041f5b1`):**

```
entity = OperatorNewWithBadAlloc(0x19c)
if (entity) {
  CBulanekCtor(
    entity,
    this->pOwnerGame,          // [this+0x84]
    slotKind,
    this,                      // pGamingHostScratch during ctor
    initialTrack = 0,          // hardcoded PUSH 0 @ 0x0041f561
    pTeamColorOverride,
    nLives,
    nSpeedParam,
    skinPaletteId);
}
CGaming_AddEntity(this, entity, register_flag = 0xFF)   // @ 0x0041f583
CGaming_RegisterObjectAtSlot(this, entity, slotKind)  // @ 0x0041f58c
CGaming_RespawnPlayerAtSafeLocation(entity)             // @ 0x0041f592
facing = (_rand() * 4) >> 15                            // 0..3 @ 0x0041f599
CBulanek_SetFacingTrack(entity, facing, sendNet = 0)    // @ 0x0041f5b1
```

**Caller parameter matrix (proven):**

| Caller | Address | slotKind | pTeamColorOverride | nLives | nSpeedParam | skinPaletteId |
|--------|---------|----------|-------------------|--------|-------------|---------------|
| `CGaming_SpawnPlayerAtSlot` | `0x00420530` | `param_1` | `0xFFFFFFFF` | **1** | **100** | **0** |
| `CGaming_SpawnPracticeDummy` | `0x0041f5d0` | `CountOccupiedPlayerSlots + 0x24` | script `teamColor` | script `nLives` | script `speed` | script `skinId` |

`CGaming_SpawnPlayerAtSlot` is used for **humans** (`0..N-1`), **co-op vampires** (`0x20..0x23`), and **campaign AI** (`0x7d..0x7f` when `bTotalSlots == 1`). Tournament / multi-life dummies use **`CGaming_SpawnPracticeDummy`** only (script `InsertOpponent` opcode).

### `CBulanekCtor` @ `0x0041e4b0`

**Signature (live Ghidra):**

```c
CBulanek *__thiscall CBulanekCtor(
    CBulanek *this,
    CGame *pGame,
    uchar slotKind,
    void *pGamingHostScratch,
    int initialTrack,
    void *pTeamColorOverride,
    int nLives,
    int nSpeedParam,
    int skinPaletteId);
```

**Band predicates (disasm @ `0x0041e5e1`–`0x0041e5f8`):**

```
isCoopSlot    = ((slotKind - 0x20) <= 3)   // 0x20..0x23
isPracticeSlot = ((slotKind - 0x24) <= 3)  // 0x24..0x27
```

**Speed clamp (`nSpeedParam` → `this+0x194`, disasm @ `0x0041e575`–`0x0041e5dd`):**

```
if (nSpeedParam >= 0xC9)  nSpeedParam = 200
else if (nSpeedParam < 0x28)  nSpeedParam = 40
// else unchanged
if (isCoopSlot)  nSpeedParam = 0x28   // overrides caller @ 0x0041e62d
```

**Lives / skin overrides (co-op only, @ `0x0041e631`–`0x0041e639`):**

```
if (isCoopSlot) {
  nLives = 1
  skinPaletteId = 4
}
this->nLivesRemaining = nLives      // +0x18C @ 0x0041e782
this->nLivesSnapshot  = nLives      // +0x190 @ 0x0041e788
```

**AI variant flag (@ `0x0041e645`–`0x0041e64e`):**

```
this->bIsAiVariant = (slotKind > 3)   // +0x199
```

**`bSlotKind` remap for AI slots (@ `0x0041e77c`–`0x0041e79b`):**

```
this->bSlotKind = slotKind            // +0x124
if (slotKind > 3) {
  this->bSlotKind = slotKind & 3
  if ((slotKind & 3) == 0)
    this->bSlotKind = 3
}
```

| slotKind | stored `bSlotKind` |
|----------|-------------------|
| `0x20`, `0x24` | **3** |
| `0x21`, `0x25`, `0x7d` | **1** |
| `0x22`, `0x26`, `0x7e` | **2** |
| `0x23`, `0x27`, `0x7f` | **3** |

**Team color (`bTeamColor` @ `+0x123`, @ `0x0041e7e0`–`0x0041e839`):**

```
if (slotKind <= 3)
  bTeamColor = CGameGetPlayerColorByte(pGame, slotKind)
else
  bTeamColor = (_rand() * 8) >> 15          // 0..7

if (isCoopSlot)
  bTeamColor = 7
else if (isPracticeSlot && pTeamColorOverride < 8)
  bTeamColor = (byte)pTeamColorOverride
```

**Walk animation table (@ `0x0041e719`–`0x0041e723`):**

```
walkTable = &gABulanekWalkAnimsNormal   // default
if (isCoopSlot)
  walkTable = &gABulanekWalkAnimsSpecial
// 4-direction AddTrackSource loop follows
```

**Skin palette when `skinPaletteId < 0` (@ `0x0041e5fd`–`0x0041e623`):**

```
skinPaletteId = gAPlayerSkinPaletteIds[(_rand() * 4) >> 15]
// skipped when isCoopSlot forces skinPaletteId = 4
```

**AI danger-zone nodes (slotKind > 3, @ `0x0041e677`–`0x0041e6ad`):**

```
for i in 0..3:
  apAiTrackHolders[i] = CGaming_AppendDangerZoneNode(pGamingHostScratch, i, zeroRect, 0, this)
```

**Ammo (@ `0x0041e7b5`–`0x0041e7d6`):**

```
bAmmoKind0 = GetMaxAmmoForKind(0)           // always
if (slotKind > 4)
  bAmmoKind1 = GetMaxAmmoForKind(1)         // all AI bands (coop, practice, campaign)
```

**Scheduler slot 0 (always, @ `0x0041e9f0`–`0x0041ea18`):**

```
delay0 = (pGame->bTotalSlots == 1) ? 0x96 : 0x1f4
// disasm: SBB trick — solo 150 ms, multi 500 ms
Scheduler_RegisterEventSlot(&scheduler, slot=0, delay0, eventKind=7)
```

**Scheduler tail branches (@ `0x0041ea1d`–`0x0041ea6a`):**

```
if (isCoopSlot) {
  Scheduler_RegisterEventSlot(&scheduler, 3, 0, 7)
  Scheduler_RegisterEventSlot(&scheduler, 4, 0, 7)
  Scheduler_RegisterEventSlot(&scheduler, 5, 100, 7)
}
else if (isPracticeSlot) {
  Scheduler_RegisterEventSlot(&scheduler, 3, 0, 7)
  Scheduler_RegisterEventSlot(&scheduler, 4, 0, 7)
}
else if (slotKind > 3) {          // campaign AI 0x7d..0x7f
  Scheduler_RegisterEventSlot(&scheduler, 2, 0, 7)
}
else {                            // human 0..3
  return this                     // no extra slots
}
```

**Speed → frame delay (@ `0x0041e91c`–`0x0041e95e`, when `nSpeedParam != 100`):**

```
delayMs = round(47.25 / (nSpeedParam / 100.0))
TM_SetFrameDelayOverrideMs(&videoTrackManager, delayMs)
```

## Functions table

| Address | Symbol | Role | Evidence |
|---------|--------|------|----------|
| `0x0041f500` | `CGaming_SpawnAndInitializePlayer` | Heap alloc `0x19c`, ctor, view attach, slot bind, respawn, random facing | Decompile; disasm `PUSH 0x19c` @ `0x0041f526`, ctor @ `0x0041f568`, AddEntity @ `0x0041f583`, Register @ `0x0041f58c` |
| `0x0041e4b0` | `CBulanekCtor` | Per-slotKind initialization; scheduler AI branches | Decompile + full disasm `0x0041e4b0`–`0x0041ea84` |
| `0x00420530` | `CGaming_SpawnPlayerAtSlot` | Thin wrapper: `(slot, 0xFFFFFFFF, 1, 100, 0)` | Decompile; xref to spawn @ `0x0042053d` |
| `0x004205a0` | `CBulanek_SpawnPlayerAndCampaignSlots` | Humans `0..bTotalSlots-1`; if solo, `0x7d..0x7f` | Decompile |
| `0x00420550` | `CGaming_SpawnCoopPartnerSlots` | Slots `0x20..0x23`; hide slots `>0` | Decompile |
| `0x0041f5d0` | `CGaming_SpawnPracticeDummy` | Auto slot `count+0x24`; passes script lives/speed/skin | Decompile; `ADD DL,0x24` path documented @ `0x0041f5fa` (R10) |
| `0x0041a390` | `CGaming_AddEntity` | View-tree attach; `entity+0x84 = CGaming*` | R10 task 03 |
| `0x00417fb0` | `CGaming_RegisterObjectAtSlot` | `apEntitySlots[slot]@+0xC8` | R10 task 03 |
| `0x00416970` | `CGaming_CountOccupiedPlayerSlots` | Counts `CGaming+0x158..0x167` | R10 task 03 |
| `0x0041b420` | `CBulanek_AllocAiTrackHolder` | AI track holder alloc in danger-zone loop | Disasm @ `0x0041e69c` |
| `0x00412930` | `CGameGetPlayerColorByte` | Human team color by slot index | Call @ `0x0041e810` |
| `0x0042f210` | `Scheduler_RegisterEventSlot` | Registers scheduler slots 0..5, kind 7 | Calls @ `0x0041ea18`..`0x0041ea6a` |

## Struct fields

| Offset | Name | slotKind branch behavior |
|--------|------|--------------------------|
| `+0x123` | `bTeamColor` | Human: `CGameGetPlayerColorByte`; AI: `_rand()%8`; coop **7**; practice: script override if `<8` |
| `+0x124` | `bSlotKind` | Raw for humans; AI: `slotKind & 3`, zero → **3** |
| `+0x18C` | `nLivesRemaining` | From caller `nLives`; coop forced **1** |
| `+0x190` | `nLivesSnapshot` | Copy of `nLives` at ctor |
| `+0x194` | `nSpeedParam` | Clamped `[40,200]`; coop forced **40** (`0x28`) |
| `+0x199` | `bIsAiVariant` | `slotKind > 3` |
| `+0x178` | `apAiTrackHolders[4]` | Filled when `slotKind > 3` |
| `+0x88` | `scheduler` | Slot 0 always; tail slots 2/3/4/5 per band table above |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| — | — | **No mutations this task** — prior rounds already applied ctor prototype, `CBulanek *` this-type, scheduler/videoTrackManager comments, spawn wrapper docs |

## Decomp fixes

| Issue | Ground truth | Notes |
|-------|--------------|-------|
| `CGaming_RespawnPlayerAtSafeLocation(this_00, …)` | `ECX` not set before call @ `0x0041f592` | Ghidra artifact; callee uses `EDI` entity pointer from stack |
| `CGaming_SpawnPracticeDummy` `extraout_DL` | `EDX = CountOccupiedPlayerSlots` then `+ 0x24` | Decompiler loses register; disasm @ R10 `0x0041f5fa` |
| Coop vs practice overlap | `0x23` is coop only; `0x24` is practice only | Predicates are **mutually exclusive** ranges (`-0x20` vs `-0x24` tests) |

## Frida

**none** — static disasm + caller decompile sufficient. Optional hook point documented in `CGaming_SpawnPlayerAtSlot` comment: rewrite ctor `nLives` arg before `+0x18C` store for tournament override experiments.

## Remaining UNK

- Exact semantic of coop scheduler slots **3/4/5** vs practice **3/4** (covered in task 02 `CBulanek_WeaponSchedulerCallback`).
- Whether `skinPaletteId = 4` for coop selects a specific weapon kind in `CWeapon_ctor` (see R10 task 09 — `a9` param naming).
- `CGaming_AppendDangerZoneNode` rect payload semantics for `apAiTrackHolders` (task 08 band).
