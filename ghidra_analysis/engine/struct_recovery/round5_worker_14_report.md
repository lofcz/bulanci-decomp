# Round 5 — Worker 14 report

## Task

| Field | Value |
|-------|--------|
| **worker** | 14 / 50 |
| **mode** | WRITE |
| **scope** | `FUN_*` network / DirectPlay / session cluster @ `0x00428000`–`0x00450000` |
| **deliverable** | Proof table + Ghidra deltas (evidence-only) |

**Note:** Lobby/session UI (`CSessionList`, `CMenu_OpenNetworkSession`, …) lives **below** `0x00428000` (`0x0040xxxx`–`0x00417xxx`). This slice’s session tie-in is **pre-match modal → `CGaming_DrainRecordedNetMessages`** after modal close.

## Status

**PARTIAL** — DirectPlay transport block was already fully named (prior netcode pass); two pre-match `FUN_*` renamed; false-split duplicate removed; `__fastcall` blocks `set_function_this_type`. `save_program bulanci.exe`.

## DirectPlay / CDSMemQueue cluster (already named)

| Address range | Symbols | Count |
|---------------|---------|------:|
| `0x0043aae0`–`0x0043b6d0` | `CDSDirectPlay_*`, `CDSDirectPlaySender_*`, `CDSDirectPlay_Send` | 28 |
| `0x0043c1d0`–`0x0043c590` | `CDSMemQueue_*` (Lock/Unlock/Read/Configure, …) | 9 |
| `0x0043ab40` | `CDSDirectPlay_EnumSessionsCallback` (label; not standalone func) | 1 |

No remaining `FUN_*` in `0x0043aae0`–`0x0043b6d0`.

## Proof table — scope `FUN_*` @ `0x00428000`–`0x00450000`

| Address | Was | Action | Status | Evidence (func @ addr) |
|---------|-----|--------|--------|---------------------------|
| `0x00439880` | `FUN_00439880` | **Renamed** `CGaming_PreMatchModal_ArmSchedulerSlot0` | DONE | Sole caller `CGaming_RunPreMatchModal@0x0041c290` cmd **0**: `if (*piVar3==0) FUN_00439880(piVar3[1])`; body tests `+0x35==0`, `Scheduler_GetEventSlot(this+4,0)`, `Scheduler_ArmSlot(this+4,0)` @ `0x00439880` |
| `0x0043aa70` | `FUN_0043aa70` | **Renamed** `CGaming_PreMatchModal_RestartDeferredAudio` | DONE | `CGaming_RunPreMatchModal` cmd **2** post-modal: `CDSAudioPlayer_Stop`, `ApplyEffectiveVolume`, DirectSound `vtable+0x30`, `CIntListInsertSortedOrAppend(&DAT_004b020c,…)`; xref `0x0041c487` |
| `0x0043ad55` | `FUN_0043ad55` | **Deleted** false function | DONE | Byte-identical tail of `CDSDirectPlaySender_ThreadEntry@0x0043ad20` (ends `0x0043ad54`); only xrefs internal jumps; uses `CDSMemQueue_Lock/Read/Unlock` + `IDirectPlay4::Send` vtbl `+0x68` |
| `0x0043aae0`–`0x0043b6d0` | — | No `FUN_*` left | SKIP | 28 named DirectPlay symbols (see § above) |
| `0x0043c1f0`–`0x0043c210` | `FUN_*` (mapping.csv stale) | Already `CDSMemQueue_Lock/Unlock/Read` | SKIP | Ghidra names; xrefs from `CDSDirectPlaySender_ThreadEntry` |
| `0x0043c590` | `CDSMpx::FUN_0043c590` | Already `CDSMemQueue_Configure` | SKIP | `MOV ESI,ECX` @ `0x0043c597`; calls `CDSMemQueue_Init@0x0043c1d0`; xrefs `CDSDirectPlaySender_ctor@0x0043b0ee`, `HandleAcquireResource`, `AttachBitstream` |
| `0x0043a030` | `FUN_0043a030` | **BLOCKED** (out of DirectPlay) | BLOCKED | `CDSAudioPlayer` stream `vtable+0x40`; sole xref `CDSAudioPlayer_Create` — Direct**Sound**, not DirectPlay |
| `0x0043a350` | `FUN_0043a350` | **BLOCKED** | BLOCKED | `CDSDirectSound` init path — audio device |
| `0x004397e0` | `FUN_004397e0` | **BLOCKED** | BLOCKED | 8-byte memmove for `CDSTrackVector`; callee `TM_InsertTrackAt@0x00439a70` — video track vector |
| `0x0043d5b0` | `FUN_0043d5b0` | **BLOCKED** | BLOCKED | BlitTable opaque nibble → RGB565 (`param_7` palette LUT) — rendering |
| `0x004438a0` | `FUN_004438a0` | **BLOCKED** | BLOCKED | BlitTable masked BGRA32 — rendering |
| `0x0042d160` | `FUN_0042d160` | **BLOCKED** | BLOCKED | `CMenu` child detach/focus — UI shell, not session enum |
| `0x0043c9b0` | `FUN_0043c9b0` | **BLOCKED** | BLOCKED | `CBulanci+0x200` audio teardown; caller `CDSApp_OnDestroy` — app shutdown |
| `0x00440adc0`–`0x0040f380` | `CSessionList_*` | **Out of range** | N/A | Session list @ `0x0040xxxx` (other R5 workers / prior passes) |

## Ghidra deltas

1. `rename_function_by_address` → `CGaming_PreMatchModal_ArmSchedulerSlot0` @ `0x00439880`
2. `rename_function_by_address` → `CGaming_PreMatchModal_RestartDeferredAudio` @ `0x0043aa70`
3. `delete_function` → removed spurious `FUN_0043ad55` @ `0x0043ad55`
4. `set_decompiler_comment` @ `CDSDirectPlaySender_ThreadEntry` (`0x0043ad20`), `CDSDirectPlaySender_EnqueueSend` (`0x0043b680`)
5. `set_function_this_type` **failed** (`__fastcall` — no implicit `this`) @ `0x00439880`, `0x0043aa70`
6. `save_program bulanci.exe`

## Verification snippets

Pre-match modal dispatch (`CGaming_RunPreMatchModal@0x0041c290`):

- Cmd 0 → `CGaming_PreMatchModal_ArmSchedulerSlot0(piVar3[1])` when `*piVar3 == 0`
- Cmd 2 → `CGaming_PreMatchModal_RestartDeferredAudio(piVar2)` after modal; then `CDSAudioPlayer` vtbl release
- Tail → `CGaming_SyncKeyLatchAfterModal` then `CGaming_DrainRecordedNetMessages` (live/replay net pump)

Send path:

- `CDSDirectPlay_Send@0x0043b6d0` → `CDSDirectPlaySender_EnqueueSend@0x0043b680` → worker `CDSDirectPlaySender_ThreadEntry@0x0043ad20` → `IDirectPlay4::Send` (vtbl index 26 / offset `+0x68`).

## Remaining UNK (this worker scope)

| Item | Reason |
|------|--------|
| `CGaming_PreMatchModal_*` `__thiscall` / `this` typing | MCP rejects `set_function_this_type` on `__fastcall`; needs `set_function_prototype` bridge |
| `FUN_0043a030`, `FUN_0043a350`, `FUN_0043b720`, … | DirectSound / WAV / DX exception — outside DirectPlay |
| `FUN_004397e0`, `FUN_00439a70` | CDSTrackVector helpers — video |
| Session UI symbols | Below `0x00428000` — not in address slice |

## Struct / doc updates

None (function-only pass). Cross-ref: `ghidra_analysis/netcode/status.md`, `ghidra_analysis/netcode/mcp_apply_log.md`, `ghidra_analysis/engine/struct_recovery/CSessionList.md`.
