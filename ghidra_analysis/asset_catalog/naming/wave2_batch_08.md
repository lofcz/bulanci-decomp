# Asset naming — Wave 2 batch 8/15 (indices 209, 220–228)

**Scope:** BitmapSprite only — `0x100d1`, `0x100dc`–`0x100e4`.

**Verification:** Ghidra insn/data xrefs + overlay script 65856 disasm + atlas frame counts. `save_program` on `bulanci.exe`.

| Idx | ID | Hex | Proposed name | Category | Key xrefs | Ghidra labels / comments | Open questions |
|-----|-----|-----|---------------|----------|-----------|--------------------------|----------------|
| 209 | 65745 | 0x100d1 | `player_enemy_sit_facing_left` | gameplay/player | `gABulanekEnemySitAnimSheet[1]` @ 0x004af00c; read @ CBulanekResolveAndBindAnimTrack when kind 0x20..0x23 | Label `gABulanekEnemySitAnimSheet_facingLeft_res_0x100d1` | Pairing with `player_sit_facing_left` (0x100d0) |
| 220 | 65756 | 0x100dc | `gaming_ambient_sky_glow` | gameplay/gaming | `CGaming_ctor` PUSH 0x100dc @ 0x00420358 → CBulanci+0x330 | PRE_COMMENT @ 0x00420358 | Exact visual (dusk halo vs. clouds) |
| 221 | 65757 | 0x100dd | `gaming_ambient_insects` | gameplay/gaming | `CGaming_ctor` PUSH 0x100dd @ 0x00420344 → CBulanci+0x32c | PRE_COMMENT @ 0x00420344 | — |
| 222 | 65758 | 0x100de | `pickup_machine_gun` | gameplay/pickups | `g_PickupKind_SpriteIdTable` kind 4 @ 0x00482804 (4827F4+16) | Label `g_PickupKind_SpriteIdTable_kind4_machineGun_res_0x100de`; PRE_COMMENT @ 0x00482804 | Weapon stat mapping in gameplay code |
| 223 | 65759 | 0x100df | `level_bedtime_bunny_clip_a` | levels/bedtime_story/decor | Script 65856 fn@0x0286 @0x035a slot 6; frames [65759,65760,65761,65763,65762] | Renamed from `chimney_smoke_a` (Agent0/10: atlas = brown bunny, not smoke) | 8f 116×114 |
| 224 | 65760 | 0x100e0 | `level_bedtime_bunny_clip_b` | levels/bedtime_story/decor | Same bunny slot-6 cycle | Renamed from `chimney_smoke_b` | 5f static |
| 225 | 65761 | 0x100e1 | `level_bedtime_bunny_clip_c` | levels/bedtime_story/decor | Same bunny cycle | Renamed from `chimney_smoke_c` | 20f idle |
| 226 | 65762 | 0x100e2 | `level_bedtime_bunny_clip_d` | levels/bedtime_story/decor | Death/decap clip (played last) | Renamed from `chimney_smoke_d` | 25f gore |
| 227 | 65763 | 0x100e3 | `level_bedtime_bunny_clip_e` | levels/bedtime_story/decor | Idle before death clip | Renamed from `chimney_smoke_e` | 20f idle |
| 228 | 65764 | 0x100e4 | `level_bedtime_butterfly_fly_a` | levels/bedtime_story/decor | Script 65856 @0x0254 InsertView CreateAnim (+50,+50) slot 8; frames [65764,65765,65766] | Renamed from erroneous `level_bedtime_lamp_flicker_a` | 65765/65766 named in other batches (butterfly b/c) |

**Registry:** Added `registry.json` assets for `0x000100d1` … `0x000100e4` (slug + folder + notes, `ship: true`).

**Catalog:** `name`, `category`, `notes` set for all ten IDs in `entries` (+ `byId` mirror).
