# Round 9 `_Globals` FUN — Task 007 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 7 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x00443db0` |
| **ghidra_name (before)** | `FUN_00443db0` |
| **prior_hint** | *(empty)* |

## Status

**DONE** — Renamed to `BlitAlphaBlend_2bitIndex_BGRA32` with BlitTable_Masked slot proof, 2bpp mask disasm/decompile match vs sibling `BlitAlphaBlend_2bitIndex_BGR24`, and BGRA32 destination stride proof.

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x00443db0` | `BlitAlphaBlend_2bitIndex_BGRA32` | **Masked blit kernel:** 2bpp mask plane + palette LUT (`param_7`) + per-pixel alpha plane (`param_9`/`param_10`) → **32bpp BGRA** destination via `g_pAlphaBlendLut`; skip mask index `param_8`; `__cdecl` leaf | See below |

### BlitTable_Masked dispatch (primary proof)

`BlitDispatch@0x004368d0` indexes `dstFormat + srcFormat*8` into `BlitTable_Masked@0x004b0bc8` ([sprite_container.md](../../formats/sprite_container.md), [round6_logic_task_38_report.md](../logic_recovery/round6_logic_task_38_report.md)).

| Source | Value |
|--------|-------|
| **Table base** | `0x004b0bc8` (`BlitTable_Masked`) |
| **Pointer VA** | `0x004b0d00` (sole xref — DATA) |
| **Slot index** | **78** (`(0x004b0d00 - 0x004b0bc8) / 4`) |
| **src format** | **9** (`78 = 9*8 + 6`) |
| **dst format** | **6** (engine 32bpp BGRA per `MapBitCountToFormat` / [status.md](../../formats/status.md): `32→6`) |

**Adjacent row (same src format 9):**

| Slot | dst | Pointer | Ghidra name |
|-----:|----:|---------|-------------|
| 76 | 4 | `0x00443a30` | `BlitKeyAndMask2bpp` (RGB565 dst, `ushort *`) |
| 77 | 5 | `0x00443c10` | `BlitAlphaBlend_2bitIndex_BGR24` |
| **78** | **6** | **`0x00443db0`** | **`BlitAlphaBlend_2bitIndex_BGRA32`** (this task) |

Ghidra `read_memory@0x004b0cf8`: `30 3a 44 00 | 10 3c 44 00 | b0 3d 44 00` → `0x00443a30`, `0x00443c10`, `0x00443db0`.

### 2bpp mask + alpha blend (behavior proof)

Shared mask walk with slot-77 sibling:

- Mask index: `*pbVar14 >> shift & 3` (2 bits per pixel)
- Phase init: `(3 - (*param_2 & 3)) * 2`; decrement shift by 2; reset to **6** when `< 2`
- Skip when index `== param_8` (transparent / key index)
- Palette color: `*(uint *)(index * 4 + param_7)`
- Alpha: `*pbVar15 + (param_11 & 0xff)` clamped via `g_pAlphaBlendLut`

**Destination format delta vs `BlitAlphaBlend_2bitIndex_BGR24@0x00443c10`:**

| Kernel | Row byte count | Dest pointer | Store |
|--------|----------------|--------------|-------|
| BGR24 @ `0x00443c10` | `(x1-x0)*3` | `uint3 *` | 3-byte BGR + separate high byte |
| **BGRA32 @ `0x00443db0`** | **`(x1-x0)*4`** | **`uint *`** | **full `uint` write (`local_30`, alpha in high byte)** |

### Signature / size / cluster

| Source | Value |
|--------|-------|
| `config/bulanci/mapping.csv` | `uchar __cdecl(int* param_1, uint* param_2, int×9)`; size **`0x18c`** (396 B) |
| `_Globals.cpp` export order | Between `BlitAlphaBlend_2bitIndex_BGR24@0x00443c10` and `BlitKeyAndMask4bpp@0x00443f40` |
| Return at RET | Plain `RET` @ `0x00443f39` — no EAX status (same as sibling blit kernels; `mapping.csv` `uchar` is stub convention) |

### Xrefs

| From | Type |
|------|------|
| `0x004b0d00` | **DATA** — `BlitTable_Masked[78]` |

No direct `CALL` (expected — `BlitDispatch` table only).

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x00443db0` → `BlitAlphaBlend_2bitIndex_BGRA32` | Success (PascalCase warnings only) |
| `set_decompiler_comment` | `0x00443db0` | Slot 78 / fmt 9→6 / LUT+alpha plane |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — static table index + decompile parity with named sibling sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Engine **format index 9** on-disk name (beyond BMP `MapBitCountToFormat` 0–6) | **UNK** — proven only as blit-table **src row 9** shared with `BlitKeyAndMask2bpp` / `BlitAlphaBlend_2bitIndex_BGR24`; not renamed in FLX docs |
| `mapping.csv` / `_Globals.cpp` / `_Globals.h` stub sync | Out of scope (FUN-only task) |
| Ghidra return type `uchar` vs decompiler `void` | Cosmetic — RET does not set AL |

## Cross-links

- [sprite_container.md](../../formats/sprite_container.md) — five blit tables, mask-plane selection
- [round6_logic_task_38_report.md](../logic_recovery/round6_logic_task_38_report.md) — `BlitDispatch` index formula
- [r9_globals_task_002_report.md](r9_globals_task_002_report.md) — prior Round 9 BlitTable_Opaque rename pattern
