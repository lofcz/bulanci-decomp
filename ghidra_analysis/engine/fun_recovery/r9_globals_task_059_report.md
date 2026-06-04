# Round 9 `_Globals` — Task 059 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 59 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec |
| **seed_address** | `0x0046a840` |
| **ghidra_name** | `FUN_0046A840` |
| **prior_hint** | R6 — dispatch UNK |
| **prior art** | [round8_fun_task_17_report.md](round8_fun_task_17_report.md); [round8_fun_task_44_report.md](round8_fun_task_44_report.md) |

## Status

**PARTIAL** — Role and xref closure re-verified via live Ghidra MCP (`connect_instance bulanci`, 2026-06-04). **WRITE:** `src/bulanci/_Globals.cpp` stub replaced with IJG `expand_bottom_edge` homolog body (register args via MSVC inline asm). **No rename:** IJG `jcprepct.c` **LOCAL** `expand_bottom_edge`; no standalone COFF/export symbol (ROUND9 no-guess rule). Ghidra prototype + plate/decompiler comments from R8 intact; **no program mutations** this pass.

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x0046a840` | `FUN_0046A840` | **Bottom-edge row padding:** duplicate row `input_rows-1` into rows `[input_rows..output_rows)` via `jcopy_sample_rows` | Live `force_decompile`; body `0046a840`–`0046a86c` (45 B / `0x2d`); signature `void __cdecl FUN_0046a840(size_t num_cols)`; **3× CODE xrefs**; PE bytes match disasm |

### IJG correspondence

| Check | Match |
|-------|-------|
| Source | `jcprepct.c` `LOCAL(void) expand_bottom_edge(JSAMPARRAY, JDIMENSION num_cols, int input_rows, int output_rows)` |
| Loop | `for (row = input_rows; row < output_rows) jcopy_sample_rows(image_data, input_rows-1, image_data, row, 1, num_cols)` |
| Export | **None** — `LOCAL`; MSVC split into standalone `0x0046a840` |
| Byte probe | `verify_ijg_byte_match.py` homolog `jcprepct.c` `_expand_bottom_edge` |

### Register calling convention (MSVC split)

Ghidra shows `in_EAX` / `unaff_EBX` / `unaff_EDI` because callers pass three args in registers; only `num_cols` is on the stack.

| Register / stack | IJG param | Set at call site |
|------------------|-----------|------------------|
| **EDI** | `image_data` (`JSAMPARRAY`) | `[component_row_ptr_array]` |
| **EAX** | `input_rows` | `[writer+0x34]` current row index |
| **EBX** | `output_rows` | `[writer+0x3c]` stop row |
| **stack** | `num_cols` | `PUSH [cinfo+0x1c]` (`image_width`) or `width×8` @ second `pre_process_data` site |

### Disassembly (Ghidra, core loop)

```
0046a840  PUSH ESI
0046a841  MOV  ESI,EAX              ; row = input_rows
0046a843  CMP  ESI,EBX
0046a845  JGE  0046a86b
0046a848  LEA  EBP,[ESI-1]          ; source = input_rows-1 (fixed)
0046a850  PUSH dword ptr [ESP+0xc]  ; num_cols
0046a855  PUSH 1                    ; num_rows
0046a857  PUSH ESI                  ; dest_row
0046a858  PUSH EDI                  ; output_array
0046a859  PUSH EBP                  ; source_row
0046a85a  PUSH EDI                  ; input_array
0046a85b  CALL jcopy_sample_rows@0x0045f810
0046a860  INC  ESI
0046a868  JL   0046a850
```

### PE bytes (`orig/bulanci.exe`, 45 B)

`56 8b f0 3b f3 7d 24 55 8d 6e ff eb 03 8d 49 00 8b 44 24 0c 50 6a 01 56 57 55 57 e8 b0 4f ff ff 83 c6 01 83 c4 18 3b f3 7c e6 5d 5e c3`

### Decompile (Ghidra, post-R8)

```c
void __cdecl FUN_0046a840(size_t num_cols)
{
  if (in_EAX < unaff_EBX) {
    iVar1 = in_EAX - 1;
    do {
      jcopy_sample_rows(unaff_EDI, iVar1, unaff_EDI, in_EAX, 1, num_cols);
      in_EAX++;
    } while (in_EAX < unaff_EBX);
  }
}
```

### Xrefs (3)

| From | Caller | Context |
|------|--------|---------|
| `0x0046ab30` | `compress_output` @ `0x0046aa00` (mislabel — CDSJpeg decompress scanline pump) | Per-component loop when `[writer+0x34] < [writer+0x3c]`; `PUSH [cinfo+0x1c]` then `CALL` |
| `0x0046a923` | `pre_process_data` @ `0x0046a870` | Mid-buffer refill: `EBX = cinfo+0xdc` (`image_width`), `EAX = [writer+0x34]`, `EDI = *component_buf` |
| `0x0046a9d9` | `pre_process_data` @ `0x0046a870` | Final padding: `PUSH width×8` (component block stride), same register setup |

### mapping.csv (reference)

```
;_Globals::FUN_0046a840;0x46a840;0x2a;__cdecl;;void;size_t
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `force_decompile` | `0x0046a840` | OK — R8 plate/decompiler comments present |
| *(none)* | — | No rename/prototype change; no `save_program` |

## Frida

**none** — Static disasm + xref closure + IJG `expand_bottom_edge` body match sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Rename to `expand_bottom_edge` | **Blocked** — LOCAL split, no COFF export symbol |
| Decompiler register args (`in_EAX` / `unaff_*`) | **Cosmetic** — MSVC register-param convention at call sites |
| `jcopy_sample_rows@0x0045f810` | Still `STUB_BODY()` — callee not in scope for task 059 |
| objdiff byte match | Requires MSVC x86 compile of slice + `jcopy_sample_rows` body; out of scope unless build unblocks |
| `compress_output` @ `0x0046aa00` mislabel | **Out of scope** — separate task |

## WRITE deliverable

| File | Change |
|------|--------|
| `src/bulanci/_Globals.cpp` | `FUN_0046a840` — `STUB_BODY()` replaced with `expand_bottom_edge` homolog; `__asm` captures EDI/EAX/EBX at entry per live disasm |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round8_fun_task_17_report.md](round8_fun_task_17_report.md)
- [round8_fun_task_44_report.md](round8_fun_task_44_report.md) — caller `pre_process_data@0x0046a870`
- [r9_globals_task_044_report.md](r9_globals_task_044_report.md) — jcprepct LOCAL WRITE precedent
