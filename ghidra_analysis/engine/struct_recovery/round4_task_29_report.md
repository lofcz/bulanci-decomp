# Round 4 — Agent todo 29 report

## Task

| Field | Value |
|-------|-------|
| **id** | 29 |
| **title** | CDSEasyMemStream IDSStream ReadBytes ECX plate at outer+0x0c |
| **types** | `CDSEasyMemStream`, `IDSStream`, `CDSException` |
| **ghidra_actions** | `create_struct`, `set_function_this_type`, `set_function_prototype`, `set_decompiler_comment`, `force_decompile`, `save_program` |
| **source** | blocker (R3 handoff UNK on stream slot `this` typing) |

## Status

**DONE**

## Evidence

### Before / after decompile (`ReadBytes@0x004307f0`)

| Phase | `this` type | Sample field access |
|-------|-------------|---------------------|
| R3 end | `CDSEasyMemStream *` | `this->dwCapacity_field` as backing ptr; `this->dwRefcount` as cursor (wrong outer offsets) |
| R4 | `IDSStream *` | `this->pBacking_heap`, `this->dwCursor`, `this->dwRing_head_offset`, `this->dwCapacity_field` |

### `IDSStream` plate struct

| Claim | Evidence |
|-------|----------|
| Size 32 B | `create_struct` + `get_struct_layout("IDSStream")` |
| Plate @ outer `+0x0c` | MI docs; `CloseStream` uses `&this[-1].dwGrowth_chunk` ≡ outer base |
| Field map | Matches `CDSEasyMemStream` outer `+0x10..+0x28` |

### Functions retyped to `IDSStream *`

| Address | Symbol | Result |
|---------|--------|--------|
| `0x004307f0` | `IDSStream::ReadBytes` | Cursor/backing/ring decompile correct |
| `0x004308c0` | `IDSStream::WriteBytes` | `EnsureCapacity` on outer via `this-0xc` |
| `0x00430980` | `IDSStream::SeekPosition` | `dwCursor` / `dwLogical_size` bounds |
| `0x00409220` | `IDSStream::GetSize` | Returns `dwLogical_size` |
| `0x00409230` | `IDSStream::TellPosition` | Returns `dwCursor` |
| `0x004309f0` | `IDSStream::SetStreamSize` | Moved to `IDSStream` class |
| `0x00409200` | `IDSStream::CloseStream` | `__thiscall` + `this->dwStreamState = 0x20`; `ReleaseBackingBuffer` on outer |

### Unchanged (outer `this`)

| Address | Symbol | Note |
|---------|--------|------|
| `0x00430d60` | `CDSEasyMemStream_InitBackingBuffer` | Correctly uses outer `CDSEasyMemStream *` |
| `0x00409170` | `CDSEasyMemStream_ctor` | MI header on outer object |

## Ghidra deltas

- **`IDSStream`** struct created (8 fields, 32 B).
- Seven stream vtable bodies moved into class namespace `IDSStream` with `IDSStream *` auto-`this`.
- `CloseStream`: `__fastcall` → `__thiscall` prototype, then `set_function_this_type`.
- PRE comments @ `ReadBytes` / `WriteBytes` (R4 plate note).
- `save_program bulanci.exe`.

## Struct doc updates

- [CDSEasyMemStream.md](./CDSEasyMemStream.md) — R4 todo 29 note; removed resolved ReadBytes UNK.
- [IDSStream.md](./IDSStream.md) — new plate layout doc.

## Remaining UNK

- `IDSStream` layout is **implementer-specific** past shared header; `CDSFilterStream_ReadBytes` still needs its own plate typing (R4 todo 30).
- `IDSEventHandler` facet field map on `CDSEasyMemStream` — unchanged.
