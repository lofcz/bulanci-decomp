# CDSAudioBankSample

## Status

**VERIFIED** (allocation size and field offsets). PCM format dwords at +0x0C..+0x13 match the `IDSAudioSource` face used by `CDSAudioPlayer_Init` (WAVEFORMATEX-style layout on the `alloc+4` pointer).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `OperatorNew` size **0x24** (36) | `0x00429858` | `CDSAudioBank::CDSAudioBank_Deserialize` — `OperatorNewWithBadAlloc(0x24)` before `CDSAudioBankSample_ctor` |
| Ghidra struct size **0x24** | — | `get_struct_layout CDSAudioBankSample` → 36 bytes after apply |
| `free` on scalar delete | `0x00429510` | `CDSAudioBankSample_ScalarDeletingDtor` — `_free(this)` when delete flag set |

## Pointer convention

- **Allocation base** offsets below match the ctor/dtor/`OperatorNew` object.
- **Bank slot pointers** stored at `CDSAudioBank+0x18` array are **`alloc + 4`** (skip primary vtable); see post-ctor `pCVar7 = pCVar7 + 4` at `0x00429572`.
- `CDSAudioBankSample_HandleResourceRead` and `ScalarDeletingDtor_thunk_Sub18` use **`this - 0x18`** / adjusted `this` consistent with embedded faces.

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `void *` | `pVftable_primary` | `CDSAudioBankSample_ctor@0x00429530` |
| 0x04 | 4 | `void *` | `pVftable_IDSAudioSource` | ctor → `g_pCDSAudioBankSample_vftable_IDSAudioSource` (`0x486f10`); bank slot stores **`alloc+4`** |
| 0x08 | 4 | `uint` | `dwSampleByteSize` | ctor `MOV [ESI+8], EBX` (`param_3`); overwrites transient copy of `pDecoder[1]` |
| 0x0C | 2 | `ushort` | `wChannels` | ctor `MOV [ESI+0xC], ECX` ← `pDecoder[2]`; `CDSAudioPlayer_Init@0x0043a760` `*(ushort*)(pSource+8)` |
| 0x0E | 2 | `ushort` | `wBitsPerSample` | `CDSAudioPlayer_Init@0x0043a760` `*(ushort*)(pSource+0xA)`; menu trace `menu_audio_mixer.jsonl` |
| 0x10 | 4 | `uint` | `dwSampleRate` | ctor `MOV [ESI+0x10], EDX` ← `pDecoder[3]`; `CDSAudioPlayer_Init` uses `pSource[3]` (byte +0x0C on slot ptr) |
| 0x14 | 4 | `void *` | `pHeldRef` | ctor `[ESI+0x14]=0`; `FUN_00439fe0@0x00439fe0` releases `*(this+0x14)` when called with subobject at +4 |
| 0x18 | 4 | `void *` | `pVftable_IDSStream` | ctor `[ESI+0x18]=0x486efc` |
| 0x1C | 4 | `uint` | `dwInitFlag` | ctor `[ESI+0x1C]=1` only (**no consumer** in binary); parallels `CDSAudioBank+0x08` `dwInitFlag`; **not** the PCM byte index in Read |
| 0x20 | 4 | `void *` | `pPcmBuffer` | ctor `Runtime_MallocOrThrow`; dtor `Runtime_Free` on `[8]` (= offset 0x20) |

## Leaf functions (slice 23)

| Address | Name | Role |
|---------|------|------|
| `0x00429530` | `CDSAudioBankSample_ctor` | Alloc layout 0x24; copies decoder format; `Runtime_MallocOrThrow` → `pPcmBuffer` |
| `0x00429370` | `CDSAudioBankSample_dtor` | `CDSIDSReferencedSub_ReleaseHeldRef(this+4)`; frees `pPcmBuffer` |
| `0x00429510` | `CDSAudioBankSample_ScalarDeletingDtor` | `_free(this)` when delete flag |
| `0x004291d0` | `CDSAudioBankSample_HandleResourceRead` | `IDSAudioSource::Read` (`ECX=alloc+4`; `memcpy` from `pPcmBuffer` + `*pReadCursor`) |
| `0x00429240` | `CDSAudioBank_ReleaseSampleSlots` | Releases slot vector (`+0x18`/`+0x1c` on **bank**); dtor/deserialize caller |
| `0x00439fe0` | `CDSIDSReferencedSub_ReleaseHeldRef` | `Release` on `*(this+0x10)` — invoked with **IDSReferenced** face at object `+4` → `pHeldRef` @ `+0x14` |

Deserialize path: `CDSAudioBank_Deserialize@0x00429858` → `OperatorNew(0x24)` → ctor; stores **`sample+4`** in bank slots (`0x00429572`).

## Ghidra apply

```
get_struct_layout CDSAudioBankSample
→ Size: 36 bytes (0x24)
```

Struct verified in Ghidra (agent slice **23**, 2026-05-30). Ctor plate comment notes `CDSAudioBankSample *this` — decompiler may still alias as `CDSAudioBank*` (thiscall ECX limitation).

## Multiple-inheritance map (0x24 object)

| Offset | Face | Vtable symbol | Slots (catalog) |
|--------|------|---------------|-----------------|
| 0x00 | `IDSReferenced` primary | `g_pCDSAudioBankSample_vftable_IDSReferenced_primary` (`0x486f34`) | 3 |
| 0x04 | **`IDSAudioSource`** (`face_8slots`) | `g_pCDSAudioBankSample_vftable_IDSAudioSource` (`0x486f10`) | 8 — bank slot pointer |
| 0x18 | `IDSStream` | `g_pCDSAudioBankSample_vftable_IDSStream` (`0x486efc`) | 4 |

Ghidra type **`IDSAudioSource`** (20 B) models the bank-slot view: `dwSampleByteSize`, `wChannels`, `wBitsPerSample`, `dwSampleRate`, `pCachedDirectSoundBuffer` @ +0x10 (aliases full-object `pHeldRef`).

## `dwInitFlag` vs `IDSAudioSource::Read` (round 3 todo 24)

| Mechanism | Field / API | Role |
|-----------|-------------|------|
| `CDSAudioBankSample_ctor@0x00429530` | `dwInitFlag` @ full **+0x1C** | Sole store: **`MOV [ESI+0x1c], 1`** @ `0x00429564`; **no reads** anywhere in `bulanci.exe` |
| `CDSAudioBankSample_HandleResourceRead@0x004291d0` | `*pReadCursor` **param** | Byte offset into PCM; advanced by Read; reset to **0** when `>= dwSampleByteSize` |
| Same @ `0x004291d0` | `[this+0x1c]` on **`IDSAudioSource*`** | **`pPcmBuffer`** base (full `+0x20`) — disasm `MOV ECX,[EBP+0x1c]; ADD ECX,EAX` before `_memcpy` |
| `CDSAudioPlayer_Init@0x0043a760` | `pSource->vtable+0x14` | `CDSWav_HandleAcquireReadThunk` → returns **0**; `vtable+0x18` Read with `player+0x2c` cursor |

Ghidra: `set_function_this_type` → **`IDSAudioSource *`** on Read @ `0x004291d0` (2026-05-30).

### `IDSAudioSource` vtable `0x486f10` (8 slots)

| Slot | Address | Symbol / role |
|------|---------|----------------|
| 0 | `0x0041a510` | `CDSWav_GetClassMeta` (shared) |
| 1 | `0x0042ac90` | shared thunk |
| 2 | `0x004330a0` | release refcount thunk |
| 3 | `0x00429350` | scalar-deleting dtor thunk (`-0x18` adjust) |
| 4 | `0x00467430` | shared no-op |
| 5 | `0x0043b950` | `CDSWav_HandleAcquireReadThunk` → **0** |
| 6 | `0x004291d0` | **Read** (PCM `memcpy`) |
| 7 | `0x00438340` | `CDSView_NoOpStub` |

## UNK

- Why ctor sets **`dwInitFlag=1`** with no consumer (reserved / dead field).
- Decoder-only `pDecoder` layout in `CDSAudioBank_Deserialize` (only `+8`/`+0xC` format copies proven at ctor).
