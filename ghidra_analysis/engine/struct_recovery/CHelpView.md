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
| `0x14` | `wViewFlags \|= 0x77f` (vs `0x67f` on `CHistoryView`) |
| `0x20`–`0x2C` | bbox `{0,0,0x212,0x1ea}` via `CDSChained_InitWithRect` (530×490; history uses `0x1fe` bottom) |
| `0x46` | `wChainFlag46 \|= 1` after init (help-only; history ctor does not set) |

## Consumers

| Function | Address | Role |
|----------|---------|------|
| `CHelpDlg_LoadHelpPage` | `0x00421c10` | Alloc view, `CDSScript_SetBoundParentView`, `CDSView__AddChild`; release via `(pHelpView->chain).pVftable_primary` |
| `CHelpDlg_ctor` | `0x00421e40` | Enumerates menu `classId == 0x81c` (`CHelpScript`); page 0 via `LoadHelpPage` |
| `CHelpView_GetClassTable` | `0x00421900` | Returns `&DAT_004b38c0`; static init `HandleClassRegister(…, classId 0x81d, …)` @ `0x0047bf60` |

## Ghidra apply

```
get_struct_layout CHelpView → size 0x68 (104), chain: CDSChained @ 0
```

**Round 3 task 10 (2026-05-30):** Verified embedded `CDSChained` (no duplicate flat prefix fields). Decompile uses `(this->chain).*`; `CHelpDlg.pHelpView` typed `CHelpView *`.

## UNK

- Class ID **2077** (`0x81d`) — `CHelpView_GetClassTable` / `DAT_004b38c0`; COL not decoded (sibling `CHistoryView` uses `0x801`).
- Semantic purpose of `wChainFlag46 \|= 1` on help pages (dialog `CPauseDlg` sets `0x2c` on `CWindow` builds only).
