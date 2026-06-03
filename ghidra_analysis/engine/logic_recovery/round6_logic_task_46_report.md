# Round 6 logic — Task 46 Report

## 1. Task

| Field | Value |
|-------|-------|
| **id** | 46 |
| **title** | Logic dispatch_45a_468: 0x00463fe0–0x00465a60 (18 funcs) |
| **range** | `dispatch_45a_468` (`0x0045a`–`0x00468`) |
| **seed_address** | *(none — slice task)* |

**Band note (re-verified):** Manifest `range_note` (“input, network, UI dispatch”) is **stale for this slice**. All 18 VAs lie in the embedded **libjpeg-6b** decompress/color/upsample/quantize cluster used by `CDSJpegImage` (same band as [round6_logic_task_43_report.md](round6_logic_task_43_report.md) and [round5_worker_15_report.md](../struct_recovery/round5_worker_15_report.md)).

## 2. Status

**PARTIAL** — Full per-function roles proven by **PE disassembly** (`orig/bulanci.exe`, capstone) + pointer/xref scan + prior round-43/5 docs. **user-ghidra-mcp** returned `Not connected` / `Connection closed` (no live decompile, rename, `set_function_this_type`, or `save_program`).

## 3. Functions

| Address | Ghidra / manifest name | Role summary | Evidence |
|---------|------------------------|--------------|----------|
| `0x00463fe0` | `FUN_00463fe0` | **Decompress main-controller `process_data` (buffered / “simple”)** — drives row output via `cinfo+0x18c` buffer controller; first pass allocates via `mem->alloc_large` vtable `+0x1c` with `need_buffer=true` (`push 1`). | Disasm; `mov [esi+4], 0x463fe0` @ `0x46418e` from `start_pass` @ `0x464130` when `pass==1`; struct offsets match IJG `jpeg_decompress_struct` (`+0x18c` main, `+0x1a0`/`+0x1a8` post). |
| `0x00464090` | `FUN_00464090` | **Decompress main-controller `process_data` (skip / “context”)** — same controller object, `alloc_large` with `need_buffer=false` (`push 0`); copies/consume rows with `cmova` clamping against output scanline budget. | Disasm; `mov [esi+4], 0x464090` @ `0x4641be` when `pass==2`; paired with `FUN_00463fe0` in `start_pass`. |
| `0x00464230` | `jinit_d_main_controller` | **IJG `jinit_d_main_controller`** — `alloc_small(0x1c)` → `cinfo+0x18c`; vtable word `0x464130` (`start_pass`); optional `FUN_0045f7f0` row-group sizing when `buffered_image`. | Disasm; direct `call 0x45f7f0`; xref `FUN_00460200@0x4602e4` (post-controller init body, task 43). |
| `0x004642f0` | `sep_upsample` | **IJG merged upsampler `sep_upsample`** — registered as upsampler method (`[ebp+4]` @ `jinit_upsampler`). | Disasm `mov [ebp+4], 0x4642f0` @ `0x4647cb`; pointer in `.text` @ `0x4647ce`. |
| `0x004643e0` | `int_upsample` | **IJG integer upsample** — calls `jmem` alloc (`0x447ce0`) and `FUN_0045f810` helper. | Disasm; call sites; pointer @ `0x464917` from upsampler init tables. |
| `0x00464660` | `h2v2_smooth_downsample` | **IJG H2V2 smooth downsample** (merged-upsampling path). | Pointer @ `0x4648dd`; standard IJG symbol (mapping.csv). |
| `0x004647a0` | `jinit_upsampler` | **IJG `jinit_upsampler`** — `alloc_small(0xa0)` → `cinfo+0x1a0`; methods `{0x4642d0, 0x4642f0}`; sampling-factor checks. | Disasm; xref `FUN_00460200@0x4602d6`. |
| `0x00464990` | `build_ycc_rgb_table` | **IJG `build_ycc_rgb_table`** (decompress color table build). | Xref from `jinit_color_deconverter@0x464eb4`, `0x464ee7`; `__stdcall` in mapping.csv. |
| `0x00464a60` | `ycc_rgb_convert` | **IJG `ycc_rgb_convert`** scanline method. | Pointer install @ `0x464ee3` inside `jinit_color_deconverter`; mapping.csv. |
| `0x00464ca0` | `ycck_cmyk_convert` | **IJG `ycck_cmyk_convert`** (YCCK → CMYK). | Pointer @ `0x464eb0` in deconverter switch; mapping.csv. |
| `0x00464e00` | `jinit_color_deconverter` | **IJG `jinit_color_deconverter`** — `alloc_small(0x18)` → `cinfo+0x1a4`; jump table on `jpeg_color_space`; wires `build_ycc_rgb_table` + convert fn ptrs. | Disasm; xref `FUN_00460200@0x4602d0`. |
| `0x00464f90` | `build_ycc_rgb_table` (dup) | **Second copy of `build_ycc_rgb_table`** (distinct VA, same size `0xc4` as `0x464990`) — tail target of `jinit_color_deconverter_dup` (`jmp 0x464f90`). | Disasm @ `0x465551`; mapping.csv duplicate entry. |
| `0x00465180` | `FUN_00465180` | **Color-converter scanline worker** (non-YCbCr path) — indexed sample fetch using tables at `cinfo+0x1a0+0x10..0x1c`; installed as module method `+0xc`. | Disasm; `mov [esi+0xc], 0x465180` @ `0x465563` in `jinit_color_deconverter_dup`; R5 worker 15 xref `jinit_color_deconverter@0x65563` cluster. |
| `0x004654f0` | `jinit_color_deconverter` (dup label) | **`jinit_merged_upsampler` / merged-upsample registrar** (manifest name collision) — `alloc_small(0x30)` → `cinfo+0x1a0`, vtable `0x465060`, branches on `cinfo+0x114==2`. | Disasm; xref `FUN_00460200@0x4602b0` when buffered output; **not** a second `jinit_color_deconverter` @ `0x464e00`. |
| `0x004655f0` | `find_biggest_color_pop` | **IJG quantizer `find_biggest_color_pop`** (`__fastcall`). | Disasm; `median_cut@0x465a9c`. |
| `0x00465620` | `find_biggest_volume` | **IJG quantizer `find_biggest_volume`** (`__fastcall`). | Disasm; `median_cut@0x465aa3`. |
| `0x00465650` | `FUN_00465650` | **Quantizer `update_box`** (`__thiscall`, `ECX`=box index) — repacks histogram counts after median plane split; called twice per new box. | Disasm; `median_cut@0x465b6e`, `0x465b76`; third caller `0x465d68` (outside slice). |
| `0x00465a60` | `median_cut` | **IJG `median_cut`** — recursive box split using pop/volume helpers + `FUN_00465650`. | Disasm; xref `0x465d72` (2-pass quantizer driver). |

### Related VA not in task list (manifest gap)

| Address | Role | Evidence |
|---------|------|----------|
| `0x00464130` | **`d_main_controller::start_pass`** — selects among `process_data` fns (`0x463f60`, `0x463fe0`, `0x464090`) by pass id / `buffered_image`. | `jinit_d_main_controller` stores `dword [edi], 0x464130` @ `0x46424d`; disasm shows pointer stores @ `0x46418e`, `0x4641be`. |

### Upstream game reachability

| Caller | Callee(s) in slice | Proof |
|--------|-------------------|--------|
| `FUN_00460200` @ `0x460200` | `jinit_color_deconverter` / `jinit_color_deconverter_dup`, `jinit_upsampler`, `jinit_d_main_controller` | Disasm @ `0x4602b0`–`0x4602e4`; documented as **`jinit_d_post_controller` init body** in task 43 |
| `jpeg_start_decompress` chain | via `FUN_004604d0` → `FUN_00460200` | task 43 + `mapping.csv` `jpeg_start_decompress@0x45ec10` |

## 4. Ghidra deltas

**None applied** — MCP offline.

**Queued (evidence-backed, not executed):**

| Action | Target | Rationale |
|--------|--------|-----------|
| `rename_function_by_address` | `FUN_00463fe0` → `process_data_simple_main` | `start_pass` installs @ `[ctrl+4]` for pass 1; IJG `jdmainct.c` |
| `rename_function_by_address` | `FUN_00464090` → `process_data_context_main` | pass 2 install @ `0x4641be` |
| `rename_function_by_address` | `0x00464130` → `start_pass_main` | vtable slot written by `jinit_d_main_controller` |
| `rename_function_by_address` | `FUN_00465650` → `update_box` | `median_cut` __thiscall helper |
| `rename_function_by_address` | `FUN_00465180` → `rgb_gray_convert` or proven convert name | Only after matching IJG `jccolor.c` / `jdcolor.c` symbol size — **do not guess** |
| `rename_function_by_address` | `FUN_004654f0` → `jinit_merged_upsampler` | Behavior vs mislabel `jinit_color_deconverter` |
| `set_plate_comment` | libjpeg `FUN_*` above | Plate = one-line IJG role + xref to `FUN_00460200` |

## 5. Frida

**none** — Static PE proof sufficient; slice is cold-path JPEG decode inside `CDSJpegImage`, not live input/dispatch.

## 6. Remaining UNK

| Item | Reason |
|------|--------|
| Exact IJG export name for `FUN_00465180` | Scanline math matches generic color convert, but symbol not in `mapping.csv`; needs Ghidra FLIRT/string or IJG source size match |
| `0x00464f90` vs `0x00464990` duplicate | Proven second build of same routine; linker duplicate — no functional UNK |
| `jinit_color_deconverter_dup@0x4654f0` manifest label | Proven merged-upsample / alternate output path init, not deconverter |
| Live Ghidra xref graph | MCP down — indirect method ptrs (upsample/color) only partially enumerated via `.text` pointer scan |

## Evidence artifacts

- PE: `orig/bulanci.exe`
- Disasm/xref script (local, not committed): `ghidra_analysis/engine/_tmp_r6_task46_disasm.py`
- Prior docs: [round6_logic_task_43_report.md](round6_logic_task_43_report.md), [round5_worker_15_report.md](../struct_recovery/round5_worker_15_report.md), `config/bulanci/mapping.csv`

## MCP blocker

**user-ghidra-mcp:** `switch_program` → `Connection closed` then `Not connected`. Re-run worker 46 when Ghidra MCP is up to apply queued renames and `save_program bulanci.exe` once.
