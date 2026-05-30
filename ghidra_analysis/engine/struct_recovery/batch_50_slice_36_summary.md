# Struct recovery — batches_50 slice 36

**Index:** 36 (`batches_50.json`)  
**Types:** `CDSRegKeyException`, `CDSResInfo`  
**Program:** `bulanci.exe`  
**Saved:** yes

## Results

| Struct | Status | Size | Ghidra |
|--------|--------|-----:|--------|
| `CDSRegKeyException` | VERIFIED | `0x48` (72) | `delete_data_type` + `create_struct` (10 fields) |
| `CDSResInfo` | VERIFIED | `0x18` (24) | `delete_data_type` + `create_struct` (5 fields) |

## Ghidra actions

- Renamed `FUN_004380c0` → `CDSRegKeyException_AllocDefault`
- Renamed `FUN_00433aa0` → `CDSResInfo_ReleaseViaChainedFace`
- Prototypes: `CDSRegKeyException_Ctor`, `CDSRegKeyException_What`, `CDSResInfo_Load`/`Save`/`ReleaseEmbeddedResource`
- `save_program bulanci.exe`

## Deliverables

- [CDSRegKeyException.md](./CDSRegKeyException.md)
- [CDSResInfo.md](./CDSResInfo.md)

## Blockers

None for slice scope. Out of scope: embed `CDSException` component on `CDSRegKeyException` (flat prefix matches `CDSResourceException` pattern); expand `IDSChained` vtable slots.
