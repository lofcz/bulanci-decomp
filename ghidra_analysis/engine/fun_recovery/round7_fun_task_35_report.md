# Round 7 FUN — Task 35 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 35 |
| **round** | 7 |
| **band** | dispatch |
| **seed_address** | `0x00467930` |
| **title** | FUN recovery: FUN_00467930 @ 0x00467930 (xrefs=2) |
| **prior_hint** | *(empty)* |

## Status

**DONE** — Live Ghidra proof matches **IJG libjpeg-6b `jccoefct.c` `compress_output`** (multipass coefficient output pass). Renamed `FUN_00467930` → **`compress_output`**; prototype and comments applied. **Note:** a separate, mislabeled `compress_output` @ `0x0046aa00` (decompress scanline pump, 7-arg) remains — out of seed scope.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x00467930` | `FUN_00467930` | **`compress_output`** | **Coef-controller output pass:** per-scan `access_virt_barray` (`mem+0x20`) for each component; nested MCU loops build `MCU_buffer` pointer list at `coef+0x18` with **0x80** byte stride; **`entropy->encode_mcu`** (`cinfo+0x15c`, slot **+4**); suspend saves `coef+0x10` / `coef+0xc`; completed iMCU row increments `coef+0x8` and **`start_iMCU_row`** via `FUN_00467690` | Decompile + disasm; IJG `jccoefct.c` `compress_output` control-flow (virt barray → MCU ptr fill → `encode_mcu` → `start_iMCU_row`); [round6_logic_task_48_report.md](../logic_recovery/round6_logic_task_48_report.md) install graph |

### Xrefs (2)

| From | Type | Context |
|------|------|---------|
| `0x00467cfd` | tail transfer | **`compress_first_pass`** — `JMP` to `compress_output` after DCT/downsample row work (IJG `return compress_output(cinfo, input_buf)` pattern) |
| `0x00467d8f` | DATA | **`FUN_00467d10`** — installs method ptr when `pass_mode == 2` (`JBUF_CRANK_DEST` → `coef->compress_data = compress_output` in IJG) |

### Disasm highlights

```
00467975  CALL [mem+0x20]           ; access_virt_barray (per component)
00467a03  SHL EAX,7                 ; block stride 0x80 (JCOEF block)
00467a25  MOV [coef+0x18+blkn*4],EAX ; MCU_buffer[] fill
00467a80  CALL [entropy+4]          ; encode_mcu(cinfo, MCU_buffer)
00467ad1  ...                       ; suspend: save coef+0x10, +0xc; return AL=0
00467ac2  CALL FUN_00467690         ; start_iMCU_row after full iMCU row
```

### Offset map (`jpeg_compress_struct *cinfo`, `my_coef_ptr coef = cinfo+0x148`)

| Offset | Field (IJG) | Use in body |
|--------|-------------|-------------|
| `cinfo+0x4` | `mem` | `access_virt_barray` @ vtable **+0x20** |
| `cinfo+0xe4` / `+0xe8` | `comps_in_scan` / `cur_comp_info[]` | component loop |
| `cinfo+0xf8` | `MCUs_per_row` | horizontal MCU bound |
| `cinfo+0x15c` | `entropy` | `encode_mcu` @ **+4** |
| `coef+0x8` | `iMCU_row_num` | virt-barray row index; post-pass increment |
| `coef+0xc` | `mcu_ctr` | suspend/resume MCU column |
| `coef+0x10` | `MCU_vert_offset` | suspend/resume row within iMCU |
| `coef+0x14` | `MCU_rows_per_iMCU_row` | outer vertical loop |
| `coef+0x18` | `MCU_buffer[]` | DCT block pointer array |
| `coef+0x40` | `whole_image[]` | per-component virt array handles |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x00467930` | `FUN_00467930` → **`compress_output`** |
| `set_function_prototype` | `0x00467930` | `bool __cdecl compress_output(int cinfo)` |
| `set_decompiler_comment` | `0x00467930` | IJG `jccoefct.c` role + `0x0046aa00` collision note |
| `set_plate_comment` | `0x00467930` | `libjpeg-6b jccoefct.c :: compress_output` |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — Static IJG compressor coef pass; no gameplay-visible state.

## Remaining UNK

| Item | Reason |
|------|--------|
| `compress_output` @ `0x0046aa00` | **Mislabel** — 7-parameter decompress output pump (`FUN_0046a870` family); should be renamed in a future task to avoid duplicate symbol with this seed |
| `FUN_00467690` | **`start_iMCU_row`** — proven callee; separate R7 manifest entry |
| `FUN_00467d10` | Coef `start_pass` dispatcher — task **37** band neighbor |
| `mapping.csv` / `_Globals.h` | Still list `FUN_00467930`; export sync is a separate pass |
| Second formal param `JSAMPIMAGE input_buf` | IJG source has two args; this build tail-jumps with **cinfo only** (`input_buf` unused in `compress_output` body) |

## Evidence paths

- [ROUND7_FUN_PROTOCOL.md](../ROUND7_FUN_PROTOCOL.md), [GHIDRA_MCP.md](../GHIDRA_MCP.md)
- [round6_logic_task_48_report.md](../logic_recovery/round6_logic_task_48_report.md), [jpeg_decoder.md](../../formats/jpeg_decoder.md)
- `config/bulanci/mapping.csv` (`0x467930`, size `0x1b5`)
