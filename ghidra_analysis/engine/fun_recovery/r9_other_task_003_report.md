# Round 9 Other — Task 003 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 3 |
| **round** | 9 (Other unit) |
| **namespace** | global (no `_Globals` mapping) |
| **band** | jpeg_codec vicinity (jcprepct VA cluster; decompress main-buffer wiring) |
| **seed_address** | `0x004613e0` |
| **ghidra_name** | `FUN_004613e0` |
| **prior_hint** | R8 task 9 — prep vtable `start_pass` homolog (jcprepct label; corrected below) |
| **prior art** | [round8_fun_task_09_report.md](round8_fun_task_09_report.md), [r9_globals_task_047_report.md](r9_globals_task_047_report.md), [r9_globals_task_044_report.md](r9_globals_task_044_report.md) |

## Status

**PARTIAL** — Live Ghidra MCP re-verify (`connect_instance("bulanci")`, 2026-06-04) proves role and xref closure. **No rename:** IJG `jdmainct.c` **METHODDEF** `start_pass_main` is `LOCAL` (vtable slot `start_pass`, not a standalone COFF/export). Plate + decompiler comments corrected from prior jcprepct mislabel; `bulanci.exe` saved.

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x004613e0` | `FUN_004613e0` | **Main-buffer controller `start_pass` (vtable slot 0)** — homolog of IJG `jdmainct.c` `start_pass_main`; dispatches `J_BUF_MODE`, wires workspace `+4` (`process_data` method), and on context path calls edge-replication tail | 120 B (`0x78`, body `004613e0`–`00461457`); **1×** DATA xref; live decompile + disasm; behavior match includes `JBUF_CRANK_DEST(2)` branch absent from `jcprepct.c` |

### Signature / size

| Source | Value |
|--------|-------|
| Ghidra (live) | `void __cdecl FUN_004613e0(int * cinfo, int pass_mode)` |
| Body | `004613e0`–`00461457` → **120 B** (`0x78`) |
| `config/bulanci/mapping.csv` | Comment row: `void __cdecl`; size **`0x78`**; `int*`, `int` — matches Ghidra |

### IJG homolog proof (`start_pass_main`, not `start_pass_prep`)

| `pass_mode` | IJG `jdmainct.c` `start_pass_main` | This binary (`FUN_004613e0`) |
|-------------|-------------------------------------|------------------------------|
| `0` + `need_context_rows` | `process_data = process_data_context_main`; `make_funny_pointers` | `prep+4 = 0x461270`; `CALL FUN_00460f30` @ `0x0046142e` |
| `0` no context | `process_data = process_data_simple_main`; reset counters | `prep+4 = 0x461200`; zero `prep+0x30/+0x34` |
| `2` | `process_data = process_data_crank_post` | `prep+4 = 0x4613b0` @ `0x00461411` |
| other | `ERREXIT(JERR_BAD_BUFFER_MODE)` → `msg_code=4` | `msg_code=4` + indirect `error_exit` @ `0x004613fd`–`0x00461409` |

**Disqualifier for jcprepct:** IJG `jcprepct.c` `start_pass_prep` accepts **only** `JBUF_PASS_THRU` and errors on all other modes — no `JBUF_CRANK_DEST(2)` branch. This binary implements mode `2` → strong match to **decompress** `jdmainct.c`.

### Decompress path closure

| Step | VA / symbol | Proof |
|------|-------------|-------|
| Module wiring | `master_selection@0x00460200` | Decompress `master_selection`; calls `FUN_00461460` when `[cinfo+0x41]==0` (`raw_data_out`) |
| Buffer init | `FUN_00461460@0x00461460` | `alloc_small(0x50)` → `cinfo+0x184`; stores method head `0x4613e0` @ `0x00461483` |
| Runtime invoke | indirect | Pass driver calls workspace `+0` (`start_pass` slot); no direct CODE xrefs to `0x4613e0` |

### IJG field mapping (this binary)

| Offset | Role |
|--------|------|
| `cinfo+0x184` (`cinfo[0x61]`) | main-buffer workspace (installed by `FUN_00461460`) |
| `cinfo+0x1a0` (`cinfo[0x68]`) | upsampler module pointer |
| `cinfo+0x1a0+8` | `need_context_rows` |
| workspace `+0` | vtable / method table head → this function @ install |
| workspace `+4` | `process_data` method pointer (runtime switch) |
| workspace `+0x30` / `+0x34` / `+0x40` / `+0x44` / `+0x4c` | pass counters / context state (zeroed on context start) |

### Disassembly highlights (live Ghidra)

| VA | Instruction | Proof |
|----|-------------|-------|
| `0x004613ee` | `MOV ESI, [EAX+0x184]` | Workspace at `cinfo+0x184` |
| `0x004613f4` | `JZ 0x46141b` | `pass_mode == 0` → start-pass path |
| `0x004613f9` | `JZ 0x461411` | `pass_mode == 2` → crank-dest path |
| `0x004613fd`–`0x00461409` | bad mode → `msg_code=4`, `error_exit` | `JERR_BAD_BUFFER_MODE` |
| `0x00461411` | `MOV [ESI+4], 0x4613b0` | `JBUF_CRANK_DEST` processor |
| `0x0046141b`–`0x00461424` | test `[cinfo+0x1a0+8]` | `upsample.need_context_rows` |
| `0x00461427` | `MOV [ESI+4], 0x461270` | context `process_data` |
| `0x0046142e` | `CALL 0x00460f30` | edge-replication tail (R9 task 44 cluster) |
| `0x0046144e` | `MOV [ESI+4], 0x461200` | simple `process_data` |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Reference (sole)** | `0x00461483` | `FUN_00461460` | **DATA** — `mov [prep], 0x4613e0` installs vtable / `start_pass` entry |
| **Callee (context path)** | `0x0046142e` | `FUN_00460f30` | Edge replication at pass start when context rows enabled |
| **Upstream init** | `0x00460354` | `master_selection` | Decompress wiring → `FUN_00461460` when `[cinfo+0x41]==0` |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_plate_comment` | `0x004613e0` | Corrected homolog to `jdmainct.c start_pass_main` + decompress path note |
| `set_decompiler_comment` | `0x004613e0` | Corrected jcprepct mislabel; documented `JBUF_CRANK_DEST` disqualifier |
| `force_decompile` | `0x004613e0` | Refreshed |
| `save_program` | `bulanci.exe` | Saved |

**Not applied:** `rename_function_by_address` — `start_pass_main` is IJG `METHODDEF` / `LOCAL`; `mapping.csv` row is comment-only (`;FUN_004613e0`).

## Frida

**none** — Static decompile/disasm + IJG `jdmainct.c` `start_pass_main` switch structure closes the role.

## Remaining UNK

| Item | Reason |
|------|--------|
| Exact export symbol | `start_pass_main` is **LOCAL** in IJG 6b; vtable stores code pointer, not a COFF label |
| `LAB_00461200` / `LAB_00461270` / `LAB_004613b0` | `process_data` bodies — separate FUN tasks |
| Indirect callers | Pass driver that invokes workspace `+0` not enumerated in this single-VA slice |
| VA cluster vs IJG file | Physically near jcprepct band; functionally decompress main-buffer `start_pass` |

## Evidence paths

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md), [GHIDRA_MCP.md](../GHIDRA_MCP.md)
- [IJG `jdmainct.c` `start_pass_main`](https://github.com/LuaDist/libjpeg/blob/master/jdmainct.c) (`mainp->pub.start_pass = start_pass_main`)
- [IJG `jcprepct.c` `start_pass_prep`](https://github.com/LuaDist/libjpeg/blob/master/jcprepct.c) (rejected: no mode-2 branch)
- [round8_fun_task_09_report.md](round8_fun_task_09_report.md) — prior jcprepct label superseded by mode-2 proof
