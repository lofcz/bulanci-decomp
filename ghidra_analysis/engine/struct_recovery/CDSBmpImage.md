# CDSBmpImage

## Status

**PARTIAL** — heap **`sizeof == 0x60`** (96 bytes) verified; five MI vtable slots and pixel header through `+0x5c` are ctor-proven. **`IDSImage::Load` / `Save`** use the stream-host face at **`+0x54`** (`ECX` on entry); pixel buffer is reached via **`this - 0x50`** (documented in Ghidra plate on `CDSBmpImage_LoadDibStream`).

## Size proof

| Claim | Address | Evidence |
|-------|---------|----------|
| `sizeof(CDSBmpImage) == 0x60` | `CPoemScroller::PickNextPoem` @ `0x00425df0` | `OperatorNewWithBadAlloc(0x60)` → `CDSObject_CtorWithImage`; surface stored as `returnPtr + 4` (`IDSImage` face) |
| Same `0x60` image object | `CDSFlxFile::CreateBoundClone` @ `0x00432c0e` | `OperatorNewWithBadAlloc(0x60)` → `CDSObject_CtorWithImage` (`CDSObject.md`) |
| Dtor from object base | `CDSBmpImage_scalar_deleting_dtor` @ `0x00432700` | `CDSObject__CDSObject_dtor_withImage(this)` — first field / primary vtable |
| Stack temporaries in `Save` | `CDSBmpImage_SaveDibStream` @ `0x00432440` | `CDSBmpImage_ctor` on stack buffers `local_c4` / `local_124` (stride / 32→24 downconvert) |
| No 100-byte factory | — | Unlike `CDSJpegImage` (`OperatorNew(0x64)` @ `0x00432070`), BMP raster uses **`0x60`** + `CDSObject_CtorWithImage` or explicit `CDSBmpImage_ctor` |

## Layout (absolute offsets, single inheritance + MI vtables)

| Offset | Size | Type | Name | Evidence (func@addr) |
|--------|------|------|------|----------------------|
| `0x00` | 4 | `void *` | `vf_primary` | `CDSBmpImage_ctor@0x00432330` `= 0x0048724c` |
| `0x04` | 4 | `void *` | `vf_IDSChained_face` | `ctor` `= 0x00487238`; `CDSImage_ctor` entry at `this+4` |
| `0x08` | 4 | `int` | `m_width` | `CDSBmpImage_ValidateStride@0x00436020` (strict path); `CDSImage__Allocate` via `Load` (`this-0x50`) |
| `0x0c` | 4 | `int` | `m_height` | same |
| `0x10` | 4 | `int` | `m_format` | `ValidateStride` DWORD-align test uses `m_format & 3`; strict path uses format index table `DAT_004b0050` |
| `0x14` | 4 | `int` | `m_stride` | `CDSImage__Allocate`; `Save` stride guard |
| `0x18` | 4 | `int` | `m_paletteMarker` | `CDSImage__Allocate` |
| `0x19` | 3 | — | `pad_19` | — |
| `0x1c` | 4 | `void *` | `m_pixels` | `Load` 16 bpp expand when `m_format == 4` (format index, not bpp) |
| `0x20` | 4 | `void *` | `m_auxBuffer` | row walk in `Load` |
| `0x24`–`0x34` | — | `int` / slots | `field_24` … `m_paletteEntries` | `CDSImage_ctor` zero-init (`CDSImage.md`) |
| `0x38` | 8 | `CDSPtrSlotVec` | `m_slotVector` | `CDSImage_dtor` slot teardown |
| `0x40` | 4 | `int` | `field_40` | `CDSImage_dtor` |
| `0x44` | 4 | `int` | `field_44` | `CDSImage_ctor` (`= 8`) |
| `0x4c` | 4 | `void *` | `vf_chainMid` | `ctor` `= 0x00487224` |
| `0x50` | 4 | `int` | `refcount` | `ctor` `= 1` |
| `0x54` | 4 | `void *` | `vf_streamHost` | `ctor` `= 0x00487208`; **`Load` / `Save` `this` pointer** (`bmp_decoder.md`) |
| `0x58` | 4 | `void *` | `vf_eventTail` | `ctor` `= 0x004871f0`; cleared `= 0` before patch |
| `0x5c` | 4 | `void *` | `m_chain` | `ctor` `CDSImage` tail `= 0` |

## MI / interface notes

| Topic | Evidence |
|-------|----------|
| `Load` / `Save` entry at `+0x54` | `CDSBmpImage_LoadDibStream@0x004320c0` — plate + decompiler `this[-1]` / `&this[-1].m_image…` = pixel base at `this-0x50` |
| Scalar deleting dtors | thunks `this-0x4`, `-0x4c`, `-0x54`, `-0x58` @ `0x004322a0`..`0x004322c0` |
| RTTI | `.?AVCDSBmpImage@@` @ `0x004afde0` (`bmp_decoder.md`) |

## Ghidra apply

**Slice 25:** flat 96 B layout (pixel fields from `+0x08`) — superseded for Load/Save decompile.

**Agent todo 35 (2026-05-30):**

```
delete_data_type CDSBmpImage
create_struct CDSBmpImage  # pVf_primary @0 + CDSImage m_image @+4 → 100 B
get_struct_layout CDSBmpImage → Size: 100
set_function_prototype CDSBmpImage_LoadDibStream(CDSBmpImage *this, CDSFilterStream *stream)
set_function_prototype CDSBmpImage_SaveDibStream(CDSBmpImage *this, void *stream)
```

Decompiler on Load/Save (`0x004320c0`, `0x00432440`) uses **`this[-1].m_image.*`** when `this` is the stream-host facet (`ECX` at object `+0x54`; asm `EDI-0x50` → embedded image). Plate comment on Load documents the adjustor. **`OperatorNew(0x60)`** heap size unchanged.

**Agent todo 26 round-2 (2026-05-30):** Prototypes on `CDSBmpImage_LoadDibStream` / `CDSBmpImage_SaveDibStream` / `CDSBmpImage_ctor`; decompiler PRE_COMMENT on Save documents `+0x54` stream-host / `this-0x50` pixel base; `save_program bulanci.exe`.

**Agent todo 26 r3 (2026-05-30):** `CDSBmpImage.pVf_primary` → `pVf_IDSReferenced`; plate on `CDSBmpImage_LoadDibStream@0x004320c0` (stream-host `+0x54` / `m_image` adjustor); prototypes refreshed on Load/Save/ctor; `save_program bulanci.exe`.

**Agent todo 26 r4 (2026-05-30):** `set_function_this_type` **`CDSImage *`** on `CDSBmpImage_ValidateStride@0x00436020` and `CDSBmpImage_FillBitmapInfoHeader@0x00436060` (Save passes `streamHost-0x50` @ `0x00432475`–`0x0043247d`); decompile now uses `nM_width` / `nM_format` / `nM_stride` / `nM_paletteEntries` on strict/loose stride checks; `set_decompiler_comment` on ValidateStride; `save_program bulanci.exe`.

## UNK

- Whether any path registers a dedicated `CDSBmpImage` class factory (text surfaces use `CDSObject_CtorWithImage` @ `0x60`, not `CDSBmpImage_ctor` directly).
- Full overlap map between this flat layout and standalone `CDSImage` for copy-blits (`CDSImage_Load@0x00437160`).
