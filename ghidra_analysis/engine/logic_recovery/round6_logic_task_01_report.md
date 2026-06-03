# Round 6 — Logic Task 01 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 1 |
| **title** | Logic cluster: CBulanek_SchedulerTick + scheduler/gaming neighbors |
| **range** | `cluster_41ab` (`0x0041a000`–`0x0041bfff`) |
| **seed_address** | `0x0041aed0` |

## Status

**DONE** — Seed `CBulanek_SchedulerTick` control flow, three enqueue targets, vtable dispatch caller, and `CBulanek *` typing are proven (live Ghidra decompile + disasm + `bulanci.ghidra.exe.c` cross-check). No `set_function_this_type` / rename required on the seed. Ghidra MCP disconnected before optional neighbor fixes; no `save_program`.

## Functions

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x0041aed0` | `CBulanek_SchedulerTick` | **Seed.** Primary `CDSView` vtable slot **30** (`g_pCBulanek_vftable_primary` @ `0x00481e54` → entry `0x00481ecc`). Pre-match scheduler **enqueue poll**: for each unarmed heap slot (`byte[slot+8] & 1 == 0`), appends `{kind, target}` nodes into dispatch list `param_1` via `Scheduler_EnqueueEvent`. Targets: (1) `videoTrackManager` embed — **kind 0**; (2) `scheduler` @ `+0x88` slots **0..5** — **kind 1** per live slot; (3) `pWeapon->trackManager` — **kind 0** on slot 0. | Live decompile; disasm `MOV EBP,ECX`, `LEA EDI,[EBP+0x88]`, `LEA ECX,[EBP+0xac]` / `[EAX+0xc]`; vtable DATA xref `0x00481ecc`; export ~85486–85517 |
| `0x0041ad80` | `Scheduler_EnqueueEvent` | Shared helper: `Runtime_MallocOrThrow(8)` → `{kind, slotRef}` → `CIntListInsertSortedOrAppend(param_1,…)`. | 14 xrefs including seed; decompile @ `0x0041ad80` |
| `0x0041a0c0` | `CGame_DestroyObjectAtSlot` | Destroys entity at slot index when `param_1 != 0xff`: `CGaming_GetObjectAtSlotSafe` → unregister → vtable `+8` release. | Decompile; `CGame *` `__thiscall` |
| `0x0041a0f0` | `CGaming_TryGetPlayerCoords` | If slot occupied and entity is human player, copies `origin_x/y` @ `+0x20/+0x24` into `param_2`. | Decompile; callers in export ~85739, ~88147 |
| `0x0041a270` | `CGaming_ReleaseAndClearSlotRef` | If `*param_1` non-null: unregister, release vfn `+8`, zero pointer. | Decompile; `CGaming *` `__thiscall` |
| `0x0041a2a0` | `CGaming_CleanupInactiveBullets` | Walks `bulletSlotVec`; removes shots with flag @ shot `+0xa8`, unregisters + dtor. | Decompile; `CGaming *` `__fastcall` |
| `0x0041a450` | `CLevelScript_ScalarDeletingDtor` | Level-script object scalar-deleting dtor. | Named; out of CBulanek scheduler path |
| `0x0041a470` | `CLevelScriptOpExt_SetOrderAxis` | Script opcode helper (`__cdecl`). | Named stub |
| `0x0041a4a0` | `CLevelScriptOpExt_InsertView` | Script opcode helper. | Named stub |
| `0x0041a4d0` | `CLevelScriptOpExt_RemoveView` | Script opcode helper. | Named stub |
| `0x0041a510` | `CDSWav_GetClassMeta` | Wav class registry pointer return. | R5 w19 meta stub |

### Seed control flow (`CBulanek_SchedulerTick`)

```
param_1 = tick-dispatch list head (CDSPtrSlotVec / int-list built by CGaming_RunPreMatchModal)
ECX     = CBulanek*

// 1) Walk / weapon anim track manager (CDSVideoPlayer @ +0xA8, scheduler @ +0xAC)
slot0 = Scheduler_GetEventSlot(&videoTrackManager.scheduler, 0)
if (slot0 && !(slot0[+8] & 1))  Scheduler_EnqueueEvent(param_1, 0, &videoTrackManager)

// 2) Player CDSUpdatedItem @ +0x88 — ctor registers slots 0..5 (eventKind 7)
for i in 0..5:
  slot = Scheduler_GetEventSlot(&scheduler, i)
  if (slot && !(slot[+8] & 1))  Scheduler_EnqueueEvent(param_1, 1, slot)

// 3) Weapon track manager (CWeapon @ +0xF8, CDSVideoPlayer @ +0x08, scheduler @ +0x0C)
if (pWeapon) {
  slot0 = Scheduler_GetEventSlot(&pWeapon->trackManager.scheduler, 0)
  if (slot0 && !(slot0[+8] & 1))  Scheduler_EnqueueEvent(param_1, 0, &pWeapon->trackManager)
}
```

**Slot registration (ctor, outside slice):** `CBulanekCtor@0x0041e4b0` calls `Scheduler_RegisterEventSlot(&scheduler, …, eventKind=7)` for slot **0** (delay `0x96` or `0x1f4` by player count) and conditional slots **2–5** by `slotKind` / AI flags (`bulanci.ghidra.exe.c` ~88817–88840). Matches tick walk **0..5**.

**Armed gate:** `TEST byte [slot+8], 0x1` / decompile `(byte)(slot+8) & 1` — same **slot[+8] bit 0** semantics as `Scheduler_ArmSlot` / `Scheduler_AckSlot` (see `main_menu_hover_audio.md`, task 04/05 reports). Enqueue only when bit **clear** (not armed).

### Caller path (static)

| Step | Function | Evidence |
|------|----------|----------|
| 1 | `CGaming_RunPreMatchModal@0x0041c290` builds `local_138` dispatch list, snapshots `g_pApp+0x100` key latch | `bulanci.ghidra.exe.c` ~86724–86768 |
| 2 | `FUN_0041b620` seeds `CGaming.schedulerFacet` slots 0/1/2 | ~86770 |
| 3 | Entity tree walk: `CDSChained_GetFirstChildView` / `GetNextSiblingView` on `pEntityViewRoot` | ~86771–86774 |
| 4 | **`CALL [entity_vtable+0x78](&local_138)`** → `CBulanek_SchedulerTick` for `CBulanek` nodes | **Only** `+0x78` indirect call with list arg in export (~86773) |
| 5 | Loop continues while `*(entity+0x4C) != 0` | Decompiler `piVar3[0x13]` — field name UNK |
| 6 | `CGaming_RegisterPreMatchSchedulerHooks`, `FUN_0041bab0`, drain list (kind 0/1/2) | ~86776–86805 |

**Note:** Runtime per-frame firing uses `CDSUpdatedItem` / `Scheduler_DispatchDueEvents` on registered slots (`tick_system.md`), not a direct `CALL` to `0x0041aed0`. Seed is the **pre-modal enqueue hook** on CDSView vfn **30** (same slot index as `CShot_SchedulerTick`, `CMina_OnSchedulerHook`).

### This typing

| Check | Result |
|-------|--------|
| Disasm entry | `MOV EBP,ECX` @ `0x0041aed2` |
| Ghidra signature | `void __thiscall CBulanek_SchedulerTick(CBulanek *this, void *param_1)` |
| Decompiler fields | `this->scheduler`, `this->videoTrackManager`, `this->pWeapon->trackManager` |

**No `set_function_this_type` change required.**

## Ghidra deltas

**none** (this session).

Prior rounds already applied: symbol `CBulanek_SchedulerTick`, `CBulanek *` `__thiscall`, decompiler comment on body. Live MCP read confirmed typing before disconnect.

## Frida

**none** — enqueue triple-path and vtable `+0x78` caller chain proven statically (export + disasm). Runtime hook would only duplicate list-building in `CGaming_RunPreMatchModal`.

## Remaining UNK

- **`*(entity+0x4C)`** in pre-match entity walk — controls sibling iteration; not mapped to a named `CBulanek` field in this task.
- **`CGaming_TryGetPlayerCoords@0x0041a0f0`** — Ghidra types `void *this` while calling `CGaming_*` slot helpers; likely `CGaming *` or `CGame *` — needs disasm `MOV ECX` proof before `set_function_this_type`.
- **`FUN_0041b620` / `FUN_0041bab0`** — pre-match scheduler seed helpers; rename/`CGaming *` fixes deferred to task 8 cluster (R5 w12 notes).
- Per-slot **post-arm callbacks** on `CBulanek.scheduler` (IDSUpdated vtable entries) — not re-derived here; see `CDSUpdatedItem.md` host catalog.
