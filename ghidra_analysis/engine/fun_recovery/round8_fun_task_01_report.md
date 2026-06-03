# Round 8 FUN — Task 01 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 1 |
| **round** | 8 |
| **band** | dispatch |
| **seed_address** | `0x00466a30` |
| **prior_hint** | R7 task 33 — zlib gen_codes idiv helper |

## Status

**PARTIAL** — R8 live Ghidra re-verify confirms R7: 19-byte signed `idiv` stub; role closed; **no rename** (not a separate zlib/COFF export).

## Function

| Address | Ghidra name | Role | Evidence |
|---------|-------------|------|----------|
| `0x00466a30` | `FUN_00466a30` | Rounded divide `(EAX×0x1fe+ECX+0xff)/(ECX×2)` for `zlib::gen_codes` 0..255 table fill | **Xrefs (2):** `zlib::gen_codes@0x00466c1c`, `0x00466c37`. **Disasm:** `IMUL`/`LEA`/`CDQ`/`IDIV`/`RET`. Decompile matches R7 |

## Ghidra deltas

None this session (R7 comments intact).

## Frida

**none**

## Remaining UNK

Exact mathematical export name; `__fastcall` vs dual-register EAX+ECX caller convention.

## Cross-links

- [round7_fun_task_33_report.md](round7_fun_task_33_report.md)
