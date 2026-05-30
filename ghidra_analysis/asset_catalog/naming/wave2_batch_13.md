# Asset naming — Wave 2 batch 13/15 (indices 325–328, 331–336)

**Scope:** Level music (`Mp3`) and level SFX PCM banks (`AudioBank`).

**Verification:** overlay script `SetMusic` xrefs + `CGaming_ctor` decomp for 0x1014b. Level MP3 ids have no `.text` immediates (script VM only).

| Idx | ID | Hex | Proposed name | Category | Key xrefs | Ghidra labels | Open questions |
|-----|-----|-----|---------------|----------|-----------|---------------|----------------|
| 325 | 65861 | 0x10145 | `music_level_vybijena` | audio/music | script 65855 `@0x00a5` SetMusic(65861,0) | — (script-only) | — |
| 326 | 65862 | 0x10146 | `music_level_exitus` | audio/music | script 65860 `@0x0094` SetMusic(65862,0) | — | — |
| 327 | 65863 | 0x10147 | `music_level_steel_works` | audio/music | script 65857 `@0x00e6` SetMusic(65863,65538) | — | — |
| 328 | 65864 | 0x10148 | `music_level_8bit` | audio/music | script 65859 `@0x0144` SetMusic(65864,65536) | — | — |
| 331 | 65867 | 0x1014b | `music_match_intro` | audio/music | `CGaming_ctor` `@00420148` PUSH 0x1014b → +0x358 | EOL comment @00420148 | Exact track title unknown |
| 332 | 65868 | 0x1014c | `music_level_bedtime_story` | audio/music | script 65856 `@0x00da` SetMusic(65868,65537) | — | — |
| 333 | 65869 | 0x1014d | `music_level_malaria` | audio/music | script 65858 `@0x00bb` SetMusic(65869,0) | — | — |
| 334 | 65870 | 0x1014e | `level_8bit_sfx_bank` | audio/sfx | AudioBankIndex 65536 → bank 65870 | — | Sample slugs in registry |
| 335 | 65871 | 0x1014f | `level_bedtime_sfx_bank` | audio/sfx | AudioBankIndex 65537 → bank 65871 | — | Sample slugs in registry |
| 336 | 65872 | 0x10150 | `level_steel_works_sfx_bank` | audio/sfx | AudioBankIndex 65538 → bank 65872 | — | Sample slugs in registry |

## Audio architecture notes

- **Level BGM:** `SetMusic(musicId, bankIndexId)` stores `musicId` at `CGaming+0x350`; `CGaming_LoadBackgroundMusic` decodes the Mp3 into `CDSAudioPlayer` at `+0x354` during level load.
- **Pre-match intro:** `65867` is **not** the level BGM — it is preloaded in `CGaming_ctor` into `CDSAudioPlayer` at `+0x358` and played during the pre-match modal (`cmd 0xed` → `FUN_0041c290`).
- **SFX banks:** PCM resources 65870–65872 are referenced indirectly via `AudioBankIndex` ids 65536–65538 (already named in catalog as `sfx_bank_level_*`).

## Registry sample slugs (AudioBank)

**65870 / 0x1014e:** `sfx_8bit_opponent_spawn`, `sfx_8bit_explosion`, `sfx_8bit_event_complete`, `sfx_8bit_beep_low`, `sfx_8bit_beep_high`

**65871 / 0x1014f:** `sfx_bedtime_lamp_click`, `sfx_bedtime_pillow_pop`, `sfx_bedtime_toy_squeak`, `sfx_bedtime_bed_creak`, `sfx_bedtime_monster_snort`

**65872 / 0x10150:** `sfx_steel_hydraulic_loop`, `sfx_steel_explosion`, `sfx_steel_spark_clang`

**Ghidra:** `save_program` after EOL comment on `00420148`.
