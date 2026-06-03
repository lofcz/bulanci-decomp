# Round 8 FUN — Task 19 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 19 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x0046db50` |
| **title** | FUN recovery: FUN_0046DB50 @ 0x0046db50 (xrefs=2) |
| **prior_hint** | R6 — dispatch UNK |

## Status

**PARTIAL** — Live Ghidra MCP re-verify (`connect_instance bulanci`, 2026-06-03) proves role and xref closure. **No rename:** IJG `jdcolor.c` / merged-upsampler **LOCAL** H2V1 row helper; no standalone COFF/export symbol (R8 no-guess rule). Name stays `FUN_0046db50`.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x0046db50` | `FUN_0046db50` | **`FUN_0046db50`** | **H2V1 merged upsample + YCbCr→colormap row helper** — installed by `start_pass_dcolor` when per-component sampling code `== 2` (2:1 horizontal, 1:1 vertical); bilinear-style short-sample blend with YCC fixed-point `0x73fc`/`-0x28ba`/`0x1b37`/`-0x1712` (`>> 0xd`); writes paired colormap indices via `cinfo+0x120` LUT (`+0x80` base) | `void __cdecl` 5-arg, **1136 B** (`0x470`, `0046db50`–`0046dfbf`); **2×** DATA xref from `start_pass_dcolor`; decompile + disasm; [round6_logic_task_45_report.md](../logic_recovery/round6_logic_task_45_report.md); R5 plate `start_pass_dcolor@0x00463c22` |

### Sampling-code dispatch (`start_pass_dcolor@0x00463bc0`)

Per-component switch on `comp_info[].component_id` encoding `(h_samp_factor) * (v_samp_factor == 2 ? 4 : 1)`:

| Code | Factors | Helper installed |
|------|---------|------------------|
| 1 | H1V1 | `LAB_0046dfc0` |
| **2** | **H2V1** | **`FUN_0046db50`** |
| 4 | H1V2 | `LAB_0046d830` |
| 8 | H2V2 | `LAB_0046c9a0` / `LAB_0046cec0` / `LAB_0046d2a0` (dither mode via `cinfo+0x44`) |

Live decompile excerpt:

```c
case 2:
  iVar10 = 0;
  local_14 = FUN_0046db50;
  local_10 = 0;
  break;
/* ... */
piVar11[-10] = (int)local_14;   /* per-component method ptr @ upsample workspace */
```

### Parameter roles (decompile)

| Param | Role |
|-------|------|
| `param_1` | `jpeg_decompress_struct*` — colormap base `*(param_1+0x120)+0x80` |
| `param_2` | Component / quant workspace — coefficient block @ `+0x50` |
| `param_3` | Short sample row buffer (+ `0x30` initial offset) |
| `param_4` | Output row pointer array (two rows) |
| `param_5` | Byte offset into each output row |

Inner loop (`ESI=6`, stride `0x10` shorts): for each output column group, blend neighboring chroma samples (fast path when neighbors zero → `sample * coef * 4`), then map blended Y/Cb/Cr through colormap LUT to two output bytes per row pair.

### Xref closure

| From | Site | Type | Notes |
|------|------|------|-------|
| `start_pass_dcolor` | `0x00463c22` | **DATA** | `local_14 = FUN_0046db50` (case 2 assignment) |
| `start_pass_dcolor` | `0x00463cbb` | **DATA** | Store method pointer into per-component upsample workspace |

No direct `CALL` xrefs — invoked only via function-pointer slot written during color pass setup.

### Callees

| Callee | Role |
|--------|------|
| — | Leaf — no calls; table-driven LUT indexing only |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_plate_comment` | `0x0046db50` | Updated — H2V1 merged upsample+quantize row helper |
| `set_decompiler_comment` | `0x0046db50` | Updated — R8 sampling-code + xref + sibling proof |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — JPEG decompress color/quantize path; static DATA xrefs + `start_pass_dcolor` decompile sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Exact IJG static name | LOCAL `METHODDEF` in modified `start_pass_dcolor` / jdmerge fusion; not in COFF export table |
| vs stock `h2v1_merged_upsample` | Stock IJG `jdmerge.c` uses a monolithic upsampler; this binary splits per-factor row helpers + colormap quant path |
| `LAB_0046dfc0` / `LAB_0046d830` | Sibling H1V1/H1V2 helpers — separate R8 tasks if manifested |
| `mapping.csv` / `_Globals.cpp` | Still `FUN_0046db50`, return type `uchar` stub vs live `void` body — export regen out of scope |

## Cross-links

- [`round6_logic_task_45_report.md`](../logic_recovery/round6_logic_task_45_report.md) — `start_pass_dcolor` slice; notes internal `FUN_0046db50` call site |
- [`round5_worker_15_report.md`](../struct_recovery/round5_worker_15_report.md) — xref plate |
- [`round7_fun_task_48_report.md`](round7_fun_task_48_report.md) — adjacent `jdcolor.c` init @ `0x00463ee0` |
- [`round7_fun_task_49_report.md`](round7_fun_task_49_report.md) — `color_convert` @ `0x00465180` |
