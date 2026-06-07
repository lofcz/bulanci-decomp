# Round 10 Deep — Task 16 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 16 |
| **round** | 10 |
| **kind** | codec |
| **prior_round** | 8 |
| **seed_address** | `0x0046a840` |
| **title** | Codec follow-up: FUN_0046A840 dispatch UNK |
| **acceptance** | 3 xref closure; rename only if unique IJG/libmad/zlib export match or bulanci symbol proof |
| **prior art** | [round8_fun_task_17_report.md](../fun_recovery/round8_fun_task_17_report.md); [r9_globals_task_059_report.md](../fun_recovery/r9_globals_task_059_report.md) |

## Status

**PARTIAL** — Live Ghidra MCP re-verification (`connect_instance bulanci`, 2026-06-07): IJG **`jcprepct.c` `expand_bottom_edge` homolog** confirmed (45 B / `0x2d`; sole callee `jcopy_sample_rows@0x0045f810`). **3× CODE xref closure** mapped with disasm at each call site. **Rename retained:** Ghidra `expand_bottom_edge` (R9 batch) justified by **bulanci symbol proof** (`mapping.csv`, `_Globals.h`, `_Globals.cpp`) — not by IJG COFF export (function is `LOCAL` in upstream). No Ghidra mutations this pass; cosmetic register-arg decompile remains.

## Functions / Struct

| Address | Ghidra (before) | Ghidra (after) | Role | Evidence |
|---------|-----------------|----------------|------|----------|
| `0x0046a840` | `FUN_0046A840` | **`expand_bottom_edge`** | **Bottom-edge row padding:** `for (row = input_rows; row < output_rows) jcopy_sample_rows(image_data, input_rows-1, image_data, row, 1, num_cols)` | Live disasm loop `0x0046a850`–`0x0046a868`; `get_function_callees` → `jcopy_sample_rows@0x0045f810` only; body `0x0046a840`–`0x0046a86c` (45 B); `verify_ijg_byte_match.py` homolog `jcprepct.c` `_expand_bottom_edge` |

### IJG correspondence

| Check | Match |
|-------|-------|
| Source | `jcprepct.c` `LOCAL(void) expand_bottom_edge(JSAMPARRAY image_data, JDIMENSION num_cols, int input_rows, int output_rows)` |
| Loop | Fixed source row `input_rows-1` (`LEA EBP,[ESI-1]` @ `0x0046a848`, not incremented) |
| IJG export | **None** — `LOCAL`; MSVC split into standalone `0x0046a840` |
| Bulanci proof | `config/bulanci/mapping.csv` `;_Globals::expand_bottom_edge;0x46a840`; `include/bulanci/_Globals.h` `static void expand_bottom_edge(size_t)`; `src/bulanci/_Globals.cpp` homolog body @ `0x0046a840` |

### Register calling convention (MSVC split)

Ghidra shows `in_EAX` / `unaff_EBX` / `unaff_EDI` because callers pass three args in registers; only `num_cols` is on the stack.

| Register / stack | IJG param | Set at call site |
|------------------|-----------|------------------|
| **EDI** | `image_data` (`JSAMPARRAY`) | `MOV EDI,[component_row_ptr]` |
| **EAX** | `input_rows` | `MOV EAX,[writer+0x34]` |
| **EBX** | `output_rows` | `MOV EBX,[writer+0x3c]` or `MOV EBX,[cinfo+0xdc]` |
| **stack** | `num_cols` | `PUSH [cinfo+0x1c]` or `PUSH width×8` |

### Disassembly (live Ghidra, core loop)

```
0046a840  PUSH ESI
0046a841  MOV  ESI,EAX              ; row = input_rows
0046a843  CMP  ESI,EBX
0046a845  JGE  0046a86b
0046a848  LEA  EBP,[ESI-1]          ; source = input_rows-1 (fixed)
0046a850  PUSH dword ptr [ESP+0xc]  ; num_cols
0046a855  PUSH 1
0046a857  PUSH ESI                  ; dest_row
0046a858  PUSH EDI
0046a859  PUSH EBP                  ; source_row
0046a85a  PUSH EDI                  ; input_array
0046a85b  CALL jcopy_sample_rows@0x0045f810
0046a860  INC  ESI
0046a868  JL   0046a850
```

### Xrefs (3) — closure with call-site disasm

| From | Caller | Register setup (disasm) | Context |
|------|--------|-------------------------|---------|
| `0x0046ab30` | `compress_output` @ `0x0046aa00` (mislabel — CDSJpeg decompress scanline pump) | `EBX=[EBP+0x3c]`, `EAX=[EBP+0x34]`, `EDI=[*component_ptr]`, `PUSH [cinfo+0x1c]` | Per-component loop when `[writer+0x34] < [writer+0x3c]`; sets `[writer+0x34]=[writer+0x3c]` after |
| `0x0046a923` | `pre_process_data` @ `0x0046a870` | `EBX=[cinfo+0xdc]`, `EAX=[writer+0x34]`, `EDI=[*component_buf]`, `PUSH [cinfo+0x1c]` | Mid-buffer refill when `*piVar1==0` and row index below `cinfo[0x37]` |
| `0x0046a9d9` | `pre_process_data` @ `0x0046a870` | `EDI=[component_buf+idx*4]`, `PUSH EDX` (`width×8` via triple `ADD EDX,EDX`) | Final padding path when output groups exhausted |

### Decompile (Ghidra, live)

```c
void __cdecl _Globals::expand_bottom_edge(size_t num_cols)
{
  if (in_EAX < unaff_EBX) {
    iVar1 = in_EAX - 1;
    do {
      jcopy_sample_rows(unaff_EDI, iVar1, unaff_EDI, in_EAX, 1, num_cols);
      in_EAX = in_EAX + 1;
    } while (in_EAX < unaff_EBX);
  }
}
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `connect_instance` | `bulanci` | OK |
| `get_function_by_address` | `0x0046a840` | `expand_bottom_edge`; sig `void __cdecl expand_bottom_edge(size_t num_cols)` |
| `get_function_xrefs` | `0x0046a840` | 3 CODE xrefs (confirmed) |
| `disassemble_bytes` | call sites `0x0046ab20`, `0x0046a910`, `0x0046a9c5` | Register/stack arg setup verified |
| `decompile_function` | `0x0046a840` + callers | OK — plate/decompiler comments from R8 present |
| *(none)* | — | No rename/prototype/comment change; no `save_program` |

**Prior rename (R9):** `rename_function_by_address` `FUN_0046a840` → `expand_bottom_edge` per `r9_partial_renames_applied.json` and bulanci `mapping.csv`.

## Decomp corrections

| Issue | IDA | Ghidra | Resolution |
|-------|-----|--------|------------|
| Function absent from IDA export | No `0x0046a840` / `expand_bottom_edge` symbol in `bulanci.ida.exe.c` | Named `expand_bottom_edge` | Static PE + IJG homolog + bulanci headers are ground truth; IDA gap expected for MSVC LOCAL split |
| Register parameters | N/A | `in_EAX` / `unaff_EBX` / `unaff_EDI` | **Cosmetic** — MSVC register calling at all 3 call sites; algorithm correct |
| Stack-only prototype | N/A | `void __cdecl expand_bottom_edge(size_t num_cols)` | Acceptable — only `num_cols` is stack param; register args documented in plate comment |

## Frida

**none** — Static disasm + 3-site xref closure + IJG `expand_bottom_edge` body match sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Decompiler register args (`in_EAX` / `unaff_*`) | **Cosmetic** — MSVC register-param convention; no Ghidra API to model mixed register/stack cdecl |
| `compress_output` @ `0x0046aa00` mislabel | **Out of scope** — separate task; true IJG `compress_output` @ `0x00467930` |
| `pre_process_data` @ `0x0046a870` | **Renamed** (R8 task 44); `_Globals.cpp` still `STUB_BODY()` — separate WRITE pass |
| objdiff byte match | Requires MSVC x86 compile of slice + `jcopy_sample_rows` body |

## Cross-links

- [round8_fun_task_17_report.md](../fun_recovery/round8_fun_task_17_report.md) — initial IJG homolog + xref map
- [round8_fun_task_44_report.md](../fun_recovery/round8_fun_task_44_report.md) — caller `pre_process_data@0x0046a870`
- [r9_globals_task_059_report.md](../fun_recovery/r9_globals_task_059_report.md) — `_Globals.cpp` WRITE + R9 no-rename note (superseded by R9 batch rename)
- [round7_fun_task_42_report.md](../fun_recovery/round7_fun_task_42_report.md) — jcprepct LOCAL no-rename precedent (IJG export rule)
