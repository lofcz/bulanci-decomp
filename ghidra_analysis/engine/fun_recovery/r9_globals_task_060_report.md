# Round 9 `_Globals` — Task 060 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 60 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec |
| **seed_address** | `0x0046b400` |
| **ghidra_name (before)** | `FUN_0046B400` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R8 FUN cluster (`round8_fun_task_47_report.md` `sep_downsample`; `round8_fun_task_50_report.md` `h2v2_downsample`); sibling `h2v2_fancy_upsample@0x0046b180` in same picker |

## Status

**DONE** — Live Ghidra MCP (`connect_instance bulanci`, 2026-06-04). **Renamed** `FUN_0046B400` → **`fullsize_smooth_downsample`**: IJG `jcsample.c` `METHODDEF fullsize_smooth_downsample` (INPUT_SMOOTHING_SUPPORTED) proven by downsampler method-picker install site + smoothing-scale algebra + 3-tap horizontal loop. **WRITE:** `src/bulanci/_Globals.cpp` body from IJG/decompile match. Ghidra prototype, decompiler comment, `save_program bulanci.exe`.

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x0046b400` | **`fullsize_smooth_downsample`** | **1:1 component downsample with input smoothing:** 3×3 neighbor-weighted horizontal filter per row; `expand_right_edge` prelude; used when component `h/v` equals `max_h/max_v` and `smoothing_factor != 0` | Live disasm/decompile; install @ `FUN_0046b590`; IJG 6b source match |

### IJG correspondence (`jcsample.c`, 6b)

| Check | Match |
|-------|-------|
| Source symbol | `METHODDEF(void) fullsize_smooth_downsample(j_compress_ptr cinfo, jpeg_component_info *compptr, JSAMPARRAY input_data, JSAMPARRAY output_data)` |
| Smoothing scales | `memberscale = 65536 - sf*512` ↔ decompile `(0x80-sf)*0x200`; `neighscale = sf*64` ↔ decompile `sf*0x40` |
| Context rows | Picker sets `downsample+8` (`need_context_rows`) when smooth method installed |
| Edge expand | `expand_right_edge(input_data-1, max_v_samp_factor+2, image_width, output_cols)` ↔ `CALL expand_right_edge@0x0046ada0` |
| Loop | `for (outrow = 0; outrow < compptr->v_samp_factor; outrow++)` 3-column first/middle/last tap pattern |

### Install site (sole xref)

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Reference (sole DATA)** | `0x0046b621` | `CDSJpegImage::FUN_0046b590` | `mov [methods+ci], fullsize_smooth_downsample` when `comp.h_samp==max_h && comp.v_samp==max_v && cinfo+0xb4!=0` |
| **Call chain** | `0x00460df3` | `jinit_compress_master` | `CALL FUN_0046b590` on compress path when `cinfo[0x2c]==0` |
| **Sibling (no smooth)** | `0x0046b615` | `LAB_0046afb0` | Same 1:1 branch when `cinfo+0xb4==0` — `fullsize_downsample` homolog (`jcopy_sample_rows` + `expand_right_edge`) |
| **Dispatch head** | `0x0046b5b5` | `sep_downsample@0x0046adf0` | `downsample+4` per-component caller |

Picker decompile excerpt:

```c
if ((iVar1 == iVar2) && (*piVar4 == param_1[0x37])) {
  if (param_1[0x2d] == 0) {
    *puVar5 = &LAB_0046afb0;
  } else {
    *puVar5 = fullsize_smooth_downsample;
    *(undefined1 *)(puVar3 + 2) = 1;   /* need_context_rows */
  }
}
```

(`param_1[0x2d]` aliases `cinfo+0xb4` smoothing_factor.)

### Field mapping (this binary)

| Offset | Role |
|--------|------|
| `cinfo+0xb4` / `cinfo[0x2d]` | `smoothing_factor` |
| `cinfo+0xdc` | `image_width` |
| `compptr+0xc` | `v_samp_factor` (row loop bound) |
| `compptr+0x1c` | `width_in_blocks` → `output_cols = *8` (DCTSIZE) |

### Disassembly highlights

| VA | Proof |
|----|-------|
| `0x0046b42f` | `CALL expand_right_edge@0x0046ada0` |
| `0x0046b43a`–`0x0046b453` | `memberscale=(0x80-EAX)*0x200`; `neighscale=EAX*0x40` |
| `0x0046b450` | Loop bound `[EBP+0xc]` = `v_samp_factor` |
| `0x0046b4cc` / `0x0046b51d` | `(result + 0x8000) >> 16` fixed-point rounding |
| `0x0046b40e` | `width_in_blocks` @ `[compptr+0x1c]` → `*8` inner count |

**Size:** 397 B (`0x18d`), matches `mapping.csv`.

### Decompile (Ghidra, post-rename)

Core scale + first-column tap:

```c
expand_right_edge(input_data - 1, ..., cinfo->image_width + 2);
memberscale = (0x80 - cinfo->smoothing_factor) * 0x200;
neighscale  = cinfo->smoothing_factor * 0x40;
/* per outrow: above/in/below rows; 3-tap horizontal smooth-downsample */
```

### mapping.csv (reference — return type was wrong)

```
;_Globals::FUN_0046b400;0x46b400;0x18d;__cdecl;;uchar;int;int;uint*;int
```

Ghidra corrected to `void __cdecl fullsize_smooth_downsample(int cinfo, int compptr, uint * input_data, int output_data)`.

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0046b400` | `FUN_0046b400` → **`fullsize_smooth_downsample`** |
| `set_function_prototype` | `0x0046b400` | `void __cdecl fullsize_smooth_downsample(...)` |
| `set_decompiler_comment` | `0x0046b400` | IJG role + install site + sibling |
| `force_decompile` | `0x0046b400` | OK |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — Static picker install + IJG algorithm match sufficient.

## Remaining UNK

| Item | Status |
|------|------|
| `LAB_0046afb0` | Not a Ghidra function — create/rename `fullsize_downsample` is separate task |
| `h2v2_fancy_upsample@0x0046b180` | **Misnamed in R8** — same picker installs it for 2h2v + smoothing; IJG name is `h2v2_smooth_downsample` (out of scope) |
| `expand_right_edge@0x0046ada0` | Still `STUB_BODY()` — callee 4-arg IJG LOCAL; 2-arg decl in header incomplete |
| objdiff byte match | Per `ijg_version_byte_match.md`, outlined METHODDEF bodies need port+objdiff pipeline |
| `mapping.csv` uchar return | Stale — void per IJG/Ghidra |

## WRITE deliverable

| File | Change |
|------|--------|
| `src/bulanci/_Globals.cpp` | `fullsize_smooth_downsample` — IJG `fullsize_smooth_downsample` body (smoothing scales + 3-tap column loops) |
| `include/bulanci/_Globals.h` | Decl updated to `void fullsize_smooth_downsample(...)` |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [ijg_version_byte_match.md](../../formats/ijg_version_byte_match.md)
- [round8_fun_task_47_report.md](round8_fun_task_47_report.md) — `sep_downsample` picker head
- [round8_fun_task_50_report.md](round8_fun_task_50_report.md) — sibling downsample rename precedent
- [r9_globals_task_044_report.md](r9_globals_task_044_report.md) — jcprepct WRITE precedent
