# Asset naming — Wave 2, Batch 2/15

**Scope:** catalog indices 45–49 and 60–64 (`id` 65581–65585, 65596–65600). All `BitmapSpecial` (class 28).

**Verification:** Ghidra MCP — `decompile_function` on `CHelpDlg_ctor` @ `00421e40`, `CHistoryDlg_ctor` @ `004231d0`, `CGaming_ctor` @ `0041ff90`; `read_memory` on `g_kHelpDlg_pageNavIconResIds` @ `00482b04` and `g_dwCgamingPreloadResourceIds` @ `00482968`. Overlay script `65860` xrefs from existing catalog.

**Outputs:** `catalog.json` (names, categories, Ghidra xrefs, notes), `registry.json` (new slug entries for previously missing hex keys).

## Per-asset summary

| Idx | ID | Hex | Path | Name (slug) | Category | Evidence | Confidence |
|-----|-----|-----|------|-------------|----------|----------|------------|
| 45 | 65581 | 0x1002d | `res_0000065581_28_BitmapSpecial.bin` | `exitus_scenery_spruce_bg` | levels/exitus/scenery | Script 65860 `@0x0480` `InsertView(SetOrderAxis(CreateImage(50,71), -25))` | High |
| 46 | 65582 | 0x1002e | `res_0000065582_28_BitmapSpecial.bin` | `exitus_scenery_tree_fence_fg` | levels/exitus/scenery | Script 65860 `@0x046a` `CreateImage(50,71)` foreground pair to 65581 | High |
| 47 | 65583 | 0x1002f | `res_0000065583_28_BitmapSpecial.bin` | `exitus_scenery_pine_branch_top` | levels/exitus/scenery | Script 65860 `@0x0255` optional `CreateImage(625,0)` | High |
| 48 | 65584 | 0x10030 | `res_0000065584_28_BitmapSpecial.bin` | `exitus_scenery_conifer_bush` | levels/exitus/scenery | Script 65860 `@0x0280` optional `CreateImage(678,250)` | High |
| 49 | 65585 | 0x10031 | `res_0000065585_28_BitmapSpecial.bin` | `proj_rocket_up` | gameplay/projectile | `CGaming_ctor` preload dword `[11]==0x10031` @ `0x00482994` (322 B) | High — PNG: rocket nose-up; was proj_syringe |
| 60 | 65596 | 0x1003c | `res_0000065596_28_BitmapSpecial.bin` | `ui_pageNav_prev_pressed` | menu/widgets/page_nav | Nav table `0x482b1c+8`; `CHelpDlg_ctor` / `CHistoryDlg_ctor` | High |
| 61 | 65597 | 0x1003d | `res_0000065597_28_BitmapSpecial.bin` | `ui_pageNav_next_hover` | menu/widgets/page_nav | Nav table `0x482b10+4` | High |
| 62 | 65598 | 0x1003e | `res_0000065598_28_BitmapSpecial.bin` | `ui_pageNav_next_normal` | menu/widgets/page_nav | Nav table `0x482b10+0`; direct xref `00421fee` | High |
| 63 | 65599 | 0x1003f | `res_0000065599_28_BitmapSpecial.bin` | `ui_pageNav_next_pressed` | menu/widgets/page_nav | Nav table `0x482b10+8` | High |
| 64 | 65600 | 0x10040 | `res_0000065600_28_BitmapSpecial.bin` | `ui_pageNav_last_hover` | menu/widgets/page_nav | Nav table `0x482b04+4` (last-button hover slot) | High |

## Page-nav table layout (`g_kHelpDlg_pageNavIconResIds` @ `0x00482b04`)

12 dwords, **normal → hover → pressed** per button (CHelpDlg CIcon bases: last `0x482b04`, next `0x482b10`, prev `0x482b1c`, first `0x482b28`):

| Button | Base | +0 (normal) | +4 (hover) | +8 (pressed) |
|--------|------|-------------|------------|--------------|
| Last | `0x482b04` | 0x10041 | **0x10040** | 0x10042 |
| Next | `0x482b10` | **0x1003e** | **0x1003d** | **0x1003f** |
| Prev | `0x482b1c` | 0x1003b | 0x1003a | **0x1003c** |
| First | `0x482b28` | 0x10038 | 0x10037 | 0x10039 |

`CHistoryDlg_ctor` mirrors the same IDs via `g_kHistoryDlg_pageNavIconResIds` @ `0x0048336c` (identical dword sequence).

## CGaming preload slot

`g_dwCgamingPreloadResourceIds` @ `0x00482968`: indices 8–10 = `0x10032..0x10034` (in-flight rocket left/right/down), **index 11 = `0x10031`** (rocket up). Agent 7/10 PNG-verified.

## Open questions

- None blocking; optional PNG review to confirm spruce vs. generic conifer on 65581/65584.

## Ghidra labels applied

Existing decompiler comments on `CHelpDlg_ctor` / `CHistoryDlg_ctor` / `CGaming_ctor` already reference nav tables and preload loop; no program save required for catalog-only batch.
