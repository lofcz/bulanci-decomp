# CDSDirectSound

## Status

**VERIFIED** — size 0x54 (84 bytes); embedded `CDSUpdatedItem`, worker block, DirectSound COM pointers, PCM format cache, shutdown flag, and worker event all have ctor/init/dtor/consumer xrefs.

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| Instance size 0x54 | `CBulanci::CDSApp_ctor` @ 0x0042b116 | `CDSDirectSound_ctor(this+0x200)` then fields at `this+0x254` |
| Alternate embed | same binary | `CDSObject::CDSDirectSound_ctor(param_1+0x80)` @ 0x00429990 region |
| Singleton pointer | `CDSObject::CDSDirectSound_ctor` @ 0x0043c7c0 | `g_pDirectSoundSingleton = this` |
| Worker/event tail | `~CDSDirectSound` @ 0x0043c8b0 | `SetEvent` handle at `ESI+0x50`; shutdown byte at `ESI+0x4c` (asm) |
| Heap singleton | `_Globals::CreateObject` @ 0x0043cb20 | `OperatorNew(0x54)` + `CDSDirectSound_ctor` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `void *` | `m_pVtable_IDSReferenced` | `CDSObject::CDSDirectSound_ctor` @ 0x0043c7c0 |
| 0x04 | 24 | `CDSUpdatedItem` | `m_updatedItem` | `CDSUpdatedItem_ctor(this+4)`; `CDSUpdatedItem_dtor(param_1+1)` @ 0x0043c8b0 |
| 0x1C | 4 | `void *` | `m_pVtable_IDSEventHandler` | `CDSObject::CDSDirectSound_ctor` @ 0x0043c7c0 → `g_pCDSDirectSound_vftable_IDSChained_minus1c` |
| 0x20 | 2 | `ushort` | `m_eventHandlerFlags` | `_Globals::CDSEventHandler_ctor` @ 0x0042f660 writes `*(ushort*)(param_1+4)` with handler base at +0x1c |
| 0x24 | 4 | `void *` | `m_pVtable_worker` | `CDSObject::CDSDirectSound_ctor` @ 0x0043c7c0; `CDSDirectSound__WorkerThreadLoop` vtable+0x10 |
| 0x28 | 4 | `void *` | `m_hWorkerThread` | `CDSWorkingThread_Start` @ 0x004471d0 stores thread handle at `this+4` (worker base +0x24) |
| 0x2C | 4 | `uint` | `m_workerThreadId` | `CDSWorkingThread_Start` @ 0x004471d0 writes `this+8` on worker base |
| 0x30 | 4 | `void *` | `m_pWorkerRef` | `CDSObject::CDSObject_ClearRefcountFields` @ 0x00447180 on `this+0x24` |
| 0x34 | 1 | `byte` | `m_bWorkerThreadExited` | `CDSWorkingThread_PollExited` @ 0x004470c0 checks `param+0x10` (worker base) |
| 0x38 | 4 | `void *` | `m_pDirectSound` | `CDSDirectSound_InitPrimary` @ 0x0043cbc0 `CoCreateInstance` → `this+0x38`; `g_pDirectSoundSingleton+0x38` guards in `CDSAudioPlayer_Init` @ 0x0043a8aa |
| 0x3C | 4 | `void *` | `m_pPrimaryBuffer` | `CDSDirectSound_InitPrimary` stores `CreateSoundBuffer` result at `this+0x3c` |
| 0x40 | 4 | `uint` | `m_reserved_40` | Zeroed in `CDSDirectSound_InitPrimary` @ 0x0043cbc0 only |
| 0x44 | 2 | `ushort` | `m_wPcmChannels` | `CDSDirectSound_InitPrimary` writes `param_2` (call site: `2` @ 0x0042afd0) |
| 0x46 | 2 | `ushort` | `m_wBitsPerSample` | `CDSDirectSound_InitPrimary` writes `param_4` (call site: `0x10` @ 0x0042afd0) |
| 0x48 | 4 | `int` | `m_nSamplesPerSec` | `CDSDirectSound_InitPrimary` writes `param_3` (call site: `0x5622` @ 0x0042afd0) |
| 0x4C | 1 | `byte` | `m_bShutdown` | ctor/dtor @ 0x0043c7c0 / 0x0043c8b0 `ESI+0x4c`; worker loop exit via `this[0x28]` with worker base (+0x24) |
| 0x50 | 4 | `void *` | `m_hWorkerEvent` | `CreateEventW` → `ESI+0x50` @ 0x0043c874; `~CDSDirectSound` `SetEvent` @ 0x0043c8ff; worker `WaitForSingleObject` at worker+0x2c |

## Ghidra apply

```
get_struct_layout CDSDirectSound → Size: 84 bytes (17 fields; m_updatedItem is CDSUpdatedItem)
```

Applied/updated via `modify_struct_field` + `add_struct_field` (batch 27). Ghidra prefixes field names (`pM_`, `bM_`, `m_updatedItem`, …).

## Leaf functions (slice 27)

| Address | Symbol | Role |
|---------|--------|------|
| 0x0043c7c0 | `CDSDirectSound_ctor` | Vtables, `CDSUpdatedItem`, worker, `CreateEventW`, `g_pDirectSoundSingleton` |
| 0x0043c8b0 | `~CDSDirectSound` | Shutdown byte + `SetEvent` on `+0x50` (asm) |
| 0x0043c8a0 | `CDSDirectSound_GetTypeInfo` | `IDSReferenced` |
| 0x0043c980 | `CDSDirectSound_DuplicateSoundBuffer` | Buffer helper |
| 0x0043ca50 | `CDSDirectSound_DtorScalar_minus04` | Subobject dtor |
| 0x0043ca60 | `CDSDirectSound_ScalarDeletingDtor_thunk_Sub24` | Worker-face thunk |
| 0x0043ca80 | `CDSDirectSound_DtorScalar_minus1c` | Event-handler dtor |
| 0x0043cb00 | `CDSDirectSound_ScalarDeletingDtor` | Scalar deleting dtor |
| 0x0043cb20 | `_Globals::CreateObject` | `OperatorNew(0x54)` + ctor |
| 0x0043cbc0 | `CDSDirectSound_InitPrimary` | `CoCreateInstance`, PCM format, primary buffer |
| 0x0043ccb0 | `CDSDirectSound_SignalEventIfVoicesActive` | Voice tick → `SetEvent` |
| 0x0043cd00 | `CDSDirectSound__WorkerThreadLoop` | `WaitForSingleObject` @ worker `+0x2c` |
| 0x0043cdc0 | `CDSDirectSound_OnPlaybackCompleteMessage` | IDSView hook: WM `0x200`/1 → read `CDSAudioPlayer::pEventTarget` (+0x18), enqueue completion (renamed from mislabel `OnMouseButtonDown`) |

**Embed:** `CBulanci::CDSApp_ctor` @ 0x0042b116 — `CDSDirectSound_ctor(app+0x200)`; next field at `app+0x254` → span **0x54**.

## Follow-up (slice 27 / batch 27)

- Prototypes set: `CDSDirectSound_InitPrimary(CDSDirectSound *this, …)`, `CDSDirectSound_SignalEventIfVoicesActive(CDSDirectSound *)`, `CDSDirectSound_WorkerThreadLoop(void *)`.

## InitPrimary call sites (agent todo 25)

| Site | Address | Evidence |
|------|---------|----------|
| `CDSDirectSound_InitPrimary` | 0x0043cbc0 | `void CDSDirectSound_InitPrimary(CDSDirectSound *this, void *hwndOwner, ushort, int, ushort)`; plate comment |
| `CDSApp_OnCreate` call | 0x0042a2fa / 0x0042a300 | `LEA ECX,[ESI+0x200]` → `CALL InitPrimary`; decompiler `&param_1->field_0x200` (= `CDSApp::directSound`) |
| `CDSApp_ctor` embed ctor | 0x0042b27d / 0x0042b293 | `LEA ECX,[ESI+0x200]` → `CDSDirectSound_ctor`; next field @ `ESI+0x254` |

`CDSApp::directSound` is `CDSDirectSound` @ `+0x200`. **R3 task 28:** `set_function_this_type(CDSDirectSound *)` @ `0x0043cbc0` — decompile uses `pM_pDirectSound` (+0x38), `pM_pPrimaryBuffer` (+0x3c), PCM fields (+0x44..+0x48). Call sites: `LEA ECX,[ESI+0x200]` @ `0x0042a2fa` / `0x0042b27d`. Residual: `CDSApp_CreateSoundBuffer((CDSApp *)this, …)` cast when calling app-shell helper.

## SetEvent offset resolution (agent todo 26)

| Consumer | Address | Offset used | Field |
|----------|---------|-------------|-------|
| `CDSDirectSound_ctor` | `0x0043c874` | `+0x50` | `m_hWorkerEvent` (`CreateEventW` store) |
| `~CDSDirectSound` | `0x0043c8ff`, `0x0043c92b` | `+0x50` | `m_hWorkerEvent` (`SetEvent` / `CloseHandle`) |
| `CDSDirectSound__WorkerThreadLoop` | `0x0043cd30` | worker `+0x2c` (= host `+0x50`) | `WaitForSingleObject` on wake event |
| `~CDSDirectSound` shutdown flag | `0x0043c90b` | `+0x4c` | `m_bShutdown` (`mov byte [esi+0x4c], 1`) |
| `CDSDirectSound_SignalEventIfVoicesActive` | `0x0043cce1` | **`+0x4c` (dword load)** | **Shipping off-by-4** — loads shutdown band, not `m_hWorkerEvent` |

**Verdict:** Ghidra layout is correct (`m_bShutdown` @ `+0x4c`, `m_hWorkerEvent` @ `+0x50`). Only `SignalEventIfVoicesActive` uses the wrong displacement in the binary; annotate at `0x0043cce1`, do not repack the struct.

## UNK

- **`m_reserved_40` (+0x40)**: only zeroed in `InitPrimary`; no other consumers found.
