# Round 9 `_Globals` — Task 052 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 52 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec |
| **seed_address** | `0x00466cc0` |
| **ghidra_name (before)** | `FUN_00466CC0` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | [round8_fun_task_32_report.md](round8_fun_task_32_report.md); [round8_fun_task_33_report.md](round8_fun_task_33_report.md) (caller `FUN_00466d40`); [round6_logic_task_47_report.md](../logic_recovery/round6_logic_task_47_report.md) (Huffman mislabel **refuted**) |

## Status

**PARTIAL** — Role, calling convention, allocator use, scaling formula, xref closure, and `.rdata` span proven via live Ghidra MCP + PE byte match. **No rename:** not a COFF/upstream IJG export; distinct from `jpeg_add_quant_table@0x0045efe0` (percent-scale / `0x51eb851f` path). Identity of the 256-byte `.rdata` template bytes is **UNK** (not byte-equal to `std_luminance_quant_tbl[64]` in `CDSJpegImage.cpp`).

## Function

| Address | Ghidra (this session) | Role | Evidence |
|---------|----------------------|------|----------|
| `0x00466cc0` | `FUN_00466CC0` | **Allocate and fill 256×`int` scaled coefficient table** for merged-upsampler quant setup: `alloc_small(cinfo, JPOOL_IMAGE=1, 0x400)` then for each byte in `.rdata` `0x0049de50..0x0049df50` store `(0xfe01 − byte×0x1fe) / ((scale_selector<<9) − 0x200)` | Live disasm/decompile; PE `orig/bulanci_insturmented.exe` @ RVA `0x66cc0` length `0x74`; sole caller |

### Signature / size

| Source | Value |
|--------|-------|
| Ghidra entry | `int * __fastcall FUN_00466CC0(int param_1)`; body `00466cc0`–`00466d33` (**0x74** B) |
| Proven registers | **`ECX`** = `scale_selector` on entry (`MOV ESI,ECX`); **`EAX`** = `jpeg_common_struct *` cinfo (passed to `alloc_small`); return **`EAX`** = allocated `int *` |
| `config/bulanci/mapping.csv` | `__fastcall`; size **`0x68`** (stale — live/PE size is **`0x74`**) |
| `_Globals.cpp` stub | `int* FUN_00466cc0(int param_1)` — **not trusted** |

### Disassembly (Ghidra + PE)

```
00466cc0  PUSH 0x400
00466cc9  MOV  ESI,ECX                    ; scale_selector
00466ccb  MOV  ECX,[EAX+4]                ; cinfo->mem
00466cce  MOV  EDX,[ECX]                  ; alloc_small
00466cd0  PUSH 1                          ; JPOOL_IMAGE
00466cd2  PUSH EAX                        ; cinfo
00466cd3  CALL EDX
00466cd5  SHL  ESI,9
00466cdb  SUB  ESI,0x200                  ; divisor = (scale<<9)-0x200
00466ce1  MOV  EBP,EAX                    ; table base
00466ce5  MOV  EDI,0x49de50
00466cf0  XOR  ECX,ECX
00466d00  MOVZX EDX,byte [EDI+ECX]
00466d04  IMUL  EDX,0x1fe
00466d0a  MOV  EAX,0xfe01
00466d0f  SUB  EAX,EDX
00466d12  IDIV  EBX                       ; EBX = ESI = divisor
00466d17  ADD  ESI,4
00466d1d  MOV  [ESI-4],EAX
00466d20  JL   00466d00                    ; 16 bytes per row
00466d22  ADD  EDI,0x10
00466d25  CMP  EDI,0x49df50
00466d2b  JL   00466cf0                    ; 16 rows -> 256 outputs
00466d2f  MOV  EAX,EBP
00466d33  RET
```

### Decompiled core (Ghidra)

```c
piVar2 = alloc_small(cinfo, 1, 0x400);
puVar6 = &DAT_0049de50;
do {
  iVar3 = 0;
  do {
    *piVar5 = (int)((uint)puVar6[iVar3] * -0x1fe + 0xfe01)
              / (scale_selector * 0x200 + -0x200);
    iVar3++;
  } while (iVar3 < 0x10);
  puVar6 += 0x10;
} while ((int)puVar6 < 0x49df50);
return piVar2;
```

### `.rdata` template (proven span, content UNK)

| Field | Value |
|-------|-------|
| Start / end | `0x0049de50` .. `0x0049df50` (256 bytes, 16×16 row walk) |
| Ghidra `read_memory` | First 16 bytes: `00 c0 30 f0 0c cc 3c fc 03 c3 33 f3 0f cf 3f ff` |
| Cross-check | **Not** equal to first 64 bytes of `std_luminance_quant_tbl` or zigzag reorder thereof (`CDSJpegImage.cpp` / Python verify this session) |
| Other xref | `FUN_00463bc0@0x00463d35` compares float path against `0x49de50` (separate dither/quant helper — out of scope) |

### Xref closure

| From | Type | Context |
|------|------|---------|
| `0x00466d7a` | UNCONDITIONAL_CALL | **`FUN_00466d40`** — per-component scaled-table alloc/dedup; `MOV EAX,EBX` (cinfo) immediately before `CALL` |

Caller chain (prior R8, re-verified callee site):

`FUN_00467340` merged-upsampler `start_pass` @ `0x00467400` when `[upsample+0x34]==0` → **`FUN_00466d40`** → **`FUN_00466cc0`**.

### Distinction from `jpeg_add_quant_table`

| | `FUN_00466cc0` | `jpeg_add_quant_table@0x0045efe0` |
|--|----------------|----------------------------------|
| Output | 256×`int` (`0x400` B) | `JQUANT_TBL` 64×`UINT16` |
| Scale | `(0xfe01−b×0x1fe)/((sel<<9)−0x200)` | `(q×scale%+50)/100` via `0x51eb851f` |
| Input | Fixed `.rdata` 256 B | Caller `basic_table` pointer |
| Pool | `JPOOL_IMAGE` (1) | `JQUANT_TBL` permanent alloc path |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x00466cc0` | Updated with R9 proven summary (alloc, formula, registers, sole caller) |
| `force_decompile` | `0x00466cc0` | Success |
| `save_program` | `bulanci.exe` | Saved |
| `rename_function_by_address` | — | **Not applied** (no unique export / game symbol proof) |

## Frida

**none** — static disasm, xref closure, and allocator pattern sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Upstream IJG symbol name | **UNK** — jdmerge 1-pass quant scaling helper shape; no COFF/ref byte match in repo |
| Meaning of `DAT_0049de50` 256 bytes | **UNK** — not Annex K.1 `std_luminance_quant_tbl` bytes from recovered `jpeg_set_linear_quality` |
| `scale_selector` value set | **UNK** — comes from `upsample+0x20` per component (`FUN_00466d40`); not traced this task |
| `mapping.csv` size `0x68` | Stale vs Ghidra/PE **`0x74`** |
| R6 “Huffman extra-bits builder” | **Refuted** for this VA (R8 task 32; same `0x1fe` idiom, different caller chain than `zlib::gen_codes`) |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round8_fun_task_32_report.md](round8_fun_task_32_report.md)
- [round8_fun_task_33_report.md](round8_fun_task_33_report.md)
- [round8_fun_task_12_report.md](round8_fun_task_12_report.md) — `FUN_00467340` `start_pass`
- [ghidra_xrefs.jsonl](../../asset_catalog/_cache/ghidra_xrefs.jsonl) — data-ptr `0x49de50` from `FUN_00466cc0`
