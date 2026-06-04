# Round 9 `_Globals` FUN — Task 005 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 5 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim *(manifest label — VA is `BlitTable_DestKey` compositor kernel)* |
| **seed_address** | `0x0043e740` |
| **ghidra_name (before)** | `FUN_0043E740` |
| **prior_hint** | *(empty)* |

## Status

**DONE** — Renamed to `BlitDestKey_Indexed2_to_BGR24` with BlitTable slot, format-index, decompile, and sibling-kernel proof.

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x0043e740` | `BlitDestKey_Indexed2_to_BGR24` | **Dest-chroma blit kernel:** 2bpp packed source → 24bpp BGR destination via 4-entry LUT (`param_7`); skip pixel when 2-bit index `== param_8` (destination chroma key) | See below |

### BlitTable_DestKey dispatch (primary proof)

`BlitDispatch@0x004368d0` indexes `(dstFormat + srcFormat*8)` into `BlitTable_DestKey@0x004b09c8` ([sprite_container.md](../../formats/sprite_container.md), [round6_logic_task_38_report.md](../logic_recovery/round6_logic_task_38_report.md)).

Ghidra `read_memory@0x004b09c8` (little-endian pointers):

| Table index | src fmt | dst fmt | Pointer VA | Symbol |
|------------:|--------:|--------:|-----------:|--------|
| 12 | 1 (2bpp) | 4 (16bpp RGB565) | `0x0043e650` | `BlitChromaKey_Indexed2_to_RGB565` *(adjacent chroma band)* |
| **13** | **1 (2bpp)** | **5 (24bpp BGR)** | **`0x0043e740`** | **`BlitDestKey_Indexed2_to_BGR24` (this task)** |
| 14 | 1 (2bpp) | 6 (32bpp BGRA) | `0x0043e820` | `Blit_DestKey_2bpp_32bpp` |

Format indices from [bmp_decoder.md](../../formats/bmp_decoder.md) / `MapBitCountToFormat`: `1→0, 2→1, 4→2, 8→3, 16→4, 24→5, 32→6`. Src **1** = 2bpp indexed; dst **5** = engine 24bpp BGR.

**Xrefs (1):** `DATA` @ `0x004b09fc` = `BlitTable_DestKey + 0x34` (slot 13). No direct `CALL` — reached only via `BlitDispatch`.

### Decompile / disasm behavior proof

Live decompile matches opaque sibling `BlitOpaque_Indexed2_to_BGR24@0x0043d350` with dest-chroma guard added (same 2-bit unpack: `>> … & 3`, `>> 2` row stride, `(3 - (*param_2 & 3)) * 2` bit phase):

| Kernel | Chroma skip | LUT |
|--------|-------------|-----|
| `BlitOpaque_Indexed2_to_BGR24@0x0043d350` | none | always write `param_7[index*4]` |
| **`BlitDestKey_Indexed2_to_BGR24@0x0043e740`** | **`if (index != param_8)`** | same 3-byte BGR write |

1bpp dest-key analogue: `BlitDestKey_Indexed1_to_BGR24@0x0043e4a0` uses `& 1` / `>> 3` instead of `& 3` / `>> 2`.

### Signature / size / export order

| Source | Value |
|--------|-------|
| `config/bulanci/mapping.csv` | `uchar __cdecl(int* param_1, uint* param_2, int×5, uint param_8)`; size **`0xd3`** (211 B) |
| `_Globals.cpp` export order | Between `Blit_DestKey_1bpp_32bpp@0x0043e580` and `Blit_DestKey_2bpp_32bpp@0x0043e820` |
| Naming cluster | `BlitDestKey_Indexed1_to_BGR24`, `BlitDestKey_Indexed4_to_RGB565`, … — **2bpp→24bpp gap filled** |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0043e740` | `FUN_0043e740` → `BlitDestKey_Indexed2_to_BGR24` |
| `set_function_prototype` | `0x0043e740` | `uchar` + 8×`__cdecl` args (matches siblings) |
| `set_decompiler_comment` | `0x0043e740` | BlitTable slot 13 + dispatch note |
| `force_decompile` | `0x0043e740` | Refreshed pseudocode |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — static BlitTable + decompile/sibling proof sufficient.

## Remaining UNK

- `mapping.csv` / `_Globals.cpp` / `_Globals.h` stubs still list `FUN_0043e740` (separate mapping pass).
- Cosmetic decompiler return `(uchar)uVar7` at epilog (loop variable reuse).

## Cross-links

- [r9_globals_task_002_report.md](r9_globals_task_002_report.md) — opaque-table naming pattern (`BlitOpaque_*`)
- [round6_logic_task_38_report.md](../logic_recovery/round6_logic_task_38_report.md) — `BlitDispatch` + five tables
- [sprite_container.md](../../formats/sprite_container.md) — `BlitTable_DestKey` routing
