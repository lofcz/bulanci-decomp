# Pass R4 — CStartGame2 report

## Task

| Field | Value |
|-------|-------|
| **scope** | Map `CStartGame2` lobby dialog: layout, `CGameCounter` / `CGameTypeDlg` relationship, bitmap refs, menu/modal flow |
| **types** | `CStartGame2`, `CWindow`, `CDSUpdatedItem`, `CGameTypeDlg`, `CGameCounter`, `CColorSet`, `CBulPicture` (downstream drawable) |
| **addresses** | `0x004104f0`, `0x00414790`, `0x0040fc20`, `0x0040d6b0`, `0x0040d7e0`, `0x004102d0`, `0x0040d930`, `0x0040f610`, `0x0040d520` |
| **inputs** | [CGameCounter.md](./CGameCounter.md), [CGameTypeDlg.md](./CGameTypeDlg.md), [CBulPicture.md](./CBulPicture.md), `main_menu.md` §4, `post_match_lobby.md` |

## Status

**DONE** — **`0xdc`** layout applied in Ghidra; slot pointer arrays and lobby tail typed; ctor / command / custom-message handlers decompile with `CStartGame2 *` this. **`CGameTypeDlg`** documented as **stack modal** (cmd `0xDA`); **`CGameCounter`** confirmed **out of scope** for this object (loading HUD only).

## Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Stack size `0xdc` | `CMenu_ShowLobby@0x00414790` | `CStartGame2_ctor(local_dc, param_1)` + `CStartGame2_dtor` |
| `CWindow` 760×560 @ (230,50) | `CStartGame2_ctor@0x004104f0` | `CWindow_BuildAt(this, 0xe6, 0x32, 0x2f8, 0x230, 0)` |
| Scheduler facet | `CStartGame2_ctor@0x004104f0` | `CDSUpdatedItem_ctor(&this->updated)`; admin `Scheduler_RegisterEventSlot(&this->updated,0,500,7)` |
| Slot name / pillar caches | `CStartGame2_ctor`, `OnCustomMsg@0x0040d6b0` | `pSlotNameEdit[slot]` @ `+0x88`; `pSlotColorSet[slot]` @ `+0x98` (cases `0xDD`/`0xDE`) |
| Level list on parent names | `CStartGame2_ctor@0x004104f0` | Loop `parent+0xc4` count, `parent+0xbc` → `CGame_FindResourceByName` → `CLevelList_AddItem` |
| Back / Start cmds | `main_menu.md`, ctor | Back `pBtnBack` cmd `0x8002`; Start child cmd `0x8003` |
| Duplicate-serial UI | `UpdateDuplicateSerialWarning@0x0040d520` | Disasm `[ESI+0x40]` widget ptr, `[ESI+0x4c]` pending byte |
| Pillar bitmap | `CColorSet_ctor_slotPillar@0x0040ffd0` | Resource **`0x10013`** |
| Walker FLX pack | `CStartGame2_ctor@0x004104f0` | `TM_BindSequence` over `g_dwLobbyWalkerResIds` **`0x100b4`–`0x100b7`** |
| Avatar drawable cast | `CStartGame2_ctor@0x004104f0` | `CheckedVirtualBaseCast(..., DAT_004b826c)` → `CGameView_ctor` (CBulPicture static-face path) |
| `CGameTypeDlg` stack embed | `CStartGame2_OnCmd@0x0040fc20` | Case **`0xDA`**: `CGameTypeDlg local_110`; `CGameTypeDlg_BuildUi(&local_110, pParentState)`; `CDSView_DoModal` |
| `CGameCounter` not on lobby | `CGameCounter.md`, `0x00410e20` | `OperatorNew(0x80)` on **StartGame/loading** path only |

### Menu flow (summary)

| Step | Address | Notes |
|------|---------|-------|
| Show lobby | `CMenu_ShowLobby@0x00414790` | Cleanup → ctor → `NetSendLobbySyncAll` → `DoModalChild` |
| Rules dialog | `OnCmd@0x0040fc20` `0xDA` | Stack **`CGameTypeDlg` (0x94 B)**; save/load rules blob from parent |
| Keyboard help | `OnCmd` `0xD2`–`0xD4` | `CKeybShow_Build` + modal |
| Setup | `OnCmd` `0xEC` | `CDSApp_ShowSetupDialog` |
| Net → UI | `OnBroadcast` / `OnCustomMsg` | Level `0xD0`, avatar `0xD1`, rename `0xD6`, colors `0xDE`, etc. |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `recreate_struct` | `CStartGame2` 220 B | `win`, `pDuplicateSerialWarning`, `bDuplicateSerialPending`, `updated`, `pSlotNameEdit[4]`, `pSlotColorSet[4]`, `pParentState`, `pGamemodeCaption`, `pPressStartHint`, `pLevelList`, `pBtnBack`, flags, pad |
| `set_function_this_type` | `0x004104f0`, `0x0040fc20`, `0x0040d6b0`, `0x0040d7e0`, `0x0040f610` | `CStartGame2 *` |
| `set_function_prototype` | `0x004104f0`, `0x0040d520`, `0x004102d0` | `__thiscall` / `__cdecl` lobby signatures |
| `force_decompile` | ctor, `OnCustomMsg` | `this->pSlotNameEdit[i]`, `this->pSlotColorSet[i]`, `&this->updated` |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CStartGame2.md](./CStartGame2.md) — **created** (layout, flow, embed notes, Ghidra apply)
- [CGameTypeDlg.md](./CGameTypeDlg.md) — cross-ref unchanged (already documents `BuildUi` caller)
- [CGameCounter.md](./CGameCounter.md) — cross-ref unchanged (loading-only)
- [CBulPicture.md](./CBulPicture.md) — referenced via `CGameView` drawable cast, not direct lobby field

## Remaining UNK

- `pDuplicateSerialWarning` assignment site (read/update only proven).
- `bLobbyFlag_bc` / `bLobbyFlag_bd` semantics beyond ctor zero-init.
- `OnCustomMsg` case `0xDF` decompile quality (`SelectPlayerByName` actual `this`).
- Full `0x8002` / `0x8003` button dispatch (often parent `CDSView` chain; see `post_match_lobby.md` §9).
