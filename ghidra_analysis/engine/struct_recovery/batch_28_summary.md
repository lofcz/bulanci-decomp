# Struct recovery batch 28/50

**Index:** 28 (`batches_50.json`)  
**Types:** `CDSDirectXException`, `CDSEasyMemStream`  
**Date:** 2026-05-30

## Results

| Struct | Status | Size | Notes |
|--------|--------|------|-------|
| `CDSDirectXException` | PARTIAL | `0x48` | HRESULT throw object; proven tail `+0x3c` message, `+0x40` context, `+0x44` hresult |
| `CDSEasyMemStream` | VERIFIED | `0x2c` | Heap-backed `IDSStream`; 11 fields through `backing_heap` at `+0x28` |

## Key evidence

- **CDSDirectXException alloc:** `OperatorNewWithBadAlloc(0x48)` in `FUN_0043b8b0` (`0x0043b8b0`) and `CDSDirectXException_ThrowFromHresult` (`0x0043b820`).
- **CDSDirectXException dtor:** `CDSDirectXException_dtor` (`0x0043b760`) releases `CDsString` at `param_1[0xf]` (`+0x3c`).
- **CDSEasyMemStream alloc:** `OperatorNewWithBadAlloc(0x2c)` in `FUN_00430e70` (`0x00430e70`).
- **CDSEasyMemStream I/O:** `ReadBytes`/`WriteBytes`/`SeekPosition` use IDS-stream-relative offsets mapped to outer `+0x14..+0x28`.

## Artifacts

- `ghidra_analysis/engine/struct_recovery/CDSDirectXException.md`
- `ghidra_analysis/engine/struct_recovery/CDSEasyMemStream.md`
- Ghidra: `create_struct` applied; `get_struct_layout` confirms `CDSEasyMemStream` = 44 bytes, `CDSDirectXException` = 72 bytes; `save_program bulanci.exe` at batch end.

## Follow-ups

- Recover `CDSException` base (batch 29 manifest) to replace `pad_0x14` and upgrade `CDSDirectXException` to VERIFIED.
- Name `CDSEasyMemStream` vtable MI bases (`CDSObject` / `IDSStream`) once parent structs are sized.
