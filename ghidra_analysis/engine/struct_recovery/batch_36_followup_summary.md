# Struct recovery batch 36 — follow-up round 2

**Agent:** 36/50  
**Input:** `batch_36_summary.md`, `CDSSimpleException.md`, `CDSStreamException.md`  
**Status:** **FOLLOWUP_COMPLETE** (documentation; Ghidra layouts unchanged)

## Actions

| Item | Source | Result |
|------|--------|--------|
| IDSChained / secondary vtables | `CDSSimpleException.md` UNK | **Resolved** — 5-slot table is primary vptr at `+0` only (`0x0048754c`); no extra subobject offsets |
| Slot 3 `What` / `FormatMessage` | Both structs | **Resolved** — decompile + `vftable_methods.csv`; simple = passthrough `What`; stream = `FormatMessage@0x00430360` |
| `inlineMessage` vs `+0x3C` tail | `CDSStreamException.md` UNK | **Resolved** — `0x14+0x28=0x3C` contiguous with `pFormatMsg`; ctor/dtor dword indices `0xF–0x11` |
| `CDSStreamException_ctor_win32` | UNK note | **Resolved** — same layout; `dwWin32Error` from `param_3` |
| `wchar_t` buffer extent | `CDSSimpleException.md` UNK | **Resolved** via alloc geometry; **`__swprintf` count** remains minor UNK |
| Parent `CDSException` struct | `batch_36_summary` deferred | **Deferred** — batch 29 manifest (`CDSException`, `CDSFilterStream`); type not in program (`validate_data_type_exists` → false) |
| Sibling exceptions (`CDSApiException`, …) | batch summary deferred | **Out of scope** for agent 36 |

## Ghidra

- `get_struct_layout("CDSSimpleException")` → 60 B (unchanged)
- `get_struct_layout("CDSStreamException")` → 80 B (unchanged)
- **No `create_struct` / `save_program`** — layouts already applied in batch 36

## Remaining UNK

- `CDSException_GetMessageW` → `__swprintf` into `+0x14` without explicit buffer size in decompilation.
- errno → string table for stream errors (documented in `formats/stream_hierarchy.md`, not struct fields).
- Formal `CDSException` base type (0x3C prefix) — batch 29.

## Deliverables touched

- [CDSSimpleException.md](./CDSSimpleException.md) — UNK trimmed; follow-up resolved section added
- [CDSStreamException.md](./CDSStreamException.md) — same
