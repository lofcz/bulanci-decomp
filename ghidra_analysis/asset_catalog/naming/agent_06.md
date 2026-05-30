# Asset verify — Agent 6/10 (Exitus script 65860 scenery)

**Scope:** `0x10025`–`0x10030`, preface `0x10035` — graves, spruce/decor parallax props placed by `res_0000065860_2026_Script`.

**Verification:** PNG multimodal review + script `65860` xref trace (`InsertView` / `LoadPreface` / `SetOrderAxis`). No `bulanci.exe` immediates (overlay script VM only). Manifest transparency keys confirm bg/fg layering.

## Xref summary (script 65860)

| ID | Hex | Opcode @ offset | Anchor | Layer |
|----|-----|-----------------|--------|-------|
| 65573 | 0x10025 | InsertView @0x02c8 | (404,56) | bg, axis −25 |
| 65574 | 0x10026 | InsertView @0x02b2 | (404,56) | fg (InsertMode 0) |
| 65575 | 0x10027 | InsertView @0x0341 | (664,113) | bg, axis −20 |
| 65576 | 0x10028 | InsertView @0x03a4 | (82,237) | bg, axis −20 |
| 65577 | 0x10029 | InsertView @0x038e | (82,237) | fg |
| 65578 | 0x1002a | InsertView @0x041d | (294,361) | bg, axis −25 |
| 65579 | 0x1002b | InsertView @0x0407 | (294,361) | fg |
| 65580 | 0x1002c | InsertView @0x032b | (664,113) | fg |
| 65581 | 0x1002d | InsertView @0x0480 | (50,71) | bg, axis −25 |
| 65582 | 0x1002e | InsertView @0x046a | (50,71) | fg |
| 65583 | 0x1002f | InsertView @0x0255 | (625,0) | optional decor (fn@0x239, 1/6) |
| 65584 | 0x10030 | InsertView @0x0280 | (678,250) | optional decor (fn@0x239, 1/6) |
| 65589 | 0x10035 | LoadPreface @0x008f | — | level intro bitmap |

## PNG verification

| ID | Visual | Verdict |
|----|--------|---------|
| 65573/65574 | Stone cross headstone, open pit, small spruce; bg transparent / fg on grass | ✓ grave pair |
| 65575/65580 | Tilted wooden cross; bg transparent / fg on grass | ✓ grave pair |
| 65576/65577 | Twin lashed-log crosses + pine; bg transparent / fg with flowers | ✓ grave pair |
| 65578/65579 | Single log cross; bg transparent / fg with Bulánek burial mound | ✓ grave pair |
| 65581/65582 | Spruce tree; bg transparent / fg adds fence planks | ✓ spruce pair |
| 65583 | Pine branch cluster from top-right | ✓ top-edge decor |
| 65584 | Conifer bush branch | ✓ bush decor |
| 65589 | Hooded Bulánek, cross + candle (LoadPreface art) | ✓ preface |

## Renames applied (mistakes fixed)

| ID | Hex | Old slug | New slug | Reason |
|----|-----|----------|----------|--------|
| 65573 | 0x10025 | `level_exitus_deco_404_parallax` | `exitus_scenery_grave_stone_cross_bg` | Coordinate/deco naming; actually stone grave bg layer |
| 65574 | 0x10026 | `level_exitus_deco_404_back` | `exitus_scenery_grave_stone_cross_fg` | `_back` ambiguous; matches `_fg` convention |
| 65575 | 0x10027 | `level_exitus_deco_664_parallax` | `exitus_scenery_grave_cross_tilted_bg` | Pairs with 65580; align `_bg/_fg` pattern |
| 65580 | 0x1002c | `exitus_scenery_grave_cross_tilted` | `exitus_scenery_grave_cross_tilted_fg` | Missing `_fg` suffix vs other pairs |

**Unchanged (verified correct):** 65576–65579, 65581–65584, 65589 `level_exitus_preface`.

**Also fixed:** `catalog.json` byClass typo `BitmapSpeecial` → `BitmapSpecial` for 65573; byClass `65589.name` was `null`.

**Registry folders:** `0x10025`–`0x10027` moved from `levels/exitus/deco` → `levels/exitus/scenery`.
