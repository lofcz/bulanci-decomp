# CDSImage

## Status

**PARTIAL** — instance size **VERIFIED** at `0x60` (96 bytes). Raster header, **`m_slotVector` + `m_slotCount` @ `+0x38..+0x40`** (subscriber list), and MI vtable **slot maps** proven (R5 w31). Scalar-deleting dtor **call graph** between MI bases still open (R5 task 35).

## Size proof table

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDSImage) == 0x60` | `CGunMouse_ctor` @ `0x00426060` | Four `CDSImage_InitDefaults` at dword indices `+4`, `+0x1c`, `+0x34`, `+0x4c` → spacing `0x18` dwords = `0x60` bytes |
| Last dword field before tail vtables | `CDSBmpImage_ctor` @ `0x00432330` | `*(this + 0x5c) = 0` with `CDSImage` base at `this+4` → image `+0x58`; `+0x5c` is chain head (4 bytes) → ends at `0x60` |
| Embedded in `CGunMouse` | `CGunMouse.md` / `CGunMouse_Draw` @ `0x00424610` | Images at `+0x10`, `+0x70`, `+0xd0`, `+0x130` each `0x60` wide |

## Layout table

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `vf_primary` | `CDSImage_InitDefaults@0x00425580` (`*param_1`); `CDSBmpImage_ctor@0x00432330` patch `0x48724c` |
| `0x04` | 4 | `int` | `m_width` | `CDSImage__Allocate@0x00436f40`; `CDSBmpImage_ValidateStride@0x00436020` (`*(this+4)`) |
| `0x08` | 4 | `int` | `m_height` | `CDSImage__Allocate@0x00436f40`; `ComputeBufferSize@0x00435fe0` (`*(this+8)`) |
| `0x0c` | 4 | `int` | `m_format` | `CDSImage__Allocate@0x00436f40`; `ComputeBufferSize@0x00435fe0` (`*(this+0xc)`) |
| `0x10` | 4 | `int` | `m_stride` | `CDSImage__Allocate@0x00436f40`; `CDSBmpImage_ValidateStride@0x00436020` |
| `0x14` | 4 | `int` | `m_paletteMarker` | `CDSImage__Allocate@0x00436f40` (`= 0xffffffff`) |
| `0x18` | 1 | `byte` | `m_fillByte` | `CDSImage__Allocate@0x00436f40` (`= 0xff`) |
| `0x19` | 3 | — | `pad_19` | — |
| `0x1c` | 4 | `void *` | `m_pixels` | `CDSImage__Allocate@0x00436f40`; `GetColorPlane@0x004360f0`; `CDSImage__FreeBuffers@0x00436d50` |
| `0x20` | 4 | `void *` | `m_auxBuffer` | `CDSImage__FreeBuffers@0x00436d50`; `CDSImage_Load@0x00437160` (indexed copy plane) |
| `0x24` | 4 | `int` | `field_24` | `CDSImage__Allocate@0x00436f40` (zeroed) |
| `0x28` | 4 | `int` | `field_28` | `CDSImage__Allocate@0x00436f40` (zeroed) |
| `0x2c` | 4 | `int` | `m_copyWidth` | `CDSImage__Allocate@0x00436f40` (`= m_width`); `CDSImage_Load@0x00437160` |
| `0x30` | 4 | `int` | `m_copyHeight` | `CDSImage__Allocate@0x00436f40` (`= m_height`); `CDSImage_Load@0x00437160` |
| `0x34` | 4 | `int` | `m_paletteEntries` | `CDSImage__Allocate@0x00436f40`; `GetColorPlane@0x004360f0`; `ComputeBufferSize@0x00435fe0` |
| `0x38` | 8 | `CDSPtrSlotVec` | `m_slotVector` | `CDSImage_dtor@0x004254f0` (`CDSPtrSlotVec_Resize(&this->m_slotVector,0)`); `CIntListInsertSortedOrAppend(&m_slotVector,…)` uses count @ `+0x40` |
| `0x40` | 4 | `int` | `m_slotCount` | **Active subscriber count** for `m_slotVector.pSlots` (CIntList layout: count @ `this+8` when `this=&m_slotVector`). Writers: ctor/InitDefaults/dtor `=0`; readers: `BroadcastFrameTimeHint@0x00436ef0`, `ODSImage__SetImage@0x00439100`, `CDynPtrArray_RemoveRange` via `&m_slotVector` |
| `0x44` | 4 | `int` | `nDefaultBppTag` | `CDSImage_ctor@0x00425460` (`MOV [this+0x44],8`); `CDSImage_InitDefaults` (`=0`); `CDSJpegImage_InitVtables`; `CDSApp_ctor` embed `=8` — **write-only**; **not** in `CDSImage_Load`/`Save` stream (R4: `[EDI-0x44]` from host `+0x54` is **`m_stride` @ +0x10**) |
| `0x48` | 4 | `int` | `nDefaultFormatTag` | `CDSImage_InitDefaults@0x00425580` (`MOV [this+0x48],8`); default format tag constant — **not** `nM_copyHeight` @ `+0x30`; **write-only** on standalone path (back-buffer embed repurposes slot as `pDirectDrawSurface`) |
| `0x4c` | 4 | `void *` | `vf_IDSChained` | `CDSImage_InitDefaults@0x00425580`; `CDSImage_ReleaseRefcount@0x004322f0` |
| `0x50` | 4 | `int` | `refcount` | `CDSImage_InitDefaults@0x00425580` (`= 1`); `CDSBmpImage_ctor@0x00432330`; `CDSImage_ReleaseRefcount@0x004322f0` |
| `0x54` | 4 | `CDSImage_StreamHostFacet` | `streamHost` | `CDSImage_InitDefaults@0x00425580`; MI entry for `CDSImage_Load`/`Save` / `CDSBmpImage_LoadDibStream` (`ECX=this+0x54`) |
| `0x58` | 4 | `CDSImage_EventFacet` | `eventFacet` | `CDSImage_InitDefaults@0x00425580`; `CDSBmpImage_ctor@0x00432330` |
| `0x5c` | 4 | `void *` | `m_chain` | `CDSImage_InitDefaults@0x00425580` (`= 0`); `CDSBmpImage_ctor@0x00432330` |

## Ghidra apply

**Applied (batch 31):** `CDSImage` 96 B; `m_slotVector` typed `CDSPtrSlotVec` (see `CDSPtrSlotVec.md`). Prototypes: `CDSImage_dtor`, `CDSImage_InitDefaults`, `CDSImage__Allocate` (renamed from `CDSImage::Allocate` @ `0x00436f40`). `CDSImage_InitDefaults` decompiles with struct fields; `CDSImage__Allocate` still shows `void *this` (`__thiscall` API limit).

```
create_struct CDSImage fields=[
  {"name":"vf_primary","type":"void *","offset":0},
  {"name":"m_width","type":"int","offset":4},
  {"name":"m_height","type":"int","offset":8},
  {"name":"m_format","type":"int","offset":12},
  {"name":"m_stride","type":"int","offset":16},
  {"name":"m_paletteMarker","type":"int","offset":20},
  {"name":"m_fillByte","type":"byte","offset":24},
  {"name":"pad_19","type":"byte[3]","offset":25},
  {"name":"m_pixels","type":"void *","offset":28},
  {"name":"m_auxBuffer","type":"void *","offset":32},
  {"name":"field_24","type":"int","offset":36},
  {"name":"field_28","type":"int","offset":40},
  {"name":"m_copyWidth","type":"int","offset":44},
  {"name":"m_copyHeight","type":"int","offset":48},
  {"name":"m_paletteEntries","type":"int","offset":52},
  {"name":"m_slotVector","type":"CDSPtrSlotVec","offset":56},
  {"name":"m_slotCount","type":"int","offset":64},
  {"name":"nDefaultBppTag","type":"int","offset":68},
  {"name":"nDefaultFormatTag","type":"int","offset":72},
  {"name":"vf_IDSChained","type":"void *","offset":76},
  {"name":"refcount","type":"int","offset":80},
  {"name":"vf_streamHost","type":"void *","offset":84},
  {"name":"vf_event","type":"void *","offset":88},
  {"name":"m_chain","type":"void *","offset":92}
]
get_struct_layout CDSImage → Size: 96 (verified batch 31)
```

## Embedded in `CDSObject` (`0x60` heap, base `CDSObject+4`)

| Topic | Evidence |
|-------|----------|
| Ctor entry | `CDSObject_CtorWithImage@0x0042563a` → `CDSImage_ctor(&this->scheduler, …)` |
| Outer vs inner vtable | Same function: `this->pVftable = CDSImage::vftable`; `scheduler` dword `0` → image primary vtable |
| MI patches (absolute on `CDSObject`) | `+0x4c` `*(pPad_48+4)`; `+0x54` / `+0x58` via `pPad_54`; `+0x50` / `+0x5c` dword init |
| Span vs standalone `0x60` image | Embed ends at host `+0x5c`; `m_chain` at `CDSImage+0x5c` maps past end of `0x60` host — tail dword is `image_tail_5c` on host (`CDSObject.md`) |
| Field alias at host `+0x34` | `CDSImage+0x30` (`m_copyHeight`) → `CDSObject+0x34`; conflicts with TM `bPlayFlags` on same host layout |

## Embedding: `CDSFont`

`CDSFont` (`0x568`) reuses this layout at **`+0x08..+0x4b`** (68 bytes). Offsets and names match `CDSImage` (`m_height` … `field_48`). Differences:

- `+0x04` is a font MI vtable (`vf_sub04`), not `m_width`.
- `CDSImage_Load` / `CDSImage_InitDefaults` run on the font object; stream `Read` uses `this` at `+0x54` (`vf_streamHost`).
- After load, ClassID-54 `.font` data writes `0x500` bytes from `+0x0c`, overlaying `m_format` through the glyph band (see `CDSFont.md`).

## MI Load / Save (`this` at stream-host `+0x54`)

| Entry | Address | `ECX` on entry | Pixel plane | Evidence |
|-------|---------|----------------|-------------|----------|
| `CDSBmpImage_LoadDibStream` | `0x004320c0` | object `+0x54` | `LEA ECX,[EDI-0x50]` → `CDSImage` @ object `+0x04` | disasm @ `0x0043214c` |
| `CDSBmpImage_SaveDibStream` | `0x00432440` | object `+0x54` | `&this[-1].m_image.*` in decompiler | same adjustor |
| `CDSJpegImage_Load` | `0x00431cc0` | object `+0x54` | `&this[-1].m_image.nM_stride` or `NULL` if `this==0x54` | decompile |
| `CDSJpegImage_Save` | `0x00432030` | IDSChained `+0x54` | `CompressFromImage(dst, &this[-1].m_image, quality@+0x60)` | disasm `[ECX+0xc]` |
| `CDSImage_Load` / `Save` | `0x00437160` / `0x00436c60` | stream-host facet | `&this[-1].m_image.*` (font @ `CDSFont+0x54`) | `CDSFont.md` |

**Ghidra (agent todo 35 r3, 2026-05-30):** Renamed `nField_44`→`nDefaultBppTag`, `nField_48`→`nDefaultFormatTag`. Plate comments on Load/Save/InitDefaults; vtable EOL @ `0x00487208` (BMP stream-host), `0x0048719c`/`0x00487184` (JPEG). `save_program` ✓.

**Ghidra (agent todo 35 r4, 2026-05-30):** `set_function_this_type` `CDSPtrSlotVec_Resize@0x00406340` + `CDSImage_dtor@0x004254f0` — dtor decompile shows `CDSPtrSlotVec::CDSPtrSlotVec_Resize` (R3 `CDSAudioBank::` prefix cleared). Disasm proof: `CDSImage_Load`/`Save` `[EDI-0x44]` → **`m_stride` @ +0x10**, not bpp tag; tags are write-only defaults (`8` = 8bpp / format tag). See [round4_task_35_report.md](./round4_task_35_report.md).

**Ghidra (pass r4 CDSImage, 2026-05-30):** `set_function_this_type` on `CDSImage__Allocate@0x00436f40`, stream-host MI `CDSImage_Load`/`Save`/`CDSBmpImage_LoadDibStream`/`SaveDibStream` → `CDSImage_StreamHostFacet *`, `CDSJpegImage_InitVtables@0x00431cf0` → `CDSJpegImage *`, `ComputeBufferSize@0x00435fe0` → `CDSImage *`. `CDSImage_ctor` / `CDSImage_InitDefaults` / `CDSBmpImage_ctor` already typed. Full consumer xref table: [pass_r4_CDSImage_report.md](./pass_r4_CDSImage_report.md). `save_program bulanci.exe` ✓.

**Ghidra (agent todo 35 r2):** `CDSImage__Allocate` / `CDSPtrSlotVec_Resize` prototypes; facet types `CDSImage_StreamHostFacet` / `CDSImage_EventFacet` @ `+0x54`/`+0x58`. Xref proof: writers = ctor, InitDefaults, JPEG/BMP init, embed ctor `8`; **zero** post-init readers on raster path.

**Ghidra (R5 worker 31, 2026-05-30):** `nField_40` → `m_slotCount`; `GetPaletteBuffer` / `GetColorPlane` / `CDSImage__FreeBuffers` → `__thiscall` + `CDSImage *`. Comments @ `BroadcastFrameTimeHint@0x00436ef0`, `ODSImage__SetImage@0x00439100`, `CDSImage_dtor`. See [round5_worker_31_report.md](./round5_worker_31_report.md).

## Subscriber slot list (`+0x38` / `+0x40`)

`m_slotVector` (`CDSPtrSlotVec`: `pSlots` + `cCapacity`) backs a **`void **` subscriber table. **`m_slotCount`** is the live element count; `CIntListInsertSortedOrAppend` / `CDynPtrArray_RemoveRange` / `CIntList_BinarySearch` take `&m_slotVector` and use count at **`this+8`** (= image `+0x40`).

| API | Address | Role |
|-----|---------|------|
| `ODSImage__SetImage` | `0x00439100` | Register/unregister `ODSImage+4` (`pVf_odsimage`) on drawable `m_slotVector` |
| `BroadcastFrameTimeHint` | `0x00436ef0` | FLX opcode `0x0C`: foreach subscriber, `CALL vfn[+0x10](image, u16)` |
| `CDSImage_dtor` | `0x004254f0` | `m_slotCount = 0`; `CDSPtrSlotVec_Resize(&m_slotVector, 0)` |

**Embed exception:** `CDSBackBuffer.embeddedImage` @ `+0x40` holds **`CDSAudioPlayer *`** on menu paths — not `m_slotCount` ([CDSBackBuffer.md](./CDSBackBuffer.md)).

## MI facet vtables (standalone `CDSImage`)

From `master_vtable_catalog.csv` + `CDSImage_InitDefaults@0x00425580`. Wrappers patch BMP/JPEG addresses at same offsets ([CDSBmpImage.md](./CDSBmpImage.md)).

| Offset | Vtable | Slots | Notable |
|--------|--------|-------|---------|
| `+0x4c` | `0x00483728` | 5 IDSChained | [3] `0x00437530` ~dtor thunk |
| `+0x54` streamHost | `0x00483740` | 6 IDSChained | [3] `0x00437520` ~dtor; [4–5] `CDSImage_Load` / `Save` |
| `+0x58` eventFacet | `0x0048375c` | 4 IDSEventHandler | [3] `0x00437540` ~dtor |

`CDSBmpImage` stream-host @ `0x00487208` — slots [4–5] = DIB Load/Save ([bmp_decoder.md](../../formats/bmp_decoder.md)).

## Consumers (xref summary)

| API | Address | Callers (representative) |
|-----|---------|--------------------------|
| `CDSImage_ctor` | `0x00425460` | `CDSObject_CtorWithImage`, `CDSBmpImage_ctor` |
| `CDSImage_InitDefaults` | `0x00425580` | `CGunMouse_ctor` (×4), `CDSImageMouse_CreateObject`, `CDSFont_AllocFactory`, `CPoemScroller_Constructor` |
| `CDSImage_dtor` | `0x004254f0` | `CDSObject_dtor`, `CDSJpegImage_dtor`, `CDSBackBuffer_dtor`, image-object unwind |
| `CDSImage__Allocate` | `0x00436f40` | `CDSImage_ctor`, `CGunMouse_Draw`, `CDSImageMouse_Draw`, `CDSBmpImage_LoadDibStream`, `CDSJpegImage::DecompressToImage`, `PickNextPoem` |
| `CDSPtrSlotVec_Resize` | `0x00406340` | `CDSImage_dtor` (`&m_slotVector, 0`); also `CGame`, `CDSUpdatedItem`, unrelated types |
| `GetColorPlane` / `GetPaletteBuffer` | `0x004360f0` / `0x004360d0` | BMP/JPEG load, `CBulAnim::SetPalette`, embed palette helpers (`CDSObject+4`) |
| Stream-host Load/Save | `+0x54` vtable | `CDSImage_Load`/`Save`, `CDSBmpImage_*DibStream`, `CDSJpegImage_Load`/`Save`, `CDSFont` stream facet |

## UNK

- Full scalar-deleting dtor thunk **call graph** between `+0x4c` / `+0x54` / `+0x58` bases (slot indices catalogued above; adjustor targets in [bmp_decoder.md](../../formats/bmp_decoder.md) — R5 task 35).
- Whether `m_slotVector` is ever grown with `CDSPtrSlotVec_Resize` to `>0` on standalone heap images (only **dtor** xref to `Resize` on image path; subscribers use `CIntList_EnsureCapacity` via insert helper).
- Embed `+0x50` refcount vs `vf_IDSChained` slot overlap with `CDSObject_CtorWithImage` write of `1` (`CDSObject.md`, agent todo 1 r3).
