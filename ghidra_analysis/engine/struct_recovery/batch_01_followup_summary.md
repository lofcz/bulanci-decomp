# Struct recovery batch 01 follow-up (round 2)

**Status:** `FOLLOWUP_DONE`  
**Program:** `bulanci.exe` — `save_program` at end of batch  
**Prior:** `batch_01_summary.md`, `CBulanci.md`, `CDSObject.md`

## Actions

| Struct | Follow-up item (from batch 01) | Result |
|--------|--------------------------------|--------|
| `CBulanci` | Ghidra size overshoot / duplicate tail past `0x4cc` | Removed **60** trailing 1-byte placeholders (`1288` → **`1228`** B). Renamed and typed tail @ `+0x4b0..+0x4c8` (`pAudioBankArray`, `field_4b4`, `audioBankCount`, `audioBankCapacity`, `pReleaseOnDestroy`, `field_4c4`, `byte_4c8`, `pad_end`). `CBulanci_ctor` / dtor use tail fields by name. |
| `CDSObject` | `0x60` superset, +3 B skew vs `ConstructTrackManager` | Rebuilt struct (**96** B). Fields at proven offsets; `paused` @ `+0x35`; `imageField_50` / `imageField_5c` @ `+0x50` / `+0x5c`. `ConstructTrackManager` and `CDSObject_CtorWithImage` decompile cleanly. |

## Ghidra deltas

- **CBulanci:** `get_struct_layout` size **1228** (`0x4cc`); tail no longer overlaps ghost block near component index `[510]` / offset `0x4b0` region.
- **CDSObject:** `get_struct_layout` size **96** (`0x60`); was **95** B with misaligned track-manager fields.

## Remaining UNK (unchanged — doc only)

- **CBulanci:** full `CDSApp` interior below `+0x284`; embedded `CGame` blob (`pPad_48` / `byte[568]`); `field_280`, `dwNetSessionField0/1`, purpose of `field_4b4` / `field_4c4` / `byte_4c8`.
- **CDSObject:** exact MI layout inside `scheduler` (`+0x04..+0x1b`); `+0x48..+0x4f` on image instances; whether `0x48`-byte TM objects use same bytes as `0x60` image tail beyond `+0x44`.

## Next batch (unchanged)

Index 1 in `batches_50.json`: **`CDSApp`**, **`CGameView`**.
