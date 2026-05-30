# Round 3 — Task 50 report

## Task

| Field | Value |
|-------|-------|
| **id** | 50 |
| **title** | Handoff CDSWav polymorphic dtor vs CDSWavStream ownership |
| **one_liner** | Class-43 heap objects are **`CDSWavStream`**; authoritative delete is **`CDSWavStream_ScalarDeletingDtor`** (stash clear at `+0x34`). **`CDSWav_ScalarDeletingDtor`** is a lighter MI path (`CDSObject` only) — not used on factory-built class-43 instances. |
| **acceptance** | Trace delete paths; confirm struct layouts; document ownership handoff from R2 `HandleResourceRead` work |
| **supersedes** | R2 todo 50 (`HandleResourceRead` `this` bases) — same types, delete-path handoff |

## Status

**DONE** — ownership and vtable routing verified via Ghidra MCP decompile + vtable bytes + xref scan. No new Ghidra mutations (decompiler comments already present from prior passes).

## Evidence

### Deleting dtors (body proof)

| Function | Address | Body | `free` |
|----------|---------|------|--------|
| `CDSWav_ScalarDeletingDtor` | `0x0041bbe0` | `CDSObject__CDSObject_dtor` only — **no** `CDSWavStream_dtor`, **no** `IDSChainedTail_ClearSubObjStash` | When `param_1 & 1` |
| `CDSWavStream_ScalarDeletingDtor` | `0x0041bc00` | `CDSWavStream_dtor` → `IDSChainedTail_ClearSubObjStash(this+0x34)` → `CDSObject__CDSObject_dtor` | When `param_1 & 1` |
| `CDSWavStream_dtor` | `0x0041a640` | `param_1+0xd` dwords → `+0x34` stash; then base dtor | Non-deleting; only from stream scalar dtor |

### Vtable slot1 / slot0 (class-43 delete routing)

| Vtable | Slot | Target | Installed by |
|--------|------|--------|--------------|
| `IDSReferenced` `0x004823c0` | slot1 | `CDSWavStream_ScalarDeletingDtor@0x0041bc00` | `CDSWavStream_Factory@0x0043bb00` → `pVftable_IDSReferenced` |
| `face_8slots` `0x0048239c` | slot3 | `CDSWavStream_ScalarDeletingDtor_thunk_Sub4@0x0041a5d0` → primary | Factory → `pVftable_face8slots` |
| `IDSEventHandler` `0x00482388` | slot3 | `CDSWavStream_ScalarDeletingDtor_thunk_Sub18@0x0041a630` → primary | Factory → `pVftable_IDSEventHandler` |
| `IDSChained5/6` `0x00482354` / `0x0048236c` | slot0 | thunks `@0x0041a603` / `@0x0041a623` → primary | Factory |
| ROM `0x00482348` | slot0 | `CDSWav_ScalarDeletingDtor@0x0041bbe0` | **No** write xref to `0x482348` in binary; not set by `CDSWavStream_Factory` or `CDSAudioBank_Ctor` |
| ROM `0x00482318` | slot0 | `CDSWav_ScalarDeletingDtor_thunk@0x0041a530` (`this-0x18`) | Alternate catalog vtable (DSM/bank facet docs); separate from class-43 factory stores |

### MI thunks → primary

| Thunk | Adjust | Resolves to |
|-------|--------|-------------|
| `CDSWav_ScalarDeletingDtor_thunk@0x0041a530` | `this - 0x18` | `CDSWav_ScalarDeletingDtor` (light) |
| `CDSWav_ScalarDeletingDtor_thunk_Sub4@0x0041a540` | `this - 4` | same (light) |
| `CDSWavStream_ScalarDeletingDtor_thunk_Sub4@0x0041a5d0` | `this - 4` | `CDSWavStream_ScalarDeletingDtor` (full) |
| `CDSWavStream_ScalarDeletingDtor_thunk_Sub18@0x0041a630` | `this - 0x18` | full |
| `CDSWavStream_ScalarDeletingDtor_thunk_Sub30@0x0041a603` | `this - 0x30` | full |
| `CDSWavStream_ScalarDeletingDtor_thunk_Sub34@0x0041a623` | `this - 0x34` | full |

**Conclusion:** Every delete entry point installed on **`CDSWavStream_Factory@0x0043bb00`** objects routes to **`CDSWavStream_ScalarDeletingDtor`**. `CDSWav_ScalarDeletingDtor` remains on legacy ROM vtables (`0x482348`, `0x482318` family) and must not be treated as the class-43 heap owner.

### Factory / alloc (ownership anchor)

| Claim | Address | Evidence |
|-------|---------|----------|
| Class-43 registry `0x2b` | `0x0047d9e0` | Factory pointer `0x0043bb00` |
| `OperatorNew(0x40)` | `0x0043bb00` | `CDSWavStream_Factory`; returns `CDSWavStream*` with `0x4823xx` vtables |
| Bank facet (same footprint, different vtables) | `0x00429480` | `CDSAudioBank_Ctor` uses `0x486exx`; **no** xref to factory; `CDSAudioBank_ScalarDeletingDtor` is separate bank teardown |

### Struct layout regression check

| Struct | Ghidra size | Notes |
|--------|-------------|-------|
| `CDSWav` | **60** (`0x3c`) | Matches slice-42 field map; tail through `dwReservedTail@0x38` |
| `CDSWav_face8slots` | **40** (`0x28`) | Face view for `HandleResourceRead` / `BindPcmMemStream` |
| `CDSWavStream` | **64** (`0x40`) | Same field offsets as `CDSWav` through `+0x38`; `pPad_operatorNew0x40@0x3c` documents `OperatorNew(0x40)` tail (aligned with R3 todo 23 `CDSAudioBank` 64 B layout) |

No struct edits required this round.

## Ghidra deltas

None — existing plate comments on `CDSWav_ScalarDeletingDtor`, `CDSWavStream_ScalarDeletingDtor`, and `CDSWavStream_dtor` match MCP decompile.

## Struct doc updates

- [CDSWav.md](./CDSWav.md) — delete-path table already **VERIFIED** (todo 50); unchanged.
- [CDSWavStream.md](./CDSWavStream.md) — Ghidra size proof row updated **60 → 64** B (`pPad_operatorNew0x40`).

## Cross-ref (R2 todo 50)

`HandleResourceRead` / DSM `this` bases remain valid ([R2 worker 50 in status.md](../status.md)): class-43 PCM reader uses **`face_8slots` (`primary+4`)**; DSM reader at **`CDSDsmFile+0x1c`** is a different object size — not a `CDSWav` heap instance.

## Remaining UNK

- Whether any runtime path ever installs vtable `0x00482348` on a live `OperatorNew(0x40)` object (no data xref to that vtable address found).
- Exact C++ type name in RTTI vs documentation alias `CDSWav` for shared layout.
