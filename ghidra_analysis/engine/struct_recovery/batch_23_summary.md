# Struct recovery batch 23/50

**Batch index:** 23 (`batches_50.json`)  
**Structs:** `CDSAudioBankSample`, `CDSAudioPlayer`  
**Program:** `bulanci.exe`

## Results

| Struct | Status | Size | Ghidra |
|--------|--------|------|--------|
| `CDSAudioBankSample` | VERIFIED | 0x24 | Applied (36 B) |
| `CDSAudioPlayer` | VERIFIED | 0x58 | Applied (88 B) |

## Evidence highlights

- **CDSAudioBankSample:** `OperatorNewWithBadAlloc(0x24)` in `CDSAudioBank_Deserialize@0x00429858`; ctor/dtor/`HandleResourceRead` define stream buffer at +0x20 and sample size at +0x08. Bank stores **`alloc+4`** pointers into slot array.
- **CDSAudioPlayer:** `OperatorNewWithBadAlloc(0x58)` in `CDSAudioPlayer_Create@0x00422382`; ctor + `Init`/`Play`/`Stop`/`OnPlaybackTick`/`ApplyEffectiveVolume` + menu mixer trace (`menu_audio_mixer.jsonl`) corroborate offsets through +0x54.

## Deliverables

- `ghidra_analysis/engine/struct_recovery/CDSAudioBankSample.md`
- `ghidra_analysis/engine/struct_recovery/CDSAudioPlayer.md`

## Ghidra

- Replaced placeholder 1-byte structs with full layouts (`delete_data_type` → `create_struct`).
- `save_program bulanci.exe` executed at batch end.

## Follow-ups

- Type `dwDecoderField0/1` on `CDSAudioBankSample` once `IDSAudioSource`/decoder layout is recovered.
- Name `dwInitParam` on `CDSAudioPlayer` after event-target / `Init` flag semantics are mapped.
