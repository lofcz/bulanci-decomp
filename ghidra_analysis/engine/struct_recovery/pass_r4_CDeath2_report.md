# Pass R4 — `CDeath2` struct recovery

## 1. Scope

Map **`CDeath2`** (`sizeof 0xfc`): relation to **`CDeath`** corpse (`0x108`), tombstone placement at **`+0xf4/+0xf8`**, and **`CBulanek_OnDeath`** spawn/bind path. Apply **`set_function_this_type`** (mirror R4 `CDeath` pass).

**Inputs:** [CDeath.md](./CDeath.md), [CDeath2.md](./CDeath2.md), [round3_task_06_report.md](./round3_task_06_report.md)

**Seed addresses:** `0x00419aa0`, `0x0041a8c0`, `0x00417af0`, `0x0041f900`, `0x0041c860`, `0x0041bcb0`

## 2. Status

**DONE** — `CDeath2` Ghidra layout **252 B**; tail `pHost` / `nM_placementOffsetX/Y` named and typed; ctors moved to `CDeath2::`; `CBulanek_OnDeath` + `CBulanek_BindDeathTombstoneAnim` decompile with typed tombstone.

## 3. Evidence

### Size and sibling

| Type | Size | Alloc site | Ctor |
|------|------|------------|------|
| `CDeath` (corpse) | `0x108` | `CBulanek_OnDeath@0x0041f900` | `CDeath::CDeath_ctor` |
| `CDeath2` (tombstone) | `0xfc` | same, slots `0x20..0x23` | `CDeath2::CDeath2_ctor` |
| `CDeath2` (factory) | `0xfc` | `CreateObject@0x0041bcb0` | `CDeath2_SubobjectCtor` |

### `+0xf4` semantic split (R3 verified, R4 reconfirmed)

| Offset | `CDeath2` | `CDeath` |
|--------|-----------|----------|
| `+0xf4` | `nM_placementOffsetX` (int, zeroed) | `dwM_modeFlags` (`deathFlags & 3`) |
| `+0xf8` | `nM_placementOffsetY` | `dwM_animHeightBias` |
| `+0xfc` | *(struct end)* | `bM_tourneyFlag` |
| `+0x100/+0x104` | — | `nM_placementOffsetX/Y` on corpse |

### Struct layout (`get_struct_layout CDeath2`)

| Offset | Name | Type | Verified |
|--------|------|------|----------|
| `0xf0` (240) | `pHost` | `CBulanek *` | yes |
| `0xf4` (244) | `nM_placementOffsetX` | `int` | yes |
| `0xf8` (248) | `nM_placementOffsetY` | `int` | yes |
| total | — | `0xfc` | **252 bytes** |

### Decompile (post-`set_function_this_type`)

| Address | Symbol | Verified |
|---------|--------|----------|
| `0x00419aa0` | `CDeath2::CDeath2_ctor` | `pHost = victim`; `nM_placementOffsetX/Y = 0`; death vtables |
| `0x0041a8c0` | `CDeath2::CDeath2_SubobjectCtor` | same zero tail; no `pHost` (factory path) |
| `0x00417af0` | `CDeath2_UpdateStateFromParams` | `dwM_netStateWord0/1` @ `+0x68/+0x6c` |
| `0x0041c860` | `CBulanek_BindDeathTombstoneAnim` | `tombstone->nM_placementOffset*` negated into `nOrigin_*` / `nDraw_pos_*`; uses `tombstone->pHost` |
| `0x0041f900` | `CBulanek_OnDeath` | `OperatorNew(0x108)` corpse + optional `OperatorNew(0xfc)` → `CDeath2_ctor(this)` → bind → `pDeath2Tombstone` |

Placement xref closure: **writers** ctor/subobject only; **reader** tombstone bind only.

## 4. Ghidra deltas

- `get_struct_layout CDeath2` — 252 B
- `modify_struct_field` — `pM_pParent` → `pHost` (`CBulanek *`); `nM_placementOffsetX/Y` @ `+0xf4/+0xf8` (`int`)
- `set_function_this_type` `CDeath2 *` @ `0x00419aa0`, `0x0041a8c0` (`CDeath2_SubobjectCtor` switched to `__thiscall` first)
- `set_function_prototype` — `CDeath2_ctor`, `CDeath2_SubobjectCtor`, `CBulanek_BindDeathTombstoneAnim(CDeath2 *)`
- `force_decompile` @ `0x00419aa0`, `0x0041a8c0`, `0x0041c860`
- `save_program bulanci.exe`

## 5. Struct doc updates

- [CDeath2.md](./CDeath2.md) — R4 layout, xref table, OnDeath path, Ghidra apply block
- [CDeath.md](./CDeath.md) — cross-link unchanged (sibling table already documents offset split)

## 6. Remaining UNK

- Interior pads `pPad_*` and `+0x30..+0x67` drawable header
- `pM_pPalette` type refinement (`void *` in layout)
- `CDeath2_HideViewIfParentShown` inner-view calling convention
- `track_manager` field map — defer to `CAnim`
