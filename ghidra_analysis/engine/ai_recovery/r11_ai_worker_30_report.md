# Round 11 — Worker 30/30 report (AI coordinator)

## Task

| Field | Value |
|-------|-------|
| **id** | 30 |
| **round** | 11 |
| **kind** | coordinator |
| **role** | manifest audit — **no Ghidra mutations** |
| **title** | Coordinator: R11 AI manifest audit |
| **manifest** | [agent_todos_30_r11_ai.json](../agent_todos_30_r11_ai.json) |
| **protocol** | [ROUND11_AI_PROTOCOL.md](../ROUND11_AI_PROTOCOL.md) |
| **slot taxonomy** | [map_slots_spawner.md](../../gameplay/map_slots_spawner.md) |

## Status

**DONE** — coordinator report published; **no** Ghidra MCP calls and **no** `save_program bulanci.exe` (per worker-30 scope). Workers **1–29** were launched in parallel; **zero** `r11_ai_task_*_report.md` files and **zero** worker jsonl lines existed on disk at coordinator publish time.

## Deliverables

| Artifact | Path |
|----------|------|
| Protocol | [ROUND11_AI_PROTOCOL.md](../ROUND11_AI_PROTOCOL.md) |
| Manifest (workers 1–29 + coordinator) | [agent_todos_30_r11_ai.json](../agent_todos_30_r11_ai.json) |
| Spawn template | [scripts/spawn_ai_agents_r11.md](../../scripts/spawn_ai_agents_r11.md) |
| Task reports (workers 1–29) | `ai_recovery/r11_ai_task_{NN}_report.md` |
| Coordinator report | this file |
| Aggregate results (append-only) | [agent_todos_30_r11_ai_results.jsonl](../agent_todos_30_r11_ai_results.jsonl) |

## Scan summary (tasks 1–29 + jsonl)

### On-disk at publish

| Check | Result |
|-------|--------|
| `ai_recovery/r11_ai_task_01..29_report.md` | **0 / 29** present |
| `agent_todos_30_r11_ai_results.jsonl` worker lines | **0 / 29** present |
| Coordinator report | this file |
| Coordinator jsonl line | appended (id **30**) |

### Prior-round evidence (pre-R11 baseline)

Workers should treat these as **starting evidence**, not R11 acceptance. Each R11 report must still prove algorithms with live xrefs / disasm per protocol.

| Prior artifact | Relevant R11 tasks |
|----------------|-------------------|
| [round6_logic_task_01_report.md](../logic_recovery/round6_logic_task_01_report.md) | 1 — `CBulanek_SchedulerTick` enqueue **DONE** |
| [round6_logic_task_02_report.md](../logic_recovery/round6_logic_task_02_report.md), [r10_deep_task_01_report.md](../deep_recovery/r10_deep_task_01_report.md) | 9 — movement step **PARTIAL→DONE** (R10 rename/prototype) |
| [round5_worker_02_report.md](../struct_recovery/round5_worker_02_report.md) | 2, 6 — tournament delays, HUD refresh |
| [round5_worker_03_report.md](../struct_recovery/round5_worker_03_report.md) | 3, 5, 22 — `_rand`, `TryBot`, fire delay, `AllocAiTrackHolder` |
| [round6_logic_task_10_report.md](../logic_recovery/round6_logic_task_10_report.md) | 20 — `TickPlayerCollisions` / pickup caller context **PARTIAL** |
| [player_controls.md](../player_controls.md) | 4, 8 — `ApplyAction`, `CanDispatchPlayerAction` gate |
| [CBulanek.md](../struct_recovery/CBulanek.md) | 7, 11, 12 — layout, `bIsAiVariant`, facing fields |
| [map_slots_spawner.md](../../gameplay/map_slots_spawner.md) | 7, 12–15, 20–21, 29 — slot bands |
| [script_dispatch_table.md](../script_dispatch_table.md) | 13, 16–18, 26–29 — opcode handlers |
| [combat_projectiles.md](../../gameplay/combat_projectiles.md), [CWeapon.md](../struct_recovery/CWeapon.md) | 23–25 |
| [damage_pipeline.md](../../gameplay/damage_pipeline.md) | 5, 10, 15 |
| [r10_deep_task_12_report.md](../deep_recovery/r10_deep_task_12_report.md) | 26 — `dangerZoneSlotVec@+0x2d8` **PARTIAL** |

---

## Archetype coverage matrix

Legend: **R11** = dedicated R11 report on disk; **Prior** = pre-R11 evidence strength; **Gap** = what R11 acceptance still requires.

### By archetype (aggregate)

| Archetype | Slots / role | R11 tasks | R11 reports | Prior baseline | Archetype readiness |
|-----------|--------------|-----------|-------------|----------------|---------------------|
| **core_scheduler** | All `CBulanek` — slots 0..5 walk | 1, 2 | 0/2 | Task 1 **DONE** (R6); task 2 switch **unnamed pseudocode** | **PARTIAL** — callback switch is R11-critical |
| **core_input** | Human + AI action dispatch | 4, 8 | 0/2 | `player_controls.md` action table; gate @ `0x4174a0` **unnamed** | **PARTIAL** |
| **taxonomy** | `bIsAiVariant`, `bSlotKind`, `IsHumanPlayer` | 7 | 0/1 | `CBulanek.md` ctor rule; caller list **incomplete** | **PARTIAL** |
| **moving_bot** | Campaign + coop movers | 3, 9, 10, 22 | 0/4 | Movement step **DONE** (R10); `_rand`/TryBot **partial**; collision **partial** | **PARTIAL** |
| **shooting_ai** | Fire delays, weapon dispatch, net | 5, 6, 23, 24, 25 | 0/5 | Formulas cited (R5); `CWeapon::Fire` bot branches **open** | **LOW** |
| **rotation** | Facing tracks + script anim dir | 11, 28 | 0/2 | `SetFacingTrack` named; script `SetAnimDirection` **handler only** | **PARTIAL** |
| **spawn** | Ctor + deep helper `0x41f230` | 12, 19 | 0/2 | Slot bands documented; per-`slotKind` ctor branches **open** | **LOW** |
| **vampire_coop** | `0x20..0x23` InsertVampires | 13 | 0/1 | Opcode + spawn symbol in `mapping.csv`; hide-slots / scoring **open** | **LOW** |
| **stationary_target** | `0x24..0x27` practice dummies | 15 | 0/1 | Spawn entry named; TryBot exclusion **unproven** | **LOW** |
| **campaign_ai** | `0x7d..0x7f` solo enemies | 14, 20, 21 | 0/3 | Slot map + collision loop **partial** (R6-10) | **PARTIAL** |
| **script_spawn** | InsertOpponent / Ex / EnemyAt | 16, 17, 18 | 0/3 | Opcodes in dispatch table; arg→spawn mapping **open** | **LOW** |
| **teleport** | `TranslateTo` on `CBulanek` | 27 | 0/1 | Handler address known | **LOW** |
| **hazard** | `DefineDangerZone` + `CMina` | 26 | 0/1 | `CGaming+0x2d8` vector **PARTIAL** (R10) | **PARTIAL** |
| **script_mover** | Gaps `4..31`, `40..99`, `108..124` | 29 | 0/1 | `BindToSlot`, `OnBitmapEvt` **partial** in slot doc | **LOW** |

### Per-task matrix (ids 1–29)

| id | Archetype | Seed | R11 report | jsonl | Prior | R11 acceptance gap |
|----|-----------|------|------------|-------|-------|-------------------|
| 1 | core_scheduler | `0x41aed0` | — | — | **DONE** R6 | Re-verify vtable slot 30 caller tree for reimplementation doc |
| 2 | core_scheduler | `0x420b30` | — | — | PARTIAL R5 | Full `switch(slotIndex)` pseudocode: reload die, fire, damage, knockdown+TryBot, pickup |
| 3 | moving_bot | `0x420a90` | — | — | PARTIAL R5 | Prove `_rand` scaling; `IsHumanPlayer` gate; fire-delay bit checks; `CanDispatch` before `Apply` |
| 4 | core_input | `0x420910` | — | — | PARTIAL | Per-action human vs AI paths + net send sites |
| 5 | shooting_ai | `0x417260` | — | — | PARTIAL R5 | Exact `_rand → ms` for slot 2; all callers |
| 6 | shooting_ai | `0x4172d0` | — | — | PARTIAL R5 | `slotKind 0x20..0x27` branch; link to callback case 3 |
| 7 | taxonomy | `0x416720` | — | — | PARTIAL | Every `IsHumanPlayer` caller in AI path |
| 8 | core_input | `0x4174a0` | — | — | PARTIAL | All rejection conditions per action index + field offsets |
| 9 | moving_bot | `0x41af70` | — | — | **DONE** R10 | Confirm vtable `+0x20`/`+0x24` commit in AI-only call paths |
| 10 | moving_bot | `0x4195f0` | — | — | PARTIAL | `SpatialQuery` obstacle bitmask for campaign slots |
| 11 | rotation | `0x4197b0` | — | — | PARTIAL | Track index ↔ facing byte; AI pickup case 4 |
| 12 | spawn | `0x41f500` | — | — | PARTIAL | All ctor branches: `0..3`, `0x20..0x27`, `0x7d..0x7f` |
| 13 | vampire_coop | `0x420630` | — | — | LOW | Vampire == coop slots; hide 1..3; `TickRoundStateAndScoring` side effects |
| 14 | campaign_ai | `0x4205a0` | — | — | PARTIAL | Solo-only spawn loop; post-spawn AI behavior |
| 15 | stationary_target | `0x41f5d0` | — | — | LOW | Stationary proof (no TryBot?); shoot-only; cloneFlag |
| 16 | script_spawn | `0x41f730` | — | — | LOW | 4-arg bytecode → `SpawnAndInitializePlayer`; slot `0x24..` alloc |
| 17 | script_spawn | `0x41f6a0` | — | — | LOW | CollInsert pool; parentSlot; kind/team/flags |
| 18 | script_spawn | `0x41f610` | — | — | LOW | Diff vs OpponentEx; position/state args |
| 19 | spawn | `0x41f230` | — | — | LOW | All callers; slot assignment; initialTrack/facing |
| 20 | campaign_ai | `0x41f0c0` | — | — | PARTIAL R6 | Full collision loop; pickup mask; slots 125..127 |
| 21 | campaign_ai | `0x41c140` | — | — | PARTIAL | Slot 127 gate; downward collision tick |
| 22 | moving_bot | `0x41b420` | — | — | **DONE** R5 rename | Array size; consumers in movement/weapon paths |
| 23 | shooting_ai | `0x4212b0` | — | — | PARTIAL | AI mine self-hit; `u16==0xFFFF` ammo-empty bot damage |
| 24 | shooting_ai | `0x420721` | — | — | LOW | Switch `bWeaponKind 0..5`; net `0x0E` |
| 25 | shooting_ai | `0x4208c0` | — | — | LOW | Scheduler slot 1; broadcast; human vs AI |
| 26 | hazard | `0x41bb30` | — | — | PARTIAL R10 | Opcode → `CGaming+0x2d8`; `CMina_RegisterDangerZone`; trace areas |
| 27 | teleport | `0x4188e0` | — | — | LOW | Bytecode args; `SetPosition`; level script usage |
| 28 | rotation | `0x418980` | — | — | LOW | Script anim direction → track manager link |
| 29 | script_mover | `0x418950` | — | — | PARTIAL | `SpawnAtView`, `BindToSlot`, `OnBitmapEvt` `0xD7`; `IsViewKind` |

### Slot band ↔ archetype map (verified taxonomy)

| Slot range (hex) | Decimal | Archetype | Primary spawn / control |
|------------------|---------|-----------|-------------------------|
| `0x00..0x03` | 0..3 | Human players | `CBulanek_SpawnPlayerAndCampaignSlots` |
| `0x20..0x23` | 32..35 | **vampire_coop** | `CGaming_SpawnCoopPartnerSlots` / `InsertVampires` |
| `0x24..0x27` | 36..39 | **stationary_target** | `CGaming_SpawnPracticeDummy` |
| `0x7d..0x7f` | 125..127 | **campaign_ai** | `CBulanek_SpawnPlayerAndCampaignSlots` (`bTotalSlots==1`) |
| `0x04..0x1f`, `0x28..0x63`, `0x6c..0x7c` | gaps | **script_mover** / hazards | `BindToSlot`, `SpawnAtView`, `TranslateTo`, `SetAnimDirection` |

---

## Round 12 gap list (post-R11)

Prioritized follow-ups once workers 1–29 land reports. These are **out of R11 manifest scope** or require cross-archetype integration.

### P0 — blocks 1:1 open_bulanci AI

| Gap | Why R12 | Depends on R11 tasks |
|-----|---------|----------------------|
| **Unified AI tick pseudocode** | Merge scheduler callback + TryBot + movement into one per-frame narrative per archetype | 1–3, 9, 22 |
| **Archetype dispatch table in code** | `open_bulanci` needs `slotKind` → behavior profile (coop / campaign / dummy / script) | 7, 12–15 |
| **WeaponSchedulerCallback full switch doc → Rust/C++** | Single largest unnamed control-flow blob for bots | 2, 5, 6 |
| **Network parity for AI fire** | Host vs client AI weapon sync (`0x0E`, `TriggerPrimaryActionAndBroadcast`) | 24, 25 |
| **Frida validation harness** | Static-only branches (dummy TryBot exclusion, tournament delays) | 3, 6, 15 |

### P1 — script / level integration

| Gap | Why R12 |
|-----|---------|
| **Per-map script corpus audit** | Correlate `res_*_Script.lua` spawn opcodes with recovered handlers (train map movers, etc.) | 16–18, 27–29 |
| **Script gap entity tick model** | Non-`CBulanek` views in gaps `4..124` — no R11 task owns full anim/event loop | 29 |
| **`OnBitmapEvt` (`0xD7`) behavior catalog** | Event codes per level for hazard triggers | 29 |
| **Danger zone ↔ campaign AI overlap** | `TraceArea` / `RetestTraceAreas` with slots 125..127 | 20, 26 |

### P2 — polish / struct closure

| Gap | Why R12 |
|-----|---------|
| **`CBulanek` Ghidra struct completion** | Undersized decompiler type; tail `pad` bands | 7, 12 |
| **`CanDispatchPlayerAction` rename + prototype** | Still `FUN_004174a0` in `player_controls.md` | 8 |
| **`CGaming_TickPlayerCollisions` interior** | R6 MCP loss left caller body open | 20 |
| **Coop vampire scoring edge cases** | Hide slots 1..3 + round state side effects | 13 |
| **Practice dummy damage-only path** | Confirm no scheduler TryBot arm in ctor | 15 |

### Suggested R12 manifest shape (draft)

| Workers | Theme |
|---------|-------|
| 1–5 | Integration: end-to-end AI tick pseudocode + archetype enum |
| 6–10 | Frida scripts + live validation (coop, campaign, dummy) |
| 11–15 | Level script opcode corpus (top 10 maps by mover count) |
| 16–20 | Net sync + multiplayer AI parity |
| 21–25 | Script-gap entity anim/event loop |
| 26–30 | Coordinator + open_bulanci port checklist |

---

## Monitoring `agent_todos_30_r11_ai_results.jsonl`

Append-only: one JSON object per finished worker (coordinator included).

### Quick status (PowerShell, repo root)

```powershell
$j = "ghidra_analysis/engine/agent_todos_30_r11_ai_results.jsonl"
(Get-Content $j -ErrorAction SilentlyContinue | Measure-Object -Line).Lines
Get-Content $j -ErrorAction SilentlyContinue | ForEach-Object { $_ | ConvertFrom-Json } |
  Group-Object status | Select-Object Name, Count
Get-Content $j -ErrorAction SilentlyContinue | ForEach-Object { $_ | ConvertFrom-Json } |
  Sort-Object { [int]$_.id } | Select-Object id, status, archetype, title
```

### Expected completion

| Milestone | Lines in jsonl | Reports in `ai_recovery/` |
|-----------|----------------|---------------------------|
| Coordinator only | 1 (id 30) | `r11_ai_worker_30_report.md` |
| Partial | 1–29 | `r11_ai_task_{01..NN}_report.md` |
| Full round | 30 | 29 task reports + coordinator report |

### Suggested jsonl fields (workers 1–29)

Per [ROUND11_AI_PROTOCOL.md](../ROUND11_AI_PROTOCOL.md):

- `id`, `round` (11), `status` (`DONE` | `PARTIAL` | `BLOCKED` | `NO_ACTION`)
- `archetype`, `title`, `seed_address`
- `report`: `ghidra_analysis/engine/ai_recovery/r11_ai_task_{NN}_report.md`
- `ghidra_actions`: list (empty if none)
- `frida`: path or `none`
- `remaining_unk`: strings if `PARTIAL`/`BLOCKED`

Coordinator line uses `kind: coordinator` and empty `ghidra_actions`.

---

## Aggregate status (at coordinator publish)

| id | R11 report | jsonl | Notes |
|----|------------|-------|-------|
| 1–29 | not started | not started | Parallel agents launched; re-scan after completion |
| 30 | this file | appended | No Ghidra mutations |

**Re-scan command:**

```powershell
1..29 | ForEach-Object {
  $n = '{0:D2}' -f $_
  $r = "ghidra_analysis/engine/ai_recovery/r11_ai_task_${n}_report.md"
  [PSCustomObject]@{ id = $_; report = (Test-Path $r) }
} | Format-Table
```

---

## Evidence

| Claim | Source |
|-------|--------|
| 30 worker tasks + archetypes | [agent_todos_30_r11_ai.json](../agent_todos_30_r11_ai.json) |
| Slot taxonomy | [map_slots_spawner.md](../../gameplay/map_slots_spawner.md) |
| Coordinator pattern | [round6_logic_worker_50_report.md](../logic_recovery/round6_logic_worker_50_report.md) |
| Prior AI partials | R5 w02/w03, R6 logic 01/02/10, R10 deep 01/12 |

## Ghidra deltas

**none** — coordinator scope.

## Frida

**none** — coordinator scope. R12 should add `scripts/frida/` hooks for TryBot, fire-delay, and dummy stationary proofs.

## Remaining UNK

- All **29** worker acceptance criteria (see per-task matrix).
- **0/29** R11 reports at publish — full round completion pending worker handoff.
