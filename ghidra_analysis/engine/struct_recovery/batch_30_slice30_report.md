# Slice 30/50 — `CDSFlxFile`, `CDSFont` (agent 30)

**Date:** 2026-05-30  
**Program:** `bulanci.exe`

## Types mapped

| Type | Size | Ghidra | Doc |
|------|------|--------|-----|
| `CDSFlxFile` | `0x50` | 20 fields; header dwords renamed | `CDSFlxFile.md` |
| `CDSFont` | `0x568` | `CDSImage` prefix + `glyphTable[256]` + tail metrics | `CDSFont.md` |
| `CDSFontGlyphRec` | 5 | `wOffsetX`, `bWidth`, `bOffsetY`, `bHeight` | `CDSFont.md` |
| `CDSObject` (consumer) | `0x60` | `DecodeFrame` / `CreateBoundClone` param (existing type) | `CDSObject.md`, chain in `CDSFlxFile.md` |

## Functions / prototypes

| Address | Symbol | Action |
|---------|--------|--------|
| `0x00432c60` | `CDSFlxFile_DecodeFrame` | `set_function_prototype` — `CDSObject *consumer` |
| `0x00432be0` | `CDSFlxFile_CreateBoundClone` | `set_function_prototype` — returns `CDSObject *` |
| `0x00432ac0` | `CDSFlxFile_BindStream` | Evidence for header field renames |
| `0x004372b0` | `CDSFont__Read` | `CDSImage_Load` + `0x500` @ `+0x0c` |
| `0x00437330` | `CDSFont__GetCharWidth` | `+0x215` justify, `ch*5+0x62`, `+0x560` default |
| `0x004101d0` | `CBulPicture_ctor` | Resource load → `pBitmap` @ `+0x68` |

## Ghidra actions

- `modify_struct_field` `CDSFlxFile`: `dwHdr_dword0..4`, `dwHdr_tailDword`, `dwChildWindowId` → `nChannels`, `nBitmapHeight`, `nBitmapWidth`, `nInMemSizeHint`, `nAnimFrameCountMinusOne`, `dwEncodedSize2`, `dwTotalSize`
- `modify_struct_field` `CDSFont`: `pGlyphTable` → `glyphTable`
- `set_function_prototype` @ `0x00432c60`, `0x00432be0`
- `save_program bulanci.exe`

## Files changed

- `struct_recovery/CDSFlxFile.md`
- `struct_recovery/CDSFont.md`
- `struct_recovery/CBulPicture.md` (FLX chain cross-ref)
- `engine/gameplay_struct_backlog.md` (slice 30 → VERIFIED)
- `struct_recovery/batch_30_slice30_report.md` (this file)

## Blockers

- ~~**`DecodeFrame` callsites**~~ **Resolved (worker 31):** static xref `0x004872c0` only; live dispatch `TM_AdvanceFrame@0x00439a15` → meta vtable slot 7 (`CDSFlxFile.md` § DecodeFrame callsites).
- **`bodySeekBiasLo`** remains zero in all in-module writers; non-zero path unproven.
- **`justifyWidth`** / `fontFooter` not modeled as separate Ghidra fields (overlap `glyphTable` payload).
