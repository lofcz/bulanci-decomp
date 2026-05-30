# Round 3 — task 35 report

## Task

| Field | Value |
|-------|-------|
| **id** | 35 |
| **title** | CDSImage MI facets + nField_44/nField_48 naming |
| **one_liner** | `CDSImage` `+0x44`/`+0x48` renamed to write-only default tags; stream/event MI facets @ `+0x54`/`+0x58` verified on BMP/JPEG Load/Save paths. |
| **acceptance** | Update CDSImage.md (+ related); Ghidra mutation if evidence supports |

## Status

**DONE**

## Evidence

| Claim | Function @ address | Evidence |
|-------|-------------------|----------|
| `CDSImage` 96 B; tags @ `+0x44` / `+0x48` | `get_struct_layout CDSImage` | `nDefaultBppTag` @ 68 (`0x44`); `nDefaultFormatTag` @ 72 (`0x48`); `streamHost` (`CDSImage_StreamHostFacet`) @ 84; `eventFacet` (`CDSImage_EventFacet`) @ 88 |
| Ctor default bpp tag `8` | `CDSImage_ctor` @ `0x00425460` | `MOV dword ptr [ESI+0x44], 8` before `CDSImage__Allocate` |
| InitDefaults: bpp `0`, format tag `8` | `CDSImage_InitDefaults` @ `0x00425580` | `nDefaultBppTag=0`; `MOV [EAX+0x48], 8` — **not** `nM_copyHeight` @ `+0x30` |
| MI vtables patched | `CDSImage_InitDefaults` @ `0x00425580` | `pVf_primary` / `nM_width` dup, `pVf_IDSChained`, `streamHost.pVftable=g_pCDSImage_vftable_IDSChained`, `eventFacet.pVftable` |
| Load/Save serialize bpp tag only | `CDSImage_Load` @ `0x00437160`, `CDSImage_Save` @ `0x00436c60` | Stream `Read`/`Write` on `&this[-1].nDefaultBppTag` (4 bytes); `this` = stream-host @ `+0x54` |
| **No raster-path reader** | `CDSImage__Allocate` @ `0x00436f40`, `GetColorPlane` @ `0x004360f0`, `CGunMouse_Draw` @ `0x00424610` | `search_instructions` in those functions: **zero** `[reg+0x44]` object-field accesses (stack `0x44` only in draw) |
| BMP Load uses stream-host MI | `CDSBmpImage_LoadDibStream` @ `0x004320c0` | Plate: ECX = `CDSBmpImage+0x54`; decompile `CDSImage__Allocate(&this[-1].m_image.nM_stride, …)` → pixel plane @ wrapper `+0x04` |
| BMP Save same adjustor | `CDSBmpImage_SaveDibStream` @ `0x00432440` | Plate added (task 35 r3) |
| JPEG wrapper sets bpp tag `8` | `CDSJpegImage_InitVtables` @ `0x00431d00` | `(m_image).nDefaultBppTag = 8`; format/MI slots patched on embedded image (100 B object) |
| Stream-host vtable (BMP) | `vftable_methods.csv` @ `0x00487208` | Slots 4–5 → `CDSBmpImage_LoadDibStream` / `CDSBmpImage_SaveDibStream` |
| Stream-host vtable (JPEG) | `vftable_methods.csv` @ `0x0048719c` | Slots 4–5 → `CDSJpegImage_Load` / `CDSJpegImage_Save` |
| Event facet vtable (JPEG) | `vftable_methods.csv` @ `0x00487184` | Separate 6-slot table (`GetResourceName`, release thunks) @ image `+0x58` adjustor |
| Back-buffer embed truncates tail MI | `CDSImage_BackBufferEmbed` 76 B | Ends at `pDirectDrawSurface` @ `+0x48`; `nField_44` → **`nDefaultBppTag`** (Ghidra rename) |
| Embed ctor writes `8` @ bpp tag | `CDSApp_ctor` @ `0x0042b170` | `(backBuffer).embeddedImage.nDefaultBppTag = 8`; `+0x48` = `pDirectDrawSurface` (not format tag) |
| Slot vector resize callee | `CDSImage_dtor` @ `0x004254f0` | Calls `CDSPtrSlotVec_Resize(&m_slotVector,0)` — decompiler may still prefix `CDSAudioBank::` (display only) |

### Offset map (`+0x44` / `+0x48`)

| Context | `+0x44` | `+0x48` |
|---------|---------|---------|
| Standalone / heap `CDSImage` (96 B) | `nDefaultBppTag` — ctor `8`, InitDefaults `0`, Load/Save stream | `nDefaultFormatTag` — InitDefaults constant `8` |
| `CDSImage_BackBufferEmbed` (76 B) | `nDefaultBppTag` — `CDSApp_ctor` writes `8` | `pDirectDrawSurface` (COM; flip/free) |
| Raster draw / allocate | **not read** after init | **not read** on standalone path |

## Ghidra deltas

- Confirmed `CDSImage`: `nDefaultBppTag`, `nDefaultFormatTag`, `streamHost`, `eventFacet` (facet structs 4 B / `pVftable` only)
- `modify_struct_field CDSImage_BackBufferEmbed` `nField_44` → `nDefaultBppTag`
- `set_function_prototype` `CDSPtrSlotVec_Resize@0x00406340` → `void __thiscall CDSPtrSlotVec_Resize(CDSPtrSlotVec *, int)`
- `set_plate_comment` `CDSBmpImage_SaveDibStream@0x00432440`
- Pre-existing plates/comments on `CDSImage_ctor`, `CDSImage_InitDefaults`, `CDSImage_Load`, `CDSBmpImage_LoadDibStream`, `CDSJpegImage_InitVtables`, `CDSApp_ctor` embed write
- `save_program bulanci.exe`

## Struct doc updates

- [CDSImage.md](./CDSImage.md) — layout names and r3 Ghidra apply note (already aligned; UNK trimmed for tag semantics)
- [CDSBackBuffer.md](./CDSBackBuffer.md) — embed `nDefaultBppTag`; cross-link this report

## Remaining UNK

- Exact meaning of default constant `8` for bpp/format tags beyond serialize (likely engine default 8 bpp / format id)
- Full scalar-deleting dtor thunk graph between `streamHost` / `eventFacet` bases (`bmp_decoder.md` / `CDSBmpImage.md`)
- `CDSImage_dtor` callee namespace display `CDSAudioBank::CDSPtrSlotVec_Resize` (prototype correct; Ghidra namespace quirk)
