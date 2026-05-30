# CDSFont

## Status

**VERIFIED** — instance size **VERIFIED** at `0x568` (1384 bytes). `CDSImage` bitmap header/body fields at `+0x08..+0x4b` are named and aligned with `CDSImage.md` (Ghidra `nM_*`). Font-specific vtable at `+0x04` replaces `CDSImage::m_width`; `glyphTable[256]` (`CDSFontGlyphRec`, 5 bytes) @ `+0x60`; tail metrics `+0x560` / `+0x564`. `justifyWidth` @ `+0x215` is a byte inside the glyph/payload blob (not a separate Ghidra field).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDSFont) == 0x568` | `CDSFont_AllocFactory` @ `0x00437561` | `OperatorNewWithBadAlloc(0x568)`; `CDSImage_InitDefaults(puVar1)`; font vtables at `puVar1[0x13]`/`[0x15]`/`[0x16]`; `puVar1[0x158]`/`[0x159]` cleared → `+0x560`/`+0x564` |
| Footer read | `CDSFont__Read` @ `0x004372b0` | `LEA ECX,[EDI+0x50c]` + stream `Read`, 8 bytes |
| Glyph/metrics blob | `CDSFont__Read` @ `0x004372b0` | `ADD EDI,0xc` + `Read`, `0x500` bytes (ClassID 54 `.font` payload; overwrites `+0x0c..+0x50b` after `CDSImage_Load`) |
| Tail metrics used | `CDSFont__GetCharWidth` @ `0x00437330` | Loads `*(int*)(this+0x560)`; byte at `this+0x215` for centered glyphs |
| Line spacing | text layout @ `0x00427a00` region | `*(int*)(font+0x564)` added per wrapped line (`bulanci.ghidra.exe.c` ~62782) |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `vf_primary` | `CDSFont_AllocFactory@0x00437561` `*puVar1 = 0x487620` |
| `0x04` | 4 | `void *` | `vf_sub04` | `CDSFont_AllocFactory@0x00437561` `puVar1[1] = 0x48760c` — **font MI slot; occupies `CDSImage::m_width` offset** |
| `0x08` | 4 | `int` | `m_height` | Same offset/name as `CDSImage.md`; `CDSImage_InitDefaults@0x00425580` `param_1[2]=0`; `CDSImage_Load@0x00437160` stream read → `EDI-0x4c` when `EDI=this+0x54` |
| `0x0c` | 4 | `int` | `m_format` | `CDSImage_InitDefaults` `param_1[3]=0`; `CDSImage_Load` / `CDSFont__Read@0x004372b0` `0x500`-byte font blob overwrites from here |
| `0x10` | 4 | `int` | `m_stride` | `CDSImage_InitDefaults` `param_1[8]=0`; `CDSImage__Allocate@0x00436f40` `*(this+0x10)`; `CDSImage_Load` → `EDI-0x44` |
| `0x14` | 4 | `int` | `m_paletteMarker` | `CDSImage__Allocate` `*(this+0x14)=0xffffffff`; `CDSImage_Load` → `EDI-0x40` |
| `0x18` | 1 | `byte` | `m_fillByte` | `CDSImage__Allocate` `*(this+0x18)=0xff`; `CDSImage_Load` 1-byte read → `EDI-0x38` |
| `0x19` | 3 | — | `pad_19` | — (padding; see `CDSImage.md`) |
| `0x1c` | 4 | `void *` | `m_pixels` | `CDSImage__Allocate` / `CDSImage__FreeBuffers@0x00436d50` — font `Load` allocates primary plane at `EDI-0x34` → **`+0x20` (`m_auxBuffer`)** when `EDI=font+0x54` |
| `0x20` | 4 | `void *` | `m_auxBuffer` | `CDSImage_Load` pixel malloc `EDI-0x34`; optional indexed plane at `EDI-0x30` (`+0x24`) |
| `0x24` | 4 | `int` | `field_24` | `CDSImage__Allocate@0x00436f40` zeroed; `CDSImage_Load` reads → `EDI-0x1c` |
| `0x28` | 4 | `int` | `field_28` | `CDSImage__Allocate` zeroed; `CDSImage_Load` 1-byte read after `field_44` cluster |
| `0x2c` | 4 | `int` | `m_copyWidth` | `CDSImage__Allocate` copied from width slot; `CDSImage_Load` stores pixel buffer pointer here during load |
| `0x30` | 4 | `int` | `m_copyHeight` | `CDSImage__Allocate` / `CDSImage_Load` secondary plane |
| `0x34` | 4 | `int` | `m_paletteEntries` | `CDSImage__Allocate` `*(this+0x34)=param_5`; `CDSImage_Load` clears to 0 |
| `0x38` | 8 | `CDSPtrSlotVec` | `m_slotVector` | `CDSImage_Load` clears `pSlots`, sets `nCapacity` from palette marker (`CDSImage.md` / `CDSImage_dtor@0x004254f0`) |
| `0x40` | 4 | `int` | `field_40` | `CDSImage_Load` assigns from fill-byte dword |
| `0x44` | 4 | `int` | `field_44` | `CDSImage_ctor@0x00425460` (`= 8`); `CDSImage_Load` 4-byte stream read |
| `0x48` | 4 | `int` | `field_48` | `CDSImage_InitDefaults` `param_1[0x12]=8` |
| `0x4c` | 4 | `void *` | `vf_event` | `CDSFont_AllocFactory@0x00437561` `puVar1[0x13] = 0x4875f8` — same offset as `CDSImage::vf_IDSChained` |
| `0x54` | 4 | `void *` | `vf_streamHost` | `CDSFont_AllocFactory@0x00437561` `puVar1[0x15] = 0x4875dc` (`Read`/`ReadNoAlloc` vtable; **`this` for stream methods**) |
| `0x58` | 4 | `void *` | `vf_chainFace` | `CDSFont_AllocFactory@0x00437561` `puVar1[0x16] = 0x4875c4` — same offset as `CDSImage::vf_event` |
| `0x60` | 5×256 | `CDSFontGlyphRec` | `glyphTable[256]` | `CDSFont__DrawChar@0x00437370` / `GetCharWidth@0x00437330` — `ch*5+0x62` width byte; Ghidra `glyphTable` |
| `0x215` | 1 | `byte` | `justifyWidth` | `CDSFont__GetCharWidth` / `DrawChar` when `*param_2 & 0x20` |
| `0x560` | 4 | `int` | `defaultAdvance` | `CDSFont_AllocFactory` init `0`; `GetCharWidth` fallback when width byte is 0 |
| `0x564` | 4 | `int` | `lineSpacing` | `CDSFont_AllocFactory` init `0`; poem/text layout consumers |

### `CDSImage` sub-layout (`+0x08..+0x4b`)

Byte-for-byte **same offsets and field names** as [`CDSImage.md`](CDSImage.md) rows `m_height` … `field_48`. Evidence is inherited from `CDSImage` helpers invoked on the font object:

| Font offset | `CDSImage` field | Primary evidence |
|-------------|------------------|------------------|
| `0x08` | `m_height` | `CDSImage_InitDefaults`, `CDSImage_Load` (`EDI-0x4c`, `EDI=font+0x54`) |
| `0x0c` | `m_format` | `CDSImage_InitDefaults`; overlaid by `.font` `0x500` stream read |
| `0x10` | `m_stride` | `CDSImage__Allocate`, `CDSImage_Load` |
| `0x14` | `m_paletteMarker` | `CDSImage__Allocate`, `CDSImage_Load` |
| `0x18` | `m_fillByte` | `CDSImage__Allocate`, `CDSImage_Load` |
| `0x1c` | `m_pixels` | `CDSImage__FreeBuffers` (`EBP+0x1c`, `EBP=font+4`) |
| `0x20` | `m_auxBuffer` | `CDSImage_Load` primary allocation target for font |
| `0x24`–`0x48` | `field_24` … `field_48` | `CDSImage__Allocate`, `CDSImage_Load`, `CDSImage_InitDefaults` |

**`this` adjustment:** `CDSFont__Read` / `ReadNoAlloc` are installed on `vf_streamHost` (`+0x54`). `CDSImage_Load` is entered with `ECX = font+0x54`, uses `EBP = ECX-0x50 = font+4` for `CDSImage__FreeBuffers` / `ComputeBufferSize`, and addresses plane fields via `EDI±imm` so absolute font offsets match the table above. `DrawChar` passes `font+4` to `CPoemScroller::BlitDispatch` (`LEA EAX,[EBP+4]@0x004373fb`).

**Not shared with stock `CDSImage`:** `+0x04` is `vf_sub04`, not `m_width`. Width/height for the atlas still land at `+0x08` / following dwords via `CDSImage_Load` stream layout.

### `CDSFontGlyphRec` (5 bytes, stride proven)

| Offset | Size | Type | Name | Evidence |
|--------|------|------|------|----------|
| `0` | 2 | `ushort` | `offsetX` | `DrawChar` `*puVar1` |
| `2` | 1 | `byte` | `width` | `DrawChar` `(byte)puVar1[1]`; `GetCharWidth` `ch*5+0x62` |
| `3` | 1 | `byte` | `offsetY` | `DrawChar` `*(byte*)(puVar1+3)` |
| `4` | 1 | `byte` | `height` | `DrawChar` `(byte)puVar1[2]` |

`256 × 5 = 0x500` matches `CDSFont__Read` stream read starting at `+0x0c`. See [`CDSFontGlyphRec.md`](CDSFontGlyphRec.md).

### Payload tail (ClassID 54 stream vs instance layout)

| Offset | Size | Name | Stream / init | Evidence |
|--------|------|------|---------------|----------|
| `0x0c` | `0x500` | `embeddedFontPayload` | `CDSFont::Read` `Read(..., 0x500)` after `ADD EDI,0xc` @ `0x004372da` | Overwrites `m_format`..`+0x50b`; `glyphTable[256]` @ `+0x60` is a **view** into this blob |
| `0x215` | 1 | `justifyWidth` | Inside `embeddedFontPayload` | `CDSFont__GetCharWidth@0x00437330` `MOVZX` from `[this+0x215]` when `*flags & 0x20` |
| `0x50c` | 4 | `int` | `fontFooter_defaultWidth` | ClassID-54 stream dword0 after `CDSImage` body (`bulanci_unpack` `_save_font` → JSON `defaultWidth`); `Read`/`ReadNoAlloc` @ `0x004372c5` |
| `0x510` | 4 | `int` | `fontFooter_lineHeight` | Stream dword1 → JSON `lineHeight`; same 8-byte `Read` as dword0 |
| `0x50c` | 8 | — | `fontFooter` (blob) | Single stream `Read`/`Write` of both dwords; **not** a separate Ghidra field (overlaps `pGlyphTable` tail @ `+0x50f..+0x513`) |
| `0x514` | `0x4c` | — | Not read from stream | Zeroed by `OperatorNew`; between footer and ctor-initialized tail |
| `0x560` | 4 | `defaultAdvance` | `CDSFont_AllocFactory` zeros `puVar1[0x158]` | `GetCharWidth` fallback: dword divide-by-4 on `[this+0x560]` |
| `0x564` | 4 | `lineSpacing` | `CDSFont_AllocFactory` zeros `puVar1[0x159]` | Poem/text wrap: add `[font+0x564]` per line (`bulanci.ghidra.exe.c` ~62782) |

**Stream order** (`CDSFont::Read@0x004372b0`): `CDSImage_Load` → footer `+0x50c` (8 B) → payload `+0x0c` (0x500 B). Footer bytes are **not** overwritten by the payload (`0x0c + 0x500 = 0x50c`).

## Ghidra apply

```
get_struct_layout CDSFont → Size: 1384 (0x568)
search_instructions operand 0x50c → CDSFont::Read + ReadNoAlloc LEA only (no post-Read consumers)
search_instructions operand 0x510/0x513 → zero hits
set_decompiler_comment@0x004372c5  fontFooter: defaultWidth@+0x50c, lineHeight@+0x510 (persist-only)
set_decompiler_comment@0x0043733a  justifyWidth @ +0x215 (glyphTable[87].width)
modify_struct_field +0x08..+0x4b → CDSImage.md canonical names (MCP success; archive may still show nM_*)
save_program bulanci.exe   # agent todo 34 / r3-worker-34 / 2026-05-30
```

`modify_struct_field` rename to canonical tail/glyph names reported success but field names may remain `pGlyphTable` / `nDefaultAdvance` / `nM_*` in the type archive (MCP quirk; same as todo 9 `nEmbeddedImage_field_44`).

## Follow-up (round 3 task 11)

- Replaced `pCdsImagePrefix[68]` with per-field `CDSImage` layout `+0x08..+0x4b` in Ghidra `CDSFont`.
- See `round3_task_11_report.md`.

## UNK

- **`fontFooter` runtime use:** dword0/1 = persisted `defaultWidth` / `lineHeight` (todo 34, 2026-05-30; `bulanci_unpack` + `search_instructions`). **No** post-`Read` instruction consumers of `+0x50c`/`+0x510`. Live metrics use `defaultAdvance` @ `+0x560` (`GetCharWidth`, `>> 2`) and `lineSpacing` @ `+0x564` (`TextShaper_LayOutAndRender`, `CEdit_BuildAt`) — ctor-zeroed, not copied from footer by `CDSFont::Read`.
- `justifyWidth` @ `+0x215` aliases `glyphTable[87].width` (`0x60 + 87×5 + 2`); not a separate Ghidra field.
- Semantics of `field_44` / `field_48` constants `8` on fonts (same open item as `CDSImage.md`).
- Whether `m_pixels` (`+0x1c`) is used on fonts after `Load` (primary plane stored at `+0x20` for stream-host entry).
