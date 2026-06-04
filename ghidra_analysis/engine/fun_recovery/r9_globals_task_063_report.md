# Round 9 `_Globals` — Task 063 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 63 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec |
| **seed_address** | `0x0046db50` |
| **ghidra_name** | `FUN_0046DB50` |
| **prior_hint** | R6 — dispatch UNK |
| **prior art** | R8 FUN task 19 (`round8_fun_task_19_report.md`); R6 logic task 45 (`start_pass_dcolor`) |

## Status

**PARTIAL** — Live Ghidra MCP re-verify confirms role, xref closure, and `void __cdecl` body. **No rename:** LOCAL IJG `jdcolor`/merged-upsampler H2V1 row helper; no standalone COFF/export symbol (ROUND9 no-guess rule). **`_Globals.cpp` body written** from decompile/disasm.

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x0046db50` | `FUN_0046db50` | **H2V1 merged upsample + YCbCr→colormap row helper** — installed by `start_pass_dcolor` when per-component sampling code `== 2` (2:1 horizontal, 1:1 vertical); bilinear-style short-sample blend with YCC fixed-point `0x73fc`/`-0x28ba`/`0x1b37`/`-0x1712` (`>> 0xd`); writes paired colormap indices via `cinfo+0x120` LUT (`+0x80` base) | `void __cdecl` 5-arg, **1136 B** (`0x470`, `0046db50`–`0046dfbf`); **2×** DATA xref from `start_pass_dcolor`; leaf (no calls); R8 + R9 `force_decompile` + disasm |

### Sampling-code dispatch (`start_pass_dcolor@0x00463bc0`)

| Code | Factors | Helper installed |
|------|---------|------------------|
| 1 | H1V1 | `LAB_0046dfc0` |
| **2** | **H2V1** | **`FUN_0046db50`** |
| 4 | H1V2 | `LAB_0046d830` |
| 8 | H2V2 | `LAB_0046c9a0` / `LAB_0046cec0` / `LAB_0046d2a0` (dither via `cinfo+0x44`) |

### Parameter roles (decompile)

| Param | Role |
|-------|------|
| `param_1` | `jpeg_decompress_struct*` — colormap base `*(param_1+0x120)+0x80` |
| `param_2` | Component workspace — quant row `*(param_2+0x50)` |
| `param_3` | Short sample row buffer (+ `0x30` initial offset) |
| `param_4` | Output row pointer array (two rows) |
| `param_5` | Byte offset into each output row |

### Xref closure

| From | Site | Type | Notes |
|------|------|------|-------|
| `start_pass_dcolor` | `0x00463c22` | **DATA** | case 2: assign method pointer |
| `start_pass_dcolor` | `0x00463cbb` | **DATA** | store into per-component upsample workspace |

No direct `CALL` xrefs — invoked only via function-pointer slot.

### Callees

| Callee | Role |
|--------|------|
| — | Leaf — LUT indexing only |

### Source write (`_Globals.cpp`)

| Item | Value |
|------|-------|
| Return | `void` (was `uchar` stub; matches Ghidra) |
| Size | `0x470` per `mapping.csv` |
| Algorithm | Faithful port of Ghidra decompile: 2-pass blend prep loop (`loop_idx` 6→2), then two output rows × 2 colormap bytes each |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x0046db50` | `void __cdecl FUN_0046db50(int,int,int,int*,int)` |
| `set_decompiler_comment` | `0x0046db50` | R9 task 63 + xref + `_Globals.cpp` write note |
| `force_decompile` | `0x0046db50` | Refreshed (pre-write verify) |
| `save_program` | `bulanci.exe` | Saved |

**Not applied:** `rename_function_by_address` — no unique upstream export.

## Frida

**none** — static DATA xrefs + decompile/disasm sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Exact IJG static symbol name | LOCAL `METHODDEF` in modified `start_pass_dcolor` / jdmerge fusion |
| vs stock `h2v1_merged_upsample` | Binary splits per-factor row helpers + colormap quant path |
| `mapping.csv` | Still lists `uchar` return — export regen out of scope |
| Sibling helpers `LAB_0046dfc0` / `LAB_0046d830` | Separate tasks |

## Cross-links

- [round8_fun_task_19_report.md](round8_fun_task_19_report.md)
- [round6_logic_task_45_report.md](../logic_recovery/round6_logic_task_45_report.md)
- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
