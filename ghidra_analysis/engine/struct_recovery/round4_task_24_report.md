# Round 4 — task 24 report

## Task

| Field | Value |
|-------|-------|
| **id** | 24 |
| **title** | CDSAudioBankSample R3 handoffs: pDecoder IDSAudioSource, IDSStream 0x486efc slots, ctor this type |
| **types** | `CDSAudioBankSample`, `IDSAudioSource`, `CDSAudioBank`, `CDSAudioPlayer` |
| **addresses** | `0x00429530`, `0x00429858`, `0x00429675`, `0x004486efc`, `0x00429360` |
| **priority** | medium |
| **source** | handoff (R3 `round3_task_24_report.md` Remaining UNK) |

## Status

**DONE** — `pDecoder` proven **`IDSAudioSource*`** (`CheckedVirtualBaseCast` `DAT_004b83c4`); **`IDSStream`** vtable **`0x486efc`** four slots plated; **`CDSAudioBankSample_ctor`** retyped (`this` + `pDecoder`); **`dwInitFlag`** confirmed write-only dead sentinel.

## Evidence

| Claim | func@addr | Evidence |
|-------|-----------|----------|
| `pDecoder` type | `CDSAudioBankSample_ctor@0x00429530` | Prototype `IDSAudioSource *pDecoder`; decompile reads `pDecoder->dwSampleByteSize`, `wChannels`, `wBitsPerSample`, `dwSampleRate`; decode via `pDecoder->pVftable+0x18` |
| Decoder cast token | `CDSAudioBank_Deserialize@0x00429858` | `CheckedVirtualBaseCast(..., DAT_004b83c4)` @ `0x00429675`; result → sample ctor 3rd-party decoder arg |
| Decoder API in deserialize | `CDSAudioBank_Deserialize@0x00429858` | `vtable+0x14` handle; `+0x18` PCM decode; `+0x1c` cleanup (disasm @ `0x004296ee` / `0x00429735` / `0x00429816`) |
| Same RTTI elsewhere | `CDSAudioPlayer_CreateFromResource`, `CGaming_LoadLevelAssetAndMusic` | Xrefs to `DAT_004b83c4` (menu/level resource → `IDSAudioSource` face) |
| `dwInitFlag` write-only | `CDSAudioBankSample_ctor@0x00429530` | Sole `MOV [ESI+0x1c], 1` @ `0x00429564`; `search_instructions` in ctor: one `mov` to `+0x1c`, zero loads |
| IDSStream slot 0..2 | vtable `0x486efc` | `0x00401600` GetClassTable; `0x004049d0` MI `-0x14`; `0x00433070` release `-0x18` (shared wav/file thunks) |
| IDSStream slot 3 | `CDSAudioBankSample_ScalarDeletingDtor_thunk_Sub18@0x00429360` | `this-0x18` → `CDSAudioBankSample_ScalarDeletingDtor`; bank-sample-specific dtor slot |
| Ctor decompile fixed | `CDSAudioBankSample_ctor@0x00429530` | Post-`set_function_this_type` + prototype: named fields (`pPcmBuffer`, `dwInitFlag`, MI vtables) — no `CDSAudioBank*` alias |

## Ghidra deltas

- `set_function_this_type` `CDSAudioBankSample_ctor@0x00429530` → **`CDSAudioBankSample *`**
- `set_function_prototype` → `CDSAudioBankSample_ctor(CDSAudioBankSample *, IDSAudioSource *pDecoder, uint readParam, uchar *pPcmByteCount)`
- `set_plate_comment` @ `0x00429530`, `0x00401600`, `0x004049d0`, `0x00433070`, `0x00429360` (IDSStream `0x486efc` slot map)
- `set_decompiler_comment` @ `0x00429564`, `0x00429675`
- `save_program bulanci.exe`

## Struct doc updates

- [CDSAudioBankSample.md](./CDSAudioBankSample.md) — `pDecoder`/`DAT_004b83c4`, IDSStream slot table, `dwInitFlag` dead-field note

## Remaining UNK

- MSVC `.?AV…` string for **`DAT_004b83c4`** (RTTI token proven by xref; label only).
- Whether every `IDSAudioSource` decoder implements non-stub **`vtable+0x14`** (bank-slot `AcquireRead` returns 0 @ `0x0043b950`; deserialize decoder uses `+0x14` for stream handle).
