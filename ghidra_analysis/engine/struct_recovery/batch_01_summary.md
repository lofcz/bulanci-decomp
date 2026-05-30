# Struct recovery batch 01/50

**Program:** `bulanci.exe`  
**Structs:** `CBulanci`, `CDSObject`  
**Saved:** `save_program bulanci.exe` (end of batch)

## Results

| Struct | Status | Runtime size proof | Ghidra struct size (post-apply) |
|--------|--------|-------------------|--------------------------------|
| `CBulanci` | PARTIAL | **`0x4CC`** — `CBulanci_CreateObject` @ `0x00402a90` | 1323 B (tail @ `+0x4b0` correct; duplicate block @ `+0x510` — cleanup) |
| `CDSObject` | PARTIAL | **`0x48`** TM / **`0x60`** image (`ConstructTrackManager`, `CDSObject_CtorWithImage`) | 47 B (core ref block applied; auto tail — cleanup) |

## Deliverables

- `CBulanci.md` — heap size, embedded `CGame` @ `+0x284`, dynarray tail `+0x4b0..+0x4c8`
- `CDSObject.md` — IDSReferenced vtable, TM layout through `+0x44`, image fields `+0x50`/`+0x5c`

## Apply notes

- Pre-existing 1-byte placeholders blocked `create_struct`; layouts applied via `add_struct_field` / `modify_struct_field`.
- **CBulanci:** proven offsets through `+0x4c8` pinned; Ghidra type overshoots `0x4CC` due to overlapping auto/duplicate tail fields.
- **CDSObject:** bare IDSReferenced ref block at `+0..+0x10` applied; full `0x60` image superset documented in `.md` for follow-up struct expansion.

## Next batch

Index 1 in `batches_50.json`: **`CDSApp`**, **`CGameView`**.
