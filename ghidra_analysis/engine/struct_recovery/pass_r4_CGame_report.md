# Pass R4 — `CGame` struct recovery

## 1. Scope

Map remaining **`CGame`** pieces after R3/R4 partial work: embedded **`CDSChain_full` @ +0x31`**, **scheduler**, **audio/cmdline tail**, **`pDirectKeyb @ +0x204`**, **level-resource table**, **lobby/net/demo block**, and close **`field_0x*`** / **`pPad_*`** gaps where evidence allows.

**Inputs:** [CGame.md](./CGame.md), [CBulanci.md](./CBulanci.md), [round3_task_01_report.md](./round3_task_01_report.md), [round3_task_13_report.md](./round3_task_13_report.md), [round3_task_27_report.md](./round3_task_27_report.md), [round4_task_07_report.md](./round4_task_07_report.md), [round4_task_27_report.md](./round4_task_27_report.md)

**Seed addresses:** `0x00414a80`, `0x00414aeb`, `0x00416030`, `0x00413560`, `0x0040a062`, `0x00415290`, `0x00414ee4`, `0x00413f30`, `0x00414ca0`, `0x00413510`, `0x00414270`

## 2. Status

**DONE (partial)** — Ghidra **`CGame`** remains **584 B** with **~90%** lobby/net/scheduler/audio fields named; **`demoJournalStream` embed**, **`pOnNetCustomStream` offset (+2)**, **chain-interior lobby vectors**, and **DirectPlay/recv multiplex @ +0x1DC** left for a follow-up MCP pass.

## 3. Evidence

### Embedded chain @ +0x31

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Sole **`CDSChain_ctor`** embed | `CGame_ctor@0x00414aeb` | `CALL CDSChain_ctor(this+0x31)` |
| Full-object teardown | `CGame_dtor@0x00414d8a` | `LEA ECX,[ESI+0x31]` → `CBulanci_DestroyConfigStore` |
| Level-resource table | `CBulanci_EnumerateLevelScripts@0x0040a062` | `ADD ECX,0x35` → `CIntListInsertSortedOrAppend` on **`chain.pLevelResourceTable`** |
| Abs offsets | — | **`CGame+0x66`** = `chain+0x35`; **`CGame+0x6e`** = `chain+0x3d` (R3-13) |
| List-head dtor only | `CDSChain_dtor@0x0042fcd0` | Invoked at **`chain+0x64`** inside destroy-config (R3/R4-27) |

### Scheduler @ +0x04

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| **`CDSUpdatedItem`** embed 24 B | `CGame_ctor@0x00414a80` | `CDSUpdatedItem_ctor(&scheduler)`; **`g_pCGame_vftable_IDSChained`** on host |
| Slot registration | `CGame_ctor` | `Scheduler_RegisterEventSlot` slots **0–7** (periods 1, 1000, 1000, 0, 1000, 60000, 400, 1) |
| Dispatch | `CGame__SchedulerDispatch@0x00416030` | Cases **0–7** documented in `CGame.md` |
| Case **1** polymorphic slot | `0x00416248` | **`pActiveCGaming`** → **`CSessionList*`** (not in-match **`CGaming*`**) |
| Case **7** keyboard | `0x004160a0`/`0x004160e3` | **`pDirectKeyb`** **`PollKeyboard`/`GetKeyEdge`** when **`bPollKeyboard`** |

### `pDirectKeyb` @ +0x204

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Heap alloc **0x20c** | `CGame_StartGame@0x00413f30` | `OperatorNew(0x20c)` → **`CDirectKeyb_ctor`** |
| Owner field | `CGame__SchedulerDispatch` | **`MOV ECX,[ESI+0x204]`** before Poll/GetKeyEdge (R4-07) |
| Ghidra type | MCP | **`CDirectKeyb * pDirectKeyb`** @ offset **516** |

### Audio / cmdline tail @ +0x22c..+0x247

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Alias **`CBulanci+0x4b0..`** | R3-01 | **`0x284+0x22c=0x4b0`**; ctor/dtor/`ReleaseAudioBank` use **`(game).pAudioBankArray`** |
| **`dwReserved_beforeBankCount`** | `CBulanci_ctor@0x0040275b` | Zero store; no reads |
| **`pCmdLine`** | `CBulanci_SetCmdLine@0x00401992` | **`&(game).pCmdLine`** |

### Lobby / net / demo block (+0x19c..+0x220)

| Offset | Name | Evidence |
|--------|------|----------|
| `+0x19c` | `bGameModeIndex` | **`0x04`** recv `CGame_ProcessNetMessage`; mode helpers |
| `+0x19d` | `bAdminByte` | **`0x64`** recv |
| `+0x19e` | `anScoreLimitByMode[4]` | Indexed sends/recvs |
| `+0x1ac` | `pOnNetCustomStream` | **`CGame_SetCommStrm@0x00414270`**; dtor **`[ESI+0x1ac]`** Release; **`0x15`** custom script |
| `+0x1b0` | `demoJournalStream` (**embed**) | Dtor **`LEA ECX,[ESI+0x1b0]`** → **`CDSEasyMemStream_dtor`**; **`OpenNetworkSession`** **`CDSEasyMemStream` alloc**; scheduler journals recv |
| `+0x1d8` | `pDirectPlay` | **`Receive`/`EnumSessions`** **`[ESI+0x1d8]`** |
| `+0x1dc` | recv / DirectPlay slot | **`OpenNetworkSession`** stores dp **`@0x00414f6d`**; **`Receive`** **`buf@+0x1dc`**; grow **`@0x0041622c`** |
| `+0x1e0` | `dwRecvBufSize` / free | **`Receive &size`**, dtor **`Runtime_Free`**, reset free |
| `+0x1e4`/`+0x1e8` | countdown tick/byte | Scheduler case **2**; net **`0x07`** |
| `+0x1f0` | `pActiveCGaming` | Session list vs **`CGaming*`** |
| `+0x1f4` | `pLobbyView` | Match net dispatch **`CGaming*`** target |
| `+0x209` | `bDemoRecording` | **`CGaming_BeginDemoRecording`** |
| `+0x20c` | `bTeamScoreCategory` | Scheduler case **6** strcmp @ **`ESI+0x20d`** (Ghidra **`bTeamScoreCategory`** @ 524) |
| `+0x220` | `pSchedTeamScoreScratch[3]` | Case **6** zero **`[0]`** |

### Chain-interior lobby fields (abs offsets, overlap `CDSChain_full`)

| Offset | Role | Evidence |
|--------|------|----------|
| `+0x86` | `bHostFlag` | **`CGame_ResetForLobby`** **`=1`**; admin send host gate |
| `+0xbc` | `levelNameList` vector | **`CGame_LevelList_AddByName`**, **`OpenNetworkSession`** loop @ **`LEA [EBX+0xbc]`** |
| `+0xcc` | `hLevelTitle` | Dtor string release; net set-level path |
| `+0xd0` | lobby scratch | Reset / open-session zero |

### Profile bind / scheduler child vectors

| Offset | Type | Evidence |
|--------|------|----------|
| `+0x20` | `CDSPtrSlotVec profileBindSlotVec` | Ctor zero; dtor **`CDSPtrSlotVec_Resize@0x00414d97`** |
| `+0x1f8` | `CDSPtrSlotVec schedChildSlotVec` | Dtor **`@0x00414cf1`** |

## 4. Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `modify_struct_field` | `pPad_20` → **`profileBindSlotVec`** `CDSPtrSlotVec` @ +32 | done |
| `modify_struct_field` | `pPad_1f8` → **`schedChildSlotVec`** `CDSPtrSlotVec` @ +504 | done |
| `add_struct_field` | **`pOnNetCustomStream`** `pointer` @ 430 | done (offset **+0x1AE**; asm uses **+0x1AC**) |
| `modify_struct_field` | lobby/net/demo renames (`bGameModeIndex`, `bDemoRecording`, `schedTeamScoreTagBuf`, …) | done |
| `set_decompiler_comment` | `0x00414aeb`, `0x0040a062`, `0x004160a0`, `0x00416167`, `0x00414d1f`, `0x00414d2f` | done |
| `modify_struct_field` | **`demoJournalStream`** embed `CDSEasyMemStream` @ +0x1B0 | **blocked** — needs delete **`pDemoJournalStream_iface` + pad** (44 B) |
| `save_program` | `bulanci.exe` | **saved** |

### `get_struct_layout CGame` (post-pass)

584 B — key tail: **`pDirectKeyb` @ 516**, **`pAudioBankArray` @ 556**, **`pCmdLine` @ 576**, **`schedChildSlotVec` @ 504**, **`pOnNetCustomStream` @ 430**.

## 5. Struct doc updates

- [CGame.md](./CGame.md) — full layout table, scheduler slot map, Ghidra apply, follow-up/UNK
- [CBulanci.md](./CBulanci.md) — no change (embed sizing already verified R3-01 / R4-12)

## 6. Remaining UNK

- **`demoJournalStream`**: runtime is **44 B embed @ +0x1B0**; Ghidra still **`pointer` @ +0x1B8** + 28 B pad — retype pass required.
- **`pOnNetCustomStream`**: dtor/asm **`+0x1AC`**, Ghidra **`+0x1AE`** (+2 vs `anScoreLimitByMode` tail).
- **`+0x1DC`**: **`CDSDirectPlay*`** after **`OpenNetworkSession`**, recv **heap buffer** after **`DPERR_BUFFERTOOSMALL`** — document or split fields.
- **`+0x28..+0x2f`**: ctor **`capacity=8`** tail after **`profileBindSlotVec`** (8 B typed).
- **Chain interior @ +0x86..+0xd0`**: lobby vectors/strings physically inside **`CDSChain_full`** — needs **`CDSChain_full` tail extension** or explicit overlay struct.
- **`CGaming` stack `game` (872 B)** vs embedded **`CGame`** — separate reconciliation (R3-12).
