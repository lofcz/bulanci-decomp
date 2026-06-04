# Round 9 `_Globals` — Task 053 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 53 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec |
| **seed_address** | `0x00466d40` |
| **ghidra_name (before)** | `FUN_00466D40` |
| **prior_hint** | *(empty in manifest)* |
| **prior art** | R8 FUN task 33 (`round8_fun_task_33_report.md`); R8 task 12 caller `FUN_00467340`; R6 logic task 47 (superseded wrapper note) |

## Status

**PARTIAL** — Role, upsample field layout, dedup algorithm, register calling convention (`EBX=cinfo`), and xref/callee closure re-verified via live Ghidra MCP (disasm, decompile, xrefs, callers/callees). **No rename:** not a COFF/upstream IJG export; protocol forbids guessed `jdmerge.c` / `jquant1.c` alias.

## Function

| Address | Ghidra (this session) | Role | Evidence |
|---------|----------------------|------|----------|
| `0x00466d40` | `FUN_00466D40` | **Merged-upsampler `start_pass` scaled-quant-table init:** for each component (`cinfo+0x64`), fill `upsample+0x34[]` via **`FUN_00466cc0`**, reusing a prior pointer when an earlier component shares the same quant selector at `upsample+0x20[]` | Body `00466d40`–`00466d8f` (**0x50** B); **1 caller** / **1 callee**; `EBX=cinfo` on entry; decompiler comment from R8 task 33 present |

### Caller context (`FUN_00467340@0x00467340`)

Sole caller at **`0x00467400`** inside merged-upsampler **`start_pass`** (vtable slot installed by `jinit_merged_upsampler`, R8 task 12):

| Gate | Instruction | Meaning |
|------|-------------|---------|
| `cinfo+0x4c == 1` | `SUB EAX,1` / `JZ 0x004673cc` @ `0x00467364`–`0x00467368` | merged (non-fancy) upsample path |
| `upsample+0x1c == 0` | `CALL zlib::gen_codes` @ `0x004673f0` | Huffman code tables built first (`PUSH ESI`; `ESI=cinfo`) |
| `upsample+0x34 == 0` | `CMP dword ptr [EDI+0x34],0` / `JNZ 0x004673c8` @ `0x004673f8`–`0x004673fc` | scaled quant table pointer array still empty |
| `EBX=cinfo` | `MOV EBX,ESI` @ `0x004673fe` | register arg before call |
| call | `CALL 0x00466d40` @ `0x00467400` | sole xref |

```
004673cc  CMP dword ptr [ESI+0x64], 3
...
004673e2  CMP byte ptr [EDI+0x1c], 0
004673ef  PUSH ESI
004673f0  CALL 0x00466b70          ; zlib::gen_codes
004673f8  CMP dword ptr [EDI+0x34], 0
004673fe  MOV EBX, ESI
00467400  CALL 0x00466d40
```

### Upsample workspace fields (proven in body)

| Offset | Use in `FUN_00466d40` |
|--------|------------------------|
| `cinfo+0x1a8` | `upsample` workspace pointer (`MOV EBP,[EBX+0x1a8]` @ entry) |
| `cinfo+0x64` | `num_components` loop bound |
| `upsample+0x20 + comp*4` | Quant table **selector** for component `comp` (`[EDI-0x14]` in loop) |
| `upsample+0x34 + comp*4` | **Output:** pointer to scaled quant table (256×`int` block from `FUN_00466cc0`) |

### Dedup algorithm (disasm + decompile match)

For `comp = 0 .. num_components-1`:

1. Load selector `qsel = upsample->quant_tbl_no[comp]` (at `+0x20`).
2. Scan `j = 0 .. comp-1`; if `quant_tbl_no[j] == qsel`, load `scaled_tbl_ptr[j]` from `upsample+0x34`; if non-NULL, reuse.
3. Else `scaled_tbl_ptr[comp] = FUN_00466cc0(qsel)` with **`EAX=cinfo`** (`MOV EAX,EBX` @ `0x00466d78`).
4. Store pointer at `upsample+0x34 + comp*4`.

### Assembly listing (Ghidra, live)

```
00466d40  PUSH EBP
00466d41  MOV  EBP,dword ptr [EBX+0x1a8]
00466d47  PUSH ESI
00466d48  XOR  ESI,ESI
00466d4a  CMP  dword ptr [EBX+0x64],ESI
00466d4d  JLE  0x00466d8d
00466d4f  PUSH EDI
00466d50  LEA  EDI,[EBP+0x34]
00466d53  MOV  ECX,dword ptr [EDI-0x14]
00466d56  XOR  EAX,EAX
00466d58  TEST ESI,ESI
00466d5a  JLE  0x00466d78
00466d5c  LEA  EDX,[EBP+0x20]
00466d60  CMP  ECX,dword ptr [EDX]
00466d62  JZ   0x00466d70
00466d64  ADD  EAX,0x1
00466d67  ADD  EDX,0x4
00466d6a  CMP  EAX,ESI
00466d6c  JL   0x00466d60
00466d70  MOV  EAX,dword ptr [EBP+EAX*4+0x34]
00466d74  TEST EAX,EAX
00466d76  JNZ  0x00466d7f
00466d78  MOV  EAX,EBX
00466d7a  CALL 0x00466cc0
00466d7f  MOV  dword ptr [EDI],EAX
00466d81  ADD  ESI,0x1
00466d84  ADD  EDI,0x4
00466d87  CMP  ESI,dword ptr [EBX+0x64]
00466d8a  JL   0x00466d53
00466d8c  POP  EDI
00466d8d  POP  ESI
00466d8e  POP  EBP
00466d8f  RET
```

### Decompile (Ghidra, live)

Confirms per-component loop, `piVar5[-5]` as quant selector (`+0x20` relative to `+0x34` slot), inner dedup scan, conditional `FUN_00466cc0(piVar5[-5])`, store to `*piVar5`. Pre-comment documents R8 task-33 proof; `unaff_EBX` holds `cinfo`.

### Xrefs / call graph

| Direction | Address | Symbol | Context |
|-----------|---------|--------|---------|
| **To** | `0x00467400` | `FUN_00467340` | UNCONDITIONAL_CALL after `zlib::gen_codes` when `upsample+0x34==0` |
| **From** | `0x00466d7a` | `FUN_00466cc0` | build scaled quant table (`EAX=cinfo`) |

No other callers; not vtable-invoked directly.

### mapping.csv stub (not trusted)

```
;_Globals::FUN_00466d40;0x466d40;0x50;__stdcall;;uchar
```

Size **0x50** matches Ghidra; `__stdcall` / `uchar` return incorrect — zero stack args, plain `RET`, no AL write.

### `_Globals.cpp` stub (not trusted)

```cpp
uchar _Globals::FUN_00466d40() { STUB_BODY(); return 0; }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `force_decompile` | `0x00466d40` | OK — R9 re-verification (no body change) |
| *(none)* | — | No rename; R8 `set_decompiler_comment` already present; no `save_program` (no durable mutations) |

## Frida

**none** — Static IJG decompressor setup; disasm/xref/callee closure sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Exact IJG upstream export name (`jdmerge.c` / `jquant1.c` family) | **UNK** — control-flow + field offsets match merged 1-pass quant setup; no COFF/`ref/libjpeg6b` byte-match |
| Meaningful Ghidra rename | **Deferred** — protocol forbids guessed IJG alias |
| Ghidra prototype (`__stdcall` / `uchar` vs void, `EBX=cinfo`) | **UNK** — register convention proven; no Ghidra custom convention applied this session |
| R6 task 47 “80 B wrapper, single call” | **Superseded** — 0x50 B dedup loop + conditional `FUN_00466cc0` per component |
| `FUN_00466cc0` upstream name | R8 task 32 scope — scales std quant bytes, separate task |

## Cross-links

- [round8_fun_task_33_report.md](round8_fun_task_33_report.md) — primary prior art for this VA
- [round8_fun_task_12_report.md](round8_fun_task_12_report.md) — caller `FUN_00467340`, call-site gates
- [round6_logic_task_47_report.md](../logic_recovery/round6_logic_task_47_report.md) — dispatch band (outdated wrapper note)
- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
