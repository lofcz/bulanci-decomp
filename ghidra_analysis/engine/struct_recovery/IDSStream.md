# IDSStream (vtable plate)

## Status

**VERIFIED** for `CDSEasyMemStream` implementer — 32-byte `IDSStream` plate at outer `+0x0c`; stream slot methods use `IDSStream *this` (ECX = plate, not outer).

## Layout (CDSEasyMemStream plate)

| Offset | Size | Type | Name | Outer equiv |
|--------|------|------|------|-------------|
| `+0x00` | 4 | `void *` | `pVftable` | `vf_IDSStream` @ outer `+0x0c` |
| `+0x04` | 4 | `uint` | `dwStreamState` | outer `+0x10` (`7` OPEN, `0x20` CLOSED) |
| `+0x08` | 4 | `uint` | `dwCursor` | outer `+0x14` |
| `+0x0c` | 4 | `uint` | `dwLogical_size` | outer `+0x18` |
| `+0x10` | 4 | `uint` | `dwCapacity_field` | outer `+0x1c` |
| `+0x14` | 4 | `uint` | `dwGrowth_chunk` | outer `+0x20` |
| `+0x18` | 4 | `uint` | `dwRing_head_offset` | outer `+0x24` |
| `+0x1c` | 4 | `void *` | `pBacking_heap` | outer `+0x28` |

## Ghidra apply

```
get_struct_layout("IDSStream") → Size: 32 bytes
```

R4 todo 29: `create_struct` + `set_function_this_type` on `CDSEasyMemStream` vtable slots (`ReadBytes`, `WriteBytes`, `SeekPosition`, `TellPosition`, `GetSize`, `SetStreamSize`, `CloseStream`). `CloseStream` required `__thiscall` prototype before `this` typing.

## Notes

- Other `IDSStream` implementers (`CDSFilterStream`, `CDSQueueStream`, …) extend different tail fields on the same vtable; do not reuse this 32 B layout blindly on those bodies (see per-type stream docs).
- Outer recovery idiom: `(CDSEasyMemStream *)&this[-1].dwGrowth_chunk` ≡ `this - 0xc` when plate is at outer `+0x0c`.
