# Round 5 — Worker 6 report

## Task

| Field | Value |
|-------|--------|
| **worker** | 6 / 50 |
| **mode** | WRITE |
| **scope** | `FUN_*` **CDSView / CWindow / CDSChained** render + input @ `0x00402000`–`0x0042c000` |
| **deliverable** | Evidence table per rename + `round5_worker_06_results.jsonl` |

**Note:** Primary vtable slots in this band are largely named already ([CDSView_vftable.md](./CDSView_vftable.md)). This pass targets remaining `FUN_*` helpers on widgets, scrollers, static text, radio, app dirty-rect flush, and modal dialog shell.

## Status

**PARTIAL** — **20** `FUN_*` renamed with xref/decompile proof; **50** remain `FUN_*` (gameplay `CBulanek` cluster, EH unwind, vector memmove, generic MSVCRT). `save_program bulanci.exe`.

## Inventory

| Metric | Count |
|--------|------:|
| `FUN_*` in band @ start | 86 |
| Renamed (this worker) | 20 |
| Remaining `FUN_*` | 50 |
| Named `CDSView`/`CWindow`/`CDSChained` in band (pre-pass) | 106 symbols |

## Proof table — renames (evidence per row)

| Address | Was | New name | Evidence (func @ addr) |
|---------|-----|----------|-------------------------|
| `0x00404290` | `FUN_00404290` | `CListViewer_IndexToItemRect` | Linear index → cell pixel rect from `nColumnWidth_px`/`nRowHeight_px` and `dwLayoutFlags&1`; callers `CListViewer_ScrollToItem@0x00404348`, `CListViewer_RenderItems@0x00406dcb`, `CListViewer_AddItem@0x00405e5e`, `CChatList_ScrollToItem@0x00405ccf` |
| `0x004058f0` | `FUN_004058f0` | `CChatList_SetScrollbarValues` | `__thiscall CChatList*`; if child scrollbars @ `+0x90`/`+0x94` have `+0x44&1`, `CScrollBar_SetValue`; sole caller `CChatList_ScrollToItem` |
| `0x00405370` | `FUN_00405370` | `CStaticText_SetLabelFromStringHandle` | `CDsStringAssignFromHandle` → `param_1[0x20]`; optional `TextShaper_LayOutAndRender` when `+0x6c&0x10`; primary vtable `+0x20`/`+0x24` (adapt + invalidate); callers `CSessionList_SetStatusFromStringHandle`, `CSetupDlg_UpdateVolumeLabel`, `CPanel_Render`, `CGaming_SetTimerLabelFromStringHandle` |
| `0x00405440` | `FUN_00405440` | `CStaticText_SetLayoutAndInvalidate` | Stores layout @ `+0x70/+0x74`; same text-shaper + vtable `+0x20`/`+0x24` path; caller `CKeybShow_Build` on static text after `BuildAtAuto` |
| `0x004075f0` | `FUN_004075f0` | `CRadio_AddOption` | `CDSView__SetSize`, `CIntListInsertSortedOrAppend` @ `+0x6c`; `TextShaper` when `+0x80&0x10`; callers `CStartGame1_BuildUi`, `CGameTypeDlg_BuildUi` (16×) |
| `0x004096d0` | `CScore_RenderHighScoreColumn` | `CStaticText_CreateAndAddChild` | `OperatorNew(0x98)` → `CStaticText_BuildAt` → `CDSView__AddChild`; scoreboard row helper (prior name misleading) |
| `0x0040f2c0` | `FUN_0040f2c0` | `CMsgDialog_ShowModalFromStringHandle` | Stack `CMsgDialog` `0x7c`, `CDSView_DoModal`, `CWindow_dtor`; callers `CDSException` dispatch `FUN_00401a70` |
| `0x00404890` | `FUN_00404890` | `CDSView_UnwindDtor_StringVec9` | EH: `_eh_vector_destructor_iterator_` 9×4 @ `+0x80`, `CDSView_dtor`; callers `CScrollBar_vDtor@0x00406433`, `CVolume_dtor@0x0040fae7`, `Unwind@00475ff0` |
| `0x004040c0` | `FUN_004040c0` | `CScroller_ResetScrollSchedulerSlot` | `CScroller+0x68` scheduler slot 0 ack + `+0x98` revision bump; caller `CScroller_OnViewEvent` |
| `0x00404110` | `FUN_00404110` | `CScroller_OnViewEvent` | `CDSView_EmptyHook27` then event kind **7**: sync `nScrollX_px`/`nScrollY_px` from scrollbar `+0xb0` @ `+0x90`/`+0x94` |
| `0x00405c40` | `FUN_00405c40` | `CScroller_ClearContent` | `CListViewer_ClearSelectedItems`, release child chain, zero content extents, `CScroller__UpdateScrollbars` |
| `0x004069b0` | `FUN_004069b0` | `CScroller_OnKeyUp` | When scrollbars enabled, `'%'`/`'`' → `CScrollBar_SetDragMode` for bar index 1 or 5 |
| `0x00427710` | `FUN_00427710` | `CGaming_SetTimerLabelFromStringHandle` | `CGaming+0xb8` widget; string assign → `CStaticText_SetLabelFromStringHandle`; callers round-timer tick paths |
| `0x0042a330` | `FUN_0042a330` | `CBulanci_ResizeClientAndDisplayMode` | `CDSView__SetSize`, `IDirectDraw` mode enum fallback 6→5→4, `CDSApp_CreateDirectInputDevice`, `RebuildBackBufferSurface`, vtable `+0x24` invalidate; callers `CBulanci_OnCreate`, windowed toggle |
| `0x00429a40` | `FUN_00429a40` | `CDSApp_ReleaseBackBufferAndSurface` | `CDSBackBuffer_FreeImageMember(app+0x7c)`, release `+0x78`, mode fields `+0xdc/+0xe0`; bracket in `CBulanci_ResizeClientAndDisplayMode` |
| `0x00429a80` | `FUN_00429a80` | `CDSApp_RefreshBlitDestRect` | Windowed: `GetClientRect`/`ClientToScreen` → `+0xcc`; else copy bbox `+0x20..+0x2c`; caller resize helper |
| `0x00429af0` | `FUN_00429af0` | `CDSApp_CreateDirectInputDevice` | `app.field_0x74` vtable `+0x18` property block → store `+0x78` |
| `0x0042bae0` | `FUN_0042bae0` | `CDSApp_FlushDirtyRects` | Walk dirty rect lists `+0x254`/`+0x264`, anim blit queue, `IDirectDrawSurface` vtable `+0x14` BitBlt |
| `0x0042bbe0` | `FUN_0042bbe0` | `CDSApp_FlushDirtyRectsIfFlagged` | If `app+0x78` && byte `+0x275`, call `CDSApp_FlushDirtyRects` |
| `0x0042afd0` | `FUN_0042afd0` | *(unchanged — BLOCKED below)* | Partial `CDSApp` ctor prefix; overlaps `CDSApp_ctor@0x0042b170` naming policy |

## Proof table — remaining `FUN_*` (no rename)

| Address | Disposition | Reason |
|---------|-------------|--------|
| `0x00403240` | BLOCKED | `uint32` overlapping memmove; no class |
| `0x00404180`, `0x00404240`, `0x004044f0` | BLOCKED | Scroller/list EH or tiny helpers — need sole-class caller proof |
| `0x004045d0`, `0x004045f0` | OUT_OF_SCOPE | MSVCRT small-string / `free` unwind |
| `0x00404c80`, `0x00405280`, `0x00405bc0` | BLOCKED | Unverified widget helpers |
| `0x00407020` | BLOCKED | Insufficient xref class proof |
| `0x0040ab90`–`0x0040ad10` | BLOCKED | `CDSView` scalar-dtor / video-player EH thunks |
| `0x0040b540` | BLOCKED | Generic `IDSReferenced` release in `Catch@` blocks only |
| `0x004165b0`–`0x0041ed10` | OUT_OF_SCOPE | `CBulanek` / `CDeath` / combat gameplay (not view shell) |
| `0x00416700` | OUT_OF_SCOPE | HUD ammo strip via `FUN_00427c90` when `CGaming` panel set |
| `0x00427c30`, `0x00427c90` | BLOCKED | HUD render — gameplay overlay |
| `0x00429880`–`0x00429bd0` | BLOCKED | Mix of app init stubs; only subset renamed where resize chain proven |
| `0x0042a070`, `0x0042a130` | BLOCKED | 16-byte vector binary-search / memmove engine helpers |
| `0x0042a550`, `0x0042a590`, `0x0042a9c0` | BLOCKED | DirectX HRESULT helpers — need isolated proof |
| `0x0042ae40`, `0x0042ae50` | BLOCKED | `CDSApp` dirty-rect vector EH / insert helpers |
| `0x0042afd0` | BLOCKED | Duplicate partial ctor vs `CDSApp_ctor` |
| `0x0042b3ae`, `0x0042b3d0`, `0x0042b48c` | BLOCKED | `CDSApp` shutdown / registry — not render-input |

*(Full list of 50 remaining addresses matches Ghidra export after this pass.)*

## Ghidra deltas

1. `rename_function_by_address` — 20 symbols (table above)
2. `set_function_this_type` — `CRadio *` @ `0x004075f0`; confirmed `CListViewer *` @ `0x00404290`, `CChatList *` @ `0x004058f0`
3. `save_program bulanci.exe` (×2)

## Struct doc updates

- [CDSView.md](./CDSView.md) — R5 worker 6 cross-link
- [CWindow.md](./CWindow.md) — R5 worker 6 cross-link

## Remaining UNK (this worker scope)

| Item | Reason |
|------|--------|
| 50 `FUN_*` in band | See table — gameplay, EH, CRT, or engine vectors without class export |
| `CStaticText_SetLayoutAndInvalidate` `this` type | Ghidra types receiver as `CStartGame2` at one call site; shared CDSView static-text layout |
| `CDSApp_FlushDirtyRects` `param_1` | Decompiler uses `CBulanci *`; body is `CDSApp` dirty-rect fields — needs `CDSApp *` prototype pass |
| Primary vtable `FUN_*` | Slots @ `0x004028a0`–`0x0042d0b0` already named; no slot gaps in `0x0047f954` |

## Cross-links

- [CDSView_vftable.md](./CDSView_vftable.md) — slot map (pre-named)
- [CDSChained.md](./CDSChained.md) — chain band @ `+0x40..+0x54`
- [round5_worker_12_report.md](./round5_worker_12_report.md) — adjacent band `0x00410000`–`0x00428000` (menus/dialogs)
