# Struct recovery batch 30/50 — follow-up round 2

**Status:** `FOLLOWUP_COMPLETE`  
**Prior:** `batch_30_summary.md`  
**Structs:** `CDSFlxFile`, `CDSFont`

## Actions taken

| Follow-up (prior batch) | Result |
|-------------------------|--------|
| Merge `*_recovered` into placeholder `CDSFlxFile` / `CDSFont` | **Done** — `CDSFlxFile` and `CDSFont` expanded to 0x50 / 0x568 in the type archive |
| Map FLX header dwords at `BindStream` | **Done** — disassembly @ `0x00432ac0` + `flx_file_format.md` file↔object table; renamed `hdr_dword0`..`hdr_dword4`, `hdr_tailDword` |
| Expand `CDSFont` glyph struct | **Done** — `CDSFontGlyphRec` (5 bytes) + `glyphTable[256]` @ `+0x60`; proof from `CDSFont::GetCharWidth` / `DrawChar` |

## Ghidra deltas

- `create_struct` `CDSFontGlyphRec` (5 bytes: `offsetX`, `width`, `offsetY`, `height`)
- `CDSFlxFile` rebuilt via `add_struct_field` (80 bytes, 20 fields — matches `CDSFlxFile_recovered`)
- `CDSFont` rebuilt via `add_struct_field` (1384 bytes; `pGlyphTable` = `CDSFontGlyphRec[256]` @ `+0x60`)
- `save_program bulanci.exe`

## Doc updates

- `CDSFlxFile.md` — header dword offsets + Ghidra apply
- `CDSFont.md` — `CDSFontGlyphRec` table, glyph stride proof
- `flx_file_format.md` — corrected outer-offset table (removed erroneous `+0x28` header dword)

## Remaining UNK

- **CDSFlxFile:** ~~`hdr_dword0`..`hdr_dword4` / `hdr_tailDword`~~ **resolved** (agent todo 33 — `CDSFlxFile.md` + `flx_file_format.md`); ~~`field_2c`~~ **resolved** (round 3 task 4 → `bodySeekBiasLo`)
- **CDSFont:** ~~`fontFooter` / `justifyWidth` layout~~ **documented** (agent todo 34 — `CDSFont.md` § Payload tail, decompiler comments); `fontFooter` dword semantics still UNK

## Agent todo 34 (2026-05-30)

- `CDSFont.md` payload-tail offset table; new `CDSFontGlyphRec.md`
- Ghidra: `set_decompiler_comment@0x004372c5`, `@0x0043733a`; `save_program`

## Handoff

Batch 30 follow-ups from round 1 are addressed. No blocking items for batch 31.
