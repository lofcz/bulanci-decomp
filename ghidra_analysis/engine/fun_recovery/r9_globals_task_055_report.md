# Round 9 `_Globals` — Task 055 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 55 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec |
| **seed_address** | `0x00467020` |
| **ghidra_name (before)** | `FUN_00467020` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R8 FUN task 35 (`round8_fun_task_35_report.md`); R8 FUN task 34 (sibling `FUN_00466f00`); R6 logic task 48 slice |

## Status

**PARTIAL** — Live Ghidra MCP re-verifies R8 closure: **IJG libjpeg-6b merged-upsampler row worker for RGB (`num_components == 3`)**. Installed only as a **method pointer** at `upsample+4` by `FUN_00467340` when `cinfo+0x4c == 1` and `cinfo+0x64 == 3` (`MOV [EDI+4], 0x467020` @ `0x004673d2`). **No rename:** no COFF/upstream export for this 0x12d B `METHODDEF`-style body (ROUND9 no-guess rule). Name stays **`FUN_00467020`**.

## Function

| Address | Ghidra (this session) | Role summary | Evidence |
|---------|----------------------|--------------|----------|
| `0x00467020` | `FUN_00467020` | **Merged-upsampler row worker (RGB / 3 components):** per output row, walk input **3 bytes/pixel** (Y,Cb,Cr); sum three AC-table lookups from ring-indexed pointer tables at `upsample+0x34/+0x38/+0x3c` with column ring `& 0xf` at `upsample+0x30`; write one output byte/pixel. **Does not** zero the output row (unlike sibling `FUN_00466f00`). | Live MCP: body `00467020`–`0046714c` (**0x12d** B); signature `void __cdecl FUN_00467020(int *cinfo, int input_buf, int *output_buf_ptr, int num_rows)`; **1×** xref **DATA** `FUN_00467340@0x004673d2`; no direct `CALL` xrefs |

### Install gate (`FUN_00467340` disasm, Ghidra live)

| VA | Instruction | Meaning |
|----|-------------|---------|
| `0x00467352` | `MOV EAX,[ESI+0x4c]` | Upsample method id (`cinfo+0x4c`) |
| `0x00467368` | `JZ 0x004673cc` | Branch when method == **1** (merged, non-fancy) |
| `0x004673cc` | `CMP [ESI+0x64], 3` | `num_components` gate |
| `0x004673d2` | `MOV [EDI+4], 0x467020` | **Install `FUN_00467020`** at upsample method slot |
| `0x004673db` | `MOV [EDI+4], 0x466f00` | Else install non-RGB sibling `FUN_00466f00` |

Caller chain (static, not re-decompiled this session): `FUN_00460200` (decompress master) → `jinit_merged_upsampler@0x00467460` → vtable `start_pass=FUN_00467340` → (pass start) stores method ptr → runtime indirect call through `upsample+4`.

### `cinfo` / upsample fields used (decompile + disasm)

| Offset | Field | Use in body |
|--------|-------|-------------|
| `cinfo+0x1a8` (`cinfo[0x6a]`) | upsample object ptr | Base for tables and ring state |
| `cinfo+0x5c` (`cinfo[0x17]`) | `output_width` | Inner pixel loop count |
| `upsample+0x18` | color-conversion table triple | `*piVar3`, `piVar3[1]`, `piVar3[2]` — Y/Cb/Cr AC bases |
| `upsample+0x30` | ring row index | Loaded per row; `(idx+1) & 0xf` after each row |
| `upsample+0x34/+0x38/+0x3c` | AC pointer ring tables | Indexed by `(ring*0x40 + col&0xf)*4` |

### Disassembly (inner pixel loop, Ghidra live)

| VA | Instruction | Meaning |
|----|-------------|---------|
| `0x00467027` | `MOV EDX,[ECX+0x1a8]` | Load upsample from cinfo |
| `0x0046702d` | `MOV EAX,[EDX+0x18]` | Color table triple |
| `0x00467030` | `MOV ECX,[ECX+0x5c]` | `output_width` |
| `0x00467084` | `MOV ECX,[EDX+0x30]` | Ring index for this row |
| `0x00467097` | `SHL ECX,0x6` | `ring * 0x40` table stride |
| `0x004670c0`–`0x004670ff` | 3× `MOVZX` + table adds | Sum Y + Cb + Cr lookups |
| `0x0046710d` | `ADD EAX,0x1` (×3) | Advance input 3 bytes (Y,Cb,Cr) |
| `0x00467110` | `AND ECX,0xf` | Column ring mask |
| `0x0046712d` | `AND EAX,0xf` | Row ring advance |

### Decompile (Ghidra live)

```c
void __cdecl _Globals::FUN_00467020(int *cinfo, int input_buf, int *output_buf_ptr, int num_rows)
{
  /* upsample = cinfo[0x6a]; tables = *(upsample+0x18); output_width = cinfo[0x17] */
  /* Per row: ring idx upsample+0x30; AC rings +0x34/+0x38/+0x3c */
  /* Inner: *out = table_Y[pb[0]] + table_Cb[pb[1]] + table_Cr[pb[2]]; pb+=3; col&=0xf */
  /* After row: *(upsample+0x30) = (ring+1)&0xf */
}
```

(Entry plate comment documents Parameters / Algorithm / Returns / Caller — installed @ `0x4673d2` when merged + 3 components.)

### Sibling comparison

| Function | When installed | Row init | Input stride | Components |
|----------|----------------|----------|--------------|------------|
| `FUN_00466f00` | merged + `num_components != 3` | `IJG_jzero_far` each row | `+num_components` per pixel | variable loop |
| **`FUN_00467020`** | merged + `num_components == 3` | none | **+3** per pixel | fixed Y,Cb,Cr |
| `FUN_00467150` | merged fancy (`cinfo+0x4c==2`) | `IJG_jzero_far` + short workspace | fancy colormap path | separate task |

### Xref closure

| Direction | Address | Context |
|-----------|---------|---------|
| **To seed (sole)** | `0x004673d2` | `FUN_00467340` — `upsample->upsample_method = FUN_00467020` (DATA) |
| **Indirect use** | runtime | Decompress upsample pass calls through `upsample+4` |

### mapping.csv stub (not trusted)

```
;_Globals::FUN_00467020;0x467020;0x124;__cdecl;;void;int*;int;int*;int
```

*(Live Ghidra body size **0x12d** B (`00467020`–`0046714c`); mapping size `0x124` is short by 9 B. Signature/return type match live Ghidra.)*

### `_Globals.cpp` stub (not trusted)

```cpp
void _Globals::FUN_00467020(int* param_1, int param_2, int* param_3, int param_4) { STUB_BODY(); }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `connect_instance` | `bulanci` | OK — 202 tools |
| *(re-verify only)* | `0x00467020` | R8 prototype + decompiler/plate comments **present**; decompile readable with `cinfo`, `output_buf_ptr`, `num_rows` |
| **Not applied** | `rename_function_by_address` | No unique IJG `jdmerge.c` export / COFF symbol for this method body |
| **Not applied** | `save_program` | No mutations this session |

## Frida

**none** — Static IJG decompressor method pointer; install gate + inner loop proven in Ghidra.

## Remaining UNK

| Item | Status |
|------|--------|
| Exact IJG `jdmerge.c` export symbol for RGB merged row worker | **UNK** — control-flow matches merged-upsampler family; no COFF-exact map in repo |
| Relationship to `h2v2_merged_upsample@0x00466300` | **Distinct** — separate VA/size; that symbol is for `cinfo+0x4c==2` fancy path |
| `mapping.csv` size `0x124` vs live `0x12d` | **Mismatch** — out of scope for FUN-only task |
| `_Globals.cpp` body recovery | Out of scope — stub remains; objdiff slice not requested |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round8_fun_task_35_report.md](round8_fun_task_35_report.md)
- [round8_fun_task_34_report.md](round8_fun_task_34_report.md) — sibling `FUN_00466f00`
- [round6_logic_task_48_report.md](../logic_recovery/round6_logic_task_48_report.md)
- [jpeg_decoder.md](../../formats/jpeg_decoder.md) — libjpeg-6b identity; `jdmerge.c` cluster
