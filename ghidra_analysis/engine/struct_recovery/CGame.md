# CGame

## Status

**PARTIAL** — Ghidra **`CGame`** is **584 B (`0x248`)**; embedded at **`CBulanci+0x284`** as member **`game`** (same type; audio-bank tail @ `+0x22c..+0x247`). No standalone `OperatorNew(sizeof CGame)`. **`CGame *`** on `CPauseDlg` / scheduler. `CGaming` stack may still show legacy oversized nested `game` until a separate pass.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Embedded span `0x248` | (derived) | `0x4cc - 0x284 = 0x248`; `CBulanci.game` typed **`CGame`** |
| Embedded base `CBulanci+0x284` | `0x004026f0` | `CBulanci_ctor`: `CGame_ctor(&this->game)` |
| Ghidra `CGame` **584** | MCP | `get_struct_layout CGame` → **584** bytes |
| Tail in embed | `0x004026f0` | `(this->game).pAudioBankArray` … `bByte_4c8` @ `game+0x22c..` |

## Layout table (proven offsets)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `+0x00` | 4 | `pointer` | `pVptr_IDSReferenced` | `CGame_ctor@0x00414a80` `*param_1 = g_pCGame_vftable_IDSReferenced` |
| `+0x04` | 24 | `CDSUpdatedItem` | `scheduler` | `CGame_ctor@0x00414a80` `CDSUpdatedItem_ctor(&this->scheduler)`; host `pVftable_IDSUpdated` @ embed+0 (`0x00414abb`); slots 0–7 |
| `+0x1c` | 4 | `pointer` | `pEventHub` | `CGame_ctor@0x00414a80` `param_1[7]` |
| `+0x30` | 1 | `byte` | `bNetState` | `CGame_StartGame@0x00413f30` `this->field_0x30 = 4/5/6`; net handlers gate on `== 6` (`damage_pipeline.md`) |
| `+0x31` | 164 | `CDSChain_full` | `chain` | `CGame_ctor@0x00414aeb` `CDSChain_ctor(&this->chain)` — see [`CDSChain.md`](./CDSChain.md) |
| `+0xD5` | 3 | `byte[3]` | `pad_after_chain` | Ghidra pad between chain tail and lobby bytes |
| `+0xD8` | 1 | `byte` | `bTotalSlots` | lobby ctor band |
| `+0xD9` | 1 | `byte` | `bJoinedSlotCount` | lobby ctor band |
| `+0xDA` | 1 | `byte` | `bHostSlotCursor` | `CPauseDlg_OnNotify@0x0040b375` |
| `+0xDB` | 1 | `byte` | `bLocalSenderSlot` | `CPauseDlg_OnNotify@0x0040b2f0` |
| `+0xDC` | 140 | `PlayerLobbyRec[4]` | `pPlayerRec` | `CGame_ctor@0x00414a80` `_eh_vector_constructor_iterator_(param_1+0x37, 0x23, 4, …)` |
| `+0x168` | 52 | `NetSlotBinding[4]` | `pSlotBinding` | follows player rec stride in ctor/dtor vector |
| `+0x1d8` | 4 | `CDSDirectPlay *` | `pDirectPlay` | `CGame__SchedulerDispatch@0x00416030` `CDSDirectPlay_Receive((CGame *)this->pDirectPlay, …)` |
| `+0x1dc` | 4 | `pointer` | `pRecvBuf` | ctor `param_1[0x77]=0` (dword index); `CPauseDlg_Build` tests `pGame+0x1dc` for lobby chat panel |
| `+0x1e0` | 4 | `uint` | `dwRecvBufSize` | scheduler receive loop `@0x004161ef` |
| `+0x1f0` | 4 | `pointer` | `pActiveCGaming` | `CGame__SchedulerDispatch@0x00416030` keyboard branch |
| `+0x204` | 4 | `pointer` | `pDirectKeyb` | `CGame_StartGame@0x00413f30` stores `OperatorNew(0x20c)` result in `this[2].field_0x42` (CMenu view); dtor `CGame_dtor@0x00414ca0` `param_1[0x7e]` vec |
| `+0x208` | 1 | `byte` | `bPollKeyboard` | `CGame__SchedulerDispatch@0x00416030` early-out |
| `+0x20d` | — | `byte` | (team score tag) | `CGame__SchedulerDispatch@0x004163a7` compares `&this->field_0x20d` to `PTR_DAT_004aef78` |
| `+0x210` | 2 | `short` | `nRoundTimerSecs` | scheduler case 4 `@0x004162bd` `*(short *)this->pPad_20a` (layout name `pad_20a`) |
| `+0x220` | 12 | `uint[3]` | `pSchedTeamScoreScratch` | scheduler case 6 clears `[0]`; ctor `param_1[0x82]` tail |
| `+0x22c` | 4 | `pointer` | `pAudioBankArray` | `CBulanci_ctor@0x0040275b` `(game).pAudioBankArray = 0` |
| `+0x230` | 4 | `uint` | `dwReserved_beforeBankCount` | `CBulanci_ctor@0x0040275b` zero only — no reads in binary |
| `+0x234` | 4 | `uint` | `dwAudioBankCount` | `CBulanci_ctor@0x00402761`; `CBulanci_dtor@0x00402d5b` loop |
| `+0x238` | 4 | `uint` | `dwAudioBankCapacity` | `CBulanci_ctor@0x00402767` `= 8` |
| `+0x23c` | 4 | `pointer` | `pReleaseOnDestroy` | `CBulanci_ctor@0x00402771`; dtor `Release` `@0x00402c5e`; `CBulanci_OnCreate` stores UI SFX bank |
| `+0x240` | 4 | `pointer` | `pCmdLine` | `CBulanci_SetCmdLine@0x00401992` `CDsStringAssignFromLiteral`; ctor/dtor `FUN_004028d0` release |
| `+0x244` | 1 | `byte` | `byte_4c8` | `CBulanci_ctor@0x004027a7` |

Offsets `+0x08..+0x2b`, `+0x38..+0x65`, `+0x88..+0xbb`, `+0x1ae..+0x1d7`, `+0x1f4..+0x203`, `+0x209..+0x20c` — padded in Ghidra (`pad_*` / `pPad_*`) pending dedicated batch evidence.

## Ghidra apply

**Agent todo 1 (2026-05-30):** `create_struct CGame_embedded` (556 B); `CBulanci.game` @ +644; `save_program bulanci.exe`. Global `CGame` (584 B) retained for `CGame *`.

**Agent todo 13 (2026-05-30):** `CGame+0x66` / `+0x6e` = `chain.pLevelResourceTable` / `chain.nLevelResourceCount` (`CDSChain_full+0x35` / `+0x3d`). **Writer:** `CBulanci_EnumerateLevelScripts@0x00409f60` → `CIntListInsertSortedOrAppend` @ `0x0040a062` (`ADD ECX,0x35`); orchestrated by `CBulanci_BuildLevelResourceTable@0x0040a0b0` from `CBulanci_InitResourceBank@0x00402180`. **Readers:** `CGame_FindResourceByName@0x00413560`, `CMenu_OpenNetworkSession@0x00414ee4`.

**Agent todo 21 (2026-05-30):** `delete_data_type` / `create_struct CGame` **584 B** with **`CDSChain_full chain` @ +0x31**; `CGame_ctor` prototype `CGame * __fastcall CGame_ctor(CGame *this)`; decompile `CDSChain_ctor(&this->chain)`; `save_program bulanci.exe`.

**Agent todo 7 r2 (2026-05-30):** `dwReserved_beforeBankCount` @ `+0x230` (ctor zero, no reads); `pCmdLine` @ `+0x240` (`CBulanci_SetCmdLine` / `CDsString_*`); comments @ `0x00401992`, `0x0040275b`; `save_program`.

**Agent todo 1 r2 (2026-05-30):** `modify_struct_field` `pScheduler` → **`CDSUpdatedItem scheduler` @ +0x04** (24 B); decompile `CGame_ctor` shows `&this->scheduler`, `pVftable_IDSUpdated`; comments @ `0x00414abb`, `CGame__SchedulerDispatch@0x00416030`; `save_program bulanci.exe`.

**Agent todo 1 r3 (2026-05-30):** Audio/cmdline tail xrefs at legacy `CBulanci+0x4b0..` confirmed ≡ `game+0x22c..` (`CBulanci_ctor@0x00402761`, `CBulanci_dtor@0x00402d5b`, `CBulanci_ReleaseAudioBank@0x00402bee`). See `round3_task_01_report.md`.

## Follow-up (round 3)

| Task | Action |
|------|--------|
| **1** | ~~Audio tail xref alias `CBulanci+0x4b0` → `game+0x22c`~~ **done** (agent todo 1 r3). |
| **12** | `CGaming.game` **872 B** `-BAD-` vs stack / `CGame` reconciliation. |
| **21** | ~~Embed `CDSChain_full` @ `+0x31`~~ **done** — `CGame.chain` in Ghidra (584 B). |
| **23** | `CPauseDlg.pGame` → **`CGame *`** (logical overlay on embed base). |

## UNK

- Level-resource table writer **resolved** (todo 13): not a bare `MOV [reg+0x66]` — inserts go through `CIntListInsertSortedOrAppend` on `&game.chain.pLevelResourceTable` during master-pack enum (class `0x7ea`).
- `pLobbyView`, `pOnNetCustomStream`, `pDemoJournalStream` semantics.
- `dwReserved_beforeBankCount` purpose if a future build reads it (currently dead store).
- Padded bands in Ghidra (`pad_*` / `pPad_*`) between proven anchors — see layout table gap note.
