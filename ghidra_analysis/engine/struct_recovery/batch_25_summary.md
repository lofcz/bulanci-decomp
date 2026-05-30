# Struct recovery batch 25/50

**Batch index:** 25 (`batches_50.json`)  
**Structs:** `CDSImageMouse`, `CDSJpegImage`  
**Program:** `bulanci.exe`

## Results

| Struct | Status | Size | Ghidra |
|--------|--------|------|--------|
| `CDSImageMouse` | PARTIAL | `0x88` | Applied (136 B) |
| `CDSJpegImage` | PARTIAL | `0x64` | Applied (100 B) |

## Evidence highlights

- **CDSImageMouse:** `PUSH 0x88` in class factory `0x0042af00` (classId `0x35`); vtables `0x486f7c` / `0x486f68`; `CDSImage_InitDefaults` on `+0x18`; `Draw`/`Erase` use backing image at `+0x1c` and dirty rect `+0x78..+0x84`. Base for `CGunMouse` (`0x218`).
- **CDSJpegImage:** `PUSH 0x64` in factory `0x00432060`; `CDSJpegImage_InitVtables` installs jpeg vtables on embedded `CDSImage` at `+0x04`; dtor `CDSImage_dtor(+4)` + `FUN_00434250(+0x58)`; Load/Save use `this−0x50` MI adjust (`bmp_decoder.md` / `CDSImage.md`).

## Deliverables

- `ghidra_analysis/engine/struct_recovery/CDSImageMouse.md`
- `ghidra_analysis/engine/struct_recovery/CDSJpegImage.md`

## Ghidra

- Replaced placeholder 1-byte structs (`delete_data_type` → `create_struct`).
- `save_program bulanci.exe` executed at batch end.

## Follow-ups

- Name factory `0x0042af00` (`CDSImageMouse` / class `0x35`) and `0x00432060` (`CDSJpegImage`) as Ghidra functions.
- Replace `embed_cdsObject` byte blob with typed `CDSObject` once the `0x60` Ghidra layout is applied consistently.
- Split `m_jpegQuality` from `CDSImage::m_format` if a dedicated field is found outside the stream-host `this` path.
