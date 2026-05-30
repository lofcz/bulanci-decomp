# Struct recovery batch 6/50

**Structs:** `CDeath2`, `CDirectKeyb`  
**Program:** `bulanci.exe`  
**Evidence:** Ghidra MCP decompile + `OperatorNew` / ctor / consumer xrefs only

## Results

| Struct | Status | Size | Proven fields |
|--------|--------|------|---------------|
| `CDeath2` | PARTIAL | `0xfc` | 11 slots (vtables, `+0x68/+0x6c` state, `+0xf4/+0xf8` tail); `CAnim` base unpadded |
| `CDirectKeyb` | VERIFIED | `0x20c` | 5 fields (vtable, DI buffers, `IDirectInput*` / `IDirectInputDevice*`) |

## Key evidence anchors

- `CDeath2_SubobjectCtor` @ `0x0041a8c0` — extends `CAnim`, installs `CDeath2` vtables, zeros `+0xf4/+0xf8`.
- `CreateObject` @ `0x0041bcb0` — `OperatorNew(0xfc)` for `CDeath2` (class id path; contrast `CDeath` @ `0x108`).
- `CDeath2_UpdateStateFromParams` @ `0x00417af0` — copies two dwords into `+0x68/+0x6c`.
- `CDirectKeyb_ctor` @ `0x004121d0` — DirectInput create/device, `memset` key buffer at `+0x4`.
- `CDirectKeyb_PollKeyboard` @ `0x00412160` — snapshot `+0x4` → `+0x104`, `GetDeviceState` into `+0x4`.
- `CreateObject` / `CGame_StartGame` — `OperatorNew(0x20c)` for keyboard object.

## Ghidra actions

- [x] `add_struct_field` `CDirectKeyb` (size `0x20c`)
- [x] `add_struct_field` `CDeath2` (size `0xfc`, sparse proven tail)
- [x] `get_struct_layout` size > 1 for both
- [x] `save_program bulanci.exe`

## Follow-ups

- Recover `CAnim` / `ODSImage` to upgrade `CDeath2` to VERIFIED and collapse Ghidra auto-pad names.
- Recover `CDeath` (`0x108`) to diff tail (`+0xfc` byte, `+0x100/+0x104`) vs `CDeath2`.
- Type-fix `CDirectKeyb_GetKeyEdge` first parameter to `CDirectKeyb*` for decompiler clarity.
