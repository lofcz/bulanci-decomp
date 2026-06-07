# Round 10 — Deep Task 01 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 1 |
| **title** | R6 rerun: FUN_0041af70 scheduler/gaming cluster |
| **kind** | logic_rerun |
| **prior_round** | 6 / task 2 |
| **seed_address** | `0x0041af70` |
| **addresses** | `0x0041af70`, `0x0041a520`–`0x0041a600` |

## Status

**DONE** — Live Ghidra MCP verification confirms R6 PARTIAL conclusions. Seed renamed `CBulanek_StepMovementAndCollision`; prototype corrected to `uchar __thiscall` (IDA match). Three code xrefs proven. Scheduler tick role via `CWeapon_OnSchedulerEvent` MI thunk and in-function `Scheduler_GetEventSlot`. Neighbor audio vtable thunks unchanged from R5.

## Functions / Struct

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x0041af70` | `CBulanek_StepMovementAndCollision` | **CBulanek movement/collision step** — weapon track anchor delta → `CDSRect_IntersectInPlace` → update walk embed (`+0x104`..`+0x118`, zero `+0x15C`..`+0x167`) → `CBulanek_CopyCollisionRectLocal` ×2 → `CBulanek_OffsetCollisionRectByFacing` ×2 → `Scheduler_GetEventSlot` bit0 → `CBulanek_ClampMoveRectByCollision` → vtable `+0x20` commit / `+0x24` fallback; returns `1` | **Disasm:** `MOV ESI,ECX` @ `0x0041af76`; callees `0x00433200`, `0x00417410`, `0x00417460`, `0x0042f1e0`, `0x004195f0`; `MOV AL,0x1` @ `0x0041b16c`. **IDA:** `char __thiscall sub_41AF70(int *this)` @ `bulanci.ida.exe.c:92943` — identical control flow and offsets (`this+62`→`+0xF8` weapon, `this+43`→`+0xAC` scheduler, vtable `+32`/`+36`→`+0x20`/`+0x24`). **Xrefs (3):** table below |
| `0x0041a520` | `CDSWav_GetTypeInfo` | `IDSReferenced` meta stub — `MOV EAX,0x4b8410; RET` | **Disasm:** `0041a520: MOV EAX,0x4b8410; RET` |
| `0x0041a530` | `CDSWav_ScalarDeletingDtor_thunk` | MI adjustor `ECX-0x18` → `CDSWav_ScalarDeletingDtor@0x0041bbe0` | **Disasm:** `SUB ECX,0x18; JMP 0x0041bbe0` |
| `0x0041a540` | `CDSWav_ScalarDeletingDtor_thunk_Sub4` | MI adjustor `ECX-4` → same scalar dtor | R5 w19 naming; no re-verify needed |
| `0x0041a550` | `CDSObject::CDSObject_dtor` | Generic chained subobject destructor (audio band, not gaming) | **Xrefs (3):** `CDSWavStream_dtor@0x41a684`, `CDSWav_ScalarDeletingDtor@0x41bbe3`, unwind stub |
| `0x0041a5c0` | `CDSWavStream_GetTypeInfo` | Class-43 factory typeinfo stub — `MOV EAX,0x4b843c; RET` | R5 w19 naming |
| `0x0041a5d0` | `CDSWavStream_ScalarDeletingDtor_thunk_Sub4` | MI adjustor `ECX-4` → `CDSWavStream_ScalarDeletingDtor@0x0041bc00` | R5 w19 naming |
| `0x0041a5e0` | `CDSWav_ReleaseChild_thunk_Sub30` | MI adjustor `ECX-0x30` → `CDSWav_ReleaseRefcount@0x00433040` | **Disasm:** `SUB ECX,0x30; JMP 0x00433040` |
| `0x0041a5f0` | `CDSWav_ReleaseChild_thunk_Sub34` | MI adjustor `ECX-0x34` → same release | R5 w19 naming |
| `0x0041a600` | `CDSWavStream_ScalarDeletingDtor_thunk_Sub30` | MI adjustor `ECX-0x30` → `CDSWavStream_ScalarDeletingDtor@0x0041bc00` | R5 w19 naming |

### Seed callers (`get_xrefs_to@0x0041af70`, live Ghidra)

| Caller | Site | Context |
|--------|------|---------|
| `CBulanek_ApplyPickupEffect` | `0x0041eb28` | After weapon ctor on pickup — `CALL` with `ECX = CBulanek*` (IDA `sub_41AF70(this)` @ ~95745) |
| `CWeapon_OnSchedulerEvent` | `0x0041b186` | Scheduler event on **CWeapon+0xA0** facet: `CBulanek_StepMovementAndCollision((CBulanek *)(param_1 - 0xa0))` — IDA `sub_41B180` returns `sub_41AF70(this - 40)` @ `93040` |
| `CWeapon::Update` | `0x0041bf61` | When walk track params change and `bField_168 == 0`: `CALL` with `ECX = CBulanek*` (IDA `sub_41AF70(this)` @ ~93729) |

### Scheduler tick role (proven)

1. **Indirect tick path:** `CWeapon_OnSchedulerEvent@0x0041b180` is the `IDSEventHandler` slot on the weapon subobject embedded at `CBulanek+0xA0`. Scheduler dispatch reaches movement step via MI adjustor `-0xA0` → `CBulanek*`.
2. **Direct scheduler read:** Inside the seed, `Scheduler_GetEventSlot(&(videoTrackManager).scheduler, 0)` @ `0x0041b0f3` (`sub_42F1E0` in IDA). If slot byte `+8` bit0 set, updates `dwFacingAxisExtent@+0xA4` from proposed rect axis (`nInitialTrack < 2` → top else left).
3. **Track index sync:** `nInitialTrack@+0x148` ← `videoTrackManager.nCurrentTrackIdx@+0xD4` before facing offset pass.

### Seed control flow (`0x0041af70`)

```
CBulanek* this (ECX, __thiscall)
  ├─ Read CWeapon* (+0xF8) → pTrackHolder anchor minus embed (+0x104/+0x108)
  ├─ Add CBulAnim* (+0xF0) chain fields → proposed top-left + delta
  ├─ CDSRect_IntersectInPlace(&origin, &delta) → store -origin in +0x114/+0x118
  ├─ Fold origin (+0x20/+0x24), collision ints (+0x10C/+0x110, +0x15C)
  ├─ CBulanek_CopyCollisionRectLocal ×2
  ├─ CBulanek_OffsetCollisionRectByFacing (origin rect, nInitialTrack)
  ├─ nInitialTrack = nCurrentTrackIdx (+0xD4)
  ├─ Scheduler_GetEventSlot(scheduler@+0xAC, 0): bit0 → dwFacingAxisExtent@+0xA4
  ├─ CBulanek_OffsetCollisionRectByFacing (proposed rect)
  ├─ CBulanek_ClampMoveRectByCollision
  ├─ vftable_primary+0x20(&proposed) — commit if true
  └─ else vftable_primary+0x24(0,0)
  return 1 (AL)
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| (pre-existing) `rename_function_by_address` | `0x0041af70` → `CBulanek_StepMovementAndCollision` | Already applied before R10 session |
| `set_function_prototype` | `uchar CBulanek_StepMovementAndCollision(void)`, `__thiscall` | Success — fixes R6 `undefined4 __fastcall` + `_Globals::` free-function typing |
| `set_decompiler_comment` | `0x0041af70` | Replaced UNCERTAIN block with verified caller/callee/offset summary |
| `force_decompile` | `0x0041af70` | Refreshed decomp |
| `save_program` | `bulanci.exe` | Saved once |

Neighbors `0x0041a520`–`0x0041a600`: **no changes** (R5 worker 19 plates intact).

## Decomp corrections (IDA vs Ghidra)

| Issue | Stale Ghidra export / pre-fix live | IDA ground truth | Post-R10 live Ghidra |
|-------|-------------------------------------|------------------|----------------------|
| Calling convention | `undefined4 __fastcall …(CBulanek *param_1)` | `char __thiscall sub_41AF70(int *this)` | `uchar __thiscall …(void *this)` |
| Return type | `undefined4` / `CONCAT31(…,1)` | `return 1` (`char`) | `return '\x01'` |
| Namespace | `_Globals::CBulanek_StepMovementAndCollision` | member of CBulanek object | `_Globals::` prefix persists (decompiler artifact); field accesses use `this+offset` matching CBulanek layout |
| `this` type | `CBulanek *param_1` (fastcall param) | `int *this` (ECX) | `void *this` — Ghidra API warns `__thiscall` ECX cannot be retyped to `CBulanek *` |
| Callee `0x00433200` | `tagRECT::CDSRect_IntersectInPlace` | `sub_433200` | Named correctly in live Ghidra |
| Vtable dispatch | `+0x20` / `+0x24` indirect | `(*this+32)` / `(*this+36)` | Match |

`bulanci.ghidra.exe.c` export is **stale** (still shows pre-fix `__fastcall` / `_Globals::`); use live MCP decompile.

## Frida

**none** — static xref + disasm + IDA correlation sufficient.

## Remaining UNK

- Primary vtable slots **`+0x20` / `+0x24`** on `CBulanek` (`0x00481e54`): indirect calls only; method names not resolved.
- Ghidra decompiler still prefixes `_Globals::` and types `this` as `void *` despite `__thiscall` — documented limitation; offsets/field names in body are correct.
- Caller decomps (`CWeapon_OnSchedulerEvent`, `CWeapon::Update`) still reference `_Globals::CBulanek_StepMovementAndCollision` until those functions are re-decompiled after seed fix.
- Field aliases `pReserved_preAmmo` / `pPad_postWalkEmbed` vs struct doc `abReserved_preAmmo` @ `+0x104` / `abPad_postWalkEmbed` @ `+0x15C` — offsets proven; naming consolidation deferred to struct batch.
