# Struct recovery batch 29/50

**Structs:** `CDSStreamStorage`, `CDSStrmResInfo`  
**Program:** `bulanci.exe`  
**Evidence:** Ghidra MCP decompile + `OperatorNew` / ctor / consumer xrefs only

## Results

| Struct | Status | Size | Proven fields |
|--------|--------|------|---------------|
| `CDSStreamStorage` | VERIFIED | `0x60` | 19 fields — MI header, stream base offset, root safe-stream, embedded `CDSCollection`, `CDSChain`, CRITICAL_SECTION |
| `CDSStrmResInfo` | PARTIAL | `0x28` | 9 fields — `CDSResInfo` keys + stream offset/size; factory alloc not disassembled |

## Key evidence anchors

- `CDSStreamStorage_ctor` @ `0x00401790` — triple vtable init, collection/chain subobjects, `InitializeCriticalSection(this+0x48)`.
- `CBulanci_OpenPackStream` @ `0x00401f1b` — `OperatorNew(0x60)` for pack stream storage.
- `CDSStreamStorage_dtor` @ `0x00433e60` — drains chain, destroys collection, deletes critical section.
- `FUN_00433f70` @ `0x00433f70` — locked stream splice; reads `CDSStrmResInfo` entry offsets +0xC/+0x14/+0x18/+0x1C/+0x20 (entry = object+4).
- `CDSStrmResInfo_Serialize` / `Deserialize` @ `0x00433940` / `0x00433980` — extends `CDSResInfo_Load/Save` with 8+4 byte tail.
- `CloseStreamByKey` @ `0x00433cb0` — binary-searches collection using `CDSStrmResInfo` COL keys `0x4873f8` / `0x4873dc`.

## Ghidra actions

- [x] `create_struct` / replace `CDSStreamStorage` (size 0x60)
- [x] `create_struct` / replace `CDSStrmResInfo` (size 0x28)
- [x] `get_struct_layout` verified size > 1 for both
- [x] `save_program bulanci.exe`

## Follow-ups

- Disassemble factory `0x00433d80` to upgrade `CDSStrmResInfo` to VERIFIED via allocation xref.
- Recover standalone `CDSCollection` / `CDSChain` structs to deduplicate embedded layouts.
- Name `resField0` / `streamKey` from `.eap` resource index walk in `CDSCollection_Load`.
