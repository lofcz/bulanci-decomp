# Struct recovery slice 39/50

**Index:** 39 (`batches_50.json`)  
**Types:** `CDSSimpleException`, `CDSStreamException`  
**Date:** 2026-05-30

## Results

| Struct | Status | Size | Notes |
|--------|--------|------|-------|
| `CDSSimpleException` | VERIFIED | `0x3C` | Leaf exception; same prefix as `CDSException` |
| `CDSStreamException` | VERIFIED | `0x50` | Stream I/O throw path; tail strings + errno at `+0x3C..0x4F` |

## Ghidra actions

- Confirmed `get_struct_layout` 60 / 80 bytes (no struct recreate needed)
- `rename_function_by_address` `FUN_00430a90` → `CDSStreamException_AllocateDefault`
- `set_function_prototype` on `CDSSimpleException_Throw`, `CDSStreamException_ctor`, `CDSStreamException_ctor_win32`, `CDSStreamException_dtor`, `CDSStreamException_AllocateDefault`
- `save_program bulanci.exe`

## Blockers

- None for slice scope. Optional: dedupe `CDSSimpleException` flat layout vs embedded `CDSException` component (see `CDSException.md`).
