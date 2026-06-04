# Round 9 `_Globals` FUN — Task 008 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 8 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x004442c0` |
| **ghidra_name (before)** | `FUN_004442C0` |
| **prior_hint** | *(empty)* |

## Status

**DONE** — Renamed to `BlitKeyAndMask_4bpp_to_BGRA32` with BlitTable slot, format-index arithmetic, decompile/sibling-kernel, and `BlitDispatch` call-site proof.

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x004442c0` | `BlitKeyAndMask_4bpp_to_BGRA32` | **Masked-table blit kernel:** 4bpp packed source nibbles + per-pixel mask plane + palette LUT (`param_7`); skip when nibble index `== param_8`; composite into **32bpp BGRA** dest via `g_pAlphaBlendLut` with extra bias `(param_11 & 0xff)` | See below |

### BlitTable_Masked dispatch (primary proof)

`CPoemScroller::BlitDispatch@0x004368d0` indexes `iVar10 = dest.nField_0c + src.nField_0c * 8` into five kernel tables ([sprite_container.md](../../formats/sprite_container.md), [round6_logic_task_38_report.md](../logic_recovery/round6_logic_task_38_report.md)). Mask-only sprites (`consumer+0x20 != NULL`, `consumer+0x18 == 0xFF`) use **`BlitTable_Masked@0x004b0bc8`**.

Ghidra `read_memory` (little-endian pointers):

| Table index | src fmt idx | dst fmt idx | Pointer VA | Symbol |
|------------:|------------:|------------:|-----------:|--------|
| 83 | 10 | 3 | `0x00443f40` | `BlitKeyAndMask4bpp` (RGB565 dest) |
| 84 | 10 | 4 | `0x00444120` | `BlitKeyAndMask_4bpp_to_BGR24` |
| 85 | 10 | 5 | `0x00000000` | *(null)* |
| **86** | **10** | **6** | **`0x004442c0`** | **`BlitKeyAndMask_4bpp_to_BGRA32` (this task)** |

**Xrefs (1):** `DATA` @ `0x004b0d20` = `BlitTable_Masked + 0x158` (slot 86). No direct `CALL` — reached only via `BlitDispatch`.

Dst format index **6** = engine 32bpp BGRA per [bmp_decoder.md](../../formats/bmp_decoder.md) (`MapBitCountToFormat`: `32→6`). Decompile writes 4-byte `uint` pixels (`puVar16`, `*puVar16 = local_30` with alpha in high byte).

### Decompile / disasm behavior proof

Live decompile matches masked **4bpp→BGR24** sibling with dest widened to BGRA32:

| Kernel | Dest stride | Chroma skip | Alpha |
|--------|-------------|-------------|-------|
| `BlitKeyAndMask_4bpp_to_BGR24@0x00444120` | `×3` (`uint3 *`) | `nibble != param_8` | `mask_byte + (param_11&0xff)` → LUT |
| **`BlitKeyAndMask_4bpp_to_BGRA32@0x004442c0`** | **`×4` (`uint *`)** | **same** | **same** |

Shared 4bpp unpack disasm: `SHR` nibble from `param_4` plane, `(1 - (*param_2 & 1)) * 4` bit phase, `AND 0xf`, `CMP` vs `param_8` @ `0x00444381`.

Contrast `BlitMasked_PAL4_to_BGRA32@0x004421d0` (10-arg, mask alpha only — no `param_11` bias).

### BlitDispatch masked call site

`BlitDispatch@0x00436baa` loads `EDX = [BlitTable_Masked + iVar10*4]`, then **`CALL EDX`** after **10** stack args (`ADD ESP, 0x28` @ `0x00436be8`). KeyAndMask branch @ `0x00436af9` pushes an 11th arg (`uVar9` chroma/alpha byte). This kernel’s 11th parameter is still listed in the prototype (matches `mapping.csv` / export cluster); when invoked from the masked path, `[EBP+0x30]` aliases the caller’s return-address byte (clamped @ entry `AND EAX,0xff`).

### Signature / size / export order

| Source | Value |
|--------|-------|
| `config/bulanci/mapping.csv` | `uchar __cdecl` + 11 args; size **`0x18a`** (394 B) |
| `_Globals.cpp` export order | Between `BlitKeyAndMask_4bpp_to_BGR24@0x00444120` and `BlitAlphaBlend_ByteIndex_RGB565@0x00444450` |
| Naming cluster | `BlitKeyAndMask4bpp`, `BlitKeyAndMask_4bpp_to_BGR24`, `BlitKeyAndMask_PAL8_to_BGRA32`, … |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x004442c0` | `FUN_004442c0` → `BlitKeyAndMask_4bpp_to_BGRA32` |
| `set_function_prototype` | `0x004442c0` | `uchar` + 11×`__cdecl` args |
| `set_decompiler_comment` | `0x004442c0` | BlitTable slot 86 + dispatch note |
| `force_decompile` | `0x004442c0` | Refreshed pseudocode |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — static BlitTable + decompile/sibling proof sufficient.

## Remaining UNK

- **Source format index 10** semantic: index arithmetic proven (`86 = 10×8 + 6`); not produced by `MapBitCountToFormat` (`1..32 → 0..7` only). Which `CDSImage`/FLX path sets `nField_0c == 10` is not traced in this task.
- `mapping.csv` / `_Globals.cpp` / `_Globals.h` stubs still list `FUN_004442c0` (separate mapping pass).
- Masked-path 10-arg vs 11-param prototype mismatch at runtime (documented above; behavior still uses `param_11` when present).

## Cross-links

- [r9_globals_task_002_report.md](r9_globals_task_002_report.md) — opaque-table naming pattern
- [r9_globals_task_005_report.md](r9_globals_task_005_report.md) — BlitTable slot + format-index proof template
- [round6_logic_task_38_report.md](../logic_recovery/round6_logic_task_38_report.md) — `BlitDispatch` + five tables
- [sprite_container.md](../../formats/sprite_container.md) — `BlitTable_Masked` routing (39/130 sprites)
