# CDSUpdatedItem

## Status

`VERIFIED` (size `0x18`; Ghidra struct matches ctor/dtor/scheduler consumers)

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Object footprint ends at `+0x17` (24 bytes) | `0x0042f060` | `CDSUpdatedItem_ctor` writes `*(byte*)(this+0x14)=0`; prior fields through `+0x10` |
| Embedded size in hosts | `0x0041edf0` (`CShot_Ctor`) | `CDSUpdatedItem_ctor(this+0x22)` (`this+0x88` bytes) inside `0xb0`-byte `CShot` |
| Global task list node is separate | `0x0042f060` | `g_pTaskList` uses `OperatorNew(0x18)` for `CDSCollection`, not `CDSUpdatedItem` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `void*` | `pVftable_IDSUpdated` | **Not** written in `CDSUpdatedItem_ctor@0x0042f060` (first store at `+4`). **Host ctor** after embed init: `CAdvertising_ctor@0x0040e670` `*(this+0x70)=0x4817b8`; `CShot_Ctor@0x004170bd` `*(this+0x88)=0x481c5c`; `ConstructTrackManager@0x00439cb6` `*(this+4)=0x487770` on `CDSVideoPlayer` scheduler embed |
| `+0x04` | 4 | `void*` | `pVftable_IDSEventHandler` | `CDSUpdatedItem_ctor@0x0042f060` (`0x4870ac`); task-list registration uses this subobject pointer (`puVar1=param_1+4`) |
| `+0x08` | 4 | `void*` | `pSelf` | `CDSUpdatedItem_ctor@0x0042f060` (`= this`) |
| `+0x0c` | 4 | `void*` | `pEventSlots` | `CDSUpdatedItem_ctor@0x0042f060` (`=0`); `Scheduler_RegisterEventSlot@0x0042f210` |
| `+0x10` | 4 | `uint32` | `cEventSlots` | `CDSUpdatedItem_ctor@0x0042f060` (`=0`); `Scheduler_DispatchDueEvents@0x0042eb30` (`param_1[4]`) |
| `+0x14` | 1 | `byte` | `bIsLive` | `CDSUpdatedItem_ctor@0x0042f060`; `CDSApp_PulseTasks` loop (`tick_system.md`) |

Heap **event slot** records (not part of this struct) are `0x1c` bytes: `Scheduler_RegisterEventSlot@0x0042f210` (`Runtime_MallocOrThrow(0x1c)`).

### Host facet pattern (`+0x00`)

MSVC multiple-inheritance: the embedded `CDSUpdatedItem` at offset *N* in a host class has a separate **IDSUpdated** vtable pointer at embed `+0x00`. `CDSUpdatedItem_ctor` initializes only the **IDSEventHandler** facet at `+4` and scheduler fields `+8..+14`; the host ctor always stores `pVftable_IDSUpdated` at embed `+0` in the same function, immediately after `CDSUpdatedItem_ctor` (verified round 3 task 25 for `CAdvertising+0x70` and cross-checked `CWindow+0x70`, `CEdit+0x68`, `CMina+0xf0`, `CGaming+0x68`, `CGame+0x4`, `CShot+0x88`, `CDSDirectSound+0x4`, `CDSVideoPlayer+0x4`).

## Ghidra apply

```
get_struct_layout CDSUpdatedItem  → Size: 24 bytes (agent todo 43, 2026-05-30)
  pVftable_IDSUpdated @ 0
  pVftable_IDSEventHandler @ 4
  pSelf @ 8
  pEventSlots @ 12
  cEventSlots @ 16   (Ghidra may still label dwEventSlots — cosmetic)
  bIsLive @ 20
```

**Prototypes (slice 41):** `CDSUpdatedItem_ctor@0x0042f060` → `CDSUpdatedItem * __fastcall CDSUpdatedItem_ctor(CDSUpdatedItem *this)`; `Scheduler_RegisterEventSlot@0x0042f210` → `void __thiscall Scheduler_RegisterEventSlot(CDSUpdatedItem *this, …)` (decompiler `this` may stay `void*`).

**Embed hosts:** **29** `CDSUpdatedItem_ctor` call sites (`get_xrefs_to@0x0042f060`, 2026-05-30). Every sampled caller stores a host **IDSUpdated** vtable at embed `+0x00` immediately after the ctor; `CDSUpdatedItem_ctor` itself only initializes `+4..+14`.

### Host facet vtable catalog (`pVftable_IDSUpdated` @ embed `+0`)

| Host ctor / site | Call @ | Embed base | Store @ embed+0 (symbol / value) |
|------------------|--------|------------|----------------------------------|
| `CEdit_BuildAt` | `0x0040779b` | `this+0x68` | host `IDSUpdated` vtable (BuildAt pattern) |
| `CScrollBar_BuildAt` | `0x004083ad` | `this+0x68` | host vtable |
| `CScroller_BuildAt` | `0x004089fd` | `updatedItem` | `0x0047f9c8` |
| `CStartGame2_ctor` | `0x0041055b` | `this+0x1c` | `g_pCStartGame2_vftable_*` |
| `ConstructTrackManager` | `0x00439cab` | `CDSVideoPlayer+0x04` | `0x00487770` |
| `CGaming_ctor` | `0x0041ffcf` | `this+0x1a` | `g_pCGaming_vftable_*` |
| `CNumCounterCtor` | `0x00427faf` | `this+0x1a` | `g_pCNumCounter_vftable_*` |
| `CGame_ctor` | `0x00414abb` | `param_1+0x04` | `g_pCGame_vftable_IDSChained` |
| `CDSDirectSound_ctor` | `0x0043c7fe` | `param_1+0x04` | `g_pCDSDirectSound_vftable_IDSChained_minus04` |
| `CScroller_ctor` | `0x0040340a` | embed | host vtable |
| `CScrollBar_ctor` | `0x004047cb` | embed | host vtable |
| `CEdit_ctor` | `0x00404d4a` | embed | host vtable |
| `CLevelScript::ctor` | `0x00418612` | timer table | `g_pCLevelScript_vftable_*` |
| `CWindow_ctor` | `0x0040baf7` | `this+0x70` | host vtable |
| `CAdvertising_ctor` | `0x0040e649` | `updatedItem` | `g_pCAdvertising_vftable_IDSUpdated` @ `0x0040e670` |
| `CStartGame1_ctor` | `0x0040f463` | `updatedItem` | host vtable |
| `CShot_SubobjectCtor` | `0x0041709d` | `this+0x22` | `g_pCShot_vftable_*` |
| `FUN_00418d70` | `0x00418dde` | embed | host vtable |
| `CMina_DefaultCtor` | `0x0041a9ce` | `this+0x3c` | `g_pCMina_vftable_*` |
| `CGaming_Ctor` | `0x0041abaa` | `this+0x1a` | `g_pCGaming_vftable_*` |
| `CMina_Ctor` | `0x0041cbaf` | `updatedItem` | `g_pCMina_vftable_IDSUpdated` |
| `InitMine` | `0x0041cd1e` | `updatedItem` | `g_pCMina_vftable_IDSUpdated` |
| `CBulanekCtor` | `0x0041e521` | `this+0x88` | `CBulanek` / `ODSImage` MI facets |
| `CShot_Ctor` | `0x0041ee61` | `updatedItem` @ `+0x88` | `g_pCShot_vftable_CDSUpdatedItem_88` |
| `CRuch_ctor` | `0x00423c5d` | embed | `g_pCRuch_vftable_*` |
| `CMenu_ctor` | `0x0042455b` | `updatedItem` @ `+0x68` | `CMenu` vtable |
| `CPoemScroller_Constructor` | `0x004262ff` | `this+0x68` | `g_pCPoemScroller_vftable_*` |
| `CMenu_ctor_with_ui` | `0x0042661d` | pad-derived embed | `CMenu` vtable |
| `CNumCounter_ctor` | `0x0042737b` | pad-derived embed | `CNumCounter` vtable |

**Ghidra embed typing (todo 43 / r2-1):** nested `CDSUpdatedItem` on `CAdvertising.updatedItem`, `CMina.updatedItem`, `CBulanek.scheduler`, `CDSVideoPlayer.scheduler`, `CShot.updatedItem`, `CMenu.updatedItem`, **`CGame.scheduler` @ +0x04** (r2 todo 1, 2026-05-30) — decompiler shows `pVftable_IDSUpdated` instead of `dwField_0x00`.

## UNK

- `CDSObject` **image** variant: same byte span may be constructed via `CDSImage_ctor` without `CDSUpdatedItem_ctor` (`CDSObject.md`).
- Per-host **slot maps** for each `IDSUpdated` vtable (ctor store only; dispatch not expanded).

## Follow-up

- Batch 42 / facet @ `+0x00`: resolved `CAdvertising+0x70` — host writes `0x4817b8` at `CAdvertising_ctor@0x0040e670`; see [round3_batch42_cdsupdateditem_facets_report.md](./round3_batch42_cdsupdateditem_facets_report.md).
