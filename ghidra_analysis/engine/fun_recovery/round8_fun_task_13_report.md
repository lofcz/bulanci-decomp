# Round 8 FUN — Task 13 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 13 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x00466a50` |
| **title** | FUN recovery: FUN_00466A50 @ 0x00466a50 (xrefs=1) |
| **prior_hint** | R7 task 50 — caller of dither size calc (`FUN_00466920`) |

## Status

**PARTIAL** — Role, upsample field writes, and xref closure proven via live Ghidra MCP (`connect_instance bulanci`). **No rename:** IJG export symbol not uniquely verified (Round 8 no-guess rule). Corrects R6 “Huffman `gen_codes` driver” attribution.

## Function

| Address | Ghidra name (before → after) | Size | CC | Role summary | Evidence |
|---------|------------------------------|-----:|-----|--------------|----------|
| `0x00466a50` | `FUN_00466a50` → **`FUN_00466a50`** (unchanged) | 285 B (`0x11d`) | `__cdecl` | **Merged-upsampler colormap index-table init:** after `FUN_00466920` computes flattened table size from `upsample+0x20[]` component factors, calls `cinfo->mem->alloc_sarray` (vtable `mem+8`) and fills per-component byte stripes using `FUN_00466a10` rounded `(index×255+div/2)/div`; stores pointer array at **`upsample+0x10`** and byte count at **`upsample+0x14`**. Runs inside extended `jinit_merged_upsampler` before mislabeled `zlib::gen_codes` builds parallel tables at `upsample+0x18`. | Decompile + disasm; sole xref |

### Xref closure

| From | Site | Type |
|------|------|------|
| `jinit_merged_upsampler` | `0x004674db` | `UNCONDITIONAL_CALL` |

### Callees

| Callee | Site | Role |
|--------|------|------|
| `FUN_00466920` | `0x00466a6a` | Ordered-dither / colormap index **size** calculation (R7 task 50) |
| `FUN_00466a10` | `0x00466adb` | Rounded byte scale `(EAX×0xff+ECX/2)/ECX` per stripe index |
| `cinfo->mem->alloc_sarray` | `0x00466a83` | `alloc_large(cinfo, JPOOL_IMAGE, size, num_components)` via `[cinfo+4]+8` |

### Upsample struct fields touched (`cinfo+0x1a8` / `param_1[0x6a]`)

| Offset | Written | Meaning (descriptive) |
|--------|---------|---------------------|
| `+0x10` | `JSAMPARRAY` (per-component row pointers) | Allocated + filled index/sample tables |
| `+0x14` | `int` | Total byte size from `FUN_00466920` |
| `+0x20` | *(read)* | Per-component alphabet / factor array fed to size calc |

### Control flow (proven)

1. `upsample = param_1[0x6a]`; `piVar11 = upsample + 0x20`.
2. `iVar3 = FUN_00466920(param_1, piVar11)` — table geometry / size.
3. `piVar4 = alloc_sarray(param_1, 1, iVar3, param_1[0x19])` — one buffer per component (`num_components` @ `cinfo+0x64`).
4. For each component `ci`: `stride = size / factors[ci]`; for each stripe index `j` in `0..factors[ci]-1`, compute scaled byte `FUN_00466a10(j)` and replicate across `stride` cells in the flattened layout (nested `for` over `iVar8` stepping by `size`).
5. On success: `*(upsample+0x10) = piVar4`, `*(upsample+0x14) = iVar3`; early exit when `num_components <= 0` still stores alloc result.

### Caller context (`jinit_merged_upsampler@0x00467460`)

```text
alloc_small(cinfo, 0x58) → cinfo+0x1a8
vtable start_pass = FUN_00467340
FUN_00466a50(cinfo)          ; this task
zlib::gen_codes(cinfo)       ; fills upsample+0x18 tables (R8 task 01 band)
if (cinfo+0x4c == 2) FUN_00467300(cinfo)
```

Standard upstream `jdmerge.c` `jinit_merged_upsampler` only calls `build_ycc_rgb_table`; this PE adds **colormap-quant table setup** (`FUN_00466a50` + `zlib::gen_codes`) on the merged-upsampler object — consistent with bulanci’s extended `0x58`-byte upsampler vs upstream `my_upsampler`.

### Disassembly (entry + core calls)

```
00466a59  MOV  EBX,[EBP+0x1a8]      ; upsample
00466a61  LEA  ESI,[EBX+0x20]       ; factor array
00466a6a  CALL 0x00466920
00466a77  MOV  EDX,[EAX+8]           ; mem->alloc_sarray
00466a83  CALL EDX                   ; alloc
00466adb  CALL 0x00466a10            ; byte scale (loop)
00466b54  MOV  [upsample+0x10], buf
00466b57  MOV  [upsample+0x14], size
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_plate_comment` | `0x00466a50` | R8 plate comment (merged-upsampler index tables; R6 correction) |
| `save_program` | `bulanci.exe` | single save at task end |

## Frida

**none** — JPEG decompress init path; static xref + decompile sufficient.

## Remaining UNK

| Item | Why still UNK |
|------|----------------|
| Exact IJG export name | No `ref/libjpeg6b` byte diff or COFF symbol in repo; behavior matches colormap-quant init on merged upsampler, not `build_ycc_rgb_table` |
| `mapping.csv` return type | Lists `uchar`; Ghidra decompile is `void __cdecl` — prototype fix out of scope |
| Relationship to `zlib::gen_codes` | Sibling init in same `jinit_merged_upsampler`; that routine is **not** zlib `trees.c` `gen_codes` (see R8 task 01) |

## Cross-links

- [`round7_fun_task_50_report.md`](round7_fun_task_50_report.md) — callee `FUN_00466920` (size calc)
- [`round8_fun_task_01_report.md`](round8_fun_task_01_report.md) — sibling `FUN_00466a30` in `zlib::gen_codes`
- [`round7_fun_task_34_report.md`](round7_fun_task_34_report.md) — `FUN_00467300` post-init when `cinfo+0x4c==2`
- [`round6_logic_task_47_report.md`](../logic_recovery/round6_logic_task_47_report.md) — R6 slice (outdated Huffman label for this VA)
- [`config/bulanci/mapping.csv`](../../../config/bulanci/mapping.csv) — `0x466a50`, size `0x11d`
