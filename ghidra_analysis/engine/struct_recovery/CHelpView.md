# CHelpView

## Status

**VERIFIED** — `sizeof == 0x68`; layout is embedded `CDSChained chain` @ 0 with help-specific vtables, bbox, and flags in ctor. Chain band `0x40`–`0x50` semantics in [CDSChained.md](./CDSChained.md) via `CDSChained_ResetChainCounters@0x0042beb0`.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CHelpView) == 0x68` | `CHelpDlg_LoadHelpPage@0x00421c10` | `OperatorNewWithBadAlloc(0x68)` then `CHelpView_ctor` |
| Ctor parameter / return | `CHelpView_ctor@0x004218a0` | `CHelpView * __fastcall CHelpView_ctor(CHelpView *this)`; calls `CDSChained_InitWithRect(&this->chain, …)` |
| No tail past chained shell | layout | Same 104-byte object as `CHistoryView`; no fields beyond `CDSChained` |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 0x68 | `CDSChained` | `chain` | `CHelpView_ctor@0x004218a0`; field map in `CDSChained.md` |

Help-specific overrides (same offsets as `CDSChained`):

| Offset | Evidence |
|--------|----------|
| `0x00`, `0x04`, `0x10`, `0x18` | Help vtables in `CHelpView_ctor@0x004218a0` (`g_pCHelpView_vftable_*`) |

**R5 worker 05:** Ghidra labels `g_pCHelpView_vftable_primary` @ `0x00482a94`, `g_pCHelpView_vftable_IDSEventHandler` @ `0x00482a4c`; slot 0 `CHelpView_GetClassTable@0x00421900`. See [round5_worker_05_report.md](./round5_worker_05_report.md).
| `0x14` | `wViewFlags \|= 0x77f` (vs `0x67f` on `CHistoryView`) |
| `0x20`–`0x2C` | bbox `{0,0,0x212,0x1ea}` via `CDSChained_InitWithRect` (530×490; history uses `0x1fe` bottom) |
| `0x46` | `wChainFlag46 \|= 1` after init (help-only; history ctor does not set) |

## Consumers

| Function | Address | Role |
|----------|---------|------|
| `CHelpDlg_LoadHelpPage` | `0x00421c10` | Alloc view, `CDSScript_SetBoundParentView`, `CDSView__AddChild`; release via `(pHelpView->chain).pVftable_primary` |
| `CHelpDlg_ctor` | `0x00421e40` | Enumerates menu `classId == 0x81c` (`CHelpScript`); page 0 via `LoadHelpPage` |
| `CHelpView_GetClassTable` | `0x00421900` | Returns `&DAT_004b38c0`; static init `HandleClassRegister(…, classId 0x81d, …)` @ `0x0047bf60` |

## Class registry (COL)

| Field | Value | Evidence |
|-------|-------|----------|
| Meta | `&DAT_004b38c0` | `CHelpView_GetClassTable@0x00421900` |
| **classId** | **0x81d** (2077) | Static init `HandleClassRegister` @ `0x0047bf60` (`FUN_0047e380` atexit tail) |
| Factory | `CreateObject@0x00421ba0` | `OperatorNew(0x68)` + `CHelpView_ctor`; returns `&this->chain` |
| Sibling script | `CHelpScript` **0x81c** @ `0x0047bed0` | `CHelpDlg_ctor` menu filter `*(entry+0xc)==0x81c` |

Help/history view/script ids are sequential in the **0x81x** band (`CHistoryScript` **0x802**, `CHistoryView` **0x80f** @ `0x0047c080` — see [CHistoryView.md](./CHistoryView.md)).

## Ghidra apply

```
get_struct_layout CHelpView → size 0x68 (104), chain: CDSChained @ 0
```

**Round 3 task 10 (2026-05-30):** Verified embedded `CDSChained` (no duplicate flat prefix fields). Decompile uses `(this->chain).*`; `CHelpDlg.pHelpView` typed `CHelpView *`.

**Round 4 task 10 (2026-05-30):** Documented classId **0x81d** + factory; `wChainFlag46 \|= 1` = **bit 0 keyboard-focusable** (same as `CWindow_BuildAt@0x004055b3`, consumed by `CDSView_AcquireKeyboardFocus` `test byte [this+0x46],1`). Decompiler comments @ `0x004218a0` / `0x004218eb` / `0x00421900` / `0x00421ba0`; `save_program`.

**R5 worker 44 (2026-05-30):** `wViewFlags \|= 0x77f` vs `CHistoryView` `0x67f` — delta **bit 0x100** matches `CWindow_BuildAt@0x00405560` container preset (`0x77f` OR mask); no isolated `TEST` on `wViewFlags&0x100` in `.text` (OR-only bundle bit). Plate @ `0x004218eb`.

## UNK

- COL human-readable type name for classId **0x81d** (registry wired; script resource tag not recovered).
