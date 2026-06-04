# Round 9 `_Globals` — Task 057 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 57 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec |
| **seed_address** | `0x00467300` |
| **ghidra_name (expected)** | `FUN_00467300` |
| **prior_hint** | R7/R8 — jdmerge row alloc helper |
| **prior art** | R7 FUN task 34 (`round7_fun_task_34_report.md`); R8 FUN task 02 (`round8_fun_task_02_report.md`); R6 logic task 48 slice |

## Status

**PARTIAL** — Live Ghidra MCP (`connect_instance("bulanci")`) re-confirms **IJG libjpeg-6b merged/fancy upsampler per-component row-buffer alloc:** loops `num_components`, calls `cinfo->mem->alloc_small(cinfo, 1, 2*cinfo[+0x5c]+4)`, stores results in `upsample+0x44[]`. **`FUN_00467300` kept** — no unique upstream IJG export/static name in repo or COFF (protocol forbids guess rename). R8 decompiler comment and `void __stdcall FUN_00467300(int *cinfo)` prototype still present; decompiler still shows `unaff_ESI` (ESI=cinfo register convention not modeled).

## Function

| Address | Ghidra name | Role summary | Evidence |
|---------|-------------|--------------|----------|
| `0x00467300` | `FUN_00467300` | **Merged-upsampler row-buffer alloc helper:** for each component index `0 .. num_components-1`, `alloc_small` → `upsample->color_buf[ci]` | Live disasm + 2 xref closure + decompile below |

### Behavior (proven, live disasm)

| Step | Action | Disasm proof |
|------|--------|--------------|
| 1 | `upsample = *(cinfo + 0x1a8)` | `MOV EAX,[ESI+0x1a8]` @ `0x00467300` |
| 2 | `row_bytes = 2 * *(cinfo + 0x5c) + 4` | `MOV EBP,[ESI+0x5c]`; `LEA EBP,[EBP+EBP*1+0x4]` @ `0x00467307`–`0x00467310` |
| 3 | Loop `ci < *(cinfo+0x64)` (`num_components`) | `CMP [ESI+0x64],EDI`; `JL 0x00467320` @ `0x0046730d` / `0x0046733a` |
| 4 | `color_buf[ci] = (**(cinfo->mem+4))(cinfo, 1, row_bytes)` | `MOV EAX,[ESI+4]`; `MOV ECX,[EAX+4]`; `PUSH EBP`; `PUSH 1`; `PUSH ESI`; `CALL ECX` @ `0x00467320`–`0x0046732a` |
| 5 | Store pointer; advance `EBX` by 4 | `MOV [EBX],EAX`; `ADD EBX,4` @ `0x0046732c` / `0x00467334` |
| 6 | Return | `RET` @ `0x0046733f` |

**Size:** `0x40` B (`0x00467300`–`0x0046733f`), matches `config/bulanci/mapping.csv`.

**Calling convention:** Entry uses **ESI=cinfo** with no prolog load from stack — callers must leave `jpeg_decompress_struct *` in **ESI** at `CALL` (both proven call sites do).

### Xref closure (Ghidra live)

| From | Symbol | Site | Gate (disasm) |
|------|--------|------|----------------|
| `0x004674ef` | `jinit_merged_upsampler` | Post-init tail | `CMP [ESI+0x4c],2`; `JNZ` skip; else `CALL 0x00467300` |
| `0x0046739a` | `FUN_00467340` | Fancy merged `start_pass` branch | `[ESI+0x4c]==2` path (`JZ` from `SUB EAX,1` chain); `CMP [EDI+0x44],0`; `JNZ` skip alloc; `MOV [EDI+4],0x467150`; `CALL 0x00467300` |

After `FUN_00467340` alloc path, each `color_buf[i]` is zeroed via `IJG_jzero_far` @ `0x0045f880` (`PUSH EBP` size + buffer ptr loop @ `0x004673b0`–`0x004673c5`).

### Caller context (disasm snippets)

**`jinit_merged_upsampler` @ `0x004674ef`:**

```
004674e9  CMP  dword ptr [ESI+0x4c], 2
004674ed  JNZ  0x004674f4
004674ef  CALL 0x00467300
```

**`FUN_00467340` @ `0x0046739a` (fancy / `cinfo+0x4c == 2`):**

```
00467386  CMP  dword ptr [EDI+0x44], 0
0046738d  MOV  dword ptr [EDI+0x4], 0x467150
00467398  JNZ  0x0046739f
0046739a  CALL 0x00467300
```

### Disassembly (Ghidra live, full body)

```
00467300  MOV  EAX, dword ptr [ESI+0x1a8]
00467306  PUSH EBP
00467307  MOV  EBP, dword ptr [ESI+0x5c]
0046730a  PUSH EDI
0046730b  XOR  EDI, EDI
0046730d  CMP  dword ptr [ESI+0x64], EDI
00467310  LEA  EBP, [EBP+EBP*0x1+0x4]
00467314  JLE  0x0046733d
00467316  PUSH EBX
00467317  LEA  EBX, [EAX+0x44]
00467320  MOV  EAX, dword ptr [ESI+0x4]
00467323  MOV  ECX, dword ptr [EAX+0x4]
00467326  PUSH EBP
00467327  PUSH 0x1
00467329  PUSH ESI
0046732a  CALL ECX
0046732c  MOV  dword ptr [EBX], EAX
0046732e  ADD  EDI, 0x1
00467331  ADD  ESP, 0xc
00467334  ADD  EBX, 0x4
00467337  CMP  EDI, dword ptr [ESI+0x64]
0046733a  JL   0x00467320
0046733c  POP  EBX
0046733d  POP  EDI
0046733e  POP  EBP
0046733f  RET
```

### Decompile (Ghidra live, post-`force_decompile`)

```c
void _Globals::FUN_00467300(int *cinfo)
{
  /* R8 verified comment still present — IJG jdmerge/jdsample helper;
     callers pass cinfo in ESI; gates @ jinit_merged_upsampler / FUN_00467340.
     Exact jdmerge.c static symbol UNK (no COFF). */
  iVar3 = 0;
  if (0 < *(int *)(unaff_ESI + 100)) {   /* +0x64 num_components */
    puVar2 = (undefined4 *)(*(int *)(unaff_ESI + 0x1a8) + 0x44);
    do {
      uVar1 = (**(code **)(*(int *)(unaff_ESI + 4) + 4))();
      *puVar2 = uVar1;
      iVar3 = iVar3 + 1;
      puVar2 = puVar2 + 1;
    } while (iVar3 < *(int *)(unaff_ESI + 100));
  }
  return;
}
```

(`cinfo` parameter declared; body still uses `unaff_ESI` — cosmetic decompiler artifact.)

### Rename decision

| Candidate | Verdict |
|-----------|---------|
| IJG `jdmerge.c` / `jdsample.c` static export | **Rejected** — no COFF/upstream symbol in repo; control-flow match only (R6/R7/R8 UNK) |
| Descriptive alias (`merged_upsample_alloc_rows`, etc.) | **Rejected** — not an upstream export; protocol forbids guess |
| Keep `FUN_00467300` | **Accepted** |

### mapping.csv stub (not trusted)

```
;_Globals::FUN_00467300;0x467300;0x40;__stdcall;;void;int*
```

### `_Globals.cpp` stub (not trusted)

```cpp
void _Globals::FUN_00467300(int* param_1) { STUB_BODY(); }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| *(none)* | — | R7/R8 prototype + decompiler comment already applied; live verification matches — no `rename_function_by_address`, no `save_program` |

## Frida

**none** — static IJG decompressor plumbing; xref/offset proof sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Exact IJG `jdmerge.c` / `jdsample.c` symbol for `0x00467300` | **UNK** — no `ref/libjpeg6b` byte diff in repo |
| Decompiler `unaff_ESI` vs ESI=cinfo | **Cosmetic** — `set_variable_storage` blocked in R7/R8 |
| `FUN_00467340` upstream name (`start_pass` family) | **Out of scope** — R9 task 058 |

## Cross-links

- [round7_fun_task_34_report.md](round7_fun_task_34_report.md) — initial MCP proof + prototype
- [round8_fun_task_02_report.md](round8_fun_task_02_report.md) — R8 re-verify
- [round8_fun_task_12_report.md](round8_fun_task_12_report.md) — `FUN_00467340` caller context
