# Struct recovery batch 47 follow-up (round 2)

**Prior:** `batch_47_summary.md`, `EHExceptionRecord.md`  
**Date:** 2026-05-30  
**Status:** **FOLLOWUP_DONE**

## Actions taken

1. **Typedef cleanup (optional follow-up from batch 47)**
   - Deleted `/Demangler/EHExceptionRecord` (1-byte demangler placeholder).
   - Created typedef `EHExceptionRecord` → `EXCEPTION_RECORD` (80 bytes, path `/EHExceptionRecord`).

2. **CRT EH handler prototypes** — first parameter typed as `EHExceptionRecord *` where library signatures use it; frame/context params left as `void *` until batch 48 defines `EHRegistrationNode` / `CONTEXT`:
   - `_UnwindNestedFrames@0x00447845` (`__stdcall`)
   - `FindHandler@0x0044b874`, `FindHandlerForForeignException@0x0044b782`
   - `CallCatchBlock@0x0044b36a`, `IsInExceptionSpec@0x0044b2a8`
   - `___DestructExceptionObject@0x0044b22f`, `___InternalCxxFrameHandler@0x0044bbe2`

3. **Forced redecompile** on the above; verified field-aware output (e.g. `ExceptionCode`, `NumberParameters`, `ExceptionInformation[]`, `ExceptionFlags`).

4. **`save_program bulanci.exe`** — saved once after Ghidra mutations.

5. **Doc:** `EHExceptionRecord.md` — Ghidra apply section updated.

## Ghidra deltas

| Before | After |
|--------|-------|
| `EHExceptionRecord` size 1 (`/Demangler/...`) | typedef alias, size **80** (`/EHExceptionRecord`) |
| `_UnwindNestedFrames`: `param_2 + 4` | `param_2->ExceptionFlags` / `PEXCEPTION_RECORD` |
| `FindHandler`: `char *` / `_s_FuncInfo *` conflation on record | `EHExceptionRecord *` with `ExceptionCode`, `ExceptionInformation[]`, etc. |

## Remaining UNK / deferred

- **Doc UNK unchanged:** `ExceptionRecord`/`ExceptionAddress` (+0x08/+0x0C); `ExceptionInformation[3..14]`; exact `ExceptionFlags` bit semantics.
- **`EHRegistrationNode *`:** not in program DB yet — batch 48; frame params still `void *` in EH prototypes.
- **`CONTEXT *`:** not resolved in `FindHandler` prototype (used `void *` for `ContextRecord`).
- **Batch 48 handoff:** define `EHRegistrationNode` and re-type frame parameters.

## Artifacts

- `ghidra_analysis/engine/struct_recovery/batch_47_followup_summary.md` (this file)
- `ghidra_analysis/engine/struct_recovery/EHExceptionRecord.md` (Ghidra apply note)
