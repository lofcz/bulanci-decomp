# Round 3 — Task 34 report

## 1. Task

| Field | Value |
|-------|-------|
| **id** | 34 |
| **title** | CDSFont fontFooter dword semantics @ +0x50c |
| **one_liner** | Name and prove the 8-byte ClassID-54 stream chunk at `CDSFont+0x50c`; decide whether to split into two dwords in Ghidra. |
| **acceptance** | Update `CDSFont.md`; Ghidra comments; `agent_todos_50_r3_results.jsonl` row |

## 2. Status

**DONE** — footer dword meanings proven from on-disk layout; no per-dword runtime consumers; Ghidra struct left as overlapping `pGlyphTable` tail (no new fields).

## 3. Evidence

| Offset | Name | Semantics | Evidence |
|--------|------|-----------|----------|
| `+0x50c` | `fontFooter_defaultWidth` | Signed `int`; default glyph advance for empty/zero-width slots in the **file** format | `tools/bulanci_unpack/bulanci_unpack.py` `_save_font`: `struct.unpack_from("<ii", raw, image_size)` dword0 → JSON `defaultWidth` (sample font `19`) |
| `+0x510` | `fontFooter_lineHeight` | Signed `int`; line height in the **file** format | Same unpacker dword1 → JSON `lineHeight` (sample `16`) |
| `+0x50c..+0x513` | `fontFooter` (8 B) | Persisted as one stream `Read`/`Write` block | `CDSFont::Read@0x004372b0` disasm: `CDSImage_Load` → `LEA ECX,[EDI+0x50c]` + `PUSH 8` @ `0x004372c5` → `ADD EDI,0xc` + `PUSH 0x500`; `ReadNoAlloc@0x00437305` symmetric `Write` |
| Layout overlap | — | Footer sits immediately after the `0x500`-byte embedded band (`+0x0c + 0x500 = +0x50c`); last 5 bytes alias `glyphTable[239]` in a flat 256×5 view | `CDSFont.md` § Payload tail; decompiler `&pGlyphTable[0xef].wOffsetX + 1` is a byte-offset artifact, not a separate field |

### Runtime consumers (negative result)

| Search | Result |
|--------|--------|
| `search_instructions` operand `0x50c` | Only `CDSFont::Read` / `CDSFont::ReadNoAlloc` `LEA [EDI+0x50c]` (+ unrelated CRT stack slots in `__write_nolock`) |
| `search_instructions` operand `0x510` / `0x513` | Zero hits |
| `search_instructions` operand `0x560` | `GetCharWidth` read; `AllocFactory` zero write only |
| `search_instructions` operand `0x564` | `TextShaper_LayOutAndRender`, `CEdit_BuildAt` reads; `AllocFactory` zero write only |

**Architectural note:** Live layout uses `nDefaultAdvance` @ `+0x560` (`GetCharWidth` fallback, scaled `>> 2`) and `nLineSpacing` @ `+0x564` (`TextShaper_LayOutAndRender`, `CEdit_BuildAt`). Those slots are **ctor-zeroed** and **not** filled from `fontFooter` by `Read`. Persisted metrics live at `+0x50c`/`+0x510` only. Cross-check: gallery `FontRenderer` uses JSON `defaultWidth/2` for preview, paralleling `>> 2` on `+0x560`, not direct use of `+0x50c`.

### `justifyWidth` @ `+0x215` (related, not footer)

| Claim | Evidence |
|-------|----------|
| Byte inside embedded `0x500` glyph band | `CDSFont::GetCharWidth@0x00437330` `MOVZX` from `[ECX+0x215]` when `*flags & 0x20`; `DrawChar` centers using same byte |
| Not a separate Ghidra field | Overlaps `glyphTable[87].width` (`0x60 + 87×5 + 2`) |

## 4. Ghidra deltas

- `set_decompiler_comment@0x004372c5` — fontFooter stream 8 B; dword0/1 = defaultWidth/lineHeight; not copied to `+0x560`/`+0x564`
- `set_decompiler_comment@0x0043733a` — `justifyWidth` @ `+0x215`
- **No** `modify_struct_field` split at `+0x50c` — single 8-byte stream block; no instruction-level dword consumers; overlaps `pGlyphTable[256]`
- `save_program bulanci.exe`

## 5. Struct doc updates

| File | Change |
|------|--------|
| `CDSFont.md` | Named `fontFooter_defaultWidth` / `fontFooter_lineHeight`; documented persist-only vs runtime `+0x560`/`+0x564` |
| `CDSFontGlyphRec.md` | Footer cross-ref unchanged |

## 6. Remaining UNK

- Why `Read` does not propagate `fontFooter` into `nDefaultAdvance` / `nLineSpacing` (possible original-engine oversight, or missing setter path not found in this binary).
- Whether any loaded font relies on non-zero `+0x560`/`+0x564` set outside `CDSFont::Read` (no writers found besides `AllocFactory` zero init).
