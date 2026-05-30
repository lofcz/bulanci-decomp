# Round 4 — Task 38 report

## Task

| Field | Value |
|-------|--------|
| **id** | 38 |
| **title** | Fix CDSMpxStream payload offsets and pSubObjStash type |
| **source** | handoff |
| **supersedes_todo_id** | 38 (R3 persist-band overlay) |
| **addresses** | `0x00432eb0`, `0x00433180`, `0x00446b00`, `0x004290c0` |

**Types:** `CDSMpxStream`, `CDSMpx`, `CDSQueueStream`, `MpxFormatTail_persist`, `mad_stream`

## Status

**DONE** — Ghidra `CDSMpxStream` persistence band offsets now match Save/Load/CreateFromHandle disasm; `pSubObjStash` typed via nested `IDSChainedStashFacet` at `P+0x3c` (`stash` pointer at `P+0x40`).

## Before / after (`get_struct_layout` head)

| Field | R4 entry (wrong) | R4 exit |
|-------|------------------|---------|
| `pPayloadStream` | offset **20** (0x14) | offset **32** (0x20) |
| `payloadStartLo` | offset **28** (0x1c) | offset **40** (0x28) |
| `payloadStartHi` | offset 44 (0x2c) | offset 44 (0x2c) ✓ |
| `dwPayloadBytes` | offset 48 (0x30) | offset 48 (0x30) ✓ |
| Stash | `pSubObjStash` @ **60** (0x3c) alone | `IDSChainedStashFacet stashFacet` @ **60**; `pSubObjStash` @ **64** (0x40) |

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Persist facet `ECX = P+0x38` | `SaveMpxFile@0x00432eb0` | `LEA [ESI-0x30]`→`P+0x08`; `[ESI-0x18]`→`P+0x20`; `[ESI-0x10/0xc]`→`P+0x28/+0x2c`; `[ESI-0x8]`→`P+0x30` |
| Load mirrors Save | `LoadMpxFile@0x00433180` | Same adjustors; `Tell64` → `[EDI-0x10/0xc]` |
| Factory uses face8-relative offsets | `CreateFromHandle@0x00446b00` | Plate: `ctx_at_face8 = P+4`; `[ctx+0x1c/0x24/0x28/0x2c]` = `P+0x20..+0x30` |
| Stash protocol | `CDSMpxStream_dtor@0x004330b0` | `IDSChainedTail_ClearSubObjStash(param_1+0xf)` → `&stashFacet@P+0x3c`; helper reads `facet+4` |
| Stash helper | `IDSChainedTail_ClearSubObjStash@0x00434250` | `*(param_1+4)`; zeroes `stash+0x10/+0x14` when non-null |

## Ghidra deltas

1. **`recreate_struct`** `IDSChainedStashFacet` (8 B: vptr + `void * pSubObjStash`)
2. **`recreate_struct`** `CDSMpxPersistFacet` (8 B vptr-only facet)
3. **`recreate_struct`** `CDSMpxStream` (39112 B) — head fields `pad_mad_overlap_14@+0x14`, payload band `+0x20..+0x30`, `stashFacet@+0x3c`, `frame@+0x44`
4. **`set_plate_comment`** @ `0x00432eb0`, `0x00433180`
5. **`set_decompiler_comment`** @ `0x004330b0` (stash facet)
6. **`save_program bulanci.exe`**

## Struct doc updates

- [CDSMpxStream.md](./CDSMpxStream.md) — layout table, MI `+0x3c` row, R4 apply block; removed UNK for payload offset drift and bare `pSubObjStash@+0x3c`
- [pass_r4_CDSMpxStream_report.md](./pass_r4_CDSMpxStream_report.md) — consolidated map (factory `0x4b` trap, lifecycle, persist vs `mad_stream`)

## Remaining UNK

- Registry shell `0x48` vs decode `0x98c8` — same allocation or not in live game.
- Non-null writers / concrete type of `stashFacet.pSubObjStash` payload object on MPx path.
- Sub-byte WAVEFORMAT ↔ dword alias inside `mpxFormatTail` during decode overlay.
