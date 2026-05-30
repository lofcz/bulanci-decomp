# Struct recovery batch 33/50

**Index:** 33 (`batches_50.json`)  
**Types:** `CDSMemoryException`, `CDSMouse`  
**Program:** `bulanci.exe`  
**Date:** 2026-05-30  
**Status:** **COMPLETE**

## Results

| Struct | Status | Size | Notes |
|--------|--------|------|-------|
| `CDSMemoryException` | PARTIAL | **0x240** | Factory + static singleton span; inline `pFormatted` @ `+0x40`; inherited `CDSException` prefix |
| `CDSMouse` | VERIFIED | **0x0C** | Abstract dual-vtable shell; registration-only in retail binary |

## Key evidence

- **`CDSMemoryException_Factory`** @ `0x00434aa0` — `PUSH 0x240` → `OperatorNewWithBadAlloc` → `CDSMemoryException_ctor`.
- **Static singleton** — `StaticInit_CDSMemoryExceptionSingleton` @ `0x0047d290` on `CDSMemoryException_004b7f80`; span to `0x004b81c0` = `0x240`.
- **`Runtime_ThrowBadAlloc`** @ `0x004349e0` — throws static singleton; writes `dwFormatArg` @ `+0x3C`.
- **`CDSMouse_Factory`** @ `0x0042aa30` — `PUSH 0xc`; vtables `0x48355c` / `0x483548`; classId **0x1d** @ `0x0047c600`.
- **Runtime heap** — no code xrefs to `CDSMouse_Factory`; concrete mice use separate factories (see `round3_task_14_report.md`).

## Ghidra actions (slice 33)

- [x] `create_struct` — `CDSMemoryException` (576 B), `CDSMouse` (12 B)
- [x] `apply_data_type` — `CDSMemoryException` @ `0x004b7f80`
- [x] `get_struct_layout` — verified sizes 576 / 12
- [x] `rename_function_by_address` — `StaticDtor_CDSMemoryExceptionSingleton_atexit` @ `0x0047ea50`
- [x] `set_function_prototype` — ctor, `What`, `CDSMouse_Factory`
- [x] `save_program bulanci.exe`

## Remaining UNK

- `CDSMemoryException` `+0x14..+0x3B`: inherited unused `CDSException::pInlineMessage` bytes (Ghidra field `pUnkBaseTail`).
- Mod/custom streams with class id `0x1d` could invoke `CDSMouse_Factory` (engine path only).

## Agent todo 39 (2026-05-30)

- **`CDSException`:** 60 B base verified (canonical prefix `0x00..0x3b`).
- **Subclass embed:** `CDSMemoryException`, `CDSResourceException`, `CDSStreamException` rebuilt with `CDSException base` @ `+0` (aligned with `CDSApiException`).
- **`save_program bulanci.exe`** after struct rebuild.

## Artifacts

- `ghidra_analysis/engine/struct_recovery/CDSMemoryException.md`
- `ghidra_analysis/engine/struct_recovery/CDSMouse.md`
- `ghidra_analysis/engine/struct_recovery/batch_33_followup_summary.md` (round 2 delta)
