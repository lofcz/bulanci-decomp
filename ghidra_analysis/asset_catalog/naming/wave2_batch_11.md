# Asset naming — Wave 2 batch 11/15

**Scope:** indices 289, 300–308 (`id` 65825, 65836–65844) · hex `0x00010121`, `0x0001012c`–`0x00010134`.

**Verification:** Ghidra MCP `decompile_function` on `CBulanci_OnCreate` @ `0x00402b20`, `CBulanek_ResolveAndBindAnimTrack` @ `0x0041c610`, `CScoreCtor` @ `0x00411010`, `CTeleportPoint_Ctor` @ `0x0041c9a0`, `CExitDlg_ctor` @ `0x00411b50`; `read_memory` on `g_k_bulanci_bitmap_cache_ids` @ `0x004ae008`; atlas dims; overlay `HistoryScript` 65843 intro page.

| Idx | ID | Hex | Name | Category | Key xrefs | Ghidra labels | Notes |
|-----|-----|-----|------|----------|-----------|---------------|-------|
| 289 | 65825 | 0x00010121 | `weapon_rocket_widget_anim_dir3` | gameplay/weapons | `gAWeaponDefsTable[5].+0x0c` @ `0x004828b8` | `g_dwResId_weapon_rocket_widget_anim_dir3` | Rocket pickup widget dir3 (kind 5); dirs 0–2 = `0x10122`–`0x10124` (held/firing, not in-flight). In-flight rockets = `0x10031`–`0x10034`. |
| 300 | 65836 | 0x0001012c | `bulanek_walk_anim_slot5` | gameplay/characters/bulanek | `gABulanekWalkAnimSheet[5]` @ `0x004ae144`; menu cache idx 79 | `g_dwResId_bulanek_walk_anim_slot5` | 87×63 · 33 fr; pairs slot4 @ sheet[4] |
| 301 | 65837 | 0x0001012d | `bulanek_walk_anim_slot3` | gameplay/characters/bulanek | `gABulanekWalkAnimSheet[11]` @ `0x004ae14c` | `g_dwResId_bulanek_walk_anim_slot3` | 55×63 · 36 fr |
| 302 | 65838 | 0x0001012e | `bulanek_walk_anim_slot2` | gameplay/characters/bulanek | `gABulanekWalkAnimSheet[10]` @ `0x004ae148` | `g_dwResId_bulanek_walk_anim_slot2` | 55×62 · 33 fr |
| 303 | 65839 | 0x0001012f | `ui_scoreboard_backdrop` | ui/scoreboard | `CScoreCtor` PUSH `0x1012f` @ `0x00411a63`; cache idx 109 | EOL @ PUSH | 276×120 · 33 fr full-screen scoreboard bitmap |
| 304 | 65840 | 0x00010130 | `teleport_point_sparkle_anim` | gameplay/entities | `CTeleportPoint_Ctor` PUSH `0x10130` @ `0x0041ca36` | EOL @ PUSH | 78×61 · 8 fr portal sparkle |
| 305 | 65841 | 0x00010131 | `exit_dlg_bulanek_anim_left` | menu/dialogs | `CExitDlg_ctor` PUSH @ `0x00411c0e`; CAnim @ (194, 22) | EOL @ PUSH | Quit-confirm left Bulánek (122×13 strip, 63 fr) |
| 306 | 65842 | 0x00010132 | `exit_dlg_bulanek_anim_right` | menu/dialogs | `CExitDlg_ctor` PUSH @ `0x00411c8a`; CAnim @ (313, 282) | EOL @ PUSH | Quit-confirm right Bulánek (610×610, 37 fr) |
| 307 | 65843 | 0x00010133 | `history_part_1` | ui/history | `CHistoryDlg_ctor` class enum `0x802`; script intro timeline | — | Registry `scripts.65843`; Bulánek definition + 1974–1980 dates |
| 308 | 65844 | 0x00010134 | `history_part_2` | ui/history | Same class enumeration | — | Registry `scripts.65844`; continues timeline + HCreateMovie clips |

**Registry:** `assets` entries for `0x00010121`, `0x0001012c`–`0x00010132`; `scripts` slugs already present for 65843–65844. **Ghidra:** `set_global` on weapon-table DWORD + walk-sheet cells; EOL comments on PUSH sites; `save_program`. No git commit.
