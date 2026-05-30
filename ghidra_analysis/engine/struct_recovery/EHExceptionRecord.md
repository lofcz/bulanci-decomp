# EHExceptionRecord

## Status

**SKIP** — MSVC CRT / SEH stub (`Visual Studio 2005 Release` EH runtime). No game `OperatorNew`, ctor, or field write in bulanci code; every xref is a linked CRT EH handler. Layout matches existing Ghidra `EXCEPTION_RECORD` (`80` bytes, `/winnt.h/EXCEPTION_RECORD`).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Binary-compatible with `EXCEPTION_RECORD` (80 bytes) | `0x00447845` | `_UnwindNestedFrames`: `RtlUnwind(..., (PEXCEPTION_RECORD)param_2, ...)` — `EHExceptionRecord*` passed where Win32 expects an 80-byte `EXCEPTION_RECORD` |
| `sizeof(EXCEPTION_RECORD) == 0x50` | (Ghidra type) | `search_data_types` → `/winnt.h/EXCEPTION_RECORD` Size: **80** |
| Placeholder demangler type is wrong | (Ghidra type) | `/Demangler/EHExceptionRecord` Size: **1** — not used for layout proof |
| No game allocation | — | Mapping / decompile: only CRT symbols (`FindHandler`, `___InternalCxxFrameHandler`, `_UnwindNestedFrames`, …); no bulanci `OperatorNew` or global |

## Layout table

Win32 `EXCEPTION_RECORD` field names. Offsets `0x14`–`0x1C` are the first three `ExceptionInformation` dwords (C++ EH uses `NumberParameters == 3`).

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `uint` | `ExceptionCode` | `FindHandler@0x0044b874` `*(int*)param_1 == 0xE06D7363`; `FindHandlerForForeignException@0x0044b782` filters `0x80000203`, `0xE0434F4D`; `_CallSETranslator@0x004478fd` `*(undefined4*)param_1`; `___DestructExceptionObject@0x0044b22f` `*param_1`; `FUN_0044b490@0x0044b490` `unaff_ESI[0]` |
| `0x04` | 4 | `uint` | `ExceptionFlags` | `_UnwindNestedFrames@0x00447845` read/modify `*(uint*)(param_2+4) &= ~2`; `TranslatorGuardHandler@0x004479d2` test `0x66`; `___InternalCxxFrameHandler@0x0044bbe2` `(byte)param_1[4] & 0x66` |
| `0x10` | 4 | `uint` | `NumberParameters` | `FindHandler@0x0044b874` `[ESI+0x10] == 3`; `___InternalCxxFrameHandler@0x0044bbe2` `*(uint*)(param_1+0x10) > 2`; `FUN_0044b490@0x0044b490` `unaff_ESI[4] == 3` |
| `0x14` | 4 | `uint` | `ExceptionInformation[0]` | `FindHandler@0x0044b874` `*(int*)(param_1+0x14)` vs `0x19930520`/`21`/`22`; `FUN_0044b490@0x0044b490` `unaff_ESI[5]` same magic check |
| `0x18` | 4 | `void *` | `ExceptionInformation[1]` | `CallCatchBlock@0x0044b36a` `*(undefined4*)(param_1+0x18)` → `__CreateFrameInfo`; `___DestructExceptionObject@0x0044b22f` `param_1[6]` as destructor `this` |
| `0x1C` | 4 | `void *` | `ExceptionInformation[2]` | `IsInExceptionSpec@0x0044b2a8` `*(int*)(param_1+0x1c)` → `_s_ThrowInfo` tail; `___InternalCxxFrameHandler@0x0044bbe2` `*(code**)(*(int*)(param_1+0x1c)+8)`; `FindHandler@0x0044b874` `*(int*)(param_1+0x1c)==0` on rethrow path |

### C++ EH semantics (this build, `NumberParameters == 3`)

| Slot | Typical value / use |
|------|---------------------|
| `[0]` @ `0x14` | EH magic `0x19930520`, `0x19930521`, or `0x19930522` |
| `[1]` @ `0x18` | Thrown object pointer (frame info key, destructor `this`) |
| `[2]` @ `0x1C` | `_s_ThrowInfo *` (type match, spec callback, dtor metadata) |

## Ghidra apply

**Typedef (batch 47 follow-up).** Deleted 1-byte `/Demangler/EHExceptionRecord`; created typedef `EHExceptionRecord` → `EXCEPTION_RECORD` (80 bytes, `/EHExceptionRecord`). Verified 2026-05-30 slice **47**: `search_data_types` → `/EHExceptionRecord` size **80**; demangler stub absent.

**CRT EH prototypes** — first parameter `EHExceptionRecord *` where the VS2005 library signature uses it; frame/context params `void *` until batch 48 (`EHRegistrationNode` at `/EHRegistrationNode` size 12 exists but pointer typedef resolution is batch 48):

| Function | Address |
|----------|---------|
| `_UnwindNestedFrames` | `0x00447845` |
| `FID_conflict:___CxxFrameHandler3` | `0x00447897` |
| `_CallSETranslator` | `0x004478fd` (slice 47 pass: `ExceptionRecord->ExceptionCode`) |
| `TranslatorGuardHandler` | `0x004479d2` |
| `___DestructExceptionObject` | `0x0044b22f` |
| `IsInExceptionSpec` | `0x0044b2a8` |
| `CallCatchBlock` | `0x0044b36a` |
| `CatchIt` | `0x0044b716` |
| `FindHandlerForForeignException` | `0x0044b782` |
| `FindHandler` | `0x0044b874` |
| `___InternalCxxFrameHandler` | `0x0044bbe2` |

`save_program bulanci.exe` after mutations.

## UNK

- *(R5 worker 43 — permanent SKIP / CRT)* `ExceptionRecord` (`+0x08`), `ExceptionAddress` (`+0x0C`): Win32 header fields; bulanci only passes whole `EXCEPTION_RECORD` to `RtlUnwind` — no game field consumer.
- *(R5 worker 43 — permanent SKIP / CRT)* `ExceptionInformation[3..14]` (`+0x20`..`+0x4C`): unused; MSVC C++ EH uses `NumberParameters == 3` only (`FindHandler@0x0044b874`).
- *(R5 worker 43 — permanent SKIP / CRT)* `ExceptionFlags & 0x66` / `& ~2`: CRT unwind bookkeeping (`_UnwindNestedFrames@0x00447845`, `TranslatorGuardHandler@0x004479d2`); no game semantics.

## Xref summary (CRT only)

| Function | Address |
|----------|---------|
| `_UnwindNestedFrames` | `0x00447845` |
| `_CallSETranslator` | `0x004478fd` |
| `TranslatorGuardHandler` | `0x004479d2` |
| `___CxxFrameHandler3` | `0x00447897` |
| `IsInExceptionSpec` | `0x0044b2a8` |
| `CallCatchBlock` | `0x0044b36a` |
| `CatchIt` | `0x0044b716` |
| `FindHandlerForForeignException` | `0x0044b782` |
| `FindHandler` | `0x0044b874` |
| `___InternalCxxFrameHandler` | `0x0044bbe2` |
| `___DestructExceptionObject` | `0x0044b22f` |
