# Round 9 `_Globals` FUN — Task 006 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 6 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x004438a0` |
| **ghidra_name (before)** | `FUN_004438A0` |
| **prior_hint** | *(empty)* |

## Status

**DONE** — Renamed to `BlitKeyAndMask_1bpp_to_BGRA32` with BlitTable slot proof, byte-for-byte sibling match to `BlitKeyAndMask_1bpp_to_BGR24`, and BGRA32 destination stride proof.

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x004438a0` | `BlitKeyAndMask_1bpp_to_BGRA32` | **Chroma+mask blit kernel:** 1bpp mask plane + 2-entry palette LUT (`param_7`) + per-pixel alpha (`*pbVar15 + (param_11 & 0xff)` clamped) → **32bpp BGRA** via `g_pAlphaBlendLut`; skip mask index `param_8`; `__cdecl` | See below |

### BlitTable_KeyAndMask dispatch (primary proof)

`BlitDispatch@0x004368d0` indexes `dstFormat + srcFormat*8` into `BlitTable_KeyAndMask@0x004b0cc8` when chroma+mask path is active ([sprite_container.md](../../formats/sprite_container.md)).

| Source | Value |
|--------|-------|
| **Table base** | `0x004b0cc8` (`BlitTable_KeyAndMask`) |
| **Pointer VA** | `0x004b0ce0` (sole xref — DATA) |
| **Slot index** | **6** (`(0x004b0ce0 - 0x004b0cc8) / 4`) |
| **src format** | **0** (engine 1bpp indexed / 2-entry palette per [bmp_decoder.md](../../formats/bmp_decoder.md)) |
| **dst format** | **6** (engine 32bpp BGRA: `32→6`) |

**Alias note:** `0x004b0ce0` is also `BlitTable_Masked[70]` (`0x004b0bc8 + 70*4`) — same DWORD in the adjacent 72-entry table block. `BlitDispatch` masked branch passes **10** stack args; KeyAndMask branch passes **11** (includes `param_11`). This kernel matches the **11-arg** KeyAndMask call pattern.

**Adjacent dst-format row (src format 0):**

| Slot | Table | dst | Pointer | Ghidra name |
|-----:|-------|----:|---------|-------------|
| 5 | KeyAndMask | 5 | `0x00443700` | `BlitKeyAndMask_1bpp_to_BGR24` |
| **6** | **KeyAndMask** | **6** | **`0x004438a0`** | **`BlitKeyAndMask_1bpp_to_BGRA32`** (this task) |

**Masked-only sibling (same src/dst indices, different table / arity):**

| Slot | Table | Pointer | Ghidra name |
|-----:|-------|---------|-------------|
| 6 | Masked | `0x00441830` | `BlitMasked_PAL1_to_BGRA32` (10-arg; alpha index = `*pbVar15` only) |

Ghidra `read_memory@0x004b0cc8` (first row): `20 35 44 00 | 00 37 44 00 | a0 38 44 00` → `0x00443520`, `0x00443700`, `0x004438a0`.

### 1bpp mask + alpha blend (behavior proof)

Shared mask walk with `BlitKeyAndMask_1bpp_to_BGR24@0x00443700`:

- Mask index: `*pbVar14 >> shift & 1`
- Phase init: `7 - (*param_2 & 7)`; decrement; reset to **7** when `< 1`
- Mask stride: `(*param_2 >> 3)` byte offset
- Skip when index `== param_8`
- Palette color: `*(uint *)(index * 4 + param_7)` (2-entry LUT)
- Alpha LUT index: `(uint)*pbVar15 + (param_11 & 0xff)`, clamp subtract to `0` when `< 0xff`

**Destination format delta vs `BlitKeyAndMask_1bpp_to_BGR24@0x00443700`:**

| Kernel | Row byte count | Dest pointer | Store |
|--------|----------------|--------------|-------|
| BGR24 @ `0x00443700` | `(x1-x0)*3` | `uint3 *` | 3-byte BGR + high byte via `g_pAlphaBlendLut` |
| **BGRA32 @ `0x004438a0`** | **`(x1-x0)*4`** | **`uint *`** | **full `uint` write (`local_30`, alpha in high byte)** |

### Signature / size / cluster

| Source | Value |
|--------|-------|
| `config/bulanci/mapping.csv` | `uchar __cdecl(int* param_1, uint* param_2, int×9)`; size **`0x18a`** (394 B) |
| `_Globals.h` / `_Globals.cpp` export order | Immediately after `BlitKeyAndMask_1bpp_to_BGR24@0x00443700` |
| Return at RET | Plain `RET` @ `0x00443a29` — no meaningful status byte (same as sibling blit kernels) |

### Xrefs

| From | Type |
|------|------|
| `0x004b0ce0` | **DATA** — `BlitTable_KeyAndMask[6]` (aliases `BlitTable_Masked[70]`) |

No direct `CALL` (expected — `BlitDispatch` table only).

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x004438a0` | `FUN_004438a0` → `BlitKeyAndMask_1bpp_to_BGRA32` |
| `set_function_prototype` | `0x004438a0` | `uchar __cdecl` + 11 params (matches `BlitKeyAndMask_1bpp_to_BGR24`) |
| `set_decompiler_comment` | `0x004438a0` | KeyAndMask slot 6 + sibling / PAL1 distinction |
| `force_decompile` | `0x004438a0` | Refreshed pseudocode |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — static BlitTable + decompile sibling match sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Engine consumer that sets **src format 8** (Masked index 70 alias) vs format 0 | **UNK** — same pointer; likely extended format row; not required for rename |
| `mapping.csv` / `_Globals.cpp` stub rename | Out of scope (FUN-only task) |

## Cross-links

- [round5_worker_14_report.md](../struct_recovery/round5_worker_14_report.md) — stale “BlitTable masked BGRA32” hint (superseded by KeyAndMask slot-6 proof)
- [r9_globals_task_007_report.md](./r9_globals_task_007_report.md) — parallel Masked-table alpha-blend naming pattern
