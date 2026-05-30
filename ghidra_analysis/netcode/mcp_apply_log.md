# Netcode Ghidra MCP apply log — 2026-05-30

Sources: 10 subagent transcripts under `agent-transcripts/c2d034a0-a20b-4c70-a075-0dc4d6801bf2/subagents/`.

## Part 1 — `net_protocol.md` updates (applied)

| Section | Change |
|---------|--------|
| Diagram header | Max opcode 0x19; 0x1A–0x63 dropped |
| CGaming offsets | `+0x1D8` = `CDSDirectPlay*`; `+0x1DC` = recv buffer; `+0x1E0` = recv size |
| Connect path | Host uses `ConnectLobby`; `ConnectTCP` join-only |
| Wire format | No encryption; app payload is type@0 only; queue u32 len internal; recording journal separate |
| Opcode table | 0x0C `[slot, weaponKind]`; 0x10 `[hit_slot, face, x, y]`; 0x19 wire `+1 world_slot, +2 kind, +3 player_slot` |
| 0x08/0x09 | Inline in `CGame_StartGame` @ `0x413ce0` |
| State gates | 0x16/0x17 ungated on receive |
| Replication | 0x0C weapon sync; 0x10 hit/respawn corrected |

## Part 2 — Renames

### Applied (was FUN_* or wrong name)

| Address | New name | Was |
|---------|----------|-----|
| `0x0043c1f0` | `CDSMemQueue_Lock` | `FUN_0043c1f0` |
| `0x0043c200` | `CDSMemQueue_Unlock` | `FUN_0043c200` |
| `0x0043c210` | `CDSMemQueue_Read` | `FUN_0043c210` |
| `0x0043c590` | `CDSMemQueue_Configure` | `FUN_0043c590` |
| `0x004470f0` | `CDSWorkingThread_ThreadProc` | `FUN_004470f0` |
| `0x004471d0` | `CDSWorkingThread_Start` | `FUN_004471d0` |
| `0x0043ab40` | `CDSDirectPlay_EnumSessionsCallback` | `LAB_0043ab40` (label) |
| `0x004128f0` | `CGame_IsLocalPlayerSlot` | `FUN_004128f0` |
| `0x00416720` | `CBulanek_IsHumanPlayer` | `FUN_00416720` |
| `0x00412640` | `CMenu_GetProfileBindIndex` | `FUN_00412640` |
| `0x00412ed0` | `CStartGame2_ApplySlotCountIfChanged` | `FUN_00412ed0` |
| `0x004144f0` | `CGame_DeferTeamScoreSchedulerEvent` | `FUN_004144f0` |
| `0x00412520` | `CGaming_BeginDemoRecording` | `FUN_00412520` |
| `0x004129c0` | `CGaming_OnResumeSyncRoundTimer` | `FUN_004129c0` |
| `0x00412fa0` | `CGaming_RegisterPreMatchSchedulerHooks` | `FUN_00412fa0` |
| `0x00413b90` | `CGame_NetSendChat_t06` | `NetSendChat` |

### Skipped (already correct)

| Address | Current name |
|---------|--------------|
| `0x00415f80` | `CGaming_DrainRecordedNetMessages` |
| `0x00416030` | `CGame::SchedulerDispatch` |

## Part 2 — Comments & bookmarks

| Action | Address | Status |
|--------|---------|--------|
| Plate `NET_DISPATCH` | `0x00415290` | OK |
| Jump-table note | `0x00415334` | OK (decompiler PRE; plate failed — not a function entry) |
| PRE `NET_MSG_0xNN` | all `CGame_NetSend*` @ `0x4124a0`–`0x414550` + `0x413ce0` | OK |
| PRE recv handlers | `0x41f210`, `0x4209f0`, `0x420510`, `0x420a70`, `0x4180c0`, `0x41d2c0`, `0x41f030`, `0x41d280`, `0x41f010`, `0x417e80`, `0x4185a0` | OK |
| Disasm recv pump | `0x00416208` | OK |
| Bookmark `NET_DISPATCH` | `0x00415290` | OK |
| Bookmark `NET_RECV_PUMP` | `0x00416030` | OK |
| Bookmark `NET_REPLAY_DRAIN` | `0x00415f80` | OK |
| Bookmark `NET_SESSION_OPEN` | `0x00414dd0` | OK |
| Bookmark `NET_SEND_t08_t09_StartGame` | `0x00413ce0` | OK |
| `save_program bulanci.exe` | — | OK |

## MCP call log (success / fail)

All rename/comment/bookmark/save calls returned success unless noted.

| Tool | Address / target | Result |
|------|------------------|--------|
| `rename_function_by_address` | 16 functions (see table) | 16× success |
| `rename_or_label` | `0x0043ab40` | success (label; not a standalone function) |
| `set_plate_comment` | `0x00415290` | success |
| `set_plate_comment` | `0x00415334` | **fail** — no function at address |
| `set_decompiler_comment` | 35 addresses (send/recv/dispatch/jump) | 35× success |
| `set_disassembly_comment` | `0x00416208` | success |
| `set_bookmark` | 5 addresses | 5× success |
| `save_program` | `bulanci.exe` | success |

## Failures / notes

1. **`0x00415334` plate comment** — address is mid-function switch code, not a function entry; applied equivalent note via `set_decompiler_comment` instead.
2. **`0x0043ab40`** — EnumSessions callback is a **label** inside the enum path, not a Ghidra function; renamed via `rename_or_label`.
3. **EOL disassembly on every send** — PRE layout comments applied at all send/recv entries; full EOL sweep on every `CDSDirectPlay_Send` call site not done (optional per transport agent pattern @ `0x41965e`).
4. **`CGame_SchedulerDispatch` rename** — skipped; already `CGame::SchedulerDispatch` in Ghidra (decompiler may still show `CGame__SchedulerDispatch`).

## Subagent sources

| ID | Title | Key contribution |
|----|-------|------------------|
| bb5009b7 | Net transport framing | Queue u32 len, no crypto, CGaming+0x1D8/+0x1DC fix |
| fc5c5dc9 | Net dispatch switch | Jump table @ 0x415e14; 0x1A–0x63 dropped; 0x19 wire order |
| 8b10fb85 | Net msgs 0x00–0x0F | 0x0C slot+weaponKind; 0x08/0x09 inline StartGame |
| 8f1cb484 | Net msgs 0x10–0x1F | 0x10 hit_slot+face+xy; 0x19 handler wire order |
| 6a03a855 | Net msgs 0x20–0x2F | Confirmed max opcode 0x19 |
| d78c5f43 | Net send functions | Full send index @ 0x4124a0–0x414550 |
| f07ca6f5 | Net recv handlers | Recv matrix; 0x0C/0x10 corrections |
| e4a0a0ff | Netcode xref | ConnectLobby host path; verification table |
| 721334cd | Net vtables FUN_* | EnumSessions callback @ 0x3ab40 |
| 33badbe5 | Net Ghidra doc synthesis | Intended MCP changelog (applied here) |

---

## Gameplay impact pass — 2026-05-30 (subagent `76e02c13` + lobby/moderation/authority)

### Repo docs

| File | Action |
|------|--------|
| `ghidra_analysis/netcode/gameplay_impact.md` | **Created** — master opcode table with addresses, GAMEPLAY_IMPACT, gates |
| `ghidra_analysis/netcode/mcp_apply_log.md` | Appended (this section) |

### Ghidra changes

| Action | Detail |
|--------|--------|
| EOL fix + GAMEPLAY_IMPACT | **0x0C**, **0x10**, **0x19** send/recv (verified via decompile before write) |
| GAMEPLAY_IMPACT | All opcodes **0x00–0x19**, **0x64** at send/recv/dispatch case sites |
| Plate `NET_DISPATCH` @ `0x00415290` | Appended **AUTHORITY** block (host/human gates, 0x16/0x17 ungated, journal 0x0C–0x19) |
| Plate pause alias @ `0x004124a0` | Documents `0x0A` = pause status, not lobby kick |
| Rename | `CMenu_NetSendKickAll` → **`CMenu_NetSendLobbySyncAll`** @ `0x00414640` |
| `save_program` | `bulanci.exe` — success |

### MCP call log (this pass)

| Tool | Count / target | Result |
|------|----------------|--------|
| `get_function_by_address` | 20+ handlers (verify before comment) | all success |
| `decompile_function` | 0x0C/0x10/0x19 send+recv, dispatch | success (EOL verified) |
| `set_decompiler_comment` | **52** addresses (see list below) | 52× success |
| `set_plate_comment` | `0x00415290`, `0x004124a0` | 2× success (plate warnings only) |
| `rename_function_by_address` | `0x00414640` → `CMenu_NetSendLobbySyncAll` | success |
| `save_program` | `bulanci.exe` | success |

**Addresses commented (`set_decompiler_comment`):**

`0x00414dd0` `0x00415390` `0x00415741` `0x004139b0` `0x00415980` `0x00412c70` `0x00415960` `0x00412cd0` `0x00415940` `0x00414340` `0x00415a20` `0x00413b90` `0x00415a50` `0x00412da0` `0x00415aa0` `0x00413ce0` `0x00415b80` `0x00415bc0` `0x004124a0` `0x00415c20` `0x00412d40` `0x00415de0` `0x00413180` `0x00415c80` `0x00412a40` `0x0041f210` `0x00412b10` `0x004209f0` `0x00412b60` `0x00420a70` `0x00412b90` `0x00417e80` `0x00412bf0` `0x00420510` `0x00412c40` `0x004180c0` `0x00412a00` `0x00415d00` `0x00412950` `0x0041d2c0` `0x00412990` `0x0041f030` `0x00412df0` `0x004185a0` `0x00414550` `0x00415151` `0x004138b0` `0x00415156` `0x00412ac0` `0x0041d280` `0x00412a80` `0x0041f010`

**Note:** `0x00420510` is Ghidra entry for `CGaming_OnNetMsg_t10_Hit` (docs shorthand `0x420510`).

### Failures

None.

---

## Session FUN_* verification pass — 2026-05-30

Cross-checked all addresses from spawn / spatial / net / gameplay subagents via `get_function_by_address`.

### Applied (were still `FUN_*` or interim names)

| Address | New name | Was |
|---------|----------|-----|
| `0x00420480` | `CGaming_OnCustomEvent_0xF6_RespawnPlayer` | `CGaming_OnCustomEvent_RespawnPlayerChain` / `FUN_00420480` |
| `0x00417a90` | `CDeath_OnCorpseHideRequestRespawn` | `FUN_00417a90` |
| `0x004169a0` | `CGame_PostTeamScoreHudEvent_0xEE` | `CGame_EnqueueLobbySpellOnEvent` |
| `0x00416a10` | `CGame_PostOpposingHudEvent_0xEF` | `CGame_EnqueueLobbySpellOffEvent` |
| `0x00417260` | `CBulanek_ArmFireDelayScheduler` | `CBulanek_ArmPostDamageScheduler` / `FUN_00417260` |

### Skipped (already correct in Ghidra)

Net transport (`CDSMemQueue_*`, `CDSWorkingThread_*`), `CGame_IsLocalPlayerSlot`, `CBulanek_IsHumanPlayer`, `CMenu_GetProfileBindIndex`, `CStartGame2_ApplySlotCountIfChanged`, `CGame_DeferTeamScoreSchedulerEvent`, `CGaming_BeginDemoRecording`, `CGaming_OnResumeSyncRoundTimer`, `CGaming_RegisterPreMatchSchedulerHooks`, `CGame_NetSendChat_t06`, `CMenu_NetSendLobbySyncAll`, `CGaming_SpawnPlayerAtSlot`, `CGame_AllSlotsHaveState`, `CDSChained_GetFirstChildView` / `GetNextSiblingView`, `CGame_ProcessNetMessage`, `CGame::SchedulerDispatch`, `CGaming_DrainRecordedNetMessages`, `SpatialQuery`, `CDSRect_Overlaps`, `CBulanek_PostScriptEvent`, `CGaming_GetObjectAtSlotSafe`.

### Skipped (by design)

| Address | Reason |
|---------|--------|
| `0x004124a0` | Kept `CGame_NetSendKick_t0a`; plate documents pause (`0x0A`) alias |
| `0x0043ad55` | Orphan tail — not renamed |
| `0x0042f730` | No session-agreed symbol (SpatialQuery helper only) |

### Still `FUN_*` in docs / mapping.csv but outside “understood + named” scope

Combat helpers referenced in `damage_pipeline.md` only (`FUN_004183d0`, `FUN_0041a2f0`, `FUN_0041d090`, `FUN_00419a00`, etc.) — behavior documented, export names not proven.

`save_program bulanci.exe` — success.

---

## Subagent struct apply pass — 2026-05-30

Applied read-only subagent proofs (`dc3e2641`, `1b07f61b`, `70ba8b7c`, `effb5806`).

| Struct | Size | Action |
|--------|------|--------|
| `CDSWorkingThread` | **0x14 (20)** | Created; embedded in `CDSDirectPlaySender.thread` @ +4 (sender still **0x50**) |
| `CObstacle` | **0x88 (136)** | Full CGameView-shell layout (bounds @ +0x74, `bActive` +0x69, `slotId` +0x70, …) |
| `CBulanek` | **0x19C (412)** | `nLivesRemaining` @ **+0x18C**, `nLivesStashMax` @ +0x190, `nSpeedParam` @ +0x194 |
| `CGame` | **0x368 (872)** | Sparse proven fields (`netState`, `bIsHost`, `pDirectPlay`, recv buf, `bDemoRecording`, `pOnNetCustomStream`, …) |
| `CGaming` | **0x36C (876)** | `pVftable_primary` + embedded **`CGame game` @ +4** |

### Renames (same pass)

| Address | New name |
|---------|----------|
| `0x004470c0` | `CDSWorkingThread_PollExited` |
| `0x00447190` | `CDSWorkingThread_StopAndJoin` |

### Docs

- `ghidra_analysis/gameplay/damage_pipeline.md` — lives decrement only on **0xD7**; `CBulanek_IsInKnockdownAnimBand` not life drain.

### Not applied (deferred)

- Full **CGame** lobby field split (40+ proven offsets from `70ba8b7c` — only critical net/lobby anchors in Ghidra).
- `CGame` nested retype of `CGame_ProcessNetMessage` `this` (manual prototype pass).

`save_program bulanci.exe` — success.

---

## CGame full field-split pass — 2026-05-30 (subagent `70ba8b7c`)

Rebuilt **`CGame`** @ **0x368 (872)** from proven decomp offsets (`CGame_ProcessNetMessage`, `CGame__SchedulerDispatch`, net send/recv, `CMenu_OpenNetworkSession`, `CGame_StartGame`, `CGame_ResetForLobby`).

| Region | Offset | Fields |
|--------|--------|--------|
| Scheduler / hub | +0x04, +0x1C | `scheduler[24]`, `eventHub` |
| Session | +0x30..+0x37 | `netState`, `localPlayerDpid`, `dpInitByte`, `bIsHost`, `profileBindMode` |
| Level index | +0x66, +0x6E | `pLevelResourceIndex`, `levelResourceCount` |
| Lobby UI | +0x86..+0x89 | `lobbyActive`, `selectedModeSlot`, `scoreLimitBySlot[4]` |
| Level / roster | +0xBC..+0xDB | `levelList`, `levelTitleAux`, `randSeed`, slot bytes |
| Players | +0xDC | `PlayerLobbyRec[4]` (**0x23** stride) |
| Bindings | +0x168 | `NetSlotBinding[4]` (**0x0D** stride) |
| Mode / admin | +0x19C..+0x19E | `gameModeIndex`, `adminByte`, `scoreLimitByMode[4]` |
| Net custom / demo | +0x1AC, +0x1B8 | `pOnNetCustomStream`, `pDemoJournalStream` |
| DirectPlay | +0x1D8..+0x1E8 | `pDirectPlay`, `pRecvBuf`, `recvBufSize`, countdown |
| Runtime ptrs | +0x1F0..+0x209 | `pActiveCGaming`, `pLobbyView`, `pDirectKeyb`, poll/demo flags |
| Endgame | +0x20C..+0x223 | team score scratch, `roundTimerSecs` |

Also created **`PlayerLobbyRec`** (35 B), **`NetSlotBinding`** (13 B). **`CGaming`** = vftable + embedded **`CGame game` @ +4** (876 B).

Minor layout gaps: `levelTitle` @ +0xCC may share bytes with padding before `levelTitleAux` (+0xD0); `pOnNetCustomStream` @ +0x1AE vs doc +0x1AC (+2).

`save_program bulanci.exe` — success.

---

## Struct layout verification pass — 2026-05-30

Session docs (`net_protocol.md`, transport/spawn/combat agents) described struct sizes/members; Ghidra had many **1-byte placeholders**. Applied via MCP (`create_struct`, `delete_data_type`, `add_struct_field`, `modify_struct_field`).

### OK (size + members match evidence)

| Struct | Size | Notes |
|--------|------|--------|
| `CDSDirectPlay` | **0x8C (140)** | vftable, COM ptrs, GUIDs, connection, flags, `localPlayerId`, embedded `sender` @ +0x3C |
| `CDSDirectPlaySender` | **0x50 (80)** | vftables/thread @ +0x04, `CDSMemQueue` @ +0x18, scratch, event, stop, COM @ +0x48/+0x4C |
| `CDSMemQueue` | **0x20 (32)** | Renamed fields to match `CDSMemQueue_Read` (+0x04/+0x0C/+0x10/+0x1C) |
| `CDSRect` | **0x10** | Already correct |
| `CDsmHeader` | **36** | Already correct (formats pass) |
| `SchedulerEventSlot` | **16** | `flags`, `lastFireMs`, `delayMs`, `id` (`tick_system.md`) |

### Partial (key fields added; gaps remain)

| Struct | Size | Applied | Still open |
|--------|------|---------|------------|
| `CGaming` | **0x36C (876)** | `gameState` +0x30, `bIsHost` +0x36, `pDirectPlay` +0x1D8, `pRecvBuf` +0x1DC, `recvBufSize` +0x1E0, `bDemoRecording` +0x209, `pEntityListHead` +0x31C | Large pads unnamed; `CGame` subobject @ +4 not split |
| `CBulanek` | **0x188 (392)** | `stateByte` +0x70, `schedulerBase` +0x88, `bPendingRespawn` +0x16A, collision box | `+0x18C` lives **past** 392 B in docs — needs larger object type or CDSView base; bytes 0–111 still anonymous |
| `CDSView` | **128** | `bSkipSpatialAt6a` @ +0x6A | Total `CDSView`/`CGameView` size not proven; 128 B is a lower bound |
| `CGame` | **868** | Opaque body placeholder | Not field-split; use `CGaming` offsets for net/lobby |

### Still placeholder / not in Ghidra

| Struct | Session refs | Status |
|--------|----------------|--------|
| `CObstacle` | SpatialQuery AABB entities | **Applied** — see Subagent struct apply pass |
| `CDSWorkingThread` | Sender secondary base | **Applied** — see Subagent struct apply pass |

`save_program bulanci.exe` — success (superseded by subagent apply pass below).
