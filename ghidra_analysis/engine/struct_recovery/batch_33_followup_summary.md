# Struct recovery batch 33 follow-up (round 2)

**Index:** 33 (`batches_50.json`)  
**Prior:** `batch_33_summary.md`  
**Program:** `bulanci.exe`  
**Date:** 2026-05-30  
**Status:** **FOLLOWUP_COMPLETE**

## Follow-ups addressed

| Item | Result |
|------|--------|
| Disassemble factory `0x00434AA0` (classId 5) | **Done** — created `CDSMemoryException_Factory`; `PUSH 0x240` → `OperatorNewWithBadAlloc` → `CDSMemoryException_ctor` |
| Size `DAT_004b7f80` static singleton | **Done** — span proof `0x004b7f80 + 0x240 == 0x004b81c0`; applied `CDSMemoryException` @ `0x004b7f80` |
| Recover `CDSMouse` layout | **Done** — created `CDSMouse_Factory` @ `0x0042aa30`; `OperatorNew(0x0C)`; dual vtables + `refCount` at `+8`; abstract 12-byte interface |

## Results (delta)

| Struct | Prior | After | Size |
|--------|-------|-------|------|
| `CDSMemoryException` | PARTIAL (no size) | PARTIAL (size + inline buffer) | **0x240** |
| `CDSMouse` | UNVERIFIED | VERIFIED (abstract interface) | **0x0C** |

## Key evidence (new)

- **`CDSMemoryException_Factory`** @ `0x00434aa0` — `PUSH 0x240` / `CALL OperatorNewWithBadAlloc`; registered classId **5** @ `0x0047d2d0`.
- **Static singleton** — `StaticInit_CDSMemoryExceptionSingleton` @ `0x0047d290` in-place ctor on `DAT_004b7f80`; next `.data` blob @ `0x004b81c0` confirms **576-byte** reservation.
- **`Runtime_ThrowBadAlloc`** @ `0x004349e0` — throws `&DAT_004b7f80`, stores arg to `+0x3C` (`DAT_004b7fbc`).
- **`CDSMouse_Factory`** @ `0x0042aa30` — `PUSH 0xc`; `[EAX]=0x48355c`, `[EAX+4]=0x483548`, `[EAX+8]=1`; registered classId **0x1d** @ `0x0047c600`.

## Ghidra actions

- [x] `create_function` — `CDSMemoryException_Factory`, `CDSMouse_Factory`, `StaticInit_CDSMemoryExceptionSingleton`
- [x] `delete_data_type` — cleared 1-byte placeholders for both structs
- [x] `create_struct` — `CDSMemoryException` (576 B), `CDSMouse` (12 B)
- [x] `apply_data_type` — `CDSMemoryException` @ `DAT_004b7f80`
- [x] `get_struct_layout` — verified sizes > 1
- [x] `save_program bulanci.exe`

## Remaining UNK

- `CDSMemoryException` bytes `+0x14..+0x3B` (`unkBaseTail`).
- Inline `wchar_t[256]` capacity at `+0x40` (size-derived only).
- `FUN_0047ea50` atexit vtable overwrite on static singleton at exit.
- Whether bare `CDSMouse` heap instances are used vs abstract registration-only base.

## Artifacts updated

- `ghidra_analysis/engine/struct_recovery/CDSMemoryException.md`
- `ghidra_analysis/engine/struct_recovery/CDSMouse.md`
