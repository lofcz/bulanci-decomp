# Round 9 `_Globals` — Task 051 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 51 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec |
| **seed_address** | `0x00466a50` |
| **ghidra_name (before → after)** | `FUN_00466A50` → **`FUN_00466A50`** (unchanged) |
| **prior_hint** | R8 — merged-upsampler colormap index tables (PARTIAL) |
| **prior art** | R8 FUN task 13 (`round8_fun_task_13_report.md`); R7 task 50 callee `FUN_00466920` |

## Status

**PARTIAL** — Role, upsample field writes, callee/caller xref closure, and control flow re-verified via live Ghidra MCP (`connect_instance bulanci`). **No rename:** no unique IJG upstream export or bulanci game symbol proof (ROUND9 no-guess rule). R8 plate/decompiler comments still present; no Ghidra mutations this session.

## Function

| Address | Ghidra name | Size | CC | Role summary | Evidence |
|---------|-------------|-----:|-----|--------------|----------|
| `0x00466a50` | `FUN_00466A50` | **0x11d** B (`00466a50`–`00466b6c`) | `__cdecl` | **Merged-upsampler colormap index-table init:** after `FUN_00466920` computes flattened table size from `upsample+0x20[]` component factors, calls `cinfo->mem->alloc_sarray` (vtable `mem+8`) and fills per-component byte stripes using `FUN_00466a10` rounded `(index×255+div/2)/div`; stores pointer array at **`upsample+0x10`** and byte count at **`upsample+0x14`**. Runs inside `jinit_merged_upsampler` before `zlib__gen_codes` builds parallel tables at `upsample+0x18`. | Live decompile + disasm; sole xref |

### Xref closure

| From | Site | Type |
|------|------|------|
| `jinit_merged_upsampler` | `0x004674db` | `UNCONDITIONAL_CALL` |

### Callees

| Callee | Site | Role |
|--------|------|------|
| `FUN_00466920` | `0x00466a6a` | Ordered-dither / colormap index **size** calculation (R7 task 50) |
| `FUN_00466a10` | `0x00466adb` | Rounded byte scale `(index×0xff+div/2)/div` per stripe index |
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
4. For each component `ci`: `stride = size / factors[ci]`; for each stripe index `j` in `0..factors[ci]-1`, compute scaled byte `FUN_00466a10(j)` and replicate across `stride` cells in the flattened layout (nested `for` over column offset stepping by `size`).
5. On success: `*(upsample+0x10) = piVar4`, `*(upsample+0x14) = iVar3`; early exit when `num_components <= 0` still stores alloc result.

### Caller context (`jinit_merged_upsampler@0x00467460`)

```text
alloc_small(cinfo, 0x58) → cinfo+0x1a8
vtable start_pass = FUN_00467340
FUN_00466a50(cinfo)          ; this task @ 0x004674db
zlib__gen_codes(cinfo)       ; fills upsample+0x18 tables
if (cinfo+0x13 == 2) FUN_00467300(cinfo)
```

Standard upstream `jdmerge.c` `jinit_merged_upsampler` only calls `build_ycc_rgb_table`; this PE adds **colormap-quant table setup** (`FUN_00466a50` + `zlib__gen_codes`) on the merged-upsampler object — consistent with bulanci’s extended `0x58`-byte upsampler vs upstream `my_upsampler`.

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

### `FUN_00466a10` scale (disasm proof)

```
00466a10  IMUL ECX,ECX,0xff          ; index * 255
00466a1b  SAR  EAX,0x1               ; div/2 (rounded half)
00466a22  IDIV ESI                   ; / divisor → AL
```

### mapping.csv stub (not trusted)

```
;_Globals::FUN_00466a50;0x466a50;0x114;__cdecl;;uchar;int*
```

*(Ghidra body size **0x11d**; return type `uchar` incorrect — live decompile is `void __cdecl` with no meaningful AL.)*

### `_Globals.cpp` stub (not trusted)

```cpp
uchar _Globals::FUN_00466a50(int* param_1) { STUB_BODY(); return 0; }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| *(none)* | — | R8 comments verified present; no rename/prototype change; no `save_program` |

**Existing comments (verified):**

- Plate @ `0x00466a50`: R8 merged-upsampler colormap index table alloc+fill; IJG symbol UNK.
- Decompiler preamble: same role summary + sole caller note.

## Frida

**none** — JPEG decompress init path; static xref + decompile sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Exact IJG upstream export name | **UNK** — no COFF export or byte-matched libjpeg symbol in repo; behavior matches colormap-quant init on merged upsampler, not upstream `build_ycc_rgb_table` |
| `mapping.csv` return type / size | **UNK** — lists `uchar` / `0x114`; Ghidra shows `void` / **0x11d** |
| Relationship to `zlib__gen_codes` | **Documented** — sibling init in same `jinit_merged_upsampler`; not zlib `trees.c` `gen_codes` (R8 task 01) |
| Rename vs keep `FUN_00466A50` | **Deferred** — insufficient unique-name proof per ROUND9 protocol |

## Cross-links

- [round8_fun_task_13_report.md](round8_fun_task_13_report.md) — R8 prior art (confirmed)
- [round7_fun_task_50_report.md](round7_fun_task_50_report.md) — callee `FUN_00466920` (size calc)
- [round8_fun_task_01_report.md](round8_fun_task_01_report.md) — sibling `zlib__gen_codes` in `jinit_merged_upsampler`
- [round7_fun_task_34_report.md](round7_fun_task_34_report.md) — `FUN_00467300` post-init when `cinfo+0x13==2`
- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
