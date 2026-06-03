# Round 7 FUN — Task 47 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 47 |
| **round** | 7 |
| **band** | dispatch |
| **seed_address** | `0x00462980` |
| **title** | FUN recovery: FUN_00462980 @ 0x00462980 (xrefs=1) |
| **prior_hint** | round6_logic_task_45 — MCU bit-buffer *(plate was wrong; see below)* |

## Status

**DONE** — Live Ghidra decompile, disasm, and xref prove IJG `jdhuff.c` LOCAL **`process_restart`** (baseline `decode_mcu` copy). Renamed with address disambiguator because `process_restart` @ `0x00462f80` already names the progressive-MCU variant.

## Function

| Address | Before | After | Role | Evidence |
|---------|--------|-------|------|----------|
| `0x00462980` | `FUN_00462980` | `process_restart_00462980` | IJG **`process_restart`**: when `cinfo->restart_interval` (`+0xfc`) and `entropy->restarts_to_go==0` (`+0x24` on baseline entropy), discard bit-buffer remainder, call `marker->read_restart_marker`, zero `last_dc_val[]`, reload `restarts_to_go`, optionally clear `insufficient_data` | Sole caller `decode_mcu@0x00462a22`; 126 B (`0x7e`); structural match to `process_restart@0x00462f80` with progressive layout offsets (`+0x28`, coef clear at `+0x18`); IJG `jdhuff.c` LOCAL called from `decode_mcu` |

### Caller gate (`decode_mcu` @ `0x00462a00`)

| VA | Instruction | Proof |
|----|-------------|-------|
| `0x00462a04` | `MOV ESI, [ESP+0x44]` | `ESI` = `j_decompress_ptr cinfo` (`param_1`) |
| `0x00462a08` | `CMP [ESI+0xfc], 0` | `restart_interval != 0` |
| `0x00462a1c` | `CMP [EDI+0x24], 0` | `restarts_to_go == 0` (baseline entropy layout) |
| `0x00462a22` | `CALL 0x00462980` | Only xref to this function |
| `0x00462a27` | `TEST AL, AL` | `boolean` return — suspend on `FALSE` |

### Disasm highlights (`process_restart_00462980`)

| VA | Instruction | IJG step |
|----|-------------|----------|
| `0x0046298d`–`0x00462999` | `bits_left` (`[EDI+0x10]`) → `SAR 3` → add to `[marker+0x14]` | `discarded_bytes += bits_left/8`; clear `bits_left` |
| `0x004629a9`–`0x004629ad` | `CALL [marker+0x8]` with `ESI=cinfo` | `read_restart_marker` |
| `0x004629c2`–`0x004629e2` | Zero loop `[EDI+0x14]` × `comps_in_scan` (`[ESI+0x124]`) | Reset `last_dc_val[ci]` |
| `0x004629e4`–`0x004629ea` | `[EDI+0x24] = [ESI+0xfc]` | `restarts_to_go = restart_interval` |
| `0x004629ed`–`0x004629f6` | If `[ESI+0x17c]==0` → `[EDI+0x8]=0` | Clear `insufficient_data` when no pending marker |

### Relation to `process_restart` @ `0x00462f80`

| Field | `0x00462980` (this task) | `0x00462f80` (R5 worker 49) |
|-------|--------------------------|-----------------------------|
| Callers | `decode_mcu` only | `jpeg_decode_mcu_DC_first`, `jpeg_decode_mcu_AC_first`, `jpeg_decode_mcu_AC_refine` |
| `restarts_to_go` offset | `entropy+0x24` | `entropy+0x28` |
| DC zero loop base | `entropy+0x14` | `entropy+0x18` (+ clears `entropy+0x14`) |
| Size | `0x7e` | `0x85` |

Binary contains two specialized copies of the same IJG LOCAL (MSVC out-of-line / layout specialization), not a bit-buffer helper.

### Control flow

```mermaid
flowchart LR
  MCU[decode_mcu 0x00462a00]
  PR[process_restart_00462980 0x00462980]
  RRM[marker read_restart_marker]
  MCU -->|"restart_interval && restarts_to_go==0"| PR
  PR --> RRM
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x00462980` → `process_restart_00462980` | Success (PascalCase warnings only) |
| `set_plate_comment` | `0x00462980` | IJG role + duplicate note |
| `save_program` | `bulanci.exe` | Saved |

**Not applied:** `set_function_prototype` — Ghidra shows `undefined4 __stdcall` with implicit `ESI=cinfo`; IJG uses `boolean process_restart(j_decompress_ptr)` via register at this call site.

## Frida

**none** — Static xref + disasm + IJG source correspondence sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Bare `process_restart` name | Collision with `0x00462f80`; address suffix disambiguates |
| Prototype / calling convention | Implicit `ESI=cinfo` not modeled in decompiler signature |
| `mapping.csv` / `_Globals.cpp` | Still `FUN_00462980` until pipeline refresh |
| Entropy struct layout | Why baseline uses `+0x24` vs progressive `+0x28` for same IJG field — struct recovery follow-up |

## Evidence paths consulted

- [ROUND7_FUN_PROTOCOL.md](../ROUND7_FUN_PROTOCOL.md)
- [GHIDRA_MCP.md](../GHIDRA_MCP.md)
- [logic_recovery/round6_logic_task_45_report.md](../logic_recovery/round6_logic_task_45_report.md)
- [struct_recovery/round5_worker_15_report.md](../struct_recovery/round5_worker_15_report.md)
- [struct_recovery/round5_worker_49_report.md](../struct_recovery/round5_worker_49_report.md)
- [formats/jpeg_decoder.md](../../formats/jpeg_decoder.md)
- IJG `jdhuff.c` `LOCAL process_restart` / `decode_mcu` (libjpeg-turbo/ijg)
