# Round 4 — agent todo 23 report (R4 worker)

## Task

| Field | Value |
|-------|-------|
| **id** | 23 |
| **title** | Retype CDSAudioBank_Deserialize MI facet this at +0x14 adjustor |
| **source** | blocker (supersedes R3 todo 23) |
| **types** | CDSAudioBank, CDSWavStream, CDSAudioBankSample |
| **addresses** | `0x00429600`, `0x00429480`, `0x00429858`, `0x0043bb00` |

## Status

**DONE** — `CDSAudioBank_Deserialize` **`this`** is **`CDSAudioBank_BankDeserializeFacet *`** (vtable dispatch ECX at full `+0x14`). Decompile uses `this->slotVector`, `this[-1].pParentOrBackref`, and typed slot fills; R3 `this[-1].pGapWavPcmHelpers + 8` artifact removed.

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| ECX at entry = facet `+0x14` | `CDSAudioBank_Deserialize@0x00429600` | `MOV EDI,ECX` @ `0x0042962b`; plate: bank deserialize MI |
| Base adjustor `-0x14` for release | `CDSAudioBank_Deserialize@0x00429600` | `LEA ECX,[EDI-0x14]` @ `0x00429630` → `CDSAudioBank_ReleaseSampleSlots` |
| Slot vector on facet `+4` | `CDSAudioBank_Deserialize@0x00429600` | `ADD EDI,4` @ `0x004296b5`; `ECX=EDI` → `CDSPtrSlotVec_Resize`; decompile `this_01 = &this->slotVector` |
| Parent backref at facet `-4` | `CDSAudioBank_Deserialize@0x00429600` | `MOV EAX,[EDI-0x4]` @ `0x00429656`; decompile `this[-1].pParentOrBackref` |
| Sample store `alloc+4` | `CDSAudioBank_Deserialize@0x00429600` | `ADD EAX,4` @ `0x004297e3` before slot write; `&pCVar5->pVftable_IDSAudioSource` |
| Factory skips bank facet | `CDSWavStream_Factory@0x0043bb00` | Zeros `+0x14` (`pVftable_sub14`); class-43 uses wav save/load @ `+0x30` |
| Facet struct layout | `get_struct_layout` | `CDSAudioBank_BankDeserializeFacet` 48 B; `slotVector` @ facet `+4` ≡ full `+0x18` |
| Ctor unchanged (primary base) | `CDSAudioBank_Ctor@0x00429480` | Still `CDSAudioBank *`; writes `pVftable_bankDeserialize` / seeds `slotVector` |

### Decompile before / after (high level)

| Before (R3 blocker) | After (R4) |
|---------------------|------------|
| `CDSAudioBank_ReleaseSampleSlots(this[-1].pGapWavPcmHelpers + 8)` | `CDSAudioBank_ReleaseSampleSlots(&this[-1].pVftable_CDSWavStream_IDSChained6)` — **same address** as `(CDSAudioBank *)((char *)this - 0x14)` |
| `CDSPtrSlotVec_Resize((CDSAudioBank *)&this->pVftable_IDSEventHandler, …)` | `this_01 = &this->slotVector`; `CDSPtrSlotVec_Resize((CDSAudioBank *)this_01, …)` |
| `*(int *)this[-1].pPad_operatorNew0x40` | `this[-1].pParentOrBackref` |

Note: `0x00429858` is the function epilog (`RET 0x4`), not a separate symbol.

## Ghidra deltas

- `create_struct` **`CDSAudioBank_BankDeserializeFacet`** (48 B; tail `pParentOrBackref` placed @ `+0x2c` so `this[-1].pParentOrBackref` ≡ full `+0x10`)
- `set_function_this_type` **`CDSAudioBank_BankDeserializeFacet *`** @ `0x00429600`
- `set_function_prototype` `void __thiscall CDSAudioBank_Deserialize(CDSAudioBank_BankDeserializeFacet *this, int *pStream)`
- `set_plate_comment` / `set_decompiler_comment` @ `0x00429600`, `0x00429630`, `0x004296b5`
- `force_decompile` @ `0x00429600`
- `save_program bulanci.exe`

## Struct doc updates

- [CDSAudioBank.md](./CDSAudioBank.md) — MI deserialize facet table; Ghidra apply log (R4 todo 23)

## Remaining UNK

- `CDSAudioBank_ReleaseSampleSlots` / `CDSPtrSlotVec_Resize` call sites still show **`CDSAudioBank::`** namespace prefix and occasional `(CDSAudioBank *)` casts on `CDSPtrSlotVec *` (display-only; prototypes unchanged).
- `CDSAudioBank_Ctor@0x00429480` — still no CALL xrefs (class-67 index vs class-43 factory path).
- Full MSVC MI inheritance order in C++ source (not required for offset proof).
