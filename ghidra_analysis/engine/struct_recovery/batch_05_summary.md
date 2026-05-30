# Struct recovery batch 05/50

**Index:** 5 (`batches_50.json`)  
**Structs:** `CDeath2`, `CDirectKeyb`  
**Program:** `bulanci.exe`

## Results

| Struct | Status | Size | Ghidra apply |
|--------|--------|------|--------------|
| `CDirectKeyb` | **VERIFIED** | `0x20c` (524) | Applied — 5 fields |
| `CDeath2` | **PARTIAL** | `0xfc` (252) | Applied — 22 fields (17 proven + 5 padding gaps) |

## Evidence highlights

### CDirectKeyb

- Allocation: `OperatorNewWithBadAlloc(0x20c)` in `CGame_StartGame@0x00413e79` and `CreateObject@0x004123f1`.
- ctor `@0x004121d0` lays out vtable, `0x100` key buffer at `+0x04`, DirectInput at `+0x204`, keyboard device at `+0x208`.
- Poll/edge: `CDirectKeyb_PollKeyboard@0x00412160` double-buffer at `+0x04` / `+0x104`; scheduler event 7 in `CGame__SchedulerDispatch@0x00416030`.

### CDeath2

- Allocation: `OperatorNewWithBadAlloc(0xfc)` in `CBulanek_OnDeath@0x0041fa21` for tournament slots (`entity kind 0x20..0x23`).
- Subobject init: `CDeath2_SubobjectCtor@0x0041a8c0` (CAnim base + CDeath2 vtables).
- Gameplay: tombstone bind via `CBulanek_BindDeathTombstoneAnim@0x0041c860`; parent pointer at `+0xf0`, placement offsets `+0xf4`/`+0xf8`.
- Network state copy: `CDeath2_UpdateStateFromParams@0x00417af0` writes `+0x68`/`+0x6c`.

## Deliverables

- `ghidra_analysis/engine/struct_recovery/CDeath2.md`
- `ghidra_analysis/engine/struct_recovery/CDirectKeyb.md`
- Ghidra: replaced 1-byte placeholders, applied layouts, `save_program bulanci.exe`

## Follow-up

- Batch 3 `CAnim` recovery will fill `CDeath2` padding regions (`0x30..0x67`, embedded anim at `+0x98`).
- Compare with `CDeath` (`0x108`) for shared death-entity field naming.
