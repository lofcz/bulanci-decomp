# Struct recovery batch 46/50

**Index:** 46 (`batches_50.json`)  
**Structs:** `_s_CatchableType` only  
**Date:** 2026-05-30  
**Agent slice:** 46 — **DONE**

## Results

| Struct | Status | Size | Ghidra |
|--------|--------|------|--------|
| `_s_CatchableType` | SKIP (CRT) / layout VERIFIED | `0x1C` (28) | `/_s_CatchableType` 28 B, 7 fields; `get_struct_layout` OK |

## Key evidence

- **CRT-only:** MSVC VS2005 Release EH (`___BuildCatchObjectHelper`, `___TypeMatch`, `FindHandler`, `CatchIt`). Catchable entries via `EHExceptionRecord.ExceptionInformation[2]` (`_s_ThrowInfo*`) → `pCatchableTypeArray` (`FindHandler@0x0044ba70`: `ThrowInfo+0xC`, count `[0]`, stride `+4` per `CatchableType*`).
- **Layout:** Offsets `0x00`–`0x18` in `___BuildCatchObjectHelper` / `___TypeMatch` (`properties`, `pType`, 12-byte PMD, `sizeOrOffset`, `copyFunction`).
- **No game footprint:** No bulanci allocation; xrefs only in `0x0044bxxx` MSVCRT.

## Ghidra actions (slice 46)

| Action | Result |
|--------|--------|
| `get_struct_layout _s_CatchableType` | Size **28**, fields `dwProperties` … `copyFunction` |
| `create_struct` | Already present (prior follow-up) — no recreate |
| `set_local_variable_type` `FindHandler@0x0044b874` `local_24` | `_s_CatchableType *` — decompile shows typed local + `CatchIt(..., local_24, ...)` |
| `set_function_prototype` `___TypeMatch` / `CatchIt` | **Blocked** — demangler `/Demangler/_s_CatchableType` (size 1) vs `/_s_CatchableType` (size 28) |
| `save_program bulanci.exe` | Yes |

## Artifacts

- `ghidra_analysis/engine/struct_recovery/_s_CatchableType.md`
- `ghidra_analysis/engine/struct_recovery/batch_46_followup_summary.md` (prior round 2)

## Blockers / follow-ups

- **Demangler name collision:** `/Demangler/_s_CatchableType` (size 1) prevents prototype parser from resolving `_s_CatchableType *` on CRT EH functions; locals can still be typed manually.
- Batch **47:** `EHExceptionRecord` (`batches_50.json` index 47).
- `_s_ThrowInfo` full layout — separate batch (UNK in `_s_CatchableType.md`).
