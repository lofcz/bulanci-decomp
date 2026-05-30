# Struct recovery batch 44 follow-up (round 2)

**Agent:** 44/50  
**Prior:** `batch_44_summary.md`, `exception.md`, `bad_exception.md`  
**Status:** **HANDOFF_CLEAN**

## Prior batch follow-ups (summary section)

Cross-batch notes only (not actionable in batch 44):

- Batch 45 — `bad_alloc`, `_LocaleUpdate` (shares `exception` CRT layout; `OperatorNewWithBadAlloc@0x00447c42` confirmed copy-and-throw via static `DAT_004b85b0`).
- Game errors — continue `CDSException` chain (batch 8 manifest), not STL placeholders.

## Actions taken

| Item | Result |
|------|--------|
| Re-read deliverables | `exception.md`, `bad_exception.md` present |
| `get_struct_layout exception` | Size 1 (placeholder; SKIP unchanged) |
| `get_struct_layout bad_exception` | Size 1 (placeholder; SKIP unchanged) |
| UNK: `exception` naming | Closed — dual Ghidra names, same addresses |
| UNK: game embed `exception` | Closed — xrefs only EH/`bad_alloc` throw glue (see `exception.md`) |
| UNK: `bad_exception` ctor | Closed — base `std::exception::exception` in `FindHandler` only |
| UNK: `bad_exception` frequency | Closed — vtable `0x00489638` → 2 DATA xrefs only |
| `create_struct` / `save_program` | Not run (CRT SKIP policy; no mutations) |

## Ghidra deltas

None.

## Evidence anchors (follow-up verification)

- `FindHandler@0x0044b874` — `type_info::operator==` vs `std::bad_exception::RTTI_Type_Descriptor`; on mismatch builds stack `exception` via `std::exception::exception@0x0044beb0`, throws `&DAT_004ac864`.
- `OperatorNewWithBadAlloc@0x00447c42` — lazy `std::bad_alloc::bad_alloc` on `DAT_004b85b0`, stack copy + `__CxxThrowException_8` with `&DAT_004ac57c`.
- `std::exception::exception` xrefs — single UNCONDITIONAL_CALL from `FindHandler` (plus DATA vtable writes on ctors/dtor).

## Remaining UNK

None for batch 44 scope. CRT layout remains documented but not applied in Ghidra per `AGENT_PROTOCOL.md`.

## Doc updates

- `exception.md` — UNK section closed with xref evidence.
- `bad_exception.md` — UNK section closed; corrected handler function name (`FindHandler`, not foreign-handler helper).
