# Round 9 `_Globals` — Task 047 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 47 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec |
| **seed_address** | `0x00461460` |
| **ghidra_name** | `FUN_00461460` |
| **prior_hint** | R7 — decompress main buffer init |
| **prior art** | [round7_fun_task_45_report.md](round7_fun_task_45_report.md), [round8_fun_task_10_report.md](round8_fun_task_10_report.md), [round6_logic_task_44_report.md](../logic_recovery/round6_logic_task_44_report.md) |

## Status

**PARTIAL** — Live Ghidra MCP re-verify (`connect_instance("bulanci")`, 2026-06-04) confirms R7/R8 role, xref closure, and decompile. **No rename:** IJG `jcprepct.c` / `jdmainct.c` **LOCAL** decompress prep sample-buffer initializer; no standalone COFF/export symbol (distinct from `jinit_d_main_controller@0x00464230`). **`_Globals.cpp` body written** from live decompile (replaces `STUB_BODY`).

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x00461460` | `FUN_00461460` | **Decompress prep sample-buffer init:** `alloc_small(cinfo,1,0x50)` → `cinfo+0x184`; stores prep method head `LAB_004613e0@0x004613e0`; if `[cinfo+0x1a0]+8` (`need_context_rows`) calls `create_context_buffer` and uses `max_v_samp_factor+2` row height, else `max_v_samp_factor`; loops `num_components` over `comp_info` with `alloc_large` per component. `buffer_mode!=0` → `err->msg_code=4`; context rows with `max_v_samp_factor<2` → `msg_code=0x2f`. | Live decompile + disasm (`0xd3` B); **1×** `UNCONDITIONAL_CALL` @ `0x00460354` from `master_selection` when `[cinfo+0x41]==0`; callee `create_context_buffer@0x004614c6` |

### Signature / size

| Source | Value |
|--------|-------|
| Ghidra (live) | `void __cdecl FUN_00461460(int * cinfo, char buffer_mode)` |
| Body | `00461460`–`00461532` → **211 B** (`0xD3`) |
| `config/bulanci/mapping.csv` | `void __cdecl`; size **`0xd3`**; `int*`, `char` — matches Ghidra |
| `_Globals.cpp` | Body transcribed from live decompile (this session) |

### IJG `jpeg_decompress_struct` / prep field mapping (this binary)

| Offset | `cinfo[]` / prep | Role in this helper |
|--------|------------------|---------------------|
| `+0x24` | `[9]` | `num_components` |
| `+0x41` | byte | `raw_data_out` (caller gate) |
| `+0xc4` | `[0x31]` | `comp_info` base (stride `0x54`) |
| `+0x118` | `[0x46]` | `max_v_samp_factor` |
| `+0x184` | `[0x61]` | prep workspace (`cinfo->prep`) |
| `+0x1a0` | `[0x68]` | downsampler module pointer |
| `+0x1a0+8` | byte | `need_context_rows` |
| `prep+0x00` | — | method head → `0x004613e0` |
| `prep+0x08[ci]` | — | per-component `alloc_large` row buffer |

### Init order inside `master_selection` (live decompile @ `0x00460200`)

```
master_selection
  … jinit_d_coef_controller(cinfo, need_full_buffer)
  if ([cinfo+0x41]==0) FUN_00461460(cinfo, '\0')   // sole xref @ 0x00460354
  mem->realize_virt_arrays + entropy start_pass
```

### Disasm anchors (live Ghidra)

| VA | Instruction | Proof |
|----|-------------|-------|
| `0x0046146c` | `PUSH 0x50` | Workspace size 80 bytes |
| `0x0046147d` | `MOV [ESI+0x184], EBX` | Store prep at `cinfo+0x184` |
| `0x00461483` | `MOV [EBX], 0x4613e0` | Prep method table head |
| `0x0046148d` | `MOV [EDX+0x8], 4` | Error when `buffer_mode!=0` |
| `0x004614a4` | `CMP byte [EDX+0x8], 0` | `need_context_rows` on `[cinfo+0x1a0]` |
| `0x004614b5` | `MOV [EAX+0x8], 0x2f` | Error when `max_v_samp_factor<2` with context |
| `0x004614c6` | `CALL create_context_buffer` | Context row pointer tables |
| `0x00461519` | `CALL [mem+0x8]` | `alloc_large` per component |
| `0x00461526` | `ADD EDI, 0x54` | `comp_info` stride |

### Relationship to other IJG inits

| Function | VA | `cinfo` slot | Size | Notes |
|----------|-----|--------------|------|-------|
| `jinit_d_main_controller` | `0x00464230` | `+0x18c` | `0x1c` | Separate main-controller object |
| `jinit_d_coef_controller` | `0x00462370` | `+0x188` | `0x74` | Called **immediately before** this function |
| `FUN_00461460` | `0x00461460` | `+0x184` | `0x50` | This task — prep workspace + `0x4613e0` method |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole CODE)** | `0x00460354` | `master_selection` | `CALL FUN_00461460` after `jinit_d_coef_controller` when `[cinfo+0x41]==0` |
| **Callee (conditional)** | `0x004614c6` | `create_context_buffer` | When `need_context_rows` ([R9 task 41/44/46](r9_globals_task_046_report.md) cluster) |
| **Method install** | `0x00461483` | `LAB_004613e0` | Prep vtable head (R8 task 9) |

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
| `decompile_function` | `0x00461460` | Matches R7/R8; plate + decompiler comments present |
| `disassemble_function` | `0x00461460` | 62 instructions; anchors above |
| `get_xrefs_to` | `0x00461460` | 1× `UNCONDITIONAL_CALL` from `master_selection@0x00460354` |
| `decompile_function` | `0x00460200` | Caller chain confirms post-coef init call |

No Ghidra mutations this session (R7/R8 comments/prototype already correct).

## Frida

**none** — Single static caller, `jpeg_decompress_struct` field offsets, and allocator vtable calls are sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| IJG export symbol name | LOCAL init in `jcprepct.c` / `jdmainct.c` cluster; not in verified export table |
| `JERR_*` for codes `4` and `0x2f` | Stores proven; macro names not matched to `jerror.h` this session |
| `create_context_buffer` register convention | PE callee uses `ESI=cinfo`; C++ port calls stub until task 41 body wires `cinfo` param |
| `FUN_004613e0` / prep vtable method names | Prep `start_pass` homolog; R8 task 9 slice |

## Cross-links

- [round7_fun_task_45_report.md](round7_fun_task_45_report.md) — R7 carry-over
- [round8_fun_task_10_report.md](round8_fun_task_10_report.md) — R8 re-verify
- [r9_globals_task_046_report.md](r9_globals_task_046_report.md) — sibling jcprepct LOCAL cluster
- [round6_logic_task_44_report.md](../logic_recovery/round6_logic_task_44_report.md) — band slice
