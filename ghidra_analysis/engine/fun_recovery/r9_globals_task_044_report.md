# Round 9 `_Globals` — Task 044 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 44 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec |
| **seed_address** | `0x00460f30` |
| **ghidra_name** | `FUN_00460F30` |
| **prior_hint** | R7/R8 — jcprepct edge expand; caller prep vtable |
| **prior art** | [round7_fun_task_42_report.md](round7_fun_task_42_report.md); [round8_fun_task_06_report.md](round8_fun_task_06_report.md) |

## Status

**PARTIAL** — Role and xref closure re-verified via live Ghidra MCP (`connect_instance bulanci`, 2026-06-04). **WRITE:** `src/bulanci/_Globals.cpp` stub replaced with decompiler-matched body. **No rename:** IJG `jcprepct.c` **LOCAL** edge-replication tail (compiler split from `create_context_buffer`); no standalone COFF/export symbol (ROUND9 no-guess rule). Ghidra prototype/comments from R7 intact; **no program mutations** this pass.

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x00460f30` | `FUN_00460F30` | **Context-row edge replication** for prep-controller `color_buf`: per `num_components`, three loops mirror/replicate fake row-pointer tables at `prep+0x38` / `prep+0x3c` and left-edge samples (jcprepct `create_context_buffer` tail homolog) | Live `force_decompile`; body `00460f30`–`00461071` (322 B / `0x142`); signature `void __cdecl FUN_00460f30(int cinfo)`; **1× CODE xref** |

### IJG field mapping (this binary)

| Offset | Role |
|--------|------|
| `cinfo+0x24` | `num_components` |
| `cinfo+0xc4` | `comp_info`; loop stride `0x54` (`0x15` dwords); `samples_per_row = [comp+0x24]*[comp+0xc] / max_v` |
| `cinfo+0x118` | `max_v_samp_factor` |
| `cinfo+0x184` | prep workspace |
| `prep+0x38` / `prep+0x3c` | real vs fake `JSAMPROW` pointer tables |
| `prep+0x08[ci]` | per-component row base (`row_base` in decompile) |

### Algorithm (three loops per component)

1. **`(max_v_samp_factor + 2) × samples_per_row`** — mirror-copy between fake (`+0x3c`) and real (`+0x38`) pointer arrays using `row_base`.
2. **`2 × samples_per_row`** — interleave-copy between row groups in fake buffer (vertical context mirroring).
3. **`samples_per_row`** — replicate leftmost sample leftward (`real_rows - samples_per_row` .. `real_rows`).

### Disassembly highlights

| VA | Proof |
|----|-------|
| `0x00460f37` / `0x00460f40` | `max_v_samp_factor`, `num_components` from `[cinfo+0x118]`, `[cinfo+0x24]` |
| `0x00460f44` / `0x00460f4e` | prep `[cinfo+0x184]`, `comp_info` `[cinfo+0xc4]` |
| `0x00460f7c`–`0x00460f83` | `samples_per_row` = `IMUL` / `IDIV` by `max_v_samp_factor` |
| `0x00460f89`–`0x00460fd4` | Loop 1: `(max_v+2)*samples` mirror copies |
| `0x00460fe2`–`0x0046101a` | Loop 2: `2*samples` vertical interleave |
| `0x00461028`–`0x0046104a` | Loop 3: left-edge replicate |
| `0x00461055` | Component advance `comp_info += 0x54` |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole CODE)** | `0x0046142e` | `FUN_004613e0` | `CALL FUN_00460f30` on prep `start_pass` when `[cinfo+0x1a0+8]!=0` (`need_context_rows`); sets `prep+4 = 0x461270` first |
| **Vtable install** | `0x00461483` | `FUN_00461460` | `mov [prep], 0x4613e0` |
| **Alloc head (sibling)** | `0x004614c6` | `create_context_buffer` | Allocates `prep+0x38/+0x3c` before edge replication |

Caller disasm @ `0x0046142e`:

```
0046141b  MOV ECX,[EAX+0x1a0]       ; downsample module
00461421  CMP byte ptr [ECX+0x8],0   ; need_context_rows
00461427  MOV [ESI+0x4],0x461270     ; pre_process_context homolog
0046142e  CALL 0x00460f30
00461433  ADD ESP,0x4
```

### mapping.csv (reference)

```
;_Globals::FUN_00460f30;0x460f30;0x142;__cdecl;;void;int
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| *(none)* | — | R7 prototype + plate/decompiler comments already present; no `save_program` |

## Frida

**none** — Static decompile/disasm + IJG `jcprepct.c` cluster match; sole xref at pass start.

## Remaining UNK

| Item | Reason |
|------|--------|
| Exact IJG static name | Edge loops are **inline** in IJG 6b `create_context_buffer`; MSVC split into `create_context_buffer@0x460e80` + this tail — no COFF label |
| `FUN_004613e0` / `LAB_00461270` naming | Prep vtable methods; separate tasks |
| objdiff byte match | Requires full TU compile of `_Globals.cpp` slice; out of scope unless build unblocks |

## WRITE deliverable

| File | Change |
|------|--------|
| `src/bulanci/_Globals.cpp` | `FUN_00460f30` — `STUB_BODY()` replaced with body matching Ghidra `force_decompile` @ `0x00460f30` |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round7_fun_task_42_report.md](round7_fun_task_42_report.md)
- [round8_fun_task_06_report.md](round8_fun_task_06_report.md)
- [round8_fun_task_09_report.md](round8_fun_task_09_report.md) — caller `FUN_004613e0`
