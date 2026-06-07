# Round 11 — AI Task 01 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 1 |
| **title** | CBulanek_SchedulerTick — pre-tick enqueue algorithm |
| **archetype** | `core_scheduler` |
| **seed_address** | `0x0041aed0` |
| **related** | `Scheduler_EnqueueEvent@0x0041ad80` |

## Status

**DONE** — Full pre-tick enqueue algorithm for slots **0..5**, kind **0/1** targets, vtable slot **30**, and live Ghidra xrefs proven (MCP decompile + IDA `sub_41AED0` / `sub_41AD80` correlate). No Ghidra mutations this session.

## AI archetype

`core_scheduler` — shared pre-match / modal scheduler poll hook used by every `CBulanek` entity (humans, co-op vampires `0x20..0x23`, practice dummies `0x24..0x27`, campaign AI `0x7d..0x7f`). Not the per-frame `Scheduler_DispatchDueEvents` path; this function only **builds** the dispatch list consumed by `CGaming_RunPreMatchModal`.

## Algorithm

### Signature & calling convention

| Source | Signature |
|--------|-----------|
| Ghidra (live) | `void __thiscall CBulanek_SchedulerTick(CBulanek *this, void *param_1)` |
| IDA | `int __thiscall sub_41AED0(_DWORD *this, int *a2)` |

- `param_1` — head of a `CDSPtrSlotVec` / int-list (`local_138` in `CGaming_RunPreMatchModal`). Nodes are 8-byte `{kind, target}` pairs appended by `Scheduler_EnqueueEvent`.
- `ECX` = `CBulanek*` (`__thiscall`).

### Vtable dispatch (slot 30)

| Item | Address | Evidence |
|------|---------|----------|
| Primary vtable | `g_pCBulanek_vftable_primary` @ `0x00481e54` | Ghidra symbol |
| Slot index | **30** → byte offset **`+0x78`** | CDSView vfn table convention |
| Function pointer cell | `0x00481ecc` | Live xref: `get_function_xrefs(0x0041aed0)` → **From `0x00481ecc` [DATA]** |
| Runtime caller | `CGaming_RunPreMatchModal@0x0041c290` | `(**(code **)(*piVar3 + 0x78))(&local_138)` on entity-tree siblings (`bulanci.ghidra.exe.c` ~86975–86978) |

### Armed gate (all three paths)

Before enqueue, each candidate checks the heap slot node at **`slotNode[+8] & 1 == 0`** (bit 0 clear = **not armed**). Same semantics as `Scheduler_ArmSlot` / `Scheduler_AckSlot` elsewhere (`tick_system.md`, R6 task 01).

IDA: `(*(_BYTE *)(sub_42F1E0(...) + 8) & 1) == 0`  
Ghidra: `(*(byte *)((int)pvVar1 + 8) & 1) == 0`

### Phase 1 — `videoTrackManager` (kind **0**)

```
slot0 = Scheduler_GetEventSlot(&this->videoTrackManager.scheduler, 0)
if (slot0 && !(slot0[+8] & 1))
    Scheduler_EnqueueEvent(param_1, 0, &this->videoTrackManager)
```

| Field | Offset | Role |
|-------|--------|------|
| `videoTrackManager` | `CBulanek+0xA8` (`this+42` DWORD index in IDA) | `CDSVideoPlayer` embed (walk/anim tracks) |
| Inner scheduler | `videoTrackManager+0x04` (`this+43` in IDA `Scheduler_GetEventSlot`) | Polls **slot 0** only |
| Enqueue **target** (`param_3`) | `&this->videoTrackManager` | Whole `CDSVideoPlayer*` |
| Enqueue **kind** (`param_2`) | **`0`** | Drain → `CGaming_ArmTrackMgrSchedulerIfUnpaused(trackMgr)` |

Live Ghidra call site: `0x0041aef5` (`Scheduler_EnqueueEvent` xref from seed).

### Phase 2 — player `scheduler` slots **0..5** (kind **1**)

```
for i in 0..5:
    slot = Scheduler_GetEventSlot(&this->scheduler, i)
    if (slot != NULL && !(slot[+8] & 1))
        Scheduler_EnqueueEvent(param_1, 1, slot)
```

| Field | Offset | Role |
|-------|--------|------|
| `scheduler` | `CBulanek+0x88` (`this+34` in IDA) | `CDSUpdatedItem` (24 B); hosts weapon/AI timer slots |
| Loop bound | **`i < 6`** | Hard-coded in disasm/decomp (`do…while uVar2 < 6`) |
| Enqueue **target** (`param_3`) | **Heap slot node** returned by `Scheduler_GetEventSlot` | Not the `CDSUpdatedItem` embed itself |
| Enqueue **kind** (`param_2`) | **`1`** | Drain → `Scheduler_ArmSlot(*(CDSUpdatedItem**)(node+0x10), *(uint*)(node+0x14))` |

**Null skip:** Unlike phase 1/3, phase 2 explicitly requires `Scheduler_GetEventSlot` ≠ NULL. Unregistered slot indices are silently skipped.

Live Ghidra call site: `0x0041af2d` (loop body enqueue).

#### Which slots 0..5 exist? (`CBulanekCtor@0x0041e4b0`)

Ctor always registers **slot 0** with `eventKind=7`:

```c
delayMs = (pGame->bTotalSlots != 1) ? 0x1f4 : 0x96;  // 500 ms vs 150 ms
Scheduler_RegisterEventSlot(&this->scheduler, 0, delayMs, 7);
```

Additional slots depend on `slotKind` (`param_3`):

| Condition | `slotKind` range | Extra `Scheduler_RegisterEventSlot` calls |
|-----------|------------------|-------------------------------------------|
| Human | `0..3` | **None** — ctor returns after slot 0 |
| Co-op vampire | `0x20..0x23` (`bVar2`) | slots **3**, **4** delay `0`; slot **5** delay **100** ms |
| Practice dummy | `0x24..0x27` (`bVar3`) | slots **3**, **4** delay `0` |
| Other AI (`bIsAiVariant`) | `≥ 4`, not co-op/practice | slot **2** delay `0` |

`bVar2 = (slotKind - 0x20) < 4`; `bVar3 = (slotKind - 0x24) < 4`; `bIsAiVariant = slotKind > 3`.

**Post-arm behavior** (when slots fire at runtime, not in this enqueue hook) is in `CBulanek_WeaponSchedulerCallback@0x00420b30` — slot **0** reload/die, **1** primary fire, **2** damage tick, **3/4** tournament delays, **5** pickup mask (task 02 scope).

### Phase 3 — weapon `trackManager` (kind **0**)

```
slot0 = Scheduler_GetEventSlot(&this->pWeapon->trackManager.scheduler, 0)
if (slot0 && !(slot0[+8] & 1))
    Scheduler_EnqueueEvent(param_1, 0, &this->pWeapon->trackManager)
```

| Field | Offset | Role |
|-------|--------|------|
| `pWeapon` | `CBulanek+0xF8` (`this+62` in IDA) | Always allocated in ctor (`OperatorNewWithBadAlloc(0x70)`) |
| `trackManager` | `pWeapon+0x08` | `CDSVideoPlayer`-like embed |
| Inner scheduler | `trackManager+0x04` (`+0x0C` from `pWeapon*`, IDA `*(this+62)+12`) | Polls **slot 0** only |
| Enqueue **target** | `&this->pWeapon->trackManager` | Weapon anim track manager |
| Enqueue **kind** | **`0`** | Same drain path as phase 1 |

Live Ghidra call site: `0x0041af62`.

**Note:** Neither IDA nor Ghidra null-checks `pWeapon`; safe because ctor always constructs it before any scheduler poll.

### `Scheduler_EnqueueEvent@0x0041ad80`

```c
void Scheduler_EnqueueEvent(void *listHead, uint kind, void *target) {
    node = Runtime_MallocOrThrow(8);
    node[0] = kind;
    node[1] = target;
    CIntListInsertSortedOrAppend(listHead, node, NULL, 1);
}
```

IDA `sub_41AD80`: `sub_42F6F0(8)` → store `a2`,`a3` → `sub_407E20` (same list helper).

### Pre-match drain (kind semantics)

After all entities poll vfn `+0x78`, `CGaming_RunPreMatchModal` drains `local_138` **back-to-front**:

| `node[0]` (kind) | Action |
|------------------|--------|
| **0** | `CGaming_ArmTrackMgrSchedulerIfUnpaused((CDSVideoPlayer*)node[1])` — arms `trackMgr->scheduler` slot 0 if `!trackMgr->bPaused` |
| **1** | `Scheduler_ArmSlot(parentUpdatedItem, slotIndex)` using fields at `node[1]+0x10` / `+0x14` |
| **2** | Audio stop hook (unrelated to `CBulanek_SchedulerTick`) |

## Functions table

| Address | Ghidra name | Role | Live xrefs |
|---------|-------------|------|------------|
| `0x0041aed0` | `CBulanek_SchedulerTick` | **Seed.** Three-phase enqueue poll; vtable slot 30 | **To:** `0x00481ecc` [DATA]. **From body:** calls `0x0041ad80` at `0x0041aef5`, `0x0041af2d`, `0x0041af62` |
| `0x0041ad80` | `Scheduler_EnqueueEvent` | Alloc 8 B `{kind,target}` → sorted int-list append | **14 callers** including seed (above), `CShot_SchedulerTick`, `CMina_OnSchedulerHook`, `CGaming_SeedMatchSchedulerEvents`, `CGaming_RegisterPreMatchSchedulerHooks`, `CGaming_EnqueuePreMatchSchedulerSlots`, `CAnim_OnSchedulerEnqueueSlot0` |
| `0x0041c290` | `CGaming_RunPreMatchModal` | Builds `local_138`, walks entity tree, indirect `vtable+0x78` | Calls `Scheduler_EnqueueEvent` indirectly via entity poll |
| `0x0041e4b0` | `CBulanekCtor` | Registers `scheduler` slots 0 (+2/3/4/5 by archetype) | Outside seed; defines which indices are non-NULL in phase 2 |
| `0x00420b30` | `CBulanek_WeaponSchedulerCallback` | Runtime slot 0..5 **fire** handlers (`eventKind=7`) | Downstream of arm/dispatch, not called from seed |
| `0x0042f1e0` | `Scheduler_GetEventSlot` | Resolve heap node for `(CDSUpdatedItem*, slotIndex)` | Called 3× per loop iteration in seed |

## Struct fields

| Offset | Field | Used in seed |
|--------|-------|--------------|
| `+0x88` | `scheduler` (`CDSUpdatedItem`) | Phase 2: `Scheduler_GetEventSlot(&scheduler, 0..5)` |
| `+0xA8` | `videoTrackManager` (`CDSVideoPlayer`) | Phase 1: inner scheduler @ `+0xAC`; enqueue `&videoTrackManager` |
| `+0xF8` | `pWeapon` (`CWeapon*`) | Phase 3: `&pWeapon->trackManager` @ `+0x08`, scheduler @ `+0x0C` |
| Slot node `+0x08` | armed/delay dword | Bit **0** armed gate for enqueue |
| `+0x199` | `bIsAiVariant` | Ctor: whether slot **2** gets registered |
| `+0x70` | `bPlayerSlot` | Indirect: drives ctor slot registration branches |

## Ghidra deltas

**none** (read-only session). Prior rounds already named `CBulanek_SchedulerTick`, typed `CBulanek * __thiscall`, and placed decompiler comment on the function body.

## Decomp fixes

| Issue | IDA vs Ghidra | Resolution |
|-------|---------------|------------|
| Seed body | **Match** — same three phases, same kind 0/1 targets, same `i < 6` loop with NULL check | No fix required |
| `Scheduler_EnqueueEvent` calling convention | Ghidra `__cdecl`; IDA `__cdecl` | Correct |
| Weapon path null check | Both omit `if (pWeapon)` | Ctor invariant; document only |
| Phase 2 redundant `GetEventSlot` | Ghidra calls `Scheduler_GetEventSlot` up to 3× per iteration | Compiler artifact; algorithm unchanged |

## Frida

**none** — static proof sufficient (live MCP decompile, IDA correlate, vtable DATA xref, 14 caller xrefs on helper).

## Remaining UNK

- **`piVar3[0x13]`** (`entity+0x4C`) in `CGaming_RunPreMatchModal` sibling-walk termination — not mapped to a named `CBulanek` field (carried from R6 task 01).
- **Slot 1** registration — not created in `CBulanekCtor`; likely armed later via `CBulanek_ArmFireDelayScheduler` / `TriggerPrimaryActionAndBroadcast` (task 02/05 scope).
- **Runtime** per-frame dispatch (`CDSApp_PulseTasks` → `Scheduler_DispatchDueEvents`) does **not** call `0x0041aed0` directly; seed is strictly the pre-modal enqueue hook.
