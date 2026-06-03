# Round 7 FUN — Task 33 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 33 |
| **round** | 7 |
| **band** | dispatch |
| **seed_address** | `0x00466a30` |
| **title** | FUN recovery: FUN_00466A30 @ 0x00466a30 (xrefs=2) |
| **prior_hint** | — (see R6 logic task 47 slice note for same VA) |

## Status

**PARTIAL** — Role and calling convention proven via Ghidra MCP (disasm, decompile, xref closure). **No rename:** not a separate zlib COFF export; compiler-outlined signed `idiv` helper with no unique upstream symbol name.

## Function

| Address | Ghidra name (before → after) | Size | Role | Evidence |
|---------|------------------------------|-----:|------|----------|
| `0x00466a30` | `FUN_00466a30` → **`FUN_00466a30`** (unchanged) | 19 B (`0x13`) | **Rounded divide helper** for the `0..255` symbol-index loop inside **`zlib__gen_codes`** (`0x00466b70`). Computes `(EAX×0x1fe + ECX + 0xff) / (ECX×2)` with **EAX** = running symbol index and **ECX** = `bits − 1`. | **Disasm:** `IMUL EAX,0x1fe`; `LEA EAX,[EAX+ECX+0xff]`; `CDQ`; `ADD ECX,ECX`; `IDIV ECX`; `RET`. **Xrefs (2):** `UNCONDITIONAL_CALL` from `zlib__gen_codes` at `0x00466c1c` (EAX=0, ECX=`bits-1`) and `0x00466c37` (EAX=loop index EBX, ECX=`bits-1`). **mapping.csv:** `__fastcall`, 2×`int`. |

### Caller context (`zlib__gen_codes@0x00466b70`)

Per-component loop (driven by `[cinfo+0x64]`):

1. `iVar5 = [quant+0x14] / *piStack_c` — base byte scale per alphabet size.
2. `iVar9 = *piStack_c - 1` — code-length parameter passed in **ECX** to the helper.
3. First `FUN_00466a30(iVar9)` with **EAX=0** → initial bucket boundary `iVar6`.
4. For `iVar10 = 0..0xff`: while `iVar6 < iVar10`, increment `cVar8` and re-call helper with **EAX=iVar10**; store `(char)iVar5 * cVar8` into the 256-byte table.

This is **not** upstream zlib `gen_codes()` from `trees.c` (which uses `next_code[]` + `bi_reverse`). The Ghidra label `zlib__gen_codes` reflects the JPEG/zlib Huffman-table band; this 19-byte helper is an **outlined division** from that routine’s inner fill loop only.

### Assembly listing

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
int __fastcall FUN_00466a30(int param_1)  // param_1 == ECX == bits-1; EAX == sym index
{
  return (in_EAX * 0x1fe + 0xff + param_1) / (param_1 * 2);
}
```

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x00466a30` | R7 task-33 proof comment |
| `save_program` | `bulanci.exe` | saved (after one lock retry) |

No `rename_function_by_address` — insufficient unique symbol per ROUND7 rules.

## Frida

**none** — Static Ghidra disasm + xref closure sufficient.

## Remaining UNK

| Item | Reason |
|------|--------|
| Exact mathematical name | No IJG/zlib export matches this 19-byte `idiv` stub; not byte-matched to a named upstream function |
| `__fastcall` prototype vs dual-register use | Ghidra models one ECX param + `in_EAX`; true convention is caller-set **EAX** + **ECX** |
| `zlib__gen_codes` vs standard zlib | Body differs from madler/zlib `gen_codes`; full routine rename is out of scope (task 33 only) |
| `mapping.csv` / `_Globals.cpp` stub | Still `FUN_00466a30`; export regen out of scope |

## Cross-links

- [`round6_logic_task_47_report.md`](../logic_recovery/round6_logic_task_47_report.md) — dispatch `0x466a` band; same VA listed as “integer divide helper for bit-length assignment”
- [`config/bulanci/mapping.csv`](../../../config/bulanci/mapping.csv) — `0x466a30`, `__fastcall`, size `0x13`
- Caller xrefs: `FUN_00467340@0x004673f0`, `jinit_merged_upsampler@0x004674e1` → `zlib__gen_codes`
