# Round 9 `_Globals` — Task 046 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 46 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec |
| **seed_address** | `0x00461160` |
| **ghidra_name** | `FUN_00461160` |
| **prior_hint** | R7 — jcprepct row shift |
| **prior art** | [round7_fun_task_44_report.md](round7_fun_task_44_report.md), [round8_fun_task_08_report.md](round8_fun_task_08_report.md), [round7_fun_task_43_report.md](round7_fun_task_43_report.md) |

## Status

**PARTIAL** — Live Ghidra MCP re-verify (`connect_instance("bulanci")`, 2026-06-04) confirms R7/R8 role and xref closure. **No rename:** MSVC-extracted **LOCAL** from IJG `jcprepct.c` `pre_process_context` / context-buffer cluster; no standalone COFF or IJG export symbol (ROUND9 no-guess rule). **`_Globals.cpp` body written** from live decompile (replaces `STUB_BODY`).

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x00461160` | `FUN_00461160` | **Context-mode prep row-pointer shift:** per component, slide fake `color_buf` `JSAMPROW` dwords left by `2 × (samples_per_row / max_v_samp_factor)` and set `prep->next_buf_stop` at `prep+0x48` on component 0 only | Live decompile + disasm (62 insns / 12 BB); **1×** `UNCONDITIONAL_CALL` @ `0x00461331`; leaf (no callees) |

### Signature / size

| Source | Value |
|--------|-------|
| Ghidra (live) | `void __cdecl FUN_00461160(int * cinfo)` |
| Body | `00461160`–`004611f9` → **154 B** (`0x9A`) |
| `config/bulanci/mapping.csv` | `void __cdecl`; size **`0x9a`**; `int*` param — matches Ghidra |
| `_Globals.cpp` | Body transcribed from live decompile (this session) |

### IJG `jpeg_compress_struct` / prep field mapping (this binary)

| Offset | `cinfo[]` / prep | Role in this helper |
|--------|------------------|---------------------|
| `+0x24` | `[9]` | `num_components` |
| `+0xc4` | `[0x31]` | `comp_info` base |
| `+0x118` | `[0x46]` | `max_v_samp_factor` |
| `+0x184` | `[0x61]` | prep workspace (`cinfo->prep`) |
| `comp+0x0c`, `comp+0x24` | via `comp+0xc` stride | `samples_per_row = [comp+0xc] × [comp+0x24]` (decompile: `comp[0]*comp[6]` with `comp` at `comp_info+0xc`) |
| `comp+0x20` | `[8]` from shifted `comp` | `next_buf_row`; `idx = row % samples_per_row` (zero → full `samples_per_row`) |
| `prep+0x38`, `+0x3c`, `+0x40` | — | fake/real `JSAMPROW` tables + active table index |
| `prep+0x48` | — | `next_buf_stop` (written when `comp_idx == 0`) |

Component stride: **`0x54`** bytes (`comp += 0x15` dwords per loop).

### Algorithm (live decompile summary)

For each component `ci` while `ci < num_components`:

1. `samples_per_row = comp[width×blocks fields at +0xc/+0x24]`.
2. `row_slots = samples_per_row / max_v_samp_factor`; `idx` from `next_buf_row % samples_per_row` (0 → `samples_per_row`).
3. If `ci == 0`: `prep+0x48 = (idx - 1) / row_slots + 1`.
4. At `fake_buffer[ci] + idx`: copy `2 × row_slots` consecutive row-pointer dwords with `*dst = *(dst-1)` (left shift by one slot per iteration).

Matches compiler-split logic from the **context-row** path of IJG `pre_process_context` (not a named symbol in stock `jcprepct.c`).

### Disasm anchors (live Ghidra)

| VA | Instruction | Proof |
|----|-------------|-------|
| `0x00461164` | `MOV ESI,[ESP+0x14]` | `cinfo` parameter |
| `0x00461168` / `0x0046116e` | `MOV EAX,[ESI+0x184]` / `MOV EBX,[ESI+0xc4]` | prep + `comp_info` |
| `0x00461187`–`0x0046119d` | `IMUL` / `IDIV` / `DIV` | `samples_per_row`, `row_slots`, `idx` |
| `0x004611b9` | `MOV [EAX+0x48],ECX` | `next_buf_stop` on comp 0 |
| `0x004611d8`–`0x004611e3` | dword copy loop | `2 × row_slots` shifts |
| `0x004611ec` | `ADD EBX,0x54` | component stride |

### Xref closure

| Direction | Address | Symbol | Notes |
|-----------|---------|--------|-------|
| **Caller (sole CODE)** | `0x00461331` | `LAB_00461270` homolog (`pre_process_context` vtable slot) | `CALL FUN_00461160`; operand confirmed `get_xrefs_to` |
| **Sibling** | `0x00461365` | downsampler `pre_process_data` indirect | After row shift |
| **Sibling** | `0x0046137d` | `FUN_00461080` | Row replicate when `[prep+0x4c]==1` ([R7 task 43](round7_fun_task_43_report.md)) |

Caller snippet:

```
00461330  PUSH EDI                 ; cinfo
00461331  CALL 0x00461160          ; row-pointer shift + next_buf_stop
00461365  CALL [downsample+4]
00461377  CMP dword ptr [ESI+0x4c],1
0046137d  CALL 0x00461080          ; top/bottom context replicate
```

### Callees

None (leaf).

## Ghidra deltas

| Action | Result |
|--------|--------|
| **none** | R7 already applied `set_function_prototype`, plate comment, decompiler pre-comment; live `get_function_documentation` + `force_decompile` confirm — no `save_program` |

## Frida

**none** — JPEG prep-controller path; static xref + decompile + IJG `jcprepct.c` cluster sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Exact IJG static symbol name | Logic inline in stock `pre_process_context`; MSVC split — no COFF label |
| `LAB_00461270` function boundary | Ghidra label for vtable method body; naming deferred to prep-controller slice |
| `FUN_00461080` / `FUN_00460f30` / `FUN_00461460` naming | Sibling jcprepct LOCALs — separate R9 tasks |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round8_fun_task_08_report.md](round8_fun_task_08_report.md)
- [round7_fun_task_44_report.md](round7_fun_task_44_report.md)
- [round7_fun_task_43_report.md](round7_fun_task_43_report.md)
