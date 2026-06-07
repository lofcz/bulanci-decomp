# Round 10 — Deep task 19 report (`CGaming_OnPlayerCollectItem` pickup)

## Task

| Field | Value |
|-------|-------|
| **id** | 19 / 20 |
| **round** | 10 (deep_recovery) |
| **kind** | logic_rerun |
| **prior_round** | 6 / task 10 |
| **title** | Gaming: CGaming_OnPlayerCollectItem + item pickup |
| **seed_address** | `0x0041a020` |
| **addresses** | `0x0041a020`, `0x0041be70`, `0x0041bee0`, `0x0041bf00`, `0x0041bf70`, `0x0041bf80` |

## Status

**DONE** — Live Ghidra MCP disasm/xref proof for item-type quip dispatch, score net path, and caller slot→`weaponKind` mapping. Prototype parameter order corrected (`pCollector` before `pPickup`). `save_program bulanci.exe` applied.

## Functions / Struct

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x0041a020` | `CGaming_OnPlayerCollectItem` | Campaign weapon-pickup completion: unregister ambient pickup view, team-score net notify, quip SFX by `weaponKind`, clear ambient anim | **Prototype (post-fix):** `void __thiscall (CGaming *this, CBulanek *pCollector, CDSView *pPickup, uint weaponKind)`. **Disasm:** `MOV EDI,ECX` save host @ `0x0041a029`; `CGaming_UnregisterAndRemoveObject(this,pPickup)` — pickup in `EAX` ends at `[ESP+4]` after prologue push @ `0x0041a02b`; `MOV ECX,EBX` / `CALL 0x00416610` @ `0x0041a039` with `EBX`←collector `[ESP+0x14]`; quip gate `CMP byte [EBX+0x70],0` @ `0x0041a040`; jmp table `JMP dword [EBP*4+0x41a0ac]` @ `0x0041a050`; spatial atten `ADD EBX,0x20` @ `0x0041a077`. **Callers (4):** `CGaming_TickPlayerCollisions` @ `0x0041f154`, `0x0041f177`, `0x0041f19a`, `0x0041f1bd` |
| `0x00416610` | `CBulanek_NetSendTeamScoreOnCollect` | Score/HUD net wrapper on collect | **Xref:** sole caller `CGaming_OnPlayerCollectItem` @ `0x0041a03b`. **Decompile:** `CGame_NetSendTeamScoreEvent_t16(*(CGame **)(this+0xf4), weaponKind, bPlayerSlot, 0)` — `pGame` from `CBulanek+0xf4`, slot from `+0x70` |
| `0x00414550` | `CGame_NetSendTeamScoreEvent_t16` | Net msg `0x16` team-score / timer HUD | **Xref from collect:** `0x00416622`. Category `weaponKind` (`param_1`): values `<3` → `CGame_DeferTeamScoreSchedulerEvent`; `==3` → adjust `CGame+0x20e` timer ±15s; optional `CDSDirectPlay_Send` + `CGame_PostTeamScoreHudEvent_0xEE` |
| `0x0041f0c0` | `CGaming_TickPlayerCollisions` | Per-entity collision tick | **Callers (3):** `CBulanek_Update@0x0041f37e`, `CGaming_TickRoundStateAndScoring@0x0041fccb`, `TriggerTeleportFX@0x0041fbaf`. **Two pickup paths:** (A) generic map slots `0x64..0x6b` → `CGame_ApplyPickup` @ `0x0041f117` (inventory); (B) single-player gate `pOwnerGame+0xd8==1` → overlap `CGaming+0x324..0x330` → `OnPlayerCollectItem` with fixed `weaponKind` |
| `0x0041be70` | `CGaming_CreateObject` | Heap factory `OperatorNew(0x36c)` + `CGaming_Ctor` | **Xref:** vtable/factory `0x0047bc00`. Neighbor only — no pickup logic |
| `0x0041bee0` | `CGaming_ScalarDeletingDtor` | Scalar-deleting dtor thunk | Unchanged (R6) |
| `0x0041bf00` | `CWeapon::Update` | Sync walk-track embed with active weapon params | **Disasm:** `MOV EAX,[ECX+0xf8]` @ `0x0041bf00` (`CBulanek.pWeapon`); **Caller:** `CWeapon_HideAssociatedView` @ `0x0041bf70` does `MOV ECX,[ECX+0x50]` (pTrackHolder) before `CALL` @ `0x0041bf77` ⇒ **ECX = `CBulanek*`** at entry |
| `0x0041bf70` | `CWeapon_HideAssociatedView` | Vtable slot `[0]` @ `0x00481ed4` | `this` typed `CWeapon*`; calls `Update` when `pTrackHolder@+0x50` non-null |
| `0x0041bf80` | `CWeapon_SetTrackHolder` | Track-holder list swap | R5 w26 — unchanged |

### Item-type dispatch table (quip bank slots)

Gated on `pCollector->bPlayerSlot == 0` (`CBulanek+0x70`). Default `ESI = 0x22` (`sample_34`). Switch on `weaponKind` (`EBP`), bounds `CMP EBP,3` / `JA` skip @ `0x0041a04b`–`0x0041a04e`.

| `weaponKind` | Dispatch target | `ESI` (bank index) | Audio ([audio_banks.md](../../formats/audio_banks.md)) |
|--------------|-----------------|--------------------|--------------------------------------------------------|
| 0 | `0x0041a057` | `0x20` | `sample_32` quip 0 |
| 1 | `0x0041a065` | `0x23` | `sample_35` quip 1 |
| 2 | `0x0041a06c` | `0x21` | `sample_33` quip 2 |
| 3 | `0x0041a05e` | `0x1f` | `sample_31` quip 3 |
| default / non-slot-0 collector | (no table) | `0x22` | `sample_34` special |

Jump table dword vector @ **`0x0041a0ac`**: `JMP dword [EBP*4+0x41a0ac]` @ `0x0041a050`.

### Caller slot → `weaponKind` (`CGaming_TickPlayerCollisions`)

Requires `pOwnerGame->bTotalSlots == 1` (`*(pOwnerGame+0xd8)==1` @ `0x0041f129`).

| `CGaming` field | Offset | `weaponKind` pushed | Call site |
|-----------------|--------|---------------------|-----------|
| ambient weapon view A | `+0x324` | `0` | `0x0041f154` |
| ambient weapon view D | `+0x330` | `3` | `0x0041f177` |
| ambient weapon view B | `+0x328` | `1` | `0x0041f19a` |
| ambient weapon view C | `+0x32c` | `2` | `0x0041f1bd` |

**Call-site disasm** (first site @ `0x0041f148`–`0x0041f154`): `MOV EDX,[ESI+0x324]`; `PUSH 0`; `PUSH EDX`; `PUSH EDI` (collector `pEntity`); `MOV ECX,ESI` (`CGaming*`); `CALL CGaming_OnPlayerCollectItem`.

### Score vs inventory side effects (xref proof)

| Effect | Function | Xref chain | Notes |
|--------|----------|------------|-------|
| **Score / net HUD** | `CBulanek_NetSendTeamScoreOnCollect` → `CGame_NetSendTeamScoreEvent_t16` | `0x0041a03b` → `0x00416622` | Sends/schedules team-score event category = `weaponKind`; includes `bPlayerSlot` |
| **World pickup removal** | `CGaming_UnregisterAndRemoveObject` | `0x0041a02b` | Clears slot `0xff`, detaches `CDSView` from entity tree — **not** ammo/inventory |
| **Inventory / ammo** | `CGame_ApplyPickup` | `CGaming_TickPlayerCollisions@0x0041f117` only | **No xref** from `CGaming_OnPlayerCollectItem` or its score callee. Generic slot loop (`bVar3` `0x6c`↓`0x64`) handles map pickups per [powerups.md](../../gameplay/powerups.md) |
| **SFX feedback** | `TriggerBankSample` → `CDSAudioPlayer_PlayAndRelease` | `0x0041a089`, `0x0041a092` | Quip slot from table above |
| **Ambient anim** | `CGaming_SetAmbientAnimMode(this,0)` | `0x0041a09e` | Clears ambient anim mode on host |

Campaign weapon overlaps (`+0x324..+0x330`) therefore adjust **score/net + presentation** locally; **inventory mutation remains on the separate `CGame_ApplyPickup` path** for standard map slot pickups.

### Seed control flow (`0x0041a020`)

```
CGaming_OnPlayerCollectItem(this, pCollector, pPickup, weaponKind)
  ├─ CGaming_UnregisterAndRemoveObject(this, pPickup)
  ├─ CBulanek_NetSendTeamScoreOnCollect(pCollector, weaponKind)
  │    └─ CGame_NetSendTeamScoreEvent_t16(pGame, weaponKind, bPlayerSlot, 0)
  ├─ if pCollector->bPlayerSlot == 0:
  │    switch weaponKind → bank quip 0x1f..0x23 else 0x22
  ├─ ComputeSpatialAttenuationDb100(&pCollector->world origin +0x20)
  ├─ TriggerBankSample(1, 0, quipSlot, atten, …) → PlayAndRelease
  └─ CGaming_SetAmbientAnimMode(this, 0)
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x0041a020` | `void CGaming_OnPlayerCollectItem(CBulanek * pCollector, CDSView * pPickup, uint weaponKind)`, `__thiscall` |
| `set_function_prototype` | `0x00416610` | `uchar CBulanek_NetSendTeamScoreOnCollect(uint weaponKind)`, `__thiscall` |
| `set_function_prototype` | `0x0041f0c0` | `void CGaming_TickPlayerCollisions(CBulanek * pEntity)`, `__thiscall` |
| `set_function_prototype` | `0x0041bf00` | `void Update(void)`, `__fastcall` |
| `set_decompiler_comment` | `0x0041a020`, `0x0041bf00` | Pickup/score/quip + `Update` ECX=`CBulanek*` notes |
| `force_decompile` | `0x0041a020`, `0x0041f0c0`, `0x0041bf00` | Refreshed |
| `save_program` | `bulanci.exe` | Saved |

**Skipped (already correct):** `0x0041be70` (`CGaming_CreateObject`), `0x0041bee0` (dtor), `0x0041bf70` (`CWeapon*` this), `0x0041bf80` (`CWeapon_SetTrackHolder`).

## Decomp corrections

| Issue | Stale export / R6 | Live Ghidra (post-R10) | Proof |
|-------|-------------------|------------------------|-------|
| Parameter order | `(this, pPickup, pCollector, kind)` in [round6_logic_task_10_report.md](../logic_recovery/round6_logic_task_10_report.md) | `(this, pCollector, pPickup, kind)` | Call-site pushes @ `0x0041f14e`–`0x0041f151`; body uses `in_stack_00000004` as collector (`+0x70`) |
| `void*` param confusion | `Unregister(this,pCollector)`; quip on `pPickup+0x70` | `Unregister(this,pPickup)`; `pCollector->bPlayerSlot` | Typed prototype + `force_decompile` |
| `CBulanek_NetSendTeamScoreOnCollect` args | `(pPickup, kind)` in export | `(pCollector, weaponKind)` | `MOV ECX,EBX` @ `0x0041a039`; callee reads `this+0xf4` / `+0x70` |
| `CWeapon::Update` `this` | `CBulanek *param_1` / `int *` mix | `in_ECX` with `+0xF8` weapon field | `HideAssociatedView` loads `pTrackHolder` into `ECX` @ `0x0041bf70` |
| `CGaming_TickPlayerCollisions` `this` | `void *this` | Still `void *this` in decompiler | API limitation on `__thiscall` ECX retype; offsets `+0x324` etc. match `CGaming` |

`bulanci.ghidra.exe.c` export remains **stale** for this cluster.

## Frida

**none** — quip jump table, call-site slot mapping, and score xref chain proven statically.

## Remaining UNK

- Whether remote peers apply weapon loadout on score event `0x16` category — receive path is `CGame_ProcessNetMessage` → `CGame_NetSendTeamScoreEvent_t16` mirror, not `CGame_ApplyPickup`; weapon sync for MP may use separate `0x0C` path ([damage_pipeline.md](../../gameplay/damage_pipeline.md)).
- `CWeapon::Update` should be documented as **`CBulanek*`-entry fastcall** in `CWeapon.md` (name/namespace vs. register proof).
- Ghidra `__thiscall` `this` on `CGaming_TickPlayerCollisions` still shows `void *` despite `CGaming`-sized field accesses.

## Evidence paths

- [round6_logic_task_10_report.md](../logic_recovery/round6_logic_task_10_report.md), [CGaming.md](../struct_recovery/CGaming.md), [CBulanek.md](../struct_recovery/CBulanek.md)
- [audio_banks.md](../../formats/audio_banks.md), [map_slots_spawner.md](../../gameplay/map_slots_spawner.md), [powerups.md](../../gameplay/powerups.md)
