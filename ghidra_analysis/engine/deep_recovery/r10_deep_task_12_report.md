# Round 10 — Deep Task 12 Report (CGaming match-modal tail)

## Task

| Field | Value |
|-------|-------|
| **id** | 12 |
| **title** | Struct deep: CGaming match-modal tail (+0x300..+0x36C) |
| **kind** | struct |
| **struct_name** | `CGaming` |
| **seed_address** | `0x00439880` |
| **addresses** | `0x00439880`, `0x0043aa70`, `0x0041ab70`, `0x00420380` |

## Status

**PARTIAL** — tail band `+0x300..+0x36C` largely closed via live disasm/xref proof. **`wModalExitCode@+0x364`** verified (7 sites). **`vecSlotVec_2e8`** and **`pad_0x30c`** remain UNK (no reads).

## Functions / Struct

| Address | Symbol | Role | Evidence |
|---------|--------|------|----------|
| `0x00439880` | `CGaming_ArmTrackMgrSchedulerIfUnpaused` | Pre-match modal: type-0 queue entry arms `CDSVideoPlayer` scheduler slot when unpaused | **Caller:** `CGaming_RunPreMatchModal@0x0041c400` only. **Body:** `bPaused==0` → `Scheduler_GetEventSlot` / `Scheduler_ArmSlot` |
| `0x0043aa70` | `CGaming_PreMatchModal_RestartDeferredAudio` | Post-modal teardown: restart deferred `CDSAudioPlayer` (type-2 queue entry) | **Caller:** `CGaming_RunPreMatchModal@0x0041c487`. Stop → volume → DirectSound play → global audio list insert |
| `0x0041c290` | `CGaming_RunPreMatchModal` | Pre-match modal coordinator: seed scheduler, `CDSView_DoModal(pPauseDlg)`, unwind audio/video | **Triggered from** `CGaming_OnCmd` when `param_1==0xED && wModalExitCode==0xFFFF` |
| `0x0041d5f0` | `CGaming_OnCmd` | Round-end + pre-match modal command router | **Disasm `0x364`:** `CMP/MOV/MOVZX` @ `0x0041d602`/`0x0041d615`/`0x0041d686`/`0x0041d69f`; bumps `nRoundEndWait@+0x338`; `EndModal(wModalExitCode)` |
| `0x004206f7` | `CGaming_OnCustomEvent` | Custom-event dispatch; gates case `0xE9` on modal sentinel | **`CMP word [ESI+0x364],0xFFFF`** before enqueue to `pPauseDlg` |
| `0x0041ab70` | `CGaming_Ctor` | Partial heap ctor (CreateObject `0x36C`); inits four slot-vec bands through `+0x360` zero | **Xref:** `CreateObject@0x0041beb0` only |
| `0x00420380` | `CGaming_ctor` | Full stack ctor: owner, HUD, pause dlg, script, audio tail, `wModalExitCode=0xFFFF` | Stack shell for `CGame_StartGame` match session |
| `0x00417c80` | `CGaming_SetAmbientAnimMode` | Writes `nAmbientAnimMode@+0x33c`; configures scheduler facet delay | **Renamed** from `CBulanci_SetAmbientAnimMode` (misleading namespace). **Xrefs:** ctor solo path, `TickAmbientAnimations`, `OnPlayerCollectItem`, `UnregisterRoundHudObjects` |
| `0x0041d940` | `CLevelScriptOpExt_LoadPreface` | Script `LoadPreface`: resource → cast → `pPrefaceDrawable@+0x360` | **`MOV [ESI+0x360],EDI`** @ `0x0041d96f`; prior Release @ `0x0041d954` |
| `0x0041b3b0` | `CGaming_OnCustomEvent_0xF5_RemoveEntitiesBySlotId` | Net purge: reverse-walk `netPurgeEntitySlotVec` | **`MOV EDI,[ECX+0x300]`** count; data `@+0x2F8`; max 2 removals |
| `0x0041b5a0` | `CGaming_RetestTraceAreasForEntity` | Walks `dangerZoneSlotVec` | Count `@+0x2E0`, slots `@+0x2D8` |
| `0x0041b420` | `CGaming_AppendDangerZoneNode` | Appends 0x20 B zone node to `dangerZoneSlotVec` | `DefineDangerZone`, `DefineTraceArea`, `CMina_RegisterDangerZone` callers |

### Tail layout (`+0x300..+0x36C`)

| Offset | Size | Type | Name | Evidence |
|--------|------|------|------|----------|
| `+0x2F8` | 8 | `CDSPtrSlotVec` | `netPurgeEntitySlotVec` | `0xF5` handler `RemoveRange(&+0x2f8,…)` |
| `+0x300` | 4 | `int` | `nNetPurgeEntitySlotCount` | `MOV EDI,[ECX+0x300]` @ `0x0041b3b3` |
| `+0x304` | 4 | `int` | `cNetPurgeEntitySlotCapacity` | ctor `=8` @ `0x0042007e` |
| `+0x30C` | 4 | `int` | `pad_0x30c` | ctor zero @ `0x004200d5` — **no read** |
| `+0x33C` | 4 | `int` | `nAmbientAnimMode` | `CGaming_SetAmbientAnimMode` writer; 0→25 s / 1→10 s scheduler delay |
| `+0x360` | 4 | `void *` | `pPrefaceDrawable` | `LoadPreface` store; `LoadLevelAssetAndMusic` / dtor Release (`vtable+8`) |
| `+0x364` | 2 | `ushort` | `wModalExitCode` | ctor `0xFFFF`; OnCmd + OnCustomEvent consumers (7 disasm hits on `0x364`) |
| `+0x368` | 4 | `uint` | `dwWeaponSpawnerMode` | unchanged (prior proof) |

**Companion count pattern (vectors `@+0x2C8..+0x2F8`):** each 16-byte band is `{CDSPtrSlotVec pSlots+cCapacity, nLiveCount, cCapacity}` — e.g. `nBulletSlotCount@+0x2D0` read by `CGaming_CleanupInactiveBullets@0x0041a2a4`.

### `wModalExitCode` consumer map

| Address | Function | Mnemonic | Role |
|---------|----------|----------|------|
| `0x004200ee` | `CGaming_ctor` | `MOV word [ESI+0x364],0xFFFF` | Init sentinel |
| `0x0041d602` | `CGaming_OnCmd` | `CMP word [ESI+0x364],0xFFFF` | First high-bit round-end cmd |
| `0x0041d615` | `CGaming_OnCmd` | `MOV [ESI+0x364],AX` | Store exit code |
| `0x0041d686` | `CGaming_OnCmd` | `MOVZX ECX,word [ESI+0x364]` | Read for `EndModal` |
| `0x0041d69f` | `CGaming_OnCmd` | `MOV [ESI+0x364],AX` | `0x8004` overwrite path |
| `0x0041d6b0` | `CGaming_OnCmd` | `CMP word [ESI+0x364],0xFFFF` | Secondary sentinel check |
| `0x004206f7` | `CGaming_OnCustomEvent` | `CMP word [ESI+0x364],0xFFFF` | Gate `0xE9` pause-dlg events |

## Ghidra deltas

| Action | Target | Result |
|--------|--------|--------|
| `modify_struct_field` | `pPad_0x360` → `pPrefaceDrawable` `void *` | applied |
| `add_struct_field` | `nAmbientAnimMode@0x33c`, `nNetPurgeEntitySlotCount@0x300`, `nPad_0x30c@0x30c` | applied |
| `add_struct_field` | `dangerZoneSlotVec@0x2d8`, `nDangerZoneSlotCount@0x2e0`, `netPurgeEntitySlotVec@0x2f8` | applied |
| `rename_function_by_address` | `0x00417c80` → `CGaming_SetAmbientAnimMode` | applied |
| `set_decompiler_comment` | `0x0041d602`, `0x0041d960`, `0x00417c87` | applied |
| `save_program` | `bulanci.exe` | saved |

## Decomp corrections (IDA vs Ghidra)

| Issue | IDA / disasm | Ghidra (before R10) | Resolution |
|-------|--------------|---------------------|------------|
| `+0x360` | `sub_41D96F` stores cast pointer; dtor `Release` | `pPad_0x360` byte[4] | **`pPrefaceDrawable`** `void *` |
| `+0x33c` | `sub_417C87` stores mode int on gaming shell | `CBulanci_SetAmbientAnimMode` namespace | Renamed **`CGaming_SetAmbientAnimMode`**; field **`nAmbientAnimMode`** |
| `+0x300` | `sub_41B3B3` count for purge walk | unnamed / confused with separate vec | **`nNetPurgeEntitySlotCount`** |
| `0x00439880` | `sub_439880` on `CDSVideoPlayer`-shaped object | Already named; comment documents pre-match role | Confirmed caller `RunPreMatchModal` only |
| `CGaming_TickAmbientAnimations` | `ECX` = `CGaming*`; reads `+0x324..+0x330` | `CBulanci *` with bogus `game.chain` offsets | Documented; `force_decompile` not required this pass |

## Frida

Not required — modal sentinel and tail offsets fully provable from disasm + decompile.

## Remaining UNK

- **`vecSlotVec_2e8` / `nVecSlot2e8Count@+0x2F0`**: ctor/dtor resize only; zero read xrefs.
- **`pad_0x30c`**: write-only in ctor.
- **`pPrefaceDrawable`** nominal type (`CDSStaticDrawableFace *` vs generic COM) — cast id `DAT_004b826c` shared with static drawable faces.
