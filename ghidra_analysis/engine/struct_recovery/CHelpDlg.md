# CHelpDlg

## Status

**VERIFIED** — size `0x9c` (156 bytes). Layout **PARTIAL** (page-index list + four nav `CIcon*`; `CWindow`/MI base through `+0x6f` not field-split).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CHelpDlg) == 0x9c` | `_Globals::CreateObject` @ `004222c0` | `OperatorNewWithBadAlloc(0x9c)` then `CHelpDlg::CHelpDlgCtor` |
| Last stored field ends `+0x9b` | `CHelpDlgCtor` @ `00421e40` | `*(this+0x98)=uVar5` (4-byte pointer) |
| Dtor clears through `+0x88` | `CHelpDlg_dtor` @ `004220e0` | Releases `+0x88`, resizes list at `+0x70` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | `0x70` | `CWindow` | `win` | `CHelpDlgCtor@00421e40` → `CWindow_BuildAt(&this->win,…)`; vtables via `(this->win).pVftable_*` |
| `0x70` | 4 | `int *` | `m_pageIds.m_data` | `CIntListInsertSortedOrAppend@00407e20` in `CHelpDlgCtor@00421e40`; `*(m_data+idx*4)` in `CHelpDlg_LoadHelpPage@00421c10` |
| `0x74` | 4 | `int` | `m_pageIds.m_capacity` | `CIntList` layout @ `CIntListInsertSortedOrAppend@00407e20` (`+4`); ctor `=0` |
| `0x78` | 4 | `int` | `m_pageIds.m_count` | ctor `=0`; bounds/nav in `CHelpDlg_LoadHelpPage@00421c10`, `CHelpDlg_OnSyntheticPageNav@00421dd0` |
| `0x7c` | 4 | `int` | `m_pageIds.m_growthChunk` | ctor `=8`; `CIntList_EnsureCapacity@0x00401d30` reads `*(this+0xc)` on list at `this+0x70` |
| `0x80` | 4 | `int` | `m_currentPage` | ctor `=-1`; `CHelpDlg_LoadHelpPage@00421c10`, `CHelpDlg_OnSyntheticPageNav@00421dd0` |
| `0x84` | 4 | `CHelpView *` | `pHelpView` | `CHelpDlg_LoadHelpPage@00421c10` (alloc `OperatorNew(0x68)` + `CHelpView_ctor`) |
| `0x88` | 4 | `CDSScript *` | `pActiveScript` | `CHelpDlg_LoadHelpPage@00421c10`, `CHelpDlg_dtor@004220e0` |
| `0x8c` | 4 | `CIcon *` | `pIconFirst` | `CHelpDlgCtor@00421e40`, `CHelpDlg_LoadHelpPage@00421c10` |
| `0x90` | 4 | `CIcon *` | `pIconPrev` | `CHelpDlgCtor@00421e40`, `CHelpDlg_LoadHelpPage@00421c10` |
| `0x94` | 4 | `CIcon *` | `pIconNext` | `CHelpDlgCtor@00421e40`, `CHelpDlg_LoadHelpPage@00421c10` |
| `0x98` | 4 | `CIcon *` | `pIconLast` | `CHelpDlgCtor@00421e40`, `CHelpDlg_LoadHelpPage@00421c10` |

## Ghidra apply

Batch 8: placeholder → `CHelpDlg` (156 B). Follow-up round 2: dialog tail fields renamed to match `CHistoryDlg` / `CIntList` (`m_pageIds_*`, `pHelpView`, `pIcon*`); `get_struct_layout` size unchanged.

**Agent todo 18 (2026-05-30):** `base_CWindow_MI` → embedded **`CWindow win`** @ 0; `pActiveScript` → `CDSScript *`; `CHelpDlgCtor` prototype; decompile shows `CWindow_BuildAt(&this->win,…)`.

**Slice 09 (2026-05-30):** `nField_0x7c` → `nM_pageIds_growthChunk`; page-list pointers aligned with `CHistoryDlg` (`pM_pageIds_data`, `nM_pageIds_capacity`, `nM_pageIds_count`). `get_struct_layout`: **Size: 156**.

**R3 todo 11 (2026-05-30):** `CWindow win` prefix field-split through `+0x6f` in decompile (`CHelpDlgCtor` → `CWindow_BuildAt(&this->win,…)`); `set_function_this_type` on ctor / `CHelpDlg_LoadHelpPage`.

**R5 worker 13 (2026-05-30):** Page swap calls `CMenu_DetachChildWithVisibility@0x0042d160` before releasing `pHelpView` (`CHelpDlg_LoadHelpPage@0x00421c61`).

Menu enumeration filters `classId == 0x81c` (`CHelpScript`) when filling `m_pageIds` (`CHelpDlgCtor@00421e40`).

## UNK

- Ghidra type-prefix on `m_pageIds_*` fields in decompiler (`pM_`/`nM_`) — cosmetic.

**Round 3 task 10:** `pHelpView` is `CHelpView *`; page load/release uses `(pHelpView->chain).pVftable_primary` — see [CHelpView.md](./CHelpView.md).

**R5 worker 05 (2026-05-30):** Primary vtable labeled `g_pCHelpDlg_vftable_primary` @ `0x00482b84` (`CHelpDlgCtor` / `CHelpDlg_dtor` xrefs). MI deleting thunks disambiguated: `CHelpDlg_ScalarDeletingDtor_thunk_Sub18` @ `0x004220b0` (`SUB ECX,0x18`), `…_Sub10` @ `0x004220d0` (`SUB ECX,0x10`), `CHelpDlg_AdjustorThunk04_Dtor` @ `0x004220c0`. Report: [round5_worker_05_report.md](./round5_worker_05_report.md).
