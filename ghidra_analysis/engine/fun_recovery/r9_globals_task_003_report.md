# Round 9 `_Globals` FUN — Task 003 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 3 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x0043dae0` |
| **ghidra_name (before)** | `FUN_0043DAE0` |
| **prior_hint** | (empty) |

## Status

**DONE** — Renamed to `BlitOpaque_RGB565_to_Indexed8` with BlitTable slot, format-index, disasm, and xref proof via Ghidra MCP.

## Function

| Address | Ghidra name (after) | Role | Evidence |
|---------|---------------------|------|----------|
| `0x0043dae0` | `BlitOpaque_RGB565_to_Indexed8` | **Opaque blit kernel:** 16 bpp RGB565 source → 8 bpp indexed destination; per-pixel reverse lookup in 256-entry `uint32` LUT at `param_8`; `__cdecl`, `uchar` return | See below |

### BlitTable_Opaque dispatch (primary proof)

`BlitDispatch@0x004368d0` indexes `(dstFormat + srcFormat*8)` into `BlitTable_Opaque@0x004b08c8` ([sprite_container.md](../../formats/sprite_container.md)).

| Source | Value |
|--------|-------|
| Table base | `0x004b08c8` |
| Slot index | **35** = `4*8 + 3` (src fmt **4**, dst fmt **3**) |
| Slot address | `0x004b0954` (`BlitTable_Opaque + 0x8C`) |
| Pointer (LE `read_memory`) | **`e0 da 43 00` → `0x0043dae0`** |
| Neighbor slots | `[34]=NULL`; `[36]=0x0043dbc0` (`BlitOpaque_RGB565_to_RGB565`) |

Format indices ([bmp_decoder.md](../../formats/bmp_decoder.md) / `MapBitCountToFormat`): **4** = engine 5-6-5 RGB565; **3** = 8 bpp indexed.

### Xrefs

| From | Type |
|------|------|
| `0x004b0954` | DATA (table entry only) |

**No direct callers** (`get_function_callers` empty) — reached only through `BlitDispatch` → `BlitTable_Opaque[35]`.

### Disasm (Ghidra)

| Pattern | Address | Meaning |
|---------|---------|---------|
| `MOV AX,word ptr [ESI]` / `ADD ESI,0x2` | `0x0043db4f` / `0x0043db94` | 16-bit RGB565 source step |
| `AND EAX,0xf800` / `SHL EAX,0x8` | `0x0043db56` / `0x0043db5b` | 5-6-5 red channel unpack |
| `AND EAX,0x7ff` / `SHL AX,0x3` / `SHL AH,0x2` | `0x0043db66`–`0x0043db6f` | green/blue unpack |
| `CMP EDX,0x100` / `CMP EAX,[EBX+EDX*4]` | `0x0043db80`–`0x0043db86` | 256-entry LUT scan (`EBX=param_8`) |
| `MOV byte ptr [EDI],AL` / `ADD EDI,0x1` | `0x0043db92` / `0x0043db97` | 8-bit dest write |

### Signature / size / cluster

| Source | Value |
|--------|-------|
| `config/bulanci/mapping.csv` | `uchar __cdecl`; size **`0xd2`**; `(int*, int*, int×4, uint, int)` |
| `_Globals.h` neighbors | `BlitOpaque_Indexed8_to_BGRA32@0x0043da40`; `BlitOpaque_RGB565_to_RGB565@0x0043dbc0` |
| `param_7` | Present in prototype; **not referenced** in decompiled body (UNK role) |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x0043dae0` | `uchar __cdecl BlitOpaque_RGB565_to_Indexed8(...)` |
| `rename_function_by_address` | `0x0043dae0` | `FUN_0043DAE0` → **`BlitOpaque_RGB565_to_Indexed8`** |
| `set_decompiler_comment` | `0x0043dae0` | Confirmed BlitTable slot 35 + LUT role |
| `force_decompile` | `0x0043dae0` | Refreshed pseudocode |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — static BlitTable + disasm proof sufficient.

## Remaining UNK

- `param_7` (`uint`): unused in function body; may be dispatcher padding or reserved for a future chroma path — not renamed or typed beyond `mapping.csv`.
- `_Globals.cpp` / `_Globals.h` / `mapping.csv` still list `FUN_0043dae0` (separate mapping export pass).

## Cross-links

- [r9_globals_task_002_report.md](r9_globals_task_002_report.md) — sibling opaque blit naming pattern
- [round6_logic_task_38_report.md](../logic_recovery/round6_logic_task_38_report.md) — `BlitDispatch` / table routing
