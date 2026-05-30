# CHistoryDlg

## Status

**VERIFIED** — `sizeof == 0x9c` (heap allocation + ctor field span).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CHistoryDlg) == 0x9c` | `0x00489214` | `_Globals::CreateObject` / `OperatorNewWithBadAlloc(0x9c)` before `CHistoryDlg_ctor@0x004231d0` |
| Last field at `+0x98` | `0x004231d0` | ctor stores `CIcon*` at `+0x98`; `CHistoryDlg_LoadHistoryPage` iterates `this+0x9c` downward over four child pointers |
| Same layout as `CHelpDlg` | `0x00422078` vs `0x004231d0` | Identical ctor field pattern; both allocate `0x9c` |

## Layout table

`CWindow` prefix (`+0x00` .. `+0x6f`): embedded **`win`** (`CWindow` @ 0); ctor calls `CWindow_BuildAt(&this->win,…)` then sets `(this->win).pVftable_*` (`CHistoryDlg_ctor@0x004231d0`).

Dialog-specific fields:

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x70` | 4 | `int*` | `m_pageIds.m_data` | `CIntListInsertSortedOrAppend(this+0x70,…)` in ctor; page lookup in `CHistoryDlg_LoadHistoryPage@0x00422f70` |
| `+0x74` | 4 | `int` | `m_pageIds.m_capacity` | `CIntList_EnsureCapacity` compares `*(this+4)` |
| `+0x78` | 4 | `int` | `m_pageIds.m_count` | ctor `=0`; clamp/nav in `CHistoryDlg_LoadHistoryPage`, `CHistoryDlg_OnEvent@0x00423130` |
| `+0x7c` | 4 | `int` | `m_pageIds.m_growthChunk` | ctor `=8`; `CIntList_EnsureCapacity@0x00401d30` resize step `*(this+0xc)` (mirrors `CHelpDlg::pageList_capacity`) |
| `+0x80` | 4 | `int` | `m_currentPage` | ctor `=-1`; `CHistoryDlg_OnEvent`, `CHistoryDlg_LoadHistoryPage` |
| `+0x84` | 4 | `CHistoryView*` | `pHistoryView` | ctor `=0`; load/release in `CHistoryDlg_LoadHistoryPage`; dtor releases |
| `+0x88` | 4 | `CDSScript*` | `pActiveScript` | ctor `=0`; refcount/`CallExport` in `CHistoryDlg_LoadHistoryPage`; dtor releases |
| `+0x8c` | 4 | `CIcon*` | `pIconFirst` | ctor `CIcon_BuildAt` + `CDSView__AddChild`; nav enable in `CHistoryDlg_LoadHistoryPage` |
| `+0x90` | 4 | `CIcon*` | `pIconPrev` | ctor |
| `+0x94` | 4 | `CIcon*` | `pIconNext` | ctor |
| `+0x98` | 4 | `CIcon*` | `pIconLast` | ctor |

Menu enumeration filters `classId == 0x802` (`CHistoryScript`) when filling `m_pageIds` (`CHistoryDlg_ctor`).

## Ghidra apply

- `create_struct` `CHistoryDlg` size `0x9c` (156 B); `get_struct_layout` OK.
- Slice **10** (2026-05-30): typed `pHistoryView` (`CHistoryView *`), `pActiveScript` (`CDSScript *`), `pIconFirst..Last` (`CIcon *`); `CHistoryDlg_ctor` prototype `CHistoryDlg * __fastcall`.
- Page load uses `_Globals::CDSScript_SetBoundParentView@0x00422620` before `CDSScript::CallExport` (`CHistoryDlg_LoadHistoryPage@0x00422f70`).
- Primary vtable `0x004833ec` (28 slots, `master_vtable_catalog.md`).
- **Agent todo 18 (2026-05-30):** `pCwindow_prefix` → embedded **`CWindow win`** @ 0; `pActiveScript` typed `CDSScript *`; `CHistoryDlg_ctor` prototype restored; `save_program`.
- **R3 todo 11 (2026-05-30):** `CWindow` prefix `+0x00..+0x6f` exposed via `(this->win).pVftable_primary` … `pDefaultFocusChild` in `CHistoryDlg_ctor@0x004231d0`; `set_function_this_type` on ctor / `CHistoryDlg_LoadHistoryPage`; `CWindow_BuildAt` typed `CWindow *`.

**R5 worker 13 (2026-05-30):** Page swap calls `CMenu_DetachChildWithVisibility@0x0042d160` before releasing `pHistoryView` (`CHistoryDlg_LoadHistoryPage@0x00422fc1`).

## UNK

- Ghidra may auto-prefix embedded `CIntList` / `int` members as `pM_data` / `nM_currentPage` in decompiler; logical names `m_pageIds.*` / `m_currentPage`.
