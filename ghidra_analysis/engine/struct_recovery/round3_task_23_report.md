# Round 3 — Task 23 report

## Task

| Field | Value |
|-------|-------|
| **id** | 23 |
| **title** | Align CDSAudioBank Ghidra layout to 0x40 heap object; name factory tail dwField_20/38 |
| **types** | CDSAudioBank, CDSWavStream, CDSApiException |
| **source** | handoff (supersedes round-2 todo 23) |

## Status

**DONE** — Ghidra `CDSAudioBank` is **64 B (`0x40`)** with field offsets matching `OperatorNew(0x40)` / shared `CDSWavStream` footprint. Factory tail fields named **`pStreamStorage` @ +0x20** and **`dwReservedTail` @ +0x38** (replacing legacy `dwField_20` / `dwField_38`). Prior misplacement (`pVftable_bankDeserialize` @ +0x20, `slotVector` @ +0x24) corrected via `recreate_struct`.

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Heap alloc `0x40` | `CDSWavStream_Factory@0x0043bb00` | `OperatorNewWithBadAlloc(0x40)`; class-43 registry @ `0x0047d9e0` |
| Factory zeros tail | `CDSWavStream_Factory@0x0043bb00` | Decompile: `pStreamStorage = NULL`, `dwReservedTail = 0`; also clears `pVftable_sub14` @ +0x14 |
| Bank ctor field writes | `CDSAudioBank_Ctor@0x00429480` | `in_EAX[5]` → +0x14 `pVftable_bankDeserialize`; `in_EAX[6/7]` → +0x18/+0x1c slot vector seeds |
| Slot vector consumer | `CDSAudioBank_ReleaseSampleSlots@0x00429240` | `(this->slotVector).nCapacity` / `.pSlots`; `CDSPtrSlotVec_Resize(&this->slotVector, 0)` |
| Deserialize resize | `CDSAudioBank_Deserialize@0x00429600` | `CDSPtrSlotVec_Resize((CDSAudioBank *)&this->pVftable_IDSEventHandler, count)` ≡ `+0x18` |
| MI typeinfo +4 adjust | `CDSAudioBank_TypeinfoAdjust_4@0x0042fd30` | Returns `param_1 + 4` (event-handler subobject) |
| Ghidra size / offsets | MCP `get_struct_layout` | 64 B; `pVftable_bankDeserialize` @ 0x14, `slotVector` @ 0x18, `pStreamStorage` @ 0x20, `dwReservedTail` @ 0x38, `pad_operatorNew0x40` @ 0x3c |

### Factory tail semantics (named)

| Offset | Name | Factory init | Post-factory use |
|--------|------|--------------|------------------|
| +0x20 | `pStreamStorage` | `NULL` @ factory | **CDSWavStream** attach/save (`FUN_0043bb50`, `CDSWavStream_SaveToStream`); unused on pure bank deserialize |
| +0x38 | `dwReservedTail` | `0` @ factory | No consumer on 0x40-byte bank/wav object; pads heap blob to `OperatorNew(0x40)` |

## Ghidra deltas

- **`recreate_struct CDSAudioBank`** — explicit 64 B layout; fixed +6 B drift on `pVftable_bankDeserialize` / `slotVector` / tail band.
- **`set_function_prototype`** `CDSAudioBank_Ctor@0x00429480`, `CDSAudioBank_Deserialize@0x00429600`.
- **`set_function_this_type`** → `CDSAudioBank *` on ctor + deserialize.
- **`save_program bulanci.exe`**.

## Struct doc updates

- [CDSAudioBank.md](./CDSAudioBank.md) — Ghidra apply log (round-3 todo 23); layout table unchanged (already VERIFIED).

## Remaining limitations

- **`CDSAudioBank_Deserialize`** decompile still uses MI pointer arithmetic (`this[-1].…`) because vtable dispatch passes **`this` at +0x14** (deserialize facet), not object base — layout offsets are correct; facet `this` adjustment is a separate Ghidra MI modeling item.
- **`CDSAudioBank_Ctor`** — no CALL xrefs; class-43 runtime uses `CDSWavStream_Factory` only.
