# Struct recovery batch 00/50

**Program:** `bulanci.exe`  
**Structs:** `CBulanci`, `CDSObject`  
**Saved:** yes (`save_program` at end of batch)

## Status table

| Struct | Status | Proven size | Ghidra size after apply | Notes |
|--------|--------|-------------|-------------------------|-------|
| `CBulanci` | **PARTIAL** | `0x4cc` (1228) | 1288 | `OperatorNewWithBadAlloc(0x4cc)` @ `CBulanci_CreateObject` (`0x00402ab1`). Ghidra type merged with prior partial layout (vtables, rect, resource pool); tail audio-bank dynarray at `+0x4b0` (`1200` dec) matches ctor/dtor. |
| `CDSObject` | **PARTIAL** | `0x48` TM / `0x60` image | 95 | Track-manager factory `OperatorNew(0x48)` @ `0x00439f9a`; image paths `OperatorNew(0x60)` @ `0x00432c0e`, `0x00439af3`. Fields through `+0x44` from `ConstructTrackManager`; image fields `+0x50`/`+0x5c` from `CDSObject_CtorWithImage`. |

## Deliverables

| File | Description |
|------|-------------|
| [CBulanci.md](./CBulanci.md) | Size proof, ctor/dtor fields, embedded `CGame` @ `+0x284` |
| [CDSObject.md](./CDSObject.md) | Dual alloc sizes, track-manager + image ctor evidence |

## Apply notes

- `create_struct` failed (placeholders already existed); fields added via `add_struct_field` after merging with existing `CBulanci` work.
- `CDSObject` rebuilt incrementally to ~`0x60` bytes; prefix `+0x04..+0x10` still reflects older refcount stubs — see UNK in per-struct docs.
- **Next batch:** recover standalone `CGame` (`0x248` at `CBulanci+0x284`) to replace `byte[584]` / pad region and reconcile `CBulanci` Ghidra size with `0x4cc`.
