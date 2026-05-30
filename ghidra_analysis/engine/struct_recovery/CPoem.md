# CPoem

## Status

**PARTIAL** — heap size **`0x1c` (28 B)** and CDS registry **classId `0x7fb` (2043)** verified; poem text handle / cached wide string at proven offsets. Full MI vtable thunk graph not field-named.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CPoem) == 0x1c` | `CPoem_factory@0x00409ab0` | `OperatorNewWithBadAlloc(0x1c)`; seven dwords written `+0`..`+0x18` |
| Factory registered | `FUN_0047df60` / static init `@0x0047b369` | `HandleClassRegister(classId 0x7fb, CPoem_factory)` |
| Pool scan uses class id | `CPoemScroller_Constructor@0x004262c0` | `CMP [poolEntry+0xc], 0x7fb` before append to scroller list |

## Class registry (CDS engine)

| Claim | Address | Evidence |
|-------|---------|----------|
| `GetClassIdentifier` | `CPoem::GetClassIdentifier@0x00409420` | `return &DAT_004b3450` |
| Factory | `CPoem_factory@0x00409ab0` | Alloc `0x1c`, installs vtables `0x4804e0`, `0x4804cc`, `0x4804b4`, `0x480498` |
| Resource name vtable | `CPoem::GetResourceName@0x00409470` | `return &DAT_004b7f04` |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `pVftable_IDSReferenced` | `CPoem_factory@0x00409ab0` `*puVar1 = 0x4804e0` |
| `0x04` | 4 | `void *` | `pVftable_IDSFacet04` | `CPoem_factory@0x00409ab0` `puVar1[1]=0x4804cc`; **not** overwritten by Deserialize (stream MI `this+4` targets `+0x18`) |
| `0x08` | 4 | `int` | `refcount` | `CPoem_factory@0x00409ab0` `puVar1[2] = 1` |
| `0x0c` | 4 | `void *` | `pVftable_IDSChained` | `CPoem_factory@0x00409ab0` `puVar1[3] = 0x4804b4` |
| `0x10` | 4 | `int` | `field_10` | `CPoem_factory@0x00409ab0` `puVar1[4] = 0` |
| `0x14` | 4 | `void *` | `pVftable_IDSStream` | `CPoem_factory@0x00409ab0` `puVar1[5] = 0x480498` |
| `0x18` | 4 | `wchar_t *` | `pWstrHandle` | `CPoem_Deserialize@0x00409100` via IDSStream MI (`ADD ECX,4` from `+0x14`); `CPoemScroller_PickNextPoem@0x00425df0`; `CPoem::Destructor@0x004094a0` |

## Key methods

| Symbol | Address | Role |
|--------|---------|------|
| `CPoem_factory` | `0x00409ab0` | Heap alloc + vtable install |
| `CPoem_Deserialize` | `0x00409100` | Load wide string from stream into `pWstrHandle` |
| `CPoem_GetText` | `0x00409870` | Export `pWstrHandle` to stream |
| `CPoem::Destructor` | `0x004094a0` | Release `pWstrHandle` via `CDsStringReleaseHeader` @ `[this+0x18]` |

## Ghidra apply

```
delete_data_type CPoem
create_struct CPoem → Size: 28 (7 fields)
get_struct_layout CPoem
set_function_prototype CPoem_factory@0x00409ab0 → CPoem * CPoem_factory(void)
set_function_prototype CPoem_Deserialize@0x00409100
set_function_prototype CPoem_GetText@0x00409870
save_program bulanci.exe
```

## IDSStream vtable `0x480498` @ `+0x14` (R4 todo 17)

| Slot | Offset | Thunk / method | MI `this` adjust | Role |
|------|--------|----------------|------------------|------|
| +3 | `0x0c` | `DeletingDestructorThunk_12@0x00409440` | `SUB ECX,0x14` | deleting dtor from stream face |
| +4 | `0x10` | `Deserialize@0x00409100` | `ADD ECX,0x4` | load poem → `pWstrHandle` @ `+0x18` |
| +5 | `0x14` | `GetText@0x00409870` | `ADD ECX,0x4` | pack poem from `pWstrHandle` |

**GetText xrefs:** no direct code xrefs (vtable `0x004804ac` only). **Indirect:** `CDSCollection_SerializeElement@0x0042ff20` → `vcall [IDSStream+0x14]` from `CDSCollection_Save@0x00431210`. **Deserialize load:** `CDSCollection_DeserializeElement@0x0042fd40` → `vcall [+0x10]` from `CDSCollection_Load@0x00431360`. Runtime menu uses pool-loaded instances via `CPoemScroller_PickNextPoem` (`poem+0x18`), not GetText.

**MI deleting dtors** (R4 plated): `DeletingDestructorThunk_4@0x00409430` (`SUB 0xc`, chained `+0x0c`); `DeletingDestructorThunk_10@0x00409460` (`SUB 0x4`, facet `+0x04`).

## UNK

- Remaining shared `CDSFileStream` / `IDSStream` slot entries at `0x480498` slots 0–2 (not CPoem-specific).

## pWstrHandle vs +0x04 / pCachedText (R3 todo 17, VERIFIED)

Deserialize/GetText are **IDSStream** virtuals (`pVftable_IDSStream` @ `+0x14`, vtable `0x480498`). Thunk `ADD ECX,4` from stream ECX stores the pack poem into **`pWstrHandle` @ primary `+0x18`**, not into `+0x04`. `+0x04` remains the factory’s second MI vtable (`0x4804cc`). `CPoemScroller_PickNextPoem` and `CPoem::Destructor` both use `+0x18` only. **`pCachedText`** is not a separate field — it is the same CDS refcounted wide-string handle as `pWstrHandle`.

## Parent / consumers

- **`CPoemScroller`** — sorted `CPoem*` list at `poemSlots`; random pick in `CPoemScroller_PickNextPoem` (`CPoemScroller.md`).
- **Asset catalog** — overlay poems class `0x7fb`; resource IDs `0x10005`..`0x1000e` (wave2 batch0).
