# CMenu



## Status



**VERIFIED (UI shell)** — heap `sizeof(CMenu) == 0xe4` (228 B); main-menu view with sub-screen stack, audio, and day/night. **PARTIAL (symbol split)** — several `CMenu_*` helpers in the `0x004127..0x00414f` band operate on embedded **`CGame`** at `CBulanci+0x284`, not on the UI `CMenu` at `+0x280` (see §Network vs UI).



## Size proof table



| Claim | Address | Evidence |

|-------|---------|----------|

| `sizeof(CMenu) == 0xe4` | `0x004251f7` | `_Globals::CMenu_CreateObject` → `OperatorNewWithBadAlloc(0xe4)` → `CMenu_ctor` |

| Ghidra struct | — | `get_struct_layout CMenu` → **228 bytes** (R5 worker 33, 2026-05-30) |

| UI instance lifetime | `0x00402490` | `CBulanci_OnEvent_MenuStateMachine` case `0xcc`: `OperatorNew(0xe4)` / ctor-with-UI → store `CBulanci.pMainMenu` @ `+0x280` |

| Class registry | `0x0047c14c` | classId **2004** (`0x7d4`); RTTI `.?AVCMenu@@` @ `0x004af894` |



## Two objects, one symbol prefix



| Object | Storage | Size | Role |

|--------|---------|------|------|

| **UI `CMenu`** | `CBulanci.pMainMenu` @ `+0x280` | `0xe4` | Visible main menu (`CMenu_ctor_with_ui`), `CDSView_DoModal` child of app |

| **Lobby `CGame`** | `CBulanci.game` @ `+0x284` | `0x248` | DirectPlay, level list, slots, recv buf — accessed by misnamed `CMenu_OpenNetworkSession`, `CMenu_ShowLobby`, `CMenu_PickSession`, `CMenu_DoModalChild` |



**Proof (network `this`):** `CMenu_OnSubScreenBack@0x00401af0` calls `CMenu_OpenNetworkSession((CMenu *)(param_1 + 0x284))` — **`CBulanci+0x284` = `CGame`**, not `pMainMenu`. See [CGame.md](./CGame.md), [CSessionList.md](./CSessionList.md).



## Layout table (UI `CMenu`, proven)



**Prefix `+0x00..+0x67`:** shared **`CDSChained` / `CWindow` dialog band** (same offsets as [CWindow.md](./CWindow.md)); `CMenu` does **not** use `bModalFlag` / `pDefaultFocusChild` @ `+0x68..+0x6f` — that band is **`CDSUpdatedItem updatedItem`** @ `+0x68` (24 B).



| Offset | Size | Type | Name | Evidence (func@addr) |

|--------|------|------|------|----------------------|

| `0x00` | 4 | `CDSView_vftable_t *` | `pVftable_primary` | `CMenu_ctor@0x00424520`, `CMenu_ctor_with_ui@0x004265e0` |

| `0x04` | 4 | `void *` | `pVftable_IDSChained` | same |

| `0x08` | 4 | `dword` | `dwField_08` | `CDSChained_ctor@0x004032d0` |

| `0x0c` | 4 | `dword` | `dwField_0c` | same |

| `0x10` | 4 | `void *` | `pVftable_IDSEventHandler` | same; `CMenu_OnKeyDown` posts to `&this+0x10` |

| `0x14` | 2 | `ushort` | `wViewFlags` | `CMenu_ctor_with_ui` `\|= 0x77f` @ `0x0042667d` |

| `0x18` | 4 | `void *` | `pVftable_IDSReferenced` | same |

| `0x1c` | 4 | `dword` | `dwField_1c` | `CDSChained_ctor` |

| `0x20` | 4 | `int` | `nBbox_left` | ctor-with-UI `(0,0,800,600)` @ `0x00426669..0x00426676` |

| `0x24` | 4 | `int` | `nBbox_top` | same |

| `0x28` | 4 | `int` | `nBbox_right` | same |

| `0x2c` | 4 | `int` | `nBbox_bottom` | same |

| `0x30` | 4 | `dword` | `dwField_30` | `CDSChained_ctor` `=0` |

| `0x34` | 4 | `dword` | `dwField_34` | same |

| `0x38` | 4 | `dword` | `dwField_38` | same |

| `0x3c` | 4 | `dword` | `dwField_3c` | same |

| `0x40` | 4 | `dword` | `dwChainHead_40` | `CDSChained_ResetChainCounters@0x0042beb0` |

| `0x44` | 2 | `ushort` | `wViewStateFlags` | `ResetChainCounters` `=1` |

| `0x46` | 2 | `ushort` | `wWidgetFlags` | ctor-with-UI `\|= 0x21` @ `0x00426683` |

| `0x48` | 2 | `ushort` | `wChainCounter_48` | `ResetChainCounters` `=0` |

| `0x4a` | 2 | `ushort` | `wChainCounter_4a` | same |

| `0x4c` | 4 | `void *` | `pParent` | `ResetChainCounters` `=0` |

| `0x50` | 4 | `dword` | `dwChainField_50` | same |

| `0x54` | 4 | `void *` | `pVftable_CDSChain_IDSReferenced` | `CDSChained_ctor` |

| `0x58` | 4 | `void *` | `pVftable_CDSChain_IDSChained` | same |

| `0x5c` | 4 | `dword` | `dwField_5c` | `CDSChained_ctor` `=0` |

| `0x60` | 4 | `dword` | `dwField_60` | same |

| `0x64` | 4 | `dword` | `dwField_64` | same |

| `0x68` | 24 | `CDSUpdatedItem` | `updatedItem` | `CDSUpdatedItem_ctor@0x0042455b`; `Scheduler_RegisterEventSlot(&updatedItem,0,0x78,7)` @ `0x0042661d` |

| `0x80` | 4 | `int` | `nPushedSubScreenId` | ctor-with-UI `= -1`; `CMenu_CmdDispatch` `0/1/2`; `CMenu_CloseCurrentSubScreen` restore `-1` |

| `0x84` | 4 | `CDSView *` | `pCurSubScreen` | `CMenu_CmdDispatch` stores `CStartGame1` / `CHistoryDlg` / `CExitDlg`; cleared in `CloseCurrentSubScreen` |

| `0x88` | 4 | `CDSBitmap *` | `pSubScreenBgBitmap` | `CMenu_LoadBackgroundResource@0x00425870` |

| `0x8c` | 4 | `CDSBitmap *` | `pTitleBitmap` | ctor-with-UI resource `0x1013b` @ `(0,0)` |

| `0x90` | 4 | `CDSBitmap *` | `pIconStartNormal` | ctor-with-UI `0x10093` @ `(100,55)` |

| `0x94` | 4 | `CDSBitmap *` | `pIconStartHighlight` | `0x10094`; hidden after build |

| `0x98` | 4 | `CDSBitmap *` | `pIconHistoryNormal` | `0x10043` @ `(100,139)` |

| `0x9c` | 4 | `CDSBitmap *` | `pIconHistoryHighlight` | `0x10044`; hidden |

| `0xa0` | 4 | `CDSBitmap *` | `pIconQuitNormal` | `0x1004b` @ `(100,223)` |

| `0xa4` | 4 | `CDSBitmap *` | `pIconQuitHighlight` | `0x1004c`; hidden |

| `0xa8` | 8 | `byte[8]` | `pad_a8_af` | **No `.text` store** (`search_byte_patterns` `89 86 a8`); 8 B gap before `CSwitch *` @ `0xb0` |

| `0xb0` | 4 | `CSwitch *` | `pBtnStart` | ctor-with-UI `CSwitch_ctor(0x23,0x25,1,0xc9)` |

| `0xb4` | 4 | `CSwitch *` | `pBtnHistory` | cmd `0xca` |

| `0xb8` | 4 | `CSwitch *` | `pBtnQuit` | cmd `0xcb` |

| `0xbc` | 4 | `byte[4]` | `pad_bc_bf` | **No `.text` store**; gap before audio players @ `0xc0` |

| `0xc0` | 4 | `CDSAudioPlayer *` | `pBackgroundMusicPlayer` | `CMenu_LoadBackgroundMusic` / `OnMusicFadeTick` |

| `0xc4` | 4 | `CDSAudioPlayer *` | `pDeferredExitVoicePlayer` | `CMenu_DispatchHotkey`; `CMenu_OnEvent(1)` release |

| `0xc8` | 4 | `CDSBitmap *` | `pHeroBitmap` | resource `0x10139` @ `(610,0)`; `SetDayNightBg` show/hide |

| `0xcc` | 2 | `ushort` | `wModalExitCodeStash` | `CMenu_DispatchHotkey` write; `CMenu_OnEvent(1)` → `CDSView__EndModal` |

| `0xd0` | 4 | `CRuch *` | `pRuch0` | ctor-with-UI loop `OperatorNew(0x80)`; `CMenu_EnableAllRuch` |

| `0xd4` | 4 | `CRuch *` | `pRuch1` | same |

| `0xd8` | 4 | `CRuch *` | `pRuch2` | same |

| `0xdc` | 4 | `CRuch *` | `pRuch3` | same |

| `0xe0` | 1 | `byte` | `bLastSplashFlag` | ctor-with-UI param; `CMenu_CmdDispatch` case `0xc9` auto-back when non-zero |

| `0xe1` | 1 | `byte` | `bDayNightLatch` | `CMenu_PollDayNight` / `SetDayNightBg` |

| `0xe2` | 1 | `byte` | `bBgMusicEnabled` | `CMenu_EnableBackgroundState` / `OnMusicFadeTick` |



**Siblings (not stored on `this`):** `CPoemScroller` (`0x128`), version `CStaticText` @ `(0xe6,0x23f)` — `CDSView__AddChild` only ([`main_menu.md`](../../gameplay/main_menu.md) §2.3).



## Menu state machine



### A. App-level (`CBulanci`)



| Event | Handler | Effect |

|-------|---------|--------|

| `0xf7` | `CBulanci_OnEvent@0x00402490` | Stack `CAdvertising` splash → schedule `0xcc` |

| `0xcc` | same | `pMainMenu = CMenu_ctor_with_ui(...)`; `CDSView_DoModal(menu)`; exit codes `-0x7ffc` / `-0x7f35` → shutdown |

| `0xcd` | same | Re-arm menu after exit-confirm |



### B. UI sub-screen index (`nPushedSubScreenId` @ `+0x80`)



| `nPushedSubScreenId` | Cmd | Child dialog | Size | Ctor |

|---------------------|-----|--------------|------|------|

| `0` | `0xc9` | `CStartGame1` | `0xa8` | `0x0040f400` |

| `1` | `0xca` | `CHistoryDlg` | `0x9c` | `0x004231d0` |

| `2` | `0xcb` | `CExitDlg` | `0x7c` | `0x00411b50` |

| `-1` | — | none (main chrome visible) | — | after `CMenu_CloseCurrentSubScreen` |



`CMenu_CmdDispatch@0x00425970`: close prior sub-screen → swap bg (`CMenu_LoadBackgroundResource`) → `OperatorNew` + ctor → `CDSView__AddChild` → optional `TriggerBankSample` (slots `0x18..0x1b`). **`0xcf`:** `CloseCurrentSubScreen(0)` + `CMenu_OnSubScreenBack` (may call **`CMenu_OpenNetworkSession` on `CGame`**). **`0x8004` / `0x80ce`:** `CMenu_DispatchHotkey` (audio-cued modal exit).



### C. Input



| Path | Function | Notes |

|------|----------|-------|

| Mouse / `CSwitch` | `Button_Click@0x00424d30` → `PostMessage(0x100, cmd)` | cmds `0xc9..0xcb` |

| Keyboard | `CMenu_OnKeyDown@0x00424fa0` | `S/H/K` → buttons; `X` → `PostMessage(0x100,0x8004)` (async exit) |

| Audio complete | `CMenu_OnEvent@0x00424f50` case `1` | reads `wModalExitCodeStash`, ends modal |



First boot: `CMenu_ctor_with_ui` with `bLastSplashFlag==0` auto-`Button_Click(pBtnStart)` → opens `CStartGame1` + plays slot `0x1b` ([`main_menu.md`](../../gameplay/main_menu.md) §10.1).



## Network session & child dialogs (on `CGame`)



These symbols retain the `CMenu_` prefix but **`this` is `CGame *` (`CBulanci.game`)**:



| Symbol | Address | `this` | Role |

|--------|---------|--------|------|

| `CMenu_OpenNetworkSession` | `0x00414dd0` | **`CGame *`** | Host/join DirectPlay, level list @ `CGame+0xbc`, `pDirectPlay`, join msg `0x00`, then `ShowLobby` |

| `CMenu_ShowLobby` | `0x00414790` | **`CGame *`** | Stack `CStartGame2` (`0xdc`), `DoModalChild`, kick sync |

| `CMenu_PickSession` | `0x00414170` | **`CGame *`** | Stack `CSessionList` (`0x7c`), modal pick → session GUID dwords |

| `CMenu_DoModalChild` | `0x00413030` | **`CGame *`** | Nested `CDSView_DoModal`; uses `CGame.pEventHub` / scheduler @ `+0x1c` |

| `CMenu_ShowConnectingDialog` | `0x004137b0` | **`CGame *`** | `CMsgDialog` “connecting…” |

| `CMenu_OnSubScreenBack` | `0x00401af0` | `CBulanci *` | Re-show buttons; **`OpenNetworkSession(game)`** |



**Join-only child dialogs** (from `OpenNetworkSession`):



| Dialog | Size | When |

|--------|------|------|

| [`CTcpIpConfig`](./CTcpIpConfig.md) | `0x70` | Join + TCP (`field_0x65==0`) — stack modal before `ConnectTCP` |

| [`CSessionList`](./CSessionList.md) | `0x7c` | After `JoinSession` — `PickSession` |

| [`CMsgDialog`](./CMsgDialog.md) | `0x70` | Connecting / fatal error |



**Lobby UI:** `CStartGame2` (`0xdc`) built on `CGame`; embeds [`CLevelList`](./CLevelList.md) (`0xe4`). See [`main_menu.md`](../../gameplay/main_menu.md) §3–4, [`net_protocol.md`](../../netcode/net_protocol.md).



## Key functions (UI `CMenu *`)



| Symbol | Address |

|--------|---------|

| `CMenu_CreateObject` | `0x004251d0` |

| `CMenu_ctor` | `0x00424520` |

| `CMenu_ctor_with_ui` | `0x004265e0` |

| `CMenu_CmdDispatch` | `0x00425970` |

| `CMenu_CloseCurrentSubScreen` | `0x00423a00` |

| `CMenu_LoadBackgroundResource` | `0x00425870` |

| `CMenu_DispatchHotkey` | `0x00425340` |

| `CMenu_OnEvent` | `0x00424f50` |

| `CMenu_OnKeyDown` | `0x00424fa0` |

| `CMenu_PollDayNight` | `0x00425400` |

| `CMenu_OnMusicFadeTick` | `0x00424080` |

| `CMenu_DetachChildWithVisibility` | `0x0042d160` |



## Ghidra apply (pass R4 + R5 worker 33, 2026-05-30)



```

recreate_struct CMenu → 228 B

  prefix +0x00..+0x67: CWindow chain band (not monolithic pad)

  updatedItem @ 0x68; menu tail 0x80..0xe3; pad_a8_af / pad_bc_bf

set_function_this_type CMenu_ctor_with_ui / CmdDispatch / CloseCurrentSubScreen / OnEvent → CMenu *

set_function_prototype CMenu_OpenNetworkSession → void __fastcall ...(CGame *this)

set_decompiler_comment @0x00426683, @0x00426c8e

save_program bulanci.exe

```

**R5 worker 13 (2026-05-30):** `FUN_0042d160` → `CMenu_DetachChildWithVisibility` — `ReleaseKeyboardFocus` / `Hide` / `CMenu_DetachChild` / `Show` on child `wChainFlag46` bits; callers include `CHelpDlg_LoadHelpPage@0x00421c61`, `CHistoryDlg_LoadHistoryPage@0x00422fc1`, `CMenu_CloseCurrentSubScreen`.



## UNK



- `dwField_08` / `dwField_0c` / `dwField_30..3c` / `dwField_5c..64` — shared `CDSChained` dwords; see [CWindow.md](./CWindow.md) §UNK (not menu-specific).

- Rename `CMenu_OpenNetworkSession` et al. to `CGame_*` — cosmetic; left as shipped symbols.

- `CMenu_ctor_with_ui` Ghidra name still `CBulanci::` in some builds — `this` typed `CMenu *`.



## Cross-links



- [`CBulanci.md`](./CBulanci.md) — `pMainMenu` @ `+0x280`, `game` @ `+0x284`

- [`CGame.md`](./CGame.md) — lobby / DirectPlay layout

- [`CWindow.md`](./CWindow.md) — dialog prefix through `+0x67`

- [`CSessionList.md`](./CSessionList.md), [`CTcpIpConfig.md`](./CTcpIpConfig.md), [`CExitDlg.md`](./CExitDlg.md)

- [`CSwitch.md`](./CSwitch.md), [`CDSUpdatedItem.md`](./CDSUpdatedItem.md)

- [`main_menu.md`](../../gameplay/main_menu.md) — narrative walkthrough


