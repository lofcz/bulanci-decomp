# Round 6 — Logic Task 06 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 6 |
| **title** | Logic cluster: CBulanek_AddEntity + scheduler/gaming neighbors |
| **range** | `cluster_41ab` (`0x0041a`–`0x0041b`) |
| **seed_address** | `0x0041a390` |

## Status

**PARTIAL** — Seed **`CGaming_AddEntity`** (Ghidra label `CBulanek_AddEntity`) control flow, **`CGaming*`** `this` proof, caller xrefs, and neighbor roles are documented from live Ghidra decompile/disasm (first MCP batch) plus `bulanci.ghidra.exe.c`. **Ghidra MCP disconnected** before `set_function_this_type` / rename / `save_program`. Prior R3 typing **`CBulanek *`** on the seed is **incorrect** (decompiler uses `this[2]` pointer arithmetic to reach `CGaming` offsets).

## Functions

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x0041a390` | `CBulanek_AddEntity` | **`CGaming::AddEntity`** — bind `entity+0x84 = this` (`CGaming*` `pGaming_host`); optional mode set via `0x004168c0`; attach entity under `pEntityViewRoot@+0x31C` with depth-sort list maintenance (`+0x348`/`+0x34C`) | **Disasm:** `MOV ESI,ECX`; `MOV [EDI+0x84],ESI` @ `0x0041a3a6`; `MOVZX EAX,[ESI+0x341]` @ `0x0041a3ac`; `[ESI+0x31c]`/`[ESI+0x348]`/`[ESI+0x34c]`; `CALL 0x004184a0` (`CGaming_InsertEntityByDepth`). **Xrefs (16):** spawn, weapons, explosions, net pickup, script insert — see table below |
| `0x004168c0` | `CBulanek_SetCloneFlag` | **`CGaming_SetEntityRegisterMode`** — single-byte store `CGaming+0x341` from `register_flag` path in AddEntity; also preset to `1` in `CGaming_SpawnPracticeDummy` | **Decompile:** `*(byte *)((int)&this[2].dwPad_08 + 1) = param_1` (= `+0x341` when `this` is `CGaming*`). **Caller:** AddEntity @ `0x0041a39d` when `register_flag != 0xFF` |
| `0x004184a0` | `CGaming_InsertEntityByDepth` | Depth-keyed sibling insert using vtable `+0x74` sort key; updates `pDepthInsertHead` / tail chain | Export comment @ `82507`; callee from AddEntity mode `1` @ `0x0041a41f` |
| `0x0041ab40` | `CDSChain_AdjustThisOffset_ThisMinus94` | **`CAnim`** MI adjustor — `CDSChain_AdjustThisOffset(this - 0x94)` | Live decompile; `__fastcall` thunk |
| `0x0041ab50` | `CAnim_ScalarDeletingDtor` | **`CAnim`** scalar deleting dtor → `CAnim_dtor`; optional `_free` | Live decompile |
| `0x0041aca0` | `CGaming_GetClassTable` | RTTI/class-registry pointer — `return &DAT_004b3788` | Live decompile |
| `0x0041acb0` | `CGaming_ScalarDeletingDtor_thunk_Sub4` | **`CGaming`** MI adjustor `-4` → `CGaming_ScalarDeletingDtor` | Export @ `85350`; `this[-1].dwWeaponSpawnerMode` base |
| `0x0041acc0` | `CGaming_ScalarDeletingDtor_thunk_Sub68` | **`CGaming`** MI adjustor `-0x68` → scalar dtor | Export @ `85359` |
| `0x0041acd0` | `CGaming_ScalarDeletingDtor_thunk_Sub10` | **`CGaming`** MI adjustor `-0x10` → scalar dtor | Export @ `85368` |
| `0x0041ace0` | `CGaming_ScalarDeletingDtor_thunk_Sub18` | **`CGaming`** MI adjustor `-0x18` → scalar dtor | Export @ `85377` |
| `0x0041acf0` | `CGameView_OnEvent` | **Vtable slot 27** — script slot events `0xD7`/`0xD8`; `0xF9` → `CGaming_UnregisterAndRemoveObject` + dtor | Export @ `85390`; R4 typed `CGameView *`; reads `pGaming_host@+0x84`, `bPlayerSlot@+0x70` |
| `0x0041ad80` | `FUN_0041ad80` | **Scheduler tick-dispatch node alloc** — `Runtime_MallocOrThrow(8)` `{kind, slotRef}` → `CIntListInsertSortedOrAppend` into caller list; shared by `CBulanek_SchedulerTick`, `CShot`, `CMina`, `CGaming_OnSchedulerTimer` | Export @ `85419`; R5 w12 notes shared engine helper |

### Seed callers (`get_xrefs_to@0x0041a390`, live Ghidra)

| Caller | Site | Context |
|--------|------|---------|
| `CGaming_SpawnAndInitializePlayer` | `0x0041f583` | After `CBulanekCtor`; `AddEntity(this, player, -1)` then separate `CGaming_RegisterObjectAtSlot` |
| `CBulanek_OnDeath` | `0x0041fa74`, `0x0041fad0` | Corpse / `CDeath2` entities into world |
| `CGaming_SpawnBulletAndPlaySound` | `0x0041f2d7` | `CShot` register with `register_flag=1` |
| `CWeapon_Fire` / pistol / grenade | `0x00421327`, `0x004213eb`, `0x004214a7`, `0x0041fdb4`, `0x0041feb2` | Projectile / mine spawn |
| `CShot_ResolveHit` | `0x0041dddeb` | Explosion entity on rocket hit |
| `_Globals::ExplodeMine` | `0x0041e11d` | Mine detonation entity |
| `CBulanek_CreateRespawnTeleportPair` | `0x0041d19c`, `0x0041d1f1` | Teleport gates (`register_flag=1`) |
| `CGame_OnNetMsg_t18_PlaceWorldPickup` | `0x0041d2b4` | Network pickup icon |
| `CBulanci_RegisterPlayerAndRespawn` | `0x0041b39b` | Lobby → match entity hookup |
| `CLevelScriptOpExt_InsertView` | `0x0041a4bb` | Script `InsertView` — `AddEntity(gaming, entity, -1)` |

### Seed control flow (`0x0041a390`)

```
CGaming* this (ECX/ESI)
  ├─ if register_flag != 0xFF → CGaming_SetEntityRegisterMode(this, register_flag)  // [this+0x341]
  ├─ *(CGaming**)((byte*)entity + 0x84) = this     // pGaming_host on CGameView entities
  └─ switch (byte [this+0x341])
       0 → CDSView__AddChild(pEntityViewRoot@+0x31C, entity, pDepthInsertHead@+0x348)
       1 → CDSView__AddChild(..., depth word @+0x34C); maybe refresh pDepthInsertHead;
             CGaming_InsertEntityByDepth(this, entity); return
       2 → CDSView__AddChild(..., 0); update head/tail if zero; return
```

**This typing correction:** `ECX`/`ESI` uses **`CGaming`** fields (`+0x31C`, `+0x341`, `+0x348`, `+0x34C`), not `CBulanek` (412 B). Wrong `CBulanek *` typing forces decompiler `this[2].…` (= offset `+0x338`…) per `bulanci.ghidra.exe.c` @ `84619`. **`entity+0x84` stores `CGaming*`** ([CShot.md](../struct_recovery/CShot.md), [combat_projectiles.md](../../gameplay/combat_projectiles.md)); R3/R4 “owner `CBulanek*`” on this store is **superseded**.

**Out of scope but paired:** `CGaming_RegisterObjectAtSlot@0x00482074` fills `apEntitySlots[slot]` @ `+0xC8` — separate from AddEntity’s view-tree path (`CGaming_SpawnAndInitializePlayer` calls both).

## Ghidra deltas

**None applied** (MCP `Not connected` after initial read-only batch).

**Recommended when MCP is back (single `save_program`):**

| Action | Target | Rationale |
|--------|--------|-----------|
| `rename_function_by_address` | `0x0041a390` → `CGaming_AddEntity` | `CGaming` field xrefs; 16 callers pass gaming modal `this` |
| `set_function_this_type` | `0x0041a390` → `CGaming *` | Disasm `[ESI+0x31c]` / `[ESI+0x348]` / `[ESI+0x341]` |
| `set_function_prototype` | `0x0041a390` | `void __thiscall CGaming_AddEntity(CGaming *this, void *entity, char register_flag)` |
| `rename_function_by_address` | `0x004168c0` → `CGaming_SetEntityRegisterMode` | Only writes `CGaming+0x341`; misnamed “SetCloneFlag” |
| `set_function_this_type` | `0x004168c0` → `CGaming *` | Same `this[2]+1` → `+0x341` proof |
| `set_function_this_type` | `0x004184a0` → `CGaming *` | InsertEntityByDepth depth lists on `CGaming` |
| `set_decompiler_comment` | `0x0041a3a6` | `entity+0x84 = CGaming* pGaming_host (not CBulanek*)` |
| `rename_function_by_address` | `0x0041ad80` → **`Scheduler_EnqueueEvent`** (not gaming-specific — tasks 01/04/05) | 8-byte `{kind,slotRef}` list insert; shared by Bulanek/CShot/Mina/CAnim |
| `force_decompile` | `0x0041a390`, `0x004168c0`, `0x004184a0` | Verify `this->pEntityViewRoot` / `bEntityRegisterMode` |
| `save_program` | `bulanci.exe` | Once per agent |

Neighbors `0x0041ab40`–`0x0041ace0`: **skip** — already named MI/RTTI thunks.

## Frida

**none** — static disasm/decompile sufficient for AddEntity host binding and register-mode branches.

## Remaining UNK

- **`bEntityRegisterMode@+0x341` init** — no explicit ctor store located; default `0` inferred from branch fall-through only.
- **Exact depth argument** to `CDSView__AddChild` in mode `1` — decompile packs words at `+0x34C`/`+0x34E` (`pDepthInsertTail` region); semantic name deferred.
- **`FUN_0041ad80` list head** — first parameter is caller-owned intrusive list (scheduler tick batch); list field on `CGaming` / `CBulanek` not pinned in this slice.
- **Live Ghidra re-verify** of neighbor addresses `0x0041acd0`/`0x0041ace0` thunk adjustors — mapped from export order, not re-disassembled this session after MCP drop.

## Struct doc updates

- [CGaming.md](../struct_recovery/CGaming.md) — added proven **`bEntityRegisterMode@+0x341`** row (R6 todo 6).
