# Round 4 — Task 34 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 34 |
| **round** | 4 |
| **title** | Trace CDSFont fontFooter to nDefaultAdvance/nLineSpacing copy path |
| **one_liner** | Prove or disprove any instruction path that copies persisted `fontFooter` dwords (`+0x50c`/`+0x510`) into runtime `defaultAdvance` / `lineSpacing` (`+0x560`/`+0x564`). |
| **prior** | [round3_task_34_report.md](./round3_task_34_report.md) |
| **structs** | CDSFont, CDSFontGlyphRec, CDSImage |

## Status

**DONE (negative result)** — Full-program `search_instructions` shows **no copy path**. `CDSFont::Read` loads footer then glyph blob only; `+0x560`/`+0x564` are zero-initialized in `CDSFont_AllocFactory` and never written elsewhere.

## Evidence

### `CDSFont::Read` stream sequence (no propagation)

| Step | Address | Action |
|------|---------|--------|
| 1 | `0x004372b9` | `CDSImage_Load` |
| 2 | `0x004372c5` | `LEA ECX,[EDI+0x50c]`; stream `Read` 8 B → `fontFooter` |
| 3 | `0x004372da` | `ADD EDI,0xc`; stream `Read` `0x500` B → embedded payload |
| — | — | **No** `MOV`/`store` to `[EDI+0x560]` or `[EDI+0x564]` before return |

Decompile confirms only two virtual `Read` calls after `CDSImage_Load`; no arithmetic bridging `+0x50c` → `+0x560`.

### Instruction xref census (entire program)

| Operand | Writers | Readers (non-CRT) |
|---------|---------|-------------------|
| `+0x50c` | — (stream `LEA` in `Read`/`ReadNoAlloc` only) | — |
| `+0x510` | — | — (zero hits) |
| `+0x560` | `CDSFont_AllocFactory@0x0043759d` `MOV [ESI+0x560],0` | `CDSFont::GetCharWidth@0x0043735d` |
| `+0x564` | `CDSFont_AllocFactory@0x004375a7` `MOV [ESI+0x564],0` | `TextShaper_LayOutAndRender@0x0043776c`, `0x00437a2f`; `CEdit_BuildAt@0x0040789e` |

No non-zero stores to `+0x560` or `+0x564` exist in `bulanci.exe`.

### Runtime consumers (still valid when fields stay zero)

| Function | Offset | Behavior when zero |
|----------|--------|-------------------|
| `CDSFont::GetCharWidth` | `+0x560` | Zero-width glyph slots return `(0+3)>>2 == 0` unless `justifyWidth` path (`+0x215`, flags `0x20`) |
| `TextShaper_LayOutAndRender` | `+0x564` | Vertical advance per line uses `font+0x564 + font_info[+8]`; with both zero, lines stack using `\n` breaks only |
| `CEdit::BuildAt` | `+0x564` | Caret bottom `lineSpacing+2`; window height uses fixed `+0x17` at `+0x2c`, not footer `lineHeight` |

### File vs instance (unpacker cross-check)

`bulanci_unpack._save_font` maps stream bytes after image to JSON `defaultWidth` / `lineHeight` at file offset `image_size` — same 8 B as `fontFooter`. Gallery preview scales `defaultWidth/2`, matching the `>> 2` applied to `+0x560` **if** that field were populated; binary never performs that copy.

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0043759d` | AllocFactory zero `defaultAdvance`; no footer copy |
| `set_decompiler_comment` | `0x004375a7` | AllocFactory zero `lineSpacing`; no footer copy |
| `set_decompiler_comment` | `0x0043735d` | GetCharWidth `>>2` fallback vs persist-only `+0x50c` |
| `search_instructions` | `0x560` / `0x564` / `0x50c` / `0x510` | Census above |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CDSFont.md](./CDSFont.md) — R4 § “footer → runtime metrics” closed as **no copy path**.

## Remaining UNK

- Whether shipped fonts ever relied on non-zero `+0x560`/`+0x564` set by a **different binary** or tool chain (not present here).
- Whether `defaultWidth` in files is only for editors/unpacker and intentionally unused at runtime (consistent with zero runtime fields + per-glyph width bytes).
