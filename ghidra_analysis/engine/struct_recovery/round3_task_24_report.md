# Round 3 — task 24 report

## Task

| Field | Value |
|-------|-------|
| **id** | 24 |
| **title** | CDSAudioBankSample dwReadCursor semantics and IDSAudioSource MI map |
| **types** | `CDSAudioBankSample`, `CDSAudioPlayer`, `CDSDirectSound` |
| **addresses** | `0x00429530`, `0x0043a760`, `0x0043cdc0`, `0x00422382` |
| **priority** | medium |
| **source** | backlog (supersedes R2 todo 24) |

## Status

**DONE** — `dwReadCursor` renamed **`dwInitFlag`** (ctor-only `=1`, no consumer); full **0x24** MI map documented; `IDSAudioSource` **Read** typed at `alloc+4`.

## Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| Sole write `+0x1C = 1` | `CDSAudioBankSample_ctor@0x00429530` | `MOV dword ptr [ESI+0x1c], 0x1` @ `0x00429564`; no other `MOV […+0x1c]` in ctor body |
| **Not** PCM byte index | `CDSAudioBankSample_HandleResourceRead@0x004291d0` | `ECX` = **IDSAudioSource\*** (`alloc+4`); `memcpy` base = `[EBP+0x1c]` = **`pPcmBuffer`** (full `+0x20`); index = `*pReadCursor` **arg** |
| Read bound | `CDSAudioBankSample_HandleResourceRead@0x004291d0` | `dwSampleByteSize - *pReadCursor`; rewind `*pReadCursor=0` when `>= dwSampleByteSize` |
| Bank slot pointer | `CDSAudioBank_Deserialize@0x00429858` | `OperatorNew(0x24)` → ctor; store **`sample+4`** in `slotVector` |
| Player binds source | `CDSAudioPlayer_Init@0x0043a760` | `pSource+8/+0xA/+0xC` = channels / bps / rate; `vtable+0x14` → `CDSWav_HandleAcquireReadThunk` returns **0** |
| Player read offset | `CDSAudioPlayer_Init@0x0043a760` | `vtable+0x18` Read with `player+0x2c` as `*pReadCursor`; fills DS buffer / streaming list |
| Create / event target | `CDSAudioPlayer_Create@0x00422382` | 4th arg → `Init` → `player+0x18` `pEventTarget` |
| Playback complete (out of scope) | `CDSDirectSound_OnPlaybackCompleteMessage@0x0043cdc0` | WM `0x200`/1 → `player+0x18`; **does not** touch sample `dwInitFlag` |

### `dwInitFlag` @ full `+0x1C` (was `dwReadCursor`)

| Aspect | Result |
|--------|--------|
| Writers | **One**: ctor `= 1` |
| Readers | **None** in `bulanci.exe` (instruction search on ctor; Read uses **face** `+0x1c` = `pPcmBuffer`, not this dword) |
| Parallel | `CDSAudioBank+0x08` `dwInitFlag` also ctor-only `= 1` (`CDSAudioBank.md`); wav `dwPcmEndBound=1` on a different facet layout |
| Conclusion | Misnamed “read cursor”; treat as **init sentinel / reserved**, not stream state |

### Multiple-inheritance map (`CDSAudioBankSample`, 36 B)

| Full offset | Face | Vtable @ | Slots | Slot → impl (bank sample) |
|-------------|------|----------|-------|---------------------------|
| `+0x00` | `IDSReferenced` primary | `0x486f34` | 3 | `[0]` typeinfo `FUN_00429340`; `[1]` scalar delete `FUN_00429510`; `[2]` release `FUN_00433040` |
| `+0x04` | **`IDSAudioSource`** (`face_8slots`) | `0x486f10` | 8 | `[0]` class meta `FUN_0041a510`; `[1]` `FUN_0042ac90`; `[2]` release thunk `FUN_004330a0`; `[3]` delete thunk `FUN_00429350`; `[4]` no-op `FUN_00467430`; `[5]` acquire-read **0** `CDSWav_HandleAcquireReadThunk@0x0043b950`; `[6]` **Read** `HandleResourceRead@0x004291d0`; `[7]` no-op `CDSView_NoOpStub@0x00438340` |
| `+0x18` | `IDSStream` | `0x486efc` | 4 | Shared stream vtable (file/bulanci/wav thunks per `vftable_methods.csv`); decode fill via ctor `pDecoder->vtable+0x18` |
| `+0x1C` | — | — | — | `dwInitFlag` (`uint`, ctor `=1`) |
| `+0x20` | — | — | — | `pPcmBuffer` (PCM heap; `Runtime_MallocOrThrow` in ctor) |

**`IDSAudioSource`** Ghidra type (**20 B**, bank-slot view): `pVftable`, `dwSampleByteSize`, `wChannels`, `wBitsPerSample`, `dwSampleRate`, `pCachedDirectSoundBuffer` (`pHeldRef` on full object @ `+0x14`). Bytes `+0x14..+0x1b` on the slot pointer are **`IDSStream` vtable** / tail overlap — do not model as extra `IDSAudioSource` fields.

## Ghidra deltas

- `modify_struct_field` `CDSAudioBankSample.dwReadCursor` → **`dwInitFlag`** @ `+0x1C`.
- `set_function_this_type` `CDSAudioBankSample_HandleResourceRead@0x004291d0` → **`IDSAudioSource *`** (decompile uses `dwSampleByteSize`, `pPcmBuffer` at face `+0x1c`).
- `set_decompiler_comment` @ `0x00429564`, `0x00429201`; plate comments @ `0x00429530`, `0x004291d0`.
- `save_program bulanci.exe`.

## Struct doc updates

- [CDSAudioBankSample.md](./CDSAudioBankSample.md) — `dwInitFlag`, MI slot table, Read/`pReadCursor` split
- [CDSAudioPlayer.md](./CDSAudioPlayer.md) — cross-ref `pSource` = `IDSAudioSource*` (`alloc+4`)

## Remaining UNK

- Semantic purpose of ctor-only **`dwInitFlag=1`** (no consumer; may be dead MSVC field or reserved for unimplemented `IDSStream` state).
- Decoder object at `pDecoder` in `CDSAudioBank_Deserialize` (only `+8`/`+0xC` format dwords copied at sample ctor — dedicated decoder struct still **PARTIAL**).
- `IDSStream` slot naming on `0x486efc` for bank samples (shared vtable with file/wav helpers).
