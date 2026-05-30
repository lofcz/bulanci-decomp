# Round 5 — worker 22 report (CGame UNK/layout)

## Task

| Field | Value |
|-------|-------|
| **id** | 22 (parent dispatch) |
| **manifest id 22** | CTeleportPoint `pOverlap_entity` (not executed this pass) |
| **executed scope** | **CGame.md** remaining UNK/layout from R4 [`pass_r4_CGame_report.md`](./pass_r4_CGame_report.md) / [`CGame.md`](./CGame.md) Follow-up |
| **source** | R5 coordinator handoff / parent WRITE MODE |
| **r4_unk_summary** | `+0x28..+0x2f`, chain-interior lobby vectors, `demoJournalStream` embed, `pOnNetCustomStream` offset, `+0x1DC` DirectPlay/recv multiplex, `pLobbyView` vs `pActiveCGaming` |

## Status

**PARTIAL** — Evidence pinned for all listed UNK except **`demoJournalStream` Ghidra retype** (needs delete/replace of `pDemoJournalStream_iface` + `pPad_netBlock` band). Ghidra: profile-bind reserved fields, `pDirectPlayActive` rename, comments, `save_program`.

## Evidence

### `+0x28..+0x2f` → `dwProfileBindReserved` / `nProfileBindPreallocCap`

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Ctor init `{0, 8}` | `CGame_ctor@0x00414a80` | After `profileBindSlotVec` zero: `field_0x28=0`, `field_0x2c=8` @ `0x00414ac0` band |
| No `CDSPtrSlotVec_Resize` on tail | `CGame_dtor@0x00414ca0` | Only `CDSPtrSlotVec_Resize(param_1+8)` — first vec @ `+0x20` |
| EnumSessions stack only | `CDSDirectPlay_EnumSessions@0x00416167` | Copies `profileBindSlotVec` + `field_0x28` into `DPENUMSESSIONS2` locals |
| No other writers | program search | `field_0x28` / `field_0x2c` on `CGame` — ctor + EnumSessions only |

### `pOnNetCustomStream` @ `+0x1AC` (not `+0x1AE`)

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Assign/release | `CGame_SetCommStrm@0x00414270` | `[this+0x1ac]` store; prior Release on same offset |
| Dtor | `CGame_dtor@0x00414ca0` | `param_1[0x6b]` → `0x1AC` |
| Ghidra field | `get_struct_layout` | `pOnNetCustomStream` @ **430** = `0x1AE` — **+2** vs asm due to `pScoreLimitByMode[4]` consuming 16 B through `+0x1AD`; asm uses **`+0x1AC`** |

### `demoJournalStream` embed @ `+0x1B0`

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Init | `CMenu_OpenNetworkSession@0x00414ff8` | `CDSEasyMemStream_InitBackingBuffer((CDSEasyMemStream *)&pOnNetCustomStream+2, 0x2000, 0x1000)` → **`+0x1B0`** |
| Dtor | `CGame_dtor@0x00414ca0` | `CDSEasyMemStream_dtor(param_1+0x6c)` → **`+0x1B0`** |
| Size | `CDSEasyMemStream` | **44 B** (`0x2C`) — ends **`+0x1DB`**; net fields follow @ **`+0x1DC`** |
| Scheduler journal | `CGame__SchedulerDispatch@0x00416030` | Case **0**: `pDemoJournalStream_iface` vfn writes when `bDemoRecording` (decompiler name; base **`+0x1B0`**) |

### `+0x1D8` / `+0x1DC` DirectPlay vs recv buffer

| Phase | `+0x1D8` `pDirectPlay` | `+0x1DC` `pDirectPlayActive` | Evidence |
|-------|------------------------|------------------------------|----------|
| Ctor | `NULL` | `NULL` | `CGame_ctor@0x00414b69` / `0x00414b7a` |
| Lobby open | often `NULL` | **`CDSDirectPlay*`** | `OpenNetworkSession@0x00414f6d` `this->pRecvBuf=pCVar7`; sends use `this->pRecvBuf` |
| Match start | **`CDSDirectPlay*`** | (session copy) | `CBulanci_CopyNetSessionFields@0x0040275b` `(game).pDirectPlay=param_2` |
| Scheduler recv | **`CDSDirectPlay*`** dp | **heap buffer** | `Receive(pDirectPlay, …, pRecvBuf, &dwRecvBufSize)` @ `0x00416167`; grow `FUN_0042f730` → `pRecvBuf` @ `0x0041622c` |
| Teardown | — | Release COM if non-null | `CGame_TeardownNetworkSession@0x00412420` `[param_1+0x1dc]` vfn+8 |

### `pActiveCGaming` vs `pLobbyView`

| Slot | Offset | Runtime type | Evidence |
|------|--------|--------------|----------|
| `pActiveCGaming` | `+0x1F0` | **`CSessionList*`** (lobby) / **`CGaming*`** (match) | Case **1** `CSessionList_GetPick((CSessionList*)pActiveCGaming)` @ `0x004162bd`; case **7** `DispatchPlayerAction((CGame*)pActiveCGaming,…)` @ `0x00416030` |
| `pLobbyView` | `+0x1F4` | **`CGaming*`** (in-match net) | `CGame_ProcessNetMessage` opcodes **`0x0C..0x19`** cast `(CGame*)pLobbyView`; `CMenu_DoModalChild` assign/clear @ `0x0041305d` |

### Chain-interior lobby fields (abs `CGame` offsets)

| Offset | Role | Evidence |
|--------|------|----------|
| `+0x86` | `bHostFlag` | `CGame_ResetForLobby` stores `1` |
| `+0xbc` / `+0xc4` | `levelNameList` / count | `FUN_004146b0` vector; `CGame_LevelList_AddByName((chain).pPad_a4+2,…)` @ `0x00414934` |
| `+0xcc` | `hLevelTitle` | Dtor `param_1[0x33]`; set-level net path |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `add_struct_field` | `dwProfileBindReserved` @ 40, `nProfileBindPreallocCap` @ 44 | done |
| `modify_struct_field` | `pRecvBuf` → `pDirectPlayActive` | done |
| `set_decompiler_comment` | `0x00414ac0`, `0x00414f6d`, `0x00414270`, `0x00414d1f` | done |
| `get_struct_layout` | `CGame` | 584 B; profile reserved + `pDirectPlayActive` |
| Retype `demoJournalStream` embed | `+0x1B0` | **not applied** — would remove `pDemoJournalStream_iface` + 28 B pad |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CGame.md](./CGame.md) — layout table, UNK/Follow-up, R5 Ghidra apply section

## Remaining UNK

- **`demoJournalStream` Ghidra struct band** @ `+0x1B0..+0x1DB` — runtime embed proven; editor still pointer + `pPad_netBlock`.
- **`pOnNetCustomStream` Ghidra offset** — field at 430 (`+0x1AE`) vs asm `+0x1AC` (+2 layout drift).
- **`nProfileBindPreallocCap`** — no allocator consumer found (ctor constant `8`).
- **`pActiveCGaming` typed as `CGaming*`** — deferred to R5 task **19** (polymorphic lobby/match).
