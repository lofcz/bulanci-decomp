# Struct recovery — batch 22 follow-up (round 2)

**Prior:** `batch_22_summary.md`  
**Structs:** `CDSApiException`, `CDSAudioBank`  
**Program:** `bulanci.exe`  
**Status:** **FOLLOWUP_DONE**

## Inputs read

- `batch_22_summary.md`
- `CDSApiException.md`, `CDSAudioBank.md`

## Actions

### CDSAudioBank — factory @ `0x0043bb00` / full `sizeof`

1. Created **`CDSWavStream_Factory@0x0043bb00`** (was undefined code): `OperatorNewWithBadAlloc(0x40)`; initializes vtables `0x4823xx`, `*(obj+0x1c)=1`, clears `+0x14`/`+0x20`/`+0x38`.
2. **`HandleClassRegister`** site @ `0x0047d9e0`: `classId == 0x2b` (43), factory `0x0043bb00` (matches resource-pack AudioBank class in catalog).
3. Created **`CDSAudioBank_Ctor@0x00429480`**: writes `CDSAudioBank` vtables `0x486exx`, **`*(this+0x08)=1`**, seeds `+0x10`/`+0x18`/`+0x1c` from `ECX`.
4. Documented MI adjust factories (`+4`/`+0xc`/`+0x14`) @ `0x0042fd30`, `0x0043bdd0`, `0x004291c0` with parent meta `0x004b3ae0`.
5. Ghidra: extended `CDSAudioBank` to **60** bytes (`dwInitFlag`, `dwField_20`, `pVftable_sub30/34`, `dwField_38`); renamed factory to `CDSWavStream_Factory`.
6. Updated `CDSAudioBank.md`.

### CDSApiException — `CDSException` base dedup

- **Deferred** to batch 29 / exception pass (no layout change; `CDSApiException` remains **VERIFIED** @ `0x44`). Cross-ref note added to `CDSApiException.md`.

### Ghidra deltas

- `create_function` `CDSWavStream_Factory`, `CDSAudioBank_Ctor`, MI adjust thunks (already existed as code).
- `rename_function_by_address` `0x0043bb00` → `CDSWavStream_Factory`.
- `modify_struct_field` / `add_struct_field` on `CDSAudioBank` (size 32 → 60).
- `save_program bulanci.exe` (once).

## Remaining UNK

| Item | Notes |
|------|--------|
| `CDSAudioBank` `dwField_20`, `dwField_38` | Factory zero only |
| `+0x24..+0x2f` gap | No xref in batch scope |
| `CDSAudioBank` vs `CDSWavStream` typing | Same `0x40` alloc for class 43; different vtable stores at factory vs `CDSAudioBank_Ctor` |
| Formal `CDSException` struct | Batch 29 manifest |

## Struct status after follow-up

| Struct | Status | Size |
|--------|--------|------|
| `CDSApiException` | VERIFIED | `0x44` |
| `CDSAudioBank` | PARTIAL | **`0x40` alloc** (Ghidra struct 60 B) |
