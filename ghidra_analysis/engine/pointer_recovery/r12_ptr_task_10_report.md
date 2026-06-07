# Round 12 — Pointer Recovery Task 10 Report

## Task

Fix `CBulanek` struct pointer arithmetic for the walk-embed field band consumed by `CBulanek_StepMovementAndCollision@0x0041af70` and `CBulanek_CopyCollisionRectLocal@0x00417410`.

## Status

**DONE** — Walk-embed band split from opaque byte blobs into six `int` fields @ `+0x104..+0x118`; post-walk transient dwords split @ `+0x15C/+0x160`; `pCachedTrackHolder` named @ `+0x164`. `save_program bulanci.exe`.

---

## Disasm proof (`search_instructions` @ `0x0041af70`)

| Offset | Address | Mnemonic | Operands | Role |
|--------|---------|----------|----------|------|
| `+0x104` | `0x0041afcb` | `SUB` | `EBX, dword ptr [ESI + 0x104]` | `dwParamA − embedBaseX` |
| `+0x108` | `0x0041afc5` | `SUB` | `EDX, dword ptr [ESI + 0x108]` | `dwParamB − embedBaseY` |
| `+0x10C` | `0x0041b067` | `ADD` | `ECX, dword ptr [ESI + 0x10c]` | read embed acc X into proposed rect |
| `+0x10C` | `0x0041b0af` | `MOV` | `dword ptr [ESI + 0x10c], EBX` | write clip-left accumulator |
| `+0x110` | `0x0041b051` | `ADD` | `EAX, dword ptr [ESI + 0x110]` | read embed acc Y into proposed rect |
| `+0x110` | `0x0041b0a5` | `MOV` | `dword ptr [ESI + 0x110], ECX` | write clip-top accumulator |
| `+0x114` | `0x0041b03b` | `MOV` | `dword ptr [ESI + 0x114], EAX` | store `−originRect.left` |
| `+0x118` | `0x0041b045` | `MOV` | `dword ptr [ESI + 0x118], EAX` | store `−originRect.top` |
| `+0x15C` | `0x0041b06f` | `ADD` | `ECX, dword ptr [ESI + 0x15c]` | transient pad into proposed left |
| `+0x15C` | `0x0041b0b8` | `MOV` | `dword ptr [ESI + 0x15c], EDI` | zero transient (EDI=0) |
| `+0x160` | `0x0041b04b` | `MOV` | `EAX, dword ptr [ESI + 0x160]` | read transient into proposed top |
| `+0x160` | `0x0041b0be` | `MOV` | `dword ptr [ESI + 0x160], EDI` | zero transient (EDI=0) |

**Not zeroed:** no `MOV [ESI+0x164]`..`[ESI+0x167]` in seed — only two dwords @ `+0x15C/+0x160` cleared.

### `pActiveAnim+4` / `+8` (anim chain offset)

| Step | Address | Instruction | Effect |
|------|---------|-------------|--------|
| load anim | `0x0041af84` | `MOV ECX, [ESI+0xf0]` | `pActiveAnim` |
| `+4` | `0x0041af90`–`0x0041af9a` | `ADD ECX,4` / `MOV ECX,[ECX]` | `deltaRect.right += anim[+4]` |
| `+8` | `0x0041af8a`–`0x0041af97` | holder `+4` path / `MOV EAX,[ECX+4]` after `ADD ECX,4` | `deltaRect.bottom += anim[+8]` |

---

## Cross-check: `CBulanek_CopyCollisionRectLocal@0x00417410`

| Offset | Address | Instruction |
|--------|---------|-------------|
| collision band | `0x00417410`–`0x0041742b` | `[ECX+0x74..0x80]` → `nCollisionLeft/Top/Right/Bottom` |
| `+0x10C` | `0x0041742e`, `0x0041743f` | `MOV EDX,[ECX+0x10c]` → add to left/right |
| `+0x110` | `0x00417436`, `0x00417448` | `MOV EDX/ECX,[ECX+0x110]` → add to top/bottom |

**Before:** `*(int *)(this->pReserved_preAmmo + 8)` / `+ 0xc` (opaque byte-array pointer math).

**After:** `this->nWalkEmbedAccX` / `this->nWalkEmbedAccY` (direct struct fields @ `+0x10C/+0x110`).

---

## Struct changes (`CBulanek`, 412 B)

### Removed / replaced

| Old field | Offset | Replacement |
|-----------|--------|-------------|
| `pReserved_preAmmo` `byte[24]` | `+0x104..+0x11B` | six `int` walk-embed fields (below) |
| `pPad_postWalkEmbed` / `pPad_weaponSync` `byte[4]` | `+0x15C` | `nWalkEmbedPadTop` `int` |
| (gap / unnamed) | `+0x160` | `nWalkEmbedPadLeft` `int` |
| unnamed `CBulanek *` | `+0x164` | `pCachedTrackHolder` `void *` |

### Added / renamed walk-embed band

| Offset | Type | Name | Disasm use |
|--------|------|------|------------|
| `+0x104` | `int` | `nWalkEmbedBaseX` | subtracted from `weapon->dwParamA` |
| `+0x108` | `int` | `nWalkEmbedBaseY` | subtracted from `weapon->dwParamB` |
| `+0x10C` | `int` | `nWalkEmbedAccX` | embed step accumulator; `CopyCollisionRectLocal` shift |
| `+0x110` | `int` | `nWalkEmbedAccY` | embed step accumulator; `CopyCollisionRectLocal` shift |
| `+0x114` | `int` | `nWalkClipNegX` | `−originRect.left` after intersect |
| `+0x118` | `int` | `nWalkClipNegY` | `−originRect.top` after intersect |
| `+0x15C` | `int` | `nWalkEmbedPadTop` | transient add then zero |
| `+0x160` | `int` | `nWalkEmbedPadLeft` | transient add then zero |
| `+0x164` | `void *` | `pCachedTrackHolder` | **not** zeroed in seed; `CBulanek_SyncWeaponWalkFromTrack@0x0041bf20` |

---

## Decompile improvement

### `CBulanek_CopyCollisionRectLocal@0x00417410` — **fixed**

```c
*outRect = *outRect + this->nWalkEmbedAccX;
outRect[1] = outRect[1] + this->nWalkEmbedAccY;
outRect[2] = outRect[2] + this->nWalkEmbedAccX;
outRect[3] = outRect[3] + this->nWalkEmbedAccY;
```

### `CBulanek_StepMovementAndCollision@0x0041af70` — partial

Prototype set to `uchar __thiscall CBulanek_StepMovementAndCollision(CBulanek *this)` but Ghidra MCP cannot retype `__thiscall` ECX `this`; decompile still shows `void *this` with `(int)this + 0x10c` style offsets. Struct fields are correct for any function with a typed `CBulanek *this` (e.g. `CopyCollisionRectLocal`). Plate/decompiler comments document intended `CBulanek*`.

---

## Ghidra actions

| Tool | Target |
|------|--------|
| `modify_struct_field` | `pPad_weaponSync` → `nWalkEmbedPadTop` `int` |
| `add_struct_field` | `nWalkEmbedPadLeft` `int` @ `+0x160` |
| `modify_struct_field` | unnamed @ `+0x164` → `pCachedTrackHolder` `void *` |
| (prior session) | six `int` walk-embed fields @ `+0x104..+0x118` replacing `pReserved_preAmmo` |
| `set_function_prototype` | `0x0041af70`, `0x00417410` |
| `force_decompile` | `0x0041af70`, `0x00417410` |
| `save_program` | `bulanci.exe` |

---

## Evidence links

- Algorithm pseudocode: [r11_ai_task_09_report.md](../ai_recovery/r11_ai_task_09_report.md)
- Struct doc baseline: [CBulanek.md](../struct_recovery/CBulanek.md) (`abReserved_preAmmo` / `abPad_postWalkEmbed` names superseded by fields above)
