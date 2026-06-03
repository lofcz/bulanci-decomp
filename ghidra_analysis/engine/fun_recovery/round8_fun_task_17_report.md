# Round 8 FUN — Task 17 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 17 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x0046a840` |
| **title** | FUN recovery: FUN_0046A840 @ 0x0046a840 (xrefs=3) |
| **prior_hint** | R6 — dispatch UNK |

## Status

**PARTIAL** — Live Ghidra proof: IJG **`jcprepct.c` `expand_bottom_edge` homolog** (duplicate bottom sample row into `[input_rows..output_rows)` via `jcopy_sample_rows`). **No rename** — MSVC-extracted **LOCAL** with no COFF/upstream export (same rule as R7 tasks 42–44).

## Function

| Address | Ghidra (before) | Ghidra (after) | Role | Evidence |
|---------|-----------------|----------------|------|----------|
| `0x0046a840` | `FUN_0046a840` | `FUN_0046a840` | **Bottom-edge row padding:** `for (row = input_rows; row < output_rows) jcopy_sample_rows(image_data, input_rows-1, image_data, row, 1, num_cols)` | Disasm loop @ `0x0046a850`–`0x0046a868`; sole callee `jcopy_sample_rows@0x0045f810`; 0x2d B; matches IJG `expand_bottom_edge` body |

### IJG correspondence

| Check | Match |
|-------|-------|
| Source | `jcprepct.c` `LOCAL(void) expand_bottom_edge(JSAMPARRAY, JDIMENSION num_cols, int input_rows, int output_rows)` |
| Loop | Fixed source row `input_rows-1` (`LEA EBP,[ESI-1]` @ `0x0046a848`, not incremented) |
| Export | **None** — `LOCAL`; MSVC split into standalone `0x0046a840` |

### Register calling convention (MSVC split)

Ghidra shows `in_EAX` / `unaff_EBX` / `unaff_EDI` because callers pass three args in registers; only `num_cols` is on the stack.

| Register / stack | IJG param | Set at call site |
|------------------|-----------|------------------|
| **EDI** | `image_data` (`JSAMPARRAY`) | `[component_row_ptr_array]` |
| **EAX** | `input_rows` | `[writer+0x34]` current row index |
| **EBX** | `output_rows` | `[writer+0x3c]` stop row |
| **stack** | `num_cols` | `PUSH [cinfo+0x1c]` (`image_width`) or `width×8` @ second `FUN_0046a870` site |

### Disassembly (core loop)

```
0046a841  MOV ESI,EAX              ; row = input_rows
0046a843  CMP ESI,EBX              ; row < output_rows?
0046a848  LEA EBP,[ESI-1]          ; source = input_rows-1 (fixed)
0046a850  PUSH [ESP+0xc]           ; num_cols
0046a855  PUSH 1                   ; num_rows
0046a857  PUSH ESI                 ; dest_row (increments)
0046a858  PUSH EDI                 ; output_array
0046a859  PUSH EBP                 ; source_row
0046a85a  PUSH EDI                 ; input_array
0046a85b  CALL jcopy_sample_rows
0046a860  INC ESI
0046a866  CMP ESI,EBX
0046a868  JL  0x0046a850
```

### Xrefs (3)

| From | Caller | Context |
|------|--------|---------|
| `0x0046ab30` | **`compress_output` @ `0x0046aa00`** (mislabel — CDSJpeg decompress scanline pump) | Per-component loop when `[writer+0x34] < [writer+0x3c]`; `PUSH [cinfo+0x1c]` then `CALL`; sets `[writer+0x34] = [writer+0x3c]` after |
| `0x0046a923` | **`pre_process_data` @ `0x0046a870`** | Mid-buffer refill: `EBX = cinfo+0xdc` (`image_width`), `EAX = [writer+0x34]`, `EDI = *component_buf` |
| `0x0046a9d9` | **`pre_process_data` @ `0x0046a870`** | Final padding path: `PUSH width×8` (component block stride), same register setup |

**Note:** Prior plate text “compress_output alloc helper” / “decompress row refill” was incorrect — function performs **row duplication padding**, not allocation.

### Decompile (post-R8)

```c
void __cdecl FUN_0046a840(size_t num_cols)
{
  /* EDI=image_data, EAX=input_rows, EBX=output_rows at entry */
  if (in_EAX < unaff_EBX) {
    iVar1 = in_EAX - 1;
    do {
      jcopy_sample_rows(unaff_EDI, iVar1, unaff_EDI, in_EAX, 1, num_cols);
      in_EAX++;
    } while (in_EAX < unaff_EBX);
  }
}
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x0046a840` → `void __cdecl FUN_0046a840(size_t num_cols)` | OK (was `uchar` in `mapping.csv` stub) |
| `set_decompiler_comment` | Entry | OK — role, register args, callers, no-rename rationale |
| `set_plate_comment` | Entry | OK — `expand_bottom_edge` homolog |
| `force_decompile` | `0x0046a840` | OK |
| `save_program` | `bulanci.exe` | Saved |

**Not applied:** `rename_function_by_address` — `expand_bottom_edge` is `LOCAL` in IJG 6b; not a documented COFF label (R7 jcprepct LOCAL precedent).

## Frida

**none** — Static libjpeg row-padding helper; disasm + xref closure sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Rename to `expand_bottom_edge` | **Blocked** — LOCAL split, no export symbol |
| Decompiler register args (`in_EAX` / `unaff_*`) | **Cosmetic** — MSVC register-param convention at call sites |
| `compress_output` @ `0x0046aa00` mislabel | **Out of scope** — separate R8 task; true IJG `compress_output` is @ `0x00467930` (R7 task 35) |
| `FUN_0046a870` / `pre_process_data` naming | **Done** — R8 task 44 renamed caller @ `0x0046a870` |
| `mapping.csv` / `_Globals.cpp` stub | Still `FUN_0046a840` / `uchar` — export sync separate pass |

## Cross-links

- [round7_fun_task_42_report.md](round7_fun_task_42_report.md), [round7_fun_task_43_report.md](round7_fun_task_43_report.md) — jcprepct LOCAL no-rename precedent
- [round7_fun_task_35_report.md](round7_fun_task_35_report.md) — `compress_output` @ `0x0046aa00` collision note
- [round8_fun_task_44_report.md](round8_fun_task_44_report.md) — caller `pre_process_data@0x0046a870`
- [round6_logic_task_42_report.md](../logic_recovery/round6_logic_task_42_report.md) — `jcopy_sample_rows@0x0045f810`
