# Round 9 `_Globals` FUN — Task 002 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 2 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | sim |
| **seed_address** | `0x0043d5b0` |
| **ghidra_name (before)** | `FUN_0043D5B0` |
| **prior_hint** | (empty) |

## Status

**DONE** — Renamed in Ghidra to `BlitOpaque_Indexed4_to_RGB565`; `save_program bulanci.exe` applied. Proof: `BlitTable_Opaque[20]` pointer, format-index closure (src 4bpp / dst RGB565), live disasm/decompile (nibble LUT + 565 pack), xref and sibling naming cluster.

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x0043d5b0` | `BlitOpaque_Indexed4_to_RGB565` | **Opaque blit kernel:** 4bpp indexed source → 16bpp RGB565 destination via caller palette LUT (`param_7`); `__cdecl`, 7 args, size `0xe3` | See below |

### BlitTable_Opaque dispatch (primary proof)

`BlitDispatch@0x004368d0` indexes `(dstFormat + srcFormat*8)` into `BlitTable_Opaque@0x004b08c8` ([sprite_container.md](../../formats/sprite_container.md), [round6_logic_task_38_report.md](../logic_recovery/round6_logic_task_38_report.md)).

**Ghidra `read_memory` @ `0x004b0914` (slots 19–22):**

| Table index | src fmt | dst fmt | Pointer VA | Ghidra / export |
|------------:|--------:|--------:|-----------:|-----------------|
| 19 | 2 (4bpp) | 3 (8bpp) | `0x0043d4f0` | `BlitOpaque_Indexed4_to_Indexed8` |
| **20** | **2 (4bpp)** | **4 (16bpp RGB565)** | **`0x0043d5b0`** | **`BlitOpaque_Indexed4_to_RGB565`** |
| 21 | 2 (4bpp) | 5 (24bpp) | `0x0043d6a0` | `Blit_Opaque_4bpp_24bpp` |

Format indices: `MapBitCountToFormat` — src **2** = 4bpp indexed; dst **4** = engine RGB565. Index `20 = 4 + 2*8`.

### Signature / size / cluster

| Source | Value |
|--------|-------|
| Ghidra (post-rename) | `BlitOpaque_Indexed4_to_RGB565(int * param_1, uint * param_2, int param_3, int param_4, int param_5, int param_6, int param_7)`; body `0043d5b0`–`0043d692` (**0xe3** B) |
| `config/bulanci/mapping.csv` | `FUN_0043d5b0`; size **`0xe3`**; 7× `int` + `uchar` return (stub) |
| Sibling | `BlitOpaque_Indexed1_to_RGB565@0x0043cf10` — same size **`0xe3`**, same 7-arg LUT pattern |
| `_Globals.cpp` export order | Between `BlitOpaque_Indexed4_to_Indexed8@0x0043d4f0` and `Blit_Opaque_4bpp_24bpp@0x0043d6a0` |

### Disasm (Ghidra)

Prolog: coord halving for 4bpp rects — `SHR EDX,0x1` / `SHR EAX,0x1` @ `0043d5bf`/`0043d5c3` (matches `BlitOpaque_Indexed4_to_Indexed8`, not 1bpp `SHR` by 3 @ `BlitOpaque_Indexed1_to_RGB565`).

Inner loop (nibble → LUT → RGB565):

- `MOV AL,byte ptr [ESI]`; `SHR AL,CL`; `AND EAX,0xf` — 4-bit index from source byte
- `ADD EAX,dword ptr [EBP+0x20]`; `MOV EAX,dword ptr [EAX]` — palette LUT via **`param_7`**
- `SHR AH,0x2`; `SHR EAX,0x3`; masks `0xf800` / `0x7ff`; `MOV word ptr [EDI],AX` — **RGB565** store (`puVar12` stride `+2`)

### Decompile (Ghidra)

Confirms: `pbVar11` nibble walk with `>> ((byte)iVar9 & 0x1f) & 0xf`, LUT `*(undefined4 *)((index)*4 + param_7)`, `ushort *` destination with 565 bit-pack in loop.

### Xrefs

| From | Type |
|------|------|
| `0x004b0918` | **DATA** — `BlitTable_Opaque[20]` function pointer |

No direct `CALL` (expected — dispatch via `BlitDispatch` table only).

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0043d5b0` → `BlitOpaque_Indexed4_to_RGB565` | Success |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — static BlitTable + disasm/decompile proof sufficient.

## Remaining UNK

- `mapping.csv` / `_Globals.cpp` / `_Globals.h` still list `FUN_0043d5b0` (separate mapping pass).
- Ghidra return type still `undefined` (siblings same); `mapping.csv` documents `uchar` — not changed this task.
- Decompiler local names for rect/stride temps (cosmetic).

## Cross-links

- [round5_worker_14_report.md](../struct_recovery/round5_worker_14_report.md) — prior blocked hint for this VA
- [main_menu.md](../../gameplay/main_menu.md) — opaque table routing example
