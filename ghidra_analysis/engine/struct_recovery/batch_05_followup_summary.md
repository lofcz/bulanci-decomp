# Struct recovery — batch 05 follow-up (round 2)

**Prior:** `batch_05_summary.md`  
**Structs:** `CDeath2`, `CDirectKeyb`  
**Program:** `bulanci.exe`  
**Status:** **FOLLOWUP_DONE** (partial structs remain)

## Inputs read

- `batch_05_summary.md`
- `CDeath2.md`, `CDirectKeyb.md`
- `CAnim.md` (batch 3 `CAnim` recovery for inheritance)

## Actions

### CDirectKeyb

- No batch-05 follow-ups and no UNK in struct doc — **no Ghidra or doc changes**.

### CDeath2

1. **CAnim inheritance / padding `0x30..0x67`, `+0x98`**
   - Confirmed `CDeath2_SubobjectCtor` / `CDeath2_ctor` call `CAnim_SubobjectCtor`; tombstone path uses `TM_*` on `this+0x98`.
   - Proven `CAnim` coordinates on `CDeath2`: `origin_x/y` `+0x20`/`+0x24`, `draw_pos_x/y` `+0x28`/`+0x2c` via `CBulanek_BindDeathTombstoneAnim@0x0041c860`.
   - Proven `m_pPalette` `+0x94`, `m_pParent` `+0xf0`, placement tail `+0xf4`/`+0xf8` (tombstone + ctors).
   - Updated `CDeath2.md` layout table and status notes.

2. **`CDeath` (`0x108`) comparison**
   - Decompiled `CDeath_SubobjectCtor@0x0041a860`, `CDeath_ctor@0x00419aa0`, `CBulanek_OnDeath@0x0041fa21`.
   - Documented shared `+0xf0` parent, tail dword shift (`CDeath2` `+0xf4/+0xf8` vs `CDeath` `+0x100/+0x104`), and `CDeath`-only `+0xf4` flags / `+0xfc` byte in `CDeath2.md`.

### Ghidra deltas

- `modify_struct_field` on `CDeath2`: `origin_x`, `origin_y`, `draw_pos_x`, `draw_pos_y`, `m_netStateWord0/1`, `m_pPalette`, `m_pParent`, `m_placementOffsetX/Y`.
- `save_program bulanci.exe` (once).

## Remaining UNK

- `CDeath2` `+0x30..+0x67` interior (ODSImage/drawable header).
- `CDeath` dedicated struct recovery (`CDeath.md`) — assigned to later batch in `batches_50.json`.
- `CAnim` bytes at `+0x68..+0x6f` vs death net dwords — semantic overlap unresolved.
- `CDirectKeyb_GetKeyEdge` `this` typing (batch 06 note, out of scope for batch 05).

## Struct status after follow-up

| Struct | Status | Size |
|--------|--------|------|
| `CDirectKeyb` | VERIFIED | `0x20c` |
| `CDeath2` | PARTIAL (richer) | `0xfc` |
