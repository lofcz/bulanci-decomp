# Struct recovery batch 36/50

**Index:** 36 (`batches_50.json`)  
**Types:** `CDSSimpleException`, `CDSStreamException`  
**Program:** `bulanci.exe`  
**Saved:** yes (`save_program` after apply)

## Results

| Struct | Status | Size | Ghidra |
|--------|--------|-----:|--------|
| `CDSSimpleException` | VERIFIED | 0x3C (60) | `create_struct` → `get_struct_layout` confirms 60 B |
| `CDSStreamException` | VERIFIED | 0x50 (80) | `create_struct` → `get_struct_layout` confirms 80 B |

## Evidence summary

Both types are **CDSException** subclasses (MSVC C++ exceptions, not CRT `std::exception`).

- **Shared prefix (0x00–0x13):** `CDSException_InitFields@0x00434a40` — vftable, `deleteOnRelease` byte at +4, cached message pointer at +8, two `uint` codes at +0xC/+0x10.
- **Inline wchar buffer (+0x14, 0x28 bytes):** `CDSException_GetMessageW@0x00434b80`; extent fixed by `OperatorNew(0x3c)` on simple throws.
- **CDSSimpleException:** `CDSSimpleException_Throw@0x00434c20` allocates **0x3C**, sets vtable **0x48754c**; no extra fields beyond base buffer.
- **CDSStreamException:** `OperatorNew(0x50)` in `RaiseStreamException@0x00430270` / `ThrowStreamErrorNoReturn@0x004302e0`; ctor `CDSStreamException_ctor@0x004300f0` adds three `CDSString` handles (+0x3C/+0x40/+0x44), engine errno (+0x48), Win32 code (+0x4C); dtor releases strings at indices 0xF–0x11.

## Deliverables

- [CDSSimpleException.md](./CDSSimpleException.md)
- [CDSStreamException.md](./CDSStreamException.md)

## Follow-ups (out of scope)

- Parent type `CDSException` formal struct (batch 26 sibling in manifest) — reuse same 0x3C prefix.
- Remaining exception siblings: `CDSApiException` (0x44), `CDSMemoryException`, `CDSDirectXException`, etc.
