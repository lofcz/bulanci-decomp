# Asset verify — Agent 4/10 (`levels/malaria`, script 65858)

**Scope:** bg, bush, preface, panorama, swamp (mid/fore), soldier walk, Bulánek walks, jungle leaves (`0x1005b`–`0x1005c`, `0x10067`–`0x10068`, `0x100f6`–`0x100fd`).

**Verification:** `res_0000065858_2026_Script.script.asm` OnInit xrefs + PNG/atlas previews + `*.atlas.json` frame counts.

## Script 65858 layer order (OnInit)

| Slot | Offset | Asset ID | Hex | Role |
|------|--------|----------|-----|------|
| preface | `0x00b6` | 65640 | `0x10068` | LoadPreface scroll |
| base | `0x00ca` | 65639 | `0x10067` | Ground plane (first image) |
| bush | `0x00e0` | 65627 | `0x1005b` | Jungle bush @ (432,61) |
| 6 | `0x00f0` | 65782 | `0x100f6` | Panorama 83×308×255 @ (492,0) |
| 7 | `0x0108` | 65787 | `0x100fb` | Khaki Bulánek walk 69×202×241 |
| 8 | `0x0120` | 65784 | `0x100f8` | Swamp mid 49×272×152 |
| 9 | `0x0138` | 65785 | `0x100f9` | Swamp fore reeds 49×135×105 |
| 10 | `0x0150` | 65788 | `0x100fc` | Dark-green walk 57×120×99 |
| 11 | `0x0168` | 65789 | `0x100fd` | Jungle leaves 52×66×66 |
| 12 | `0x0180` | 65783 | `0x100f7` | Canopy/treeline 82×124×191 |
| overlay | `0x0232` | 65628 | `0x1005c` | Fore-branch vignette (last image) |
| 13 | `0x0242` | 65786 | `0x100fa` | Soldier 96×280×279 @ (0,236) |

## Fixes applied

| ID | Hex | Was | Now | Reason |
|----|-----|-----|-----|--------|
| 65628 | `0x1005c` | `level_malaria_bg` (duplicate) | **`level_malaria_fore_branches`** | Visual: bare branches on transparent 800×515; drawn **after** all layers, not ground |
| 65639 | `0x10067` | `level_malaria_bg` (duplicate slug) | **`level_malaria_bg`** (kept) | Visual: full grass/swamp floor; first InsertView @ `0x00ca` |
| 65789 | `0x100fd` | `name: null` in `entries` | **`level_malaria_jungle_leaves`** | Sync with registry / byClass |
| 65786–65788 | `0x100fa`–`0x100fc` | `category: level/anim` | **`levels/malaria`** | Level decor anims belong under malaria folder |

## Confirmed correct (no rename)

| Name | ID | Hex | Visual / atlas |
|------|----|-----|----------------|
| `level_malaria_bush` | 65627 | `0x1005b` | Isolated leafy bush 310×228 |
| `level_malaria_preface` | 65640 | `0x10068` | Sepia Bulánek vs snake intro |
| `level_malaria_panorama_anim` | 65782 | `0x100f6` | 83f wide jungle panorama strip |
| `level_malaria_canopy_detail` | 65783 | `0x100f7` | 82f fern/treeline |
| `level_malaria_swamp_mid` | 65784 | `0x100f8` | 49f wide swamp foliage |
| `level_malaria_swamp_fore` | 65785 | `0x100f9` | 49f reed sprigs |
| `level_malaria_soldier_walk_4dir` | 65786 | `0x100fa` | 96f large figure (4-dir walk) |
| `level_malaria_bulanek_walk` | 65787 | `0x100fb` | 69f khaki character |
| `level_malaria_bulanek_swamp_walk` | 65788 | `0x100fc` | 57f smaller green walk |
| `level_malaria_jungle_leaves` | 65789 | `0x100fd` | 52f paired leaves sway |

**Out of scope (not in hex list):** `0x1005a` = steel-works smokestack (script 65857).

**Patched:** `catalog.json` (`entries` + `byClass`), `registry.json` (`0x1005c`, `0x10067`).
