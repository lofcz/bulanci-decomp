# EHRegistrationNode

## Status

**SKIP** — MSVC CRT C++ EH frame registration node (`Visual Studio 2005 Release` EH runtime). No game `OperatorNew`, ctor, or field write; every consumer is linked CRT (`FindHandler`, `___FrameUnwindToState`, `CatchIt`, `_CallSETranslator`, …). Stack-embedded in SEH prologs; not allocated by bulanci code. Extended form is `TranslatorGuardRN` (batch 49, 40 bytes).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Minimum object ends at `+0x0B` | `FindHandler@0x0044b874` | Reads `dword ptr [ECX+0x8]` when `_s_FuncInfo::maxState >= 0x81` |
| Same span for unwind | `___FrameUnwindToState@0x0044b109` | `MOVSX ESI, byte ptr [EBX+0x8]` / `MOV ESI, dword ptr [EBX+0x8]`; writes `[EBX+0x8]` in unwind loop |
| `sizeof == 0x0C` (12 bytes) | (derived) | Last proven field `state` at `+0x08` (4 bytes) → bytes `0x00..0x0B` |
| Demangler stub removed (slice 48) | (Ghidra) | Deleted `/Demangler/EHRegistrationNode` (1 byte); canonical struct at program root, size **12** |
| No game allocation | — | Mapping / xrefs: CRT EH symbols only (`0x00447xxx`–`0x0044bxxx`); no bulanci `OperatorNew` |

## Layout table

Matches the leading 12 bytes of `TranslatorGuardRN` (see [TranslatorGuardRN.md](./TranslatorGuardRN.md)); larger guard nodes reuse this header then add translator fields at `+0x0C`.

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `EHRegistrationNode *` | `pNext` | `_CallSETranslator@0x004478fd` — `FS:[0]` saved into stack node `[EBP-0x28]` before `FS:[0] := &node` (same slot as `TranslatorGuardRN.pNext`) |
| `0x04` | 4 | `void *` | `pFrameHandler` | `_CallSETranslator@0x004478fd` — `MOV [EBP-0x24], 0x4479d2` (`TranslatorGuardHandler`); `_UnwindNestedFrames@0x00447845` — `RtlUnwind(param_1, …)` with `EHRegistrationNode*` as target frame (Win32 `EXCEPTION_REGISTRATION_RECORD` handler @ `+4`) |
| `0x08` | 4 | `int` / `char` | `state` | `FindHandler@0x0044b874` — `(char)param_2[8]` if `maxState < 0x81` else `*(int*)(param_2+8)`; `___FrameUnwindToState@0x0044b109` — read/write `[param_1+8]`; `CatchIt@0x0044b716` — `MOV dword ptr [ESI+0x8], EAX` after try high |

### `state` width

| Condition | Access | Evidence |
|-----------|--------|----------|
| `maxState < 0x81` | signed byte | `FindHandler@0x0044b891` `MOVSX ECX, byte ptr [ECX+0x8]`; `___FrameUnwindToState@0x0044b124` |
| `maxState >= 0x81` | dword | `FindHandler@0x0044b897` `MOV ECX, dword ptr [ECX+0x8]`; `___FrameUnwindToState@0x0044b12a` |

## Ghidra apply

**Applied** (batch 48, slice agent 2026-05-30). Removed conflicting 1-byte `/Demangler/EHRegistrationNode`; recreated **12**-byte struct at program root (`create_struct` + `modify_struct_field` on `pNext`). `save_program bulanci.exe` after mutations.

| Offset | Size | Type | Name (Ghidra) |
|--------|------|------|---------------|
| `0x00` | 4 | `EHRegistrationNode *` | `pNext` |
| `0x04` | 4 | `pointer` | `pFrameHandler` |
| `0x08` | 4 | `int` | `nState` |

**CRT EH prototypes** — `EHRegistrationNode *` on frame/nested-frame parameters (`void *` was blocking pointer resolution while demangler stub existed):

| Function | Address |
|----------|---------|
| `_UnwindNestedFrames` | `0x00447845` |
| `FID_conflict:___CxxFrameHandler3` | `0x00447897` (`ContextRecord` still `void *`) |
| `_CallSETranslator` | `0x004478fd` |
| `CallCatchBlock` | `0x0044b36a` |
| `FindHandlerForForeignException` | `0x0044b782` |
| `FindHandler` | `0x0044b874` |
| `___InternalCxxFrameHandler` | `0x0044bbe2` |

Decompile check: `FindHandler@0x0044b874` reads `(char)Frame->nState` / `Frame->nState` per `maxState` threshold (was `(int)Frame + 8`). `_UnwindNestedFrames` passes `Frame` to `RtlUnwind` as `EHRegistrationNode *`.

Prefer `TranslatorGuardRN` when the stack object is the 40-byte translator guard (batch 49).

## UNK

- *(R5 worker 43 — documented overload, not a field)* **`CSET_SPECIAL` (`pExcept == 0x123`)**: `_CallSETranslator@0x0044790b`–`0x00447915` — `MOV EAX,0x4479a6`; `MOV ECX,[EBP+0xc]`; `MOV [ECX],EAX` — writes **ExceptionContinuation** into `void **` at `param_2`, not `pNext` (see `TranslatorGuardRN.md`).
- *(R5 worker 43 — not struct storage)* **`CallCatchBlock` `param_2 - 4`**: `CallCatchBlock@0x0044b36a` reads saved **EBP** in the SEH activation record, not `EHRegistrationNode` bytes.
- *(R5 worker 43 — build fact)* **`ENABLE_EHTRACE` tail**: absent in VS2005 Release (no access past `+0x08` on plain `EHRegistrationNode*`).
- *(R5 worker 43)* **Game xrefs**: none (CRT-only).

## Xref summary (CRT only)

| Function | Address |
|----------|---------|
| `_JumpToContinuation` | `0x0044780e` |
| `_UnwindNestedFrames` | `0x00447845` |
| `___CxxFrameHandler3` | `0x00447897` |
| `_CallSETranslator` | `0x004478fd` |
| `_CallCatchBlock2` | `0x00447b77` |
| `CallCatchBlock` | `0x0044b36a` |
| `CatchIt` | `0x0044b716` |
| `FindHandlerForForeignException` | `0x0044b782` |
| `FindHandler` | `0x0044b874` |
| `___InternalCxxFrameHandler` | `0x0044bbe2` |
| `___FrameUnwindToState` | `0x0044b109` |
