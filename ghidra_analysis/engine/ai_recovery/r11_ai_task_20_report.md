# Round 11 — AI Task 20 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 20 |
| **title** | `CGaming_TickPlayerCollisions` — campaign overlap AI |
| **archetype** | `campaign_ai` |
| **seed_address** | `0x0041f0c0` |
| **addresses** | `0x0041f0c0`, `0x0041a020` |
| **acceptance** | Full collision loop; pickup mask; slots 125..127 |

## Status

**DONE** — Full per-entity collision algorithm recovered from live Ghidra decompile + disasm (`0x0041f0c0`–`0x0041f1cc`). Campaign slots **125..127** (`0x7d..0x7f`) integration proven via spawn path + `CBulanek_Update` caller chain + `CMina_UpdateTraceAreas` slot sweep. Ghidra decompiler comments applied; `save_program bulanci.exe`.

## AI archetype

| Slots (dec / hex) | Role | Spawn | Collision entry |
|-------------------|------|-------|-----------------|
| **125..127** / `0x7d..0x7f` | Singleplayer **campaign AI** opponents | `CBulanek_SpawnPlayerAndCampaignSlots@0x004205a0` when `pOwnerGame->bTotalSlots == 1` | Each entity's `CBulanek_Update@0x0041fc80` → `CGaming_TickPlayerCollisions(pGaming, self)` on rect change |
| **101..107** / `0x65..0x6b` | World weapon/mine pickups (not AI) | `CGaming_RandomPickupSpawner_Tick` | Scanned **against** human-like collectors inside this function |
| **0..3** | Human players | Standard spawn loop | Same `CBulanek_Update` path; full pickup + solo ambient paths when gated |

**Clarification:** This function does **not** iterate slots 125..127 internally. Campaign AI **reaches** it because each `CBulanek` in those slots runs `CBulanek_Update` when its world rect changes. Separately, `CMina_UpdateTraceAreas@0x00419fd0` sweeps slots `0x7f`↓`0x7d` for mine/trap trace areas when `dwTraceAreasActive != 0` (called from `CBulanek_Update` when `apAiTrackHolders[0] != NULL`).

**Doc correction:** [map_slots_spawner.md](../../gameplay/map_slots_spawner.md) states `CGaming_OnResumeOrStartGame` calls `CGaming_TickPlayerCollisions` on slots 125..127 — live decompile @ `0x0041c140` shows **`CGaming_RetestTraceAreasForEntity`** instead (`0x0041c1a0` region). Task **21** owns that wake path.

## Algorithm

Pseudocode (proven constants from disasm):

```c
// void __thiscall CGaming_TickPlayerCollisions(CGaming* this, CBulanek* pEntity)
void CGaming_TickPlayerCollisions(CGaming* gaming, CBulanek* entity) {
    CGaming_InsertEntityByDepth(gaming, entity);          // 0x004184a0 @ 0x0041f0c9

    if ((entity->dwView_flags & 1) == 0)                  // TEST [EDI+0x44],1 @ 0x0041f0ce
        goto retest_trace;

    // --- Path A: map weapon/mine slots 101..107 ---
    if (CBulanek_IsHumanPlayer(entity)) {                 // 0x00416720 @ 0x0041f0da
        byte slot = 0x6c;                                   // MOV BL,0x6c @ 0x0041f0e5
        do {
            slot--;
            void* pickup = CGaming_GetObjectAtSlotSafe(gaming, slot);  // 0x00416810
            if (CBulanek_TestRectOverlapWithEntity(entity, pickup)) {  // 0x00419530
                CGame_ApplyPickup(entity, 0xff, slot);    // kind from pickup if 0xff @ 0x0041f117
            }
        } while (slot > 0x64);                            // slots 0x65..0x6b (101..107)
    }

    // --- Path B: solo ambient weapon views (NOT inventory) ---
    if (gaming->pOwnerGame->bTotalSlots == 1) {           // CMP [EAX+0xd8],1 @ 0x0041f129
        static const struct { CDSView** view; uint kind; } kSolo[] = {
            { &gaming->pAmbientAnim0,   0 },              // +0x324 → PUSH 0 @ 0x0041f154
            { &gaming->pAmbientSky,     3 },              // +0x330 → PUSH 3 @ 0x0041f177
            { &gaming->pAmbientAnim1,   1 },              // +0x328 → PUSH 1 @ 0x0041f19a
            { &gaming->pAmbientInsects, 2 },              // +0x32c → PUSH 2 @ 0x0041f1bd
        };
        for (each entry in kSolo) {
            if (CBulanek_TestRectOverlapWithEntity(entity, *entry.view))
                CGaming_OnPlayerCollectItem(gaming, entity, *entry.view, entry.kind);
        }
    }

retest_trace:
    CGaming_RetestTraceAreasForEntity(gaming, entity);    // 0x0041b5a0 @ 0x0041f1c5 — always
}
```

### `CBulanek_IsHumanPlayer` gate (Path A) — campaign slots

| `bPlayerSlot` (`+0x70`) | Branch | Effect on campaign AI |
|-------------------------|--------|------------------------|
| `< 4` | `CGame_IsLocalPlayerSlot(pGame, slot)` | Human local players only |
| `>= 4` (includes **125..127**) | `(pGame+0x36) == 0` | **All** non-human-slot entities share one solo/net flag; when zero (typical solo), campaign AI **can** collect map pickups via `CGame_ApplyPickup` |

Path B has **no** `IsHumanPlayer` gate — only `bTotalSlots == 1`. Campaign AI opponents in slots 125..127 can therefore trigger `OnPlayerCollectItem` on ambient weapon views in solo play.

### Overlap test (`CBulanek_TestRectOverlapWithEntity@0x00419530`)

- Rejects null target or target with `+0x4c == 0` (no drawable / inactive view).
- Compares `CDSRect_Overlaps(target+0x20, entity+0x20)` — world collision rects.

### Path A vs Path B side effects

| Path | Callee | Inventory | Score / SFX |
|------|--------|-----------|-------------|
| **A** (`0x65..0x6b`) | `CGame_ApplyPickup` | **Yes** — ammo @ `entity+0x11c+kind`, net `0x19` if human | Pickup sound via `CBulanek_PlayItemPickupSound` |
| **B** (solo `+0x324..+0x330`) | `CGaming_OnPlayerCollectItem` | **No** — unregisters view only | `CBulanek_NetSendTeamScoreOnCollect`, quip if `bPlayerSlot==0`, `SetAmbientAnimMode(0)` |

See [r10_deep_task_19_report.md](../deep_recovery/r10_deep_task_19_report.md) for `OnPlayerCollectItem` quip table.

### Caller graph (who passes `pEntity`)

```
CBulanek_Update @ 0x0041fc90
  if (pGamingHostScratch) CGaming_TickPlayerCollisions(scratch, this)
  // Campaign AI 0x7d..0x7f: pGamingHostScratch set at ctor; Update on movement

CGaming_TickRoundStateAndScoring @ 0x0041f37e
  CGaming_TickPlayerCollisions(this, param_1)   // e.g. after OnDeath scoring refresh

TriggerTeleportFX @ 0x0041fccb
  CGaming_TickPlayerCollisions(pGaming_host, (CBulanek*)teleportPoint)
  // Teleport overlap entity — special cast; only when fx_coord changes
```

### Campaign spawn (context)

```c
// CBulanek_SpawnPlayerAndCampaignSlots @ 0x004205a0
if (bTotalSlots == 1) {
    for (byte s = 0x7d; s < 0x80; s++)
        CGaming_SpawnPlayerAtSlot(gaming, s);   // slots 125, 126, 127
}
```

## Functions table

| Address | Symbol | Role | Evidence |
|---------|--------|------|----------|
| `0x0041f0c0` | `CGaming_TickPlayerCollisions` | **Seed.** Per-entity collision + pickup + trace retest | Decompile; disasm `0x0041f0c0`–`0x0041f1cc`; xrefs (3) |
| `0x0041a020` | `CGaming_OnPlayerCollectItem` | Solo ambient weapon collect (score/quip, no ammo) | 4 call sites `0x0041f154`..`0x0041f1bd` |
| `0x004184a0` | `CGaming_InsertEntityByDepth` | Depth-sort entity before collision | CALL @ `0x0041f0c9` |
| `0x00416720` | `CBulanek_IsHumanPlayer` | Path A gate | CALL @ `0x0041f0da` |
| `0x00416810` | `CGaming_GetObjectAtSlotSafe` | Bounds-checked slot lookup | CALL in loop @ `0x0041f0fe` |
| `0x00419530` | `CBulanek_TestRectOverlapWithEntity` | AABB overlap | CALL @ `0x0041f106`, `0x0041f13f`, etc. |
| `0x0041eba0` | `CGame_ApplyPickup` | Map pickup inventory + net | CALL @ `0x0041f117` |
| `0x0041b5a0` | `CGaming_RetestTraceAreasForEntity` | Mine/trap danger-zone sweep | CALL @ `0x0041f1c5` |
| `0x0041fc80` | `CBulanek_Update` | Primary caller for moving entities | CALL @ `0x0041fc90` region |
| `0x004205a0` | `CBulanek_SpawnPlayerAndCampaignSlots` | Spawns campaign AI into 125..127 | `local_4 = 0x7d` loop |
| `0x00419fd0` | `CMina_UpdateTraceAreas` | Separate slot sweep `0x7f`↓`0x7d` for trace areas | `while (slot > 0x7d)` |

## Struct fields

| Struct | Offset | Name | Use in this function |
|--------|--------|------|----------------------|
| `CBulanek` | `+0x44` | `dwView_flags` | Bit 0 must be set or entire pickup block skipped |
| `CBulanek` | `+0x70` | `bPlayerSlot` | `125..127` for campaign AI; `OnPlayerCollectItem` quip gate uses `== 0` only |
| `CBulanek` | `+0x84` | `pGamingHostScratch` | `CBulanek_Update` passes this as `CGaming*` host |
| `CBulanek` | `+0xF4` | `pGame` | `IsHumanPlayer` / `ApplyPickup` net paths |
| `CGaming` | `+0x84` | `pOwnerGame` | `bTotalSlots` gate @ `+0xd8` |
| `CGaming` | `+0x324` | `pAmbientAnim0` | Solo overlap target, `weaponKind=0` |
| `CGaming` | `+0x328` | `pAmbientAnim1` | Solo overlap target, `weaponKind=1` |
| `CGaming` | `+0x32C` | `pAmbientInsects` | Solo overlap target, `weaponKind=2` |
| `CGaming` | `+0x330` | `pAmbientSky` | Solo overlap target, `weaponKind=3` |
| `CGame` | `+0xD8` | `bTotalSlots` | `== 1` enables Path B (solo campaign) |
| `CGame` | `+0x36` | *(byte)* | `IsHumanPlayer` for `bPlayerSlot >= 4` |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0041f0c0` | R11 task20 overview (callers + three paths) |
| `set_decompiler_comment` | `0x0041f0e5` | Slot loop `0x65..0x6b` / `CGame_ApplyPickup` |
| `set_decompiler_comment` | `0x0041f129` | Solo `bTotalSlots==1` ambient weapon gate |
| `save_program` | `bulanci.exe` | Saved |

**Skipped (R10):** `set_function_prototype` on `0x0041f0c0` / `0x0041a020` already applied; symbol names current.

## Decomp fixes

| Issue | Stale | Live | Proof |
|-------|-------|------|-------|
| `this` type on seed | `void *this` | Still `void *` in decompiler | Field offsets `+0x324` etc. match `CGaming`; MCP `__thiscall` limitation |
| `OnPlayerCollectItem` arg order | `(pPickup, pCollector)` in R6 | `(pCollector, pPickup)` | R10 call-site pushes; unchanged |
| `map_slots_spawner` resume hook | `TickPlayerCollisions` on 125..127 | `RetestTraceAreasForEntity` @ `0x0041c140` | Decompile `OnResumeOrStartGame` |

## Frida

**Not required** — loop bounds, gate bytes, and call-site constants proven statically.

Optional: hook `0x0041f0c0` with `scripts/frida/` logging `bPlayerSlot`, overlap slot, and path (A vs B) when testing campaign AI pickup behavior on a solo map.

## Remaining UNK

| Item | Why |
|------|-----|
| `CGame+0x36` field name | Used for `IsHumanPlayer` when `bPlayerSlot >= 4`; likely solo-host / non-networked flag — not typed in `CGame.md` |
| Whether campaign AI **should** collect ambient weapon views | Engine allows it (no slot filter on Path B); design intent unproven |
| `TriggerTeleportFX` casting `CTeleportPoint*` → `CBulanek*` | Works via embedded layout / overlap entity; full struct proof deferred to `CTeleportPoint.md` |

## Evidence paths

- [round6_logic_task_10_report.md](../logic_recovery/round6_logic_task_10_report.md), [r10_deep_task_19_report.md](../deep_recovery/r10_deep_task_19_report.md)
- [CGaming.md](../struct_recovery/CGaming.md), [CBulanek.md](../struct_recovery/CBulanek.md)
- [map_slots_spawner.md](../../gameplay/map_slots_spawner.md), [powerups.md](../../gameplay/powerups.md)
