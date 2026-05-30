# Struct recovery — batch 23 follow-up (round 2)

**Prior:** `batch_23_summary.md`  
**Structs:** `CDSAudioBankSample`, `CDSAudioPlayer`  
**Program:** `bulanci.exe`  
**Status:** **FOLLOWUP_DONE**

## Inputs read

- `batch_23_summary.md`
- `CDSAudioBankSample.md`, `CDSAudioPlayer.md`

## Actions

### CDSAudioBankSample — type `dwDecoderField0/1`

1. Disassembled `CDSAudioBankSample_ctor@0x00429530`: copies `pDecoder+8` / `pDecoder+0xC` into object `+0x0C` / `+0x10` before final `dwSampleByteSize` write.
2. Cross-checked `CDSAudioPlayer_Init@0x0043a760` and `menu_audio_mixer.jsonl` / Frida trace: on the bank **slot** pointer (`alloc+4`), `+0x08` = channels, `+0x0A` = bits, `+0x0C` = sample rate — same layout as WAVEFORMATEX tail.
3. Ghidra: `dwDecoderField0` → `wChannels` (ushort @0x0C), `dwDecoderField1` → `dwSampleRate` (@0x10), added `wBitsPerSample` (ushort @0x0E).
4. Updated `CDSAudioBankSample.md`.

### CDSAudioPlayer — name `dwInitParam`

1. `CDSAudioPlayer_Create@0x00422382` → `Init(this, source, flags, param_4, param_1)` where `param_4` is `TriggerBankSample`’s `eventTarget` (`IDSEventHandler*`, often `0`).
2. `FUN_00422470@0x00422470` clears `*(player+0x18)` before `CDSAudioPlayer_Stop` (CBulanek death cleanup).
3. Ghidra: `dwInitParam` → `pEventTarget` (`void *` @0x18); `CDSAudioPlayer_Init` prototype updated (5th arg = category → `+0x4C`).
4. Updated `CDSAudioPlayer.md`.

### Ghidra deltas

- `modify_struct_field` on `CDSAudioBankSample`: `wChannels`, `dwSampleRate`.
- `add_struct_field` `wBitsPerSample` @0x0E on `CDSAudioBankSample`.
- `modify_struct_field` on `CDSAudioPlayer`: `pEventTarget`.
- `set_function_prototype` `CDSAudioBankSample_ctor`, `CDSAudioPlayer_Init`.
- `save_program bulanci.exe` (once).

## Agent todo 28 (2026-05-30)

- **Traced** `pEventTarget` dispatch: `OnPlaybackTick` → `PostMessage(CDSDirectSound+0x1c,0x200,1,player)` → `CDSDirectSound_OnPlaybackCompleteMessage@0x0043cdc0` reads `player+0x18` → `CStartGame2_EnqueueEvent` → e.g. `CMenu_OnEvent(1)`.
- Ghidra: renamed `CDSDirectSound_OnMouseButtonDown` → `CDSDirectSound_OnPlaybackCompleteMessage`; comments on `CDSAudioPlayer_Create`/`Init`/`OnPlaybackTick`/anchor addresses; prototypes on create/release helpers.

## Round-2 todo 24 (2026-05-30)

- **Catalogued** all 24 `TriggerBankSample` xrefs: sole non-menu non-zero `pEventTarget` host is **`CBulanek+0x10`** → `CBulanek_OnEvent(1)` (death/hit/delayed quip chain). Menu host `CMenu+0x10` unchanged.
- Ghidra: plate/decompiler comments on `CDSDirectSound_OnPlaybackCompleteMessage`, `CDSAudioPlayer_Create`, `CMenu_DispatchHotkey`, `CBulanek_OnEvent`, `CBulanek_OnDeath`; `save_program`.

## Remaining UNK

- `CDSAudioBankSample`: `dwReadCursor` semantics; full MI map; standalone `IDSAudioSource` struct (decoder-only layout not fully recovered).

## Struct status after follow-up

| Struct | Status | Size |
|--------|--------|------|
| `CDSAudioBankSample` | VERIFIED | `0x24` |
| `CDSAudioPlayer` | VERIFIED | `0x58` |
