# Pass R4 — CDSImage struct map

## Task

| Field | Value |
|-------|-------|
| **scope** | Full `CDSImage` 96 B layout: raster header, `CDSPtrSlotVec` @ `+0x38`, write-only tags @ `+0x44`/`+0x48`, MI `streamHost` @ `+0x54`, `eventFacet` @ `+0x58`, consumer xrefs |
| **prior** | [round3_task_35_report.md](./round3_task_35_report.md), [round4_task_35_report.md](./round4_task_35_report.md) |
| **related** | [CDSObject.md](./CDSObject.md) (embed @ `CDSObject+4`, heap `0x60`) |

## Status

**DONE** — Ghidra struct 96 B verified; facet types applied; image ctor/allocate/stream MI `this` typing applied; consumers catalogued.

## Layout (Ghidra `get_struct_layout`)

| Offset | Size | Ghidra name | Role |
|--------|------|-------------|------|
| `0x00` | 4 | `pVf_primary` | Primary vtable |
| `0x04`–`0x34` | 52 | `nM_*` / `pM_*` | Width, height, format, stride, planes, copy box, palette count |
| `0x38` | 8 | `m_slotVector` | `CDSPtrSlotVec` — cleared in dtor via `CDSPtrSlotVec_Resize(&m_slotVector, 0)` |
| `0x40` | 4 | `nField_40` | Zeroed before slot resize (`CDSImage_dtor`) |
| `0x44` | 4 | `nDefaultBppTag` | Write-only; ctor `8`, InitDefaults `0`, JPEG `8` |
| `0x48` | 4 | `nDefaultFormatTag` | Write-only; InitDefaults `8` (not `m_copyHeight` @ `+0x30`) |
| `0x4c` | 4 | `pVf_IDSChained` | IDSChained MI vtable |
| `0x50` | 4 | `nRefcount` | InitDefaults `= 1`; BMP ctor / embed paths may patch adjacent MI |
| `0x54` | 4 | `streamHost` | `CDSImage_StreamHostFacet` — Load/Save / codec stream entry (`ECX` on MI thunks) |
| `0x58` | 4 | `eventFacet` | `CDSImage_EventFacet` — resource/event MI |
| `0x5c` | 4 | `pM_chain` | Chain head; JPEG wrapper aliases wrapper `+0x60` |

## Evidence highlights

### MI stream host @ `+0x54`

| Entry | Address | `this` on entry | Pixel plane |
|-------|---------|-----------------|-------------|
| `CDSImage_Load` | `0x00437160` | `CDSImage+0x54` | `(CDSImage *)((byte *)this - 0x54)` |
| `CDSImage_Save` | `0x00436c60` | same | same adjustor |
| `CDSBmpImage_LoadDibStream` | `0x004320c0` | BMP object `+0x58` (`m_image+0x54`) | `CDSImage__Allocate((CDSImage *)(this - 0x14), …)` in decompiler |
| `CDSJpegImage_Load` / `Save` | per `CDSJpegImage.md` | wrapper `+0x58` | `&m_image` via `this-0x50` family |

### Tags (not in generic image stream)

| Field | Writers | Raster readers |
|-------|---------|----------------|
| `nDefaultBppTag` @ `+0x44` | `CDSImage_ctor`, `InitDefaults`, `CDSJpegImage_InitVtables`, `CDSApp_ctor` embed | **None** after init (`CDSImage__Allocate`, `GetColorPlane`) |
| `nDefaultFormatTag` @ `+0x48` | `CDSImage_InitDefaults` (`MOV 8`) | **None** on standalone path |

Stream with `EDI = streamHost`: **`[EDI-0x44]` → `m_stride` @ `+0x10`**, not bpp tag @ `+0x44` hex (see round4 task 35 disasm table).

### Slot vector

`CDSImage_dtor@0x004254f0`: `nField_40 = 0`; `CDSPtrSlotVec_Resize(&m_slotVector, 0)`; then free `pM_auxBuffer` / `pM_pixels`.

## Consumers

| Symbol | Address | Xref callers |
|--------|---------|--------------|
| `CDSImage_ctor` | `0x00425460` | `CDSObject_CtorWithImage@0x0042566b`, `CDSBmpImage_ctor@0x0043237b` |
| `CDSImage_InitDefaults` | `0x00425580` | `CGunMouse_ctor` (×4), `CDSImageMouse_CreateObject`, `CDSFont_AllocFactory`, `CPoemScroller_Constructor`, `0x004372a0` |
| `CDSImage_dtor` | `0x004254f0` | `CDSObject_dtor`, `CDSJpegImage_dtor`, `CDSBackBuffer_dtor`, image unwind helpers |
| `CDSImage__Allocate` | `0x00436f40` | `CDSImage_ctor`, `CGunMouse_Draw` (×4), `CDSImageMouse_Draw`, `CDSBmpImage_LoadDibStream`, `CDSJpegImage::DecompressToImage`, `PickNextPoem` |
| `CDSPtrSlotVec_Resize` | `0x00406340` | `CDSImage_dtor@0x00425536` (+ many non-image owners) |

Embed hosts: `CDSObject` image variant (`0x60` alloc), `CDSFont` (`CDSImage_Load` on `+0x54`), `CDSImageMouse.savedBackground`, `CGunMouse` four embedded images, `CDSBackBuffer` 76 B tail.

## Ghidra deltas (pass r4)

| Action | Target | Result |
|--------|--------|--------|
| `set_function_this_type` | `0x00436f40` | `CDSImage::CDSImage__Allocate` — struct field writes in decompile |
| `set_function_this_type` | `0x00437160`, `0x00436c60` | `CDSImage_StreamHostFacet::CDSImage_Load` / `Save` |
| `set_function_this_type` | `0x004320c0`, `0x00432440` | `CDSImage_StreamHostFacet::CDSBmpImage_*DibStream` |
| `set_function_this_type` | `0x00431cf0` | `CDSJpegImage::CDSJpegImage_InitVtables` — nested `m_image.*` |
| `set_function_this_type` | `0x00435fe0` | `CDSImage::ComputeBufferSize` |
| (prior r4) | `0x004254f0`, `0x00406340` | `CDSImage_dtor`, `CDSPtrSlotVec_Resize` namespace fix |
| (prior r3/r2) | `0x00425460`, `0x00425580`, `0x00432330` | `CDSImage_ctor`, `InitDefaults`, `CDSBmpImage_ctor` |
| `set_decompiler_comment` | `0x00437160` | Stream-host / stride vs tag clarification |
| `force_decompile` | allocate, load, BMP load, JPEG init | Refreshed typed bodies |
| `save_program` | `bulanci.exe` | success (`save_all_programs`) |

## Struct doc updates

- [CDSImage.md](./CDSImage.md) — consumers table + pass r4 Ghidra note
- [round4_task_35_report.md](./round4_task_35_report.md) — unchanged (tag/stream proof); cross-linked from CDSImage.md

## Remaining UNK

- Scalar-deleting dtor thunk graph between `streamHost` / `eventFacet` bases (`bmp_decoder.md`, `CDSBmpImage.md`).
- `GetColorPlane` / `GetPaletteBuffer` / `CDSImage__FreeBuffers`: still `__fastcall` — convert to `__thiscall` + `CDSImage *` for field-aware decompile.
- Stream-host decompile uses facet pointer arithmetic (`this + -0x14`); use disasm `LEA`/`EDI-imm` for ground truth when labels disagree.
