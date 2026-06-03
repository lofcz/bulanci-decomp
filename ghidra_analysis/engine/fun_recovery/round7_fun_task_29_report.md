# Round 7 — FUN task 29 report

## Task

| Field | Value |
|-------|-------|
| **id** | 29 |
| **band** | dispatch |
| **seed_address** | `0x00467690` |
| **prior** | R6 logic task 48 — coef-controller scan cursor reset; exact IJG symbol deferred |

## Status

**OK** — Disasm + decompile + xref closure match IJG `jccoefct.c` `start_iMCU_row` (LOCAL). Renamed in Ghidra; `__fastcall` prototype and decompiler comment applied; program saved.

## Function

| Address | Ghidra name (before) | Ghidra name (after) | Role | Evidence |
|---------|---------------------|---------------------|------|----------|
| `0x00467690` | `FUN_00467690` | `start_iMCU_row` | **IJG coef-controller iMCU-row counter reset:** after completing an iMCU row (`coef->iMCU_row_num` already incremented by caller), sets `MCU_rows_per_iMCU_row` and zeros `mcu_ctr` / `MCU_vert_offset` on `cinfo->coef` (`cinfo+0x148`) | Disasm ECX=cinfo; field offsets match `my_coef_controller` (+8/+0xc/+0x10/+0x14) and `jpeg_compress_struct` (+0xe0/+0xe4/+0xe8); control-flow identical to libjpeg-6b `start_iMCU_row` |

### Disassembly (`0x00467690`–`0x004676d5`)

```
00467690  MOV EAX,[ECX+0x148]     ; coef = cinfo->coef
00467696  MOV EDX,1               ; default MCU_rows_per_iMCU_row = 1 (interleaved)
0046769b  CMP [ECX+0xe4],EDX      ; comps_in_scan < 2 ?
004676a1  JG  0x004676ca          ; interleaved → skip v_samp/last_row branch
004676a3  PUSH ESI
004676a4  MOV ESI,[ECX+0xe0]      ; total_iMCU_rows
004676aa  MOV ECX,[ECX+0xe8]      ; cur_comp_info[0]
004676b0  SUB ESI,1
004676b2  CMP [EAX+0x8],ESI       ; iMCU_row_num < total_iMCU_rows-1 ?
004676b5  POP ESI
004676b6  JNC 0x004676c7
004676b8  MOV EDX,[ECX+0xc]       ; v_samp_factor
004676bb  XOR ECX,ECX
004676bd  MOV [EAX+0x14],EDX      ; MCU_rows_per_iMCU_row
004676c0  MOV [EAX+0xc],ECX       ; mcu_ctr = 0
004676c3  MOV [EAX+0x10],ECX      ; MCU_vert_offset = 0
004676c6  RET
004676c7  MOV EDX,[ECX+0x48]      ; last_row_height (bottom iMCU row)
004676ca  XOR ECX,ECX
004676cc  MOV [EAX+0x14],EDX
004676cf  MOV [EAX+0xc],ECX
004676d2  MOV [EAX+0x10],ECX
004676d5  RET
```

### Field map (proven)

| Offset | IJG field | Notes |
|--------|-----------|-------|
| `cinfo+0x148` | `cinfo->coef` | coef controller object |
| `coef+0x8` | `iMCU_row_num` | read-only here; callers increment before tail-call |
| `coef+0xc` | `mcu_ctr` | zeroed |
| `coef+0x10` | `MCU_vert_offset` | zeroed |
| `coef+0x14` | `MCU_rows_per_iMCU_row` | 1 if interleaved; else `v_samp_factor` or `last_row_height` |
| `cinfo+0xe0` | `total_iMCU_rows` | |
| `cinfo+0xe4` | `comps_in_scan` | `< 2` ⇒ non-interleaved branch |
| `cinfo+0xe8` | `cur_comp_info` | first component `+0xc` / `+0x48` |

### Callers (3 xrefs, `get_function_xrefs`)

| Caller | Site | Context |
|--------|------|---------|
| `jpeg_compress_data` | `0x0046771d` | After `coef->iMCU_row_num++` at end of coef write pass |
| `FUN_00467930` | `0x00467ac2` | Same tail after multi-scan MCU coordinator finishes iMCU row |
| `FUN_00467d10` | `0x00467d25` | Coef `start_pass`: resets `iMCU_row_num=0` then initializes first row |

All three are in the IJG compressor coef-controller cluster ([round6_logic_task_48_report.md](../logic_recovery/round6_logic_task_48_report.md)).

### Upstream match

libjpeg-6b `jccoefct.c` `LOCAL(void) start_iMCU_row(j_compress_ptr cinfo)` — comment: *"Reset within-iMCU-row counters for a new row"*. Branch structure and assignments match byte-for-byte in pseudocode.

## Ghidra deltas

| Action | Address | Detail |
|--------|---------|--------|
| `rename_function_by_address` | `0x00467690` | `FUN_00467690` → `start_iMCU_row` |
| `set_function_prototype` | `0x00467690` | `void start_iMCU_row(void)` + `__fastcall` (ECX = `cinfo`) |
| `set_decompiler_comment` | `0x00467690` | IJG role + field offsets + caller note |
| `force_decompile` | `0x00467690` | Refresh after rename |
| `save_program` | `bulanci.exe` | Once after mutations |

## Frida

**none** — Static IJG codec plumbing; no gameplay-visible state.

## Remaining UNK

- **`_Globals::` namespace** — Ghidra still nests under `_Globals::start_iMCU_row`; `mapping.csv` still lists `_Globals::FUN_00467690` until mapping pass.
- **`jpeg_compress_struct *` typing** — prototype uses implicit ECX; full struct type not applied (no `jpeg_compress_struct` DT in Ghidra yet).
- **Sibling coef helpers** — `FUN_00467930`, `FUN_00467d10`, `FUN_00467600` remain `FUN_*` (other R7 tasks).

## Evidence paths consulted

- [ROUND7_FUN_PROTOCOL.md](../ROUND7_FUN_PROTOCOL.md), [GHIDRA_MCP.md](../GHIDRA_MCP.md)
- [round6_logic_task_48_report.md](../logic_recovery/round6_logic_task_48_report.md)
- [jpeg_decoder.md](../../formats/jpeg_decoder.md)
- `config/bulanci/mapping.csv` (`0x467690`)
- libjpeg-6b `jccoefct.c` `start_iMCU_row` (upstream control-flow reference)
