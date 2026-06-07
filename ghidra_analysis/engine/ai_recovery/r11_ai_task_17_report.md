# Round 11 — AI Task 17 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 17 |
| **title** | SpawnOpponentEx — skin pool + 6-arg spawn |
| **archetype** | `script_spawn` |
| **seed_address** | `0x0041f6a0` |
| **addresses** | `0x0041f6a0`, `0x0041f230`, `0x00418b20` (CollInsert), `0x00417f40` (LookupBulletFrameStrip) |
| **acceptance** | CollInsert pool; parentSlot; kind/team/flags semantics |

## Status

**DONE** — Live Ghidra MCP (`connect_instance("bulanci")`, `bulanci.exe`) + overlay script `65859` (`res_0000065859_2026_Script.script.asm`). CollInsert → global-12 pool build, `SpawnOpponentEx` 6-arg map, and `frameId`/pool-index path into `CGameView_LookupBulletFrameStrip` disasm-proven. Decompiler comment corrected @ `0x0041f6a0` (pool index, not random). Shared `CGaming_SpawnBulletAndPlaySound` / `CShot` pipeline cross-ref [r11_ai_task_18_report.md](./r11_ai_task_18_report.md).

## AI archetype

| Role | Engine reality |
|------|----------------|
| Script “opponent” ambush (8-bit zombie peek) | **`CShot`** entity with **CollInsert-selected `BitmapSpecial` frame strip** — not `CBulanek` |
| Skin variety | Script builds a **`CDSCollection`** at level init; **`poolIndex` (3rd arg)** picks `m_items[poolIndex]` at spawn time |
| Parent view | **`parentSlot` (4th arg)** resolves `GetObjectAtSlotSafe(slot)+0x20` for SFX attenuation anchor and immediate `SpatialQuery` hit target |

Manifest listed `0x00418770` — live Ghidra name is **`CLevelScriptOpExt_BindToSlot`** (opcode slot registration). **Out of scope** for skin pool; pool builder is **`CollInsert` @ `0x00418b20`**.

## Algorithm

### Phase 1 — OnInit skin pool (`CollInsert` → global 12)

Level **65859** (`V zajetí 8-bitu`) `OnInit` @ `0x009f`:

```text
gvar12 = NewCollection()                    // empty CDSCollection, capacity 32
CollInsert(gvar12, -1, GetImage(65633))     // pool[0] — level_8bit_spawn_pool_0 (9×5)
CollInsert(gvar12, -1, GetImage(65634))     // pool[1] — level_8bit_spawn_pool_1 (9×5)
CollInsert(gvar12, -1, GetImage(65635))     // pool[2] — level_8bit_spawn_pool_2 (5×9)
CollInsert(gvar12, -1, GetImage(65632))     // pool[3] — level_8bit_spawn_pool_3 (5×9)
```

`CLevelScriptOpExt_CollInsert` @ `0x00418b20`:

```text
CollInsert(coll, index, value):
  coll, index, value = ReadSubExpr() × 3
  if coll == NULL: return NULL
  insertIdx = index
  if index < 0: insertIdx = coll.m_count          // -1 → append
  CDSCollection_Insert(coll, value, insertIdx)
  return coll
```

`GetImage(id)` @ `0x00418b60` loads `CMenuGetResourceById` handle stored in collection slot.

`OnDeinit` @ `0x047d`: `SetGlobalVar(12, FreeObject(gvar12))`.

### Phase 2 — `SpawnOpponentEx` (opcode 98 @ `0x0041f6a0`)

**Script arity:** 6 sub-expressions — `(x, y, poolIndex, parentSlot, flags, skinPool)`.

```
SpawnOpponentEx(x, y, poolIndex, parentSlot, flags, skinPool):
  // First two ReadSubExpr write directly into local int[2] at [ESP+0x10]/[ESP+0x14]
  x, y, poolIndex, parentSlot, flags, skinPool = ReadSubExpr() × 6
  if parentSlot == 0xFF:
    audioAnchor = NULL
    ownerSlotForShot = 0xFF
  else:
    view = CGaming_GetObjectAtSlotSafe(pGaming, parentSlot)   // 0x00416810
    audioAnchor = view + 0x20
    ownerSlotForShot = parentSlot
  CGaming_SpawnBulletAndPlaySound(
    pGaming,
    &{x,y},
    poolIndex,              // → CShot::bDirection AND LookupBulletFrameStrip index
    ownerSlotForShot,
    flags,
    audioAnchor,
    skinPool                // → LookupBulletFrameStrip frameId (collection handle)
  )
  return 0
```

**Disasm proof:** `CALL 0x0041f230` @ `0x0041f720`; 6th arg `PUSH EDI` @ `0x0041f70c` (skin pool); `PUSH 0` only in sibling `SpawnEnemyAt` @ `0x0041f672`.

### Phase 3 — Frame strip from pool (`LookupBulletFrameStrip` @ `0x00417f40`)

```
LookupBulletFrameStrip(gaming, poolIndex, frameId, weaponStrength):
  if frameId == 0:
    return gaming.pPad_71[(poolIndex + weaponStrength*4)*4 + 0x17]   // default bullet table
  else:
    item = *(void**)(frameId.m_items + poolIndex*4)   // coll+0x8
    return HandleVirtualBaseCast(item, 0xD)             // BitmapSpecial → frame strip
```

**Not random:** `poolIndex` is the **explicit script constant** (e.g. `IntConst(3)` or `IntConst(1)`). Prior comment “random thumb” was **incorrect** — corrected in Ghidra.

### Phase 4 — Ambush sequence (script 65859, `OnBitmapEvt` slot 9)

| Step | Bytecode | Meaning |
|------|----------|---------|
| 1 | `SpawnOpponentEx(221, 136, 3, 9, 2, gvar12)` | Spawn at (221,136); pool entry **[3]** = res `65632`; parent **slot 9**; `flags=2` → `weaponStrength=1` |
| 2 | `SpawnAtView(0, 9)` | Play sample **0** at slot-9 view (opcode 65 = `TriggerLevelScriptSound`, not entity spawn) |
| Alt | `SpawnOpponentEx(260, 96, 1, 9, 2, gvar12)` | Same parent slot; pool **[1]** = res `65634` |

`CShot_Ctor` runs immediate `SpatialQuery` on spawn rect; overlap with slot-9 view → `CShot_ResolveHit` → `CGaming_OnSlotPlacementEvent` (script events `0xD7`/`0xD8`).

### Arg semantics (proven)

| Arg | Script name | C / engine use |
|-----|-------------|----------------|
| 1–2 | `x`, `y` | World origin → `CShot+0x20` rect via `local int[2]` |
| 3 | `poolIndex` | `CShot::bDirection` (+0xA4); **index into `skinPool.m_items`** when `frameId≠0` |
| 4 | `parentSlot` | `0xFF` → no view lookup; else `GetObjectAtSlotSafe` → audio anchor `+0x20`, `CShot::bOwnerSlotId` |
| 5 | `flags` | `(flags&3)≠0` allocates shot; `weaponStrength=(flags&3)-1`; `(flags&0xF0)` → optional SFX bank sample |
| 6 | `skinPool` | `CDSCollection*` passed as `frameId` to `LookupBulletFrameStrip` |

**`flags=2` on 65859:** `(2&3)-1 = 1` → pellet-strength `CShot` path; no high-nibble SFX.

## Functions table

| Address | Symbol | Role |
|---------|--------|------|
| `0x00418a50` | `CLevelScriptOpExt_NewCollection` | Allocates `CDSCollection` (vtable `0x47f700`, cap 32) |
| `0x00418b20` | `CLevelScriptOpExt_CollInsert` | `CDSCollection_Insert`; index `-1` → append @ `m_count` |
| `0x00418b60` | `CLevelScriptOpExt_GetImage` | Resource id → heap image handle for pool entries |
| `0x004310b0` | `CDSCollection_Insert` | Memmove insert when `param_2 == m_count`; increment count |
| `0x0041f6a0` | `CLevelScriptOpExt_SpawnOpponentEx` | Opcode 98; 6-arg spawn; passes pool handle to helper |
| `0x0041f230` | `CGaming_SpawnBulletAndPlaySound` | `CShot` alloc + `AddEntity` + optional SFX |
| `0x0041edf0` | `CShot_Ctor` | Position, `poolIndex`, owner slot, `weaponStrength`, `frameId` |
| `0x00417f40` | `CGameView_LookupBulletFrameStrip` | Pool vs default table branch |
| `0x00416810` | `CGaming_GetObjectAtSlotSafe` | `parentSlot` view resolve |
| `0x00418770` | `CLevelScriptOpExt_BindToSlot` | *(manifest address)* slot bind — not pool-related |

## Struct fields

| Struct | Offset | Field | CollInsert / spawn use |
|--------|--------|-------|------------------------|
| `CDSCollection` | `+0x08` | `m_items` | `void**` array of `GetImage` handles |
| `CDSCollection` | `+0x0C` | `m_count` | Append index when script passes `-1` |
| `CShot` | `+0x20` | `pWorldRect` | Spawn `(x,y)` + frame w/h |
| `CShot` | `+0xA4` | `bDirection` | Stores **`poolIndex`** from script arg 3 |
| `CShot` | `+0xA5` | `bOwnerSlotId` | `parentSlot` (or `0xFF`) |
| `CShot` | `+0xA6` | `bWeaponStrength` | `(flags&3)-1` |
| `CDSView` / entity | `+0x20` | world origin | Parent view anchor for audio + hit test |

## Ghidra deltas

| Action | Address | Detail |
|--------|---------|--------|
| `set_decompiler_comment` | `0x0041f6a0` | Replaced “random thumb” with proven 6-arg map + pool index semantics |
| `save_program` | `bulanci.exe` | Persisted comment |

## Decomp fixes

| Item | Fix |
|------|-----|
| “Random skin from pool” | **Wrong** — `poolIndex` arg selects `m_items[poolIndex]` deterministically |
| `extraout_*` local names in `SpawnOpponentEx` | Ghidra artifact; disasm stack layout maps to `{x,y}`, `poolIndex`, `parentSlot`, `flags`, `skinPool` |
| `0x00418770` in task manifest | Live symbol is `BindToSlot`, not CollInsert — documented above |

## Frida

Not run — static disasm + overlay bytecode sufficient.

## Remaining UNK

| Item | Notes |
|------|-------|
| `HandleVirtualBaseCast(..., 0xD)` RTTI name | Frame-strip facet id `0xD` on `BitmapSpecial` — class id string not closed this pass |
| Other levels using `SpawnOpponentEx` | Only **65859** pool documented; grep other overlay scripts for `CollInsert` + opcode 98 |
| Whether `poolIndex` ever equals `_rand()` at script level | No engine-side random; script could pass `Rand()` — not seen on 65859 |

## Cross-references

- [r11_ai_task_18_report.md](./r11_ai_task_18_report.md) — `SpawnEnemyAt` diff (`frameId=0`)
- [agent_05.md](../../asset_catalog/naming/agent_05.md) — pool resource ids `0x10060`–`0x10063`
- [script_dispatch_table.md](../script_dispatch_table.md) — opcodes 50 (`CollInsert`), 98 (`SpawnOpponentEx`)
- [CDSCollection.md](../struct_recovery/CDSCollection.md) — `m_items` / `Insert` layout
- [combat_projectiles.md](../../gameplay/combat_projectiles.md) — `CShot` spawn / hit pipeline
