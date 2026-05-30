# Naming wave 2 — remaining 16 assets

**Date:** 2026-05-30  
**Scope:** Final unnamed gallery tiles after wave 2 batches 00–14 (indices 300–306, 307–309, 319–324).

**Verification:** `python ghidra_analysis/asset_catalog/scripts/list_unnamed_assets.py` → **0 unnamed**.

## Step 1 — Exact IDs

| Idx | ID | Hex | Class | Path |
|-----|-----|-----|-------|------|
| 300 | 65836 | 0x0001012c | BitmapSprite | `res_0000065836_52_BitmapSprite.bin` |
| 301 | 65837 | 0x0001012d | BitmapSprite | `res_0000065837_52_BitmapSprite.bin` |
| 302 | 65838 | 0x0001012e | BitmapSprite | `res_0000065838_52_BitmapSprite.bin` |
| 303 | 65839 | 0x0001012f | BitmapSprite | `res_0000065839_52_BitmapSprite.bin` |
| 304 | 65840 | 0x00010130 | BitmapSprite | `res_0000065840_52_BitmapSprite.bin` |
| 305 | 65841 | 0x00010131 | BitmapSprite | `res_0000065841_52_BitmapSprite.bin` |
| 306 | 65842 | 0x00010132 | BitmapSprite | `res_0000065842_52_BitmapSprite.bin` |
| 307 | 65843 | 0x00010133 | HistoryScript | `res_0000065843_2050_HistoryScript.bin` |
| 308 | 65844 | 0x00010134 | HistoryScript | `res_0000065844_2050_HistoryScript.bin` |
| 309 | 65845 | 0x00010135 | HistoryScript | `res_0000065845_2050_HistoryScript.bin` |
| 319 | 65855 | 0x0001013f | Script | `res_0000065855_2026_Script.bin` |
| 320 | 65856 | 0x00010140 | Script | `res_0000065856_2026_Script.bin` |
| 321 | 65857 | 0x00010141 | Script | `res_0000065857_2026_Script.bin` |
| 322 | 65858 | 0x00010142 | Script | `res_0000065858_2026_Script.bin` |
| 323 | 65859 | 0x00010143 | Script | `res_0000065859_2026_Script.bin` |
| 324 | 65860 | 0x00010144 | Script | `res_0000065860_2026_Script.bin` |

## Step 2 — Names and evidence

| ID | Hex | Name | Category | Evidence |
|-----|-----|------|----------|----------|
| 65836 | 0x0001012c | `bulanek_walk_anim_slot5` | gameplay/characters/bulanek | `gABulanekWalkAnimSheet[5]` @ `0x004ae144`; `CBulanci_OnCreate` → `CBulanek_ResolveAndBindAnimTrack`; 87×63 · 33 fr; menu cache idx 79 |
| 65837 | 0x0001012d | `bulanek_walk_anim_slot3` | gameplay/characters/bulanek | `gABulanekWalkAnimSheet[11]` @ `0x004ae14c`; 55×63 · 36 fr |
| 65838 | 0x0001012e | `bulanek_walk_anim_slot2` | gameplay/characters/bulanek | `gABulanekWalkAnimSheet[10]` @ `0x004ae148`; 55×62 · 33 fr |
| 65839 | 0x0001012f | `ui_scoreboard_backdrop` | ui/scoreboard | `CScoreCtor` PUSH `0x1012f` @ `0x00411a63`; cache idx 109; 276×120 · 33 fr |
| 65840 | 0x00010130 | `teleport_point_sparkle_anim` | gameplay/entities | `CTeleportPoint_Ctor` PUSH `0x10130` @ `0x0041ca36`; 78×61 · 8 fr |
| 65841 | 0x00010131 | `exit_dlg_bulanek_anim_left` | menu/dialogs | `CExitDlg_ctor` PUSH @ `0x00411c0e`; CAnim @ (194, 22); 122×13 · 63 fr |
| 65842 | 0x00010132 | `exit_dlg_bulanek_anim_right` | menu/dialogs | `CExitDlg_ctor` PUSH @ `0x00411c8a`; CAnim @ (313, 282); 610×610 · 37 fr |
| 65843 | 0x00010133 | `history_part_1` | ui/history | `CHistoryDlg_ctor` CMP `[obj+0xc], 0x802` @ `0x0042329e`; script intro timeline through 1980 |
| 65844 | 0x00010134 | `history_part_2` | ui/history | Same class enumeration; HCreateMovie clips 65652/65605; pairs anim 77825–77827 |
| 65845 | 0x00010135 | `history_part_3` | ui/history | Same class enumeration; page 3 prose 1993–1998; HCreateMovie 65613/65590; pairs anim 77829–77831 |
| 65855 | 0x0001013f | `level_vybijena` | levels/vybijena | GetInfo `"Vybíjená"` / `"Dj Slaughter"`; `CLevelScript` enum CMP `0x7ea` @ `0x00409fb7`; LoadPreface 65847 + JPEG 65846 |
| 65856 | 0x00010140 | `level_bedtime_story` | levels/bedtime_story | GetInfo `"Na dobrou noc"`; SetMusic(65868, 65537); audio_banks.md row |
| 65857 | 0x00010141 | `level_steel_works` | levels/steel_works | GetInfo `"Noční směna"`; SetMusic(65863, 65538) |
| 65858 | 0x00010142 | `level_malaria` | levels/malaria | GetInfo `"Malárie"`; GUID A0A144D0-26B0-4049-A66F-7AE630965103 |
| 65859 | 0x00010143 | `level_8bit_captivity` | levels/8bit | GetInfo `"V zajetí 8-bitu"`; SetMusic(65864, 65536); hazard anims 0x100fe..0x10108 |
| 65860 | 0x00010144 | `level_exitus` | levels/exitus | GetInfo `"Exitus"`; **CMP `0x10144`** @ `0x0041c24b` in `CGaming_OnResumeOrStartGame` (7 s timer branch) |

## Catalog / registry / Ghidra

- **`catalog.json`:** `name`, `category`, `notes` on all 16 IDs (`entries` + `byClass` mirrors).
- **`registry.json`:** Added `assets` slugs `0x0001012c`–`0x00010135`, `0x0001013f`–`0x00010144` (scripts section slugs were already present from batch 12).
- **Ghidra (`bulanci.exe`):** EOL comments on PUSH/CMP sites above; plate comments on walk-sheet globals @ `0x004ae144`/`148`/`14c`; class-enumeration comments @ `0x0042329e`, `0x00409fb7`; **program saved**.

## Prior batch notes

Detailed xref tables live in [wave2_batch_11.md](./wave2_batch_11.md) (65836–65844) and [wave2_batch_12.md](./wave2_batch_12.md) (65845–65860). This file records the final apply + verification pass.
