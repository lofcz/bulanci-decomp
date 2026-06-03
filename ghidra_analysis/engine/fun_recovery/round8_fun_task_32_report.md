# Round 8 FUN — Task 32 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 32 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x00466cc0` |
| **title** | FUN recovery: FUN_00466CC0 @ 0x00466cc0 (xrefs=1) |
| **prior_hint** | — |

## Status

**PARTIAL** — Role and xref closure proven; **no rename** (scales `DAT_0049de50` std quant matrix — IJG shape matches `jpeg_add_quant_table` family but no unique export name / COFF proof in repo).

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x00466cc0` | `FUN_00466cc0` | **`FUN_00466cc0`** | **Alloc + scale 256-byte std quant matrix** from `DAT_0049de50..0x49df50` into 256×`int` block (`0x400` B) by `param_1` selector (`__fastcall`: `ECX`=scale, `EAX`=cinfo → `alloc_small(cinfo,1,0x400)`) | 116 B (`0x74`); nested 16-byte row loops; formula `(0xfe01−byte×0x1fe)/((scale−1)×512)` per disasm @ `0x466d00` |

### Xref closure

| From | Type | Context |
|------|------|---------|
| `0x00466d7a` | CALL | Sole caller **`FUN_00466d40@0x00466d40`** — per-component scaled quant table alloc/dedup (R8 task 33) |

### Caller chain

`FUN_00467340` (merged-upsampler **`start_pass`**, R8 task 12) @ `0x00467400` when `[upsample+0x34]==0` → **`FUN_00466d40`** → **`FUN_00466cc0(scale_selector)`**.

### Disasm highlights (live Ghidra MCP 2026-06-03)

- `PUSH 0x400` → `alloc_small` via `[cinfo+4]` (JPOOL_IMAGE=1)
- Outer loop: `EDI` walks `0x49de50..0x49df50` in 16-byte steps (16 rows × 16 cols)
- Inner: `IMUL EDX,0x1fe; SUB EAX,EDX from 0xfe01; IDIV EBX` where `EBX=(scale<<9)−0x200`

## Ghidra deltas

| Action | Result |
|--------|--------|
| **none** | Prior UNCERTAIN comment retained |

## Frida

**none**

## Remaining UNK

| Item | Reason |
|------|--------|
| Exact IJG name | No `ref/libjpeg6b` byte match; not in `jpeg_decoder.md` name list; distinct from `jpeg_add_quant_table@0x0045efe0` (percent-scale path) |
| R6 mislabel | [round6_logic_task_47](../logic_recovery/round6_logic_task_47_report.md) called this “Huffman extra-bits table builder” — **refuted** (same `imul 0x1fe` idiom as quant scaling, not zlib `gen_codes`) |

## Cross-links

- [round8_fun_task_33_report.md](round8_fun_task_33_report.md) — sole caller dedup driver
- [round8_fun_task_12_report.md](round8_fun_task_12_report.md) — `FUN_00467340` merged-upsampler `start_pass`
- [round6_logic_task_47_report.md](../logic_recovery/round6_logic_task_47_report.md) — original slice (Huffman label wrong for this VA)
