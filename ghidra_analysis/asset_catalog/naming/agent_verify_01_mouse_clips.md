# Agent 1/10 — Bedtime story gun-mouse clips (65767–65774)

**Scope:** Script 65856 slot 9 — `CreateAnim` @ overlay `+0x0495` / `BindToSlot(..., 9)`; `SetAnim(GetSlot(9), GetGlobalVar(7), …)` drives clip index 0..7.

**User hint confirmed:** Grey **mouse** with rifle (not Bulánek). Prior registry notes incorrectly described Bulánek poses.

## Script context

| Offset | Role |
|--------|------|
| `fn@0x0394` | Spawns gun-mouse: danger zones, obstacles, bg `CreateImage(..., 65606)`, 8-clip anim |
| `@0x0495` | `frames=[65767..65774]` |
| `@0x04ca` | `BindToSlot(..., IntConst(9))` |
| Main loop | `SetAnim(GetSlot(9), GetGlobalVar(7), IntConst(1))` — `GetGlobalVar(7)` = clip index |

## Visual verification (atlas PNG)

| Idx | ID | Hex | New slug | Frames | Visual |
|-----|-----|-----|----------|--------|--------|
| 0 | 65767 | 0x100e7 | `level_bedtime_mouse_pop_r` | 7 | Pop up from cover, facing right |
| 1 | 65768 | 0x100e8 | `level_bedtime_mouse_hide_r` | 6 | Sink / hide into cover, right |
| 2 | 65769 | 0x100e9 | `level_bedtime_mouse_shoot_r` | 10 | Shoot + muzzle flash, right |
| 3 | 65770 | 0x100ea | `level_bedtime_mouse_death_r` | 6 | Death gib, right |
| 4 | 65771 | 0x100eb | `level_bedtime_mouse_pop_l` | 6 | Pop up, facing left |
| 5 | 65772 | 0x100ec | `level_bedtime_mouse_hide_l` | 6 | Hide into cover, left |
| 6 | 65773 | 0x100ed | `level_bedtime_mouse_shoot_l` | 9 | Shoot from mount, left |
| 7 | 65774 | 0x100ee | `level_bedtime_mouse_death_l` | 6 | Death gib, left |

All sheets **54×55** per frame (except where noted in atlas).

## IDs 65759–65765 (out of scope — not mouse walk)

| IDs | Current names | Script slot | Actual content |
|-----|---------------|-------------|----------------|
| 65759–65763 | `level_bedtime_bunny_clip_*` | 6 @ `+0x035a` | Brown bunny cycle (was misnamed chimney smoke; Agent0/10) |
| 65764–65766 | `level_bedtime_butterfly_fly_*` | 8 @ `+0x0254` | Butterfly fly loop |

No separate mouse **walk** sheets in this ID band; slot-9 uses pop/hide/shoot/death clips only.

## Artifacts

- `catalog.json` — `name`, `category` → `levels/bedtime_story/mouse`, `notes` (16 rows: `entries` + `byClass`)
- `registry.json` — slugs `0x000100e7`..`0x000100ee`, folder `levels/bedtime_story/mouse`
- **Ghidra:** PRE_COMMENT @ `0x00409fb7` (CLevelScript enum); `save_program` on `bulanci.exe`
- No `.text` PUSH of overlay sprite IDs (script VM only)

**Not done:** `catalog.md` regen, git commit.
