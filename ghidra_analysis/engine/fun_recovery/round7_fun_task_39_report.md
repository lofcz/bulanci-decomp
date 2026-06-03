# Round 7 FUN — Task 39 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 39 |
| **round** | 7 |
| **band** | dispatch |
| **seed_address** | `0x00460160` |
| **title** | FUN recovery: FUN_00460160 @ 0x00460160 (xrefs=1) |
| **prior_hint** | *(empty)* |

## Status

**DONE** — Body matches IJG **`prepare_range_limit_table`** (`jdmaster.c`, LOCAL static). Renamed in Ghidra; prior R6 plate text (`jdmainct.c` / “main-controller”) corrected.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x00460160` | `FUN_00460160` | `prepare_range_limit_table` | Build **`sample_range_limit`** lookup block for libjpeg-6b decompress: `alloc_small(JPOOL_IMAGE, 0x580)`; zero pre-table; identity `0..255`; post-IDCT `0xFF` / `0` segments; 128-byte tail `memcopy` from `cinfo+0x120` | Disasm + decompile; size `0x580 = (5×256+128)×1` for 8-bit IJG; field store `@ cinfo+0x120`; algorithm = `jdmaster.c::prepare_range_limit_table` |

### Disasm proof (highlights)

| Step | Instruction / pattern | IJG source correspondence |
|------|----------------------|---------------------------|
| Alloc | `PUSH 0x580; PUSH 0x1; PUSH EBX; CALL [mem+0]` | `(5*(MAXJSAMPLE+1)+CENTERJSAMPLE)*sizeof(JSAMPLE)` → **1408** bytes |
| Publish ptr | `ADD ESI,0x100; MOV [EBX+0x120],ESI` | `table += (MAXJSAMPLE+1); cinfo->sample_range_limit = table` |
| Pre-table | `memset(base, 0, 0x100)` | `MEMZERO(table-(MAXJSAMPLE+1), …)` |
| Identity | `MOV [EAX+ESI],AL` loop `0..0xff` | `table[i] = (JSAMPLE)i` |
| Post-IDCT hi | `memset(+0x200, 0xff, 0x180)` | fill `MAXJSAMPLE` for `i in [CENTERJSAMPLE .. 2*(MAXJSAMPLE+1))` |
| Post-IDCT lo | `memset(+0x380, 0, 0x180)` | second-half zero region |
| Tail copy | `MOVSD.REP` × `0x20` (128 B) from `[EBX+0x120]` | `MEMCOPY(..., cinfo->sample_range_limit, CENTERJSAMPLE*sizeof(JSAMPLE))` |

### Xrefs (1)

| From | Context |
|------|---------|
| `0x00460214` | `FUN_00460200` decompress-master init tail: `MOV EBX,ESI` then `CALL prepare_range_limit_table` immediately after `jinit_d_main_controller` |

**Note:** `FUN_00460200` is the compiler-outlined body of **`jinit_master_decompress`** (R6 task 43); not `jdmainct.c`.

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x00460160` | `prepare_range_limit_table` |
| `set_function_prototype` | `0x00460160` | `void prepare_range_limit_table(void *cinfo)` *(jpeg_decompress_struct* type absent in DB)* |
| `set_plate_comment` | `0x00460160` | jdmaster.c LOCAL + caller note |
| `set_decompiler_comment` | `0x00460160` | R7 task39 size/segment proof |
| `force_decompile` | `0x00460160` | OK |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — stock IJG decompress setup; static disasm + IJG source match sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Decompiler `cinfo` param | MSVC outlined chunk still shows `unaff_EBX`; caller passes `cinfo` in **EBX** (`MOV EBX,ESI` @ `0x00460212`) — behavior proven, typing cosmetic |
| `FUN_00460200` export name | Separate R7 task (id 40); tail of same `jinit_master_decompress` path |
| `jpeg_decompress_struct` in Ghidra | No datatype applied; `void *cinfo` only |

## Evidence paths consulted

- [ROUND7_FUN_PROTOCOL.md](../ROUND7_FUN_PROTOCOL.md), [GHIDRA_MCP.md](../GHIDRA_MCP.md)
- [round6_logic_task_43_report.md](../logic_recovery/round6_logic_task_43_report.md) (prior slice; corrected module attribution)
- [jpeg_decoder.md](../../formats/jpeg_decoder.md)
- `config/bulanci/mapping.csv` (`FUN_00460160`, 0x95 B)
- IJG `jdmaster.c::prepare_range_limit_table` (libjpeg-turbo reference; 8-bit size **0x580**)
