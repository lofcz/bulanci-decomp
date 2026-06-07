# Round 12 — Pointer Task 02 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 2 |
| **title** | `CBulanek_CopyCollisionRectLocal` — walk-embed pointer arithmetic |
| **function** | `CBulanek_CopyCollisionRectLocal@0x00417410` |
| **seed** | `CBulanek_StepMovementAndCollision@0x0041af70` |

## Status

**DONE** — `pReserved_preAmmo byte[24]` at `CBulanek+0x104` replaced with six named `int` walk-embed fields; `outRect` retyped `tagRECT *`; decompile uses `nWalkEmbedAccX` / `nWalkEmbedAccY` at `+0x10C` / `+0x110`. `CBulanek` size remains **412 B**. Program saved.

## Issue (pre-fix)

Decompiler treated `pReserved_preAmmo` as a pointer and emitted bogus pointer arithmetic:

```c
*outRect = *outRect + *(int *)(this->pReserved_preAmmo + 8);
outRect[1] = outRect[1] + *(int *)(this->pReserved_preAmmo + 0xc);
```

Disassembly reads **absolute** `CBulanek` offsets `+0x10C` and `+0x110`, not byte-indexed through a pointer.

## Disassembly evidence (`CBulanek_CopyCollisionRectLocal`)

### Collision copy (local band @ `+0x74..+0x80`)

| Address | Instruction | Meaning |
|---------|-------------|---------|
| `0x00417410` | `MOV EDX, [ECX+0x74]` | `nCollisionLeft` |
| `0x00417419` | `MOV EDX, [ECX+0x78]` | `nCollisionTop` |
| `0x0041741f` | `MOV EDX, [ECX+0x7c]` | `nCollisionRight` |
| `0x00417425` | `MOV EDX, [ECX+0x80]` | `nCollisionBottom` |

### `outRect` writes (`EAX` = stack arg)

| Address | Instruction | `tagRECT` field |
|---------|-------------|-----------------|
| `0x00417413` | `MOV EAX, [ESP+0x4]` | param pointer |
| `0x00417417` | `MOV [EAX], EDX` | `left` |
| `0x0041741c` | `MOV [EAX+0x4], EDX` | `top` |
| `0x00417422` | `MOV [EAX+0x8], EDX` | `right` |
| `0x0041742b` | `MOV [EAX+0xc], EDX` | `bottom` |

### Walk-embed accumulator adds

| Address | Instruction | Absolute offset |
|---------|-------------|-----------------|
| `0x0041742e` | `MOV EDX, [ECX+0x10c]` | `nWalkEmbedAccX` |
| `0x00417434` | `ADD [EAX], EDX` | add to `left` |
| `0x00417436` | `MOV EDX, [ECX+0x110]` | `nWalkEmbedAccY` |
| `0x0041743c` | `ADD [EAX+0x4], EDX` | add to `top` |
| `0x0041743f` | `MOV EDX, [ECX+0x10c]` | `nWalkEmbedAccX` (right) |
| `0x00417445` | `ADD [EAX+0x8], EDX` | add to `right` |
| `0x00417448` | `MOV ECX, [ECX+0x110]` | `nWalkEmbedAccY` (bottom) |
| `0x0041744e` | `ADD [EAX+0xc], ECX` | add to `bottom` |

## Caller evidence (`CBulanek_StepMovementAndCollision@0x0041af70`)

| Call site | Setup | Stack slot |
|-----------|-------|------------|
| `0x0041b09c` | `LEA EAX, [ESP+0x4c]` → `PUSH EAX` | `&originCollisionRect` (`tagRECT`) |
| `0x0041b0c4` | `LEA EDX, [ESP+0x3c]` → `PUSH EDX` | `&proposedCollisionRect` (`tagRECT`) |

Seed also reads/writes embed band at `+0x104`, `+0x108`, `+0x10C`, `+0x110`, `+0x114`, `+0x118` (e.g. `MOV [ESI+0x10c], EAX` @ `0x0041b0a1`, `SUB EBX, [ESI+0x104]` @ `0x0041afcb`).

## Ghidra mutations

| Action | Target | Result |
|--------|--------|--------|
| `modify_struct_field` | `pReserved_preAmmo` → `int[6]` / `pWalkEmbed` | Interim — array index `[2]`/`[3]` |
| `modify_struct_field` | `offset:0x104..0x118` → six `int` fields | Named walk-embed band |
| `set_function_prototype` | `0x00417410` → `void __thiscall CBulanek_CopyCollisionRectLocal(CBulanek *this, tagRECT *outRect)` | Success |
| `force_decompile` | `0x00417410`, `0x0041af70`, `0x004195c0` | Refreshed |
| `save_program` | `bulanci.exe` | Saved |

### `CBulanek` walk-embed fields (`+0x104..+0x11B`)

| Offset | Name | Seed usage |
|--------|------|------------|
| `+0x104` | `nWalkEmbedBaseX` | `SUB` weapon track anchor (`0x0041afcb`) |
| `+0x108` | `nWalkEmbedBaseY` | `SUB` weapon track anchor (`0x0041afc5`) |
| `+0x10C` | `nWalkEmbedAccX` | `CopyCollisionRectLocal` add; seed `MOV [ESI+0x10c]` |
| `+0x110` | `nWalkEmbedAccY` | `CopyCollisionRectLocal` add; seed `MOV [ESI+0x110]` |
| `+0x114` | `nWalkClipNegX` | seed `MOV [ESI+0x114]` (`-origin.left`) |
| `+0x118` | `nWalkClipNegY` | seed `MOV [ESI+0x118]` (`-origin.top`) |

## Post-fix decompile (verified)

### `CBulanek_CopyCollisionRectLocal@0x00417410`

```c
void __thiscall CBulanek::CBulanek_CopyCollisionRectLocal(CBulanek *this, tagRECT *outRect)
{
  outRect->left   = this->nCollisionLeft;
  outRect->top    = this->nCollisionTop;
  outRect->right  = this->nCollisionRight;
  outRect->bottom = this->nCollisionBottom;
  outRect->left   = outRect->left   + this->nWalkEmbedAccX;
  outRect->top    = outRect->top    + this->nWalkEmbedAccY;
  outRect->right  = outRect->right  + this->nWalkEmbedAccX;
  outRect->bottom = outRect->bottom + this->nWalkEmbedAccY;
}
```

### Seed call sites (excerpt)

```c
CBulanek_CopyCollisionRectLocal(this, &originCollisionRect);
...
CBulanek_CopyCollisionRectLocal(this, &proposedCollisionRect);
```

## Notes

- **Do not `remove_struct_field` on `pReserved_preAmmo`** — Ghidra collapses tail layout (412→388 B). Safe path: `modify_struct_field` same-size `int[6]`, then `offset:0x10c`/`0x110` naming.
- Replacing with nested `CBulanekWalkEmbed` struct also collapsed layout in trial; flat `int` fields at absolute offsets retained 412 B.
- `CBulanek_StepMovementAndCollision` still types `this` as `void*` (Ghidra `__thiscall` ECX API limit); embed accesses there remain `(int)this+0x10c` until `set_function_this_type` pass.

## Correlation

- [r11_ai_task_09_report.md](../ai_recovery/r11_ai_task_09_report.md) — algorithm doc for walk-embed band
- [r12_ptr_task_05_report.md](./r12_ptr_task_05_report.md) — `tagRECT` stack typing in same seed
