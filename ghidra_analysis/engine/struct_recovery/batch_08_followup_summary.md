# Struct recovery batch 8/50 — follow-up round 2

**Status:** `FOLLOWUP_COMPLETE`  
**Prior:** `batch_08_summary.md` (`CGunMouse`, `CHelpDlg`)  
**Program:** `bulanci.exe` (saved)

## Actions

### CGunMouse — coord queue (was UNK `coordRing`)

- Decompiled `CDSVec_ResizeIntPair@004250c0`, `CGunMouse_CoordRing_{Push,Pop,Clear,Reserve}@00424370..004253d0`, `CGunMouse_Draw@00424610`.
- Proved 16-byte ring at `+0x208`: `m_data`, `m_capacity`, `m_size`, `m_head` (8-byte `(x,y)` slots; capacity fixed at `0x1e` by ctor).
- Ghidra: created `CGunMouseCoordQueue` (16 B); replaced `coordRing` byte blob with `m_coordQueue` on `CGunMouse`.
- Updated `CGunMouse.md` layout + nested queue table.

### CHelpDlg — align with CHistoryDlg / CIntList

- Decompiled `CIntListInsertSortedOrAppend@00407e20`, `CHelpDlgCtor@00421e40`, `CHelpDlg_LoadHelpPage@00421c10`.
- Confirmed `+0x70..+0x78` matches `CIntList` (`m_data`, `m_capacity`, `m_count`); `+0x7c` is separate `field_0x7c` (ctor `=8`, unread).
- Ghidra: renamed dialog tail fields to `CHistoryDlg` convention (`m_pageIds_*`, `m_currentPage`, `pHelpView`, `pActiveScript`, `pIcon*`); typed `pHelpView` / `pActiveScript` / icons.
- Updated `CHelpDlg.md` layout table and UNK section.

## Ghidra deltas

| Type | Change |
|------|--------|
| `CGunMouseCoordQueue` | **new** — 4 fields, 16 B |
| `CGunMouse` | `coordRing` → `m_coordQueue` (`CGunMouseCoordQueue`) |
| `CHelpDlg` | Field renames + pointer types on tail (`CHelpView *`, `CDSScript *`, `CIcon *`) |

`get_struct_layout`: `CGunMouse` still **536 B**; `CHelpDlg` still **156 B**.

## Remaining UNK (deferred)

- `CGunMouse`: `CDSImage` embed sub-fields only (top-level `CDSImage` applied slice 09); `trackManager` → `CDSVideoPlayer` @ `+0x1b0` (agent todo 10, 2026-05-30).
- `CHelpDlg` / both dialogs: `base_CWindow_MI` (`0x00..0x6f`) not field-split; `field_0x7c` at `+0x7c` unread.

## Artifacts

- `CGunMouse.md`, `CHelpDlg.md` (updated)
- `batch_08_followup_summary.md` (this file)
