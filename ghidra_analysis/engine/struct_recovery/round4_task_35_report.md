# Round 4 — Task 35 Report

## Task

| Field | Value |
|-------|-------|
| **id** | 35 |
| **round** | 4 |
| **title** | CDSImage bpp tag semantics and dtor thunk namespace fix |
| **one_liner** | Close R3 blockers: fix `CDSPtrSlotVec_Resize` / `CDSImage_dtor` decompiler namespace; prove `nDefaultBppTag`/`nDefaultFormatTag` write-only semantics and correct Load/Save stream field mapping. |
| **prior** | [round3_task_35_report.md](./round3_task_35_report.md) |
| **structs** | CDSImage, CDSPtrSlotVec, CDSBackBuffer, CDSBmpImage, CDSJpegImage |

## Status

**DONE**

## Evidence

### Tag fields: writers only, no raster readers

| Field | Offset | Writers | Readers (image raster path) |
|-------|--------|---------|----------------------------|
| `nDefaultBppTag` | `+0x44` | `CDSImage_ctor` `MOV [this+0x44],8`; `CDSImage_InitDefaults` `=0`; `CDSJpegImage_InitVtables` `=8`; `CDSApp_ctor` embed `=8` | **None** in `CDSImage__Allocate`, `GetColorPlane`, `CDSImage_dtor` (`search_instructions` +0x44]) |
| `nDefaultFormatTag` | `+0x48` | `CDSImage_InitDefaults` `MOV [this+0x48],8`; JPEG init patches MI at adjacent slots | **None** in `CDSImage__Allocate`, `GetColorPlane` |

Constant **`8`**: engine default tag pair — **8 bpp** default on heap ctor / JPEG wrapper (`nDefaultBppTag`), **format tag 8** on `InitDefaults` (`nDefaultFormatTag`). Not consumed after init on standalone raster path; back-buffer **76 B** embed repurposes `+0x48` as `pDirectDrawSurface` (see `CDSBackBuffer.md`).

### Load/Save stream: `nDefaultBppTag` is **not** serialized (R3 correction)

With stream-host `this` @ `CDSImage+0x54` (`EDI`):

| Disasm | Effective object offset | Field |
|--------|-------------------------|-------|
| `LEA ECX,[EDI-0x4c]` + Read 8 | `+0x08` | height band |
| `LEA ECX,[EDI-0x44]` + Read 4 | `+0x10` | **`m_stride`** |
| `LEA ECX,[EDI-0x40]` + Read 4 | `+0x14` | `m_paletteMarker` |
| `LEA ECX,[EDI-0x3c]` + Read 4 | `+0x18` | `m_fillByte` area |
| `LEA ECX,[EDI-0x38]` + Read 1 | `+0x1c` | palette/aux flag byte |
| `LEA ECX,[EDI-0x1c]` + Read 4 | `+0x38` | `m_slotVector` head |

`nDefaultBppTag` lives at **`+0x44` hex**; the operand **`[EDI-0x44]`** subtracts **0x44 hex (68 dec)** from a host at **+0x54**, landing at **`+0x10` (`m_stride`)**, not the tag slot. Decompiler may still label `&this[-1].nDefaultBppTag` — misleading.

### Dtor namespace fix (R3 blocker closed)

| Before | After |
|--------|-------|
| `CDSAudioBank::CDSPtrSlotVec_Resize((CDSAudioBank *)&this->m_slotVector,0)` | `CDSPtrSlotVec::CDSPtrSlotVec_Resize(&this->m_slotVector,0)` |

`set_function_this_type` on `CDSPtrSlotVec_Resize@0x00406340` → `CDSPtrSlotVec *` and on `CDSImage_dtor@0x004254f0` → `CDSImage *`; `force_decompile` both.

## Ghidra deltas (R4)

| Action | Target | Result |
|--------|--------|--------|
| `set_function_this_type` | `0x00406340` | `CDSPtrSlotVec::CDSPtrSlotVec_Resize` |
| `set_function_prototype` | `0x00406340` | `void __thiscall CDSPtrSlotVec_Resize(CDSPtrSlotVec *, int)` |
| `set_function_this_type` | `0x004254f0` | `CDSImage::CDSImage_dtor` |
| `set_decompiler_comment` | `0x004254af`, `0x004255a5`, `0x0043718b`, `0x00436c8c` | Tag semantics + stride stream correction |
| `force_decompile` | `0x004254f0`, `0x00406340` | Typed callee / fields |
| `save_program` | `bulanci.exe` | success |

## Struct doc updates

- [CDSImage.md](./CDSImage.md) — R4 tag semantics + Load/Save stream map correction
- [CDSPtrSlotVec.md](./CDSPtrSlotVec.md) — R4 namespace fix note
- [pass_r4_CDSImage_report.md](./pass_r4_CDSImage_report.md) — consolidated 96 B map, MI facets, consumers, extended `set_function_this_type` pass

## Remaining UNK

- Full scalar-deleting dtor thunk graph for `streamHost` / `eventFacet` MI bases (still in `bmp_decoder.md` / `CDSBmpImage.md`).
- Whether persisted image streams ever carry bpp/format tags outside `CDSImage_Load`/`Save` (BMP/JPEG codecs use separate DIB/JPEG paths).
