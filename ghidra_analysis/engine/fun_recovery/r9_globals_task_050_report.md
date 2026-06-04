# Round 9 `_Globals` — Task 050 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 50 |
| **round** | 9 |
| **namespace** | `_Globals` |
| **band** | jpeg_codec |
| **seed_address** | `0x00466a30` |
| **ghidra_name (expected)** | `FUN_00466A30` |
| **prior_hint** | R7/R8 — zlib gen_codes idiv helper (PARTIAL) |
| **prior art** | R7 FUN task 33 (`round7_fun_task_33_report.md`); R8 FUN task 01 (`round8_fun_task_01_report.md`); R6 logic task 47 slice |

## Status

**PARTIAL** — Live Ghidra MCP re-verifies R7/R8: 19-byte signed `idiv` stub; role and xref closure closed. **No rename:** compiler-outlined divide helper with no unique IJG/zlib COFF export name (ROUND9 no-guess rule).

## Function

| Address | Ghidra (this session) | Role | Evidence |
|---------|----------------------|------|----------|
| `0x00466a30` | `FUN_00466a30` | **Rounded divide helper** for the `0..255` symbol-index loop inside **`zlib::gen_codes`** (`0x00466b70`). Computes `(EAX×0x1fe + ECX + 0xff) / (ECX×2)` with **EAX** = running symbol index and **ECX** = `bits − 1` (`__fastcall param_1`). | **Size:** 19 B (`0x13`, body `00466a30`–`00466a42`). **Signature:** `int __fastcall FUN_00466a30(int param_1)`. **Disasm:** `IMUL EAX,0x1fe`; `LEA EAX,[EAX+ECX+0xff]`; `CDQ`; `ADD ECX,ECX`; `IDIV ECX`; `RET`. **Xrefs (2):** `UNCONDITIONAL_CALL` from `zlib::gen_codes` at `0x00466c1c` and `0x00466c37`. |

### Assembly listing (Ghidra)

```
00466a30  IMUL EAX,EAX,0x1fe
00466a36  LEA  EAX,[EAX + ECX*0x1 + 0xff]
00466a3d  CDQ
00466a3e  ADD  ECX,ECX
00466a40  IDIV ECX
00466a42  RET
```

### Decompiled body

```c
int __fastcall _Globals::FUN_00466a30(int param_1)  // param_1 == ECX == bits-1; EAX == sym index
{
  return (in_EAX * 0x1fe + 0xff + param_1) / (param_1 * 2);
}
```

### Caller context (`zlib::gen_codes@0x00466b70`)

Per-component loop (driven by `[cinfo+0x64]` / `param_1+100`):

1. `iVar5 = [quant+0x14] / alphabet_size` — base byte scale per component table.
2. `iVar9 = alphabet_size - 1` — passed in **ECX** via `__fastcall`.
3. First `FUN_00466a30(iVar9)` with **EAX=0** → initial bucket boundary `iVar6`.
4. For `iVar10 = 0..0xff`: while `iVar6 < iVar10`, increment `cVar8` and re-call helper with **EAX=iVar10**; store `(char)iVar5 * cVar8` into the 256-byte table.

**PE register proof at call sites** (`orig/bulanci.exe`):

| VA | Bytes (key ops) | Registers before `CALL 0x466a30` |
|----|-----------------|----------------------------------|
| `0x00466c1c` | `… 8B CD 33 C0 33 DB E8 …` | `MOV ECX,EBP` (bits−1); `XOR EAX,EAX` (sym=0); `XOR EBX,EBX` |
| `0x00466c37` | `… 8B CD 8B C3 E8 …` | `MOV ECX,EBP` (bits−1); `MOV EAX,EBX` (loop sym index) |

**Upstream callers of `zlib::gen_codes`:** `FUN_00467340@0x004673f0`, `jinit_merged_upsampler@0x004674e1`.

This helper is **not** upstream zlib `gen_codes()` from `trees.c` (which uses `next_code[]` + `bi_reverse`). The Ghidra label `zlib::gen_codes` reflects the JPEG/zlib Huffman-table band in this binary; the 19-byte function is an **outlined signed `idiv`** from that routine’s inner fill loop only.

### mapping.csv stub (not trusted)

```
;_Globals::FUN_00466a30;0x466a30;0x13;__fastcall;;int;int
```

### `_Globals.cpp` stub (not trusted)

```cpp
int _Globals::FUN_00466a30(int param_1) { STUB_BODY(); return 0; }
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x00466a30` | R9 task-50 re-verification comment (extends R7 task-33 note) |
| `save_program` | `bulanci.exe` | saved |

No `rename_function_by_address` — insufficient unique upstream symbol per ROUND9 rules.

## Frida

**none** — Static Ghidra disasm + xref closure + PE call-site register proof sufficient.

## Remaining UNK

| Item | Status |
|------|--------|
| Exact mathematical / upstream export name | **UNK** — no IJG/zlib COFF export matches this 19-byte `idiv` stub |
| `__fastcall` prototype vs dual-register use | **Documented** — Ghidra models one ECX param + `in_EAX`; true convention is caller-set **EAX** + **ECX** |
| `zlib::gen_codes` vs standard zlib `gen_codes` | **Out of scope** — full routine rename deferred; this task covers helper only |
| `mapping.csv` / `_Globals.cpp` body | **Out of scope** — export regen / logic port not in R9 FUN task scope |

## Cross-links

- [ROUND9_GLOBALS_FUN_PROTOCOL.md](../ROUND9_GLOBALS_FUN_PROTOCOL.md)
- [round7_fun_task_33_report.md](round7_fun_task_33_report.md)
- [round8_fun_task_01_report.md](round8_fun_task_01_report.md)
- [round6_logic_task_47_report.md](../logic_recovery/round6_logic_task_47_report.md)
- [config/bulanci/mapping.csv](../../../config/bulanci/mapping.csv) — `0x466a30`, size `0x13`
