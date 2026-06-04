# Round 9 `_Globals` FUN — Task 004 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 4 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x0043e040` |
| **ghidra_name (before)** | `FUN_0043E040` |
| **prior_hint** | (empty) |

## Status

**DONE** — Renamed to `BlitOpaque_BGR24_to_BGRA32` with BlitTable_Opaque slot + format-index + disasm proof (Ghidra MCP live).

## Function

| Address | Ghidra name (after) | Role | Evidence |
|---------|---------------------|------|----------|
| `0x0043e040` | `BlitOpaque_BGR24_to_BGRA32` | **Opaque blit kernel:** 24bpp BGR source (3-byte stride) → 32bpp BGRA destination (4-byte stride); zero high byte (alpha=0); `__cdecl`, void return | See below |

### BlitTable_Opaque dispatch (primary proof)

`BlitDispatch@0x004368d0` indexes `(dstFormat + srcFormat*8)` into `BlitTable_Opaque@0x004b08c8` ([sprite_container.md](../../formats/sprite_container.md), [round6_logic_task_38_report.md](../logic_recovery/round6_logic_task_38_report.md)).

| Field | Value |
|-------|-------|
| Table base | `0x004b08c8` |
| Slot index | **46** = `dst_fmt 6 + src_fmt 5 × 8` |
| Slot VA | `0x004b0980` |
| Pointer stored | **`0x0043e040`** |

`read_memory@0x004b0970` (32 B):

| Slot | Index (src→dst) | Pointer | Named neighbor |
|-----:|-----------------|---------|----------------|
| 44 | 4→4 | `0x0043dec0` | `Blit_Opaque_24bpp_16bpp` |
| 45 | 5→5 | `0x0043df90` | `BlitOpaque_BGR24_to_BGR24` |
| **46** | **5→6** | **`0x0043e040`** | **this task** |
| 47 | 5→7 | `0x0043e0f0` | *(out of scope)* |

Format indices from [bmp_decoder.md](../../formats/bmp_decoder.md) / `MapBitCountToFormat`: `24→5`, `32→6`.

### Xrefs

| From | Type | Note |
|------|------|------|
| `0x004b0980` | DATA | Sole xref — `BlitTable_Opaque[46]`; no direct `CALL` |

### Disasm / algorithm

- Src pointer: `param_4 + rect_top*param_5 + left*3` (`ADD ESI,3` per pixel).
- Dst pointer: `param_3 + rect_top*param_6 + left*4` (`ADD EDI,4` per pixel).
- Inner loop @ `0x0043e0bc`: `XOR EAX,EAX` / `MOV AL,[ESI+2]` / `SHL EAX,16` / `MOV AX,[ESI]` / `MOV [EDI],EAX` — packs B,G,R into dword with **alpha byte 0**.
- Row advance: src `+ param_5 + (width_delta)*-3`; dst `+ param_6 + (width_delta)*-4`.

### Signature / size / cluster

| Source | Value |
|--------|-------|
| `config/bulanci/mapping.csv` | `uchar __cdecl(int*×2, int×4)`; size **`0xa7`** (167 B) |
| `_Globals.cpp` export order | Between `Blit_Opaque_24bpp_16bpp@0x0043dec0` and `BlitOpaque_BGRA32_to_Palette8@0x0043e0f0` |
| Sibling naming | Matches `BlitOpaque_BGR24_to_BGR24@0x0043df90` (slot 45) |

Decompiler body matches sibling opaque 24bpp kernels (rect clip, nested row/col loops); differs from `Blit_Opaque_24bpp_16bpp` only in dst stride (×4 vs ×2) and lack of RGB565 bit-pack.

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0043e040` → `BlitOpaque_BGR24_to_BGRA32` | Success (`_Globals` namespace) |
| `set_decompiler_comment` | `0x0043e040` | BlitTable slot 46 + format proof |
| `force_decompile` | `0x0043e040` | Symbol + comment refreshed |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — static table pointer + disasm sufficient.

## Remaining UNK

- `mapping.csv` / `_Globals.cpp` / `_Globals.h` still list `FUN_0043e040` (separate mapping pass).
- Slot 47 pointer `0x0043e0f0` naming vs format index 5→7 not verified this task.

## Cross-links

- [r9_globals_task_002_report.md](r9_globals_task_002_report.md) — BlitTable_Opaque index formula + format map
- [sprite_container.md](../../formats/sprite_container.md) — five blit tables @ `0x004b08c8`
- [main_menu.md](../../gameplay/main_menu.md) — opaque dispatch example (src=5,dst=5)
