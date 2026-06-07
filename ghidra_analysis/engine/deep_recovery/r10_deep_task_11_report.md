# Round 10 — Deep Task 11 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 11 |
| **title** | Struct deep: CMina landmine |
| **kind** | struct |
| **struct_name** | `CMina` |
| **seed_address** | `0x0041b210` |
| **addresses** | `0x0041b210`, `0x0041a2f0`, `0x0041b390` |

## Status

**PARTIAL** — Tail offsets `+0x108`..`+0x114` (`bArmed` gate, owner, gaming host) and trigger event `0xF2` proven with live disasm + IDA correlation. `CExplosion_CollectLandminesInBlastRect` @ `0x0041a2f0` linked to `CExplosion_ApplyAreaDamage` chain-detonation loop. **Arming (`bArmed = 1`) has no native store site** in program-wide instruction search — deferred to Frida.

## Functions / Struct

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x0041cb70` | `CMina_Ctor` | Weapon-fired mine ctor; zeros tail; binds anim | `MOV byte ptr [ESI+0x114],0` @ `0x0041cc0d`; `MOV [ESI+0x108],EAX` owner; `MOV [ESI+0x110],ECX` from `owner+0x84` |
| `0x0041cce0` | `InitMine` | Map-deploy ctor; same tail layout | `MOV [ESI+0x108],EBX` (NULL); `MOV [ESI+0x110],EAX` deploy ctx; `MOV byte ptr [ESI+0x114],BL` (BL=0 @ `0x0041cd16`) |
| `0x0041e070` | `_Globals::_Globals__ExplodeMine` | Detonate if armed; spawn `CExplosion` | **Disasm:** `CMP byte ptr [ESI+0x114],0` @ `0x0041e096`; `MOV [ESI+0x114],0` @ `0x0041e0b2`; reads `[ESI+0x108]` / `[EAX+0x70]` owner slot; `LEA EDX,[ESI+0x20]` explosion origin; `MOV ECX,[ESI+0x110]` → `CGaming_AddEntity`. **IDA:** `sub_41E070` @ `bulanci.ida.exe.c:95220` identical |
| `0x0041efb0` | `CMina_OnEvent` | **Trigger** — custom event dispatch | **Disasm:** `SUB EAX,0xF2` / `JZ 0x0041efcc` → `CALL 0x0041e070` with **ECX unchanged** (`this`). Event `0xF3` → early `RET`. Else `JMP CGameView_OnEvent`. **Vtable:** `g_pCMina_vftable_primary` slot 27 @ `0x004825e8` |
| `0x0041b210` | `CMina_OnSchedulerHook` | Scheduler slot-0 enqueue (anim + `updatedItem`) | Live decompile; calls `CAnim_OnSchedulerEnqueueSlot0` then `Scheduler_EnqueueEvent` kind 1 on `updatedItem` |
| `0x0041a2f0` | `CGaming::CExplosion_CollectLandminesInBlastRect` | Blast query: class `0x816` mines in rect | **Disasm:** walk `CGaming+0x31c` → `+0x54` child chain; `CMP [EAX+8],0x816`; AABB from `[ESI+0x20..0x2c]`; `CDSRect_Intersect`. **Xref:** sole caller `CExplosion_ApplyAreaDamage` @ `0x0041e296` |
| `0x0041e140` | `CExplosion_ApplyAreaDamage` | AOE + chain mines | After entity damage loops: `CALL 0x0041a2f0` then `CALL 0x0041e070` per collected mine (`0x0041e2aa`) |
| `0x0041eda2` | `DetonatePlayerMines` | Owner self-detonate all mines | Filters `classId==0x816` && `mine+0x108==owner`; calls `ExplodeMine` |
| `0x0041b390` | `CBulanci_RegisterPlayerAndRespawn` | *(manifest neighbor — not CMina)* | Decompile shows `CGaming_AddEntity` + respawn only; no `CMina` field access |

### CMina tail layout (proven)

| Offset | Size | Name | Proof |
|--------|------|------|-------|
| `0x108` | 4 | `pOwnerBulanek` | `CMina_Ctor` write; `ExplodeMine` `[ESI+0x108]`; `DetonatePlayerMines` `pCurrentView[0x42]` |
| `0x10C` | 4 | `pDangerZoneNode` | `InitMine`/`CMina_Ctor` zero; `CMina_RegisterDangerZone` `*(param+0x1c)` with `param=&updatedItem` |
| `0x110` | 4 | `nDeployOrOwnerCtx` | Weapon: `owner->pGaming_host`; deploy: deploy context; `ExplodeMine` `CGaming_AddEntity` ECX |
| `0x114` | 1 | `bArmed` | Ctor/Init `=0`; `ExplodeMine` CMP/MOV byte; Ghidra `get_struct_layout CMina` → 280 B (`0x118`) |

### Arming vs trigger (offsets)

| Phase | Mechanism | Offset / event | Evidence |
|-------|-----------|----------------|----------|
| **Init disarmed** | Ctor / `InitMine` | `bArmed@+0x114 := 0` | `0x0041cc0d`, `0x0041cd84` (BL cleared @ `0x0041cd16`) |
| **Arm delay (UNK)** | `CGameEntity_SetEntityType(this, 0xfffffff2)` after deploy | `animBase` entity type `@+0x6c` | Both ctors `PUSH -0xE; CALL 0x00418fe0` — landing timer hypothesis; **no `MOV byte [reg+0x114],1` in entire `.text`** (`search_instructions` operand `0x114`) |
| **Trigger** | `CMina_OnEvent` custom msg | `eventId == 0xF2` (242) | `0x0041efb0` → `ExplodeMine`; gameplay doc `combat_projectiles.md` |
| **Trigger guard** | `ExplodeMine` early out | `bArmed@+0x114 == 0` → return 0 | `0x0041e096` |
| **Chain trigger** | Explosion AOE | N/A (bypasses step-on) | `ApplyAreaDamage` → `CollectLandminesInBlastRect` → `ExplodeMine` (still respects `bArmed`) |

### `CExplosion_CollectLandminesInBlastRect` link (`0x0041a2f0`)

```
CExplosion_ApplyAreaDamage (ECX=CExplosion*)
  local_1010 = this->pGaming          ; [ESI+0x84] @ 0x41e175
  … entity CollectEntitiesInBlastRect …
  iVar3 = CExplosion_CollectLandminesInBlastRect(local_1010, &blastRect, mineBuf)  ; CALL @ 0x41e296
  while (iVar3 > 0)
      ExplodeMine(mineBuf[iVar3--])     ; CALL @ 0x41e2aa, ECX = CMina*
```

Collect helper filters views whose `GetTypeInfo()->classId == 0x816`, compares mine `animBase` bbox (`+0x20..+0x2c`) against expanded blast rect (`±0x3C` from explosion bbox).

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0041efb0` → `CMina_OnEvent` | Was misnamed `CExplosion_OnEvent` |
| `set_function_prototype` | `0x0041e070` → `uchar __thiscall CMina_ExplodeMine(void)` | Decomp now shows `+0x114`/`+0x108`/`+0x110`/`+0x20` correctly |
| `set_decompiler_comment` | `0x0041e096`, `0x0041a31d`, `0x0041efb0` | R10 offset/classId notes |
| `save_program` | `bulanci.exe` | Saved |

## Decomp corrections (IDA vs Ghidra)

| Issue | Before | After R10 |
|-------|--------|-----------|
| `ExplodeMine` this type | `CDSView*` / `param_1[2].win.wViewFlags` for `bArmed` | `void* this`; `*(char*)(this+0x114)` |
| `CMina_OnEvent` name | `CExplosion_OnEvent` | `CMina_OnEvent`; passes `this` to `ExplodeMine` |
| `CollectLandminesInBlastRect` this | Already `CGaming*` | Confirmed `ECX=[explosion+0x84]` at call site |

## Frida

**Recommended** — hook `CMina_ExplodeMine@0x0041e070` and watch `*(uint8_t*)(mine+0x114)` from deploy through first `0xF2` event to locate the native arm write (if any) or confirm script/timer path.

## Remaining UNK

- **`bArmed = 1` writer** — static search found only zero writes to `byte [ESI+0x114]`; arm moment likely scheduler/anim completion (not yet traced to a named function).
- **`0x0041b390`** — manifest address is `CBulanci_RegisterPlayerAndRespawn`, not mine logic.
- **`pOwnerBulanek` Ghidra type** — struct field still `-BAD-` at `+0x108` (should be `CBulanek *`).
