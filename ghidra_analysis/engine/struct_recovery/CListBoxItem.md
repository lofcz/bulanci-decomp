# CListBoxItem

## Status

**VERIFIED** — size `0x14` (20 bytes). List-row payload base for `CSessionItem`, level rows, and lobby chat lines (`widgets.md` §14).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Base ends before subclass fields | `0x0040e8b0` | `CSessionItem_Initialize` calls `CListBoxItem_ctor` then writes session dwords at `this+0x14..+0x20` |
| Subclass alloc `0x24` | `0x0040f380` | `CSessionList_AppendEnumSession`: `OperatorNewWithBadAlloc(0x24)` → `0x14` base + `0x10` tail |
| Ghidra layout | — | `get_struct_layout CListBoxItem` → **20 bytes** (round 3 task 19 merge) |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `pVftable` | `CListBoxItem_ctor@0x0040b640` → `0x47ff50`; overridden in `CSessionItem_Initialize@0x0040e8b0` |
| `0x04` | 4 | `dword` | `dwPad_04` | No write in `CListBoxItem_ctor@0x0040b640` or `CMenu::CListBoxItem_ctorWithAssoc@0x0040b6d0`; no read in `CLevelList_RenderItem@0x0040d490` / `CListBox_RenderItem@0x00405fc0` (aligns with `CItemInfo.dwPad_04`) |
| `0x08` | 4 | `dword` | `dwAssocOrNext` | `CListBoxItem_ctor@0x0040b640` `= 0`; `CMenu::CListBoxItem_ctorWithAssoc@0x0040b6d0` stores menu assoc at `+0x08` |
| `0x0c` | 4 | `dword` | `dwRowFlags` | `CListBoxItem_ctor@0x0040b640` `= 0`; `CLevelList_RenderItem@0x0040d490` / `CListBox_RenderItem@0x00405fc0` test `(item+0xc) & 1` |
| `0x10` | 4 | `dword` | `dwLabelStringHandle` | `CListBoxItem_ctor@0x0040b640` via `CDsStringAssignFromHandle`; `CListBoxItem_dtor@0x00404cb0` releases `param_1[4]` |

## Ghidra apply

```
get_struct_layout CListBoxItem → Size: 20 bytes
  pVftable, dwPad_04, dwAssocOrNext, dwRowFlags, dwLabelStringHandle
```

Round 3 task 19: deleted 1-byte `/CListBoxItem` placeholder, cloned former `CListBoxItemRow` layout onto `/CListBoxItem`, removed duplicate row type. `save_program bulanci.exe`.

Slice 13 (2026-05-30): re-verified `get_struct_layout` — **20 bytes**, fields unchanged; no doc/Ghidra delta required.

**R3 task 28 (2026-05-30):** `set_function_this_type(CListBoxItem *)` @ `0x0040b640` / `0x0040b6d0`. Decompile uses `pVftable`, `dwAssocOrNext`, `dwRowFlags`, `dwLabelStringHandle`; assoc ctor stores `pAssoc` at `+0x08`.

## UNK

| Field | What is proven | What is not |
|-------|----------------|-------------|
| `dwPad_04` (`+0x04`) | Slot exists between vftable and `+0x08`; never written in either list-row ctor | No consumer xref in `RenderItem` paths; padding / reserved per `CItemInfo` parent layout (`batch_12_followup_summary.md`) |
| `dwAssocOrNext` (`+0x08`) | Zeroed in default ctor; menu ctor writes assoc pointer | No read xref in list renderers — may be menu-only back-pointer |
