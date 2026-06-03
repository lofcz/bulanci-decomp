# Round 7 FUN — Task 45 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 45 |
| **round** | 7 |
| **band** | dispatch |
| **seed_address** | `0x00461460` |
| **title** | FUN recovery: FUN_00461460 @ 0x00461460 (xrefs=1) |
| **prior_hint** | R6 task 44 — decompress buffer init; plate from `_apply_r6_dispatch_codec.py` |

## Status

**PARTIAL** — Decompress-path sample-buffer initializer at `cinfo+0x184` proven via live Ghidra xrefs, decompile, and disasm. Corrects stale decompiler text that called this a “compressor pass.” **No unique IJG export symbol** (distinct from `jinit_d_main_controller@0x00464230`); kept `FUN_00461460`.

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x00461460` | `FUN_00461460` | **libjpeg-6b decompress sample-buffer init (LOCAL):** `alloc_small(cinfo,1,0x50)` → `cinfo+0x184`; stores method pointer `0x004613e0` at workspace base; if module at `cinfo+0x1a0` has `need_context_rows` (`byte+8`), calls `create_context_buffer` (`FUN_00460e80`) and uses `max_v_samp_factor+2` (`cinfo+0x118`) for row height, else `max_v_samp_factor`; loops `num_components` (`cinfo+0x24`) over `comp_info` (`cinfo+0xc4`) with `alloc_large` per component. `param_2!=0` → `err->msg_code=4` (`JERR_*` macro UNK). `max_v_samp_factor<2` with context rows → `msg_code=0x2f`. | **Xref:** sole `UNCONDITIONAL_CALL` from `FUN_00460200@0x00460354` when `[cinfo+0x41]==0` (not `raw_data_out`). **Disasm:** `mov [esi+0x184],ebx` @ `0x46147d`; `mov [ebx],0x4613e0` @ `0x461483`; `cmp [edx+0x8],0` on `[esi+0x1a0]` @ `0x4614a4`; `call 0x00460e80` @ `0x4614c6`; component loop `edi+=0x54` @ `0x461526`. **Decompile:** post-`jinit_d_coef_controller` in `FUN_00460200` before `realize_virt_arrays` / entropy `start_pass`. **R6:** [round6_logic_task_44_report.md](../logic_recovery/round6_logic_task_44_report.md) slice row; neighbor `jinit_d_coef_controller` → `cinfo+0x188`. |

### Init order inside `FUN_00460200` (decompress master body)

```
FUN_00460200 (jinit_d_post_controller init tail)
  … jinit_color_deconverter / jinit_upsampler / jinit_d_main_controller@0x464230 …
  jinit_d_coef_controller(cinfo, need_full_buffer)
  if (!raw_data_out) FUN_00461460(cinfo, '\0')   // sole xref @ 0x460354
  mem->realize_virt_arrays + entropy start_pass
```

### Disasm proof (selected)

| VA | Instruction | Meaning |
|----|-------------|---------|
| `0x0046146c` | `PUSH 0x50` | Workspace size 80 bytes |
| `0x0046147d` | `MOV [ESI+0x184], EBX` | Store at decompress `cinfo+0x184` |
| `0x00461483` | `MOV [EBX], 0x4613e0` | First vtable / method slot |
| `0x0046148d` | `MOV [EDX+0x8], 4` | Error when `param_2!=0` |
| `0x0046149e` | `MOV EDX, [ESI+0x1a0]` | Module pointer (upsampler slot post-init) |
| `0x004614a4` | `CMP byte [EDX+0x8], 0` | `need_context_rows` gate |
| `0x004614b5` | `MOV [EAX+0x8], 0x2f` | `JERR_*` when `max_v_samp_factor<2` |
| `0x004614c6` | `CALL 0x00460e80` | Context row pointer tables |
| `0x00461519` | `CALL [mem+0x8]` | `alloc_large` per component |

### Relationship to other IJG inits

| Function | VA | `cinfo` slot | Size | Notes |
|----------|-----|--------------|------|-------|
| `jinit_d_main_controller` | `0x00464230` | `+0x18c` | `0x1c` | Separate main-controller object; called earlier in same parent |
| `jinit_d_coef_controller` | `0x00462370` | `+0x188` | `0x74` | Called **immediately before** `FUN_00461460` |
| `FUN_00461460` | `0x00461460` | `+0x184` | `0x50` | This task — sample-buffer workspace + `0x4613e0` method |

Not called from `jinit_compress_master` / `jinit_c_main_controller` (compress path uses `cinfo+0x140` / `0x40`-byte controller).

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `connect_instance` | `bulanci` | 202 tools registered |
| `set_decompiler_comment` | `0x00461460` | Decompress-path proof; removes compressor mislabel |
| `set_plate_comment` | `0x00461460` | Parameters / Algorithm / Returns / Caller sections |
| `force_decompile` | `0x00461460` | Shows `create_context_buffer()` callee (Ghidra rename of `FUN_00460e80`) |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — Single static caller, `jpeg_decompress_struct` field offsets, and allocator calls are sufficient; no gameplay state.

## Remaining UNK

| Item | Reason |
|------|--------|
| IJG export symbol name | LOCAL init in `jdmainct.c` / shared `jcprepct.c`-style helpers; not in `mapping.csv` or `jpeg_decoder.md` verified table |
| Whether to merge with `jinit_d_main_controller` | Compiler may have split objects; three distinct slots (`+0x184`, `+0x18c`, `+0x188`) argue against merge |
| `FUN_004613e0` / `FUN_00461080` / `FUN_00461160` labels | Method at `0x4613e0` is code, not a data vtable; full prep-controller method names deferred (out of scope) |
| `JERR_*` for codes `4` and `0x2f` | Stores proven; macro names not matched to `jerror.h` this session |

## Cross-links

- [round6_logic_task_44_report.md](../logic_recovery/round6_logic_task_44_report.md) — band slice including `0x00461460`
- [round6_logic_task_43_report.md](../logic_recovery/round6_logic_task_43_report.md) — `FUN_00460200` parent
- [round7_fun_task_32_report.md](round7_fun_task_32_report.md) — `decompress_data` on `cinfo+0x188` coef path
- [jpeg_decoder.md](../../formats/jpeg_decoder.md) — libjpeg-6b identity
