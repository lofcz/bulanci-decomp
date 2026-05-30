# CGame

## Status

**PARTIAL** — Ghidra **`CGame`** **584 B (`0x248`)**; embedded at **`CBulanci+0x284`**. Round-5 worker 22 closed **`+0x28..+0x2f`** profile-bind reserved dwords, **`+0x1AC`** custom-stream anchor, **`+0x1DC`** lobby-DP vs recv-buffer multiplex, and **`pActiveCGaming` / `pLobbyView`** roles. **Still open:** retype **`+0x1B0..+0x1DB`** as embedded **`CDSEasyMemStream`** (Ghidra still **`pDemoJournalStream_iface` + `pPad_netBlock`**), and name chain-tail lobby vectors on **`CDSChain_full`** (not only abs **`CGame+0xbc`**).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Embedded span `0x248` | (derived) | `0x4cc − 0x284 = 0x248`; `CBulanci.game` typed **`CGame`** |
| Embedded base `CBulanci+0x284` | `0x004026f0` | `CBulanci_ctor`: `CGame_ctor(&this->game)` |
| Ghidra `CGame` **584** | MCP | `get_struct_layout CGame` → **584** bytes |
| Tail in embed | `0x004026f0` | `(game).pAudioBankArray` … `bMenuLastSplashFlag` @ `game+0x22c..` |

## Layout table (proven offsets)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `pointer` | `pVptr_IDSReferenced` | `CGame_ctor@0x00414a80` |
| `+0x04` | 24 | `CDSUpdatedItem` | `scheduler` | `CGame_ctor`: `CDSUpdatedItem_ctor(&scheduler)`; host `pVftable_IDSUpdated` @ embed+0; slots 0–7 registered (`Scheduler_RegisterEventSlot` ×8) |
| `+0x1c` | 4 | `pointer` | `pEventHub` | `CGame_ctor`: `param_1[7]`; `Scheduler_PopHook` / `BroadcastEvent` base |
| `+0x20` | 8 | `CDSPtrSlotVec` | `profileBindSlotVec` | `CGame_ctor@0x00414ac0` zero `{pSlots,nCapacity}`; dtor `CDSPtrSlotVec_Resize(&+0x20,0)` @ `0x00414d97` |
| `+0x28` | 4 | `uint` | `dwProfileBindReserved` | `CGame_ctor@0x00414ac0` `=0`; stacked in `CDSDirectPlay_EnumSessions@0x00416167` with `profileBindSlotVec`; **no** `CDSPtrSlotVec_Resize` |
| `+0x2c` | 4 | `int` | `nProfileBindPreallocCap` | `CGame_ctor` `=8`; ctor-only besides EnumSessions stack — **not** `profileBindSlotVec.nCapacity` |
| `+0x30` | 1 | `byte` | `bNetState` | `CGame_StartGame`, `CGame_ResetForLobby`, `CGame_ProcessNetMessage` gate match opcodes on `== 6` |
| `+0x31` | 164 | `CDSChain_full` | `chain` | `CDSChain_ctor@0x00414aeb`; dtor `CBulanci_DestroyConfigStore@0x00414d8a` — [`CDSChain.md`](./CDSChain.md), R3/27, R4-27 |
| `+0x35` | — | *(in chain)* | `pLevelResourceTable` | `CBulanci_EnumerateLevelScripts@0x0040a062` `ADD ECX,0x35`; readers `CGame_FindResourceByName@0x00413560`, `CMenu_OpenNetworkSession@0x00414ee4` |
| `+0x3d` | — | *(in chain)* | `dwLevelResourceCount` | Same insert path / find-resource walk — R3 task 13 |
| `+0x66` | — | *(abs)* | `chain.pLevelResourceTable` | `0x31+0x35`; heap rows **`CLevelScriptResource`** (20 B) |
| `+0x6e` | — | *(abs)* | `chain.dwLevelResourceCount` | `0x31+0x3d` |
| `+0x86` | 1 | `byte` | `bHostFlag` | `CGame_ResetForLobby@0x00413542` `=1`; `CGame_NetSendAdminByte_t64@0x00412d6a` host gate — **physically inside `chain` blob** (`chain+0x55`) |
| `+0xbc` | 12+ | `vec` | `levelNameList` | Growable **`CDsString*`** vector: `FUN_004146b0` / `CGame_LevelList_AddByName@0x00414920`; dtor `FUN_004146b0@+0xbc` (`param_1+0x2f` dwords); `FUN_00413620` reads head — **`chain.pPad_a4+2`** (`CGame+0xbd`) |
| `+0xc4` | 4 | `int` | `levelNameList_count` | `FUN_00413620@0x0041372a`; vector `+8` |
| `+0xcc` | 4 | `CDSString` | `hLevelTitle` | Net **`0x05 SetLevel`** writes `(chain).pPad_a4+0x12`; dtor `CDsStringReleaseHeader@+0xcc` (`param_1[0x33]`) |
| `+0xd0` | 4 | `uint` | `dwLobbyScratch` | `CGame_ResetForLobby@0x00413549` zero; `CMenu_OpenNetworkSession@0x00414e28` zero |
| `+0xd5` | 3 | `byte[3]` | `pad_after_chain` | Ghidra pad between chain tail and lobby bytes |
| `+0xd8` | 1 | `byte` | `bTotalSlots` | Lobby ctor / roster msgs |
| `+0xd9` | 1 | `byte` | `bJoinedSlotCount` | JoinHost / RosterReply |
| `+0xda` | 1 | `byte` | `bHostSlotCursor` | `CPauseDlg_OnNotify@0x0040b375` |
| `+0xdb` | 1 | `byte` | `bLocalSenderSlot` | `CPauseDlg_OnNotify@0x0040b2f0` |
| `+0xdc` | 140 | `PlayerLobbyRec[4]` | `pPlayerRec` | `CGame_ctor` vector @ `+0x37` dword index |
| `+0x168` | 52 | `NetSlotBinding[4]` | `pSlotBinding` | Follows player rec stride; slot-state net ops |
| `+0x19c` | 1 | `byte` | `bGameModeIndex` | **`0x04 SlotCount`** recv/send; `CGameGetModeAndScoreLimit@0x00412660` |
| `+0x19d` | 1 | `byte` | `bAdminByte` | **`0x64 AdminByte`** recv @ `CGame_ProcessNetMessage` |
| `+0x19e` | 16 | `uint[4]` | `anScoreLimitByMode` | Indexed by `bGameModeIndex`; `CGame_NetSendSlotCount_t04` |
| `+0x1ac` | 4 | `pointer` | `pOnNetCustomStream` | **`CGame_SetCommStrm@0x00414270`** `[this+0x1ac]`; dtor Release `param_1[0x6b]`; net **`0x15`** → script export #9 |
| `+0x1b0` | 44 | `CDSEasyMemStream` | `demoJournalStream` | **Embed** — dtor `CDSEasyMemStream_dtor@0x00414d1f` on `param_1+0x6c`; `OpenNetworkSession@0x00414ff8` `InitBackingBuffer((CDSEasyMemStream *)&pOnNetCustomStream+2, 0x2000, 0x1000)`; scheduler case **0** journals when `bDemoRecording` — **Ghidra still split @ +0x1B8 iface + 28 B pad** |
| `+0x1d8` | 4 | `CDSDirectPlay *` | `pDirectPlay` | Scheduler **`Receive`** `dp=this->pDirectPlay` @ `0x00416167`; **`CBulanci_CopyNetSessionFields@0x0040275b`** assigns match-time DP |
| `+0x1dc` | 4 | `pointer` | `pDirectPlayActive` | **Phase-multiplexed:** lobby `OpenNetworkSession` stores **`CDSDirectPlay*`** (`0x00414f6d`); most **`CDSDirectPlay_Send`** use this slot; in-match scheduler **`Receive`** passes **heap recv buffer**; **`DPERR_BUFFERTOOSMALL`** grow writes buffer here @ `0x0041622c` |
| `+0x1e0` | 4 | `uint` | `dwRecvBufSize` | Scheduler `Receive` **`&size@+0x1E0`**; dtor **`Runtime_Free@0x00414d05`** when slot was heap |
| `+0x1e4` | 4 | `uint` | `dwCountdownTick` | Scheduler case **2** @ `0x0041631f`; `CGame_ResetForLobby` zero |
| `+0x1e8` | 1 | `byte` | `bCountdownByte` | Net **`0x07 Countdown`** @ `CGame_ProcessNetMessage` |
| `+0x1f0` | 4 | `pointer` | `pActiveCGaming` | **Polymorphic `pointer`:** lobby **`CSessionList*`** (scheduler case **1** @ `0x00416248`); in-match **`CGaming*`** (case **4** timer, case **7** keyboard / `DispatchPlayerAction`) |
| `+0x1f4` | 4 | `pointer` | `pLobbyView` | In-match **`CGaming*`** for net handlers (`ProcessNetMessage` **`0x0C..0x19`**); `CMenu_DoModalChild` stores modal child @ `0x0041305d`; cleared after modal |
| `+0x1f8` | 8 | `CDSPtrSlotVec` | `schedChildSlotVec` | `CGame_dtor@0x00414cf1` → `CDSPtrSlotVec_Resize(&+0x1f8,0)` |
| `+0x204` | 4 | `CDirectKeyb *` | `pDirectKeyb` | `CGame_StartGame` `OperatorNew(0x20c)`; scheduler **`PollKeyboard`/`GetKeyEdge`** @ `0x004160a0`/`0x004160e3` — R4 task 07 |
| `+0x208` | 1 | `byte` | `bPollKeyboard` | Scheduler case **7** early-out @ `0x0041607c` |
| `+0x209` | 1 | `byte` | `bDemoRecording` | `CGaming_BeginDemoRecording@0x00412523`; scheduler recv journal gate @ `0x00416196` |
| `+0x20a` | 2 | `byte[2]` | `schedTeamScoreTagBuf` | Scheduler case **6** indexes `[scratch+2]`; case **4** reads **`nRoundTimerSecs`** via `(short*)pPad_20a` |
| `+0x20c` | 1 | `byte` | `bTeamScoreCategory` | Case **6** strcmp base @ `0x00416381`; **`−0x31`** before net send |
| `+0x20e` | 2 | `short` | `nRoundTimerSecs` | Net **`0x12 RoundTimer`** → `field_0x20e`; overlaps `schedTeamScoreTagBuf` tail in Ghidra |
| `+0x220` | 12 | `uint[3]` | `pSchedTeamScoreScratch` | Case **6** clears `[0]` @ `0x004163cc` |
| `+0x22c` | 4 | `pointer` | `pAudioBankArray` | `CBulanci_ctor@0x0040275b` — R3 task 01 alias `CBulanci+0x4b0` |
| `+0x230` | 4 | `uint` | `dwReserved_beforeBankCount` | Ctor zero only; **no reads** (R3-01) |
| `+0x234` | 4 | `uint` | `dwAudioBankCount` | `CBulanci_dtor` loop |
| `+0x238` | 4 | `uint` | `dwAudioBankCapacity` | Ctor `= 8` |
| `+0x23c` | 4 | `pointer` | `pReleaseOnDestroy` | UI SFX bank `Release` |
| `+0x240` | 4 | `pointer` | `pCmdLine` | `CBulanci_SetCmdLine@0x00401992` |
| `+0x244` | 1 | `byte` | `bMenuLastSplashFlag` | `CBulanci_ctor@0x004027a7` (was `bByte_4c8`) |

## Scheduler (`CDSUpdatedItem` @ +0x04)

| Slot | Period (ms) | Role | Handler |
|------|-------------|------|---------|
| 0 | 1 | DirectPlay recv pump | case **0** → `CDSDirectPlay_Receive` → `CGame_ProcessNetMessage` |
| 1 | 1000 | Session enum | case **1** → `CSessionList_*` on **`pActiveCGaming`** |
| 2 | 1000 | Lobby countdown tick | case **2** → `dwCountdownTick` / `CGame_NetSendCountdown_t07` |
| 3 | 0 | (armed ad hoc) | default → `CGame_NetSendOpposingEvent_t17` |
| 4 | 1000 | Round timer HUD | case **4** → `CGaming_OnTimerCountdown(pActiveCGaming, nRoundTimerSecs−1)` |
| 5 | 60000 | — | default branch |
| 6 | 400 | Team score flush | case **6** → strcmp **`bTeamScoreCategory`** vs `PTR_DAT_004aef78` |
| 7 | 1 | Keyboard poll | case **7** → **`pDirectKeyb`** when **`bPollKeyboard`** && **`pActiveCGaming`** |

Dispatch: `CGame__SchedulerDispatch@0x00416030`.

## Ghidra apply

**Pass R5 worker 22 (2026-05-30)** — CGame UNK/layout follow-up:

- `add_struct_field` `dwProfileBindReserved` @ +40, `nProfileBindPreallocCap` @ +44
- `modify_struct_field` `pRecvBuf` → **`pDirectPlayActive`** (multiplex documented)
- `set_decompiler_comment` @ `0x00414ac0`, `0x00414f6d`, `0x00414270`, `0x00414d1f`
- `save_program bulanci.exe`

**Pass R4 CGame (2026-05-30):** scheduler, chain embed, lobby/net block, `pDirectKeyb`, `profileBindSlotVec`, `schedChildSlotVec`, decompiler comments.

**Prior rounds (unchanged):** R3-01 audio tail; R3-13 level-resource writer; R3-27 chain dtor parity; R4-01 embed destroy; R4-07 `pDirectKeyb`; R4-21 `CDSChain_full` embed; R4-27 `DestroyConfigStore` on `chain`.

## Follow-up

| Task | Action |
|------|--------|
| **demoJournalStream** | Replace **`pDemoJournalStream_iface` + `pPad_netBlock`** with **`CDSEasyMemStream demoJournalStream` @ +0x1B0** (44 B); verify `pDirectPlay`/`pDirectPlayActive` not overlapping stream tail in Ghidra |
| **Chain interior** | Add **`levelNameList` / `hLevelTitle` / `bHostFlag`** fields on `CDSChain_full` past `+0xA4` tail (or overlay struct) — abs offsets proven on `CGame` |
| **pActiveCGaming** | R5 task **19**: keep **`pointer`** for `CSessionList*` vs `CGaming*` polymorphism |
| **12** | `CGaming.game` stack **`872 B`** vs embedded `CGame` reconciliation |
| **23** | `CPauseDlg.pGame` → **`CGame *`** |

## UNK

- **`demoJournalStream` Ghidra retype** — runtime embed proven @ `+0x1B0` (`Init`/`dtor`); struct editor still pointer+pad through `+0x1DB`.
- **`pActiveCGaming` static type** — intentionally `pointer` until split lobby/match fields are justified.
- **`dwReserved_beforeBankCount`**: dead store (R3-01).
- **`pAuxHeap`** on chain list-head: teardown-only (R3-27).
- **`nProfileBindPreallocCap` consumer** — ctor `=8` and EnumSessions stack only; no growth path located.
