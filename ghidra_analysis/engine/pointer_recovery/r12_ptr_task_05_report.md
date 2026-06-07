# Round 12 — Pointer Task 05 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 5 |
| **title** | tagRECT stack layout — `CDSRect_IntersectInPlace` + `CBulanek_StepMovementAndCollision` call site |
| **seed_address** | `0x0041af70` |
| **callee_address** | `0x00433200` |
| **call_site** | `0x0041b01e` |

## Status

**DONE** — `local_30` retyped to `tagRECT` in seed; `CDSRect_IntersectInPlace` second parameter corrected from `int *` to `tagRECT *`. Call site decompile now shows `CDSRect_IntersectInPlace(&local_40, &local_30)` with field accesses on both rects. Program saved.

## Issue (pre-fix)

In `CBulanek_StepMovementAndCollision@0x0041af70`, Ghidra declared four overlapping `int` locals at `Stack[-0x30]`..`Stack[-0x24]`:

| Local | Offset | Pre-fix type |
|-------|--------|--------------|
| `local_30` | `Stack[-0x30]` | `int` |
| `local_2c` | `Stack[-0x2c]` | `int` |
| `local_28` | `Stack[-0x28]` | `int` |
| `local_24` | `Stack[-0x24]` | `int` |

Disassembly at the call site passes `LEA EDX,[ESP+0x2c]` (address of `local_30.left`) as the second argument and `LEA ECX,[ESP+0x20]` (address of `local_40`) as the first — both are 16-byte `tagRECT` stack slots.

Pre-fix decompile showed `CDSRect_IntersectInPlace(&local_40, &local_30)` where `local_30` was typed `int`, causing the callee to treat the second arg as `int *` and use `param_1[0..3]` indexing instead of `tagRECT` fields.

`CDSRect_IntersectInPlace@0x00433200` had signature `void __thiscall CDSRect_IntersectInPlace(tagRECT * this, int * param_1)` — second parameter wrong.

## Disassembly evidence

### Call site @ `0x0041b01e`

```
0041afe5  LEA  EDX,[ESP + 0x2c]     ; &local_30 (second RECT)
0041afed  PUSH EDX
0041afee  LEA  ECX,[ESP + 0x20]     ; &local_40 (first RECT, in-place dest)
0041b01e  CALL 0x00433200           ; CDSRect_IntersectInPlace
```

Stack writes before call populate the second rect:

```
0041afd7  MOV  [ESP+0x2c], EBX       ; local_30.top
0041afd1  MOV  [ESP+0x30], EDX       ; local_30.left
0041afe1  MOV  [ESP+0x38], EDX       ; local_30.right
0041b00e  MOV  [ESP+0x38], EBX       ; local_30.bottom (after prior setup)
```

### Callee entry @ `0x00433200`

```
00433200  MOV  EAX, ECX              ; dest RECT* (ECX regparam)
00433202  MOV  ECX, [ESP + 0x4]      ; src RECT* (stack)
00433206  MOV  EDX, [ECX]            ; src->left
00433208  CMP  EDX, [ECX + 0x8]      ; left vs right
...
00433244  RET  0x4                   ; stdcall cleanup, 1 stack arg
```

Convention matches sibling `CDSRect_Intersect@0x00433280`: `__thiscall` with `tagRECT *` in ECX (`this` / dest) and `tagRECT *` on stack (`param_1` / src). Not a C++ member `this` — Ghidra namespace artifact on `tagRECT::`.

## Ghidra mutations

| Action | Target | Result |
|--------|--------|--------|
| `set_local_variable_type` | `local_30` @ `0x0041af70` → `tagRECT` | Success — absorbed overlapping `local_2c`/`local_28`/`local_24`; `local_20` also coalesced to `tagRECT` |
| `set_function_prototype` | `0x00433200` → `void __thiscall CDSRect_IntersectInPlace(tagRECT * this, tagRECT * param_1)` | Success — `param_1` `int *` → `tagRECT *` |
| `force_decompile` | `0x00433200`, `0x0041af70` | Refreshed |
| `save_program` | `bulanci.exe` | Saved |

## Post-fix decompile (verified)

### `CDSRect_IntersectInPlace@0x00433200`

```c
void __thiscall tagRECT::CDSRect_IntersectInPlace(tagRECT *this, tagRECT *param_1)
{
  // param_1->left/right/top/bottom used throughout (no int* indexing)
  // this->left/right/top/bottom written in-place
}
```

Parameters: `this` (`tagRECT *`, ECX), `param_1` (`tagRECT *`, Stack[0x4]).

### `CBulanek_StepMovementAndCollision@0x0041af70` — call site region

```c
tagRECT local_40;   // origin/holder rect
tagRECT local_30;   // proposed delta rect
...
local_30.top  = w->dwParamB - embedY;
local_30.left = w->dwParamA - embedX;
local_30.bottom = local_30.top  + anim->height;
local_30.right  = local_30.left + anim->width;
...
tagRECT::CDSRect_IntersectInPlace(&local_40, &local_30);
iVar6 = local_30.left - local_40.left;
iVar4 = local_30.top  - local_40.top;
```

Stack locals after fix:

| Local | Offset | Post-fix type |
|-------|--------|---------------|
| `local_40` | `Stack[-0x38]` | `tagRECT` |
| `local_30` | `Stack[-0x2c]` | `tagRECT` |
| `local_20` | `Stack[-0x18]` | `tagRECT` |
| `local_10` | `Stack[-0x10]` | `tagRECT` |

## Prototype verification

| Param | Type | Storage | Role |
|-------|------|---------|------|
| `this` / `a` | `tagRECT *` | ECX | In-place destination rect (mutated) |
| `param_1` / `b` | `tagRECT *` | Stack[0x4] | Source rect (read-only) |

Matches disasm and sibling `CDSRect_Intersect@0x00433280` pattern. Semantically equivalent to `(tagRECT *a, tagRECT *b)` with MSVC `__thiscall` ECX-first convention used by this binary's RECT helpers.

## Correlation

Prior algorithm doc ([r11_ai_task_09_report.md](../ai_recovery/r11_ai_task_09_report.md)) already described `tagRECT origin` + `tagRECT delta` at this call site; this task fixes Ghidra's stack typing to match that model.
