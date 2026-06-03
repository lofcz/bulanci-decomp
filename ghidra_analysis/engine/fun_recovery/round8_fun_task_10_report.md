# Round 8 FUN — Task 10 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 10 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x00461460` |
| **title** | FUN recovery: FUN_00461460 @ 0x00461460 (xrefs=1) |
| **prior_hint** | R7 task 45 — decompress main buffer init |
| **carry_over** | [round7_fun_task_45_report.md](round7_fun_task_45_report.md) |

## Status

**PARTIAL** — Live Ghidra MCP re-verify (`connect_instance bulanci`, 2026-06-03) confirms R7 role and xref closure. **No rename:** IJG `jcprepct.c` / `jdmainct.c` **LOCAL** decompress prep sample-buffer initializer; no standalone COFF/export symbol (distinct from `jinit_d_main_controller@0x00464230`). Name stays `FUN_00461460`.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x00461460` | `FUN_00461460` | **`FUN_00461460`** | **libjpeg-6b decompress prep sample-buffer init** — `alloc_small(cinfo,1,0x50)` → `cinfo+0x184`; stores prep method table `FUN_004613e0@0x004613e0`; if `[cinfo+0x1a0]+8` (`need_context_rows`) calls `create_context_buffer` and uses `max_v_samp_factor+2` row height, else `max_v_samp_factor`; loops `num_components` over `comp_info` with `alloc_large` per component. `buffer_mode!=0` → `err->msg_code=4`; context rows with `max_v_samp_factor<2` → `msg_code=0x2f`. | `void __cdecl FUN_00461460(int *cinfo, char buffer_mode)`; body `0xd3` B (00461460–00461532); 1× CODE xref; disasm + decompile match R7; callee `create_context_buffer@0x00460e80` (R7/R8 task 5) |

### IJG field mapping (this binary)

| Offset | IJG role |
|--------|----------|
| `cinfo+0x24` | `num_components` |
| `cinfo+0x41` | `raw_data_out` (caller gate) |
| `cinfo+0xc4` | `comp_info` (stride `0x54`) |
| `cinfo+0x118` | `max_v_samp_factor` |
| `cinfo+0x184` | prep workspace (`cinfo->prep`) |
| `cinfo+0x1a0+8` | `downsample.need_context_rows` |
| `prep+0x00` | first method slot → `0x004613e0` |
| `prep+0x08[ci]` | per-component `alloc_large` row buffer |

### Init order inside `master_selection`

```
master_selection @ 0x00460200 (jdmaster.c body)
  … jinit_d_coef_controller(cinfo, need_full_buffer) @ 0x00460345
  if ([cinfo+0x41]==0) FUN_00461460(cinfo, '\0')   // sole xref @ 0x00460354
  mem->realize_virt_arrays + entropy start_pass
```

### Disasm proof (selected)

| VA | Instruction | Meaning |
|----|-------------|---------|
| `0x0046146c` | `PUSH 0x50` | Workspace size 80 bytes |
| `0x0046147d` | `MOV [ESI+0x184], EBX` | Store prep object at decompress `cinfo+0x184` |
| `0x00461483` | `MOV [EBX], 0x4613e0` | Prep method table head |
| `0x0046148d` | `MOV [EDX+0x8], 4` | Error when `buffer_mode!=0` |
| `0x004614a4` | `CMP byte [EDX+0x8], 0` | `need_context_rows` gate on `[cinfo+0x1a0]` |
| `0x004614b5` | `MOV [EAX+0x8], 0x2f` | `JERR_*` when `max_v_samp_factor<2` |
| `0x004614c6` | `CALL create_context_buffer` | Context row pointer tables |
| `0x00461519` | `CALL [mem+0x8]` | `alloc_large` per component |
| `0x00461526` | `ADD EDI, 0x54` | `comp_info` stride loop |

### Relationship to other IJG inits

| Function | VA | `cinfo` slot | Size | Notes |
|----------|-----|--------------|------|-------|
| `jinit_d_main_controller` | `0x00464230` | `+0x18c` | `0x1c` | Separate main-controller object; called earlier in same parent |
| `jinit_d_coef_controller` | `0x00462370` | `+0x188` | `0x74` | Called **immediately before** this function |
| `FUN_00461460` | `0x00461460` | `+0x184` | `0x50` | This task — prep workspace + `0x4613e0` method |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole CODE)** | `0x00460354` | `master_selection` | `CALL FUN_00461460` after `jinit_d_coef_controller` when `[cinfo+0x41]==0` |
| **Callee (conditional)** | `0x004614c6` | `create_context_buffer` | When `need_context_rows`; R7/R8 task 5 |
| **Method install** | `0x00461483` | `FUN_004613e0` | Prep vtable head stored at workspace base (R8 task 9 slice) |

Caller disasm @ `0x00460354` (live MCP):

```
00460345  CALL jinit_d_coef_controller
0046034d  CMP byte ptr [ESI+0x41], BL    ; raw_data_out
00460350  JNZ 0x0046035c
00460354  CALL 0x00461460                ; buffer_mode=0 (EBX cleared)
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `connect_instance` | `bulanci` | 202 tools registered |
| `set_function_prototype` | `0x00461460` | `void __cdecl FUN_00461460(int *cinfo, char buffer_mode)` (was `undefined` / `uchar`) |
| `set_plate_comment` | `0x00461460` | Caller updated to `master_selection`; callee `create_context_buffer` |
| `set_decompiler_comment` | `0x00461460` | Caller xref + jcprepct/jdmainct LOCAL note |
| `force_decompile` | `0x00461460` | Refreshed; param names `cinfo` / `buffer_mode` |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — Single static caller, `jpeg_decompress_struct` field offsets, and allocator calls are sufficient; no gameplay state.

## Remaining UNK

| Item | Reason |
|------|--------|
| IJG export symbol name | LOCAL init in `jcprepct.c` / `jdmainct.c` cluster; not in `mapping.csv` verified table |
| Whether to merge with `jinit_d_main_controller` | Three distinct slots (`+0x184`, `+0x18c`, `+0x188`) argue against merge |
| `FUN_004613e0` / prep vtable method names | Prep `start_pass` homolog; R8 task 9 slice |
| `JERR_*` for codes `4` and `0x2f` | Stores proven; macro names not matched to `jerror.h` this session |
| `mapping.csv` / `_Globals.cpp` | Still `FUN_00461460` / `uchar` return — export regen out of scope |

## Cross-links

- [round7_fun_task_45_report.md](round7_fun_task_45_report.md) — R7 carry-over
- [round7_fun_task_41_report.md](round7_fun_task_41_report.md) — `create_context_buffer` callee
- [round6_logic_task_44_report.md](../logic_recovery/round6_logic_task_44_report.md) — band slice
- [round8_fun_task_06_report.md](round8_fun_task_06_report.md) — sibling jcprepct LOCAL tail
