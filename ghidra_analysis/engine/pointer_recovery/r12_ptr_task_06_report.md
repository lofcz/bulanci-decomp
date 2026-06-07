# Round 12 — Pointer Task 06 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 6 |
| **title** | `Scheduler_GetEventSlot` pointer arithmetic — `CDSVideoPlayer` embed vs `CBulanek+0xAC` |
| **seed_address** | `0x0041af70` |
| **callee_address** | `0x0042f1e0` |
| **call_site** | `0x0041b0f3` |

## Status

**DONE** — Struct layouts verified correct; disassembly proves `+0xAC` is **`videoTrackManager.scheduler`** (inner `CDSUpdatedItem` @ `CDSVideoPlayer+4`), **not** the standalone `CBulanek.scheduler` @ `+0x88`. `+0xD4` proven as **`videoTrackManager.nCurrentTrackIdx`**. No struct mutations required. Decompiler comments @ `0x0041b0e0` / `0x0041b0e7`; `save_program bulanci.exe`.

## Question

Layout doc lists both `scheduler` @ `+0x88` and `videoTrackManager` @ `+0xA8`. Seed uses `LEA ECX,[ESI+0xAC]` before `Scheduler_GetEventSlot`. Is `+0xAC` the inner track-manager scheduler or a mis-typed standalone path?

**Answer:** Inner track-manager scheduler. Arithmetic: `0xA8 + 0x4 = 0xAC`.

## Struct layouts (live `get_struct_layout`)

### `CDSVideoPlayer` — 72 bytes (`0x48`)

| Offset | Size | Type | Name |
|--------|------|------|------|
| `+0x00` | 4 | `void *` | `pVftable_IDSChained` |
| `+0x04` | 24 | `CDSUpdatedItem` | `scheduler` |
| `+0x1C` | 16 | `CDSTrackVector` | `trackVector` |
| `+0x2C` | 4 | `int` | `nCurrentTrackIdx` |
| `+0x30` | 4 | `void *` | `pRenderTarget` |
| … | … | … | tail through `+0x44` |

### `CBulanek` — 412 bytes (`0x19C`) — relevant band

| Offset | Size | Type | Name |
|--------|------|------|------|
| `+0x88` | 24 | `CDSUpdatedItem` | `scheduler` (player host scheduler) |
| `+0xA0` | 4 | `pointer` | `vftable_event` |
| `+0xA4` | 4 | `uint` | `dwFacingAxisExtent` |
| `+0xA8` | 72 | `CDSVideoPlayer` | `videoTrackManager` |
| `+0xF8` | 4 | `CWeapon *` | `pWeapon` |
| `+0x148` | 4 | `int` | `nInitialTrack` (runtime staging for facing idx in this function) |

**Two schedulers:** `CBulanek+0x88` (host) and `videoTrackManager+0x04` (walk/anim track mgr). They are distinct objects; seed uses the **inner** one.

## Disassembly evidence

### Seed call site @ `0x0041b0e0`–`0x0041b0f8`

```
0041b0e0  MOV  EDX, dword ptr [ESI + 0xd4]   ; nCurrentTrackIdx
0041b0e6  PUSH EDI
0041b0e7  LEA  ECX, [ESI + 0xac]             ; &videoTrackManager.scheduler
0041b0ed  MOV  dword ptr [ESI + 0x148], EDX   ; stage track idx for facing helper
0041b0f3  CALL 0x0042f1e0                    ; Scheduler_GetEventSlot
0041b0f8  TEST byte ptr [EAX + 0x8], 0x1      ; slot descriptor bit0 (armed)
```

| Instruction | Effective address | Resolved field |
|-------------|---------------------|----------------|
| `[ESI+0xF8]` @ `0x0041af78` | `CBulanek+0xF8` | `pWeapon` |
| `[ESI+0xF0]` @ `0x0041af84` | `CBulanek+0xF0` | `pActiveAnim` |
| `[ESI+0x104]` / `[ESI+0x108]` | walk embed | weapon-track anchor delta |
| `[ESI+0xD4]` @ `0x0041b0e0` | `0xA8+0x2C` | `videoTrackManager.nCurrentTrackIdx` |
| `LEA [ESI+0xAC]` @ `0x0041b0e7` | `0xA8+0x04` | `videoTrackManager.scheduler` |
| `[ESI+0xA4]` @ `0x0041b10b` | `CBulanek+0xA4` | `dwFacingAxisExtent` (vertical vs horizontal pick) |
| `[ESI+0x148]` | `CBulanek+0x148` | temp copy of `nCurrentTrackIdx` for `OffsetCollisionRectByFacing` |

### Cross-check: `CBulanek_SchedulerTick@0x0041aed0`

Same binary distinguishes both schedulers in one function:

```
0041aed6  LEA  ECX, [EBP + 0xac]    ; videoTrackManager.scheduler → GetEventSlot slot 0
0041aeeb  LEA  EAX, [EBP + 0xa8]    ; &videoTrackManager → EnqueueEvent target
0041af01  LEA  EDI, [EBP + 0x88]    ; standalone CBulanek.scheduler → slots 0..5 loop
```

If `+0xAC` were the standalone scheduler (`+0x88`), it would be `+0x88`, not `+0xAC`.

### Cross-check: `CBulanek_TryBotRandomAction@0x00420adf`

```
00420adf  MOV  EAX, dword ptr [ESI + 0xd4]   ; ApplyAction arg = nCurrentTrackIdx
```

## `Scheduler_GetEventSlot@0x0042f1e0`

```c
void * __thiscall CDSUpdatedItem::Scheduler_GetEventSlot(CDSUpdatedItem *this, uint param_1)
{
  if (Scheduler_IsSlotLive(this, param_1))
    return *(void **)((int)this->pEventSlots + param_1 * 4);
  return (void *)0x0;
}
```

First argument must be **`CDSUpdatedItem*`** at the embed base (`+0xAC`). Passing `+0xB8` (`scheduler.pEventSlots`) would misalign vtable/`pSelf` reads — disasm proves `+0xAC`.

## Ghidra mutations

| Action | Target | Result |
|--------|--------|--------|
| `get_struct_layout` | `CDSVideoPlayer`, `CBulanek`, `CDSUpdatedItem` | Layouts match [CDSVideoPlayer.md](../struct_recovery/CDSVideoPlayer.md) / [CBulanek.md](../struct_recovery/CBulanek.md) — **no field moves** |
| `set_function_prototype` | `0x0041af70` → `uchar __thiscall CBulanek_StepMovementAndCollision(CBulanek *this)` | Applied; MCP warns ECX auto-`this` stays `void*` in decompiler |
| `set_decompiler_comment` | `0x0041b0e0`, `0x0041b0e7` | Field-path annotations for `nCurrentTrackIdx` / inner scheduler |
| `force_decompile` | `0x0041af70` | Refreshed |
| `save_program` | `bulanci.exe` | Saved |

**Not applied:** `__fastcall CBulanek*` — attempted; decompiler field resolution **regressed** (wrong fields e.g. `bAmmoKind0` for `pWeapon`, `dwCurrentFrameIdx` for `nCurrentTrackIdx`). Reverted to `__thiscall`.

**API limit:** `set_parameter_type` / `HighFunctionDBUtil.updateDBVariable` on ECX auto-`this` → *"Auto-parameter may not be modified"* (same as R10/R12 sibling tasks).

## Post-fix decompile (seed @ `0x0041af70`)

Semantically correct region (offset arithmetic matches struct paths):

```c
*(undefined4 *)((int)this + 0x148) = *(undefined4 *)((int)this + 0xd4);
pvVar3 = CDSUpdatedItem::Scheduler_GetEventSlot((CDSUpdatedItem *)((int)this + 0xac), 0);
if ((*(byte *)((int)pvVar3 + 8) & 1) != 0) {
  if (*(int *)((int)this + 0x148) < 2)
    *(LONG *)((int)this + 0xa4) = originRect.top;
  else
    *(LONG *)((int)this + 0xa4) = originRect.left;
}
```

**Intended symbolic form** (manual retype of `this` in Ghidra UI, or when ECX typing is unlocked):

```c
this->nInitialTrack = this->videoTrackManager.nCurrentTrackIdx;
slot0 = Scheduler_GetEventSlot(&this->videoTrackManager.scheduler, 0);
if (slot0 && (slot0[8] & 1))
  this->dwFacingAxisExtent = (this->nInitialTrack < 2) ? originRect.top : originRect.left;
```

`CBulanek_SchedulerTick` (`CBulanek *this` already typed) shows `(this->videoTrackManager).scheduler.pEventSlots` — cosmetic off-by-field-name; **address at `+0xAC` is correct** per LEA.

## Offset summary

| CBulanek offset | Path | Role in seed |
|-----------------|------|--------------|
| `+0x88` | `this->scheduler` | **Not used** in seed (used in `CBulanek_SchedulerTick` slots 0..5) |
| `+0xA8` | `this->videoTrackManager` | Embed base |
| `+0xAC` | `this->videoTrackManager.scheduler` | `Scheduler_GetEventSlot` arg |
| `+0xD4` | `this->videoTrackManager.nCurrentTrackIdx` | Facing-axis track index |
| `+0xA4` | `this->dwFacingAxisExtent` | Written when slot0 armed |
| `+0x148` | `this->nInitialTrack` | Staging copy of `nCurrentTrackIdx` during step |

## Correlation

- [r11_ai_task_03_report.md](../ai_recovery/r11_ai_task_03_report.md) — same `+0xAC` / `+0xD4` paths in bot idle branch
- [round6_logic_task_02_report.md](../logic_recovery/round6_logic_task_02_report.md) — seed control-flow pseudocode
- [round6_logic_task_01_report.md](../logic_recovery/round6_logic_task_01_report.md) — `CBulanek_SchedulerTick` dual-scheduler poll
