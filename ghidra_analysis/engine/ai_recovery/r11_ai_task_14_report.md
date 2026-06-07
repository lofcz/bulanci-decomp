# Round 11 — Task 14 report

## Task

| Field | Value |
|-------|-------|
| **id** | 14 |
| **worker** | 14 / 30 |
| **title** | Campaign AI enemies (slots `0x7d..0x7f`) |
| **archetype** | `campaign_ai` |
| **seed_address** | `0x004205a0` |
| **addresses** | `0x004205a0`, `0x00420610` |
| **acceptance** | Solo-only spawn loop; AI behavior after spawn; collision tick inclusion |

## Status

**DONE** — spawn gate, slot loop, ctor branch, post-spawn scheduler/fire paths, and collision/trace inclusion verified via Ghidra decompile + disasm (`bulanci.exe`). No Ghidra mutations required (symbols and plate comments already present).

## AI archetype

**Campaign AI** — three `CBulanek` opponents at entity slots **125..127** (`0x7d..0x7f`). Spawned only when **`CGame.bTotalSlots == 1`** (solo human). Same class (`0x7ec`) and shared action motor as humans/co-op, but **distinct ctor scheduler profile** and **distinct post-fire AI branch** in `CWeapon_Fire` (40% pain tick vs co-op `TryBotRandomAction`).

## Algorithm

### Call chain (spawn)

```
Level script opcode 50 (InsertBulanci)
  → CLevelScript::CLevelScriptOpExt_InsertBulanci @ 0x00420610
       ECX = *(CGaming **)(scriptCtx + 0x458)
  → _Globals::CBulanek_SpawnPlayerAndCampaignSlots(CGaming *this) @ 0x004205a0
```

### `CBulanek_SpawnPlayerAndCampaignSlots` @ `0x004205a0`

**Prototype (disasm):** `void __fastcall` with `ECX = CGaming *` (`MOV ESI,ECX` @ `0x004205a7`).

```
SpawnPlayerAndCampaignSlots(cgaming):
  CGaming_SetEntityRegisterMode(cgaming, 1)     // field_0x341 clone/register mode @ 0x004168c0
  total = cgaming->pOwnerGame->bTotalSlots      // byte @ CGame+0xd8 via [ESI+0x84] @ 0x004205b4

  slot = 0
  while slot < total:
    CGaming_SpawnPlayerAtSlot(cgaming, slot)    // 0x00420530
    slot++

  if total == 1:                                // CMP AL,1 @ 0x004205e5; JNZ skip
    slot = 0x7d
    while slot < 0x80:                          // CMP BL,0x80 @ 0x004205ff; JC loop
      CGaming_SpawnPlayerAtSlot(cgaming, slot)
      slot++
```

| Constant | Value | Evidence |
|----------|-------|----------|
| Human slot range | `0 .. bTotalSlots-1` | Loop @ `0x004205c8`–`0x004205df` |
| Solo gate | `bTotalSlots == 1` | `CMP AL,1` @ `0x004205e5` |
| Campaign slot range | `0x7d, 0x7e, 0x7f` | `MOV BL,0x7d` @ `0x004205e9`; exit `CMP BL,0x80` |
| Clone flag | `1` | `PUSH 1` @ `0x004205a5` before `SetEntityRegisterMode` |

### Per-slot spawn wrapper

`CGaming_SpawnPlayerAtSlot` @ `0x00420530` → `CGaming_SpawnAndInitializePlayer(this, slotKind, 0xFFFFFFFF, nLives=1, nSpeed=100, skinPaletteId=0)`:

1. `OperatorNewWithBadAlloc(0x19c)` + `CBulanekCtor`
2. `CGaming_AddEntity(player, -1)`
3. `CGaming_RegisterObjectAtSlot(cgaming, player, slotKind)` → `bPlayerSlot` @ `+0x70` = **125..127** for campaign
4. `CGaming_RespawnPlayerAtSafeLocation`
5. `_rand` → `CBulanek_SetFacingTrack(facing 0..3, sendNet=0)`

Deathmatch lives: **`nLives = 1`** hardcoded (not tournament `InsertOpponent` path).

### `CBulanekCtor` branch for `slotKind ∈ {0x7d, 0x7e, 0x7f}`

Campaign slots satisfy **`slotKind > 3`** and are **outside** co-op (`0x20..0x23`) and practice (`0x24..0x27`) bands:

| Field / action | Campaign AI value | Evidence |
|----------------|-------------------|----------|
| `bIsAiVariant` | `1` | `3 < slotKind` @ `0x0041e5e1` region |
| `bSlotKind` | `slotKind & 3` → **1, 2, 3** | Mask @ ctor; `(slotKind&3)==0` → force `3` (not hit for 125..127) |
| `bPlayerSlot` | **125, 126, 127** | `RegisterObjectAtSlot` writes actual slot to `+0x70` |
| Danger zones | 4× `CGaming_AppendDangerZoneNode` | `if (3 < slotKind)` loop |
| `bAmmoKind1` | `GetMaxAmmoForKind(1)` | `if (4 < slotKind)` |
| `bTeamColor` | `_rand` → `0..7` | AI color branch (`slotKind >= 4`) |
| `nLives` / `nSpeed` | `1` / `100` | `SpawnPlayerAtSlot` hardcode |
| Scheduler slot **0** | delay **150 ms** (`0x96`) when `bTotalSlots==1` | `(-(bTotalSlots!=1) & 0x15e) + 0x96` |
| Scheduler slot **2** | delay **0**, `eventKind=7` | Else branch at ctor end (`uVar9=2`) — **not** slots 3/4/5 (co-op/dummy) |

Co-op (`0x20..0x23`) and practice (`0x24..0x27`) register additional scheduler slots 3/4/5; **campaign AI does not**.

### Post-spawn AI behavior

#### Visibility hook (`CBulanek_OnShow` @ `0x00417380`)

When shown (`param_1 != 0`):

1. `CBulanek_ArmFireDelayScheduler` @ `0x00417260` — **matches campaign gate** (`bPlayerSlot > 3`, not in `0x20..0x27`): arms scheduler **slot 2** with **`((_rand()*11>>15)+5)*1000` ms** → **5000..15000 ms**.
2. `CBulanek_ArmTournamentSchedulerDelays` @ `0x004172d0` — **does not run** for campaign (gate requires `0x20..0x27` only).

#### Scheduler dispatch (`eventKind 7` → `CBulanek_WeaponSchedulerCallback` @ `0x00420b30`)

| Slot | Campaign behavior |
|------|-------------------|
| **0** | Reload/HUD tick (shared) |
| **2** | If weapon scheduler slot 0 armed → `CBulanek_OnTakeDamage(-1,-1,0)` |
| **3** | `TryBotRandomAction` + re-arm tournament delays — **not registered at ctor** for campaign |

#### Fire animation end (`CWeapon_Fire` @ `0x00421520`, `param_2 == 0xFFFF`)

For `bPlayerSlot > 3`:

```
if bPlayerSlot in 0x20..0x27:          // co-op OR practice
    TryBotRandomAction(holder)
else:                                   // campaign 0x7d..0x7f and any other high slot
    if RandInRange(0,99) < 0x28:       // 40% (disasm threshold 0x28)
        OnTakeDamage(holder, -1, -1, 0)
```

**Campaign AI does not take the co-op `TryBotRandomAction` path after every shot**; it uses a **40% pain/knockdown tick** instead.

#### Script event `0x102`

`CBulanek_OnEvent` case `0x102` @ `0x00421160` → **`CBulanek_TryBotRandomAction(this)`** unconditionally (level-script driven bot pulse).

#### `TryBotRandomAction` gate (`IsHumanPlayer` @ `0x00416720`)

Disasm @ `0x00420a93`–`0x00420a9a`: `CALL IsHumanPlayer`; **`JZ` exit if false**. For slots `>= 4`, returns **`(CGame+0x36 == 0)`** — in solo campaign (`bTotalSlots==1`) this is typically **true**, enabling autonomous `ApplyAction` / `_rand` action `0..3` when invoked (events `0x102`, not the default post-fire path).

#### Death respawn arm

`CBulanek_OnDeath` @ `0x0041fb80`: for slots matching campaign band (same gate as `ArmFireDelayScheduler`), **re-arms scheduler slot 2** if not already armed.

### Collision / trace inclusion

| Mechanism | Campaign slots `0x7d..0x7f` | Evidence |
|-----------|------------------------------|----------|
| **Mine trace sweep** | Explicit downward loop `0x80..0x7d` | `CMina_UpdateTraceAreas` @ `0x00419fd0` — `do { slot--; CheckTraceAreasForSlot } while (slot > 0x7d)` |
| **Resume / start game** | If slot `0x7f` occupied, retest slots `0x7d..0x7f` | `CGaming_OnResumeOrStartGame` @ `0x0041c140` — `GetObjectAtSlotSafe(0x7f)` gate; loop `128 - c` for `c = 3..0` → slots 125..127 |
| **OnDeath scoring tick** | Per-death `TickPlayerCollisions` on dying entity | `CBulanek_OnDeath` → `CGaming_TickRoundStateAndScoring` → `CGaming_TickPlayerCollisions` @ `0x0041f350` |
| **Pickup overlap** | Human-local player scans `0x6c..` pickups; campaign entities get depth insert + `RetestTraceAreasForEntity` | `CGaming_TickPlayerCollisions` @ `0x0041f0c0` |

**Decomp fix:** [`map_slots_spawner.md`](../../gameplay/map_slots_spawner.md) §4 claims `OnResumeOrStartGame` calls `CGaming_TickPlayerCollisions` on slots 125..127; live decompile @ `0x0041c140` calls **`CGaming_RetestTraceAreasForEntity`** (task **21**). Mine trace loop and death-path collision tick remain proven for campaign slots.

## Functions table

| Address | Symbol | Role |
|---------|--------|------|
| `0x00420610` | `CLevelScriptOpExt_InsertBulanci` | Script opcode 50 entry; loads `CGaming*` @ `ctx+0x458`, calls spawn |
| `0x004205a0` | `CBulanek_SpawnPlayerAndCampaignSlots` | Human loop + solo campaign `0x7d..0x7f` loop |
| `0x004168c0` | `CGaming_SetEntityRegisterMode` | `field_0x341 = 1` clone mode before spawn |
| `0x00420530` | `CGaming_SpawnPlayerAtSlot` | `nLives=1`, `nSpeed=100` wrapper |
| `0x0041f500` | `CGaming_SpawnAndInitializePlayer` | Alloc/ctor/register/respawn/facing |
| `0x0041e4b0` | `CBulanekCtor` | AI variant + scheduler slot 0/2 profile for campaign |
| `0x00417380` | `CBulanek_OnShow` | Arms fire-delay scheduler on visibility |
| `0x00417260` | `CBulanek_ArmFireDelayScheduler` | Slot 2, 5–15 s for campaign band |
| `0x00420b30` | `CBulanek_WeaponSchedulerCallback` | Scheduler slot dispatch (`eventKind 7`) |
| `0x00421520` | `CWeapon_Fire` | Post-fire: 40% `OnTakeDamage` for campaign (not `TryBot`) |
| `0x00420a90` | `CBulanek_TryBotRandomAction` | Script `0x102`; co-op post-fire path |
| `0x00420d40` | `CBulanek_OnEvent` | Event `0x102` → `TryBotRandomAction` |
| `0x0041f0c0` | `CGaming_TickPlayerCollisions` | Collision/pickup tick (death caller chain) |
| `0x0041c140` | `CGaming_OnResumeOrStartGame` | Slot `0x7f` gate → retest `0x7d..0x7f` |
| `0x00419fd0` | `CMina_UpdateTraceAreas` | Trace loop includes `0x7d..0x7f` |
| `0x00416720` | `CBulanek_IsHumanPlayer` | Autonomous AI gate (`CGame+0x36==0` for slot ≥ 4) |

## Struct fields

| Offset | Name | Campaign AI use |
|--------|------|-----------------|
| `CGame+0xd8` | `bTotalSlots` | Solo gate (`== 1` spawns `0x7d..0x7f`) |
| `CGaming+0x341` | `field_0x341` | Set to `1` during spawn (`SetEntityRegisterMode`) |
| `CBulanek+0x70` | `bPlayerSlot` | **125..127** (registry slot, not masked `bSlotKind`) |
| `CBulanek+0x124` | `bSlotKind` | **1, 2, 3** (`slotKind & 3`) |
| `CBulanek+0x199` | `bIsAiVariant` | `1` |
| `CBulanek+0x88` | `scheduler` | Slots 0 (150 ms) + 2 (fire-delay / damage tick) |
| `CBulanek+0x178` | `apAiTrackHolders[4]` | Danger-zone nodes from ctor |
| `CLevelScriptCtx+0x458` | `pGaming` | `InsertBulanci` → spawn target |

## Ghidra deltas

**None** — `CBulanek_SpawnPlayerAndCampaignSlots` already named; plate comment documents solo `0x7d..0x7f` loop; `CGaming_SpawnPlayerAtSlot` / ctor comments document caller chain. No `save_program`.

## Decomp fixes

| Issue | Resolution |
|-------|------------|
| `mapping.csv` lists `CBulanek*` param on `0x004205a0` | Disasm: `ECX` is `CGaming*`; decompile `CGaming *param_1` is correct |
| `map_slots_spawner.md` resume hook calls `TickPlayerCollisions` | Live decompile: `CGaming_RetestTraceAreasForEntity` — defer full collision-loop doc to task **20** |
| `TryBotRandomAction` decompiler `if (IsHumanPlayer)` | Disasm `JZ` after `TEST AL` — non-zero **enters** bot logic (symbol = “local autonomous entity”, task **7**) |

## Frida

**Not required** — spawn loop and branch constants closed statically.

Optional validation: hook `0x004205a0` and log `bTotalSlots` + second-loop slot indices; hook `CWeapon_Fire` @ `0x00421520` tail to count 40% `OnTakeDamage` vs co-op `TryBot` path by `bPlayerSlot`.

## Remaining UNK

| Item | Why |
|------|-----|
| Exact level-script use of event **`0x102`** on campaign slots | Xref into `OnEvent` proven; per-map script survey out of scope |
| Semantic of scheduler **slot 2 → OnTakeDamage** vs fire-delay re-arm cycle | Behavior proven; anim-band interaction deferred to tasks **3**, **8** |
| `CGame+0x36` field name / mode matrix for `IsHumanPlayer` | Gate proven; full taxonomy task **7** |

## Evidence paths consulted

- [`ROUND11_AI_PROTOCOL.md`](../ROUND11_AI_PROTOCOL.md), [`agent_todos_30_r11_ai.json`](../agent_todos_30_r11_ai.json)
- [`map_slots_spawner.md`](../../gameplay/map_slots_spawner.md), [`CBulanek.md`](../struct_recovery/CBulanek.md)
- [`round6_logic_task_10_report.md`](../logic_recovery/round6_logic_task_10_report.md)
- Ghidra MCP: `decompile_function`, `disassemble_function`, `get_xrefs_to` on listed addresses
