# Round 10 — Deep Task 02 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 2 |
| **title** | R6 rerun: CWeapon_OnSchedulerEvent + neighbors |
| **kind** | logic_rerun |
| **prior_round** | 6 / task 3 |
| **seed_address** | `0x0041b180` |
| **addresses** | `0x0041b180`, `0x0041a610`–`0x0041a640`, `0x0041a6a0`, `0x0041a710`, `0x0041a780`, `0x0041a7f0`, `0x0041a860` |
| **acceptance** | Trace weapon scheduler event dispatch; verify `this` typing; document fire/cooldown path with xref proof |

## Status

**DONE** — Live Ghidra MCP + IDA correlation. **`CWeapon_OnSchedulerEvent` is not `CWeapon*`** — disasm, IDA, and vtable bytes prove **`CBulanek+0xA0` IDSEventHandler MI facet** adjusting to `CBulanek*` before `CBulanek_StepMovementAndCollision`. Scheduler **tick enqueue** (`CBulanek_SchedulerTick` → `Scheduler_EnqueueEvent` → `TM_AdvanceFrame`) is a **separate path** from the seed thunk. **Fire/cooldown** uses `CWeapon::Fire` @ `0x004212b0` (IDSChained vfn[4], FLX opcode `0x0C`). R6 PARTIAL Ghidra mutations applied.

## Functions / Struct

| Address | Symbol (live Ghidra) | Role | Evidence |
|---------|----------------------|------|----------|
| **`0x0041b180`** | **`CWeapon_OnSchedulerEvent`** | **IDSEventHandler slot 0** on **`CBulanek+0xA0`** (`g_pCBulanek_vftable_event` @ `0x00481ddc`). MI adjustor `ECX -= 0xA0` → `CBulanek*`; delegates to movement step. **Not `CWeapon*`.** | **Disasm:** `ADD ECX,0xffffff60; CALL 0x0041af70; RET 0xC`. **IDA** `sub_41B180`: `return sub_41AF70(this - 40)` (`bulanci.ida.exe.c:93038–93040`; `this-40` ×4 = `-0xA0`). **Xref:** DATA only @ `0x00481ddc` (`read_memory` → `80 b1 41 00`). **Ctor:** `MOV [ESI+0xa0],0x481ddc` per `CBulanek.md` |
| `0x0041af70` | `CBulanek_StepMovementAndCollision` | Weapon-aim walk step: `pWeapon->dwParamA/B`, `pTrackHolder` anchor → collision clamp → vtable `+0x20`/`+0x24` | **Xrefs (3):** seed @ `0x0041b186`, `CBulanek_ApplyPickupEffect@0x0041eb28`, `CBulanek_SyncWeaponWalkFromTrack@0x0041bf61`. R10 task 01 verified |
| `0x0041bf00` | `CBulanek_SyncWeaponWalkFromTrack` | **`CBulanek*` ECX** — caches weapon track params; on change + `bField_168==0` → movement step | **Disasm:** `MOV EAX,[ECX+0xf8]` (pWeapon); cmp `+0x154/+0x158/+0x164` vs weapon `+0x5c/+0x60/+0x50`; `JMP 0x0041af70`. **IDA** `sub_41BF00(this)` with `this+62` = weapon @ `+0xF8` |
| `0x0041bf70` | `CWeapon::CWeapon_HideAssociatedView` | CWeapon IDSChained slot 0 — calls sync with **`pTrackHolder`** (`CWeapon+0x50`) as `CBulanek*` | **IDA** `sub_41BF70`: `sub_41BF00(*(this+20))` @ `93741–93743` |
| `0x0041aed0` | `CBulanek::CBulanek_SchedulerTick` | Per-frame enqueue: player scheduler @ `+0x88`, walk slots `0..5`, **weapon** `pWeapon->trackManager.scheduler` | **Decompile** + **IDA** `sub_41AED0`: `sub_41AD80(a2,0,*(this+62)+8)` for weapon TM |
| `0x0041ad80` | `Scheduler_EnqueueEvent` | Alloc 8-byte `{kind, slotRef}` → sorted tick list | **Xrefs:** `CBulanek_SchedulerTick`, `CShot`, `CMina`, `CGaming_OnSchedulerTimer` |
| `0x0042eb30` | `Scheduler_DispatchDueEvents` | Fires due slots; calls host `pVftable_IDSUpdated+0x10(slotIdx)` | Weapon TM host: `0x00487770` slot 4 → `CDSVideoPlayer_AdvanceFrameAndPauseIfDone@0x00439b90` → `TM_AdvanceFrame` |
| `0x004212b0` | `CWeapon::CWeapon_Fire` | **Fire/cooldown dispatch** — FLX `0x0C` subscriber; mine `kind==2`, shot `kind==4`, pistol `kind==5`, empty `0xFFFF` ammo/bot branch | **Vtable** `g_pCWeapon_vftable` @ `0x00481ed4` slot 4; reads `pTrackHolder`, `dwParamB` weapon kind |
| `0x0041a610` | `CDSChain_AdjustThisOffset_ThisMinus30` | CDSWavStream MI adjustor `-0x30` | R5 w19; unchanged |
| `0x0041a620`–`0x0041a630` | CDSWavStream dtor thunks | MI adjustors → scalar dtor | R4/R5; unchanged |
| `0x0041a640` | `CDSWavStream_dtor` | Stream non-deleting dtor | R4 task 50 |
| `0x0041a6a0` | `CreateObject` (class `0x7ec`) | `OperatorNew(0x19c)` → `CBulanek_ShellCtor` | Export static-init |
| `0x0041a710` | `CreateObject` (class `0x7eb`) | `OperatorNew(0x70)` → `ODSImage_FactoryCtor` | Export |
| `0x0041a780` | `CreateObject` (class `0x800`) | `OperatorNew(0x98)` → `CBitmap_ViewHeader_Init` | Export |
| `0x0041a7f0` | `CreateObject` (class `0x7ef`) | `OperatorNew(0xf0)` → `CAnim_SubobjectCtor` | Export |
| `0x0041a860` | `CDeath_SubobjectCtor` | CAnim prefix + zeros `+0x100/+0x104` | [CDeath.md](../struct_recovery/CDeath.md) |

### `this` proof — seed is **not** `CWeapon*`

| Check | Result | Proof |
|-------|--------|-------|
| Entry ECX object | **`CBulanek + 0xA0`** event facet | Disasm subtracts `0xA0` before callee; IDA `this - 40` (dwords) |
| Vtable owner | **`CBulanek::g_pCBulanek_vftable_event`** @ `0x00481ddc` | `read_memory@0x00481ddc` slot0 = `0x0041b180`; IDA `CBulanek::vftable = &sub_41B180` @ line 3044 |
| `CWeapon` has separate event vtable | **`0x00481eec`** slot0 = `0x00418f60` (`CWeapon_GetClassTable`) | `master_vtable_catalog.csv`; not the seed |
| Callee `this` | **`CBulanek*`** | `MOV ESI,ECX` @ `0x0041af76`; accesses `+0xF8` pWeapon, `+0x104` embed |

### Scheduler dispatch (weapon band)

```
Frame tick (CGaming / entity scheduler hooks)
  CBulanek_SchedulerTick@0x0041aed0(CBulanek*, tickList*)
    ├─ videoTrackManager.scheduler slot0 → Scheduler_EnqueueEvent(list, 0, &videoTrackManager)
    ├─ player scheduler@+0x88 slots 0..5 → Scheduler_EnqueueEvent(list, 1, slotPtr)
    └─ pWeapon->trackManager.scheduler slot0 → Scheduler_EnqueueEvent(list, 0, &trackManager)
         ↓ (tick list processed elsewhere)
  Scheduler_DispatchDueEvents@trackManager.scheduler+4
    └─ [IDSUpdated+0x10] → CDSVideoPlayer_AdvanceFrameAndPauseIfDone@0x39b90
         └─ TM_AdvanceFrame → FLX frame decode / delay arm

Parallel: IDSEventHandler facet (seed — indirect runtime only)
  vtbl@0x481ddc slot0 → CWeapon_OnSchedulerEvent@0x0041b180
    ECX = CBulanek+0xA0; ECX -= 0xA0
    CBulanek_StepMovementAndCollision(CBulanek*)   // weapon aim delta / collision
```

### Fire / cooldown path (separate from seed)

```
FLX track FrameTimeHint (opcode 0x0C) on weapon IDSChained face
  CWeapon::CWeapon_Fire@0x004212b0(CWeapon*, ..., u16 trigger_state)
    kind 2 + param 0 → CMina spawn + DecrementWeaponAmmo
    kind 4 + param != 0xFFFF → CShot spawn + SetAmmo cooldown
    kind 5 + param 0 → pistol CShot + SetAmmo(7)
    param 0xFFFF → empty-ammo UI / bot TryBotRandomAction / shotgun pickup guard

Param sync without scheduler event:
  CWeapon IDSChained[0] @0x41bf70 OR direct call
    CBulanek_SyncWeaponWalkFromTrack(CBulanek*) @0x41bf00
      if pWeapon dwParamA/B/pTrackHolder changed → CBulanek_StepMovementAndCollision
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x0041b180` | `uchar __fastcall CWeapon_OnSchedulerEvent(int this_cbulanek_plus_A0, uint, uint, uint)` — preserves MI adjustor decomp (`param + -0xa0`); `__thiscall`+`CBulanek*` **rejected** (breaks to `&this[-1].pCorpseAnim`) |
| `set_decompiler_comment` | `0x0041b180` | NOT `CWeapon*`; vtbl `0x481ddc`; IDA cross-ref |
| `rename_function_by_address` | `0x0041bf00` → `CBulanek_SyncWeaponWalkFromTrack` | Was misleading `Update` under `CWeapon::` namespace |
| `set_function_prototype` | `0x0041bf00` | `void __fastcall CBulanek_SyncWeaponWalkFromTrack(CBulanek *param_1)` |
| `set_decompiler_comment` | `0x0041bf00` | ECX=`CBulanek*`; field offsets; caller `0x41bf70` |
| `force_decompile` | `0x0041b180`, `0x0041bf00` | Refreshed |
| `save_program` | `bulanci.exe` | Saved once |

Neighbors `0x0041a610`–`0x0041a640`, factories `0x0041a6a0`–`0x0041a7f0`, `CDeath_SubobjectCtor`: **no changes** (R4/R5 already typed).

## Decomp corrections (IDA vs Ghidra)

| Issue | Stale export / pre-fix | IDA (`bulanci.ida.exe.c`) | Post-R10 live Ghidra |
|-------|------------------------|---------------------------|----------------------|
| Seed `this` type | `CWeapon*` implied by symbol | `int *this` at **facet** `+0xA0`; `this-40` adjust | `int this_cbulanek_plus_A0`; cast `(void*)(param - 0xa0)` — **not** `CWeapon*` |
| Seed convention | `undefined __fastcall` (1 param) | `char __thiscall` + 3 stack args, `RET 0xC` | `uchar __fastcall` ECX + 3 stack params |
| Seed return | `void` / missing | `char` (=1 via callee) | `uchar` from movement step |
| `0x0041bf00` name/`this` | `CWeapon::Update(CBulanek*)` fastcall | `sub_41BF00(int *this)` = `CBulanek*` | `CBulanek_SyncWeaponWalkFromTrack(CBulanek *param_1)` |
| Movement callee namespace | `_Globals::` prefix | `sub_41AF70(int *this)` | `_Globals::` artifact; body uses `CBulanek` fields (task 01) |
| `__thiscall` on MI adjustor | — | N/A | **Do not** set `CBulanek*` this on `0x0041b180` — Ghidra maps to wrong struct offset |

`bulanci.ghidra.exe.c` on disk is **stale** (no `0x0041b180` symbols); use live MCP decompile.

## Frida

**none** — MI adjustor, vtable bytes, disasm field offsets, and IDA tripartite caller closure are static-closed.

## Remaining UNK

- **Runtime poster** to `g_pCBulanek_vftable_event` slot 0 — no code xref to `0x0041b180` besides vtable DATA (indirect virtual only).
- **`CWeapon_OnSchedulerEvent` stack params** (`param_1..3`) — unused in thunk body; event-kind semantics UNK until dispatcher closure.
- Primary vtable **`+0x20` / `+0x24`** on `CBulanek` (`0x00481e54`) — indirect inside movement step only.
- Ghidra **`CWeapon::` namespace** prefix on `CBulanek_SyncWeaponWalkFromTrack` — cosmetic; offsets in body correct.

## Struct doc updates

**none** — [CWeapon.md](../struct_recovery/CWeapon.md) and [CBulanek.md](../struct_recovery/CBulanek.md) already document `pWeapon@+0xF8`, `trackManager.scheduler@+0x0C`, and `vftable_event@+0xA0`.
