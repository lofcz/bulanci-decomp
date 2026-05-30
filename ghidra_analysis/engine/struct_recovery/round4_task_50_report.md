# Round 4 — Task 50 report

## Task

| Field | Value |
|-------|-------|
| **id** | 50 |
| **round** | 4 (MCP apply pass; supersedes R3 read-only verify) |
| **title** | Verify R3 closure: Handoff CDSWav polymorphic dtor vs CDSWavStream ownership |
| **one_liner** | Class-43 heap owner is **`CDSWavStream`**; apply Ghidra **`CDSWav_face8slots`** / primary `this` typing on PCM paths; plate delete routing vs light `CDSWav_ScalarDeletingDtor`. |
| **prior** | [round3_task_50_report.md](./round3_task_50_report.md) |
| **structs** | CDSWav, CDSWav_face8slots, CDSWavStream, CDSDsmFile |

## Status

**DONE** — R3 ownership re-verified; R4 applied prototypes, `set_function_this_type`, plates, and `save_program` (not read-only).

## Evidence

### Delete paths (unchanged from R3)

| Function | Address | Body | Class-43 route |
|----------|---------|------|----------------|
| `CDSWav_ScalarDeletingDtor` | `0x0041bbe0` | `CDSObject__CDSObject_dtor` only | ROM vtable `0x00482348` slot0 — **not** factory-installed |
| `CDSWavStream_ScalarDeletingDtor` | `0x0041bc00` | `CDSWavStream_dtor` → `IDSChainedTail_ClearSubObjStash` @ `+0x34` | `IDSReferenced` `0x004823c0` slot1 |
| `CDSWavStream_dtor` | `0x0041a640` | Clears stash at `this->pVftable_IDSChained5` (+0x34) | Only from stream scalar dtor |

### Vtable bytes (re-check)

| Vtable | Offset | Target | Bytes |
|--------|--------|--------|-------|
| `0x004823c0` | slot1 | `0x0041bc00` | `00 bc 41 00` |
| `0x00482348` | slot0 | `0x0041bbe0` | `e0 bb 41 00` |
| `0x0048239c` | slot3 | `0x0041a5d0` (thunk → stream dtor) | `d0 a5 41 00` |

Factory `CDSWavStream_Factory@0x0043bb00` still installs `0x4823xx` only; no write to `0x00482348`.

### Struct sizes (regression)

| Struct | Size | Notes |
|--------|------|-------|
| `CDSWav` | 60 | Primary / shared layout |
| `CDSWav_face8slots` | 40 | Face @ `primary+4` |
| `CDSWavStream` | 64 | `OperatorNew(0x40)` + pad @ `+0x3c` |

### PCM `this` typing (R4 delta)

| Function | Address | `this` after R4 | Decompile highlight |
|----------|---------|-----------------|---------------------|
| `CDSWav_HandleResourceRead` | `0x0043b960` | `CDSWav_face8slots *` | `this->dwPcmEndBound - *pReadCursor`; stream vcall on `this->pStreamStorage` |
| `CDSWav_BindPcmMemStream` | `0x0043ba30` | `CDSWav_face8slots *` | Slice/bind on `pStreamStorage`, `dwPcmBindHi`, `dwPcmEndBound` |
| `CDSWav_ReleaseRefcount` | `0x00433040` | `CDSWav *` (primary) | `this->dwRefcountOrSlots`, `pVftable_IDSEventHandler` |
| `CDSDsmFile_HandleResourceRead` | `0x00428ad0` | `CDSDsmFile *` @ `obj+0x1c` | DSM loop — separate from wav face (R2 rename retained) |

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `set_function_prototype` + `set_function_this_type` | `0x0043b960` | `CDSWav_face8slots::CDSWav_HandleResourceRead` |
| `set_function_prototype` + `set_function_this_type` | `0x0043ba30` | `CDSWav_face8slots::CDSWav_BindPcmMemStream` (`__thiscall`) |
| `set_function_prototype` + `set_function_this_type` | `0x00433040` | `CDSWav::CDSWav_ReleaseRefcount` |
| `set_function_prototype` + `set_function_this_type` | `0x0041a640` | `CDSWavStream::CDSWavStream_dtor` |
| `set_function_this_type` | `0x0041bbe0`, `0x0041bc00` | `CDSWav *` / `CDSWavStream *` |
| `set_plate_comment` | `0x0041bbe0`, `0x0041bc00`, `0x0043b960` | Light vs authoritative delete; ECX=face |
| `force_decompile` | wav PCM + stream dtor | Typed field access confirmed |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CDSWav.md](./CDSWav.md) — R4 apply log.
- [CDSWavStream.md](./CDSWavStream.md) — no layout change.

## Remaining UNK

- Runtime installer for ROM vtable `0x00482348` (still no data xref).
- `CDSWav_BindPcmMemStream` decompiler still shows `this[1].pVftable_face8slots` for QWORD high half of seek — cosmetic; asm uses `face+0x28`.
- Exact MSVC RTTI spelling `CDSWav` vs `CDSWavStream` on class-43 instances.
