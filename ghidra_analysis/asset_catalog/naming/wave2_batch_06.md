# Asset naming — Wave 2 batch 6/15 (indices 139–148)

**Scope:** BitmapSpecial `0x0001008b`–`0x00010094` (decimal 65675–65684).

**Verification:** Ghidra MCP `get_xrefs_to` + `decompile_function` on `CScrollBar_BuildAt` @ `0x00408370` and `CMenu_ctor_with_ui` @ `0x004265e0`. Catalog xrefs agree with live decompilation.

| Idx | ID | Hex | Slug | Folder | Role / table slot | Key xrefs (Ghidra) |
|-----|-----|-----|------|--------|-------------------|-------------------|
| 139 | 65675 | 0x1008b | `scroll_v_bottom_normal` | ui/widgets/scrollbar | `g_dwScrollbarVertDefaultBitmapIds[6]` @ `DAT_004ae5b8` | `CBulanci_OnCreate` → `0x004ae0ac`; `CScrollBar_BuildAt` → `0x004ae5b8` |
| 140 | 65676 | 0x1008c | `ui_scrollbar_h_arrow_left_normal` | ui/widgets/scrollbar | `g_dwScrollBarBitmapIds_H[0]` @ `DAT_004ae5c4` | `CBulanci_OnCreate` → `0x004ae0b0`; `CScrollBar_BuildAt` DATA+READ @ `0x004ae5c4` |
| 141 | 65677 | 0x1008d | `ui_scrollbar_h_arrow_right_normal` | ui/widgets/scrollbar | `g_dwScrollBarBitmapIds_H[6]` @ `DAT_004ae5dc` | `CBulanci_OnCreate` → `0x004ae0b4`; `CScrollBar_BuildAt` @ `0x004ae5dc` |
| 142 | 65678 | 0x1008e | `ui_scrollbar_v_arrow_up_normal` | ui/widgets/scrollbar | `g_dwScrollBarBitmapIds_V[0]` @ `DAT_004ae5a0` (array base) | `CBulanci_OnCreate` → `0x004ae0b8`; `CScrollBar_BuildAt` @ `0x004ae5a0` |
| 143 | 65679 | 0x1008f | `ui_scrollbar_v_arrow_down_disabled` | ui/widgets/scrollbar | `g_dwScrollBarBitmapIds_V[8]` @ `DAT_004ae5c0` | `CBulanci_OnCreate` → `0x004ae0bc`; `CScrollBar_BuildAt` @ `0x004ae5c0` |
| 144 | 65680 | 0x10090 | `ui_scrollbar_h_arrow_left_disabled` | ui/widgets/scrollbar | `g_dwScrollBarBitmapIds_H[2]` @ `DAT_004ae5cc` | `CBulanci_OnCreate` → `0x004ae0c0`; `CScrollBar_BuildAt` @ `0x004ae5cc` |
| 145 | 65681 | 0x10091 | `ui_scrollbar_h_arrow_right_disabled` | ui/widgets/scrollbar | `g_dwScrollBarBitmapIds_H[8]` @ `DAT_004ae5e4` | `CBulanci_OnCreate` → `0x004ae0c4`; `CScrollBar_BuildAt` @ `0x004ae5e4` |
| 146 | 65682 | 0x10092 | `ui_scrollbar_v_arrow_up_disabled` | ui/widgets/scrollbar | `g_dwScrollBarBitmapIds_V[2]` @ `DAT_004ae5a8` | `CBulanci_OnCreate` → `0x004ae0c8`; `CScrollBar_BuildAt` @ `0x004ae5a8` |
| 147 | 65683 | 0x10093 | `icon_start` | menu/icons | `g_dwMenuIconRes_StartNormal` @ `DAT_004af90c` | `CBulanci_OnCreate` preload; `CMenu_ctor_with_ui` READ @ `0x004267fc` → CDSBitmap `(100,55)` |
| 148 | 65684 | 0x10094 | `icon_start_hi` | menu/icons | `g_dwMenuIconRes_StartHighlight` @ `DAT_004af910` | `CBulanci_OnCreate` preload; `CMenu_ctor_with_ui` READ @ `0x00426866`; hidden until hover swap |

## Notes

- **Two scrollbar skins:** indices 139–146 split between the legacy 9-slot **vertical default** tail (`scroll_v_bottom_normal`) and the shared **BuildAt** 9-ID tables `g_dwScrollBarBitmapIds_H` / `_V` used when `param_5==NULL` (decomp comments @ `0x00408370`).
- **Menu icons:** renamed from `ui_menu_icon_start_*` to `icon_start` / `icon_start_hi` to match `icon_quit` / `icon_quit_hi` (`0x1004b`/`0x1004c`) registry style.
- **Registry:** added `registry.json` keys `0x0001008b`–`0x00010094` (`ship: true`). Gap was between batch-5 entries (`0x1008a`) and batch-7 (`0x10095`).
- **Ghidra:** `save_program` after verification (globals already commented in decompilation).

## Open questions

- None for this slice; thumb/normal/pressed slots for the same arrays were named in wave-2 batch 5 (`0x10081`–`0x1008a`).
