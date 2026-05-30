# Struct recovery batch 16/50

**Index:** 16 (`batches_50.json`)  
**Types:** `CSessionItem`, `CSessionList`  
**Protocol:** evidence-only (`AGENT_PROTOCOL.md`)

## Results

| Struct | Status | Size | Ghidra |
|--------|--------|------|--------|
| `CSessionItem` | VERIFIED | `0x24` | 8 named fields + `pad_04`; layout 36 B |
| `CSessionList` | PARTIAL | `0x7c` | Tail child ptrs at `+0x70..+0x78`; base `byte[0x70]` placeholder |

## Key evidence

- **CSessionItem:** `OperatorNewWithBadAlloc(0x24)` @ `CSessionList_AppendEnumSession` (`0x0040f380`); ctor copies DirectPlay fields to `+0x14..+0x20`; `CMenu_PickSession` reads all four into caller buffer.
- **CSessionList:** `OperatorNewWithBadAlloc(0x7c)` @ `CreateObject` (`0x0040e980`); stack `local_94[124]` in `CMenu_PickSession` (`0x00414170`); dialog builds listbox/static/join at `+0x70/74/78`.

## Deliverables

- `ghidra_analysis/engine/struct_recovery/CSessionItem.md`
- `ghidra_analysis/engine/struct_recovery/CSessionList.md`
- Program saved: `bulanci.exe`

## Follow-ups

- Replace `CSessionList.base_0x00` with recovered `CWindow` when that struct is verified in an earlier batch.
- Recover `CListBoxItem` (batch not in 16) to replace duplicated base fields inside `CSessionItem` documentation.
