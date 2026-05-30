# Round 5 — worker 40 report (CStartGame2 lobby shell UNKs)

## Task

| Field | Value |
|-------|-------|
| **worker** | 40 / 50 |
| **mode** | WRITE |
| **scope** | Close R4 **UNK** items on stack lobby shell `CStartGame2` (`0xdc`) |
| **types** | `CStartGame2`, `CWindow`, `CLevelList`, `CDSUpdatedItem`, `CNumEdit` |
| **addresses** | `0x004104f0`, `0x0040d520`, `0x0040d3f0`, `0x0040d6b0`, `0x0040d570`, `0x0040f610`, `0x00480ce8` |
| **acceptance** | Evidence for duplicate-serial / flag / `0xDF` / tail pads; Ghidra hygiene; update `CStartGame2.md`; `save_program` |

## Status

**DONE** — duplicate-serial hook and tail flags resolved or downgraded with proof; `0xDF` decompile fixed via `CLevelList *` this on `SelectPlayerByName`; `pad_be` confirmed unused.

## Evidence

| Claim | Func @ addr | Evidence |
|-------|-------------|----------|
| `+0x40` / `+0x4c` are **CWindow** chain band, not separate lobby fields | `CWindow.md`, `UpdateDuplicateSerialWarning@0x0040d520` | Disasm `CMP [ESI+0x40]` / `MOV byte [ESI+0x4c]`; IDA `this+64` / `this+76` ≡ `+0x40` / `+0x4c` on 112 B prefix |
| Duplicate-serial UI hook is **vtable-only** | `0x00480ce8` slot 4 | `get_xrefs_to 0x0040d520` → **DATA** only (`IDSEventHandler` @ `this+0x10`); no store of non-null `+0x40` in `.text` |
| Duplicate-serial string unused elsewhere | `0x0040d520` | Literal `L"Cannot start the game - duplicated serial numbers !"` @ `0x004815d0`; sole PUSH xref is `UpdateDuplicateSerialWarning` |
| Duplicate **player names** use modal, not serial hook | `Tick_CheckDuplicateNames@0x0040f610` | On clash → `CMsgDialog_ShowModalFromStringHandle@0x0040f2c0`; OK (`AX==-0x7ffc`) → `PostMessage(this+0x10, 0x100, 0x8004)`; focus `pSlotNameEdit[slot]` |
| `SelectPlayerByName` **`this` = `CLevelList*`** | `OnCustomMsg@0x0040d6b0` case `0xDF`, `SelectPlayerByName@0x0040d3f0` | IDA `sub_40D3F0(*(int **)(this + 0xb4), …)`; disasm `EDI=[ECX]` list, walk `[EDI+0xac]` / count `listViewer.nItemCount` after `set_function_this_type` |
| Net name for `0xDF` from parent stash | `OnCustomMsg@0x0040d6b0` | `CDsStringAssignFromHandle(..., pParentState + 0xcc)` — `CMenu` modal string stash band (not re-derived here) |
| `pad_bc` / `pad_bd` lobby-only | `CStartGame2_ctor@0x004104f0` | `MOV byte [ESI+0xbc],0` / `[+0xbd],0` @ epilogue; **no** other `CStartGame2` consumer |
| Min/max on **`CNumEdit`**, not lobby | `CNumEdit_BuildAt@0x00406000` | `OperatorNew(0xc4)`; stores `param_7` low/high @ **`+0xbc` / `+0xbd`** on **num edit** object; vtable `CNumEdit` |
| `pad_be[30]` unused | — | No `.text` consumer of `CStartGame2+0xbe..0xdb` in export pass |

## Ghidra deltas

- `modify_struct_field` `CStartGame2`: `pDuplicateSerialWarning` → **`dwChainHead_40`** (`dword`); `bDuplicateSerialPending` → **`bParentByte0_serialPending`** (`byte`)
- `modify_struct_field` `bLobbyFlag_bc` / `bLobbyFlag_bd` → **`pad_bc`** / **`pad_bd`**
- `set_function_this_type` **`CLevelList *`** @ `CStartGame2_SelectPlayerByName@0x0040d3f0`
- `set_function_prototype` **`void __thiscall CStartGame2_UpdateGamemodeCaption(CStartGame2 *)`** @ `0x0040d570`
- `set_decompiler_comment` @ `0x0040d520`, `0x0040d6b0`, `0x004104f0`
- `force_decompile` `OnCustomMsg`, `SelectPlayerByName` — shows `CLevelList::…SelectPlayerByName(this->pLevelList, name)` and `listViewer.nItemCount` / `pItems` walk
- `save_program bulanci.exe`

## Struct doc updates

- [CStartGame2.md](./CStartGame2.md) — UNK section closed/updated; layout notes for `+0x40..+0x4c`, tail pads, `0xDF` path

## Remaining UNK (this slice)

- **Duplicate-serial hook** — no assignment of warning widget or pending byte found; likely dead vestige of DirectPlay serial check (contrast live duplicate-**name** tick path).
- **`CStartGame2+0xac` on `CLevelList`** inside `SelectPlayerByName` — decompile now uses `listViewer.pItems`; Ghidra struct field names on `CListBox`/`CListViewer` still partial (see [CLevelList.md](./CLevelList.md)).
- **`pParentState+0xcc`** stash field name on `CMenu` — consumer only; full `CMenu` map deferred.
