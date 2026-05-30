# Struct recovery batch 48/50

**Index:** 48 (`batches_50.json`)  
**Types:** `EHRegistrationNode`  
**Date:** 2026-05-30  
**Agent:** slice 48 (parallel RE)

## Results

| Struct | Status | Size | Notes |
|--------|--------|------|-------|
| `EHRegistrationNode` | SKIP | `0x0C` (12) | CRT SEH/C++ EH frame header (`pNext`, `pFrameHandler`, `nState`); no game allocation |

## Key evidence

- **`FindHandler@0x0044b874` / `___FrameUnwindToState@0x0044b109`:** `state` at `+0x08` (byte vs dword per `_s_FuncInfo::maxState` threshold `0x80`).
- **`CatchIt@0x0044b716`:** `MOV dword ptr [ESI+0x8], EAX` — writes catch state after unwind.
- **`_CallSETranslator@0x004478fd`:** FS exception-list link at `+0x00`, handler `0x4479d2` at `+0x04` on stack guard.
- **`_UnwindNestedFrames@0x00447845`:** `EHRegistrationNode*` passed to `RtlUnwind` as target frame.
- **No game footprint:** CRT EH runtime only.

## Ghidra actions (slice 48)

- Deleted conflicting `/Demangler/EHRegistrationNode` (1 byte) and pointer stub.
- `create_struct EHRegistrationNode` — 12 bytes at program root; `pNext` → `EHRegistrationNode *`.
- `set_function_prototype` on seven CRT EH handlers (frame/nested `EHRegistrationNode *`; `FID_conflict:___CxxFrameHandler3` keeps `void *` for `_CONTEXT` — type not in DB).
- `batch_decompile` on handlers; verified `FindHandler` uses `Frame->nState`.
- `save_program bulanci.exe`

## Artifacts

- `ghidra_analysis/engine/struct_recovery/EHRegistrationNode.md`

## Blockers / deferred

- **`_CONTEXT *`:** not applied on handlers that need it (`FindHandler` context param remains `void *`).
- **`CatchIt@0x0044b716`:** prototype already had `EHRegistrationNode *` on param 2; call sites from `FindHandler` still show arity/order mismatch in decompiler (pre-existing).
- **`___FrameUnwindToState@0x0044b109`:** first parameter still `int` in Ghidra (frame pointer); optional re-type to `EHRegistrationNode *`.
- Batch 49: `TranslatorGuardRN` (40-byte extension embedding this 12-byte header).
