# Slice 22/50 agent report — `CDSApiException`, `CDSAudioBank`

**Date:** 2026-05-30  
**Program:** `bulanci.exe`

## Types mapped

| Type | Ghidra size | Status |
|------|-------------|--------|
| `CDSApiException` | 68 (`0x44`) | VERIFIED — `CDSException base` + `pFormattedMessage` + `dwWin32Error` |
| `CDSAudioBank` | 60 (`0x40` alloc) | VERIFIED layout — `CDSPtrSlotVec slotVector` @ `+0x18` |
| `CDSException` (base) | 60 | Used embedded by `CDSApiException` |
| `CDSPtrSlotVec` (embedded) | 8 | `slotVector` member |

## Leaf functions (evidence)

| Address | Name | Role |
|---------|------|------|
| `0x00434d00` | `CDSApiException_ThrowFromGetLastError` | `OperatorNew(0x44)`, vtable `0x487564`, tail fields |
| `0x00434c70` | `CDSApiException_What` | Formats via `pFormattedMessage`, reads `dwWin32Error` |
| `0x00434a40` | `CDSException_InitFields` | Shared prefix |
| `0x00429480` | `CDSAudioBank_Ctor` | Bank vtables `0x486exx`, `dwInitFlag=1`, seeds `+0x10`/`slotVector` |
| `0x004293e0` | `CDSAudioBank_dtor` | `ReleaseSampleSlots`, `CDSPtrSlotVec_Resize` |
| `0x00429240` | `CDSAudioBank_ReleaseSampleSlots` | Release each sample in `slotVector` |
| `0x00429600` | `CDSAudioBank_Deserialize` | Resize slots, `CDSAudioBankSample` `0x24` per entry |
| `0x00406340` | `CDSPtrSlotVec_Resize` | `{pSlots, nCapacity}` helper |
| `0x0043bb00` | `CDSWavStream_Factory` | Class 43 `OperatorNew(0x40)`, wav vtables |

## Ghidra actions

- Verified `get_struct_layout` for `CDSApiException`, `CDSAudioBank`
- `CDSAudioBank`: replaced `pSampleSlots`/`dwSampleSlots` with `CDSPtrSlotVec slotVector` @ `+0x18`; restored 60-byte tail
- Renamed `FUN_00429240` → `CDSAudioBank_ReleaseSampleSlots`
- Prototypes: `CDSAudioBank_ReleaseSampleSlots`, `CDSAudioBank_Ctor`, `CDSPtrSlotVec_Resize`
- `save_program bulanci.exe`

## Files changed

- `struct_recovery/CDSAudioBank.md`
- `struct_recovery/CDSApiException.md`
- `struct_recovery/batch_22_agent_slice22.md` (this file)

## Blockers

- None for slice scope. `CDSAudioBank_Ctor` still has no CALL xrefs (class-67 / MI path only).
- `+0x24..+0x2f` on `0x40` layout: wav-only; bank ctor does not touch.
- `CDSPtrSlotVec_Resize` call sites may still show `CDSAudioBank *` cast when `this` is `bank+0x18` (decompiler quirk).
