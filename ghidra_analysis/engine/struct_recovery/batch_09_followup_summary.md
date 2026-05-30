# Struct recovery follow-up — batch 09/50

**Prior:** `batch_09_summary.md` (`CHistoryScript`, `CHistoryView`)  
**Status:** **FOLLOWUP_COMPLETE** (3/3 prior follow-ups addressed; minor UNK remain)

## Actions taken

| Follow-up (prior summary) | Result |
|---------------------------|--------|
| Recover `CDSScript` base → upgrade `CHistoryScript` | **Done** — `CHistoryScript.script` is embedded `CDSScript` @ 0; tail @ 0x430 unchanged. Ctor prototype fixed @ `0x004226c0`; decompile shows `this->nNestedRefCount`, `pBoundView`, `InstallOpcodeTable(&this->script, 0x2d, …)`. |
| Recover `CDSChained` for view dedup | **Done** — rebuilt `CDSChained` (was 1-byte placeholder) to **0x68** / 29 fields; `CHistoryView` wraps `CDSChained chained` @ 0. |
| Map gap `0x40`–`0x4B` via `ResetChainCounters` | **Done** — `CDSChained_ResetChainCounters@0x0042beb0`: `dwChainRoot@0x40`, four `ushort` @ `0x44..0x4a`, `pParent@0x4c`; `dwField_50@0x50` also zeroed. |

## Ghidra deltas

- `modify_struct_field` `CHistoryScript.pCdsscriptBase` → `CDSScript script`
- `delete_data_type` + `create_struct` `CDSChained` (104 bytes)
- `create_struct` `CHistoryView` (`CDSChained` @ 0)
- `set_function_prototype` `CHistoryScript::ctor@0x004226c0`, `CHistoryView_ctor@0x00422a70`
- `save_program bulanci.exe` (×2)

## Struct status after follow-up

| Struct | Was | Now |
|--------|-----|-----|
| `CHistoryScript` | PARTIAL | **VERIFIED** (size 0x444; `CDSScript` + MI tail) |
| `CHistoryView` | PARTIAL | **VERIFIED** (alias of `CDSChained` layout @ 0x68) |
| `CDSChained` | placeholder | **VERIFIED** (shared view header) |

## Remaining UNK

- `CHistoryScript` `+0x438` sub-object / `FUN_00434250` cleanup semantics.
- `CDSChained` dword/ushort semantics at `+0x08..+0x1c`, `+0x30..+0x3c`, `+0x5c..+0x64` (only zero-init proven).
- Dedup sibling structs (`CHelpView`, `CMsgDialog`, …) to `CDSChained` in Ghidra — layout doc ready, per-class vtables still separate.
- Class IDs `0x800` / `0x801` COL tables not decoded.

## Docs updated

- `CHistoryScript.md`, `CHistoryView.md`, new `CDSChained.md`
