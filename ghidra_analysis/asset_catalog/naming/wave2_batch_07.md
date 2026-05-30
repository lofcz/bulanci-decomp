# Asset naming — Wave 2 batch 7/15

**Scope:** BitmapSpecial index 149 + BitmapSprite indices 200–208 (`unnamed_batches.json` batch 7): `0x10095`, `0x100c8`..`0x100d0`.

**Verification:** Ghidra MCP xref trace + `CScoreCtor` / `CBulanekCtor` / `CBulanek_ResolveAndBindAnimTrack` decompile; sprite atlas 8-frame walk (104×55) for `0x100c8`.

| Idx | ID | Hex | Name | Category | Key xrefs | Ghidra labels | Notes |
|-----|-----|-----|------|----------|-----------|---------------|-------|
| 149 | 65685 | 0x00010095 | `ui_scoreboard_background` | ui/score | `CScoreCtor` PUSH @ `0x00411825`; `CBulanci_OnCreate` READ `g_dwResId_ui_scoreboard_background` @ `0x004ae0cc` | `g_dwResId_ui_scoreboard_background` | BitmapSpecial 800×600; single-player high-score panel only |
| 200 | 65736 | 0x000100c8 | `player_walk_normal_left` | gameplay/character | `gABulanekWalkAnimsNormal[1]` @ `0x004af2f0`; `gAPlayerSkinPaletteIds[2]` @ `0x004ae0f4`; `CBulanekCtor` loop @ `0x0041e723` | `g_dwResId_player_walk_normal_left`, `g_dwResId_player_skin_palette_02` | 8-frame left walk |
| 201 | 65737 | 0x000100c9 | `player_walk_special_left` | gameplay/character | `gABulanekWalkAnimsSpecial[1]` @ `0x004af2e0`; tournament CBulanek branch | `g_dwResId_player_walk_special_left` | Special-variant left |
| 202 | 65738 | 0x000100ca | `player_walk_normal_right` | gameplay/character | `gABulanekWalkAnimsNormal[2]` @ `0x004af2f4`; palette @ `0x004ae0f8` | `g_dwResId_player_walk_normal_right`, `g_dwResId_player_skin_palette_03` | Right walk |
| 203 | 65739 | 0x000100cb | `player_walk_special_right` | gameplay/character | `gABulanekWalkAnimsSpecial[2]` @ `0x004af2e4` | `g_dwResId_player_walk_special_right` | Special-variant right |
| 204 | 65740 | 0x000100cc | `player_sit_facing_up` | gameplay/character | `gABulanekSitAnimSheet[3]` @ `0x004af004` | `g_dwResId_player_sit_facing_up` | 133px tall sit-up pose |
| 205 | 65741 | 0x000100cd | `player_enemy_sit_facing_up` | gameplay/character | `gABulanekEnemySitAnimSheet[3]` @ `0x004af014` | `g_dwResId_player_enemy_sit_facing_up` | Enemy types `0x20`–`0x23` |
| 206 | 65742 | 0x000100ce | `player_sit_facing_down` | gameplay/character | `gABulanekSitAnimSheet[0]` @ `0x004aeff8`; READ @ `0x0041c654` | `g_dwResId_player_sit_facing_down` | Sit bind track index 0 |
| 207 | 65743 | 0x000100cf | `player_enemy_sit_facing_down` | gameplay/character | `gABulanekEnemySitAnimSheet[0]` @ `0x004af008`; READ @ `0x0041c663` | `g_dwResId_player_enemy_sit_facing_down` | Enemy sit down |
| 208 | 65744 | 0x000100d0 | `player_sit_facing_left` | gameplay/character | `gABulanekSitAnimSheet[1]` @ `0x004aeffc` | `g_dwResId_player_sit_facing_left` | Sit left |

**Sit sheet order:** down `0x100ce`, left `0x100d0`, (right `0x100d2` batch 10 neighbor), up `0x100cc`.

**Registry:** added `0x00010095`, `0x000100c8`..`0x000100d0`. **Ghidra:** `set_global` on all DWORD slots; `save_program` after batch.
