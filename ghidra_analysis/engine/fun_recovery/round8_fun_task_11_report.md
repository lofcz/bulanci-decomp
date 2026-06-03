# Round 8 FUN — Task 11 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 11 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x00466920` |
| **title** | FUN recovery: FUN_00466920 @ 0x00466920 (xrefs=1) |
| **prior_hint** | R7 task 50 — merged-upsampler dither table size |
| **carry_over** | [round7_fun_task_50_report.md](round7_fun_task_50_report.md) |

## Status

**PARTIAL** — Live Ghidra MCP re-verify (`connect_instance bulanci`, 2026-06-03) confirms R7 closure. **No rename:** IJG `jinit_merged_upsampler` / merged-upsampler **LOCAL** ordered-dither table size helper; no standalone COFF/export symbol (R8 no-guess rule). Name stays `FUN_00466920`.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x00466920` | `FUN_00466920` | **`FUN_00466920`** | **Merged-upsampler ordered-dither sample-table size** — factorial/permutation walk over `param_2[]` until product exceeds `param_1[0x15]`; optional index remap via `jpeg_natural_order` tail @ `0x0049df50` when `param_1[0xb]==2`; returns byte count for `alloc_large` in caller; raises **JERR `0x38`** when derived base `< 2` | `int __cdecl FUN_00466920(int * cinfo, int * counts)`; body **231 B** (`0xE7`, `00466920`–`00466a06`); **1×** CODE xref; decompile matches R7; **not** zlib `gen_codes` (R6 correction) |

### IJG field mapping (this binary)

| Offset | Role in this helper |
|--------|---------------------|
| `cinfo+0x2c` (`param_1[0xb]`) | When `== 2`, permutation index uses `(&jpeg_natural_order)[i]` @ `0x0049df50` |
| `cinfo+0x54` (`param_1[0x15]`) | Target / cap for factorial product search |
| `cinfo+0x64` (`param_1[0x19]`) | Component count — outer loop bound |
| `*cinfo` error mgr | On bad base: `err->msg_code = 0x38`, `err->msg_parm.i = product`, then `error_exit` |

### Algorithm (decompile summary)

1. Find smallest integer base `iVar8 ≥ 1` such that `iVar8^num_components > cap` (nested multiply loop).
2. If `iVar8 < 2`, emit JERR `0x38` and call `error_exit`.
3. Initialize `param_2[]` to base `iVar8`, compute initial product `iVar5`.
4. Repeatedly increment permutation digits (optionally via natural-order index); return first product `≥ cap`, or previous product when overflow detected.

Return value is passed to `FUN_00466a50` as `alloc_large` size (`cinfo+4` vtable slot `+8`).

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole CODE)** | `0x00466a6a` | `FUN_00466a50` | `iVar3 = FUN_00466920(param_1, upsampler+0x20)` then `alloc_large(cinfo, 1, iVar3, num_components)` |
| **Init chain** | `0x00467460` | `jinit_merged_upsampler` | Calls `FUN_00466a50(param_1)` after component/width guards; then `zlib__gen_codes` (separate path) |

Caller excerpt (live decompile):

```c
piVar11 = (int *)(param_1[0x6a] + 0x20);  /* upsampler workspace counts */
iVar3 = FUN_00466920(param_1, piVar11);
piVar4 = alloc_large(param_1, 1, iVar3, param_1[0x19]);
```

### Callees

| Callee | Role |
|--------|------|
| — | Leaf helper — no direct calls; uses `error_exit` via `**(code **)param_1` on JERR path |

## Ghidra deltas

| Action | Result |
|--------|--------|
| **none** | R7 annotations verified intact: plate comment (merged-upsampler dither / `jpeg_natural_order`), decompiler pre-comment, prototype `int __cdecl FUN_00466920(int *, int *)`; no `save_program` |

## Frida

**none** — JPEG decompress merged-upsampler init; static xref + decompile + `jinit_merged_upsampler` chain sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Exact IJG static name | LOCAL helper inlined/split from merged-upsampler init in libjpeg-6b; no COFF/export string in `bulanci.exe` |
| `FUN_00466a50` naming | R8 task 13 — dither buffer fill after size calc |
| `FUN_00466a10` | Fixed-point sample value helper called from `FUN_00466a50` loop |
| JERR `0x38` string | Error code proven in disasm/decompile; message table label not resolved this session |
| `mapping.csv` / `_Globals.cpp` | Still `FUN_00466920` — export regen out of scope |
