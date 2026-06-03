# Round 6 — Logic task 04 report (CShot scheduler cluster)

## Task

| Field | Value |
|-------|-------|
| **id** | 4 |
| **title** | Logic cluster: CShot_SchedulerTick + scheduler/gaming neighbors |
| **range** | cluster_41ab (`0x0041a000`–`0x0041bfff`) |
| **seed_address** | `0x0041b1d0` |

## Status

**DONE** — Seed `CShot_SchedulerTick` control flow, slot-0 gate, and vtable dispatch path documented from Ghidra decompile + scheduler slot semantics (`main_menu_hover_audio.md`). Neighbor addresses catalogued; no `this` / MI fixes required (symbols and `CShot *` already applied). Ghidra MCP disconnected after initial `batch_decompile`; no program mutations.

## Functions

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x0041b1d0` | `CShot_SchedulerTick` | **Seed.** Primary `CDSView` vtable slot **30** (`g_pCShot_vftable_primary` @ `0x00481cbc`). On scheduler dispatch, reads **event slot 0** on embedded `CDSUpdatedItem` @ `this+0x88`; if heap slot byte at **`slot+0x08` bit 0 is clear**, calls `Scheduler_EnqueueEvent(param_1, 1, slot)` (manual enqueue toward shared `FUN_0041ad80` dispatch path). Does not call `CShot::Update` directly. | Vtable DATA xref `0x00481d34`; decompile; `combat_projectiles.md` slot 30 + `RegisterEventSlot(...,0,0x32,6)`; slot `+8` bit0 semantics in `main_menu_hover_audio.md` (`Scheduler_ArmSlot` / `Scheduler_AckSlot`); `mapping.csv` `__thiscall` `CShot*` |
| `0x0041a8c0` | `CDeath2_SubobjectCtor` | Factory subobject init: `CAnim_SubobjectCtor` then installs `CDeath2` vtables; zeros tail `+0xf4/+0xf8`. Same cluster band; not on `CShot` path. | `CDeath2.md`; decompile; `__thiscall` `CDeath2*` |
| `0x0041a920` | `CTeleportPoint_InitVtables` | `CAnim` subobject ctor + `CTeleportPoint` vtable install (`+0`, `+4`, `+0x10`, `+0x88`, `+0x98` dword indices). Map entity factory helper. | Decompile; `mapping.csv` `__fastcall` |
| `0x0041a970` | `CTeleportPoint_GetClassTable` | RTTI / class table pointer `&DAT_004b3838`. | Decompile (2 insn) |
| `0x0041a980` | `CShot::CDSChain_AdjustThisOffset_ThisMinus84` | **MI adjustor** on `CShot` `IDSUpdatedItem` facet (`0x00481c5c`): `ECX` inner pointer → `CDSChain_AdjustThisOffset((CDSChain *)(param_1 - 0x84))`. Offset **0x84** matches `updatedItem` @ `+0x88` minus `IDSChained` @ `+0x04`. | Decompile; `combat_projectiles.md` CDSUpdatedItem facet list; `CShot.md` `+0x88` |
| `0x0041a990` | `CMina_DefaultCtor` | Default-construct shell: SEH frame → `CAnim_SubobjectCtor` → `CDSUpdatedItem_ctor` at **`param_1 + 0x3c`** dwords (= byte `+0xF0`) → `CMina` vtables + `g_pCMina_vftable_IDSUpdated`. | `CMina.md`; decompile |
| `0x0041aa30` | `CMina_GetClassTable` | Class table `&DAT_004b384c`. | Decompile |
| `0x0041aa40` | `CDSChain_ReleaseChild` | Shared **IDSChained** vtable slot 2: `CDSObject_ReleaseViaVtable((int *)(param_1 - 4))`. Used by many `CDSView` derivatives. | `CDSView_vftable.md`; decompile |
| `0x0041aa50` | `CMina_ScalarDeletingDtor_thunk_Subf0` | Scalar-deleting dtor thunk; adjusts `this` by `-0xF0` to real `CMina*`. | Decompile; `__thiscall` `CMina*` |
| `0x0041aa60` | `CMina_ScalarDeletingDtor_thunk_Sub18` | Scalar-deleting dtor thunk via `updatedItem` facet (`-0x18` dword adjust in decompiler view). | Decompile; `__thiscall` `CMina*` |

## Seed control flow (`CShot_SchedulerTick`)

### Registration (ctor, outside slice)

`CShot_Ctor@0x0041edf0` embeds `CDSUpdatedItem` @ `+0x88` and registers slot **0** with delay **0x32** (50 ms) and event kind **6** (`Scheduler_RegisterEventSlot(&updatedItem, 0, 0x32, 6)` per `combat_projectiles.md` / `CShot.md`).

### Dispatch entry

| Step | Behavior | Proof |
|------|----------|-------|
| Caller | No direct `CALL` xrefs — only **vtable pointer** at `0x00481d34` (primary facet slot 30). Invoked when `Scheduler_DispatchDueEvents` runs the owning object’s periodic callback (`tick_system.md` / `CDSApp_PulseTasks` → `g_pTaskList`). | `get_xrefs_to@0x0041b1d0` → DATA `0x00481d34` |
| Slot lookup | `pvVar1 = Scheduler_GetEventSlot(&this->updatedItem, 0)` | Decompile @ `0x0041b1d0` |
| Gate | `if ((*(byte *)((int)pvVar1 + 8) & 1) == 0)` then `Scheduler_EnqueueEvent(param_1, 1, pvVar1)` | Decompile; same **`slot[+8]` bit 0** “armed” flag documented for `Scheduler_ArmSlot` / `Scheduler_AckSlot` in [main_menu_hover_audio.md](../../gameplay/main_menu_hover_audio.md) (Frida-verified on menu path) |
| `param_1` | Opaque scheduler / dispatch context passed from engine (`void*`); not the `CShot*` | Decompile signature; compare `CBulanek_SchedulerTick` / `CMina_OnSchedulerHook` same second parameter pattern in `mapping.csv` |

### Relation to `CShot::Update`

| Callback | Address | Facet | Role |
|----------|---------|-------|------|
| `CShot::Update` | `0x0041df60` | Primary vtable slot **11** | Per-frame movement / pellet collision |
| `CShot_SchedulerTick` | `0x0041b1d0` | Primary vtable slot **30** | Scheduler slot-0 side effect (enqueue when not armed) |
| `FUN_00419b40` | `0x00419b40` | `IDSUpdatedItem` facet | Track / anim advance (separate hook; R5 worker 12) |

Shared engine helper `FUN_0041ad80@0x0041ad80` (14 xrefs) is the scheduler **tick-dispatch node** allocator; `Scheduler_EnqueueEvent(..., 1, slot)` on the seed path targets that family (R5 worker 12 SKIP table).

## Ghidra deltas

**none** — `CShot_SchedulerTick` already `__thiscall` with `CShot *`; decompiler comment present; neighbors already renamed. No `set_function_this_type` / `save_program` (MCP unavailable for re-verify after initial decompile).

## Frida

**none** — Slot `+8` bit-0 semantics statically aligned with Frida-proven menu scheduler documentation (`scripts/frida/menu_hover_audio_trace.js` cited in `main_menu_hover_audio.md`). Seed body is three calls; no runtime-only branches identified.

## Remaining UNK

- Exact payload semantics of `Scheduler_EnqueueEvent(param_1, **1**, slot)` vs event kind **6** registered in `CShot_Ctor` (engine-wide scheduler; out of round-6 slice unless xref-driven).
- Whether slot **30** is ever invoked outside `Scheduler_DispatchDueEvents` (no extra xrefs found; vtable-only entry).

## Evidence paths touched

- [ROUND6_LOGIC_PROTOCOL.md](../ROUND6_LOGIC_PROTOCOL.md)
- [tick_system.md](../tick_system.md)
- [combat_projectiles.md](../../gameplay/combat_projectiles.md)
- [struct_recovery/CShot.md](../struct_recovery/CShot.md)
- [struct_recovery/CGaming.md](../struct_recovery/CGaming.md)
- [struct_recovery/CDSUpdatedItem.md](../struct_recovery/CDSUpdatedItem.md)
- [gameplay/main_menu_hover_audio.md](../../gameplay/main_menu_hover_audio.md) (scheduler slot `+8` bit 0)
