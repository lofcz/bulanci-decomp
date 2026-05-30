# Wave 2 — batch 03/15 (BitmapSpecial)

**Scope:** catalog indices 65–69, 90–94 (`0x10041`–`0x10045`, `0x1005a`–`0x1005e`).  
**Artifacts:** `catalog.json` (name / category / notes), `registry.json` (slug / folder / notes). No commit.

## Summary

| Idx | ID | Hex | Name | Category | Registry slug | Primary xrefs |
|-----|-----|-----|------|----------|---------------|---------------|
| 65 | 65601 | 0x10041 | `ui_pageNav_last_normal` | menu/widgets/page_nav | `ui_pageNav_last_normal` | `.rdata` 0x482b04, 0x48336c → CHelpDlgCtor @00422041, CHistoryDlg_ctor @004233d1 |
| 66 | 65602 | 0x10042 | `ui_pageNav_last_pressed` | menu/widgets/page_nav | `ui_pageNav_last_pressed` | +8 B triplet from 0x10041; same readers |
| 67 | 65603 | 0x10043 | `ui_menu_history_normal` | menu/main | `ui_menu_history_normal` | DAT_004af914; CMenu_ctor_with_ui READ @004268e3; CBulanci_OnCreate preload |
| 68 | 65604 | 0x10044 | `ui_menu_history_highlight` | menu/main | `ui_menu_history_highlight` | DAT_004af918; CMenu_ctor_with_ui @00426963 |
| 69 | 65605 | 0x10045 | `ui_history_movie_still_1986` | menu/history | `ui_history_movie_still_1986` | overlay HistoryScript 65844 HCreateMovie @0x06b7 (350,90); pairs MPX 77824/77825 |
| 90 | 65626 | 0x1005a | `level_steel_works_smokestack` | levels/steel_works | `level_steel_works_smokestack` | script 65857 InsertView (736,323); 64×192 |
| 91 | 65627 | 0x1005b | `level_malaria_bush` | levels/malaria | `level_malaria_bush` | script 65858 InsertView (432,61); 310×228 |
| 92 | 65628 | 0x1005c | `level_malaria_bg` | levels/malaria | `level_malaria_bg` | script 65858 InsertView (0,0); 800×515 |
| 93 | 65629 | 0x1005d | `level_8bit_wire_pyramid` | levels/8bit | `level_8bit_wire_pyramid` | script 65859 SetOrderAxis z=-67 @ (300,106) |
| 94 | 65630 | 0x1005e | `level_8bit_wire_glyph_sm` | levels/8bit | `level_8bit_wire_glyph_sm` | script 65859 SetOrderAxis z=-30 @ (133,55); 36×68 |

## Ghidra MCP xref verification

| Address | MCP `get_xrefs_to` | Catalog readers |
|---------|-------------------|-------------------|
| `0x00482b04` (65601) | CHelpDlgCtor @00422041 [DATA] | CHistoryDlg_ctor @004233d1, CHelpDlgCtor @00422041 |
| `0x004af914` (65603) | CMenu_ctor_with_ui @004268e3 [READ] | same + CBulanci_OnCreate via preload block |
| `0x004af918` (65604) | *(not re-queried; cached)* | CMenu_ctor_with_ui @00426963 |
| 65605 / 65626–65630 | No `.text` / `.data` immediates | Overlay script VM only (InsertView / HCreateMovie) |

`FUN_00421e40` in cached xrefs updated to **CHelpDlgCtor** where touched (Ghidra current name @00421e40).

## Notes

- **65605:** Timeline row `12. 7. 1986` in `res_0000065844_2050_HistoryScript.script.asm` immediately precedes the HCreateMovie call; distinct from `0x10036` (1995 archaeology still).
- **65626–65630:** Names/categories from prior agent_09 pass; this batch added missing **registry** keys `0x1005a`–`0x1005e` and filled **category/notes** on UI entries 65601–65605 in both `entries` and `byClass.BitmapSpecial`.
- **Open:** 65626 smokestack — pipe vs. stack semantics (visual); 65630 small wire glyph gameplay role.

## Files changed

- `ghidra_analysis/asset_catalog/catalog.json`
- `ghidra_analysis/asset_catalog/registry.json`
- `ghidra_analysis/asset_catalog/naming/wave2_batch_03.md` (this report)
