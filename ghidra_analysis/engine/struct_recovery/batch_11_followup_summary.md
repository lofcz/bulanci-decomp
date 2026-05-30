# Struct recovery batch 11 follow-up (round 2)

**Prior:** `batch_11_summary.md`  
**Program:** `bulanci.exe`  
**Status:** **HANDOFF_PROGRESS** (follow-ups addressed; minor UNK remains)

## Actions taken

| Follow-up (prior summary) | Result |
|---------------------------|--------|
| Recover `CDSScript` base for `CHistoryScript` | Embedded `CDSScript` @ 0 in Ghidra; doc updated (`CHistoryScript.md`) |
| Recover shared `CDSChained` layout | Created `CDSChained` (`0x68`, 29 fields); new `CDSChained.md` |
| Map gap `0x40`–`0x4B` on views | Decomposed via `CDSChained_ResetChainCounters@0x0042beb0`: `dwChainRoot`, four `ushort` chain fields, `pParent`, `dwField_50` |
| `CHistoryView` Ghidra type | Recreated with `CDSChained chain` @ 0 (was missing / placeholder) |

## Ghidra deltas

- **`CDSChained`**: removed 1-byte placeholder → full `0x68` layout (chain header + bbox + chain vtables).
- **`CHistoryScript`**: `pCdsscriptBase[0x430]` → embedded **`CDSScript`** + tail fields unchanged at `0x430..0x440`.
- **`CHistoryView`**: new struct, single member `CDSChained chain` (104 bytes).
- **`save_program bulanci.exe`** — once at end.

## Evidence anchors (new)

- `CDSChained_ResetChainCounters@0x0042beb0` — writes `+0x40`, `+0x44..0x4a` (ushorts), `+0x4c`, `+0x50`.
- `CDSChained_ctor@0x004032d0` / `FUN_0040b560@0x0040b560` — header through `+0x64`, then reset.
- `CHistoryView_ctor@0x00422a70` — `CDSChained *` parameter, calls `FUN_0040b560`.

## Remaining UNK

- `CHistoryScript+0x438` sub-object semantics; `nNestedRefCount` role on history path.
- `CDSChained` dword/ushort semantic names for `dwField_08..64` and chain-flag bit meanings.
- COL / class IDs `0x800` / `0x801` not decoded.

## Deliverables

- `CDSChained.md` (new)
- `CHistoryScript.md`, `CHistoryView.md` (updated)
- This summary
