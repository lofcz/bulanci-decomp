# Asset naming — Wave 2 batch 9/15

**Scope:** BitmapSprite indices 229, 270–278 (`unnamed_batches.json` batch 9).

**Verification:** Script 65856 `@0x0254` (butterfly triple); `CWeapon::ctor` @ `0x0041dbc0` decompile + `gAWeaponDefsTable` struct-table xrefs @ `0x00482820`..`0x00482864`; sprite headers 561×561 / 397×397 dims.

| Idx | ID | Hex | Name | Category | Key xrefs | Ghidra labels | Notes |
|-----|-----|-----|------|----------|-----------|---------------|-------|
| 229 | 65765 | 0x000100e5 | `level_bedtime_butterfly_fly_b` | levels/bedtime_story/decor | script 65856 `@0x0254` frames `[65764,65765,65766]` | — (script VM only) | Corrected mis-name `level_bedtime_lamp_flicker_b`; clip B of 3-part fly loop, 31f 561×561 |
| 270 | 65806 | 0x0001010e | `weapon_pistol_pickup_anim_dir0` | combat/weapon | `gAWeaponDefsTable[0]+0x00` @ `0x00482820`; `CBulanci_OnCreate` copy @ `0x004ae114` | `g_weapon_def_pistol_anim_dir0` | Pistol kind 0 pickup widget track 0 |
| 271 | 65807 | 0x0001010f | `weapon_pistol_pickup_anim_dir1` | combat/weapon | row0 `+0x04` @ `0x00482824` | `g_weapon_def_pistol_anim_dir1` | dir1 |
| 272 | 65808 | 0x00010110 | `weapon_pistol_pickup_anim_dir2` | combat/weapon | row0 `+0x08` @ `0x00482828` | `g_weapon_def_pistol_anim_dir2` | dir2; dir3 is `0x1010d` (batch 8 neighbor) |
| 273 | 65809 | 0x00010111 | `weapon_special_pickup_anim_dir3` | combat/weapon | row1 `+0x0c` @ `0x00482848` | `g_weapon_def_special_anim_dir3` | Special weapon kind 1 |
| 274 | 65810 | 0x00010112 | `weapon_special_pickup_anim_dir0` | combat/weapon | row1 `+0x00` @ `0x0048283c` | `g_weapon_def_special_anim_dir0` | dir0 |
| 275 | 65811 | 0x00010113 | `weapon_special_pickup_anim_dir1` | combat/weapon | row1 `+0x04` @ `0x00482840` | `g_weapon_def_special_anim_dir1` | dir1 |
| 276 | 65812 | 0x00010114 | `weapon_special_pickup_anim_dir2` | combat/weapon | row1 `+0x08` @ `0x00482844` | `g_weapon_def_special_anim_dir2` | dir2 |
| 277 | 65813 | 0x00010115 | `weapon_mine_pickup_anim_dir3` | combat/weapon | row2 `+0x0c` @ `0x00482864` | `g_weapon_def_mine_anim_dir3` | Mine kind 2 |
| 278 | 65814 | 0x00010116 | `weapon_mine_pickup_anim_dir0` | combat/weapon | row2 `+0x00` @ `0x00482858` | `g_weapon_def_mine_anim_dir0` | dir0; dir1–2 are `0x10117`–`0x10118` (batch 10) |

**Registry:** added `0x000100e5`, `0x0001010e`..`0x00010116` slugs. **Ghidra:** labels on weapon-table DWORDs; `save_program` after batch.
