# Round 6 — Logic task 10 report (`CGaming_OnPlayerCollectItem` cluster)

## Task

| Field | Value |
|-------|-------|
| **id** | 10 |
| **title** | Logic cluster: CGaming_OnPlayerCollectItem + scheduler/gaming neighbors |
| **range** | `cluster_41ab` (`0x0041a000`–`0x0041bfff`) |
| **seed_address** | `0x0041a020` |

## Status

**PARTIAL** — Seed and neighbor control flow documented from Ghidra decompile/disasm/xrefs (`mapping.csv`, `report.json`). **Ghidra WRITE apply + `save_program` not completed:** MCP disconnected after the initial read batch (before `set_function_this_type` / rename / save).

## Functions

| Address | Ghidra name (current) | Role summary | Evidence |
|---------|----------------------|--------------|----------|
| `0x0041a020` | `_Globals::CGaming_OnPlayerCollectItem` | Pickup completion on host `CGaming`: unregister world pickup view, net score notify, bank quip by weapon kind, clear ambient anim mode | **This:** `MOV EDI,ECX` @ `0x0041a029`. **Callees:** `CGaming_UnregisterAndRemoveObject@0x00419ca0`, `CBulanek_NetSendTeamScoreOnCollect@0x00416610`, `ComputeSpatialAttenuationDb100@0x00422500`, `TriggerBankSample@0x00422430`, `CDSAudioPlayer_PlayAndRelease@0x004223c0`, `CBulanci_SetAmbientAnimMode@0x00417c80`. **Callers (4):** `CGaming_TickPlayerCollisions@0x0041f0c0` @ `0x0041f154`, `0x0041f177`, `0x0041f19a`, `0x0041f1bd` |
| `0x0041be70` | `_Globals::CreateObject_0041be70` | `CGaming` class factory: `OperatorNew(0x36c)` + `CGaming_ctor` | Decompile: alloc `0x36c` (= `sizeof(CGaming)`), ctor call. **Not** pickup logic; neighbor in task slice |
| `0x0041bee0` | `CGaming::CGaming_ScalarDeletingDtor` | Scalar-deleting dtor thunk; calls `CGaming_dtor`, optional `free` | Decompile; vtable `CGaming` primary slot `[1]` @ `0x00482764` |
| `0x0041bf00` | `CWeapon::Update` | Sync `CBulanek` walk-track sources with active `CWeapon` `dwParamA`/`dwParamB`/`pTrackHolder`; may call `FUN_0041af70` when `bField_168==0` | Decompile uses `param_1->pWeapon`, `pWalkTrackSources[2/3]`, `pPad_postWalkEmbed+8`; callee `0x0041af70`. **Entry register typing UNK** (see UNK) |
| `0x0041bf70` | `CWeapon::CWeapon_HideAssociatedView` | `IDSChained` vtable slot `[0]` @ `0x00481ed4`: if `this+0x50` (`pTrackHolder`) non-null, calls `Update` | Decompile: test `[param_1+0x50]`; `+0x50` = `CWeapon.pTrackHolder` per [CWeapon.md](../struct_recovery/CWeapon.md). **This type:** decompiler shows `int` — should be `CWeapon *` (disasm pending) |
| `0x0041bf80` | `CWeapon::CWeapon_SetTrackHolder` | Track-holder list swap on `CBulanek` host (`AddRef`, sorted listener insert) | R5 worker 26: asm + rename applied; no change this task |

### Seed: `CGaming_OnPlayerCollectItem@0x0041a020`

**Prototype (asm + `mapping.csv`):** `void __thiscall CGaming_OnPlayerCollectItem(CGaming *this, CDSView *pPickup, CBulanek *pCollector, uint weaponKind)` — `RET 0xc` ⇒ three stack args after `this`.

**Control flow (ordered):**

1. `CGaming_UnregisterAndRemoveObject(this, pPickup)` — clears slot registry / view tree for collected pickup (`0x00419ca0`).
2. `CBulanek_NetSendTeamScoreOnCollect(pCollector, weaponKind)` — `ECX=EBX` collector, `PUSH EBP` kind @ `0x00416610`.
3. **Quip bank slot** in `ESI`:
   - Default `0x22` (`sample_34` — special/default quip per [audio_banks.md](../../formats/audio_banks.md)).
   - If `pCollector->bPlayerSlot == 0` (`CMP byte [EBX+0x70],0` @ `0x0041a040`), switch on `weaponKind` (`EBP`, bounds `<=3`):
     - `0 → 0x20`, `1 → 0x23`, `2 → 0x21`, `3 → 0x1f` via jump table `0x0041a0ac` @ `0x0041a050`.
   - Non-zero `bPlayerSlot` keeps default `0x22` (no per-kind table).
4. Spatial attenuation: `ComputeSpatialAttenuationDb100(&pCollector->worldRect)` (`ADD EBX,0x20` before call @ `0x0041a077`).
5. `TriggerBankSample(1, 0, slot, attenuation, …)` → `CDSAudioPlayer_PlayAndRelease`.
6. `CBulanci_SetAmbientAnimMode(this, 0)` — `ECX=EDI` (`CGaming*`) @ `0x0041a09c`.

**Caller context:** Only direct xrefs are inside `CGaming_TickPlayerCollisions@0x0041f0c0` (collision/overlap tick for campaign script slots `125..127` per [map_slots_spawner.md](../../gameplay/map_slots_spawner.md)). Net path `CGame_OnNetMsg_t19_PlayerPickedUpWorldObj@0x0041f010` is adjacent but does **not** xref the seed (separate apply-pickup chain).

## Ghidra deltas

**Not applied** (MCP disconnect). Intended mutations when Ghidra is back:

| Action | Address | Target |
|--------|---------|--------|
| `set_function_this_type` | `0x0041a020` | `CGaming *` |
| `set_function_prototype` | `0x0041a020` | `void __thiscall CGaming_OnPlayerCollectItem(CGaming *this, CDSView *pPickup, CBulanek *pCollector, uint weaponKind)` |
| `rename_function_by_address` | `0x0041a020` | `CGaming_OnPlayerCollectItem` (parent `CGaming`, drop `_Globals::`) |
| `rename_function_by_address` | `0x0041be70` | `CGaming_CreateObject` |
| `set_function_this_type` | `0x0041bf70` | `CWeapon *` (prove with disasm: `ECX` = weapon, `[ECX+0x50]` = `pTrackHolder`) |
| `force_decompile` | `0x0041a020`, `0x0041bf70` | Verify typed `this` / collector fields |
| `save_program` | `bulanci.exe` | once |

**Skipped (already correct / out of scope):** `0x0041bee0` (`CGaming_ScalarDeletingDtor`), `0x0041bf80` (`CWeapon_SetTrackHolder` — R5 w26).

## Frida

**none** — pickup→quip path proven statically (disasm jump table + bank slot IDs match `audio_banks.md`). Optional follow-up: hook `0x0041a020` with `scripts/frida/bulanci_audio_logger.js` (`triggerKind` `pickup_or_other`) to log live `weaponKind` / `bPlayerSlot` at runtime; not required for acceptance.

## Remaining UNK

| Item | Why |
|------|-----|
| `CWeapon::Update@0x0041bf00` entry `ECX` type | Decompile shows `CBulanek *`; `mapping.csv` lists `CWeapon *` — need disasm at `0x0041bf00` / call site from `0x0041bf70` before `set_function_this_type` |
| `CGaming_TickPlayerCollisions` interior | Caller not decompiled this session (MCP loss); only four call sites to seed documented |
| Ghidra program save | Blocked on MCP reconnect |

## Evidence paths consulted

- [ROUND6_LOGIC_PROTOCOL.md](../ROUND6_LOGIC_PROTOCOL.md), [AGENT_PROTOCOL.md](../struct_recovery/AGENT_PROTOCOL.md)
- [CGaming.md](../struct_recovery/CGaming.md), [CWeapon.md](../struct_recovery/CWeapon.md), [CBulanek.md](../struct_recovery/CBulanek.md)
- [tick_system.md](../tick_system.md), [combat_projectiles.md](../../gameplay/combat_projectiles.md)
- [map_slots_spawner.md](../../gameplay/map_slots_spawner.md), [audio_banks.md](../../formats/audio_banks.md)
- `config/bulanci/mapping.csv` (symbol sizes/prototypes)
