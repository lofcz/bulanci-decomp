# CDSAudioPlayer

## Status

**VERIFIED**

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `OperatorNew` size **0x58** (88) | `0x00422382` | `CDSAudioPlayer_Create` — `OperatorNewWithBadAlloc(0x58)` |
| Alternate alloc same size | `0x00422a03` | `CBulanci` menu path — `OperatorNewWithBadAlloc(0x58)` before ctor |
| Ghidra struct size **0x58** | — | `get_struct_layout CDSAudioPlayer` → 88 bytes |
| Inherits `CDSObject` refcount face | `0x0043a4f0` | ctor sets vtables `0x4877ac` / `0x487798`; dtor restores `g_pCDSObject_vftable_IDSReferenced` |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `void *` | `pVftable_primary` | `CDSAudioPlayer_ctor@0x0043a4f0` |
| 0x04 | 4 | `void *` | `pVftable_IDSReferenced` | ctor; dtor |
| 0x08 | 4 | `uint` | `dwRefCount` | ctor `= 1`; `Play@0x0043a9d0` pre-increment before `vtable+8` release in `PlayAndRelease` |
| 0x0C | 4 | `void *` | `pSource` | ctor `0`; `Init@0x0043a760` assigns `IDSAudioSource*`; dtor `Release` |
| 0x10 | 4 | `void *` | `pTrackSync` | ctor `0`; `OnPlaybackTick@0x0043a1f0` → `TM_SeekToFrame(*(this+0x10), …)` when non-null |
| 0x14 | 4 | `void *` | `pDirectSoundBuffer` | ctor `0`; `Init` / `Play` / `Stop` / `ApplyEffectiveVolume` IDirectSound buffer vtable calls |
| 0x18 | 4 | `void *` | `pEventTarget` | `Init` stores 4th arg; `CDSAudioPlayer_Create@0x00422382` passes `param_4` from `TriggerBankSample` (`eventTarget`); cleared `FUN_00422470@0x00422470` |
| 0x1C | 1 | `byte` | `bStreaming` | `Init` streaming flag; `Play` loop test with `bLooping` |
| 0x1D | 1 | `byte` | `bLooping` | `Play` assigns `param_1` |
| 0x1E | 1 | `byte` | `bSourceReadActive` | `Init` sets/clears; dtor calls `pSource->vtable+0x1c` when set |
| 0x1F | 1 | `byte` | `bReleaseOnStop` | ctor `= 1`; `Stop` assigns `param_1` |
| 0x20 | 1 | `byte` | `bInStreamingTickList` | ctor `0`; `Init` sets when registered; `dtor@0x0043a650` removes from `DAT_004b021c` |
| 0x21 | 1 | `byte` | `bPlayPending` | `Play`/`Stop`/`RefillDirectSoundBuffer` |
| 0x22 | 1 | `byte` | `bPlaying` | `Play`/`Stop`/`OnPlaybackTick`; menu trace `playing_0x22` |
| 0x23 | 1 | `byte` | `bRefillArmed` | `OnPlaybackTick`; buffer worker `FUN_0043a350@0x0043a350` clears |
| 0x24 | 1 | `byte` | `bRefillBusy` | `OnPlaybackTick` early-out; `FUN_0043a350` sets/clears |
| 0x28 | 4 | `uint` | `dwSourceBytesConsumed` | `Init` += bytes copied; `FUN_0043a350` streaming advance |
| 0x2C | 4 | `uint` | `dwSourceReadCursor` | `Init` from `pSource->vtable+0x14`; `FUN_0043a350` `Read` into buffer |
| 0x30 | 4 | `uint` | `dwDsBufferBytes` | `Init` `uVar8*8`; ring wrap `FUN_0043a350` |
| 0x34 | 4 | `uint` | `dwDsPeriodBytes` | `Init`; `OnPlaybackTick` refill sizing |
| 0x38 | 4 | `uint` | `dwDsRingPos` | `Init` `0`; `FUN_0043a350` advances |
| 0x3C | 4 | `uint` | `dwDsSubOffset` | `RefillDirectSoundBuffer@0x0043a0f0`; `OnPlaybackTick` |
| 0x40 | 4 | `uint` | `dwPlaybackBytePos` | `RefillDirectSoundBuffer`; `OnPlaybackTick` vs `pSource+4` size |
| 0x44 | 4 | `uint` | `dwPlayStartMs` | `Play` `= g_dwElapsedMs`; `RefillDirectSoundBuffer` (no DS buffer) |
| 0x48 | 4 | `uint` | `dwStopMs` | `Stop` `= g_dwElapsedMs` |
| 0x4C | 4 | `uint` | `dwCategoryIndex` | `Init` 5th arg (`pCategory`); `ApplyEffectiveVolume@0x0043a060` indexes `DAT_004b8370` |
| 0x50 | 4 | `int` | `nCachedEffectiveDb100` | `ApplyEffectiveVolume`; menu trace `cachedEffectiveDb_0x50` |
| 0x54 | 4 | `uint` | `dwVolumePercent` | ctor `100`; `SetVolumePercent@0x0043a0d0`; menu trace |

**Padding:** bytes **0x25–0x27** are compiler padding between `bRefillBusy` and `dwSourceBytesConsumed` (no direct xref).

## Leaf functions (slice 23)

| Address | Name | Role |
|---------|------|------|
| `0x0043a4f0` | `CDSAudioPlayer_ctor` | Vtables `0x4877ac`/`0x487798`; `dwRefCount=1`, `dwVolumePercent=100` |
| `0x0043a760` | `CDSAudioPlayer_Init` | Binds `pSource` (bank slot **`sample+4`**); DS buffer; streaming tick list `DAT_004b021c` |
| `0x0043a9d0` | `CDSAudioPlayer_Play` | `bLooping`, `bPlaying`, refcount bump |
| `0x0043a4a0` | `CDSAudioPlayer_Stop` | `dwStopMs`, optional `Release` path |
| `0x0043a1f0` | `CDSAudioPlayer_OnPlaybackTick` | Refill sizing; arms `bRefillArmed`; completion → `CDSView__PostMessage` |
| `0x0043a0f0` | `CDSAudioPlayer_RefillDirectSoundBuffer` | Non-streaming refill |
| `0x0043a350` | `CDSAudioPlayer_FillDirectSoundBuffer` | Streaming worker — typed decompile uses `pSource`, `dwDsPeriodBytes`, `bRefillArmed`/`bRefillBusy` |
| `0x00422310` | `CDSAudioPlayer_Create` | `OperatorNew(0x58)` + ctor + `Init` |
| `0x00422470` | `CBulanek_ReleaseAudioPlayerRef` | Clears `pEventTarget` (`+0x18`), `Stop`, nulls `int*` slot |
| `0x0043cdc0` | `CDSDirectSound_OnPlaybackCompleteMessage` | **Reads** `pEventTarget` on WM `0x200`/1; enqueues to handler |

**`pSource` layout** (pointer is **IDSAudioSource** face, bank slot `alloc+4`): `+4` `dwSampleByteSize`, `+8` `wChannels`, `+0xA` `wBitsPerSample`, `+0xC` `dwSampleRate`, `+0x10` cached duplicate `IDirectSoundBuffer*` when streaming (`Init@0x0043a760`).

## Ghidra apply

```
get_struct_layout CDSAudioPlayer
→ Size: 88 bytes (0x58)
```

Struct verified in Ghidra (agent slice **23**, 2026-05-30). Renamed `FUN_0043a350` → `CDSAudioPlayer_FillDirectSoundBuffer` (prototype `CDSAudioPlayer *this`). `Init` prototype set; decompiler keeps `void *this` per Ghidra thiscall API limit.

## Playback-complete dispatch (`pEventTarget` @ +0x18)

| Step | Address | Action |
|------|---------|--------|
| Store | `CDSAudioPlayer_Init@0x0043a760` | `*(this+0x18) = pEventTarget` (4th arg; `TriggerBankSample` `eventTarget`) |
| Signal | `CDSAudioPlayer_OnPlaybackTick@0x0043a22e` | `CDSView__PostMessage(g_pDirectSoundSingleton+0x1c, 0x200, 1, player, 0)` — **lParam is player**, not `pEventTarget` |
| Dispatch | `CDSDirectSound_OnPlaybackCompleteMessage@0x0043cdc0` | On WM `0x200` / wParam `1`: if `bReleaseOnStop` and `player->pEventTarget` non-null → `CStartGame2_EnqueueEvent(pEventTarget, 0x200, 1, player, 0)`; then `Release` player and remove from active list `DAT_004b020c` |
| Clear | `CBulanek_ReleaseAudioPlayerRef@0x00422470` | Zeros `+0x18` before `Stop` so completion cannot fire into a dying host |

`OnPlaybackTick` never reads `+0x18`; the **only** read is `CDSDirectSound_OnPlaybackCompleteMessage` (`piVar1[6]` = offset `0x18`).

### `pEventTarget` host catalog (round-2 todo 24, full xref audit)

Full-binary xref of `TriggerBankSample@0x00422430` (24 call sites): **only two** pass non-zero `eventTarget` (5th arg). All other bank SFX / spatial helpers pass `0`.

| Host object | `pEventTarget` address | `OnEvent` handler | TriggerBankSample call site | Event **1** completion behavior |
|-------------|------------------------|-------------------|----------------------------|----------------------------------|
| `CMenu` | `CMenu+0x10` (`field_0x10`) | `CMenu_OnEvent@0x00424f50` | `CMenu_DispatchHotkey@0x00425371` (slots `0x1a` X / `0x1c` F12) | `CDSView__EndModal(this, +0xcc)`; release `pDeferredExitVoicePlayer` @ +0xc4 |
| `CBulanek` | `pCdsViewPrefix+0x10` (= `CBulanek+0x10`) | `CBulanek_OnEvent@0x00420d40` | `CBulanek_OnDeath@0x0041fa21` (death quip 0x16/0x17); `CBulanek_OnEvent` case `0xdb@0x00420f14` (hit quip); chained from case `1@0x00420e60` (delayed quip) | **Death voice done** → play delayed quip (same target, store @ +0x174); **hit quip done** → clear +0x170; **delayed quip done** → `CBulanek_PostScriptEvent(0xdb)` if killer slot valid |

Player ref slots on `CBulanek`: death voice @ `pPad_mid+0x58`, hit quip @ +0x170, delayed quip @ +0x174. `CGameCounter_OnEvent@0x0040b1b4` explicitly passes `eventTarget=0`.

## UNK

- Whether `pTrackSync` is always `TM_*` track manager or other sync object (only `TM_SeekToFrame` xref proven).
