# Round 7 FUN — Task 38 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 38 |
| **round** | 7 |
| **band** | dispatch |
| **seed_address** | `0x0045f860` |
| **title** | FUN recovery: FUN_0045F860 @ 0x0045f860 (xrefs=1) |
| **prior_hint** | round6_logic_task_42 — candidate `jcopy_block_row` (`jutils.c`) |

## Status

**DONE** — IJG **`jcopy_block_row`** proven: `memcpy(output, input, num_blocks×128)` with cdecl `(void*, void*, int)`, immediate neighbor `jcopy_sample_rows@0x0045f810`, and sole caller pushing `num_blocks=1` in a `+0x80` DCT-block stride loop. Renamed in Ghidra; program saved.

## Function

| Address | Before | After | IJG name | Role | Evidence |
|---------|--------|-------|----------|------|----------|
| `0x0045f860` | `FUN_0045f860` | `jcopy_block_row` | `jcopy_block_row` | Copy `num_blocks` coefficient blocks (`JBLOCK` = 64×`JCOEF` = 128 B each) | Live decompile + disasm; `jutils.c` `memcpy(output_row, input_row, num_blocks * (DCTSIZE2 * sizeof(JCOEF)))`; 27 B (`0x1b`); sits immediately after `jcopy_sample_rows@0x0045f810` |

### Decompile (live)

```c
void __cdecl jcopy_block_row(void *input_row, void *output_row, int num_blocks)
{
  _memcpy(output_row, input_row, num_blocks << 7);
}
```

### Disasm

| VA | Instruction | Proof |
|----|-------------|-------|
| `0x0045f864`–`0x0045f868` | `MOV` src/dst from `[ESP+4]` / `[ESP+8]` | Arg order: `input_row`, `output_row` |
| `0x0045f860` | `MOV EAX, [ESP+0xc]` | `num_blocks` |
| `0x0045f86c` | `SHL EAX, 7` | ×128 bytes per block |
| `0x0045f872` | `CALL _memcpy@0x00447e90` | MSVCRT thunk |

### Xrefs (1)

| From | Context |
|------|---------|
| `0x00461f8f` | `CALL jcopy_block_row`; preceded by `PUSH 1` (`num_blocks`), `PUSH` dst/src ptrs; loop uses `ADD reg, 0x80` block stride (`0x00461f3b` / `0x00461f40`). **Parent function not defined** in Ghidra (orphan code in `0x00461cd0`–`0x00462370` gap) — not inside `decompress_data@0x004619e0` (ends `0x00461b6e`). Likely `jdcoefct.c` coef-buffer path (cf. IJG `jcopy_block_row(..., 1)` in AC/DC refinement). |

### IJG correspondence

| Check | Match |
|-------|-------|
| File / API | `jutils.c` `GLOBAL(void) jcopy_block_row(JBLOCKROW, JBLOCKROW, JDIMENSION)` |
| Size formula | `num_blocks * 128` ≡ `DCTSIZE2 * sizeof(JCOEF)` |
| Cluster | Adjacent to recovered `jcopy_sample_rows@0x0045f810` (same `jutils.c` translation unit) |
| R6 objdiff 14% | Expected for 27 B `memcpy` thunk — not a semantic mismatch |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `rename_function_by_address` | `0x0045f860` → `jcopy_block_row` | Success |
| `set_function_prototype` | `void __cdecl jcopy_block_row(void *input_row, void *output_row, int num_blocks)` | Success |
| `set_decompiler_comment` | `0x0045f860` | IJG role + caller `0x00461f8f` |
| `force_decompile` | `0x0045f860` | Refreshed |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — Stock libjpeg-6b utility; static IJG source + xref/disasm proof suffices.

## Remaining UNK

| Item | Notes |
|------|-------|
| Caller @ `0x00461f8f` | Ghidra has no function entry covering this site; separate FUN task to create/decompile parent (`consume_data` / AC-refine candidate in `jdcoefct.c` gap) |
| `mapping.csv` / `_Globals.cpp` | Still list `FUN_0045f860` stub — export pipeline out of R7 scope |
| Progressive `jinit` wiring | `puVar3[3]=decompress_data` only; `consume_data` slot not explicitly set in live `jinit_d_coef_controller` decompile (task 32) |
