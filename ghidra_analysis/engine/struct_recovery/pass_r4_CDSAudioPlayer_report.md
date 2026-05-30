# Pass R4 — CDSAudioPlayer report

## Task

| Field | Value |
|-------|-------|
| **scope** | `CDSAudioPlayer` layout, `Init`, `Create`, playback-complete path, link to `CDSAudioBankSample` / `IDSAudioSource` |
| **types** | `CDSAudioPlayer`, `IDSAudioSource`, `CDSAudioBankSample`, `CDSDirectSound` |
| **addresses** | `0x0043a4f0`, `0x0043a760`, `0x00422382`, `0x0043a1f0`, `0x0043cdc0`, `0x00422430` |
| **inputs** | [CDSAudioPlayer.md](./CDSAudioPlayer.md), [CDSAudioBankSample.md](./CDSAudioBankSample.md), [round3_task_24_report.md](./round3_task_24_report.md) |
| **priority** | medium |

## Status

**DONE** — **0x58** layout confirmed; **`pSource`** typed **`IDSAudioSource *`**; member **`set_function_this_type`** on Init/Play/Stop/tick/refill path; playback-complete **`pEventTarget`** chain re-verified in decompile.

## Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Alloc size 0x58 | `CDSAudioPlayer_Create@0x00422382` | `OperatorNewWithBadAlloc(0x58)`; ctor then `Init` |
| Bank slot → player | `TriggerBankSample@0x00422430` | `CDSAudioPlayer_Create(…, *(IDSAudioSource**)(*(bank+0x18)+slot*4), …)` |
| Init binds source | `CDSAudioPlayer_Init@0x0043a760` | `this->pSource = pSource`; reads `dwSampleByteSize`, `wChannels`, `wBitsPerSample`, `dwSampleRate`; `vtable+0x14`/`+0x18`/`+0x1c` |
| Read cursor on player | `CDSAudioPlayer_Init@0x0043a760` | `pSource->vtable+0x18` with **`&this->dwSourceReadCursor`** — not bank `dwInitFlag` (R3 todo 24) |
| Streaming DS dup | `CDSAudioPlayer_Init@0x0043a760` | `pSource->pCachedDirectSoundBuffer` duplicate or assign from new `pDirectSoundBuffer` |
| Playback end signal | `CDSAudioPlayer_OnPlaybackTick@0x0043a1f0` | `bPlaying=0`; `CDSView__PostMessage(…, 0x200, 1, this, 0)` — **lParam = player** |
| Completion dispatch | `CDSDirectSound_OnPlaybackCompleteMessage@0x0043cdc0` | WM `0x200`/wParam `1`: if `bReleaseOnStop` && `pEventTarget` → `CStartGame2_EnqueueEvent(pEventTarget, 0x200, 1, player, 0)`; `Release` player; remove from `DAT_004b020c` |
| Create passes event host | `CDSAudioPlayer_Create@0x00422382` | 4th arg `pEventTarget` → `Init` → `this->pEventTarget` (+0x18) |

### `IDSAudioSource` link (from R3/R4 bank sample work)

| Step | Evidence |
|------|----------|
| Bank stores **`sample+4`** | `CDSAudioBank_Deserialize@0x00429858` — slot vector holds `IDSAudioSource*` face |
| Sample Read | `CDSAudioBankSample_HandleResourceRead@0x004291d0` — `IDSAudioSource::Read`; `this+0x1c` on face = **`pPcmBuffer`** (full +0x20), not `dwInitFlag` |
| Player consumes Read | `Init` `vtable+0x18` with `&dwSourceReadCursor`; streaming worker `CDSAudioPlayer_FillDirectSoundBuffer@0x0043a350` |

## Ghidra deltas

- `modify_struct_field` `CDSAudioPlayer.pSource` → **`IDSAudioSource *`** (+ rename @ offset 0x0C)
- `set_function_this_type` → **`CDSAudioPlayer *`** on: `0x0043a4f0`, `0x0043a760`, `0x0043a9d0`, `0x0043a4a0`, `0x0043a1f0`, `0x0043a0f0`, `0x0043a350`, `0x0043a060`
- `set_function_prototype`:
  - `CDSAudioPlayer_Init(CDSAudioPlayer *, IDSAudioSource *pSource, uint, void *, undefined *)` **__thiscall**
  - `CDSAudioPlayer_Create(…, IDSAudioSource *pSource, …)` **__cdecl**
  - `CDSAudioPlayer_OnPlaybackTick` / `RefillDirectSoundBuffer` → **__thiscall** (was __fastcall)
- `set_decompiler_comment` @ `0x00422430`, `0x0043cdc0`
- `save_program bulanci.exe`

## Struct doc updates

- [CDSAudioPlayer.md](./CDSAudioPlayer.md) — `pSource` type, `IDSAudioSource` offset table, R4 Ghidra apply table

## Remaining UNK

- Semantic of `pTrackSync` beyond `TM_SeekToFrame` xref (unchanged).
- Whether every bank slot’s `pCachedDirectSoundBuffer` is populated before streaming duplicate path (only Init branches proven).
