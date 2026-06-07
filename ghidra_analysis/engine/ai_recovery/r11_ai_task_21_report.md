# Round 11 — AI Task 21 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 21 |
| **title** | CGaming_OnResumeOrStartGame — wake slots 125..127 |
| **archetype** | campaign_ai |
| **seed_address** | `0x0041c140` |
| **addresses** | `0x0041c140`, `0x00417e40` |
| **acceptance** | Slot 127 gate; downward collision/trace wake loop for campaign slots |

## Status

**DONE** — Live Ghidra MCP (`connect_instance("bulanci")`, `bulanci.exe`): full disasm + decompile for `CGaming_OnResumeOrStartGame`, callee chain, and related slot helper `CGaming_IsPlayerSlotOccupied`. Slot **127** occupancy gate and downward **125..127** loop proven @ `0x0041c1c0`–`0x0041c1e8`. **Correction:** wake loop calls **`CGaming_RetestTraceAreasForEntity`** (`0x0041b5a0`), **not** `CGaming_TickPlayerCollisions` (`0x0041f0c0`) — prior `map_slots_spawner.md` pseudocode was wrong on the callee name.

## AI archetype

| Slots (dec) | Slots (hex) | Role | Wake path |
|-------------|-------------|------|-----------|
| **125..127** | `0x7d..0x7f` | Solo **campaign AI** enemies (`CBulanek`, `bIsAiVariant=1`) spawned by `CBulanek_SpawnPlayerAndCampaignSlots` when `bTotalSlots==1` | Gated on slot **127** non-null; loop retests danger/trace zones for **127 → 126 → 125** |
| **0..(bTotalSlots−1)** | `0x00..` | Human player slots | Separate pre-loop: same `RetestTraceAreasForEntity` per occupied slot |

Campaign bots in slots `0x7d..0x7f` need trace-area state refreshed on resume so `DefineTraceArea` / mine / step-on triggers registered during `OnInit` fire correctly before the next physics tick. The resume hook does **not** run full pickup/collision overlap (`TickPlayerCollisions`); it only re-evaluates **danger-zone trace rectangles** (`dangerZoneSlotVec@+0x2D8`, count `@+0x2E0`).

## Algorithm

### `CGaming_OnResumeOrStartGame` @ `0x0041c140`

**Signature (live Ghidra + `mapping.csv`):**

```c
uchar __thiscall CGaming_OnResumeOrStartGame(CGaming *this, char bRunning);
```

**Invocation:** `CDSView` vtable DATA xref @ `0x004827a0` (secondary render/show hook slot 15). Also fires script export **slot 10** (`OnGameStart`) via `CallExport(pLevelScript, 10, 0, NULL)` @ `0x0041c163`.

**Pseudocode (`bRunning != 0` — start or resume):**

```
CDSView_NoOpStub()                                    // vtable +0x3c stub @ 0x00438340

CallExport(pLevelScript, exportIndex=10, argc=0)    // OnGameStart lifecycle

if (pLevelBgmPlayer) CDSAudioPlayer_Play(1)

pGame = this->pOwnerGame                            // [this+0x84]
if (!pGame->bIsClient && pGame->bGameModeIndex == 1)
    CGaming_OnResumeSyncRoundTimer(pGame)

// --- human-slot trace retest ---
slot = pGame->bTotalSlots                           // [pGame+0xD8]
while (slot != 0) {
    slot--
    entity = CGaming_GetObjectAtSlotUnchecked(this, slot)
    CGaming_RetestTraceAreasForEntity(this, entity)
}

// --- campaign-slot wake (acceptance target) ---
if (CGaming_GetObjectAtSlotSafe(this, 0x7f) != NULL) {   // PUSH 0x7f @ 0x0041c1c0
    counter = 3
    do {
        slot = 0x80 - counter                         // 3→127, 2→126, 1→125
        entity = CGaming_GetObjectAtSlotUnchecked(this, slot)
        CGaming_RetestTraceAreasForEntity(this, entity)
        counter--
    } while (counter != 0)
}

// --- engine scheduler arms (host only) ---
if (!pGame->bIsClient && this->dwWeaponSpawnerMode > 2) {
    delayMs = (pGame->bTotalSlots == 1) ? 10000 : 5000
    Scheduler_RegisterEventSlot(&this->schedulerFacet, slotId=1, delayMs, flags=6)
}
if (!pGame->bIsClient) {
    if (CGame_FindResourceByName(pGame, levelName)->id == 0x10144)   // Exitus graveyard
        Scheduler_RegisterEventSlot(&this->schedulerFacet, slotId=2, 7000, flags=6)
}

return 1
```

**Pause branch (`bRunning == 0`):** if `pLevelBgmPlayer`, `CDSAudioPlayer_Stop(1)` @ `0x0041c26a`.

### Slot 127 gate (disasm proof)

| Address | Instruction | Meaning |
|---------|-------------|---------|
| `0x0041c1c0` | `PUSH 0x7f` | Probe slot **127** |
| `0x0041c1c4` | `CALL 0x00416810` | `CGaming_GetObjectAtSlotSafe` — returns `apEntitySlots[127]` or NULL |
| `0x0041c1c9` | `TEST EAX,EAX` | Skip wake loop if slot 127 empty |
| `0x0041c1cb` | `JZ 0x0041c1ea` | Gate taken |

### Downward wake loop (disasm proof)

| Address | Instruction | Meaning |
|---------|-------------|---------|
| `0x0041c1cd` | `MOV BL, 0x3` | Loop counter = 3 |
| `0x0041c1d0` | `MOV EDX, 0x80` | Base 128 |
| `0x0041c1d5` | `SUB DL, BL` | Slot = 128 − counter → **127, 126, 125** |
| `0x0041c1da` | `CALL 0x004168d0` | `CGaming_GetObjectAtSlotUnchecked` |
| `0x0041c1e0` | `CALL 0x0041b5a0` | **`CGaming_RetestTraceAreasForEntity`** |
| `0x0041c1e5` | `ADD BL, 0xff` | counter-- |
| `0x0041c1e8` | `JNZ 0x0041c1d0` | while counter != 0 |

### `CGaming_RetestTraceAreasForEntity` @ `0x0041b5a0` (callee semantics)

```
if (this->pOwnerGame == NULL) return
entity->vtable[+0x70](&worldRect)           // fetch entity bounds
slotKind = entity[+0x1c]                      // bSlotKind byte
for (i = nDangerZoneSlotCount; i > 0; i--)
    CGaming_CheckTraceAreasForEntity(
        this,
        dangerZoneSlotVec[i-1],
        slotKind,
        &worldRect,
        entity[+0x198])                       // bDrawAsMuted flag
```

Reverse-walks `dangerZoneSlotVec` so freshly armed trace areas from level `OnInit` / `OnGameStart` immediately evaluate enter/leave script exports (slots 7/8) for campaign entities.

### `CGaming_IsPlayerSlotOccupied` @ `0x00417e40` (related helper, not in wake path)

**Not called** from `OnResumeOrStartGame`. Used elsewhere (`OnSlotPlacementEvent`, `TryGetPlayerCoords`, `CBulanek_OnEvent`, …) to test whether a slot holds a **`CBulanek`** (`classId == 0x7ec`):

```
if (slot == 0xFF || slot >= 0x80) return false
view = this->apEntitySlots[slot]              // [ECX + slot*4 + 0xC8] @ 0x00417e4f
if (!view) return false
classId = view->GetRuntimeClass()->id         // CMP [EAX+8], 0x7ec @ 0x0041e62
return classId == 0x7ec
```

Wake gate uses **`GetObjectAtSlotSafe(127)`** (any non-null `CDSView*`), not `IsPlayerSlotOccupied`.

## Functions table

| Address | Symbol | Role | Evidence |
|---------|--------|------|----------|
| `0x0041c140` | `CGaming_OnResumeOrStartGame` | Resume/start hook: script `OnGameStart`, BGM, human + campaign trace retest, scheduler arms | Disasm `0x0041c140`–`0x0041c277`; vtable DATA `0x004827a0` |
| `0x00416810` | `CGaming_GetObjectAtSlotSafe` | Bounds-checked slot lookup (`slot < 0x80`) | Decompile; gate CALL @ `0x0041c1c4` |
| `0x004168d0` | `CGaming_GetObjectAtSlotUnchecked` | `apEntitySlots[slot]@+0xC8` | Decompile; loop CALLs @ `0x0041c1b1`, `0x0041c1da` |
| `0x0041b5a0` | `CGaming_RetestTraceAreasForEntity` | Reverse-walk danger zones for one entity | Decompile; wake CALL @ `0x0041c1e0` |
| `0x0041b3b0` | `CGaming_CheckTraceAreasForEntity` | Per-zone enter/leave bitmask + script dispatch | Callee of retest (R10 t12) |
| `0x00417e40` | `CGaming_IsPlayerSlotOccupied` | Slot holds `CBulanek` (class `0x7ec`) | Disasm `0x00417e40`–`0x00417e75`; **not** xref'd from seed |
| `0x0041f0c0` | `CGaming_TickPlayerCollisions` | Full collision + pickup overlap; **ends with** `RetestTraceAreasForEntity` | Decompile tail @ `0x0041f1c0` region — **not** used on resume wake |
| `0x004129c0` | `CGaming_OnResumeSyncRoundTimer` | Round-timer net sync when `bGameModeIndex==1` | CALL @ `0x0041c18e` |
| `0x00438c40` | `CDSScript::CallExport` | Script lifecycle dispatch | CALL @ `0x0041c163`, export **10** |
| `0x0042f210` | `Scheduler_RegisterEventSlot` | Arms engine timers slots 1/2 | CALLs @ `0x0041c21a`, `0x0041c25f` |

## Struct fields

| Object | Offset | Name | Used in seed |
|--------|--------|------|--------------|
| `CGaming` | `+0x84` | `pOwnerGame` | `bTotalSlots`, client flag, game mode |
| `CGaming` | `+0xC8` | `apEntitySlots[128]` | Slot 127 gate + wake loop |
| `CGaming` | `+0x2D8` | `dangerZoneSlotVec` | `RetestTraceAreasForEntity` walk |
| `CGaming` | `+0x2E0` | `nDangerZoneSlotCount` | Loop bound |
| `CGaming` | `+0x344` | `pLevelScript` | `CallExport` target |
| `CGaming` | `+0x354` | `pLevelBgmPlayer` | Play/stop BGM |
| `CGaming` | `+0x368` | `dwWeaponSpawnerMode` | Pickup timer arm gate (`> 2`) |
| `CGaming` | `+0x68` | `schedulerFacet` | `Scheduler_RegisterEventSlot` base |
| `CGame` | `+0xD8` | `bTotalSlots` | Human-slot loop bound |
| `CGame` | `+0x36` | `bIsClient` (chain field) | Host-only scheduler arms |
| `CGame` | `+0x19C` | `bGameModeIndex` | Round-timer sync gate |
| `CBulanek` | `+0x1C` | `bSlotKind` | Trace-area slot encoding |
| `CBulanek` | `+0x198` | `bDrawAsMuted` | Passed to `CheckTraceAreasForEntity` |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0041c1a3` | Human-slot retest loop |
| `set_decompiler_comment` | `0x0041c1c0` | Slot 127 gate comment |
| `set_decompiler_comment` | `0x0041c1d0` | Downward 127→125 wake loop |
| `set_function_prototype` | `0x00417e40` | `bool __thiscall CGaming_IsPlayerSlotOccupied(CGaming *this, uchar slot)` |
| `save_program` | `bulanci.exe` | saved |

## Decomp fixes

| Issue | Ground truth (disasm) | Notes |
|-------|----------------------|-------|
| Wake callee name in `map_slots_spawner.md` | `CALL 0x0041b5a0` = `RetestTraceAreasForEntity` | Doc said `TickPlayerCollisions`; update gameplay doc |
| `CGaming_RetestTraceAreasForEntity(this_00, …)` / `this_01` | `ECX = ESI` (`this`) throughout @ `0x0041c1af`/`0x0041c1d7` | Ghidra loses `this` on stack-call pattern |
| `CGaming_IsPlayerSlotOccupied` `this` type | `[ECX + slot*4 + 0xC8]` = `CGaming::apEntitySlots` | Prototype updated; decompiler still shows `CBulanek *` (Ghidra `__thiscall` ECX retype API limitation) |
| `GetObjectAtSlotSafe` vs `IsPlayerSlotOccupied` | Gate uses raw pointer test, not class-id filter | Campaign slot may hold `CBulanek`; gate fires if **any** view bound |

## Frida

**none** — static disasm sufficient. Optional: hook `0x0041c1c4` return to log slot-127 gate on resume; hook `0x0041b5a0` to count danger-zone retests per campaign slot.

## Remaining UNK

- Whether slot **127** is always the last campaign bot spawned or can hold non-`CBulanek` script views (gate is pointer-only).
- Full interior of `CGaming_CheckTraceAreasForEntity` enter/leave script dispatch (task 08 band).
- Client-side behavior when `bIsClient != 0` — scheduler arms skipped; trace retest still runs on all peers.

## Evidence paths consulted

- [ROUND11_AI_PROTOCOL.md](../ROUND11_AI_PROTOCOL.md), [agent_todos_30_r11_ai.json](../agent_todos_30_r11_ai.json)
- [map_slots_spawner.md](../../gameplay/map_slots_spawner.md), [CGaming.md](../struct_recovery/CGaming.md)
- [script_lifecycle.md](../script_lifecycle.md) — export slot 10 / engine timer distinction
- `config/bulanci/mapping.csv`, `bulanci.ghidra.exe.c`
