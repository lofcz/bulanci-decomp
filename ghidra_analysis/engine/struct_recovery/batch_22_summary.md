# Struct recovery batch 22/50

**Structs:** `CDSApiException`, `CDSAudioBank`  
**Program:** `bulanci.exe`  
**Evidence:** Ghidra MCP decompile + allocation / ctor / consumer xrefs only

## Results

| Struct | Status | Size | Proven fields |
|--------|--------|------|---------------|
| `CDSApiException` | VERIFIED | `0x44` | Full layout: `CDSException` base through `+0x3b`, `pFormattedMessage` `+0x3c`, `dwWin32Error` `+0x40` |
| `CDSAudioBank` | VERIFIED | `0x40` alloc / **60** B Ghidra | MI vtables `+0x00..+0x14`, `CDSPtrSlotVec slotVector` @ `+0x18`, stream/tail per `CDSAudioBank.md` |

## Key evidence anchors

- `CDSApiException_ThrowFromGetLastError` @ `0x00434d00` — `OperatorNew(0x44)`, vtable `0x487564`, Win32 code at `+0x40`.
- `CDSApiException_What` @ `0x00434c70` — formats into `+0x3c` using `*(this+0x40)`.
- `CDSAudioBank_dtor` @ `0x004293e0` — releases samples via `FUN_00429240`, clears slots at `+0x18`.
- `CDSAudioBank_Deserialize` @ `0x00429600` — resizes slot array, constructs `CDSAudioBankSample` (`0x24`) per entry.
- `CDSAudioBank_ResizeSlots` @ `0x00406340` — `{pData,+0x0; capacity,+0x4}` sub-object.

## Ghidra actions

- [x] Removed placeholder 1-byte structs; `create_struct` `CDSApiException` (68 bytes)
- [x] `create_struct` `CDSAudioBank` (32 bytes with alignment tail)
- [x] `get_struct_layout` verified size > 1 for both
- [x] `save_program bulanci.exe`

## Follow-ups (post slice-22 agent)

- `CDSWavStream_Factory` @ `0x0043bb00` named; class 43 uses wav vtables on same `0x40` footprint.
- `CDSException` base applied (round 3 task 27); `CDSApiException` embeds `base`.
- `CDSAudioBank_ReleaseSampleSlots` @ `0x00429240`; `slotVector` typed — see `batch_22_agent_slice22.md`.
