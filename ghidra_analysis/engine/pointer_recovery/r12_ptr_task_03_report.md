# Round 12 — Pointer Task 03 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 3 |
| **title** | `CBulanek_OffsetCollisionRectByFacing` pointer arithmetic @ `0x00417460` |
| **seed_address** | `0x00417460` |
| **caller_address** | `0x0041af70` (`CBulanek_StepMovementAndCollision`, 2 call sites) |
| **reference** | [r11_ai_task_09_report.md](../ai_recovery/r11_ai_task_09_report.md), [r12_ptr_task_06_report.md](./r12_ptr_task_06_report.md) |

## Status

**DONE** — Seed decompile uses `this->nFacingAxisExtent`, `tagRECT *` rect parameters with field accesses, and `nTrackIdx` threshold `> 1`. Struct field renamed `dwFacingAxisExtent` → `nFacingAxisExtent` @ `CBulanek+0xA4`. Caller `0x0041af70` force-decompiled; second `OffsetCollisionRectByFacing` call now passes `&originRect, &proposedCollisionRect`. Caller `this` remains `void *` in decompile (Ghidra ECX auto-param API limit; documented in existing plate comments). `save_program bulanci.exe`.

## Issue (pre-fix)

| Issue | Pre-fix decompile |
|-------|-------------------|
| Rect args | `int *param_1`, `int *param_2` with `param_1[1]`, `*param_2`, `param_2[3]` indexing |
| Facing field | `this->dwFacingAxisExtent` or `*(int *)(this + 0xa4)` when struct misaligned |
| Track param | `int param_3` unnamed |
| Caller 1st call | `(int *)((int)this + 0x20)` as src rect |
| Caller 2nd call | `&local_40.left`, `local_20` mixed int*/rect |

## Disassembly evidence — seed @ `0x00417460`

`__thiscall`: `ECX` = `this`; stack args `[ESP+4]` src rect, `[ESP+8]` dst rect, `[ESP+0xC]` track idx. `RET 0xC` = 3 stack parameters.

```
00417460  CMP  dword ptr [ESP + 0xc], 0x2     ; nTrackIdx vs 2 (JL → horizontal branch)
00417465  MOV  ECX, dword ptr [ECX + 0xa4]  ; this->nFacingAxisExtent
0041746b  JL   0x00417486                   ; trackIdx < 2 → horizontal (tracks 0,1)
```

### Vertical branch (`nTrackIdx >= 2`, tracks 2/3)

```
0041746d  MOV  EAX, dword ptr [ESP + 0x4]   ; pSrcRect
00417471  MOV  EDX, dword ptr [EAX + 0x4]   ; pSrcRect->top
00417474  MOV  EAX, dword ptr [ESP + 0x8]   ; pDstRect
00417478  ADD  dword ptr [EAX], ECX           ; pDstRect->left += nFacingAxisExtent
0041747a  ADD  dword ptr [EAX + 0x4], EDX     ; pDstRect->top += pSrcRect->top
0041747d  ADD  dword ptr [EAX + 0x8], ECX     ; pDstRect->right += nFacingAxisExtent
00417480  ADD  dword ptr [EAX + 0xc], EDX     ; pDstRect->bottom += pSrcRect->top
00417483  RET  0xc
```

### Horizontal branch (`nTrackIdx < 2`, tracks 0/1)

```
00417486  MOV  EAX, dword ptr [ESP + 0x8]   ; pDstRect
0041748a  MOV  EDX, dword ptr [ESP + 0x4]   ; pSrcRect
0041748e  MOV  EDX, dword ptr [EDX]         ; pSrcRect->left
00417490  ADD  dword ptr [EAX], EDX           ; pDstRect->left += pSrcRect->left
00417492  ADD  dword ptr [EAX + 0x4], ECX     ; pDstRect->top += nFacingAxisExtent
00417495  ADD  dword ptr [EAX + 0x8], EDX     ; pDstRect->right += pSrcRect->left
00417498  ADD  dword ptr [EAX + 0xc], ECX     ; pDstRect->bottom += nFacingAxisExtent
0041749b  RET  0xc
```

| Offset / operand | Resolved meaning |
|------------------|------------------|
| `[ECX+0xA4]` @ `0x00417465` | `CBulanek.nFacingAxisExtent` |
| `[ESP+0x4]` | `tagRECT *pSrcRect` |
| `[ESP+0x8]` | `tagRECT *pDstRect` |
| `[ESP+0xC]` | `int nTrackIdx` |
| `[EAX+0x4]` on src | `pSrcRect->top` (vertical) |
| `[EDX]` on src | `pSrcRect->left` (horizontal) |

## Disassembly evidence — caller call sites @ `0x0041af70`

`ESI` = `this` (`MOV ESI,ECX` @ `0x0041af76`).

### Call 1 @ `0x0041b0db` (origin collision rect)

```
0041b0c9  MOV  EAX, dword ptr [ESI + 0x148]  ; nInitialTrack (staged track idx)
0041b0cf  PUSH EAX
0041b0d0  LEA  ECX, [ESP + 0x50]             ; &originCollisionRect (stack tagRECT)
0041b0d4  PUSH ECX                           ; pDstRect
0041b0d5  LEA  EBX, [ESI + 0x20]             ; &nOrigin_x → tagRECT overlay
0041b0d8  PUSH EBX                           ; pSrcRect
0041b0d9  MOV  ECX, ESI                      ; this
0041b0db  CALL 0x00417460
```

| Push order | Argument | Proof |
|------------|----------|-------|
| 3rd | `nInitialTrack` @ `+0x148` | `[ESI+0x148]` @ `0x0041b0c9` |
| 2nd | dst stack rect | `LEA [ESP+0x50]` @ `0x0041b0d0` |
| 1st | `&nOrigin_x` @ `+0x20` | `LEA [ESI+0x20]` @ `0x0041b0d5` |

### Scheduler gate + `nFacingAxisExtent` refresh (between calls)

```
0041b0e0  MOV  EDX, dword ptr [ESI + 0xd4]   ; videoTrackManager.nCurrentTrackIdx (+0xA8+0x2C)
0041b0ed  MOV  dword ptr [ESI + 0x148], EDX  ; stage to nInitialTrack
0041b0f3  CALL Scheduler_GetEventSlot        ; LEA ECX,[ESI+0xAC] @ 0x0041b0e7
0041b0fe  CMP  dword ptr [ESI + 0x148], 0x2
0041b10b  MOV  dword ptr [ESI + 0xa4], EAX     ; nFacingAxisExtent = originRect.left (vertical)
0041b117  MOV  dword ptr [ESI + 0xa4], ECX     ; nFacingAxisExtent = originRect.top (horizontal)
```

### Call 2 @ `0x0041b130` (proposed collision rect)

```
0041b11d  MOV  EDX, dword ptr [ESI + 0x148]  ; staged nInitialTrack
0041b123  PUSH EDX
0041b124  LEA  EAX, [ESP + 0x40]             ; &proposedCollisionRect
0041b128  PUSH EAX                           ; pDstRect
0041b129  LEA  ECX, [ESP + 0x24]             ; &originRect (proposed world rect)
0041b12d  PUSH ECX                           ; pSrcRect
0041b12e  MOV  ECX, ESI
0041b130  CALL 0x00417460
```

## Struct field — `nFacingAxisExtent` @ `+0xA4`

Live `get_struct_layout CBulanek` (412 B):

| Offset | Size | Type | Name |
|--------|------|------|------|
| `+0xA0` | 4 | `pointer` | `vftable_event` |
| `+0xA4` | 4 | `int` | `nFacingAxisExtent` |
| `+0xA8` | 72 | `CDSVideoPlayer` | `videoTrackManager` |

Disasm `MOV ECX,[ECX+0xA4]` @ `0x00417465` matches `+0xA4`. Renamed from `dwFacingAxisExtent` (`uint` → `int`; pixel extent, signed-safe in adds).

## Ghidra MCP actions applied

| Action | Target | Result |
|--------|--------|--------|
| `modify_struct_field` | `CBulanek.dwFacingAxisExtent` → `nFacingAxisExtent` (`int`) | Success @ `+0xA4` |
| `set_function_prototype` | `0x00417460` → `void __thiscall CBulanek_OffsetCollisionRectByFacing(CBulanek *this, tagRECT *pSrcRect, tagRECT *pDstRect, int nTrackIdx)` | Success |
| `set_function_prototype` | `0x0041af70` → `uchar __thiscall CBulanek_StepMovementAndCollision(CBulanek *this)` | Success (ECX still `void *` in decompile) |
| `force_decompile` | `0x00417460`, `0x0041af70` | Refreshed |
| `save_program` | `bulanci.exe` | Saved |

## Post-fix decompile (verified)

### `CBulanek_OffsetCollisionRectByFacing@0x00417460`

```c
void __thiscall
CBulanek::CBulanek_OffsetCollisionRectByFacing
          (CBulanek *this, tagRECT *pSrcRect, tagRECT *pDstRect, int nTrackIdx)
{
  int extent = this->nFacingAxisExtent;
  if (1 < nTrackIdx) {                          // vertical: tracks 2,3
    pDstRect->left   += extent;
    pDstRect->top    += pSrcRect->top;
    pDstRect->right  += extent;
    pDstRect->bottom += pSrcRect->top;
  } else {                                      // horizontal: tracks 0,1
    pDstRect->left   += pSrcRect->left;
    pDstRect->top    += extent;
    pDstRect->right  += pSrcRect->left;
    pDstRect->bottom += extent;
  }
}
```

Parameters: `this` (`CBulanek *`, ECX), `pSrcRect` (`tagRECT *`, Stack[0x4]), `pDstRect` (`tagRECT *`, Stack[0x8]), `nTrackIdx` (`int`, Stack[0xC]).

### Caller excerpt — `CBulanek_StepMovementAndCollision@0x0041af70`

```c
CBulanek_OffsetCollisionRectByFacing
          (this, (tagRECT *)((int)this + 0x20), &originCollisionRect,
           *(int *)((int)this + 0x148));
/* scheduler gate may refresh +0xA4 from originRect axis */
CBulanek_OffsetCollisionRectByFacing
          (this, &originRect, &proposedCollisionRect, *(int *)((int)this + 0x148));
```

Second call is fully `tagRECT *`-clean. First call still casts `(tagRECT *)((int)this + 0x20)` because caller `this` types as `void *` (ECX retype blocked).

### Other callers (propagated)

`CBulanek_GetWorldCollisionRect@0x004195c0`:

```c
CBulanek_OffsetCollisionRectByFacing
          (this, (tagRECT *)&this->nOrigin_x, (tagRECT *)param_1,
           (this->videoTrackManager).nCurrentTrackIdx);
```

## Algorithm summary

Offsets collision rect `pDstRect` toward the walk-facing axis using grid-line coordinate `nFacingAxisExtent`:

| `nTrackIdx` | Facing | Added to dst horizontal edges | Added to dst vertical edges |
|-------------|--------|------------------------------|----------------------------|
| `0`, `1` (horizontal walk) | left/right | `pSrcRect->left` | `nFacingAxisExtent` |
| `2`, `3` (vertical walk) | up/down | `nFacingAxisExtent` | `pSrcRect->top` |

`nFacingAxisExtent` is refreshed in `StepMovementAndCollision` when `videoTrackManager.scheduler` slot 0 bit0 is armed: track `< 2` → `originRect.top`; else → `originRect.left`.

## Blockers / deferred

| Item | Reason |
|------|--------|
| `CBulanek_StepMovementAndCollision` `this` → `CBulanek *` in decompile | Ghidra MCP: `__thiscall` ECX auto-parameter cannot be retyped via API (`set_function_this_type` unavailable) |
| `CBulanek_GetWorldCollisionRect` `param_1` → `tagRECT *` | Out of scope; seed/caller rect typing for `OffsetCollisionRectByFacing` is fixed |
