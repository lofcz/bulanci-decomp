# Round 11 — AI Task 29 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 29 |
| **worker** | 29 / 30 |
| **title** | Script gap movers — `SpawnAtView`, `BindToSlot`, `OnBitmapEvt`, `IsViewKind` |
| **archetype** | `script_mover` |
| **seed_address** | `0x00418950` |
| **addresses** | `0x00418950`, `0x00418770`, `0x00418260`, `0x00418a00`, `0x00419280`, `0x00418000`, `0x00417fb0` |
| **acceptance** | Script-controlled entities in slots `4..31`, `40..99`, `108..124`; animation event bridge; `IsViewKind` idle gate |

## Status

**DONE** — Opcode handlers, slot registry, animation→script callback chain, spatial audio trigger, and `IsViewKind` idle predicate documented from live Ghidra disasm/decompile + level-script corpus (`map_slots_spawner.md`, `res_0000065856/657/659`). Ghidra plate comments applied at `0x00418950` and `0x00418a00`.

## AI archetype

**`script_mover`** — non-`CBulanek` views (`CreateAnim`, `CreateImage`, `CreateObstacle`) placed in the three engine **script gaps** between hardcoded slot bands. These entities have **no TryBot / movement AI**; behavior is entirely **bytecode-driven** via `BindToSlot` + lifecycle exports (`OnBitmapEvt`, timers, trace areas). Paired opcodes from other R11 tasks: `TranslateTo` (27), `SetAnimDirection` (28), `InsertView` (51).

### Slot gap map (reimplementation contract)

| Range (dec) | Range (hex) | Capacity | Master-pack usage |
|-------------|-------------|----------|-------------------|
| **4..31** | `0x04..0x1f` | 28 | **Primary** — 8-bit map slots 9–20 (anims + obstacles); bedtime map 4–8 (moving hazards); steel works 6, 10–14 |
| **40..99** | `0x28..0x63` | 60 | **Sandbox** — train / large maps (`res_65858` catalog); bedtime `bind_to_slot(..., 50)` for order-axis decor |
| **108..124** | `0x6c..0x7c` | 17 | **Auxiliary** — no curated master-pack `BindToSlot` ≥108 found; reserved for secondary toggles |

Engine spawners (players, pickups, campaign AI) **never write** these ranges; only `CGaming_RegisterObjectAtSlot` via opcode **56** (`BindToSlot`).

## Algorithm

### 1. `BindToSlot` — opcode 56 (`0x00418770`)

```text
BindToSlot(script):
  view  = ReadSubExpr()          // EDI
  slot  = ReadSubExpr()          // AL (low byte)
  gaming = script.pGaming @ +0x458
  CGaming_RegisterObjectAtSlot(gaming, view, slot)
  return view                    // EDI
```

**`CGaming_RegisterObjectAtSlot` (`0x00417fb0`)** — disasm-proven:

```text
RegisterObjectAtSlot(CGaming* this, void* entity, byte slot):
  if slot < 0x80:
      apEntitySlots[slot] @ this+0xC8 = entity    // MOV [ECX+ESI*4+0xC8]
      entity+0x14 |= 0x200                         // registration flag
      entity+0x70 = slot                           // gaming_slot_id
      return
  // unbind path (slot >= 0x80, scripts use IntConst(255))
  if entity+0x70 != 0xFF:
      apEntitySlots[oldSlot] = NULL
      entity+0x70 = 0xFF
```

Typical level pattern:

```lua
engine.insert_view(engine.set_order_axis(
    engine.bind_to_slot(engine.create_anim(x, y, delay, {frameIds...}), slot), depth))
```

`InsertView` (opcode 51) attaches the view tree; **`BindToSlot` is a separate step** that fills `apEntitySlots` and stamps `gaming_slot_id` for `GetSlot` / `OnBitmapEvt`.

### 2. `SpawnAtView` — opcode 65 (`0x00418950`) — spatial audio, not spawn

**Correction:** dispatch table note "spawns projectile/decal" is **wrong**. Live disasm + `TriggerLevelScriptSound` prove **bank sample playback** at a bound slot's coordinates.

```text
SpawnAtView(script):
  sampleId = ReadSubExpr()       // EDI — first subexpr
  slotId   = ReadSubExpr()       // AL  — second subexpr (byte)
  gaming   = script.pGaming @ +0x458
  TriggerLevelScriptSound(gaming, slotId, sampleId)
  return 0
```

**`TriggerLevelScriptSound` (`0x00418000`)**:

```text
TriggerLevelScriptSound(CGaming* this, byte slotId, int sampleId):
  obj = GetObjectAtSlotSafe(this, slotId)
  if obj == NULL:
      // slot 255 / empty → stereo, no pan
      TriggerBankSample(bank=1, audioCtx@+0x80, sampleId, attenuation=0, ...)
  else:
      if GetClassId(obj) == 0x7f0:          // CGameView/CBitmap bounds path
          obj.vfn[+0x70](&rect)              // anim bounds callback
      else:
          rect = obj+0x20..0x2c             // origin + width/height
      attenuation = ComputeSpatialAttenuationDb100(&rect)
      TriggerBankSample(..., sampleId, attenuation, ...)
  CDSAudioPlayer_PlayAndRelease(player, loop=0)
```

Luau API: `engine.spawn_at_view(sampleId, slotId)` — e.g. `spawn_at_view(0, 9)` plays bank sample 0 panned to slot 9's position (`audio_banks.md` §5.2). `slotId == 255` → global stereo (bedtime map `spawn_at_view(0, 255)`).

### 3. `OnBitmapEvt` — export slot 3 (`0x00418260` + thunk `0x00419280`)

**Not engine message `0xD7`.** `0xD7` is **`OnSlotPlaced`** (export 4, `CGameView_OnEvent`). `OnBitmapEvt` fires from **FLX animation frame markers** via IDSAnim vtable slot `[4]` @ `0x00481f00`.

**Thunk `CBitmap_FireOnBitmapEvtFromView` (`0x00419280`)** — `this` = view`+0x8c` (IDSAnim face):

```text
FireOnBitmapEvtFromView(IDSAnim* animFace, u16 eventCode):
  view = animFace - 0x8c
  if view.gaming_slot_id (+0x70) == 0xFF: return
  if view.gaming_host (+0x84) == NULL: return
  if (gaming_host.view_flags (+0x44) >> 4) & 1: return   // hidden/frozen host
  CLevelScript_FireOnBitmapEvt_FromView(scriptHost, view, eventCode)
```

**Bridge `CLevelScript_FireOnBitmapEvt_FromView` (`0x00418260`)** — `this` = `CBulanci*`, `pScript` @ `this+0x344`:

```text
FireOnBitmapEvt_FromView(CBulanci* host, CGameView* view, u16 eventCode):
  local0 = view.gaming_slot_id @ +0x70
  local1 = eventCode
  CDSScript::CallExport(host.pScript, exportIdx=3, argc=2, &locals)
  // → script OnBitmapEvt(slot, evt)
```

8-bit map (`res_65859`) binds cabinet anim to **slot 9**; `OnBitmapEvt` switches on `slot` and `evt` to spawn opponents, toggle obstacles (slots 11/13/15/17), and call `spawn_at_view` for retro SFX. Event codes are **per-map script constants** (0..5 on slot 9), not engine-fixed.

### 4. `IsViewKind` — opcode 87 (`0x00418a00`)

```text
IsViewKind(script):
  view = ReadSubExpr()
  if view == NULL: return false
  meta = view->vfn[0](&DAT_004b3768)     // CAnim hierarchy meta object
  if !ClassRegEntry_ListContains(meta, &DAT_004b3768): return false
  schedSlot = Scheduler_GetEventSlot(view + 0xac, 0)   // CAnim.scheduler
  return schedSlot != NULL && ((*(byte*)(schedSlot + 8) & 1) == 0)
```

Returns **true** when the view is **CAnim-shaped** and **scheduler event slot 0 is not armed** (anim idle / between clips). 8-bit `OnTimer` uses `is_view_kind(get_slot(9))` to **suppress enemy spawn RNG** while the slot-9 cabinet animation is still playing.

## Functions table

| Address | Symbol | Role | Evidence |
|---------|--------|------|----------|
| `0x00418770` | `CLevelScriptOpExt_BindToSlot` | Opcode 56: read view+slot → `RegisterObjectAtSlot` | Disasm `CALL 0x00417fb0`; returns view EDI |
| `0x00417fb0` | `CGaming_RegisterObjectAtSlot` | Write `apEntitySlots@+0xC8`; `entity+0x70`; `+0x14\|=0x200`; unbind when slot≥0x80 | Disasm `+0xC8`, `+0x70`, `+0x14` |
| `0x00418950` | `CLevelScriptOpExt_SpawnAtView` | Opcode 65: `TriggerLevelScriptSound` | Disasm `CALL 0x00418000`; args sampleId, slotId |
| `0x00418000` | `TriggerLevelScriptSound` | Slot lookup + spatial attenuation + `TriggerBankSample` | Disasm class `0x7f0` branch @ `0x00418032` |
| `0x00418260` | `CLevelScript_FireOnBitmapEvt_FromView` | `CallExport(3,2)` with slot + eventCode | Disasm `view+0x70`, `host+0x344`, `CALL 0x00438c40` |
| `0x00419280` | `CBitmap_FireOnBitmapEvtFromView` | IDSAnim vfn[4]; visibility/slot guards | Disasm `this-0x8c`, `JMP 0x00418260` |
| `0x00418a00` | `CLevelScriptOpExt_IsViewKind` | CAnim meta + scheduler slot0 idle test | Disasm `+0xac`, `AND AL,1` negate |
| `0x00416810` | `CGaming_GetObjectAtSlotSafe` | Bounds-checked slot read (SpawnAtView path) | Caller `0x0041800d` |
| `0x0042e960` | `ClassRegEntry_ListContains` | RTTI list walk for `IsViewKind` | Push `0x4b3768` @ `0x00418a14` |
| `0x0042f1e0` | `Scheduler_GetEventSlot` | `IsViewKind` anim idle probe | `view+0xac`, index 0 |
| `0x00438c40` | `CDSScript::CallExport` | Script lifecycle dispatch primitive | `OnBitmapEvt` export 3 |

## Struct fields

| Object | Offset | Name | Role in script movers |
|--------|--------|------|------------------------|
| `CLevelScript` | `+0x458` | `pGaming` | Passed to `RegisterObjectAtSlot` / `TriggerLevelScriptSound` |
| `CBulanci` | `+0x344` | `pLevelScript` | `CallExport` target for `OnBitmapEvt` |
| `CGameView` / `CBitmap` | `+0x70` | `gaming_slot_id` | Written by `BindToSlot`; read by `OnBitmapEvt`, `GetSlot` |
| `CGameView` | `+0x84` | `gaming_host` | Must be non-null for bitmap events |
| `CGameView` | `+0x44` | `view_flags` | Bit 4 set → suppress `OnBitmapEvt` (hidden) |
| `CGameView` | `+0x14` | `wViewFlags` | `\|= 0x200` on slot registration |
| `CGaming` | `+0xC8` | `apEntitySlots[128]` | Slot registry (512 B) |
| `CAnim` | `+0xAC` | `scheduler` | `IsViewKind` probes event slot 0 |
| Scheduler event | `+0x08` | `flags` | Bit 0 armed → `IsViewKind` returns false |

## Ghidra deltas

| Action | Target |
|--------|--------|
| `set_decompiler_comment` | `0x00418950` — opcode 65 = spatial bank sample (`TriggerLevelScriptSound`), not projectile |
| `set_decompiler_comment` | `0x00418a00` — CAnim idle predicate via `Scheduler_GetEventSlot(view+0xac,0)` |
| `save_program` | `bulanci.exe` |

Symbols `CLevelScriptOpExt_BindToSlot`, `CLevelScriptOpExt_SpawnAtView`, `CLevelScriptOpExt_IsViewKind`, `CLevelScript_FireOnBitmapEvt_FromView`, `CBitmap_FireOnBitmapEvtFromView` were already named in prior rounds.

## Decomp fixes

| Issue | Correction |
|-------|------------|
| `SpawnAtView` "projectile/decal" in `script_dispatch_table.md` | Opcode 65 triggers **`TriggerLevelScriptSound`** only; rename mentally to **`PlaySoundAtView(sampleId, slotId)`** |
| R11 manifest "event `0xD7`" for `OnBitmapEvt` | **`0xD7` = `OnSlotPlaced` (export 4)**; `OnBitmapEvt` = export 3 via **IDSAnim / FLX `0x0C`** frame hints |
| `BindToSlot` decomp `param_1[1].nOpcodeCount` for gaming ptr | Live disasm: **`[ESI+0x458]`** = `CLevelScript.pGaming` |
| `FireOnBitmapEvt_FromView` `this[5].pChain_pad_48` | Script pointer is **`*(CBulanci+0x344)`** per disasm @ `0x00418270` |
| `IsViewKind` "ClassID `0x7ef`" shorthand | Proven path is **`ClassRegEntry_ListContains` with sentinel `0x4b3768`** (CAnim hierarchy), not a direct `IsKindOf` immediate |

## Frida

Not required — opcode shapes, slot writes, and `CallExport(3,…)` argv layout closed from disasm.

## Remaining UNK

| Item | Reason |
|------|--------|
| Per-map `OnBitmapEvt` event code catalog | Script-layer constants; only 8-bit slot 9/10/12/14/16 partially documented |
| Gap 3 (`108..124`) live map examples | No master-pack `BindToSlot` ≥108 in curated corpus |
| `DAT_004b3768` static init | Runtime-zero in Ghidra read; populated at PE load — exact `ClassRegEntry` fields not re-dumped this pass |
| Full anim-tick model for gap entities | Frame advance / `TranslateTo` coupling deferred to R12 per worker-30 gap list |

## Cross-links

- [map_slots_spawner.md](../../gameplay/map_slots_spawner.md) — slot taxonomy + `BindToSlot` / `OnBitmapEvt` overview
- [script_dispatch_table.md](../script_dispatch_table.md) — opcode index (note SpawnAtView correction)
- [script_lifecycle.md](../script_lifecycle.md) — export slot 3 vs 4 (`0xD7`)
- [CBitmap.md](../struct_recovery/CBitmap.md) — IDSAnim vfn[4] bridge
- [audio_banks.md](../../formats/audio_banks.md) — `spawn_at_view` sample tables
- [r10_deep_task_03_report.md](../deep_recovery/r10_deep_task_03_report.md) — `RegisterObjectAtSlot` vs `AddEntity`
