# Asset naming — Wave 2 batch 14/15 (final, 9 assets)

**Batch:** indices 337–346 · IDs `65873`, `77824`–`77831` · hex `0x00010151`, `0x00013000`–`0x00013007`

**Verification:** Ghidra MCP `search_instructions` (PUSH `0x10003` @ `00420180` in `CGaming_ctor` → bank index `65539` / PCM `65873`); overlay `HistoryScript` `HCreateMovie` disasm; DSM dimensions in `formats/dsm_file_format.md`. No `.text` immediates for overlay MPX IDs (script VM only).

| Idx | ID | Hex | Class | Proposed name | Category | Key xrefs | Ghidra / labels | Open questions |
|-----|-----|-----|-------|---------------|----------|-----------|-----------------|----------------|
| 337 | 65873 | 0x00010151 | AudioBank | `sfx_bank_gameplay_default_pcm` | audio/sfx | `CGaming_ctor` PUSH `0x10003` @ `00420180`; partner of index `65539` (`sfx_bank_gameplay_default`) | 1 insn hit (synthetic audio-bank-mirror); `audio_banks.md` §5.1 | Per-slot sample slugs (8 WAVs) TBD in registry |
| 339 | 77824 | 0x00013000 | BitmapSpecial | `history_mpx_olympics_1988_still` | ui/history | `HistoryScript` 65844 `@0x06b7` `HCreateMovie(350,90,65605,77824,77825)` | Overlay-only; `bulanci.ghidra.exe.c` MPX comment @84854 | Movie still id `65605` slug says 1986 but prose below is 24.8.1988 Olympics |
| 340 | 77825 | 0x00013001 | BitmapJpegAnim | `history_mpx_olympics_1988_anim` | ui/history | Same `@0x06b7`; DSM 166×154 · 180 fr · 22.05 kHz | — | — |
| 341 | 77826 | 0x00013002 | BitmapSpecial | `history_mpx_pillow_war_torch_still` | ui/history | `HistoryScript` 65844 `@0x069d` `HCreateMovie(30,266,65652,77826,77827)` | Pairs `history_movie_torch_cauldron` (65652) | — |
| 342 | 77827 | 0x00013003 | BitmapJpegAnim | `history_mpx_pillow_war_torch_anim` | ui/history | Same `@0x069d`; DSM 166×153 · 149 fr · 44.1 kHz | — | — |
| 343 | 77828 | 0x00013004 | BitmapSpecial | `history_mpx_riot_anniversary_1995_still` | ui/history | `HistoryScript` 65845 `@0x06c9` `HCreateMovie(350,90,65613,77828,77829)` | Pairs `history_p3_cinematic_still` (65613) | — |
| 344 | 77829 | 0x00013005 | BitmapJpegAnim | `history_mpx_riot_anniversary_1995_anim` | ui/history | Same `@0x06c9`; DSM 166×154 · 287 fr · 22.05 kHz | — | — |
| 345 | 77830 | 0x00013006 | BitmapSpecial | `history_mpx_archeology_1995_still` | ui/history | `HistoryScript` 65845 `@0x06af` `HCreateMovie(30,266,65590,77830,77831)` | Pairs `ui_history_movie_archeology_1995` (65590) | — |
| 346 | 77831 | 0x00013007 | BitmapJpegAnim | `history_mpx_archeology_1995_anim` | ui/history | Same `@0x06af`; DSM 166×153 · 396 fr · 22.05 kHz | — | — |

**Pairs (HCreateMovie still + MPX anim):**

| Still | Anim | History page | Anchor text |
|-------|------|--------------|-------------|
| 77826 | 77827 | 65844 p.2 | Druhá polštářová válka (1986) |
| 77824 | 77825 | 65844 p.2 | Olympijské hry / Mazlík gold (24.8.1988) |
| 77830 | 77831 | 65845 p.3 | Archeologický nález (23.2.1995) |
| 77828 | 77829 | 65845 p.3 | Výročí vzpoury (5.5.1995) |

**Updates:** `catalog.json` (`name`, `category`, `notes` on `entries` + `byClass`), `registry.json` slugs under `0x00010151` / `0x00013000`–`0x00013007`. No git commit.
