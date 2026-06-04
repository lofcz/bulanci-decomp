# Round 9 Other unit — Task 005 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 5 |
| **round** | 9 |
| **unit** | other |
| **namespace** | `CDSJpegImage` |
| **band** | jpeg_codec |
| **seed_address** | `0x0046b590` |
| **ghidra_name (before)** | `FUN_0046b590` |
| **prior_hint** | R6 logic task 44: callee #2 of `jinit_compress_master` when `!raw_data_in`; R8 cluster renamed all METHODDEF targets this function installs |
| **prior art** | [round6_logic_task_44_report.md](../logic_recovery/round6_logic_task_44_report.md); [round8_fun_task_47_report.md](round8_fun_task_47_report.md)–50 (downsample METHODDEF renames); [r9_globals_task_060_report.md](r9_globals_task_060_report.md) (picker install sites) |

## Status

**DONE** — Live Ghidra MCP (`connect_instance bulanci`, 2026-06-04). **Renamed** `FUN_0046b590` → **`jinit_downsampler`**: IJG `jcsample.c` `GLOBAL jinit_downsampler` proven by `jinit_compress_master` callee order, `0x34`-byte downsampler alloc at `cinfo+0x154`, `sep_downsample` vtable head, and per-component method-picker branches matching stock 6b source. Ghidra prototype + decompiler comment; `save_program bulanci.exe`.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x0046b590` | `FUN_0046b590` | **`jinit_downsampler`** | IJG compressor downsampler init: allocate `jpeg_downsampler` (`0x34` B), set `downsample` method + `need_context_rows`, pick per-component `METHODDEF` downsample | Live disasm/decompile; sole caller; pointer stores to R8-renamed `_Globals::*_downsample` |

### IJG correspondence (`jcsample.c`, 6b)

| Check | Match |
|-------|-------|
| Source symbol | `GLOBAL(void) jinit_downsampler(j_compress_ptr cinfo)` |
| Alloc | `(*cinfo->mem->alloc_small)(cinfo, JPOOL_IMAGE, SIZEOF(struct jpeg_downsampler))` ↔ `PUSH 0x34` / `CALL [cinfo->mem]` |
| Store downsampler | `cinfo->downsample = downsample` ↔ `MOV [cinfo+0x154], EAX` (`cinfo[0x55]`) |
| Head methods | `downsample->downsample = sep_downsample` ↔ `MOV [EBX+4], 0x46adf0` (`sep_downsample`) |
| `need_context_rows` | `FALSE` until smooth methods ↔ `MOV byte [EBX+8], 0` then set `1` when smooth picker fires |
| Component loop | `for (ci = 0; ci < num_components; ci++)` ratio branches for fullsize / h2v1 / h2v2 / int | Disasm picker @ `0x0046b600`–`0x0046b6cb` |

### Method pointer installs (disasm)

| VA | Condition | Target | IJG role |
|----|-----------|--------|----------|
| `0x0046b5b5` | always | `0x0046adf0` | `sep_downsample` (dispatch head) |
| `0x0046b630` | `h==max_h && v==max_v`, `smoothing_factor==0` | `0x0046afb0` | `fullsize_downsample` (`LAB`, not yet renamed) |
| `0x0046b621` | same + `smoothing_factor!=0` | `0x0046b400` | `fullsize_smooth_downsample` |
| `0x0046b651` | `h*2==max_h && v==max_v` | `0x0046b000` | `h2v1_downsample` |
| `0x0046b67f` | `h*2==max_h && v*2==max_v`, no smooth | `0x0046b0b0` | `h2v2_downsample` |
| `0x0046b673` | `h*2==max_h && v*2==max_v`, smooth | `0x0046b180` | IJG `h2v2_smooth_downsample` (Ghidra still misnamed `h2v2_fancy_upsample`) |
| `0x0046b69c` | integral ratios | `0x0046ae80` | `int_downsample` |
| `0x0046b5cf` / `0x0046b6a6` | smoothing unsupported / bad ratios | `msg_code` `0x19` / `0x26` → `error_exit` | IJG `ERREXIT` paths |

### Field mapping (this binary)

| Offset | Role |
|--------|------|
| `cinfo+0x154` / `cinfo[0x55]` | `downsample` pointer |
| `cinfo+0xb3` | `smoothing_factor` high byte (INPUT_SMOOTHING gate) |
| `cinfo+0xb4` / `cinfo[0x2d]` | `smoothing_factor` (picker) |
| `cinfo+0xd8` / `cinfo[0x36]` | `max_h_samp_factor` |
| `cinfo+0xdc` / `cinfo[0x37]` | `max_v_samp_factor` |
| `cinfo+0x3c` / `cinfo[0xf]` | `num_components` |
| `cinfo+0x44` / `cinfo[0x11]` | `comp_info` array |
| `compptr+0x8` / `[-4]` from `+0xc` | `h_samp_factor` |
| `compptr+0xc` | `v_samp_factor` |
| downsampler `+0` | `start_pass` (shared 1-byte RET `0x00467430`, Ghidra label `CDSApp_PreCreateHook`) |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole)** | `0x00460de0` | `CDSJpegImage::jinit_compress_master` | `CALL` when `(char)cinfo[0x2c]==0` (`!raw_data_in`) — matches `jcinit.c` order: color converter → **downsampler** → prep controller |
| **DATA refs (outbound)** | `0x0046b5b5` … `0x0046b69c` | `_Globals::*_downsample`, `LAB_0046afb0` | Method picker stores only |

### `jinit_compress_master` excerpt (post-rename decompile)

```c
if ((char)param_1[0x2c] == '\0') {
  jinit_color_converter(param_1);
  jinit_downsampler(param_1);
  jinit_c_prep_controller(param_1,'\0');
}
```

### mapping.csv (stale)

```
;CDSJpegImage::FUN_0046b590;0x46b590;0x147;__cdecl;;uchar;int*
```

Size `0x147` (327 B) matches Ghidra body `0046b590`–`0046b6d6`. Return type **`uchar`** is wrong — IJG/Ghidra: **`void`**.

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0046b590` | `FUN_0046b590` → **`jinit_downsampler`** |
| `set_function_prototype` | `0x0046b590` | `void __cdecl jinit_downsampler(int *cinfo)` |
| `set_decompiler_comment` | `0x0046b590` | IJG role + sole caller + picker |
| `force_decompile` | `0x0046b590` | OK — displays `CDSJpegImage::jinit_downsampler` |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — Static `jinit_compress_master` chain + METHODDEF install table sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| `start_pass` @ downsampler `+0` (`0x00467430`) | Shared 1-byte RET; Ghidra mislabels `CDSApp_PreCreateHook` — linker dedupe, not app hook (see R6 task 48) |
| `0x0046b180` smooth 2h2v slot | Ghidra name `h2v2_fancy_upsample` likely wrong for compress picker; IJG `h2v2_smooth_downsample` — separate rename task |
| `LAB_0046afb0` | `fullsize_downsample` not yet a named function |
| `mapping.csv` uchar return | Stale — void per IJG/Ghidra |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [GHIDRA_MCP.md](../GHIDRA_MCP.md)
- [ijg_version_byte_match.md](../../formats/ijg_version_byte_match.md) — outlined `jinit_downsampler` vs inlined METHODDEF bodies
- [round6_logic_task_44_report.md](../logic_recovery/round6_logic_task_44_report.md) — compress-master callee list
