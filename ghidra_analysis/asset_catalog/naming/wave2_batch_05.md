# Naming wave 2 — batch 5/15 (indices 129–138)

**Class:** BitmapSpecial only  
**Source:** `unnamed_batches.json` → `gallery_no_registry_slug`  
**Verification:** Ghidra MCP `read_memory` on `g_kScrollBarBitmapIds_V/H` + `g_volumeSliderBitmapIds`; `decompile_function` @ `CScrollBar_BuildAt` (00408370), `CSetupDlgCtor` (0040e290); `get_xrefs_to` on table bases.

## Slot layout (`CScrollBar_BuildAt` 9-bitmap order)

| Offset | Role | State |
|--------|------|-------|
| +0x80 | top/left button | normal |
| +0x84 | top/left button | pressed |
| +0x88 | top/left button | disabled |
| +0x8c | thumb | normal |
| +0x90 | thumb | pressed |
| +0x94 | thumb | disabled |
| +0x98 | bottom/right button | normal |
| +0x9c | bottom/right button | pressed |
| +0xa0 | bottom/right button | disabled |

Default tables: vertical `g_kScrollBarBitmapIds_V` @ **004ae5a0**, horizontal `g_kScrollBarBitmapIds_H` @ **004ae5c4**, setup volume `g_volumeSliderBitmapIds` @ **004aea34** (reuses horizontal thumb triple).

## Registry + catalog

| Idx | ID | Hex | Slug / `name` | Category | Table slot | Key xrefs |
|-----|-----|-----|---------------|----------|------------|-----------|
| 129 | 65665 | 0x10081 | `scroll_thumb_pressed` | ui/widgets/scrollbar | H[4], vol[4] | 004ae5d4 ← CScrollBar_BuildAt; 004aea44 ← CSetupDlgCtor; 004ae084 preload |
| 130 | 65666 | 0x10082 | `scroll_v_thumb_pressed` | ui/widgets/scrollbar | V[4] | 004ae5b0 ← CScrollBar_BuildAt |
| 131 | 65667 | 0x10083 | `scroll_thumb_normal` | ui/widgets/scrollbar | H[3], vol[3] | 004ae5d0, 004aea40 |
| 132 | 65668 | 0x10084 | `scroll_v_thumb_normal` | ui/widgets/scrollbar | V[3] | 004ae5ac |
| 133 | 65669 | 0x10085 | `scroll_thumb_disabled` | ui/widgets/scrollbar | H[5], vol[5] | 004ae5d8, 004aea48 |
| 134 | 65670 | 0x10086 | `scroll_v_thumb_disabled` | ui/widgets/scrollbar | V[5] | 004ae5b4 |
| 135 | 65671 | 0x10087 | `scroll_v_bottom_pressed` | ui/widgets/scrollbar | V[7] | 004ae5bc |
| 136 | 65672 | 0x10088 | `scroll_h_top_pressed` | ui/widgets/scrollbar | H[1] | 004ae5c8 |
| 137 | 65673 | 0x10089 | `scroll_h_bottom_pressed` | ui/widgets/scrollbar | H[7] | 004ae5e0 |
| 138 | 65674 | 0x1008a | `scroll_v_top_pressed` | ui/widgets/scrollbar | V[1] | 004ae5a4 |

**Rename note:** 65665 was `ui_setup_volume_slider_thumb_pressed` but Ghidra memory shows it is the **horizontal** scrollbar thumb-pressed (and volume slider thumb-pressed); vertical thumb-pressed is **65666** (`0x10082`).

### Memory dumps (little-endian DWORDs)

`g_kScrollBarBitmapIds_V` @ 004ae5a0: `8e 8a 92 84 82 86 8b 87 8f` → slots 1/3/4/7/0 = **8a, 84, 82, 86, 87** (this batch).  
`g_kScrollBarBitmapIds_H` @ 004ae5c4: `8c 88 90 83 81 85 8d 89 91` → slots 1/3/4/5/7 = **88, 83, 81, 85, 89** (this batch).  
`g_volumeSliderBitmapIds` @ 004aea34: indices 3–5 = **83, 81, 85** (shared thumb triple).

## Ghidra follow-up

- [ ] `save_program` after review (labels already use `g_kScrollBarBitmapIds_*` / `g_volumeSliderBitmapIds` in decompiler).
- No `.text` PUSH immediates for these IDs (data-table driven).
