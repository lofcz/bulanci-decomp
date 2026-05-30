# Round 4 — Task 25 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 25 |
| **round** | 4 |
| **title** | Xref heap CDSVideoPlayer classId 0x31 factory consumer path |
| **source** | `agent_todos_50_r4.json` (handoff from R3 todo 25) |
| **supersedes_todo_id** | 25 |
| **prior** | [round3_task_25_report.md](./round3_task_25_report.md) |
| **types** | `CDSVideoPlayer`, `CDSAudioVideoPlayer`, `CMovieView` |

## Status

**DONE** — Factory registration, dispatch, and stream-driven consumer paths traced; no direct `CALL` to heap factory; gameplay movie path confirmed embedded-only.

## Evidence

### Registration (static init)

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Class id **0x31** registered once | `CDSVideoPlayer_StaticClassRegister@0x0047d750` | `PUSH 0x439f50` (factory), `PUSH g_pCDSDsmFileClassName`, `PUSH 0x31`, `MOV ECX,0x4b834c`, `CALL HandleClassRegister@0x0042e910` |
| Meta / typeinfo node | `CDSVideoPlayer_GetTypeInfo@0x00439d10` | `return &DAT_004b834c` |
| Static init caller | xref `0x0047f584` | DATA — CRT static init table only |
| `PUSH 0x31` in .text | program-wide | **1** site (`search_instructions`) — registration only, not gameplay |

### Heap factory (no direct callers)

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Alloc **0x48** + `ConstructTrackManager` | `CDSVideoPlayer__CreateTrackManagerHeap@0x00439f50` | `OperatorNewWithBadAlloc(0x48)` @ `0x00439f9a` |
| Sole xref to factory entry | `get_xrefs_to(0x00439f50)` | **DATA** @ `0x0047d751` inside `StaticClassRegister` — no `CALL` sites |
| Teardown when heap-owned | `CDSVideoPlayer_ScalarDeletingDtor@0x00439fc0` | `TM_Destructor` + `_free` when `freeFlag&1` |

### Runtime dispatch (`InitializeByClassId`)

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Fast path | `InitializeByClassId@0x0042ef00` | `classId < 0x1000` → `g_apClassByIdTable[classId]` → `CALL [entry+0xc]` (factory) |
| Slow path | same | Walk `g_pClassRegHead` comparing `node[2] == classId` |
| **Callers** (stream-driven class ids) | `CDSChain_Append@0x0042fbe5` | Loop: stream `vtable+0x10` read → `InitializeByClassId(classId, …)` → append child |
| | `CDSCollection_DeserializeElement@0x0042fd96` | Stream read → `InitializeByClassId(local_18, …)` |
| | `CDSStreamStorage_CreateFilterSafeStream@0x00434791` | Same factory dispatch pattern |

### Gameplay path (embedded, not heap factory)

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Movie playback alloc **0x50** | `CMovieView::StartPlayback@0x00422d50` | `OperatorNew(0x50)` → `CDSAudioVideoPlayer_Constructor` → `m_pPlayer` |
| Embedded TM | `CDSAudioVideoPlayer_Constructor@0x0043bca0` | `ConstructTrackManager(&videoTrackManager, …)` @ parent `+0x08` |
| Render target handoff | `StartPlayback@0x00422d50` | `ODSImage__SetImage(trackImage, pPlayer->videoTrackManager.pRenderTarget)` |
| Stop uses typed parent | `CDSAudioVideoPlayer_Stop@0x0043bc00` | `CDSAudioPlayer_Stop(this->pAudioPlayer, 1)` — `set_function_this_type` **CDSAudioVideoPlayer \*** (R4) |

### Shipped content

| Claim | Evidence |
|-------|----------|
| No hardcoded `classId==0x31` in gameplay | Only `CMP EAX,0x31` hit in `CGame_ProcessNetMessage@0x004152ec` is **DPlay system message** `0x31`, unrelated to `CDSVideoPlayer` registry |
| Heap factory reachable in principle | Any `.dsm` / collection stream that serializes **class id 0x31** would hit `CreateTrackManagerHeap` via `InitializeByClassId`; **no such immediate** in binary text and no retail asset scan in-repo |

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `set_decompiler_comment` | `0x0047d750`, `0x0042ef00`, `0x00439f50`, `0x0042fd96`, `0x0043bc00` | R4 todo 25 factory/dispatch notes |
| `set_function_this_type` | `CDSAudioVideoPlayer_Stop@0x0043bc00` | `CDSAudioVideoPlayer *` in class namespace |
| `get_xrefs_to` / `search_instructions` | factory + `PUSH 0x31` | Confirmed DATA-only factory xref |
| `save_program` | `bulanci.exe` | saved |

## Struct doc updates

- [CDSVideoPlayer.md](./CDSVideoPlayer.md) — R4 factory consumer path table
- [CDSAudioVideoPlayer.md](./CDSAudioVideoPlayer.md) — R4 embedded vs heap dispatch cross-ref

## Remaining UNK

- Whether any **shipped** overlay / level resource embeds deserialize **class id 0x31** (engine path exists; no in-repo asset proof).
- `g_apClassByIdTable[0x31]` fast-path slot vs linked-list-only lookup for this registration (no `InitializeClassIdLookup(&DAT_004b834c)` in `CBulanci_ctor`; slow list walk still resolves `0x31` after static init).
