# Round 3 — Task 10 report

## Task

| Field | Value |
|-------|--------|
| **id** | 10 |
| **title** | Dedup CHelpView to CDSChained embed @ 0 |
| **types** | `CHelpView`, `CDSChained`, `CHelpDlg` |
| **addresses** | `CHelpView_ctor@0x004218a0`, `CHelpDlg_LoadHelpPage@0x00421c10`, `CDSChained_ResetChainCounters@0x0042beb0`, `CHelpDlg_ctor@0x00421e40` |
| **acceptance** | Prove or document UNK in `CHelpView.md` with func@addr evidence |

## Status

**DONE**

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| Alloc size `0x68` | `CHelpDlg_LoadHelpPage@0x00421c10` | `OperatorNewWithBadAlloc(0x68)` → `CHelpView_ctor` |
| Embedded chained init | `CHelpView_ctor@0x004218a0` | `CDSChained_InitWithRect(&this->chain, {0,0,0x212,0x1ea})`; vtables on `(this->chain).pVftable_*` |
| Help-specific flags | `CHelpView_ctor@0x004218a0` | `(this->chain).wViewFlags \|= 0x77f`; `(this->chain).wChainFlag46 \|= 1` |
| Release path uses chain primary vtable | `CHelpDlg_LoadHelpPage@0x00421c10` | `(**(code **)((int)(this->pHelpView->chain).pVftable_primary + 8))()` |
| Dialog enumerates help scripts | `CHelpDlg_ctor@0x00421e40` | Menu filter `*(entry+0xc) == 0x81c` (`CHelpScript`) |
| Chain counter band shared | `CDSChained_ResetChainCounters@0x0042beb0` | Writes `+0x40..+0x50` on any `CDSChained` base |
| Parity with history view shell | `CHistoryView_ctor@0x00422a70` | Same `CDSChained_InitWithRect` pattern; bbox bottom `0x1fe`, flags `0x67f`, no `wChainFlag46` tweak |

## Ghidra deltas

- **`get_struct_layout CHelpView`** — **104 bytes**; single field `chain` : `CDSChained` @ 0 (no flat duplicate prefix).
- **`get_struct_layout CHistoryView`** — same embed pattern (reference sibling).
- Decompile @ `0x004218a0` / `0x00421c10` already uses `this->chain` / `pHelpView->chain` (no struct mutation required this slice).
- **`save_program`** — not required (layout pre-applied; verification-only pass).

## Struct doc updates

| File | Change |
|------|--------|
| [CHelpView.md](./CHelpView.md) | **New** — size proof, layout, ctor deltas vs `CHistoryView`, consumers |
| [CHelpDlg.md](./CHelpDlg.md) | Removed stale UNK that `CHelpView` was missing from catalog |

## Remaining UNK

- `CHelpView` class id `0x81d` (2077) — registry COL not decoded.
- Why help ctor sets `wChainFlag46 \|= 1` while `CHistoryView` does not.
- Sibling dedup still open for `CMsgDialog` and other view shells (see batch 09 follow-up).
