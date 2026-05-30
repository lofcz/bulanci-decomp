# Struct recovery batch 18/50

**Index:** 18 (`batches_50.json`)  
**Types:** `CSessionItem`, `CSessionList`  
**Program:** `bulanci.exe`  
**Evidence:** Ghidra MCP decompile, disassembly (`PUSH` alloc sizes), ctor/consumer xrefs only

## Results

| Struct | Status | Size | Notes |
|--------|--------|------|-------|
| `CSessionItem` | VERIFIED | `0x24` | Extends `CListBoxItem` (`0x14`); four session dwords at `+0x14..+0x20` |
| `CSessionList` | VERIFIED | `0x7c` | Dialog prefix + `pSessionListBox` / `pCaptionStatic` / `pJoinButton`; typed prototypes (slice 18) |

## Key evidence

- **CSessionItem alloc:** `PUSH 0x24` in `CSessionList_AppendEnumSession@0x0040f380` (`0x0040f3a1`).
- **CSessionItem fields:** `CSessionItem_Initialize@0x0040e8b0` (vtable `0x4808b8`, dwords from enum buffer); `CMenu_PickSession@0x00414170` reads `+0x14..+0x20` from picked row.
- **CSessionList alloc:** `PUSH 0x7c` in `CreateObject@0x0040e980` (`0x0040e9a1`); stack `local_94[124]` in `CMenu_PickSession@0x00414170`.
- **Not struct size:** `0xf4` in `CSessionList_BuildDialog` is dialog height for `CWindow_BuildAt`, not `sizeof(CSessionList)`.

## Ghidra actions

- [x] `CSessionItem` / `CSessionList` layouts (`get_struct_layout`: 36 and 124 bytes)
- [x] `DPEnumSessionInfo` / `CDPEnumSessionInfo` sparse enum buffer types
- [x] Prototypes: `CSessionList_BuildDialog`, `GetPick`, `AppendEnumSession`, `SelectBySessionGuid`, `SetStatusFromStringHandle`; `CSessionItem_Initialize`; `CMenu_PickSession` local `CSessionList`
- [x] `save_program bulanci.exe` (slice 18)

## Artifacts

- `ghidra_analysis/engine/struct_recovery/CSessionItem.md`
- `ghidra_analysis/engine/struct_recovery/CSessionList.md`
- `ghidra_analysis/engine/struct_recovery/CDPEnumSessionInfo.md`
- `ghidra_analysis/engine/struct_recovery/DPEnumSessionInfo.md`
- `ghidra_analysis/engine/struct_recovery/batch_18_followup_summary.md`

## Follow-ups

- `CSessionList_SelectBySessionGuid` / `SetStatusFromStringHandle`: `__thiscall` ECX still shows `CGame *` in decompile (Ghidra API limitation); prototypes/plate comments document `CSessionList *`; scheduler case 1 call sites annotated @ `0x004162bd` / `0x0041630d` (agent todo 19). Runtime `this` during EnumSessions is lobby `CSessionList*` via `CGame::pActiveCGaming`.
- Dialog prefix padding `+0x40..+0x67` and global `CWindow` 105 B vs proven `0x70` prefix (cross-batch).
- Cancel button: child only, no `this` member.
