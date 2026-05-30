# Struct recovery batch 7/50

**Structs:** `CExitDlg`, `CExplosion`  
**Program:** `bulanci.exe`  
**Evidence:** Ghidra MCP decompile + `OperatorNew` / ctor / consumer xrefs only

## Results

| Struct | Status | Size | Proven fields |
|--------|--------|------|---------------|
| `CExitDlg` | PARTIAL | `0x7C` | `CWindow` base through `+0x6C`; tail `+0x70` read on `0x8004`; `+0x74`/`+0x78` ctor-only |
| `CExplosion` | PARTIAL | `0xF4` | `CAnim` base `0xF0` + `ownerSlot` @ `+0xF0`; bbox `+0x20..+0x2C`, `pGaming` `+0x84`, vtables `+0x88..+0x98` |

## Key evidence anchors

- `CExitDlg_ctor` @ `0x00411b50` — `CWindow_BuildAt(0xdd,0x18,0x2fa,0x239,0)`; vtables; zeros `+0x70..+0x78`.
- `CreateObject` @ `0x0041212d` / `CMenu_CmdDispatch` @ `0x00425a0f` — `OperatorNew(0x7c)`.
- `CExitDlg_RouteSyntheticCloseEvent` @ `0x0040b290` — cmd `0x8004` uses `+0x70`, posts via `+0x4c`.
- `CExplosion_Ctor` @ `0x0041ce30` — `CAnim` sub-ctor, vtables, `ownerSlot` @ `+0xF0`, bbox centering, `TM_BindSequence` @ `+0x98`.
- `CShot_ResolveHit` — `OperatorNew(0xf4)` when `weapon+0xa6==2`.
- `ApplyAreaDamage` @ `0x0041e140` — blast AABB from `+0x20..+0x2C` ±`0x3C`; `pGaming` @ `+0x84`.
- `DamageAtPoint` @ `0x0041b250` — `ownerSlot` @ `+0xF0` passed to `CGaming_OnSlotPlacementEvent`.

## Ghidra actions

- [x] `create_struct` `CExitDlg` (size `0x7C`)
- [x] `create_struct` `CExplosion` (size `0xF4`)
- [x] `get_struct_layout` verified size > 1 for both
- [x] `save_program bulanci.exe`

## Follow-ups

- Recover `CWindow` / `CDSChained` base to upgrade `CExitDlg` tail fields `+0x74`/`+0x78`.
- Recover `CAnim` (batch index 3) to expand `CExplosion` layout below `+0x98`.
- Find writer for `CExitDlg+0x70` (likely button/anim hookup during `CButton` build).
