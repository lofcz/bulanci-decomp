# Struct recovery batch 12 follow-up

**Status:** FOLLOWUP_COMPLETE  
**Prior:** `batch_12_summary.md`  
**Structs:** `CItemInfo`, `CLevelList`

## Actions taken

| Item | Result |
|------|--------|
| `CItemInfo+0x04` | **Padding** — not written in `CItemInfo::CreateObject@0x404c50` (`puVar1[1]` untouched); `CListBoxItem_ctor@0x40b640` also skips `+0x04` |
| `CItemInfo+0x08` / `+0x0c` | `+0x0c` used as row **`flags`** when object is a `CListBoxItem` (`*(byte*)(item+0xc)&1` in `CListBox_RenderItem@0x405fc0`, `CLevelList_RenderItem@0x40d490`); both dwords zeroed in `CreateObject` and `CListBoxItem_ctor` |
| `CItemInfo` MFC tree | Class **2013** (`0x7dd`) @ `0x47b1da`; parent meta `0x4b7bfc` (`CDSObject`). **`CListBoxItem`** (2014 / `0x7de`) parent `0x4b33b8` @ `0x47b205` |
| `CLevelList` class id | **2060** (`0x80c`) @ `0x47b9ba`; factory `0x40ba30`, parent `CListBox` meta `0x4b33e0`, self meta `0x4b36b8` |
| `CLevelList.base` interior | Cross-linked `widgets.md` §13–14 and Ghidra `CListBox` (`p_base` 204 B + tail `labelFont`…`dwTintColor`); no new fields on `CLevelList` |
| Ghidra | `create_function` + rename **`CItemInfo::CreateObject`** @ `0x404c50` (was `DAT_00404c50`) |

## Ghidra deltas

- `CItemInfo::CreateObject` defined and decompiled at `0x404c50`
- `save_program bulanci.exe`

## Doc updates

- `CItemInfo.md` — padding, flags inheritance, registration disasm, `CreateObject` decompile
- `CLevelList.md` — class id proof, `CListBox` interior pointer

## Remaining UNK

- `CItemInfo+0x08`: zero-init only; likely CDS chain/refcount slot per `widgets.md` §13 `CListBoxItem` row layout — no direct read xref in follow-up scope
- Full decomposition of `CListBox.p_base` (`undefined1[204]`) — owned by `CListBox` / `CListViewer` recovery, not `CLevelList`
