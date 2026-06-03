# Round 6 Logic — Task 05 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 5 |
| **title** | Logic cluster: CMina_OnSchedulerHook + scheduler/gaming neighbors |
| **range** | `cluster_41ab` (`0x0041a000`–`0x0041bfff`) |
| **seed_address** | `0x0041b210` |

## Status

**DONE** — seed `CMina_OnSchedulerHook` control flow, caller path, and dual enqueue semantics proven statically (live Ghidra decompile + disasm + export cross-check). Neighbor symbols already correctly named/typed in Ghidra; no `this`/MI reparent fixes required on this slice.

## Functions

| Address | Name | Role summary | Evidence |
|---------|------|--------------|----------|
| `0x0041b210` | `CMina_OnSchedulerHook` | **Seed.** Vtable slot **30** on `g_pCMina_vftable_primary` (`0x0048257c` → entry `0x004825f4`). Two-phase scheduler hook: (1) delegate to `CAnim::FUN_0041b190` on embedded `animBase` (track-manager slot 0, enqueue **kind 0**); (2) read `updatedItem` scheduler slot 0; if event byte `+0x8` bit 0 clear, enqueue **kind 1** via `FUN_0041ad80`. | Live decompile; disasm: `MOV ESI,ECX` → `CALL 0x0041b190` → `ADD ESI,0xf0` → `CALL Scheduler_GetEventSlot` → `TEST [EAX+0x8],0x1` → `CALL 0x0041ad80`; xref **to** `0x004825f4` [DATA] only (virtual dispatch). Export `bulanci.ghidra.exe.c` matches. |
| `0x0041aa70` | `CMina_ScalarDeletingDtor_thunk_Sub98` | MSVC scalar-deleting dtor thunk for MI face at `animBase+0x98` (`vftable_anim_sub`); adjusts `this` by **−0x98** then calls `CMina_ScalarDeletingDtor`. | Vtable `g_pCMina_vftable_IDSAnim` slot 3 @ `0x00482520`; decompile shows `&this[-1].animBase…` adjust pattern. |
| `0x0041aa80` | `CMina_IdentityCast_Subec` | MI identity/`dynamic_cast` adjustor: `param_1 + (-0xec)` → `CDSChain_AdjustThisOffset`. Face at `CMina+0xec` inside `animBase` tail. | Decompile `param_1 + -0xec`; vtable cluster neighbor. |
| `0x0041aa90` | `CMina_AdjustOffsetDtor` | Scalar-deleting dtor thunk via `bArmed` field anchor (`+0x114`); forwards to `CMina_ScalarDeletingDtor` with reparented `CMina*`. | Vtable `g_pCMina_vftable_IDSChained` slot 3 @ `0x00482560`; decompile. |
| `0x0041aaa0` | `CMina_ReleaseViaVtable_Subf0` | Release/refcount helper on embedded `CDSUpdatedItem` (`+0xf0`); adjust **−0xf0** → `CDSObject_ReleaseViaVtable`. | Decompile `param_1 + -0xf0`. |
| `0x0041aab0` | `CMina_ScalarDeletingDtor_thunk_Sub88` | Scalar-deleting dtor thunk for face at `animBase+0x88` (`IDSUpdated` anim facet); adjust **−0x88**. | Vtable `0x00482520` slot 3 maps here; decompile. |
| `0x0041aac0` | `CMina_ScalarDeletingDtor_thunk_Sub10` | Scalar-deleting dtor thunk for `IDSReferenced` face at `+0x10`; adjust **−0x10**. | Vtable `0x00482548` slot 3; decompile. |
| `0x0041aad0` | `CExplosion_InitVtables` | **Neighbor (CExplosion).** Runs `CAnim_SubobjectCtor`, installs `g_pCExplosion_*` vtable cluster on a `0xf4`-byte explosion view (primary, IDSChained, IDSEventHandler, IDSAnim faces). | Decompile writes seven vtable pointers; caller `CExplosion_CreateObject` @ `0x0041ce30` region (export). |
| `0x0041ab20` | `CExplosion_GetClassTable` | Returns static class metadata pointer `&DAT_004b386c`. | Decompile `return &DAT_004b386c`. |
| `0x0041ab30` | `CAnim_ScalarDeletingDtor_thunk_Sub10` | Shared **CAnim** scalar-deleting dtor thunk (IDSReferenced at `+0x10`); used by multiple entity views in this cluster. | Decompile → `CAnim_ScalarDeletingDtor` with `track_manager` anchor adjust. |

### Seed control flow (`CMina_OnSchedulerHook`)

```
param_1 = tick-dispatch int-list head (same pointer passed through scheduler walk)
ECX     = CMina*

CAnim::FUN_0041b190(&animBase, param_1)
    └─ if track_manager.scheduler slot0 not armed (byte+8 bit0==0):
         FUN_0041ad80(param_1, kind=0, &animBase.track_manager)

if updatedItem.scheduler slot0 not armed:
    FUN_0041ad80(param_1, kind=1, slot0_ptr)
```

**Caller path:** no direct `CALL` xrefs — invoked only through **`g_pCMina_vftable_primary[30]`** during scheduler pre-tick walks (same pattern as `CShot_SchedulerTick` @ `0x0041b1d0`, which only enqueues `updatedItem` kind **1**). Parent walk examples: `CGaming_RunPreMatchModal` scheduler seeding, `CBulanek_SchedulerTick` (`bulanci.ghidra.exe.c` ~85498–85516).

**`FUN_0041ad80` semantics (static, shared helper):** allocates 8-byte `{kind, slotRef}` node, appends into sorted int-list at `param_1`. Dispatch loop (`CGaming` modal path ~86783–86805): kind **0** → arm track-manager scheduler; kind **1** → `Scheduler_ArmSlot` on embedded `CDSUpdatedItem` scheduler; kind **2** → audio stop.

**Contrast vs `CShot_SchedulerTick`:** CMina override calls base `CAnim` hook first (landing animation / track manager) **then** arms mine `updatedItem` slot — consistent with mine ctor binding `TM_SetTrack` on deploy (`CMina_Ctor` / `InitMine` @ `0x0041cb70` / `0x0041cce0`).

## Ghidra deltas

**none** (this session).

Prior rounds already applied: `CMina_OnSchedulerHook` name, `CMina*` `__thiscall` typing, and MI thunk names on `0x0041aa70`–`0x0041aac0`. Live Ghidra `batch_decompile` (2026-06-03) confirmed decompiler uses `CMina` / `CAnim` / `CExplosion` field names. Ghidra MCP disconnected before any new `set_decompiler_comment` / `save_program`.

**Out of slice (not mutated):** `CAnim::FUN_0041b190` @ `0x0041b190`, `_Globals::FUN_0041ad80` @ `0x0041ad80` — deferred rename to task 6 / scheduler worker per R5 notes.

## Frida

**none** — dual enqueue and slot-flag gate proven by disasm + decompile; runtime would only duplicate static facts.

## Remaining UNK

- **`CAnim::FUN_0041b190`** — behavior proven (track-manager slot-0 enqueue) but symbol still `FUN_*`; rename blocked to owning task.
- **`FUN_0041ad80`** — body proven (`Runtime_MallocOrThrow(8)` + sorted list insert); canonical rename `Scheduler_EnqueueEvent` (or similar) not applied here.
- **`updatedItem` slot-0 callback** after arm — CMina reuses shared CDSUpdatedItem vtable entries (`0x0041a980`, `0x00418ed0` per `vftable_methods.csv`); timer semantics not re-derived in this task.
- **`CMina_IdentityCast_Subec`** — still `__fastcall` + `int param_1` in decompiler (MI adjustor stub); no layout impact.
