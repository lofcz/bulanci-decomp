# Round 9 Other unit — Task F (006) Report

## Task

| Field | Value |
|-------|-------|
| **id** | 6 |
| **round** | 9 |
| **unit** | Other |
| **namespace** | `CDSJpegImage` |
| **seed_address** | `0x0046c8f0` |
| **ghidra_name (before)** | `FUN_0046c8f0` |
| **prior_hint** | R5/R6/R8 — plate `jinit_compress_master` helper; R9 globals tasks 61–62 documented vtable slots from this body |

## Status

**DONE** — Live Ghidra MCP (`connect_instance bulanci`, 2026-06-04). Renamed to upstream IJG **`jinit_c_master_control`** (`jcmaster.c`): `alloc_small(0x20)` → `cinfo+0x13c`, installs `prepare_for_pass` / `pass_startup` / `finish_pass` vtable; `initial_setup` + `jpeg_validate_script` branch; `transcode_only` sets `pass_type`; `total_passes` scales with `optimize_coding`. Sole caller `jinit_compress_master@0x00460dc8`. `save_program bulanci.exe` applied.

## Function

| Address | Ghidra (after) | Role | Evidence |
|---------|----------------|------|----------|
| `0x0046c8f0` | **`jinit_c_master_control`** | **IJG compress master controller init:** allocate `jpeg_comp_master` (0x20 B) via `cinfo->mem->alloc_small(cinfo, JPOOL_PERMANENT=1, 0x20)`; wire method pointers; run `initial_setup`; default or validate scan script; set `optimize_coding` when progressive; initialize `pass_type` / `pass_number` / `scan_number` / `total_passes` from `transcode_only` and `num_scans` | See below |

### Caller xref (install site)

| From | Type | Notes |
|------|------|-------|
| `jinit_compress_master@0x00460dc8` | **CALL** | First callee in compressor init chain (`jcinit.c` order); `jinit_c_master_control(cinfo, FALSE)` — `PUSH 0` + `CALL` before color/downsample/DCT modules |

Decompile @ `0x00460dc0`: `jinit_c_master_control((int)param_1,'\0');` then `jinit_color_converter` / `FUN_0046b590` / … per [round6_logic_task_44_report.md](../logic_recovery/round6_logic_task_44_report.md).

### `jpeg_comp_master` vtable install (disasm proof)

| Slot offset | VA | Ghidra name | IJG `jpeg_comp_master` field |
|-------------|-----|-------------|------------------------------|
| `[0]` | `0x0046c690` | `prepare_for_pass` | `prepare_for_pass` (R9 task 61) |
| `[1]` | `0x0046c850` | `pass_startup` | `pass_startup` (R9 task 62) |
| `[2]` | `0x0046c880` | `FUN_0046c880` | `finish_pass` — out of scope |

| VA | Instruction | Proof |
|----|-------------|-------|
| `0x0046c8fc`–`0x0046c901` | `PUSH 0x20; PUSH 1; PUSH ESI; CALL [mem->alloc_small]` | `alloc_small(cinfo, 1, 0x20)` |
| `0x0046c905` | `MOV [ESI+0x13c], EDI` | `cinfo->master = block` |
| `0x0046c910` | `MOV [EDI], 0x46c690` | slot `[0]` |
| `0x0046c916` | `MOV [EDI+4], 0x46c850` | slot `[1]` |
| `0x0046c91d` | `MOV [EDI+8], 0x46c880` | slot `[2]` |
| `0x0046c924` | `MOV byte [EDI+0xd], 0` | `call_pass_startup = FALSE` |

### Callee chain

| Callee | VA | When |
|--------|-----|------|
| `initial_setup` | `0x0046bdf0` | Always (`CALL` @ `0x0046c927`) |
| `jpeg_validate_script` | `0x0046bfd0` | `cinfo->scan_info != NULL` (`cinfo+0xac` @ `0x0046c932`) |

### Control flow ↔ IJG `jinit_c_master_control` (libjpeg-6b `jcmaster.c`)

| Step | Bulanci field / offset | Proof |
|------|------------------------|-------|
| No `scan_info` | `cinfo+0xac == 0` → `progressive_mode=0` @ `+0xd4`, `num_scans=1` @ `+0xa8` | `0x0046c93b`–`0x0046c941` |
| Has `scan_info` | `CALL jpeg_validate_script` | `0x0046c934` |
| Progressive → optimize | `cinfo+0xd4 != 0` → `optimize_coding=1` @ `+0xb2` | `0x0046c94b`–`0x0046c953` |
| `transcode_only == 0` | `master+0x10` (`pass_type`) = **0** (`main_pass`) | `0x0046c95a`–`0x0046c972` |
| `transcode_only != 0` | `pass_type = 2 - optimize_coding` → **1** (`huff_opt_pass`) or **2** (`output_pass`) | `NEG`/`SBB`/`ADD 2` @ `0x0046c960`–`0x0046c96d` |
| Counters cleared | `master+0x14`, `master+0x1c` = 0 (`pass_number`, `scan_number`) | `0x0046c975`–`0x0046c978` |
| `total_passes` | `master+0x18` = `num_scans` or `2*num_scans` if `optimize_coding` | `0x0046c97b`–`0x0046c998` |

Upstream source: [libjpeg `jcmaster.c` `jinit_c_master_control`](https://github.com/libjpeg-turbo/ijg/blob/main/jcmaster.c) — same alloc, vtable trio, `initial_setup`, scan-script branch, transcode `pass_type`, and `total_passes` scaling (libjpeg-6b omits newer `block_size`/`arith_code` optimize hack present in later IJG).

**Not** `jinit_compress_master` — that name belongs to the orchestrator @ `0x00460dc0` which calls this function first.

### Signature / size

| Source | Value |
|--------|-------|
| Ghidra (post-rename) | `void __cdecl jinit_c_master_control(int * cinfo, char transcode_only)`; body `0046c8f0`–`0046c99e` (**0xaf** B) |
| `config/bulanci/mapping.csv` | `CDSJpegImage::FUN_0046c8f0`; size **`0xaf`**; `__cdecl`; `uchar` return *(incorrect — body is void)* |

### `_Globals.cpp` stub (not trusted)

```cpp
uchar CDSJpegImage::FUN_0046c8f0(int param_1, char param_2) { STUB_BODY(); return 0; }
```

(`src/bulanci/CDSJpegImage.cpp` @ `0x0046c8f0`)

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0046c8f0` → `jinit_c_master_control` | Success |
| `set_function_prototype` | `void __cdecl jinit_c_master_control(int * cinfo, char transcode_only)` | Success |
| `set_decompiler_comment` | Entry | IJG role + caller + field offsets |
| `force_decompile` | `0x0046c8f0` | Refreshed |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — static disasm/decompile + sole caller + IJG `jcmaster.c` export match sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| `mapping.csv` / `CDSJpegImage.cpp` still list `FUN_0046c8f0` | Separate mapping pass |
| Vtable slot `[2]` `FUN_0046c880` (`finish_pass`) | Separate FUN task |
| `initial_setup` / `jpeg_validate_script` use `__stdcall` in Ghidra | Cosmetic — callees take `cinfo` via established convention |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [r9_globals_task_061_report.md](r9_globals_task_061_report.md) — `prepare_for_pass` slot `[0]`
- [r9_globals_task_062_report.md](r9_globals_task_062_report.md) — `pass_startup` slot `[1]`
- [round6_logic_task_44_report.md](../logic_recovery/round6_logic_task_44_report.md) — compress-master callee order
