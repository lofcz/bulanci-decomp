# `CDSBmpImage` — the BMP loader / saver

**TL;DR.** `CDSBmpImage` is **bespoke code**, not a port of any third-party
BMP library (no libbmp, no FreeImage, no DevIL, no stb_image). The whole
class is ~1.5 KB of code spread over 12 functions, but only **two** of
them are non-trivial — `Load` (`FUN_004320c0`, ~70 lines) and `Save`
(`FUN_00432440`, ~110 lines). The rest are 1–6-line wrappers (vtable
thunks, ctor, scalar-deleting destructor and its four base-class
adjusters, two BMP-specific helpers used only by `Save`). There is no
library "in the large" to attribute, so the match track still has to
byte-match these routines directly; the good news is that the surface is
so small that "by hand" means two functions of ~70 and ~110 lines plus a
fistful of thunks.

The on-disk format the loader accepts is the standard Microsoft DIB v3:

* 14-byte `BITMAPFILEHEADER` (magic `0x4D42` = `'BM'`).
* 40-byte `BITMAPINFOHEADER` (`biSize` field is **not** checked — only
  the size of the read is fixed at 40, so any V3 header is fine, and a
  V4/V5 header will be silently truncated and the trailing bytes will
  shift the pixel-data offset, which `bfOffBits` rescues).
* `biCompression` **must be 0** (`BI_RGB`). Any other value raises
  `CDSSimpleException(9, 0xB)`. **No `BI_RLE4`, no `BI_RLE8`, no
  `BI_BITFIELDS`, no JPEG/PNG-in-BMP is supported.** ← *this answers
  the open question in `STATUS.md`.*
* For `biBitCount ∈ {1, 2, 4, 8}` the palette is read
  (`(biClrUsed ? biClrUsed : default_for_depth) * 4` bytes) immediately
  after the header; defaults come from `DAT_004b0030 = {2, 4, 16, 256,
  256, 256, 256, -1}` indexed by the engine's format index (see below).
* `bfOffBits` is honoured before the pixel-data read (the file cursor
  is repositioned via `Seek(startPos + bfOffBits)`).
* Pixel rows are read **bottom-up** (one `Read` call per row, of stride
  bytes), which is the canonical BMP row order.
* `biBitCount == 16` is interpreted as **DIB 5-5-5 little-endian** and
  in-place expanded to the engine's native **5-6-5** with a trivial
  green-bit shift (no green-bit duplication, no error diffusion).

The on-disk format the saver emits is identical: 14+40 bytes of header,
optional palette, bottom-up rows. The saver also has two adaptive
guards:

* **Stride DWORD-alignment guard** — if the source image's stride is
  not a multiple of 4 (`FUN_00436020` with `strict=false` returns
  `false`), the saver allocates a fresh `CDSBmpImage` at the *same*
  format, copy-blits into it (which re-allocates with DWORD-aligned
  stride per `FUN_00436f40`), and recurses.
* **32 bpp → 24 bpp downconvert** — once stride is OK, if the image
  is format-index 6 (32 bpp), the saver builds a fresh `CDSBmpImage`
  at format-index 5 (24 bpp), copy-blits, and recurses. So 32-bpp
  sources are never written to disk directly; they are always
  truncated to 24 bpp first.

For format-index 4 (16 bpp), the saver does a 5-6-5 → 5-5-5 row-pack
into a malloc'd scratch buffer (`FUN_00425130`) and writes the packed
rows. For all other formats the rows are written verbatim.

## Why bespoke and not a library?

Three independent signals all point the same way:

1. **No string fingerprints.** `bulanci.exe` contains exactly **one**
   string referencing BMP (`.?AVCDSBmpImage@@`, the RTTI class name at
   `0x004afde0` — pointed to by every one of the class's four
   `RTTICompleteObjectLocator`s). No format string, no error string,
   no version banner. libbmp, FreeImage, DevIL and stb_image all ship
   with characteristic strings (`"FreeImage_Load"`,
   `"ILU_INTERNAL_ERROR"`, etc.) and none of them are present.

2. **Error reporting is engine-native.** Both validation paths (bad
   magic, non-zero compression) throw `CDSSimpleException(9, 0xB)` via
   `FUN_00434c20`, the engine-wide exception ctor also used by FLX,
   DSM, JPEG, etc. A library port would carry its own error codes.

3. **I/O is engine-native.** Reads/writes are dispatched through the
   `CDSStreamStorage` virtual interface (vtable slots `+0x10` = read,
   `+0x14` = write, `+0x20` = tell, `+0x28` = seek). There is no
   `FILE*`, `HANDLE`, `fread`, `IStream` or `FreeImageIO` anywhere in
   the BMP path. Every known third-party BMP loader of the era either
   uses `FILE*` (libbmp, stb), `IStream*` (Windows COM/IPicture), or
   its own bridge struct (FreeImage); none of those signatures appear.

The code structure also matches a textbook hand-rolled BMP reader
("read file header → check magic → read info header → check
compression → optional palette → bottom-up rows") line for line. The
5-5-5 ↔ 5-6-5 conversions in `Load` and `Save` are written as exact
symmetric inverses of each other (`Load` masks `0xfc1f` and shifts
green up; `Save` masks `0xf81f` and shifts green down) — that's the
tell of one author writing both sides from first principles, not a
library bridge.

## Engine pixel-format index

A single switch (`FUN_00435c70`) maps `biBitCount` to an internal
"format index" used everywhere in `CDSImage`:

| Format index | biBitCount | Engine layout       | Default palette entries |
|-------------:|-----------:|---------------------|------------------------:|
| 0            | 1          | indexed             | 2   |
| 1            | 2          | indexed             | 4   |
| 2            | 4          | indexed             | 16  |
| 3            | 8          | indexed             | 256 |
| 4            | 16         | **5-6-5**           | 256 (unused at depth) |
| 5            | 24         | BGR triples         | 256 (unused at depth) |
| 6            | 32         | BGRA quads          | 256 (unused at depth) |
| 7            | (default)  | (unknown)           | -1  |

The bits-per-pixel and default-palette tables live at `DAT_004b0050`
and `DAT_004b0030` respectively. The key wart here is **format 4**:
the engine stores 16 bpp as RGB 5-6-5, but the BMP standard stores it
as 5-5-5. So `Load` and `Save` are the *only* places in the codebase
that do the format-index-4 ↔ DIB-5-5-5 conversion; once the data is
inside the engine, everything else assumes 5-6-5.

## Address map (12 functions, addresses are stable RVAs)

| Address     | Role                                                                | Body lines |
|-------------|---------------------------------------------------------------------|-----------:|
| `004320c0`  | `CDSBmpImage::Load(IDSStream*)` — BMP reader                        | ~70        |
| `00432440`  | `CDSBmpImage::Save(IDSStream*)` — BMP writer                        | ~110       |
| `00432330`  | `CDSBmpImage::ctor` — wires the five vftables, calls `CDSImage::ctor` (`FUN_00425460`) | 12 |
| `00432700`  | `CDSBmpImage::~CDSBmpImage` scalar-deleting destructor (primary)    | 5          |
| `00432290`  | `CDSBmpImage::__getClassData` — returns `&DAT_004b7dc4` (zero-init slot, looks like a "class globals" pointer that is never written) | 1 |
| `004322a0`  | scalar-deleting-dtor thunk for the +0x4c base sub-object (adjusts `this -= 0x4c`, tail-calls `FUN_00432700`) | 2 |
| `004322b0`  | scalar-deleting-dtor thunk for the +0x54 base sub-object            | 2          |
| `004322c0`  | scalar-deleting-dtor thunk for the +0x58 base sub-object            | 2          |
| `004322e0`  | scalar-deleting-dtor thunk for the +0x04 base sub-object            | 2          |
| `00425130`  | row-buffer (re)alloc helper used by `Save` for the 5-6-5 → 5-5-5 scratch (only caller: `FUN_00432440` at `0043262f`) | 6 |
| `00436020`  | `CDSBmpImage::ValidateStride(strict)` — `strict=false` returns "stride is DWORD-aligned"; `strict=true` returns "stride equals the minimal `(bpp*width+7)/8`". Called from `Save` (`0043247d`). | 5 |
| `00436060`  | `CDSBmpImage::FillBitmapInfoHeader(out40)` — fills the 40-byte `BITMAPINFOHEADER` for the saver (`biSize=40`, `biPlanes=1`, `biCompression=0`, `biSizeImage=stride*height`, etc.). Called from `Save` (`00432561`). | 18 |

The five vftables that `MODULES.md` counts come from `ctor` writing
into `this+0x00`, `this+0x04`, `this+0x4c`, `this+0x54`, and
`this+0x58`. They live back-to-back in `.rdata` starting at
`0x004871f0` and each is preceded by a `RTTICompleteObjectLocator`:

| Sub-object offset | RTTI descriptor | VFT address  | Notable slots                                                |
|------------------:|-----------------|--------------|--------------------------------------------------------------|
| `+0x00` (primary) | `0x004a3f84`    | `0x0048724c` | `[__getClassData, ~CDSBmpImage, CDSImage::Release]`          |
| `+0x04`           | `0x004a3ff0`    | `0x00487238` | `[CDSChain::Release, ..., ..., ~CDSBmpImage_thunk(-0x4)]`     |
| `+0x4c`           | `0x004a4004`    | `0x00487224` | `[..., ..., ..., ~CDSBmpImage_thunk(-0x4c)]`                 |
| `+0x54`           | `0x004a4018`    | `0x00487208` | `[inherited_release, CDSImage::Release_chain, …, ~CDSBmpImage_thunk(-0x54), **Load**, **Save**]` |
| `+0x58`           | (pre-`0x004871f0`) | `0x004871f0` | `[..., release_thunk(-0x54), release_thunk(-0x58), ~CDSBmpImage_thunk(-0x58)]` |

Each RTTI descriptor reads `{flags=0, this_offset=<sub_obj_offset>,
cd_offset=0, type_desc_ptr=0x004afde0}` and they all share the same
`.?AVCDSBmpImage@@` type-descriptor at `0x004afde0` — that's the only
BMP-related string in the binary.

The two virtual slots **that matter for matching** are pinned at
`Load = +0x18` and `Save = +0x1c` of the `+0x54` sub-object vtable (the
one that hosts `CDSImage::Load`/`Save`). The JPEG decoder next door uses
the same vtable shape — its `Load`/`Save` will be at the same offsets of
its own `+0x54` sub-object vtable.

## `Load` — `FUN_004320c0` reconstructed

```c
void CDSBmpImage::Load(IDSStream *stm) {
    uint64_t startPos = stm->Tell();                // vftbl[+0x20]
    BITMAPFILEHEADER fh;
    stm->Read(&fh, 14);                             // vftbl[+0x10]
    if (fh.bfType != 0x4D42)
        CDSSimpleException::throw_(9, 0xB);

    BITMAPINFOHEADER ih;
    stm->Read(&ih, 40);                             // vftbl[+0x10]
    if (ih.biCompression != 0)
        CDSSimpleException::throw_(9, 0xB);

    int fmt = MapBitCountToFormat(ih.biBitCount);   // FUN_00435c70
    int paletteEntries = ih.biClrUsed
                       ? ih.biClrUsed
                       : g_defaultPaletteSize[fmt]; // DAT_004b0030
    CDSImage::Init(this->image, ih.biWidth, ih.biHeight,
                   fmt, /*strictStride=*/0, paletteEntries);
                                                    // FUN_00436f40

    stm->Seek(startPos + fh.bfOffBits, SEEK_SET);   // vftbl[+0x28]

    if (ih.biBitCount < 9)
        stm->Read(this->image.GetPaletteBuffer(),
                  paletteEntries * 4);

    int    stride = this->image.stride;             // image+0x10
    int    h      = this->image.height;             // image+0x08
    uint8_t *plane = (uint8_t *)this->image.GetColorPlane();
    uint8_t *row   = plane + stride * h;
    for (int i = 0; i < h; ++i) {
        row -= stride;
        stm->Read(row, stride);
    }

    if (fmt == 4) {                                 // DIB 5-5-5 → engine 5-6-5
        uint16_t *p = (uint16_t *)plane;
        int w = this->image.width;
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                uint16_t v = p[x] & 0xfc1f;         // keep R+B
                p[x] = ((uint16_t)(uint8_t)(v >> 8) << 1) << 8
                     |  (uint8_t)v
                     | ((p[x] & 0x3e0) << 1);       // shift G up by 1
            }
            p = (uint16_t *)((uint8_t *)p + stride);
        }
    }
}
```

The 5-5-5 → 5-6-5 expansion zeros the new green LSB rather than
duplicating the high bit. This is the simplest possible conversion
(and exactly what a textbook port would do).

## `Save` — `FUN_00432440` reconstructed

```c
void CDSBmpImage::Save(IDSStream *stm) {

    // ---- outer guard: stride must be DWORD-aligned ----
    if (!CDSImage::ValidateStride(this->image, /*strict=*/false)) {
        CDSBmpImage copy;
        CDSBmpImage::ctor(&copy, &this->image.width,
                          this->image.format, /*strictStride=*/0);
        memcpy(copy.image.palette, this->image.palette,
               this->image.paletteEntries * 4);
        CPoemScroller::BlitDispatch(&copy.scroller, 0, &this->image, 0, 0);
        copy.Save(stm);                              // recurse
        copy.~CDSBmpImage();
        return;
    }

    // ---- inner guard: 32 bpp source → 24 bpp copy ----
    if (this->image.format == 6 /*32 bpp*/) {
        CDSBmpImage copy;
        CDSBmpImage::ctor(&copy, &this->image.width,
                          /*format=*/5 /*24 bpp*/, /*strictStride=*/0);
        CPoemScroller::BlitDispatch(&copy.scroller, 0, &this->image, 0, 0);
        copy.Save(stm);                              // recurse
        copy.~CDSBmpImage();
        return;
    }

    // ---- main write path (formats 0..5) ----
    BITMAPINFOHEADER ih;
    CDSImage::FillBitmapInfoHeader(this->image, &ih);   // FUN_00436060

    int paletteEntries = (1 <= this->image.paletteEntries
                       && this->image.paletteEntries <= 256)
                       ? this->image.paletteEntries : 0;
    int paletteBytes   = paletteEntries * 4;

    BITMAPFILEHEADER fh = {
        .bfType    = 0x4D42,
        .bfSize    = this->image.stride * this->image.height
                   + paletteBytes + 14 + ih.biSize,
        .bfOffBits = 14 + ih.biSize + paletteBytes,
        .bfReserved1 = 0, .bfReserved2 = 0,
    };
    stm->Write(&fh, 14);                                // vftbl[+0x14]
    stm->Write(&ih, 40);

    if (paletteBytes)
        stm->Write(this->image.GetPaletteBuffer(), paletteBytes);

    int       stride = this->image.stride;
    int       h      = this->image.height;
    uint8_t  *row    = (uint8_t *)this->image.GetColorPlane()
                     + stride * h;
    uint16_t *scratch = NULL;
    if (this->image.format == 4)
        scratch = malloc(stride);                       // FUN_00425130 wrapper

    while (h--) {
        row -= stride;
        if (this->image.format == 4) {
            // engine 5-6-5 → DIB 5-5-5: drop green LSB, repack into scratch
            uint16_t *src = (uint16_t *)row;
            for (int x = 0; x < this->image.width; ++x) {
                uint16_t v = src[x] & 0xf81f;          // keep R+B
                scratch[x] = (uint8_t)v
                           | ((uint16_t)(uint8_t)(v >> 9) << 8)
                           | ((src[x] >> 1) & 0x3e0);  // shift G down by 1
            }
            stm->Write(scratch, stride);
        } else {
            stm->Write(row, stride);
        }
    }
    free(scratch);
}
```

## Implications for the match track

* Promote `CDSBmpImage` to **Verified** in `STATUS.md`.
* Two functions need byte-exact matching: `Load` (`004320c0`) and
  `Save` (`00432440`). Together they're under 200 lines of C.
* The two BMP-only helpers (`ValidateStride` `00436020`,
  `FillBitmapInfoHeader` `00436060`) and the row scratch realloc
  (`00425130`) are 5–18 lines each and only called by `Save`.
* `FUN_00435c70` is a perfect compile-template candidate for the
  bit-count switch — match by switch-table data only.
* The four scalar-deleting destructor thunks (`004322a0..004322e0`)
  can be matched trivially or generated by giving Ghidra the correct
  multiple-base class layout for `CDSImage`. They differ only in the
  adjuster constant fed into a tail-call of `FUN_00432700`.
* The constructor at `00432330` only needs the five `vftable&`s and
  the call to `CDSImage::ctor` (`FUN_00425460`) wired up — match
  emerges for free once `CDSImage` itself is verified.

There is **no third-party library to attribute**, so we do not get a
bulk-match win here — but the surface is so small that "by-hand" means
two functions of ~70 and ~110 lines plus a few thunks.

## Open questions

None remaining at the BMP layer. Anything further (in particular the
`CDSImage` base-class layout, the `CPoemScroller::BlitDispatch` and
`GetColorPlane` plumbing, and the engine's palette ownership model)
belongs in the `CDSImage` / Stream-hierarchy entries.

## Artefacts

* `bmp_decoder.md` (this file).
* No `.py` artefact — the format the loader accepts is dead-standard
  Microsoft DIB; `Pillow` or `wand` will decode it natively, and the
  unpacker already exposes BMP payloads as plain `.bmp` files via the
  generic resource dump path.
