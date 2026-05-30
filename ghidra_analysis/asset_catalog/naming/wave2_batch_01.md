# Asset naming — Wave 2, Batch 1/15 (indices 15–19, 40–44)

**Scope:** master-pack overlay resources `65551`–`65555`, `65576`–`65580`.

**Verification:** Ghidra MCP `search_instructions` + decompile; script `res_0000065860_2026_Script.script.asm` for Exitus props; poem `.txt` titles for CPoem slugs.

| Idx | ID | Hex | Proposed name | Category | Key xrefs | Ghidra labels | Open questions |
|-----|-----|-----|---------------|----------|-----------|---------------|----------------|
| 15 | 65551 | 0x1000f | `poem_do_boje` | menu/credits | CPoemScroller::Constructor @00426453 (class 0x7fb pool) | PRE_COMMENT @00426453 lists 0x1000f..0x10011 | Poem order in scroller vs. pack index |
| 16 | 65552 | 0x10010 | `poem_divne_zvuky` | menu/credits | same enumeration | PRE_COMMENT @00426453 lists 0x10010 | — |
| 17 | 65553 | 0x10011 | `poem_zpoved` | menu/credits | same enumeration | same | — |
| 18 | 65554 | 0x10012 | `pack_dsm_inner_bulanci_tmp` | pack/stub | none (overlay-only) | — | Whether stub is ever invoked at install time |
| 19 | 65555 | 0x10013 | `ui_post_match_pillar_bitmap` | ui/lobby | PUSH 0x10013 @0041005e `CColorSet_ctor_slotPillar`; cache +0x10 @004ae018 | EOL @0041005e; label `g_k_bulanci_bitmap_cache_ids_slot4_ui_post_match_pillar`; decomp PRE_COMMENT | Exact lobby screen (post-match vs. team pick) |
| 40 | 65576 | 0x10028 | `exitus_scenery_grave_double_bg` | levels/exitus/scenery | script 65860 @0x03a4 InsertView axis -20 | script.asm inline comments | — |
| 41 | 65577 | 0x10029 | `exitus_scenery_grave_double_fg` | levels/exitus/scenery | script 65860 @0x038e InsertView (82,237) | same | — |
| 42 | 65578 | 0x1002a | `exitus_scenery_grave_single_bg` | levels/exitus/scenery | script 65860 @0x041d SetOrderAxis -25 | same | — |
| 43 | 65579 | 0x1002b | `exitus_scenery_grave_bulanek_fg` | levels/exitus/scenery | script 65860 @0x0407 InsertView (294,361) | same | — |
| 44 | 65580 | 0x1002c | `exitus_scenery_grave_cross_tilted` | levels/exitus/scenery | script 65860 @0x032b InsertView (664,113) | same | Pairs with 0x10027 parallax at same anchor |

**Poems:** Titles from unpacked `.txt` — *Do boje*, *Divné zvuky*, *Zpověď*. No `PUSH` immediate for poem IDs; `CPoemScroller` walks the resource pool for `classId == 0x7fb`.

**65555:** Only exe hit is `CMenu::CColorSet_ctor_slotPillar` loading `0x10013` for tinted pillar chrome; also prefetched via `g_k_bulanci_bitmap_cache_ids` slot 4.

**Exitus graves:** Script-bytecode only (`65860`); parallax pairs: 65576/65577 @ (82,237), 65578/65579 @ (294,361); 65580 solo @ (664,113) with `0x10027` back-layer.

**Artifacts:** `catalog.json` + `registry.json` updated for these IDs; Ghidra `save_program` on `bulanci.exe`.
