# Round 10 — Deep Task 03 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 3 |
| **title** | R6 rerun: CBulanek_AddEntity + entity factory |
| **seed_address** | `0x0041a390` |
| **prior** | R6 logic task 06 (`round6_logic_task_06_report.md`) |
| **acceptance** | Prove entity slot allocation path from CGaming; fix decomp mistakes on AddEntity parameters |

## Status

**DONE** — Live Ghidra MCP (`connect_instance("bulanci")`, `bulanci.exe`) + IDA/Ghidra export correlation. **`CGaming_AddEntity`** view-tree factory proven; **slot table binding is a separate `CGaming_RegisterObjectAtSlot` step** on `apEntitySlots@+0xC8`. One automatic slot-picker path (`CountOccupiedPlayerSlots + 0x24`) disasm-proven.

## Functions / Struct

| Address | Symbol (live Ghidra) | Role | Evidence |
|---------|----------------------|------|----------|
| `0x0041a390` | `CGaming_AddEntity` | **View-tree attach only** — sets `entity+0x84 = CGaming*`; optional `bEntityRegisterMode@+0x341`; `CDSView__AddChild` under `pEntityViewRoot@+0x31C` per mode `0/1/2` | **Disasm:** `MOV ESI,ECX`; `MOV [EDI+0x84],ESI` @ `0x0041a3a6`; `MOVZX EAX,[ESI+0x341]` @ `0x0041a3ac`; `[ESI+0x31c]`/`[ESI+0x348]`/`[ESI+0x34c]`; **no** `[ECX+slot*4+0xC8]` store. **IDA** `sub_41A390`: `a2[33]=this` (= `+0x84`), branches on `*(this+833)` (= `+0x341`). **Xrefs (16)** unchanged from R6 |
| `0x00417fb0` | `CGaming_RegisterObjectAtSlot` | **Slot table write** — `apEntitySlots[slot]@CGaming+0xC8`; `entity+0x14\|=0x200`; `entity+0x70=slot`; `slot>=0x80` clears prior slot | **Disasm:** `MOV [ECX+ESI*4+0xc8],EAX` @ `0x00417fc1`; `OR [EAX+0x14],0x200` @ `0x00417fc8`; `MOV [EAX+0x70],DL` @ `0x00417fce`. **IDA** `sub_417FB0`: `*(this+a3+50)=a2` (= `+0xC8`). **mapping.csv** `0x417fb0` |
| `0x004168d0` | `CGaming_GetObjectAtSlotUnchecked` | Slot table read — `return *(CGaming+0xC8+slot*4)` | Decompile `+200` (= decimal **200** = **`0xC8`**). IDA `sub_4168D0` |
| `0x004168c0` | `CGaming_SetEntityRegisterMode` | Writer for `bEntityRegisterMode@+0x341` when `register_flag != 0xFF` | `MOV [ECX+0x341],AL` (IDA `*(this+833)=a2`) |
| `0x004184a0` | `CGaming_InsertEntityByDepth` | Mode-`1` depth sibling insert after `AddChild` | Callee @ `0x0041a41f` from AddEntity |
| `0x0041f500` | `CGaming_SpawnAndInitializePlayer` | **Factory pairing:** `OperatorNew(0x19c)` → `CBulanekCtor` → **`AddEntity(-1)`** @ `0x0041f583` → **`RegisterObjectAtSlot(slot)`** @ `0x0041f58c` → respawn + facing | Disasm sequence; IDA `sub_41F500` |
| `0x0041f5d0` | `CGaming_SpawnPracticeDummy` | **Auto slot pick:** if `CountOccupiedPlayerSlots < 4`, mode `1`, spawn at **`count + 0x24`** | `ADD DL,0x24` @ `0x0041f5fa` after `CALL 0x00416970` |
| `0x00416970` | `CGaming_CountOccupiedPlayerSlots` | Counts non-null dwords in **`CGaming+0x158`..+0x167`** (4 entries) | Disasm `ADD ECX,0x158` loop |
| `0x0041d26f` | `CGame_SpawnPickupObject` | Pickup factory: alloc anim → **`RegisterObjectAtSlot`** → caller then **`AddEntity`** | Register before view attach (`CGame_OnNetMsg_t18` @ `0x0041d2b4`) |
| `0x0041ab40` | `CDSChain_AdjustThisOffset_ThisMinus94` | `CAnim` MI adjustor `-0x94` | Neighbor; no entity-factory role |
| `0x0041ab50` | `CAnim_ScalarDeletingDtor` | `CAnim` scalar deleting dtor | Neighbor |
| `0x0041aca0` | `CGaming_GetClassTable` | RTTI `&DAT_004b3788` | Neighbor |
| `0x0041acb0`–`0x0041ace0` | `CGaming_ScalarDeletingDtor_thunk_*` | MI adjustors → `CGaming_ScalarDeletingDtor` | Neighbor thunks |
| `0x0041acf0` | `CGameView_OnEvent` | Vtable slot 27; uses `entity+0x84` `pGaming_host`, `+0x70` slot byte | Events `0xD7`/`0xD8`/`0xF9` |
| `0x0041ad80` | `Scheduler_EnqueueEvent` | 8-byte `{kind,slotRef}` scheduler node alloc (not slot table) | Shared tick helper; not `apEntitySlots` |

### Slot allocation path (proven)

`CGaming_AddEntity` **does not** allocate or write `apEntitySlots`. Slot binding is always via **`CGaming_RegisterObjectAtSlot`** (or unregister with `slot >= 0x80`).

```
Entity factory (typical player spawn @ CGaming_SpawnAndInitializePlayer)
  ├─ OperatorNew(0x19c) + CBulanekCtor(slotKind in ctor args)
  ├─ CGaming_AddEntity(this, entity, 0xFF)     // view tree + pGaming_host@+0x84
  ├─ CGaming_RegisterObjectAtSlot(this, entity, slotKind)  // apEntitySlots[slot]@+0xC8
  └─ CGaming_RespawnPlayerAtSafeLocation + facing

Auto slot index (practice dummy @ CGaming_SpawnPracticeDummy, only when count < 4)
  slotKind = CGaming_CountOccupiedPlayerSlots(this) + 0x24   // disasm @ 0x0041f5fa
  → then same SpawnAndInitializePlayer chain above
```

**Pickup path order differs:** `CGame_SpawnPickupObject` calls **`RegisterObjectAtSlot` first**, then the net handler calls **`AddEntity`** (`0x0041d2b4`).

### AddEntity control flow (`0x0041a390`)

```
CGaming* this (ECX)
  ├─ if register_flag != 0xFF → CGaming_SetEntityRegisterMode(this, register_flag)  // +0x341
  ├─ *(CGaming**)(entity + 0x84) = this
  └─ switch (byte [this+0x341])
       0 → CDSView__AddChild(pEntityViewRoot@+0x31C, entity, pDepthInsertHead@+0x348)
       1 → CDSView__AddChild(..., pDepthInsertTail@+0x34C); maybe refresh head; CGaming_InsertEntityByDepth; return
       2 → CDSView__AddChild(..., 0); update tail/head if zero; return
```

### Seed callers (`get_xrefs_to@0x0041a390`, live Ghidra — 16 sites)

| Caller | Site | Slot binding |
|--------|------|--------------|
| `CGaming_SpawnAndInitializePlayer` | `0x0041f583` | `RegisterObjectAtSlot` @ `0x0041f58c` (explicit `param_1`) |
| `CGame_SpawnPickupObject` → `CGame_OnNetMsg_t18` | `0x0041d2b4` | Register @ `0x0041d26f` before AddEntity |
| `CGaming_SpawnBulletAndPlaySound` | `0x0041f2d7` | Projectile; register mode `1` via AddEntity arg |
| `CWeapon_Fire` / pistol / grenade | `0x00421327`..`0x004214a7`, `0x0041fdb4`, `0x0041feb2` | Mode `1`; slot from weapon/spawn helper |
| `CShot_ResolveHit` / `ExplodeMine` | `0x0041ddeb`, `0x0041e11d` | Explosion entities |
| `CBulanek_OnDeath` | `0x0041fa74`, `0x0041fad0` | Corpse entities |
| `CBulanek_CreateRespawnTeleportPair` | `0x0041d19c`, `0x0041d1f1` | Teleport gates |
| `CBulanci_RegisterPlayerAndRespawn` | `0x0041b39b` | Lobby hookup |
| `CLevelScriptOpExt_InsertView` | `0x0041a4bb` | `AddEntity(gaming, entity, -1)` only — script slot via separate `BindToSlot` opcode |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x00417fb0` | Fixed stale `+0x200` hex confusion → **`apEntitySlots@+0xC8`** |
| `set_decompiler_comment` | `0x0041f5fa` | Document `slot = CountOccupiedPlayerSlots + 0x24` |
| `set_function_prototype` | `0x0041a390` | `void CGaming_AddEntity(void *entity, char register_flag)` `__thiscall` |
| `save_program` | `bulanci.exe` | Saved |

**Already applied (R6 `_apply_r6_ghidra.py`):** rename `CGaming_AddEntity`, `CGaming_SetEntityRegisterMode`; `set_function_this_type` → `CGaming *` on `0x0041a390`, `0x004168c0`, `0x004184a0`; plate comment @ `0x0041a3a6`.

## Decomp corrections

| Issue | IDA ground truth | Ghidra before fix |
|-------|------------------|-------------------|
| **`this` type** | `int this` with offsets `+796/+833/+840/+844` = `CGaming` fields | R3 had `CBulanek *` → `this[2].…`; **fixed R6/R10** |
| **`entity+0x84` store** | `a2[33] = this` (132 B = `0x84`) | Now `*(CGaming**)(entity+0x84)=this` |
| **`register_flag`** | `char a3`; `-1` skips mode set | `0xFF` in disasm @ `0x0041a394` |
| **Slot table offset** | `sub_417FB0` / `sub_4168D0` use index `+50` DWORDs = **`+0xC8`** | Comment said `CGaming+0x200` (wrong hex); decompile used decimal `+200` (= `0xC8`, correct value, misleading label) |
| **`CGaming.md` Key methods** | `CGaming_RegisterObjectAtSlot` @ **`0x00417fb0`** (`mapping.csv`) | Doc listed **`0x00482074`** — **stale/wrong VA** (no function at that address in live Ghidra) |
| **`CGaming_SpawnPracticeDummy`** | `slot = count + 36` via `ADD DL,0x24` | IDA `v7` undefined; Ghidra `extraout_DL` — both wrong variable name; **disasm proves `EDX` (= count) + `0x24`** |

## Frida

**none** — static disasm + xref closure sufficient.

## Remaining UNK

- **Semantic name for `CGaming+0x158` quad** used by `CountOccupiedPlayerSlots` (not `apPlayerHudViews@+0x148`; disasm `ECX+0x158`).
- **`bEntityRegisterMode@+0x341` default** — no ctor store located; inferred `0` from branch fall-through only.
- **Depth argument packing** for mode `1` `CDSView__AddChild` tail word at `+0x34C` region — cosmetic naming deferred.
