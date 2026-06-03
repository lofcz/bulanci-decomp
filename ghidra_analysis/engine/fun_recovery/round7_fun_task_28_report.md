# Round 7 — FUN task 28 report

## Task

| Field | Value |
|-------|-------|
| **id** | 28 |
| **round** | 7 |
| **band** | dispatch |
| **seed_address** | `0x00461540` |
| **title** | FUN recovery: FUN_00461540 @ 0x00461540 (xrefs=3) |
| **prior_hint** | R6 logic task 44/45 — jdcoefct.c decompress_onepass row-window helper |

## Status

**DONE** — Live Ghidra decompile/disasm + IJG `jdcoefct.c` `LOCAL start_iMCU_row` match (decompress coef path). Symbol already `start_iMCU_row`; plate/decompiler/disasm comments refreshed; program saved.

## Function

| Address | Ghidra name (before) | Ghidra name (after) | IJG name | Role | Evidence |
|---------|---------------------|---------------------|----------|------|----------|
| `0x00461540` | `FUN_00461540` / `start_iMCU_row` | `start_iMCU_row` | `start_iMCU_row` (`jdcoefct.c` LOCAL) | After each completed output iMCU row in **one-pass** coef decode (`jpeg_decompress_data` = IJG `decompress_onepass`), resets `MCU_rows_per_iMCU_row` and zeros `MCU_ctr` / `MCU_vert_offset` on `cinfo->coef` (`cinfo+0x188`). Uses `output_iMCU_row` (`+0x80`) for bottom-row test (lockstep with `input_iMCU_row` after caller’s `++`). | 73 B (`0x49`); `__fastcall` ECX=`cinfo`; byte-match to libjpeg-6b `start_iMCU_row` with `input_iMCU_row` → `output_iMCU_row` substitution at tail call site |

### Disassembly (`0x00461540`–`0x00461588`)

```
00461540  MOV EAX,[ECX+0x188]     ; coef = cinfo->coef
00461546  MOV EDX,1               ; MCU_rows_per_iMCU_row = 1 (interleaved)
0046154b  CMP [ECX+0x124],EDX     ; comps_in_scan > 1 ?
00461551  JG  0x0046157d
00461553  …                       ; non-interleaved: output_iMCU_row vs total_iMCU_rows-1
0046156b  MOV EDX,[comp+0xc]      ; v_samp_factor
0046157a  MOV EDX,[comp+0x48]      ; last_row_height (last iMCU row)
0046157d  MOV [coef+0x1c],EDX     ; MCU_rows_per_iMCU_row
00461582  MOV [coef+0x14],0       ; MCU_ctr
00461585  MOV [coef+0x18],0       ; MCU_vert_offset
00461588  RET
```

### Field map (decompress `jpeg_decompress_struct`)

| Offset | IJG field | Notes |
|--------|-----------|-------|
| `cinfo+0x188` | `cinfo->coef` | `my_coef_ptr` |
| `coef+0x14` | `MCU_ctr` | zeroed |
| `coef+0x18` | `MCU_vert_offset` | zeroed |
| `coef+0x1c` | `MCU_rows_per_iMCU_row` | 1, `v_samp_factor`, or `last_row_height` |
| `cinfo+0x80` | `output_iMCU_row` | row bound (post-increment at caller) |
| `cinfo+0x11c` | `total_iMCU_rows` | |
| `cinfo+0x124` | `comps_in_scan` | |
| `cinfo+0x128` | `cur_comp_info` | first component `+0xc` / `+0x48` |

### Upstream match (`jdcoefct.c`)

```c
LOCAL(void) start_iMCU_row(j_decompress_ptr cinfo)
{
  my_coef_ptr coef = (my_coef_ptr) cinfo->coef;
  if (cinfo->comps_in_scan > 1)
    coef->MCU_rows_per_iMCU_row = 1;
  else if (cinfo->input_iMCU_row < cinfo->total_iMCU_rows-1)
    coef->MCU_rows_per_iMCU_row = cinfo->cur_comp_info[0]->v_samp_factor;
  else
    coef->MCU_rows_per_iMCU_row = cinfo->cur_comp_info[0]->last_row_height;
  coef->MCU_ctr = 0;
  coef->MCU_vert_offset = 0;
}
```

Tail of `decompress_onepass` (`jpeg_decompress_data@0x004615b0`) after `output_iMCU_row++` / `input_iMCU_row++`:

```c
  if (++(cinfo->input_iMCU_row) < cinfo->total_iMCU_rows) {
    start_iMCU_row(cinfo);
    return JPEG_ROW_COMPLETED;  /* 3 */
  }
```

Disasm @ `0x0046178b`–`0x004617b0`: increments `+0x80` and `+0x88`, `CALL 0x00461540`, returns `3`.

### Xrefs

| From | Type | Context |
|------|------|---------|
| `0x004617a9` | **CALL** (proven) | `jpeg_decompress_data` — end of iMCU row, before `return 3` |
| `0x00461993` | **CALL** (insn scan) | Undefined gap between `jpeg_decompress_data` and `decompress_data`; same rel32 target — likely unanalyzed `jdcoefct.c` glue (not decompiled) |
| `0x0046159e` | xref only | No matching `CALL 0x00461540` in instruction search — treat as stale/false |

Manifest `xref_count=3`; **one** closure-quality caller (`jpeg_decompress_data`).

### Pair symbol (compress)

| Address | TU | Role |
|---------|-----|------|
| `0x00467690` | `jccoefct.c` | `start_iMCU_row` (compress); R7 task 29 — `cinfo+0x148`, `iMCU_row_num` |

Same IJG LOCAL name, different addresses — Ghidra holds both as `start_iMCU_row`.

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| (prior) `rename_function_by_address` | `0x00461540` → `start_iMCU_row` | Already applied |
| (prior) `set_function_prototype` | `void start_iMCU_row(void)` `__fastcall` | Already applied |
| `set_decompiler_comment` | `0x00461540` | jdcoefct vs jccoefct pair + caller |
| `set_plate_comment` | `0x00461540` | IJG LOCAL + compress pair |
| `set_disassembly_comment` | `0x004617a9` | Call-site label |
| `force_decompile` | `0x00461540` | Refreshed |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — Stock libjpeg-6b coef-controller plumbing.

## Remaining UNK

| Item | Notes |
|------|-------|
| `mapping.csv` / `_Globals.h` | Still `_Globals::FUN_00461540` — export sync pass |
| `CALL @0x00461993` | Needs function creation / analysis in gap `0x004617ef`–`0x004619df` |
| Duplicate Ghidra name | Two `start_iMCU_row` globals (`0x00461540`, `0x00467690`) — disambiguate in export if tooling requires unique COFF labels |
| `jpeg_decompress_data` naming | IJG `decompress_onepass` — separate R7 scope |

## Evidence paths

- [ROUND7_FUN_PROTOCOL.md](../ROUND7_FUN_PROTOCOL.md), [GHIDRA_MCP.md](../GHIDRA_MCP.md)
- [round6_logic_task_44_report.md](../logic_recovery/round6_logic_task_44_report.md), [round6_logic_task_45_report.md](../logic_recovery/round6_logic_task_45_report.md)
- [round7_fun_task_29_report.md](./round7_fun_task_29_report.md) (compress `start_iMCU_row`)
- [round7_fun_task_32_report.md](./round7_fun_task_32_report.md) (`decompress_data@0x004619e0`)
- [jpeg_decoder.md](../../formats/jpeg_decoder.md), `config/bulanci/mapping.csv` (`0x461540`)
- libjpeg-6b `jdcoefct.c` `start_iMCU_row` / `decompress_onepass`
