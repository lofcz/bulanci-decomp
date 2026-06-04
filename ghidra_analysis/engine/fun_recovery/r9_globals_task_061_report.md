# Round 9 `_Globals` — Task 061 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 61 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec |
| **seed_address** | `0x0046c690` |
| **ghidra_name (before)** | `FUN_0046C690` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R8 FUN task 18 (`round8_fun_task_18_report.md`) — caller cluster + `select_scan_parameters` xrefs |

## Status

**DONE** — Live Ghidra MCP (`connect_instance bulanci`, 2026-06-04) confirms IJG **`jcmaster.c` `prepare_for_pass`**: `switch(master->pass_type)` on `cinfo+0x13c+0x10`; calls `select_scan_parameters` + `per_scan_setup`; dispatches compress-module `start_pass` vtables; JERR `0x30` on invalid pass type. Renamed to **`prepare_for_pass`** (upstream IJG export); `save_program bulanci.exe` applied.

## Function

| Address | Ghidra (after) | Role | Evidence |
|---------|----------------|------|----------|
| `0x0046c690` | **`prepare_for_pass`** | **IJG compress master per-pass setup:** branches on `master->pass_type` (`cinfo[0x4f]+0x10`); main / Huffman-opt / output pass paths call `select_scan_parameters` + `per_scan_setup` then module `start_pass` hooks; sets `call_pass_startup` (`master+0xc`) and `is_last_pass` (`master+0xd`) | See below |

### Vtable install (primary xref proof)

`CDSJpegImage::FUN_0046c8f0` (`jinit_compress_master` variant) allocates 0x20-byte master block and stores function pointers:

| Slot | VA | IJG `jpeg_comp_master` field |
|------|-----|------------------------------|
| `[0]` | **`0x0046c690`** | **`prepare_for_pass`** |
| `[1]` | `0x0046c850` | `pass_startup` |
| `[2]` | `LAB_0046c880` | `finish_pass` |

**Ghidra xref:** `FUN_0046c8f0@0x0046c910` → **DATA** → `prepare_for_pass`. Matches `jpegint.h` `struct jpeg_comp_master` slot order.

### Callee chain (compress master cluster)

| Callee | VA | Call sites in `prepare_for_pass` |
|--------|-----|----------------------------------|
| `select_scan_parameters` | `0x0046c3d0` | `0x0046c6d7`, `0x0046c714`, `0x0046c7a8` |
| `per_scan_setup` | `0x0046c4c0` | `0x0046c6dc`, `0x0046c719`, `0x0046c7ad` |

(R8 task 18 documented the three `select_scan_parameters` call sites from this caller.)

### Decompile ↔ IJG `prepare_for_pass` behavior

| Pass | `master+0x10` | Bulanci proof |
|------|---------------|---------------|
| **main_pass** | `0` | `select_scan_parameters` + `per_scan_setup`; if `!raw_data_in` (`cinfo+0xb0`) call cconvert/downsample/prep `start_pass`; fdct/entropy/coef/main `start_pass`; `call_pass_startup = !optimize_coding` (`cinfo+0xb2`) |
| **huff_opt_pass** | `1` | `select_scan_parameters` + `per_scan_setup`; if `Ss!=0 \|\| Ah==0 \|\| arith_code` (`+0x12c`, `+0x134`, `+0xb1`) entropy+coef opt pass; else increment `pass_number`, set `pass_type=2` fallthrough (`0x0046c700`–`0x0046c704`) |
| **output_pass** | `2` | if `!optimize_coding` rescan setup; entropy `start_pass(0)` + coef `JBUF_CRANK_DEST(2)`; marker writer vtable `+4`/`+8` when `master+0x1c==0` |
| **default** | other | `JERR 0x30` (`MOV [cinfo+8],0x30` @ `0x0046c6b4`) + error exit |

All paths set `is_last_pass = (pass_number == total_passes - 1)` (`master+0x14` vs `master+0x18`).

### Signature / size

| Source | Value |
|--------|-------|
| Ghidra (post-rename) | `void __cdecl prepare_for_pass(int * cinfo)`; body `0046c690`–`0046c848` (**0x1b9** B) |
| `config/bulanci/mapping.csv` | `FUN_0046c690`; size **`0x1b9`**; `__cdecl`; `uchar` return *(incorrect — body is void)* |

### Disasm highlights

| VA | Proof |
|----|-------|
| `0x0046c696` | `MOV EDI,[ESI+0x13c]` — `cinfo->master` |
| `0x0046c69c` | `CMP [EDI+0x10],0` / `1` / `2` — pass_type switch |
| `0x0046c6b4` | `MOV [EAX+8],0x30` — JERR_BAD_STATE |
| `0x0046c809`–`0x0046c813` | `(total_passes<=1)-1 & 3` — JBUF_SAVE_AND_PASS vs JBUF_PASS_THRU for coef |

### mapping.csv stub (not trusted)

```
;_Globals::FUN_0046c690;0x46c690;0x1b9;__cdecl;;uchar;int*
```

### `_Globals.cpp` stub (not trusted)

```cpp
uchar _Globals::FUN_0046c690(int* param_1) { STUB_BODY(); return 0; }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0046c690` → `prepare_for_pass` | Success |
| `set_function_prototype` | `void __cdecl prepare_for_pass(int * cinfo)` | Success |
| `set_decompiler_comment` | Entry | IJG role + vtable install + field offsets |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — static libjpeg compress master vtable + decompile/disasm proof sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| `mapping.csv` / `_Globals.cpp` / `_Globals.h` still list `FUN_0046c690` | Separate mapping pass |
| Decompiler `unaff_EDI` at `select_scan_parameters` call sites | **Cosmetic** — ESI holds `cinfo` at entry; callees use register-arg convention |
| `CDSJpegImage::FUN_0046c8f0` upstream name (`jinit_compress_master`) | Out of scope — separate FUN task |
| Sibling slot `[1]` `FUN_0046c850` (`pass_startup`) | Task 062 |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round8_fun_task_18_report.md](round8_fun_task_18_report.md) — `select_scan_parameters` + caller cluster
- IJG `jcmaster.c` `prepare_for_pass` / `jpegint.h` `jpeg_comp_master` (libjpeg-6b)
