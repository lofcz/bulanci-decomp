# CDSAudioBank

## Status

**VERIFIED** (layout through `+0x38`, alloc `0x40`) — shared **`0x40`** heap footprint with **`CDSWavStream`** on class-43 resources; bank vs wav vtable sets documented (round-2 todo 23, round-3 todo 23). Ghidra struct **64** bytes (`pad_operatorNew0x40` @ `+0x3c`); embedded **`CDSPtrSlotVec slotVector`** @ `+0x18`; `pGapWavPcmHelpers` @ `+0x24` (wav-only, bank ctor does not init). `+0x08` `dwInitFlag`; `+0x14` `pVftable_bankDeserialize` (factory zeros for class-43 wav). Factory tail: `pStreamStorage` @ `+0x20`, `dwReservedTail` @ `+0x38` (zeroed by `CDSWavStream_Factory`).

## CDSAudioBank vs CDSWavStream (class-43)

Two **distinct** MSVC types (`.?AVCDSAudioBank@@` @ `0x004afc64`, `.?AVCDSWavStream@@` @ `0x004af260`) on the **same `OperatorNew(0x40)` layout** — multiple-inheritance-style vtable pointers at different offsets, not a single renamed class.

| | **CDSWavStream** | **CDSAudioBank** |
|---|------------------|------------------|
| **Registry** | Class id **43** decimal (`PUSH 0x2b` @ `0x0047d9ea`); factory `CDSWavStream_Factory@0x0043bb00`; typeinfo `0x004b843c` | Typeinfo `0x004b3ae0` used @ `0x0047c4b0` with class id **67** decimal (`PUSH 0x43`) — **AudioBankIndex**, not the PCM blob |
| **Catalog “AudioBank.wav” (class 43)** | Runtime factory + vtables | Domain name only; **not** `CDSAudioBank_Ctor` |
| **Vtables `+0x00..+0x18`** | `0x4823c0`, `0x48239c`, `0x482388` | `0x486eec`, `0x486ed8`, `0x486ec0`, `dwInitFlag@+0x08` |
| **`+0x14`** | Factory → **0** | `0x486ea4` — `CDSAudioBank_Deserialize` vtable; **only** ctor/dtor write |
| **`+0x30`, `+0x34`** | `0x48236c`, `0x482354` (stream attach / save) | Not set by `CDSAudioBank_Ctor` |
| **`+0x08`** | `dwPcmEndBound` on wav (`CDSWav_HandleResourceRead`) | Bank ctor writes **1** (`in_EAX[2]=1`) — same physical offset, facet-specific meaning (todo 50) |
| **Construction** | Factory + `CGaming_LoadBackgroundMusic@0x0041b6d0` (inline) | `CDSAudioBank_Ctor@0x00429480` — **no CALL xrefs** |
| **Destruction** | `CDSWavStream_ScalarDeletingDtor@0x0041bc00` | `CDSAudioBank_ScalarDeletingDtor@0x004294b0` → bank dtor restores `0x486exx` |
| **PCM / samples on class-43** | `CDSWav::FUN_0043ba30` (`+0x04` face); stream `FUN_0043bb50` / `CDSWavStream_SaveToStream` (`+0x30`) | `CDSAudioBank_Deserialize@0x00429600` requires `+0x14` bank vtable — **not** installed by class-43 factory |

**Practical rule:** treat **class-43 heap instances as `CDSWavStream`** for vtable dispatch; use **`CDSAudioBank`** naming for the **bank facet** (`0x486exx`, slot vector `+0x18`/`+0x1c`, dtor/deserialize) when those pointers are present.

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `OperatorNew(0x40)` for class **43** (`0x2b`) | `0x0043bb00` | `CDSWavStream_Factory` — `PUSH 0x40` before `OperatorNewWithBadAlloc`; registered @ `0x0047d9e0` (`PUSH 0x2b`, factory `0x0043bb00`) |
| Slot release loop | `0x00429240` | `CDSAudioBank_ReleaseSampleSlots` — walks `slotVector.nCapacity`, `Release` each `pSlots[i-1]` |
| Slot vector at `+0x18` | `0x00406340` | `CDSPtrSlotVec_Resize(&slotVector, …)`; dtor also `Resize(param_1+6)` ≡ `+0x18` |
| Deserialize uses same tail | `0x00429600` | `CDSAudioBank_Deserialize` → `ResizeSlots` on `&pVftable_IDSEventHandler`; `FUN_00429240(this-0x14)` |
| 8-byte slot-only heap blob | `0x00401d70` | `CBulanci_BuildBitmapCache` → `OperatorNewWithBadAlloc(8)` then `ResizeSlots` only |
| Embedded in `CDSDsmFile` | `0x00429060` | `HandleDestructInstance` → `CDSPtrSlotVec_Resize` at `+0x54` (handle collection, not full `0x40` object) |
| `+0x38` cleared on factory alloc | `0x0043bb00` | `CDSWavStream_Factory` → `MOV [EAX+0x38], 0` |

## Layout

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| 0x00 | 4 | `void *` | `pVftable_IDSReferenced` | Bank: `CDSAudioBank_Ctor@0x00429480` → `0x486eec`; Wav factory: `0x4823c0` @ `CDSWavStream_Factory@0x0043bb00` |
| 0x04 | 4 | `void *` | `pVftable_IDSEventHandler` | Bank: `0x486ed8`; Wav: `0x48239c` (`face_8slots`) @ factory |
| 0x08 | 4 | `uint` | `dwInitFlag` | `CDSAudioBank_Ctor` → `*(this+0x08) = 1` only (wav factory does not set) |
| 0x0C | 4 | `void *` | `pVftable_sub0c` | Bank: `0x486ec0` @ ctor; wav factory does not set |
| 0x10 | 4 | `void *` | `pParentOrBackref` | `CDSAudioBank_Deserialize` tests `*(this-4)`; `CDSAudioBank_Ctor` → `+0x10 = param_1` |
| 0x14 | 4 | `void *` | `pVftable_sub14` | Bank: `0x486ea4` @ ctor (Deserialize vtable); factory → **0** |
| 0x18 | 8 | `CDSPtrSlotVec` | `slotVector` | `CDSAudioBank_ReleaseSampleSlots@0x00429240` → `nCapacity` / `pSlots`; `CDSPtrSlotVec_Resize@0x00406340` |
| 0x20 | 4 | `pointer` | `pStreamStorage` | `CDSWavStream_Factory` zero; `FUN_0043bb50` / `CDSWavStream_SaveToStream` (task 08) |
| 0x24 | 12 | — | *(gap)* | `FUN_0043ba30` uses `+0x24`/`+0x28` on wav path |
| 0x30 | 4 | `void *` | `pVftable_CDSWavStream_IDSChained6` | Factory → `0x48236c`; bank ctor does not set |
| 0x34 | 4 | `void *` | `pVftable_CDSWavStream_IDSChained5` | Factory → `0x482354`; bank ctor does not set |
| 0x38 | 4 | `uint` | `dwReservedTail` | `CDSWavStream_Factory` → `[EAX+0x38]=0`; no runtime consumer on 0x40-byte object |
| 0x3C | 4 | `byte[4]` | `pad_operatorNew0x40` | Heap alloc padding to `OperatorNew(0x40)` |

## Ghidra apply

```
get_struct_layout CDSAudioBank → size 64 (0x40)
  slotVector @ 0x18; pVftable_bankDeserialize @ 0x14; gapWavPcmHelpers @ 0x24; wav vtables @ 0x30/0x34
rename 0x0043bb00 CDSWavStream_Factory; 0x00429240 CDSAudioBank_ReleaseSampleSlots
prototype CDSAudioBank_Ctor(CDSAudioBank *, void *) @ 0x00429480
prototype CDSAudioBank_Deserialize(CDSAudioBank *, int *pStream) @ 0x00429600
decompiler comments: class-43 = CDSWavStream facet; bank facet = 0x486exx + Deserialize
recreate_struct CDSAudioBank 64 B with explicit offsets (round-3 todo 23): pVftable_bankDeserialize @ 0x14, slotVector @ 0x18, pStreamStorage @ 0x20, dwReservedTail @ 0x38, pad @ 0x3c
set_function_prototype / set_function_this_type @ 0x00429480, 0x00429600
save_program bulanci.exe  (round-2 todo 23, round-3 todo 23, 2026-05-30)
```

## CGame consumer (audio bank tail)

`CGame+0x22c..+0x247` holds **`pAudioBankArray`** / count / capacity — dynarray of bank **handles**, not embedded `CDSAudioBank` objects (`CGame.md`, `CBulanci_ctor@0x0040275b`). Teardown: `CDSPtrSlotVec_Resize` on `(game)+0x2c4..0x2f4` @ `CBulanci_dtor@0x00402c8a` region.

## Class registry note

- **Class 43** (decimal, `0x2b`): `CDSWavStream_Factory@0x0043bb00`, typeinfo `CDSWavStream` (`0x004b843c`).
- **Class 67** (decimal, `0x43`): `CDSAudioBank` typeinfo `0x004b3ae0` @ `0x0047c4b0` (index metadata — do not confuse with class 43).
- MI typeinfo adjust thunks (`CDSAudioBank_TypeinfoAdjust_*`, parent meta `0x004b3ae0`): `+4` @ `0x0042fd30`, `+0xc` @ `0x0043bdd0`, `+0x14` @ `0x004291c0` (registered @ `0x0047c4e8`..`0x0047c548`).

## Follow-up

- Round-3 task **08**: `pStreamStorage` / `dwReservedTail` semantics.
- Round-3 task **02**: type relationship (this section).

## UNK

- `CDSAudioBank_Ctor` — no direct CALL xrefs; possible indirect use via class-67 registration (`0x00429470`).
- Full MI inheritance declaration order (C++ source).
- `+0x24..+0x2f` — wav PCM helpers (`FUN_0043ba30`); bank ctor does not initialize.
