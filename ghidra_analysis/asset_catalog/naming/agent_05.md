# Asset verify — Agent 5/10 (script 65859 hazards + pools)

**Level:** V zajetí 8-bitu (`level_8bit_captivity`, script `65859` / `0x10143`)  
**Method:** `res_0000065859_2026_Script.script.asm` xrefs + atlas PNG review under `unpacked/overlay/`.

## ID map correction (spawn pools vs decor)

| Hex | ID | Slug | Role in OnInit |
|-----|-----|------|----------------|
| `0x10060`–`0x10063` | 65632–65635 | `level_8bit_spawn_pool_{3,0,1,2}` | **SpawnOpponentEx** skin thumbs (`CollInsert` → global 12) |
| `0x10064` | 65636 | `level_8bit_bg` | Full-screen background `(0,0)` — **not** a spawn pool |
| `0x10065` | 65637 | `level_8bit_wireframe_dome` | Wire hemisphere decor `(576,80)` — **not** a spawn pool |
| `0x1005d`–`0x1005f` | 65629–65631 | `wire_pyramid` / `wire_glyph_sm` / `wire_prism` | Additional 1-bit wireframe props |

CollInsert order: `65633, 65634, 65635, 65632` → pool indices `[0..3]`.

## Visual verification (atlases)

| Hex | Slug | Frames | Visual | Verdict |
|-----|------|--------|--------|---------|
| `0x100fe` | `level_8bit_pillow_burst` | 7 | Wireframe **box** shatter | Name OK (pillow hazard slot 10); art is wireframe crate/box |
| `0x100ff` | `level_8bit_crate_break_5f` | 5 | Wireframe crate break | OK (`5f` = 5 frames) |
| `0x10100` | ~~`crate_break_7f`~~ → **`crate_break_6f_slot12`** | 6 | Wireframe crate break | **Renamed** (was mis-tagged 7f) |
| `0x10101` | ~~`crate_break_6f`~~ → **`crate_break_6f_slot14`** | 6 | Wireframe crate break | **Renamed** (disambiguate two 6-frame breaks) |
| `0x10102`–`0x10107` | `zombie_peek_{a..f}` | 13–20 | Green stitched Bulánek from grey hole | OK — **zombie**, not prop |
| `0x10108` | `level_8bit_knockout_fall` | 20 | Player fall + blood at slot 20 | OK — distinct from slot-9 ambush |

**SetAnim chain (slot 9):** `[65795, 65798, 65796, 65799, 65794, 65797]` → clips **a, b, c, d, e, f** (indices 0–5). `OnBitmapEvt` + `SpawnOpponentEx` spawn opponents at slot-9 view.

## Catalog / registry edits

- Renamed `level_8bit_crate_break_7f` → `level_8bit_crate_break_6f_slot12`.
- Renamed `level_8bit_crate_break_6f` → `level_8bit_crate_break_6f_slot14`.
- Normalized `category` → `levels/8bit` for hazard sprites; clarified notes (frame counts, slot/obstacle pairs).
- Fixed `byClass.BitmapSprite` null `name` fields for `65796`–`65800`.
- `registry.json`: matching slug/notes updates for `0x10064`/`0x10065` (not spawn pools).

## Ghidra

PRE_COMMENT on:

- `CLevelScriptOpExt_CollInsert` @ `0x00418b20`
- `CLevelScriptOpExt_SpawnOpponentEx` @ `0x0041f6a0`
- `CLevelScriptOpExt_CreateImage` @ `0x0041d820`

(Resource IDs live in overlay script bytecode only — no `.text` immediates.)
