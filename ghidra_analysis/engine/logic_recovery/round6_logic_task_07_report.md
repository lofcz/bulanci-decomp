# Round 6 — Logic Task 07 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 7 |
| **title** | Logic cluster: CExplosion_CollectLandminesInBlastRect + scheduler/gaming neighbors |
| **range** | `cluster_41ab` (`0x0041a`–`0x0041b`) |
| **seed_address** | `0x0041a2f0` |

## Status

**DONE** — Seed blast landmine query, caller chain-detonation path, and `CGaming*` / `CExplosion*` typing are **re-verified** (Ghidra batch decompile + seed disasm). Neighbor slice fully characterized. **No new Ghidra mutations** this session (R3/R4/R5 already applied on seed and blast helpers; MCP disconnected before optional `CGaming*` fix @ `0x0041b500`).

## Functions

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x0041a2f0` | `CGaming::CExplosion_CollectLandminesInBlastRect` | Walks entity view chain from `CGaming+0x31c` (`pEntityViewRoot`), sibling links via `+0x4C` / `CDSChained_GetNextSiblingView`. For each view, `GetTypeInfo` → filter **`classId == 0x816`** (CMina landmine). Copies entity AABB (`+0x20..+0x2C`), `rect_Intersect` with `blastRect`; on overlap, stores `CDSView*` in `mineBuf[]`. Returns count. | **Disasm entry:** `MOV ECX,[ECX+0x31c]; ADD ECX,0x54` → `CDSChained_GetFirstChildView`; loop `CMP [EAX+0x8],0x816`. **Decompile:** `CGaming *this`. **Caller:** `CExplosion_ApplyAreaDamage@0x0041e296` with `local_1010 = explosion->pGaming`. |
| `0x0041adb0` | `CBulanek::CBulanek_dtor` | Player avatar teardown: restore six vtables; release three audio player refs (`+0x5C..+0x5D`); remove self from host `CIntList` @ `param_1[0x3C]`; nested `CDSVideoPlayer` / `CDSUpdatedItem` / `CDSView` dtors. | **Decompile:** vtable restores to `g_pCBulanek_*`. **Xref:** `CBulanek_DtorScalar@0x0041bc23`. Not on explosion path — cluster neighbor only. |
| `0x0041b190` | `CAnim::FUN_0041b190` | **Scheduler hook (vtable slot 30):** if scheduler slot 0 on `track_manager` is not armed (`slot+8` bit0 clear), `Scheduler_EnqueueEvent(param_1, 0, &track_manager)`. Shared by `CExplosion`, `CTeleportPoint`, `CMina`, death anim types. | **Decompile:** uses `(this->track_manager).scheduler`. **Xrefs:** DATA vtables; **CALL** `CMina_OnSchedulerHook@0x0041b219`; thunk @ `0x0041b1c0`. |
| `0x0041b1c0` | `CAnim::thunk_FUN_0041b190` | MI/vtable thunk — identical body to `0x0041b190` (death anim vtables `CDeath` / `CDeath2` slot 30). | **Decompile:** same as `0x0041b190`. **Xref:** `JMP` target from thunk entry. |
| `0x0041b250` | `CExplosion::CExplosion_DamageAtPoint` | Epicenter / ray-step damage: reverse-walk `entityBuf`; skip slot `0xFF`; intersect probe rect with entity bbox (`vtable+0x70`); on hit, `CGaming_TryGetPlayerCoords` + up to **4×** `CGaming_OnSlotPlacementEvent` using `this->pGaming` and `this->bOwnerSlot@+0xF0`. Returns **1** if ray blocked (octant flag `|0x80` when `rayStep`). | **Decompile:** `CExplosion *this`. **Xrefs:** `CExplosion_ApplyAreaDamage@0x0041e1f0`, `@0x0041e273` (8-dir loop, 15 steps). |
| `0x0041b390` | `CBulanci::CBulanci_RegisterPlayerAndRespawn` | Decompiler: `CBulanek::AddEntity` + `CGaming_RespawnPlayerAtSafeLocation`. **Caller mismatch:** sole dynamic caller `CGaming_TickAmbientAnimations@0x0041b535` passes **`[ECX+0x324..0x330]`** (`pAmbientAnim*`) as stack arg while **`ECX` is the gaming shell** (see `0x0041b500` disasm). Treat **symbol / callee typing as UNK** until live disasm of `0x0041b390` body is rechecked. | **Xref:** 4× `CALL` from `0x0041b500` switch cases. |
| `0x0041b3b0` | `CGaming::FUN_0041b3b0` | Custom-event purge: reverse-iterate `vecSlotVec_2f8` (`+0x2F8`); if `entity+0x3C == param_1`, remove from vector, `CGaming_UnregisterAndRemoveObject`, `Release` — **max 2** removals. | **Decompile:** `CGaming *this`; UNCERTAIN comment notes net case `0xF5`. **This:** already `CGaming *`. |
| `0x0041b420` | `CGaming::CGaming_AppendDangerZoneNode` | Allocates `0x20` B zone node (`OperatorNew`), copies rect, flags, optional owner view (`OR 0x200` on owner `+0x14`); `CIntListInsertSortedOrAppend` on `vecSlotVec_2d8` (`+0x2D8`); tail-call **`CMina_UpdateTraceAreas`**. | **R5 w44:** renamed + `set_function_this_type` → `CGaming *`. Script export **#28** `DefineDangerZone` → this. |
| `0x0041b4a0` | `CMina::CMina_ReleaseDangerZoneNode` | Clears danger rect on node (`+4` non-zero path), refreshes trace areas; `CIntList_BinarySearch` + `CDynPtrArray_RemoveRange` on gaming list; `_free` node. | **R5 w44:** `CMina *` + rename. Used from mine dtor / danger-zone teardown. |
| `0x0041b500` | `CGaming::CGaming_TickAmbientAnimations` | Scheduler **event 0** (solo/tutorial ambient): `_rand` → switch 0..3 picks one of **`CGaming+0x324..+0x330`** ambient anim pointers, calls `0x0041b390`, then `CBulanci_SetAmbientAnimMode@0x00417c80` with mode **1**. | **Disasm:** `MOV ESI,ECX`; reads `[ESI+0x324]`…`[ESI+0x330]` — matches `CGaming.md` ambient fields. **Xref:** `CGaming_OnSchedulerTimer` → `CALL 0x0041b500`. **Decompiler wrong:** shows `CBulanci *` / bogus `param_1->game.chain` offsets. |

### Seed control flow (blast landmines)

```
CExplosion_ApplyAreaDamage (CExplosion* ESI)
  ├─ blastRect = bbox ± 0x3C (60 px)
  ├─ local_1010 = this->pGaming  (+0x84)
  ├─ CollectEntitiesInBlastRect(CGaming*, blastRect, buf, includeHidden=1)
  ├─ if count==0 → return
  ├─ DamageAtPoint @ epicenter + 8-direction ray loop (15 steps, UNK_004828c0 deltas)
  └─ for i = CollectLandminesInBlastRect(local_1010, blastRect, buf); i>0; i--
        └─ _Globals__ExplodeMine(mineBuf[i-1])   // chain-detonate CMina
```

### Seed callers (`get_xrefs_to@0x0041a2f0`)

| Caller | Site | Context |
|--------|------|---------|
| `CExplosion_ApplyAreaDamage` | `0x0041e296` | After entity AOE; `MOV ECX,[ESP+0x40]` = `pGaming` stack slot → landmine collect → `ExplodeMine` loop |

## Ghidra deltas

**None applied** (MCP disconnected after read-only `batch_decompile` / seed disasm; prior rounds already hold).

**Re-verified (no change needed):**

| Address | Existing typing | Proof |
|---------|-----------------|-------|
| `0x0041a2f0` | `CGaming *` | Disasm `[ECX+0x31c]+0x54`; decompile `CGaming::CExplosion_CollectLandminesInBlastRect` |
| `0x0041b250` | `CExplosion *` | Caller passes `local_100c` explosion; uses `pGaming`, `bOwnerSlot` |
| `0x0041b420` | `CGaming *` | R5 worker 44 |
| `0x0041b4a0` | `CMina *` | R5 worker 44 |

**Recommended when MCP is back:**

| Action | Target | Rationale |
|--------|--------|-----------|
| `set_function_this_type` | `0x0041b500` → `CGaming *` | Disasm `ESI=ECX` + offsets `+0x324..+0x330` are CGaming ambient anim fields, not `CBulanci` |
| `force_decompile` | `0x0041b500` | After `this` fix, should show `pAmbientAnim0`…`pAmbientSky` |
| `rename_function_by_address` | `0x0041b190` → `CAnim_OnSchedulerEnqueueSlot0` | Body proven; shared vtable slot 30 |
| `disassemble` + possible rename | `0x0041b390` | Decompiler name/callees disagree with ambient-tick caller |
| `save_program` | `bulanci.exe` | Once if any mutation applied |

## Frida

**none** — landmine class filter `0x816`, view-chain walk, and `ApplyAreaDamage` → `ExplodeMine` chain are proven statically (see [combat_projectiles.md](../../gameplay/combat_projectiles.md), [damage_pipeline.md](../../gameplay/damage_pipeline.md)). Runtime hook only needed if a future build moves `pEntityViewRoot` or changes class id.

## Remaining UNK

- `0x0041b390` — true role when passed **`CBulAnim*`** from ambient tick (decompiler still shows `CBulanek::AddEntity` with `(CBulanek *)this`).
- `CAnim::FUN_0041b190` / `FUN_0041ad80` paired rename (R5 worker 02 note) — behavior known, symbol pending.
- `CGaming::FUN_0041b3b0` — which net/custom event supplies `param_1` (case `0xF5` hypothesis).
- `CMina_UpdateTraceAreas` callee convention at tail of `CGaming_AppendDangerZoneNode` — decompiler casts `CGaming*` to `CMina*` (may be host-pointer pattern; not re-disassembled this session).

## Struct doc updates

**none** — [CExplosion.md](../struct_recovery/CExplosion.md) and [CGaming.md](../struct_recovery/CGaming.md) already document `pGaming@+0x84`, `pEntityViewRoot@+0x31C`, ambient anims `@+0x324..+0x330`, and collect-helper ECX rules (R3 todo 8 / R4 re-verify).
