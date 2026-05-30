# Struct recovery batch 16 follow-up (round 2)

**Prior:** `batch_16_summary.md`  
**Program:** `bulanci.exe`  
**Status:** **FOLLOWUP_COMPLETE**

## Follow-ups from batch 16

| Item | Result |
|------|--------|
| Replace `CSessionList.base_0x00` with recovered `CWindow` dialog base | **Done** — `base_0x00 byte[112]` replaced with named fields `+0x00..+0x6c` matching `CMsgDialog` / `CExitDlg` dialog shell (`CWindow_BuildAt@0x00405560`, `CSessionList_BuildDialog@0x0040c2d0`) |
| Recover `CListBoxItem` for `CSessionItem` base docs | **Done** — `CListBoxItem.md` + Ghidra type `CListBoxItemRow` (20 B); `CSessionItem` base field names aligned |

## Ghidra deltas

| Type | Before | After |
|------|--------|-------|
| `CSessionList` | 124 B, `base_0x00 byte[112]` + tail ptrs | 124 B, full dialog header through `pDefaultFocusChild` + `pSessionListBox` / `pCaptionStatic` / `pJoinButton` |
| `CSessionItem` | `field_08`, `field_0c`, `dwField_0x04` | `pAssocOrNext`, `rowFlags`, `dwField_04` (matches list-row base) |
| `CListBoxItemRow` | — | **new**, 20 B (`pVftable` … `labelStringHandle`) |
| `CListBoxItem` | placeholder 1 B | unchanged (MCP cannot resize empty struct) |

`save_program bulanci.exe` at end of batch.

## Evidence used

- **CWindow base:** `CMsgDialog.md` / `get_struct_layout CMsgDialog` (112 B); `CSessionList_BuildDialog@0x0040c2d0` (`CWindow_BuildAt`, bbox `+0x20..+0x2c`, `wContainerFlags` at `+0x46`).
- **CListBoxItem:** `CListBoxItem_ctor@0x0040b640`, `CListBoxItem_dtor@0x00404cb0`, `CLevelList_RenderItem@0x0040d490` (`+0xc` flags, `+0x10` label).

## Artifacts

- `CListBoxItem.md` (new)
- `CSessionItem.md`, `CSessionList.md` (updated)

## Remaining UNK

- Ghidra `/CListBoxItem` placeholder vs `CListBoxItemRow` naming (manual rename/merge when Java scripts compile).
- `CSessionList` `+0x22..+0x23` implicit padding vs explicit `wPad_16` on `CMsgDialog`.
- `dwField_04` on list rows; cancel button still not stored on `CSessionList`.
