# CDSFontGlyphRec

## Status

**VERIFIED** — 5-byte stride; embedded in `CDSFont::glyphTable[256]` @ parent `+0x60`.

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0` | 2 | `ushort` | `offsetX` | `CDSFont__DrawChar@0x00437370` |
| `2` | 1 | `byte` | `width` | `CDSFont__DrawChar`; `CDSFont__GetCharWidth@0x00437330` `ch*5+0x62` |
| `3` | 1 | `byte` | `offsetY` | `CDSFont__DrawChar` |
| `4` | 1 | `byte` | `height` | `CDSFont__DrawChar` |

## Ghidra apply

```
get_struct_layout CDSFontGlyphRec → 5 bytes (wOffsetX, bWidth, bOffsetY, bHeight)
Parent CDSFont.pGlyphTable @ +0x60 as CDSFontGlyphRec[256]
```

Canonical field names per table; Ghidra may still prefix `w`/`b` on the standalone type.

## Parent overlap

- `glyphTable[0xef].offsetX + 1` @ `+0x50c` is the start of the 8-byte `fontFooter` stream chunk (`fontFooter_defaultWidth` + `fontFooter_lineHeight`) — see [`CDSFont.md`](CDSFont.md) § Payload tail (todo 34).
