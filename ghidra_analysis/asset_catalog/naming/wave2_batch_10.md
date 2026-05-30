# Asset naming — Wave 2 batch 10/15 (indices 279–288)

**Scope:** BitmapSprite only — `65815`/`0x10117` … `65824`/`0x10120` (`res_0000065815_52` … `res_0000065824_52`).

**Verification:** `catalog.json` struct-table xrefs in `CWeapon::ctor` (`0x0041dbc0`); Ghidra MCP `get_bulk_xrefs` / `get_xrefs_to` on `.rdata` slots (`0x0048285c` … `0x00482898`) returned **no direct code refs** (IDs consumed via `gAWeaponDefsTable` static init, same as pistol/special rows in batch 9).

| Idx | ID | Hex | Proposed name | Category | Key xrefs | Registry slug | Open questions |
|-----|-----|-----|---------------|----------|-----------|---------------|----------------|
| 279 | 65815 | 0x10117 | `weapon_mine_pickup_anim_dir1` | combat/weapon | `CWeapon::ctor` row2 `+0x04` @ `0x0048285c` | `weapon_mine_pickup_anim_dir1` | — |
| 280 | 65816 | 0x10118 | `weapon_mine_pickup_anim_dir2` | combat/weapon | row2 `+0x08` @ `0x00482860` | `weapon_mine_pickup_anim_dir2` | Renamed from `*_widget_*` (batch 10) |
| 281 | 65817 | 0x10119 | `weapon_detonator_pickup_anim_dir3` | combat/weapon | row3 `+0x0c` @ `0x00482880` | `weapon_detonator_pickup_anim_dir3` | Remote-detonator pickup kind 3 |
| 282 | 65818 | 0x1011a | `weapon_detonator_pickup_anim_dir0` | combat/weapon | row3 `+0x00` @ `0x00482874` | `weapon_detonator_pickup_anim_dir0` | — |
| 283 | 65819 | 0x1011b | `weapon_detonator_pickup_anim_dir1` | combat/weapon | row3 `+0x04` @ `0x00482878` | `weapon_detonator_pickup_anim_dir1` | — |
| 284 | 65820 | 0x1011c | `weapon_detonator_pickup_anim_dir2` | combat/weapon | row3 `+0x08` @ `0x0048287c` | `weapon_detonator_pickup_anim_dir2` | — |
| 285 | 65821 | 0x1011d | `weapon_mg_pickup_anim_dir3` | combat/weapon | row4 `+0x0c` @ `0x0048289c` | `weapon_mg_pickup_anim_dir3` | Machine-gun pickup kind 4 |
| 286 | 65822 | 0x1011e | `weapon_mg_pickup_anim_dir0` | combat/weapon | row4 `+0x00` @ `0x00482890` | `weapon_mg_pickup_anim_dir0` | — |
| 287 | 65823 | 0x1011f | `weapon_mg_pickup_anim_dir1` | combat/weapon | row4 `+0x04` @ `0x00482894` | `weapon_mg_pickup_anim_dir1` | — |
| 288 | 65824 | 0x10120 | `weapon_mg_pickup_anim_dir2` | combat/weapon | row4 `+0x08` @ `0x00482898` | `weapon_mg_pickup_anim_dir2` | — |

## `gAWeaponDefsTable` animFrameIds (this batch)

| Row | Pickup kind | dir0 (+0x00) | dir1 (+0x04) | dir2 (+0x08) | dir3 (+0x0c) |
|-----|-------------|--------------|--------------|--------------|--------------|
| 2 | mine (2) | 65814 (batch 9) | **65815** | **65816** | 65813 (batch 9) |
| 3 | detonator (3) | **65818** | **65819** | **65820** | **65817** |
| 4 | machine gun (4) | **65822** | **65823** | **65824** | **65821** |

HUD / still icons on same rows (not in this batch): row2 `0x100aa`/`0x100a3` (65706/65699); row3 `0x100ab` (65707 @ `+0x10`); row4 `0x100a2`/`0x100a8` (65698/65704).

## Files touched

- `ghidra_analysis/asset_catalog/catalog.json` — `name`, `category`, `notes` (entries + `byClass.BitmapSprite`)
- `ghidra_analysis/asset_catalog/registry.json` — new `assets` keys `0x10117`–`0x10120`
- `ghidra_analysis/asset_catalog/naming/unnamed_batches.json` — slugs cleared for indices 279–288

**Note:** `catalog.json` `entries` currently lists some weapon BitmapSprite IDs twice (e.g. `65815`); dedupe is out of scope for this batch.

**Ghidra:** No `save_program` required (labels already on struct-table slots from prior weapon-config work).
