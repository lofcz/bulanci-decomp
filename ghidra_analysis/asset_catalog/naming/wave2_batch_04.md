# Wave 2 naming — batch 04/15 (BitmapSpecial)

**Scope:** indices 95–99, 124–128 (`unnamed_batches.json` batch 4).

**Verification:** overlay script 65859 (`res_0000065859_2026_Script`) for 8-bit level assets; Ghidra decompile for HUD/setup (`CSetupDlgCtor`, `CShotCounterCtor`, `CPanelCtor`). Program saved (`bulanci.exe`).

| Idx | ID | Hex | Name | Category | Key xrefs | Ghidra | Registry |
|-----|-----|-----|------|----------|-----------|--------|----------|
| 95 | 65631 | 0x1005f | `level_8bit_wire_prism` | levels/8bit | script 65859 `@0x0169` InsertView (58,250) z=-25 | PRE_COMMENT `CLevelScriptOpExt_CreateImage` | added |
| 96 | 65632 | 0x10060 | `level_8bit_spawn_pool_3` | levels/8bit | script 65859 CollInsert [3] → SpawnOpponentEx | PRE_COMMENT `CLevelScriptOpExt_CollInsert` | added |
| 97 | 65633 | 0x10061 | `level_8bit_spawn_pool_0` | levels/8bit | CollInsert [0] (first pool entry) | same | added |
| 98 | 65634 | 0x10062 | `level_8bit_spawn_pool_1` | levels/8bit | CollInsert [1] | same | added |
| 99 | 65635 | 0x10063 | `level_8bit_spawn_pool_2` | levels/8bit | CollInsert [2] | same | added |
| 124 | 65660 | 0x1007c | `hud_shot_counter_ammo_strip` | gameplay/hud | `g_dwShotCounterIconResIds[0]` @ 0x004af938; `CShotCounterCtor` | PRE_COMMENT @ `CShotCounterCtor` | added |
| 125 | 65661 | 0x1007d | `ui_setup_volume_slider_end_pressed` | ui/setup | `g_volumeSliderBitmapIds[7]` @ 0x004aea50 | label + PRE_COMMENT `CSetupDlgCtor` | added |
| 126 | 65662 | 0x1007e | `ui_setup_volume_slider_end_normal` | ui/setup | `g_volumeSliderBitmapIds[6]` @ 0x004aea4c | label @ 0x004aea4c | added |
| 127 | 65663 | 0x1007f | `ui_setup_volume_slider_end_disabled` | ui/setup | `g_volumeSliderBitmapIds[8]` @ 0x004aea54 | label @ 0x004aea54 | added |
| 128 | 65664 | 0x10080 | `hud_panel_player_bitmap` | gameplay/hud | PUSH 0x10080 @ `CPanelCtor` 0x00428265; menu cache @ 0x004ae080 | PRE_COMMENT @ push site | added |

**CollInsert order (global 12, script 65859):** 65633 → 65634 → 65635 → 65632 (pool indices 0–3).

**Open questions:** spawn-pool thumbs map to which opponent archetypes (visual only in script VM).
