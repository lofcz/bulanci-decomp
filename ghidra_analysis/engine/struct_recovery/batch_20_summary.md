# Struct recovery batch 20/50

**Structs:** `CDSSafeStream`, `CDSSafeStreamInfo`  
**Program:** `bulanci.exe`  
**Evidence:** Ghidra MCP decompile + `OperatorNew` / ctor / consumer xrefs only

## Results

| Struct | Status | Size | Proven fields |
|--------|--------|------|---------------|
| `CDSSafeStream` | PARTIAL | `0x48` | 13 fields — MI vtables, flags, refcnt, chain head, CS, name handle |
| `CDSSafeStreamInfo` | VERIFIED | `0x18` | 6 fields — vtables, chain links, thread id, stream slice ptr |

## Key evidence anchors

- `CDSSafeStream_ctor` @ `0x00433ab0` — installs four MI vtables, embeds `CDSChain`, init CS at `+0x2c`, calls `FUN_00446ea0`.
- `CDSSafeStream_dtor` @ `0x00433bc0` — clears thread list (`FUN_00446c30`), releases name @ `+0x44`, destroys CS, `CDSChain_dtor`.
- `OperatorNew(0x48)` @ `0x00434160` (CBulanci attach) and @ `0x00434760` (stream-storage wrap over `CDSFilterStream`).
- `FUN_00446d90` @ `0x00446d90` — CS-guarded lookup/create of per-thread `CDSSafeStreamInfo` node; `OperatorNew(0x18)`.
- `FUN_00446ea0` @ `0x00446ea0` — copies resource name, registers calling thread's stream slice.
- `CDSSafeStreamInfo_dtor` @ `0x00446cd0` — releases `pStream` at `+0x14`.
- IDSStream dispatch: `Read/Seek/Tell/GetSize` @ `0x00446f90`..`0x00447000` delegate through `FUN_00446d90`.

## Ghidra actions

- [x] `get_struct_layout CDSSafeStream` → 72 bytes
- [x] `get_struct_layout CDSSafeStreamInfo` → 24 bytes
- [x] `save_program bulanci.exe`

## Follow-ups

- Recover `CDSFilterStream` base (batch index 19) to upgrade `CDSSafeStream` to full VERIFIED MI layout.
- Recover `CDSChain` / `CDSChained` to name `field_20..28` and `CDSSafeStreamInfo` link fields.
- Name `FUN_00446ea0` / `FUN_00446d90` / `FUN_00446c30` once chain helpers are catalogued.
