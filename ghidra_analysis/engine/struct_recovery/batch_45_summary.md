# Struct recovery batch 45/50

**Index:** 45 (`batches_50.json`)  
**Structs:** `bad_alloc`, `_LocaleUpdate`  
**Program:** `bulanci.exe`  
**Date:** 2026-05-30

## Results

| Struct | Status | Size | Ghidra apply |
|--------|--------|------|--------------|
| `bad_alloc` | SKIP | 12 (= `exception`) | No |
| `_LocaleUpdate` | SKIP (layout proven) | 13 (0x0D) | No |

## Evidence summary

- **bad_alloc:** Derives from `std::exception` only (vptr swap after base ctor). `OperatorNewWithBadAlloc` uses 12-byte stack object and static `DAT_004b85b0`; engine-wide allocator hook, but type is CRT.
- **_LocaleUpdate:** VS2005 CRT stack guard; ctor at `0x004487c8` defines four fields at 0/4/8/0xC; dozens of CRT `_l` function callers; manual teardown via `ptd+0x70` bit 2.

## Artifacts

- `ghidra_analysis/engine/struct_recovery/bad_alloc.md`
- `ghidra_analysis/engine/struct_recovery/_LocaleUpdate.md`

## Ghidra MCP (agent slice 45, 2026-05-30)

- Decompiled: `std::bad_alloc::bad_alloc@0x00447be4`, `_LocaleUpdate@0x004487c8`, `OperatorNewWithBadAlloc@0x00447c42`, `getSystemCP@0x0044c8ab`
- `get_struct_layout`: both placeholders size 1 (unchanged; no `create_struct`)
- **Rename:** `FUN_0047ee91` → `std_bad_alloc_atexit_teardown` (static `DAT_004b85b0` `_atexit` hook)
- **`save_program bulanci.exe`:** run after rename

## Dependency

- Batch 44 documented `exception` @ 12 bytes (`exception.md`). `bad_alloc` adds no fields.

## Notes for batch 46+

- `_LocaleUpdate` safe to type as 13-byte CRT struct if CRT namespace separation is added later.
