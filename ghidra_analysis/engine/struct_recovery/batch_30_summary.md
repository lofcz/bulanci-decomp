# Struct recovery batch 30/50

**Structs:** `CDSFlxFile`, `CDSFont`  
**Program:** `bulanci.exe`  
**Evidence:** Ghidra MCP decompile + instruction search + factory functions created at `0x00432a50` / `0x00437561`

## Results

| Struct | Status | Size | Proven fields |
|--------|--------|------|---------------|
| `CDSFlxFile` | VERIFIED | `0x50` | 17 named slots — 5 vtables, refcount, stream tell, source/decode pointers, body cursors, child-window id |
| `CDSFont` | PARTIAL | `0x568` | 8 named anchors + glyph/width accessors at `+0x60`/`+0x62`; `CDSImage` core reused |

## Key evidence anchors

- `CDSFlxFile_CreateObject` @ `0x00432a50` — `OperatorNew(0x50)`; stamps vtables `0x4872d0` / `0x4872a4` / `0x487290` / `0x487274` / `0x48725c`.
- `CDSFlxFile_BindStream` @ `0x00432ac0` — 36-byte header; outer `+0x30` tell, `+0x38` stream, `+0x40`/`+0x44` cursors.
- `CDSFlxFile::DecodeFrame` @ `0x00432c60` — chunk dispatcher; circular body over `+0x3c..+0x44`.
- `CDSFont_AllocFactory` @ `0x00437561` — `OperatorNew(0x568)`; `CDSImage_InitDefaults`; font vtables `0x487620` family.
- `CDSFont__Read` @ `0x004372b0` — `CDSImage_Load` + `0x500` bytes `@+0x0c` + 8-byte footer `@+0x50c`.

## Ghidra actions

- [x] `create_struct` `CDSFlxFile_recovered` (size 0x50)
- [x] `create_struct` `CDSFont_recovered` (size 0x568)
- [x] `get_struct_layout` verified size > 1 for both recovered types
- [x] `create_function` `CDSFlxFile_CreateObject`, `CDSFont_AllocFactory`
- [x] `save_program bulanci.exe`

## Follow-ups

- Merge `*_recovered` layouts into placeholder `CDSFlxFile` / `CDSFont` types (placeholders still 1 byte).
- Map FLX header dwords at `BindStream` negative adjustor offsets (`flx_file_format.md` open items).
- Expand `CDSFont` glyph struct (`(u16 x,u8 w,u8 y,u8 h,u8 width)` × 256) using `open_bulanci/assets/fonts/bitmap/*.font.json` as a cross-check only after dword proofs.
