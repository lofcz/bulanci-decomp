# Pass R4 — CMenu struct recovery report

## Task

| Field | Value |
|-------|-------|
| **Scope** | Map and describe **CMenu** members: layout, menu state machine, network session, child dialogs |
| **Inputs** | `main_menu.md`, batch summaries (15–18, 23, 41), [CSessionList.md](./CSessionList.md), Ghidra `bulanci.exe` |
| **Deliverables** | [CMenu.md](./CMenu.md), Ghidra struct + `this` typing, `save_program` |

## Status

**DONE** — UI `CMenu` layout `0xe4` applied in Ghidra; state machine and child-dialog tables documented; network helpers retyped to **`CGame *`**.

## Evidence

### Size & allocation

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CMenu)==0xe4` | `0x004251f7` | `CMenu_CreateObject` → `OperatorNew(0xe4)` |
| UI menu instance | `0x00402490` | Menu event `0xcc` → `CBulanci+0x280` |

### UI layout (ctor / dispatch)

| Offset | Name | Evidence |
|--------|------|----------|
| `0x80` | `nPushedSubScreenId` | `CMenu_ctor_with_ui@0x004265e0` init `-1`; `CMenu_CmdDispatch` sets `0/1/2` |
| `0x84` | `pCurSubScreen` | `CmdDispatch` stores sub-dialog; `CloseCurrentSubScreen` release |
| `0x88` | `pSubScreenBgBitmap` | `LoadBackgroundResource@0x00425870` |
| `0x8c..0xc8` | title / icons / hero | `CMenu_ctor_with_ui` resource IDs |
| `0xb0..0xb8` | `CSwitch *`×3 | cmds `0xc9/0xca/0xcb` |
| `0xc0/0xc4` | music / exit voice | `OnMusicFadeTick`, `DispatchHotkey`, `OnEvent(1)` |
| `0xcc` | `wModalExitCodeStash` | `DispatchHotkey` + `OnEvent(1)` |
| `0xd0..0xdc` | `CRuch *`×4 | ctor loop + `EnableAllRuch` |
| `0xe0..0xe2` | splash / day-night / bg flag | ctor + `PollDayNight` |

### Network `this` ≠ UI menu

| Instruction | Evidence |
|-------------|----------|
| `CMenu_OpenNetworkSession((CMenu *)(param_1 + 0x284))` | `CMenu_OnSubScreenBack@0x00401b3a` — **`CBulanci+0x284` = embedded `CGame`**, not `pMainMenu` @ `+0x280` |
| `OpenNetworkSession` reads `+0x66/+0x6e`, `+0xbc`, `pDirectPlay` | Same function @ `0x00414dd0` — matches [CGame.md](./CGame.md) |

### Child dialogs

| Dialog | Size | Entry |
|--------|------|-------|
| `CStartGame1` | `0xa8` | `CmdDispatch` `0xc9` @ `0x00425970` |
| `CHistoryDlg` | `0x9c` | `0xca` |
| `CExitDlg` | `0x7c` | `0xcb` |
| `CTcpIpConfig` | `0x70` | `OpenNetworkSession` join/TCP @ `0x00414dd0` |
| `CSessionList` | `0x7c` | `PickSession@0x00414170` |
| `CStartGame2` | `0xdc` | `ShowLobby@0x00414790` |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `recreate_struct` | `CMenu` | 228 B; `updatedItem@0x68`; menu fields `0x80..0xe3` |
| `set_function_this_type` | `CMenu_ctor_with_ui`, `CmdDispatch`, `CloseCurrentSubScreen`, `OnEvent` | `CMenu *` |
| `set_function_prototype` | `CMenu_OpenNetworkSession@0x00414dd0` | `void __fastcall ...(CGame *this)` |
| `set_function_this_type` | `ShowLobby`, `PickSession`, `DoModalChild` | `CGame *` |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- **Created** [CMenu.md](./CMenu.md) — full layout, dual-object note, state machine, network/child-dialog cross-links.

## Remaining UNK

- Per-field names for `CMenu+0x40..0x67` (shared `CWindow` chain band).
- Optional symbol rename `CMenu_OpenNetworkSession` → `CGame_OpenNetworkSession` (behavior already typed).
- `pad_a8` / `pad_bc` — no reads/writes in menu paths.
