# Struct recovery — batch 43 follow-up (round 2)

**Prior:** `batch_43_summary.md`  
**Structs:** `ODSImage`, `type_info`  
**Program:** `bulanci.exe`  
**Status:** **FOLLOWUP_DONE**

## Inputs read

- `batch_43_summary.md`
- `ODSImage.md`, `type_info.md`
- `CDSImage.md` (batch 31 — `sizeof == 0x60`, Ghidra struct 96 B)

## Actions

### type_info

- Batch 43 had **SKIP** with no follow-up items — **no Ghidra or doc changes**.

### ODSImage — `pDrawable` typing

1. Decompiled `ODSImage__SetImage@0x00439100`, `SetOwner@0x00439050`.
2. Proven consumers match `CDSImage` layout from batch 31:
   - Width/height: `param_1[1]`/`[2]` vs `*(drawable+4)`/`+8` → `CDSImage::nM_width` / `nM_height`.
   - Slot list: `drawable + 0x38` binary search / remove → `CDSImage::m_slotVector` region.
3. `modify_struct_field` on `ODSImage`: `pDrawable` → `CDSImage *`.
4. Post-change decompile: `SetOwner` uses `pCVar1->nM_width` / `nM_height`.

### ODSImage — `FUN_004228f0` vs `CDSBitmap_ctor`

1. Compared `CDSBitmap_ShellCtor@0x004228f0` (was `ODSImage::FUN_004228f0` / `CDSBitmap_SubobjectCtor`) with `CDSBitmap_ctor@0x004393d0`.
2. **Not mergeable** as one symbol: shell ctor skips `ODSImage_ctor` and initial image `param_3`; full ctor calls `ODSImage_ctor(this+0x68, param_3)`.
3. Renamed / confirmed Ghidra name **`CDSBitmap_ShellCtor`**; plate comment documents callers (`CSwitch`, `CMovieView`, `CDSAnim`, `CSpells`) and distinction from `ODSImage_ctor@0x00418c00`.
4. Updated `ODSImage.md` naming table + UNK cleanup.

### Ghidra deltas

- `modify_struct_field` `ODSImage.pDrawable` → `CDSImage *`
- `rename_function` `0x004228f0` → `CDSBitmap_ShellCtor` (already partially named `CDSBitmap_SubobjectCtor`)
- `set_decompiler_comment` @ `0x004228f0`
- `get_struct_layout ODSImage` → 16 B, `pDrawable` typed `CDSImage *`
- `save_program bulanci.exe` (once)

## Remaining UNK

- `ODSImage::pOwner` still generic `pointer` (owner is `CBulanci*` in `SetOwner` decompile — typing deferred).
- `vf_primary` / `vf_odsimage` necessity per embedder.
- `FUN_0042ea80` AddRef in `ODSImage__SetImage` — resource interface not fully named.
- Full-object ctors (`CGunMouse`, `CWeapon`, etc.) — out of `0x10` mixin scope.

## Struct status after follow-up

| Struct | Status | Size | Notes |
|--------|--------|------|-------|
| `ODSImage` | VERIFIED | `0x10` | `pDrawable` → `CDSImage *`; shell ctor named |
| `type_info` | SKIP | 1 (CRT) | unchanged |
