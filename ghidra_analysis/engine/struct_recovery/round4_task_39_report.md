# Round 4 — Task 39 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 39 |
| **round** | 4 |
| **title** | Exception throw-site this types and vtable slot catalogs |
| **one_liner** | Close R3 blockers: typed throw alloc locals (`CDSApiException *`, …); document IDSChained 5-slot vtables for exception siblings. |
| **prior** | [round3_task_39_report.md](./round3_task_39_report.md) |
| **types** | `CDSException`, `CDSApiException`, `CDSDirectXException`, `CDSResourceException`, `CDSStreamException`, `CDSSimpleException` |

## Status

**DONE** (throw-site locals largely fixed; vtable catalogs documented; one decompiler namespace cosmetic remains)

## Evidence

### Throw-site decompile (after `set_local_variable_type` + `force_decompile`)

| Address | Function | Before | After |
|---------|----------|--------|-------|
| `0x00434d00` | `CDSApiException_ThrowFromGetLastError` | `CDSException *local_4`; `local_4[1].*` tail | `CDSApiException *local_4`; `pFormattedMessage`, `dwWin32Error` |
| `0x0043b820` | `CDSDirectXException_ThrowFromHresult` | `CDSException *this`; `this[1].*` | `CDSDirectXException *`; `pMessage_handle`, `dwContext_code`, `dwHresult` |
| `0x00434c20` | `CDSSimpleException_Throw` | `CDSException *local_4` | `CDSSimpleException *local_4` (still casts for `InitFields` / vtable — see UNK) |
| `0x004346f0` | `CDSResourceException_ThrowFromResourceId` | Already `CDSResourceException *local_10` | Unchanged; decompiler **display** prefix still `CDSStreamStorage::` |

`CDSStreamException_ctor@0x004300f0` was already correct (`CDSStreamException *this`, `&this->base`, tail fields).

### IDSChained vtable @ `+0` (5 slots, `vftable_methods.csv` + decompile)

Shared tail slot `[4]` = `CItemInfo::CDSObject_GetThis@0x00434b10` on all listed types. Slot `[2]` = `CDSException::CDSException_ReleaseViaFlag@0x0042fff0`.

| Class | Vtable | [0] | [1] | [2] | [3] | [4] |
|-------|--------|-----|-----|-----|-----|-----|
| `CDSSimpleException` | `0x48754c` | `GetClassTable@0x434ac0` | `CDSException_DtorScalar@0x434ae0` | `ReleaseViaFlag@0x2fff0` | `What@0x434ad0` | `CDSObject_GetThis@0x34b10` |
| `CDSApiException` | `0x487564` | `GetClassTable@0x434b00` | `DtorScalar@0x434e10` | `ReleaseViaFlag` | `What@0x434c70` | `CDSObject_GetThis` |
| `CDSResourceException` | `0x4874b0` | `GetClassTable@0x434430` | `DtorScalar@0x4348e0` | `ReleaseViaFlag` | `What@0x434350` | `CDSObject_GetThis` |
| `CDSStreamException` | `0x4870cc` | (see [CDSStreamException.md](./CDSStreamException.md)) | | | | |
| `CDSDirectXException` | `0x489474` | `FUN_0043b750` … | catalog in `master_vtable_catalog.csv` | | | |

`read_memory@0x487564` (20 B): five LE32 pointers matching the table above.

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `set_local_variable_type` | `local_4` @ `0x00434c20` | `CDSSimpleException *` |
| `set_local_variable_type` | `local_4` @ `0x00434d00` | `CDSApiException *` |
| `set_local_variable_type` | `this`, `local_10` @ `0x0043b820` | `CDSDirectXException *` |
| `set_function_prototype` | `0x004346f0` | `void __cdecl CDSResourceException_ThrowFromResourceId(uint)` |
| `set_plate_comment` / `set_decompiler_comment` | throw sites `0x434c20`, `0x434d00`, `0x4346f0` | alloc / vtable notes |
| `force_decompile` | throw helpers above | typed tails where struct embed applies |
| `save_program` | `bulanci.exe` | success |

`run_script_inline` namespace reparent for `0x004346f0` → `CDSResourceException` class failed (OSGi `ClassNotFoundException`; same failure mode as [CDSCollection.md](./CDSCollection.md)).

## Struct doc updates

- [CDSApiException.md](./CDSApiException.md) — R4 throw decompile + vtable table
- [CDSResourceException.md](./CDSResourceException.md) — vtable slots; throw namespace note
- [CDSSimpleException.md](./CDSSimpleException.md) — R4 throw local type
- [CDSDirectXException.md](./CDSDirectXException.md) — R4 throw fix (closes UNK)
- [CDSException.md](./CDSException.md) — shared `ReleaseViaFlag` / `DtorScalar` cross-ref

## Remaining UNK

- `CDSSimpleException_Throw`: `CDSException_InitFields((CDSException *)local_4, …)` and vtable store on cast base — needs `InitFields(&local_4->base, …)` in decompiler (signature / promotion limitation).
- `CDSResourceException_ThrowFromResourceId`: symbol name is correct; listing decompile may still show `CDSStreamStorage::` parent prefix until manual Symbol Tree reparent.
- `CDSMemoryException` / other siblings: vtable slot **names** for a few `FUN_*` entries in `vftable_methods.csv` not renamed in Ghidra (catalog only).
