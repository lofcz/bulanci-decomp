# Struct recovery batch 47/50

**Index:** 47 (`batches_50.json`)  
**Types:** `EHExceptionRecord`  
**Date:** 2026-05-30

## Results

| Struct | Status | Size | Notes |
|--------|--------|------|-------|
| `EHExceptionRecord` | SKIP | `0x50` (80) | CRT alias of Win32 `EXCEPTION_RECORD`; demangler placeholder was 1 byte |

## Key evidence

- **`_UnwindNestedFrames@0x00447845`:** `EHExceptionRecord*` cast to `PEXCEPTION_RECORD` for `RtlUnwind` — layout matches 80-byte Win32 record.
- **Field xrefs (CRT EH only):** `ExceptionCode@0`, `ExceptionFlags@4`, `NumberParameters@0x10`, `ExceptionInformation[0..2]@0x14..0x1c` proven across `FindHandler`, `___InternalCxxFrameHandler`, `CallCatchBlock`, `IsInExceptionSpec`, `___DestructExceptionObject`.
- **No game footprint:** all consumers are `Visual Studio 2005 Release` EH runtime symbols; no bulanci allocation or ctor.

## Artifacts

- `ghidra_analysis/engine/struct_recovery/EHExceptionRecord.md`

## Ghidra actions

- [x] `create_struct` — skipped (CRT stub per `AGENT_PROTOCOL.md`)
- [x] typedef `EHExceptionRecord` → `EXCEPTION_RECORD` — applied in follow-up round 2 (`batch_47_followup_summary.md`)
- [x] slice **47** verification pass — confirmed `/EHExceptionRecord` size 80; typed `_CallSETranslator@0x004478fd` first param (`ExceptionRecord->ExceptionCode` in decompile)
- [x] `save_program bulanci.exe` — slice 47 + prior follow-up

## Follow-ups

- Batch 48: `EHRegistrationNode` (paired EH frame node).
- Optional: replace `/Demangler/EHExceptionRecord` with typedef to `/winnt.h/EXCEPTION_RECORD` to improve decompilation of EH handlers.
