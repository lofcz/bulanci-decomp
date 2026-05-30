# Asset naming — Agent 2/10 (bedtime level scenery)

**Scope:** BitmapSpecial scenery for Na dobrou noc / Bedtime story — `0x10046`–`0x10049`, `0x10053`, `0x1006a`–`0x1006b` (ids 65606–65609, 65619, 65642–65643).

**Verification:** PNG multimodal review + script 65856 disasm xrefs (`InsertView` / `CreateAnim` pairings). No `bulanci.exe` .text immediates (overlay-script only).

## Per-asset summary

| ID | Hex | Old slug | New slug | Category | Script placement | PNG |
|----|-----|----------|----------|----------|------------------|-----|
| 65606 | 0x10046 | `level_scenery_panel_sm` | `level_bedtime_rock_cluster` | levels/bedtime_story/scenery | fn@0x394 `@0x0485` InsertView z=-25; bird-nest CreateAnim slot 9 | Gray rocks + grass (68×61) |
| 65607 | 0x10047 | `level_scenery_panel_md` | `level_bedtime_berry_bush` | levels/bedtime_story/scenery | fn@0x4e1 `@0x0575` z=-25; butterfly CreateAnim slot 4 @ (+82,+1) | Large berry bush (148×116) |
| 65608 | 0x10048 | `level_scenery_platform` | `level_bedtime_berry_bush_sm` | levels/bedtime_story/scenery | fn@0x5c9 `@0x05e5` SetObstacleBounds z=-40 | Small berry bush (101×76) |
| 65609 | 0x10049 | `level_scenery_ground` | `level_bedtime_fallen_log` | levels/bedtime_story/scenery | fn@0x286 `@0x0350`; chimney-smoke CreateAnim slot 6 @ (+49,-15) | Fallen log on grass (135×114) |
| 65619 | 0x10053 | *(ok)* `level_bedtime_bg_fullscreen` | — | levels/bedtime_story | OnInit `@0x00e9` InsertView (0,0) | Meadow grass field 800×515 |
| 65642 | 0x1006a | *(ok)* `level_bedtime_mushroom_house` | — | levels/bedtime_story | fn@0x01e7 `@0x020a` z=-39 + bed bounds; butterfly slot 8 | Red-spot mushroom house (220×226) |
| 65643 | 0x1006b | `level_bedtime_grass_edge` | `level_bedtime_roof_eave` | levels/bedtime_story | fn@0x01e7 `@0x01ee` at (+8,+141) vs house anchor | Mossy roof eave (113×49) — **not grass** |

## Script 65856 helper map

| Helper | Role | Scenery IDs | CreateAnim |
|--------|------|-------------|------------|
| OnInit `@0x00e9` | Full-screen backdrop | 65619 | — |
| fn@0x01e7 | Mushroom house cluster | 65643, 65642 | 65764–65766 butterfly slot 8 |
| fn@0x286 | Log + obstacles | 65609 | 65759–65763 chimney smoke slot 6 |
| fn@0x394 | Rock + danger zones | 65606 | 65767–65774 bird nest slot 9 |
| fn@0x4e1 | Large bush | 65607 | 65775–65779 butterfly slot 4 |
| fn@0x5c9 | Small bush platform | 65608 | — |

## Corrections applied

- Generic `level_scenery_*` slugs → bedtime-specific names (only consumer is script 65856).
- `level_bedtime_grass_edge` → `level_bedtime_roof_eave` (PNG shows diagonal mossy wood beam, not grass).
- Folder `gameplay/level_scenery` → `levels/bedtime_story/scenery` for 65606–65609.
- Added `levels/bedtime_story/scenery` registry category.

## Open questions

- None blocking.
