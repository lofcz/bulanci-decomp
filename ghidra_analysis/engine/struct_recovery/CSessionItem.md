# CSessionItem

## Status

**VERIFIED** — size `0x24` (36 bytes); extends **`CListBoxItem`** (`0x14`) + four session dwords at `+0x14..+0x20` (see `CListBoxItem.md`).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Heap allocation `0x24` | `0x0040f380` | `_Globals::CSessionList_AppendEnumSession`: `OperatorNewWithBadAlloc(0x24)` before `CSessionItem_Initialize` |
| Object span ends at `+0x24` | `0x0040e8b0` | `CSessionItem_Initialize` writes last field at `this+0x20` (4 bytes) |
| Ghidra struct size | — | `get_struct_layout` → **36 bytes** after apply |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `vftable` | `CListBoxItem` base — `CListBoxItem_ctor@0x0040b640` → `0x47ff50`; `CSessionItem_Initialize@0x0040e8b0` → `0x4808b8` (Ghidra field name `vftable`) |
| `0x04` | 4 | `dword` | `dwField_04` | `CListBoxItem` base — not written in ctor (see `CListBoxItem.md`) |
| `0x08` | 4 | `dword` | `pAssocOrNext` | `CListBoxItem_ctor@0x0040b640` `= 0` |
| `0x0c` | 4 | `dword` | `rowFlags` | `CListBoxItem_ctor@0x0040b640` `= 0`; `CLevelList_RenderItem@0x0040d490` bit 0 |
| `0x10` | 4 | `dword` | `labelStringHandle` | `CListBoxItem_ctor@0x0040b640` via `CDsStringAssignFromHandle(this+0x10, …)` |
| `0x14` | 4 | `uint` | `hostIp` | `CSessionItem_Initialize@0x0040e8b0` ← `param_1+0x08`; read `CMenu_PickSession@0x00414170`, `CSessionList_GetPick` consumers |
| `0x18` | 4 | `uint` | `port` | `CSessionItem_Initialize@0x0040e8b0` ← `param_1+0x0c`; read `CMenu_PickSession@0x00414170` |
| `0x1c` | 4 | `uint` | `playerCount` | `CSessionItem_Initialize@0x0040e8b0` ← `param_1+0x10`; read `CMenu_PickSession@0x00414170` |
| `0x20` | 4 | `uint` | `sessionFlags` | `CSessionItem_Initialize@0x0040e8b0` ← `param_1+0x14`; read `CMenu_PickSession@0x00414170` |

## DirectPlay enumeration input

See [DPEnumSessionInfo.md](./DPEnumSessionInfo.md) (live prototype) and [CDPEnumSessionInfo.md](./CDPEnumSessionInfo.md) (padded doc view). `CSessionItem_Initialize@0x0040e8b0` uses `DPEnumSessionInfo *sessionInfo` in Ghidra decompile (not full `DPSESSIONDESC2`).

## Ghidra apply

```
get_struct_layout CSessionItem → Size: 36 bytes
```

Fields applied batch 16; base names aligned to `CListBoxItem` in batch 16 follow-up. Canonical `CListBoxItem` struct (20 B) merged from `CListBoxItemRow` in round 3 task 19. `save_program bulanci.exe`.

## UNK

- `+0x04` (`dwField_04`): list-row chain slot (see `CListBoxItem.md`).
- `CDPEnumSessionInfo` bytes `+0x00..+0x07` and `+0x18..+0x2f` (no reads in `CSessionItem_Initialize`).
- `CSessionList_SelectBySessionGuid@0x0040c570` compares **16 bytes** at `item+0x14` (four dwords: IP, port, count, flags) — not a separate GUID field.
