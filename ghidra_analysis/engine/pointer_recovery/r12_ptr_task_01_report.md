# Round 12 — Pointer Task 01 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 1 |
| **title** | CBulanek_StepMovementAndCollision pointer arithmetic @ `0x0041af70` |
| **seed_address** | `0x0041af70` |
| **reference** | [r11_ai_task_09_report.md](../ai_recovery/r11_ai_task_09_report.md), [r12_ptr_task_05_report.md](./r12_ptr_task_05_report.md) |

## Status

**PARTIAL** — Stack `tagRECT` locals and `CDSRect_IntersectInPlace` call site fixed. `CBulanek *this` prototype accepted in stored signature but **ECX auto-parameter remains `void *`** (Ghidra MCP API limitation); body still uses `(int)this + 0xNN` and `Scheduler_GetEventSlot((CDSUpdatedItem *)((int)this + 0xac),0)` instead of field paths. Program saved.

## Known issues (pre-fix)

| Issue | Pre-fix decompile |
|-------|-------------------|
| Prototype | `uchar __thiscall _Globals::CBulanek_StepMovementAndCollision(void *this)` |
| Stack overlap | `int local_30` passed to `CDSRect_IntersectInPlace` as 2nd arg |
| Collision copy | `CBulanek_CopyCollisionRectLocal(this, local_20)` with `int local_20[3]` |
| Scheduler base | `Scheduler_GetEventSlot((CDSUpdatedItem *)((int)this + 0xac),0)` |
| Member access | `*(int *)((int)this + 0xf8)` etc. throughout |

## Disassembly evidence (offsets)

All from `search_instructions` scoped to `function:0x0041af70` and `analyze_function_complete` disasm. `ESI` = `this` after `MOV ESI,ECX` @ `0x0041af76`.

### CBulanek `this` (ESI)

| Offset | Field (struct layout) | Instruction | Address |
|--------|----------------------|-------------|---------|
| `+0x20` | `nOrigin_x` | `LEA EBX,[ESI+0x20]` | `0x0041b0d5` |
| `+0x24` | `nOrigin_y` | `ADD EAX,[ESI+0x24]` | `0x0041b059` |
| `+0xA4` | `dwFacingAxisExtent` | `MOV [ESI+0xa4],EAX/ECX` | `0x0041b10b`, `0x0041b117` |
| `+0xAC` | `videoTrackManager.scheduler` (`+0xA8` embed `+4`) | `LEA ECX,[ESI+0xac]` | `0x0041b0e7` |
| `+0xD4` | `videoTrackManager.nCurrentTrackIdx` | `MOV EDX,[ESI+0xd4]` | `0x0041b0e0` |
| `+0xF0` | `pActiveAnim` | `MOV ECX,[ESI+0xf0]` | `0x0041af84` |
| `+0xF8` | `pWeapon` | `MOV EBX,[ESI+0xf8]` | `0x0041af78` |
| `+0x104` | walk embed base X (in `pReserved_preAmmo`) | `SUB EBX,[ESI+0x104]` | `0x0041afcb` |
| `+0x108` | walk embed base Y | `SUB EDX,[ESI+0x108]` | `0x0041afc5` |
| `+0x10C` | walk embed acc X | `ADD ECX,[ESI+0x10c]` / `MOV [ESI+0x10c]` | `0x0041b067`, `0x0041b0af` |
| `+0x110` | walk embed acc Y | `ADD EAX,[ESI+0x110]` / `MOV [ESI+0x110]` | `0x0041b051`, `0x0041b0a5` |
| `+0x114` | walk clip neg X | `MOV [ESI+0x114],EAX` | `0x0041b03b` |
| `+0x118` | walk clip neg Y | `MOV [ESI+0x118],EAX` | `0x0041b045` |
| `+0x148` | `nInitialTrack` | `MOV EAX,[ESI+0x148]` / `MOV [ESI+0x148],EDX` | `0x0041b0c9`, `0x0041b0ed` |
| `+0x15C` | transient pad dword 0 (`pPad_postWalkEmbed`) | `MOV [ESI+0x15c],EDI` (zero) | `0x0041b0b8` |
| `+0x160` | transient pad dword 1 | `MOV [ESI+0x160],EDI` (zero) | `0x0041b0be` |
| `+0x0` vftable | primary vtable | `MOV EDX,[ESI]` / indirect `CALL [EDX+0x20]` | `0x0041b14c`, `0x0041b158` |

`+0xAC` proof: `get_struct_layout CBulanek` → `videoTrackManager` @ `+0xA8` (168); `get_struct_layout CDSVideoPlayer` → `scheduler` @ `+4` → `168+4=172=0xAC`.

### CWeapon `pWeapon` (EBX after `MOV EBX,[ESI+0xf8]`)

| Offset | Field | Instruction | Address |
|--------|-------|-------------|---------|
| `+0x50` | `pTrackHolder` | `MOV EAX,[EBX+0x50]` | `0x0041af7e` |
| `+0x5C` | `dwParamA` | `MOV EBX,[EBX+0x5c]` | `0x0041afc2` |
| `+0x60` | `dwParamB` | `MOV EDX,[EBX+0x60]` | `0x0041afbf` |

### CBulAnim chain (`pActiveAnim` in ECX)

| Offset | Role | Instruction | Address |
|--------|------|-------------|---------|
| `+0x4` | anim delta X | used in `deltaRect.right` path | `0x0041af97` area |
| `+0x8` | anim delta Y | `ADD` into `deltaRect.bottom` | `0x0041afd5` area |

### Stack `tagRECT` call site @ `0x0041b01e`

```
0041afe5  LEA  EDX,[ESP+0x2c]    ; &deltaRect
0041afee  LEA  ECX,[ESP+0x20]    ; &originRect
0041b01e  CALL CDSRect_IntersectInPlace
```

## Ghidra MCP actions applied

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x0041af70` → `uchar __thiscall CBulanek::CBulanek_StepMovementAndCollision(CBulanek *this)` | Accepted; **warning**: ECX `this` cannot be retyped via API |
| `set_parameter_type` | `this` → `CBulanek *` | **Failed** — register auto-parameter |
| `set_local_variable_type` | `local_30` → `tagRECT` | Success — coalesced `local_2c`/`local_28`/`local_24` |
| `set_local_variable_type` | `local_20` → `tagRECT` | Success — coalesced `local_14` |
| `rename_variable` | `local_40`→`originRect`, `local_30`→`deltaRect`, `local_20`→`proposedCollisionRect`, `local_10`→`originCollisionRect` | Success |
| `set_decompiler_comment` | `0x0041af70`, `0x0041af76`, `0x0041b0e7` | Success |
| `set_plate_comment` | `0x0041af70` | Success — documents `CBulanek*` ECX + offset map |
| `force_decompile` | `0x0041af70` | After each mutation |
| `save_program` | `bulanci.exe` | Saved |

## Before / after decompile

### Before (session start)

```c
uchar __thiscall _Globals::CBulanek_StepMovementAndCollision(void *this)
{
  tagRECT local_40;
  int local_30;
  int local_2c;
  int local_28;
  int local_24;
  int local_20 [3];
  ...
  local_2c = *(int *)(iVar2 + 0x60) - *(int *)((int)this + 0x108);
  local_30 = *(int *)(iVar2 + 0x5c) - *(int *)((int)this + 0x104);
  ...
  tagRECT::CDSRect_IntersectInPlace(&local_40,&local_30);
  ...
  CBulanek::CBulanek_CopyCollisionRectLocal(this,local_20);
  ...
  pvVar3 = CDSUpdatedItem::Scheduler_GetEventSlot((CDSUpdatedItem *)((int)this + 0xac),0);
}
```

### After (post-fix)

```c
uchar __thiscall _Globals::CBulanek_StepMovementAndCollision(void *this)
{
  tagRECT originRect;
  tagRECT deltaRect;
  tagRECT proposedCollisionRect;
  tagRECT originCollisionRect;
  ...
  deltaRect.top  = *(int *)(iVar2 + 0x60) - *(int *)((int)this + 0x108);
  deltaRect.left = *(int *)(iVar2 + 0x5c) - *(int *)((int)this + 0x104);
  deltaRect.bottom = deltaRect.top + *(int *)(*(int *)((int)this + 0xf0) + 8);
  deltaRect.right  = deltaRect.left + *(int *)(*(int *)((int)this + 0xf0) + 4);
  ...
  tagRECT::CDSRect_IntersectInPlace(&originRect,&deltaRect);
  ...
  CBulanek::CBulanek_CopyCollisionRectLocal(this,&proposedCollisionRect.left);
  ...
  pvVar3 = CDSUpdatedItem::Scheduler_GetEventSlot((CDSUpdatedItem *)((int)this + 0xac),0);
}
```

### Target (when `CBulanek *this` propagates — cf. `CBulanek_CopyCollisionRectLocal@0x00417410`)

```c
uchar __thiscall CBulanek::CBulanek_StepMovementAndCollision(CBulanek *this)
{
  CWeapon *w = this->pWeapon;
  ...
  deltaRect.left = w->dwParamA - *(int *)&this->pReserved_preAmmo[0];  // +0x104
  ...
  Scheduler_GetEventSlot(&this->videoTrackManager.scheduler, 0);
  ...
}
```

## Remaining UNK

| Item | Blocker |
|------|---------|
| `void *this` in decompiler header | Ghidra MCP: `set_parameter_type` / `set_function_prototype` cannot retype ECX auto-parameter; `set_function_this_type` tool absent from current MCP |
| Raw `(int)this + 0xNN` member reads | Depends on `CBulanek *this` propagation (sibling `CBulanek_CopyCollisionRectLocal` already has `CBulanek *this`) |
| `Scheduler_GetEventSlot((CDSUpdatedItem *)((int)this + 0xac),0)` | Same — needs `&this->videoTrackManager.scheduler` |
| `(tagRECT *)((int)this + 0x20)` for `OffsetCollisionRectByFacing` | Should be `&this->nOrigin_x` or `tagRECT*` to origin pair; decompiler mis-casts `void* this` |
| `&proposedCollisionRect.left` / `&originCollisionRect.left` | Partial — full `tagRECT *` preferred (compare `OffsetCollisionRectByFacing` 2nd pass uses `&originRect`) |
| Walk-embed field names in `CBulanek` struct | `pReserved_preAmmo@+0x104` still `byte[24]` — offsets proven in disasm; rename deferred to struct batch |
| `_Globals::` namespace prefix | Function not reparented to `CBulanek` class (would follow successful `this` retype) |

## Correlation

- Algorithm and offset map: [r11_ai_task_09_report.md](../ai_recovery/r11_ai_task_09_report.md)
- `tagRECT` stack coalescing at `CDSRect_IntersectInPlace` call: [r12_ptr_task_05_report.md](./r12_ptr_task_05_report.md) (same seed; task 01 extends to full-function pointer recovery + `this` typing)
