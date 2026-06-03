# Round 7 — FUN Task 10 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 10 |
| **band** | sim |
| **seed_address** | `0x0042fdf0` |
| **prior_hint** | (none) |

## Status

**DONE** — Renamed; `IDSStream *` dest typing applied.

## Function

| Address | Ghidra name (after) | Role | Evidence |
|---------|---------------------|------|----------|
| `0x0042fdf0` | `IDSStream_CopyBulk64` | `__thiscall` dest stream `this`: 64 KiB stack buffer (`alloca 0x10024`); loop read source vtable+`0x10` / write dest vtable+`0x14`; optional progress callback; 64-bit byte count | **Xrefs_to:** `CDSMpxPersistFacet::SaveMpxFile@0x00432efa`, `CDSWavStream_SaveToStream@0x0043baeb`. **Disasm:** `MOV ESI,0x10000` chunk; `CALL [EDX+0x10]` / `CALL [EAX+0x14]` |

## Ghidra deltas

| Action | Detail |
|--------|--------|
| `rename_function_by_address` | `IDSStream_CopyFromStream` → `IDSStream_CopyBulk64` |
| `set_function_prototype` | `void IDSStream_CopyBulk64(void)` / `__thiscall` |
| `set_function_this_type` | `IDSStream *` |
| `save_program` | batch save |

## Frida

**none**

## Remaining UNK

- Full formal parameter list for source stream pointer, `uint64` size, and callback typedef (decompiler still uses stack locals).
