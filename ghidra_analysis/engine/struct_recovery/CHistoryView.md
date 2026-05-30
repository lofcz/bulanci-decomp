# CHistoryView

## Status

**VERIFIED** — `sizeof == 0x68`; layout is embedded `CDSChained chained` with history-specific vtables/`wViewFlags` in ctor. Gaps `0x40`–`0x4B` / `0x50` mapped in `CDSChained.md` via `CDSChained_ResetChainCounters@0x0042beb0`.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CHistoryView) == 0x68` | `0x00422f00` | `CreateObject` → `OperatorNewWithBadAlloc(0x68)` then `CHistoryView_ctor` |
| Same size in dialog path | `0x00422f70` | `CHistoryDlg_LoadHistoryPage` → `OperatorNew(0x68)` → `CHistoryView_ctor` |
| Ctor parameter type | `0x00422a70` | `CHistoryView_ctor(CDSChained *param_1)` |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|-------|------|------|------|----------------------|
| 0x00 | 0x68 | `CDSChained` | `chain` | `CDSChained::FUN_0040b560@0x00422a70`; see `CDSChained.md` for field map |

History-specific overrides (same offsets as `CDSChained`):

| Offset | Evidence |
|--------|----------|
| `0x00`, `0x04`, `0x10`, `0x18` | History vtables written in `CHistoryView_ctor@0x00422a70` |
| `0x14` | `wViewFlags \|= 0x67f` |
| `0x20`–`0x2C` | bbox `{0,0,0x212,0x1fe}` via `FUN_0040b560` |
| `0x4C` on **parent** | `CHistoryView_OnEvent@0x00422670` reads `*(parent+0x4c)` for `CMenu_EnableBackgroundState` on movie events `0xf0`/`0xf1` (`(this->chain).pParent` is parent view) |

## Ghidra apply

```
get_struct_layout CHistoryView → size 0x68 (104), chain: CDSChained @ 0
```

Batch 09 / slice 11: `CDSChained` embedded as `chain`; `CHistoryView_ctor@0x00422a70`; `CHistoryView_OnEvent` uses `(this->chain).pParent` then parent’s `+0x4c` menu pointer.

## Follow-up

- **Slice 11 (2026-05-30):** Ghidra struct verified; ctor renamed `CHistoryView_ctor`; decompile uses `this->chain` member.

## UNK

- Class ID `0x801` (2049) — `CHistoryView_GetClassTable@0x00422ad0`, COL not decoded.
- No separate tail beyond `CDSChained`; distinguish from larger `CWindow` dialogs (`0x70`+) in a future batch.
