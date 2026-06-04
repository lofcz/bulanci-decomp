# Round 9 Other — Task 004 Report (`CDSJpegImage`)

## Task

| Field | Value |
|-------|-------|
| **id** | 4 |
| **round** | 9 |
| **unit** | other |
| **namespace** | `CDSJpegImage` |
| **seed_address** | `0x0046abd0` |
| **ghidra_name (before)** | `FUN_0046abd0` |
| **prior_hint** | R8 task 45 — PARTIAL, stale role “sample row merge”; decompiler comment wrongly cited `jccoefct.c` |
| **prior art** | [round8_fun_task_45_report.md](round8_fun_task_45_report.md), [round8_fun_task_46_report.md](round8_fun_task_46_report.md), [round7_fun_task_41_report.md](round7_fun_task_41_report.md) |

## Status

**DONE** — Live Ghidra MCP (`connect_instance("bulanci")`, 2026-06-04). Renamed **`FUN_0046abd0` → `create_context_buffer`**; behavior matches **IJG libjpeg-6b `jcprepct.c` `LOCAL create_context_buffer`** (compress `CONTEXT_ROWS` path). `CDSJpegImage.cpp` / `.h` body written from live decompile; `bulanci.exe` saved.

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x0046abd0` | **`create_context_buffer`** | **Context-row prep buffer setup:** `alloc_small` fake `JSAMPROW` ring (`5 × max_v_samp_factor × num_components`); per-component `alloc_sarray` (`3 × rgroup` rows); `memcpy` true row ptrs into middle of fake array; wraparound `fake[i]=true[i]`, `fake[4×rgroup+i]=true[2×rgroup+i]`; `prep->color_buf[ci]=fake+rgroup` | Live decompile + disasm; sole `CALL` from `jinit_c_prep_controller@0x0046ad3d` when `[downsample+8]!=0`; byte-level match to IJG `jcprepct.c` `create_context_buffer` ([libjpeg-turbo 2.1.5 `jcprepct.c`](https://github.com/libjpeg-turbo/libjpeg-turbo/blob/2.1.5/jcprepct.c)) |

### Signature / size

| Source | Value |
|--------|-------|
| Ghidra (post-rename) | `void __stdcall create_context_buffer(void)` — **`cinfo` in `EDI`** at entry (caller `jinit_c_prep_controller`) |
| Body | `0046abd0`–`0046acec` → **285 B** (`0x11d`) |
| `config/bulanci/mapping.csv` | `CDSJpegImage::FUN_0046abd0`; size **`0x11d`** (was erroneous `uchar` / `__stdcall`) |
| C++ port | `static void create_context_buffer(int* cinfo)` — explicit `cinfo` param for readability |

### IJG `jpeg_compress_struct` / prep field mapping (this binary)

| Offset | Role in this function |
|--------|------------------------|
| `cinfo+0x04` | `mem` → `alloc_small` / `alloc_sarray` vtable |
| `cinfo+0x3c` | `num_components` |
| `cinfo+0x44` | `comp_info` (stride **`0x54`** per component) |
| `cinfo+0xd8` | `max_h_samp_factor` (width divisor in `alloc_sarray` size) |
| `cinfo+0xdc` | `max_v_samp_factor` (`rgroup_height`) |
| `cinfo+0x144` | `prep` controller; `color_buf[]` at **`prep+0x08`** |

### Disasm anchors (live Ghidra)

| VA | Instruction | Proof |
|----|-------------|-------|
| `0x0046abd3` | `MOV EAX,[EDI+0x3c]` | `num_components` |
| `0x0046abda` | `MOV EBX,[EDI+0x144]` | `prep` |
| `0x0046abe1` | `MOV ESI,[EDI+0xdc]` | `max_v_samp_factor` |
| `0x0046abe7`–`0x0046abf7` | `IMUL` / `alloc_small` | fake size `num_comp × rgroup × 5 × 4` |
| `0x0046ac47`–`0x0046ac68` | width `IMUL`/`IDIV`, `alloc_sarray` | `3 × rgroup` row storage per component |
| `0x0046ac73` | `CALL 0x00447e90` | `memcpy(dst, true, rgroup×0xc)` — 3×rgroup row pointers |
| `0x0046ac95`–`0x0046acac` | wrap loop | `fake[4×rgroup+i]←true[2×rgroup+i]`; `fake[i]←true[i]` |
| `0x0046acc3` | `MOV [ECX],EBP` | `prep->color_buf[ci] = fake + rgroup` |
| `0x0046acbe` | `ADD [ESP+0x10],0x54` | `comp_info` stride |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole CODE)** | `0x0046ad3d` | `jinit_c_prep_controller` | Context-row branch: after `prep+4 = compress_output`, inline `CALL create_context_buffer` |
| **Homolog** | `0x00460e80` | `_Globals::create_context_buffer` | Decompress-side **split head** (R7 task 41, `0xaa` B) — different layout/offsets, same IJG source function family |

### Control flow

```
jinit_compress_master
  → jinit_c_prep_controller (need_context_rows)
       → prep+4 = compress_output
       → create_context_buffer  (this seed, EDI=cinfo)
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0046abd0` → `create_context_buffer` | Success |
| `set_function_prototype` | `void create_context_buffer(void)` + `__stdcall` | Success |
| `set_decompiler_comment` | `0x0046abd0` | Corrected `jcprepct.c` role (was stale `jccoefct.c`) |
| `set_plate_comment` | `0x0046abd0` | `libjpeg-6b jcprepct.c :: create_context_buffer` |
| `force_decompile` | `0x0046abd0` | Refreshed |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — static disasm/decompile + IJG source control-flow match sufficient.

## Remaining UNK

- Ghidra still shows `unaff_EDI` until register calling convention is modeled (PE passes `cinfo` in `EDI`; C++ port uses `int* cinfo` parameter).
- `mapping.csv` / `report.json` still list `FUN_0046abd0` until export regen (out of scope).
