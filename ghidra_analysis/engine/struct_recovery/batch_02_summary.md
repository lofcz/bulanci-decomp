# Struct recovery batch 2/50

**Batch index:** 2 (`batches_50.json`)  
**Structs:** `CDSScript`, `CAdvertising`  
**Program:** `bulanci.exe`  
**Agent slice 02 (2026-05-30):** Re-verified layouts in Ghidra; refreshed `CAdvertising` struct + ctor/dtor/timer/key prototypes; `CDSScript_InstallOpcodeTable` rename; `save_program`.  
**Saved:** yes (`save_program` at end of slice 02 pass)

## Results

| Struct | Status | Size | Ghidra apply |
|--------|--------|------|--------------|
| CDSScript | VERIFIED / PARTIAL layout | 0x430 (1072) | `create_struct` — 15 fields, `framePtr` @ 0x42c |
| CAdvertising | VERIFIED / PARTIAL layout | 0x90 (144) | `create_struct` — 11 fields, `m_bBlockDismiss` @ 0x8c |

## Evidence highlights

- **CDSScript:** ctor/dtor/Run/CallExport/InstallOpcodeTable/ReadU8; size bound by `CLevelScript` using `+0x434` immediately after base (`OperatorNew 0x460` is subclass-only).
- **CAdvertising:** stack `[144]` in menu state machine `0xf7` splash path; MI ctor sets four vtables + `CDSUpdatedItem@+0x70`; dismiss gated at `+0x8c`.

## Deliverables

- `CDSScript.md`
- `CAdvertising.md`
- This summary

## Notes

- Removed 1-byte placeholder structs via inline Ghidra script before `create_struct`.
- Gap `CDSScript+0xe0..+0x42b` left as padding; opcode extensions live in subclass constructors.
- Next batch index 3: `CAnim`, `CBitmap`.

## Follow-up (agent todo 04, 2026-05-30)

- Deleted nested `/CDSScript/CDSScript` (1 B) + stale pointer type; canonical `/CDSScript` **1072 B** retained.
- `CDSScript_dtor@0x00438400`, `CDSScript_ctor@0x00438390`, `CDSScript_InstallOpcodeTable@0x00438310`, `CDSScript_Run@0x00438b30` — `CDSScript *` prototypes applied; `save_program bulanci.exe`.
