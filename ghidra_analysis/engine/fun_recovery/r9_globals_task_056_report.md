# Round 9 `_Globals` — Task 056 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 56 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec |
| **seed_address** | `0x00467150` |
| **ghidra_name (before)** | `FUN_00467150` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R8 FUN task 36 (`round8_fun_task_36_report.md`); R8 FUN task 12 (`FUN_00467340` install site); R6 logic task 48 |

## Status

**PARTIAL** — Live Ghidra MCP re-verified role, xref closure, disasm, and decompile. **No rename** (no COFF-exact IJG export; distinct from `h2v2_smooth_downsample@0x00464660`). Prototype corrected to `void __cdecl` (disasm: plain `RET`, no return value).

## Function

| Address | Ghidra (before) | Ghidra (after) | Role | Evidence |
|---------|-----------------|----------------|------|----------|
| `0x00467150` | `FUN_00467150` | **`FUN_00467150`** | **IJG libjpeg-6b merged-upsampler fancy upsample method** (`cinfo[0x13]==2`): per output row `IJG_jzero_far`; per-component fancy colormap/dither using `short` workspace at `upsample+0x44[]`, colormap `cinfo+0x120`, range tables `upsample+0x10`/`+0x18`, toggles scan direction `upsample+0x54` | Sole **DATA** xref `FUN_00467340@0x0046738d` → `mov [upsample+4],0x467150`; body `0x1a2` B; callee `IJG_jzero_far@0x0045f880` |

### Install site (proven)

`FUN_00467340` (`start_pass` for merged upsampler) when `cinfo+0x4c == 2`:

```
0046736d: JZ  0x00467386         ; method == 2 (fancy merged)
00467386: CMP dword ptr [EDI+0x44],0
0046738d: MOV dword ptr [EDI+0x4],0x467150   ; upsample method = FUN_00467150
00467394: MOV byte ptr [EDI+0x54],0x0
0046739a: CALL 0x00467300        ; row-buffer alloc if upsample+0x44[0]==0
004673b4: CALL 0x0045f880        ; IJG_jzero_far each color_buf row (start_pass only)
```

Row alloc (`FUN_00467300`) and initial `jzero` run in **`FUN_00467340`**, not in `FUN_00467150`. The method itself calls `IJG_jzero_far` each pass row @ `0x004671a8`.

### `cinfo` / upsample fields (live decompile)

| Field | Offset | Use in `FUN_00467150` |
|-------|--------|------------------------|
| `cinfo` row width | `cinfo[0x17]` (`+0x5c`) | `len` — `IJG_jzero_far` size |
| colormap | `cinfo[0x48]` (`+0x120`) | LUT base for fancy sample |
| num components | `cinfo[0x19]` (`+0x64`) | component loop bound |
| upsample struct | `cinfo[0x6a]` (`+0x1a8`) | `upsample+0x44` workspace, `+0x54` direction, `+0x10`/`+0x18` range ptrs |
| method id | `cinfo[0x13]` (`+0x4c`) | **Not read here** — gate is in `FUN_00467340` |

### Disassembly (key sites)

| VA | Instruction | Meaning |
|----|-------------|---------|
| `0x004671a8` | `CALL 0x0045f880` | `IJG_jzero_far(*input_buf, cinfo[0x17])` |
| `0x004671be` | `LEA ECX,[ESI+0x44]` | component `short` workspace pointer array |
| `0x004671e1` | `CMP byte ptr [ESI+0x54],0` | forward vs reverse scan |
| `0x00467240` | `MOVSX EDX,word ptr [ECX+ESI*2]` | fancy predictor from workspace |
| `0x0046724f`–`0x00467266` | colormap byte gathers | `cinfo+0x120` + range tables |
| `0x004672d5` | `MOV byte ptr [ESI+0x54],CL` | toggle direction each output row |
| `0x004672f1` | `RET` | no return value |

### Xref closure

| From | Type | Context |
|------|------|---------|
| `0x0046738d` | **DATA** | `FUN_00467340` — `upsample+4` method when `cinfo+0x4c==2` |

No direct `CALL` xrefs (vtable-style upsample callback).

### Callees

| Callee | Site | When |
|--------|------|------|
| `IJG_jzero_far` | `0x004671a8` | each output row in method loop |

### Size / signature

| Source | Value |
|--------|-------|
| Ghidra (live) | `00467150`–`004672f1` → **0x1a2** B |
| `config/bulanci/mapping.csv` | `0x199` B (stub understates by 9 B) |
| Ghidra signature (post-R9) | `void __cdecl FUN_00467150(int *cinfo, int output_buf, uint **input_buf, int num_rows)` |

### Distinct from named IJG symbols

| VA | Ghidra name | Relation |
|----|-------------|----------|
| `0x00464660` | `h2v2_smooth_downsample` | H2V2 **downsample** — different role |
| `0x00467150` | `FUN_00467150` | merged-upsampler **fancy upsample** method — no matching export in repo |

### Decompile (post-R9 prototype)

```c
void FUN_00467150(int *cinfo, int output_buf, uint **input_buf, int num_rows)
{
  upsample = cinfo[0x6a];
  len = cinfo[0x17];
  for (each row) {
    IJG_jzero_far(*input_buf, len);
    for (each component) {
      /* fancy colormap + short workspace dither; reverse scan if upsample+0x54 */
    }
    upsample[0x54] = !upsample[0x54];
  }
}
```

### mapping.csv stub (not trusted)

```
;_Globals::FUN_00467150;0x467150;0x199;__cdecl;;uchar;int*;int;uint**;int
```

### `_Globals.cpp` stub (not trusted)

```cpp
uchar _Globals::FUN_00467150(int* param_1, int param_2, uint** param_3, int param_4) { STUB_BODY(); return 0; }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` | `0x00467150` → `void FUN_00467150(int *cinfo, int output_buf, uint **input_buf, int num_rows)` | OK (was `uchar` per mapping stub) |
| `set_decompiler_comment` | Entry | OK — R9 role + xref + UNK symbol |
| `force_decompile` | `0x00467150` | OK — void return, no spurious `return (uchar)num_rows` |
| `save_program` | `bulanci.exe` | Saved |

## Frida

**none** — static xref + disasm/decompile proof sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Exact IJG `jdmerge.c` / `jquant*.c` upstream export name | **UNK** — control-flow family match only; no COFF-exact symbol in repo |
| Meaningful rename | **Deferred** — protocol forbids guess; not `h2v2_smooth_downsample` |
| `mapping.csv` / `_Globals.cpp` / `_Globals.h` still `uchar` / size `0x199` | Separate mapping pass |
| `FUN_00467340` upstream `start_pass` name | Out of scope (task 56 seed only) |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round8_fun_task_36_report.md](round8_fun_task_36_report.md)
- [round8_fun_task_12_report.md](round8_fun_task_12_report.md)
- [round6_logic_task_48_report.md](../logic_recovery/round6_logic_task_48_report.md)
