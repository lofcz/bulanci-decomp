# Round 5 worker 11/50 — UI/dialog/widgets FUN_* band

## Task

| Field | Value |
|-------|-------|
| Worker | 11 / 50 |
| Mode | WRITE — evidence-only `rename_function_by_address` |
| Band | `0x00400000`–`0x00410000` (UI / dialog / widget `.text`) |
| Slice | `0x0040332c`–`0x0040384a` (chunk `0x51e` = `(0x10000/50)`) |
| Program | `bulanci.exe` |

## Status

**DONE** — slice had **0** `FUN_*` entry points; **2** ambiguous global symbols renamed with vtable / RTTI proof. `save_program bulanci.exe`.

## Slice inventory (16 entry points)

| Address | Symbol (after) | Evidence class |
|---------|----------------|----------------|
| `0x00403390` | `CDSView_GetClassTable` | `MOV EAX, 0x4b3bac` — CDSView meta |
| `0x004033a0` | `CDSView_GetDataKindStubZero` | returns 0 |
| `0x004033b0` | `CGaming_IsSchedulerSlotActive` | scheduler slot probe |
| `0x004033d0` | `CScroller_ctor` | `CScroller` construction |
| `0x00403470` | `CScroller_GetClassTable` | `MOV EAX, 0x4b3384` |
| `0x00403480`–`0x004034e0` | `CScroller_*` dtors / thunks | adjustor thunks @ `+0x68` / `+0x18` |
| `0x004034a0` | `CDSObject_ReleaseViaVtable_ThisMinus0x10` | shared IDSChained release |
| `0x004034d0` | `CScroller_GetTypeInfo` | `MOV EAX, 0x4b7c40` |
| `0x00403500` | **`CItemInfo_GetTypeInfo`** (was `CItemInfo::GetTypeInfo`) | returns `&DAT_004b33b8`; vtable `0x47faa4` slot 0 |
| `0x00403510` | `CDSObject_AssignIDSReferencedVftable` | `MOV [ECX], 0x47f6a8` |
| `0x00403520` | **`CBlackView_GetClassTable`** (was `GetClassTable`) | returns `&DAT_004b3428`; `CBlackView` vtable `0x47fb04` slot 0 |
| `0x00403530` | `CScrollBar_Render` | scroll widget blit (`CScrollBar.md`) |

## Ghidra deltas

| Old | New | Address | Evidence |
|-----|-----|---------|----------|
| `GetClassTable` | `CBlackView_GetClassTable` | `0x00403520` | Decompile `return &DAT_004b3428`; `vftable_methods.csv` `CBlackView;0047fb04;;0;00403520`; asm `MOV EAX,0x4b3428` |
| `CItemInfo::GetTypeInfo` | `CItemInfo_GetTypeInfo` | `0x00403500` | Decompile `return &DAT_004b33b8`; `CItemInfo.md` RTTI @ `0x4b33b8`; class reg `MOV ECX,0x4b33b8` @ `0x47b1d0` |

## Struct doc updates

- [CItemInfo.md](./CItemInfo.md) — RTTI table + Ghidra apply note

## Remaining UNK

- None in slice for `FUN_*` / ambiguous `GetClassTable` names.
- `FUN_00403240` @ `0x00403240` is **worker 10** slice (just below `0x40332c`) — not in scope.

## References

- [movie_cinema_views.md](../../gameplay/movie_cinema_views.md) — `CBlackView` @ `0x403520`
- [CScrollBar.md](./CScrollBar.md) — `CScrollBar_Render` @ `0x403530`
- [master_vtable_catalog.csv](../master_vtable_catalog.csv)
