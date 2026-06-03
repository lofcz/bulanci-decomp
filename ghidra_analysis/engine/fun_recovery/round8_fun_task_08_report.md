# Round 8 FUN — Task 08 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 8 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x00461160` |
| **title** | FUN recovery: FUN_00461160 @ 0x00461160 (xrefs=1) |
| **prior_hint** | [round7_fun_task_44_report.md](round7_fun_task_44_report.md) — jcprepct row shift |

## Status

**PARTIAL** — Role and sole xref closure re-verified via live Ghidra MCP (`connect_instance bulanci`, 2026-06-03). **No rename:** MSVC-extracted **LOCAL** from IJG `jcprepct.c` `pre_process_context` / context-buffer cluster; no standalone COFF or IJG export symbol (Round 8 no-guess rule). Ghidra already holds correct `void __cdecl` prototype, plate comment, and decompiler comment from Round 7 — **no program mutations** this pass.

## Function

| Address | Ghidra name | Size | CC | Role summary | Evidence |
|---------|-------------|-----:|-----|--------------|----------|
| `0x00461160` | `FUN_00461160` | 154 B (`0x9A`) | `__cdecl` | **Context-mode prep row-pointer shift:** per component, slide fake `color_buf` `JSAMPROW` dwords left by `2 × samples_per_row` and recompute `prep->next_buf_stop` at `prep+0x48` (comp 0 only). | Live decompile; disasm 62 insns / 12 BB; 1× `UNCONDITIONAL_CALL` @ `0x00461331`; IJG `jcprepct.c` cluster ([task 43](round7_fun_task_43_report.md) call chain) |

### IJG field mapping (this binary)

| Offset | IJG / prep role |
|--------|-----------------|
| `cinfo+0x24` (`cinfo[9]`) | `num_components` |
| `cinfo+0xc4` (`cinfo[0x31]`) | `comp_info` (stride `0x54` / Ghidra `0x15` dwords) |
| `cinfo+0x118` (`cinfo[0x46]`) | `max_v_samp_factor` |
| `cinfo+0x184` (`cinfo[0x61]`) | prep workspace (`cinfo->prep`) |
| `comp+0x00`, `comp+0x18` | `DCT_h_scaled_size`, `width_in_blocks` → `samples_per_row = width_in_blocks × DCT_h_scaled_size` |
| `comp+0x20` (`piVar6[8]`) | `next_buf_row` (modulo `samples_per_row`; zero → treat as full period) |
| `prep+0x38`, `prep+0x3c`, `prep+0x40` | fake/real `JSAMPROW` tables + active table index |
| `prep+0x48` | `next_buf_stop` (IJG `my_prep_controller`) |

### Algorithm (live decompile summary)

For each component `ci`:

1. `samples_per_row = comp->width_in_blocks × comp->DCT_h_scaled_size` (decompile: `piVar6[6] * *piVar6`).
2. `row_slots = samples_per_row / max_v_samp_factor`; `idx = next_buf_row % samples_per_row` (zero → `samples_per_row`).
3. If `ci == 0`: `prep->next_buf_stop = (idx - 1) / row_slots + 1` at `prep+0x48`.
4. At `fake_buffer[ci] + idx` (dword index into row-pointer ring): copy `2 × row_slots` consecutive `JSAMPROW` pointers from slot `idx-1` leftward (`*dst = *(dst-1)` loop).

Matches compiler-split logic from the **context-row** path of IJG `pre_process_context` (pointer advance / stop update after a downsample step), not a named function in stock `jcprepct.c`.

### Disassembly highlights (`0x00461160`–`0x004611f9`)

| VA | Proof |
|----|-------|
| `0x00461164` | `cinfo` from stack (`[ESP+0x14]`) |
| `0x00461168` / `0x0046116e` | `prep = [cinfo+0x184]`, `comp_info = [cinfo+0xc4]` |
| `0x00461176` | `num_components` at `[cinfo+0x24]` |
| `0x00461187`–`0x0046119d` | `IMUL` width × blocks; `IDIV` by `[cinfo+0x118]`; `DIV` for `next_buf_row % samples_per_row` |
| `0x004611b9` | Store `prep+0x48` on first component only |
| `0x004611cc`–`0x004611e3` | Inner shift: `LEA` row base + `idx*4`; dword copy loop `2*row_slots` times |
| `0x004611ec` | Component stride `comp_info += 0x54` |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole CODE)** | `0x00461331` | `LAB_00461270` homolog (`pre_process_context` vtable slot) | `CALL FUN_00461160` with `cinfo` on stack; bytes `E8 2A FE FF FF` |
| **Sibling (same block)** | `0x00461365` | downsampler `pre_process_data` indirect | After row shift |
| **Sibling** | `0x0046137d` | `FUN_00461080` | Row replicate when `[prep+0x4c]==1` ([R7 task 43](round7_fun_task_43_report.md)) |
| **Pass-start edge** | `0x0046142e` | `FUN_004613e0` | Calls `FUN_00460f30` once ([R7 task 42](round7_fun_task_42_report.md)) |
| **Vtable head** | `0x00461483` | `FUN_00461460` | Installs `&LAB_004613e0` at `prep+0` |

Caller snippet (from [R7 task 43](round7_fun_task_43_report.md), confirmed `search_instructions` operand `00461160` → single hit @ `0x00461331`):

```
00461330  PUSH EDI                 ; cinfo
00461331  CALL 0x00461160          ; row-pointer shift + next_buf_stop
00461365  CALL [downsample+4]
00461377  CMP dword ptr [ESI+0x4c],1
0046137d  CALL 0x00461080          ; top/bottom context replicate
```

### Callees

None (leaf helper).

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| — | `0x00461160` | **Skipped** — R7 already applied `set_function_prototype` (`void __cdecl FUN_00461160(int * cinfo)`), plate comment, decompiler comment; live `get_function_documentation` confirms |
| `save_program` | — | **Not run** (no mutations) |

**Not applied:** `rename_function_by_address` — no unique IJG export; `mapping.csv` still lists erroneous `uchar` return (export regen out of scope).

## Frida

**none** — JPEG prep-controller path; static xref + decompile + IJG source cluster sufficient.

## Remaining UNK

| Item | Why still UNK |
|------|----------------|
| Exact IJG static name | Logic is inline in `pre_process_context` in stock libjpeg-6b; MSVC emitted a separate `FUN_00461160` — no COFF label |
| `LAB_00461270` function boundary | Ghidra has no function at caller entry; vtable method body treated as label — naming deferred to prep-controller slice |
| `mapping.csv` / `_Globals.cpp` | Still `FUN_00461160` / `uchar`; stale vs Ghidra `void` |
