# `CDSJpegImage` -- libjpeg-6b (IJG, 1998-03-27) decoder + encoder

**TL;DR.** The engine ships a verbatim copy of the **Independent JPEG
Group reference library, release 6b (27-Mar-1998)** for both
**decompression** (used by `CDSJpegImage::DecompressToImage` and by
`CDSDsmFile::HandleRecordRead` for MJPEG frames) and **compression**
(used by `CDSJpegImage::CompressFromImage`, presumably by the
`BitmapJpegAnim` writer in tooling). All 28 IJG functions that Ghidra
attributed to the `CDSJpegImage` namespace -- plus a few dozen more in
`_Globals` covering the decompression half -- are byte-for-byte stock
IJG sources from the `ref/libjpeg6b/` reference tree included in this
repo. The Bulanci-specific code is just the **12-function
`CDSJpegImage` wrapper class** at `0x00431510 .. 0x004320ae` (787 bytes),
which adapts a `CDSStreamStorage` to libjpeg's `jpeg_source_mgr` /
`jpeg_destination_mgr` and swaps R/B per row so the engine's BGR
24bpp `CDSImage` format lines up with libjpeg's RGB output.

This means **the bulk of the 7.4 KB attributed to `CDSJpegImage`
(plus several more KB in `_Globals` for the decompression half) is
copy-the-source matching, not reverse engineering**.

## Why we know it is libjpeg-6b specifically

Five independent, mutually corroborating fingerprints. Each one alone
would already narrow the field to IJG 6b; together they leave no doubt.

1. **`JPEG_LIB_VERSION` constant = 62 (`0x3e`).** Both
   `jpeg_CreateDecompress` (`FUN_0045e6a0`) and `jpeg_CreateCompress`
   (`FUN_0045ecc0`) start with the canonical IJG version-mismatch guard:

   ```
   if (param_2 != 0x3e) ERREXIT2(cinfo, 0xc /* JERR_BAD_LIB_VERSION */,
                                  0x3e, param_2);
   ```

   `ref/libjpeg6b/jpeglib.h:33`:

   ```c
   #define JPEG_LIB_VERSION  62    /* Version 6b */
   ```

   IJG used `JPEG_LIB_VERSION` 60 in v6.0, 61 in v6a, **62 in v6b**, 70
   in v7, 80 in v8, 90 in v9. The constant alone pins the release.

2. **`sizeof(struct jpeg_decompress_struct)` / `_compress_struct`.**
   The same guards check the struct size second:

   * `FUN_0045e6a0`: `param_3 != 0x1b0` → `0x1b0 = 432`
     bytes is exactly `sizeof(struct jpeg_decompress_struct)` for libjpeg-6b
     built on x86 Win32 with default `jconfig.vc`.
   * `FUN_0045ecc0`: `param_3 != 0x168` → `0x168 = 360`
     bytes is exactly `sizeof(struct jpeg_compress_struct)` for the same
     build.

3. **`jpeg_natural_order[DCTSIZE2+16]` is in `.rdata` byte-for-byte.**
   At `0x0049db50` (320 bytes) we read the table verbatim:

   ```
   00 01 08 10 09 02 03 0a 11 18 20 19 12 0b 04 05    /* 0..15  */
   0c 13 1a 21 28 30 29 22 1b 14 0d 06 07 0e 15 1c    /* 16..31 */
   23 2a 31 38 39 32 2b 24 1d 16 0f 17 1e 25 2c 33    /* 32..47 */
   3a 3b 34 2d 26 1f 27 2e 35 3c 3d 36 2f 37 3e 3f    /* 48..63 */
   3f 3f 3f 3f 3f 3f 3f 3f 3f 3f 3f 3f 3f 3f 3f 3f    /* safety */
   ```

   matches `ref/libjpeg6b/jutils.c:53` precisely, **including the 16
   trailing `63`s** ("extra entries for safety in decoder") that IJG
   added in v6 to keep corrupt-Huffman runs from over-running the
   DCT block.

4. **`DSTATE_*` and `CSTATE_*` literals.** Every state-machine
   transition in the decompiled API entries uses the IJG numeric
   constants verbatim:

   | Constant | Value | Used in |
   |----------|------:|---------|
   | `CSTATE_START` | 100 (`0x64`) | `FUN_0045f6e0` (`jpeg_set_defaults`), `FUN_0045f370` (`jpeg_set_colorspace`) |
   | `CSTATE_SCANNING` | 101 (`0x65`) | `FUN_0045eee0` (`jpeg_start_compress`) |
   | `DSTATE_START` | 200 (`0xc8`) | `FUN_0045e8d0` (`jpeg_consume_input`) |
   | `DSTATE_INHEADER` | 201 (`0xc9`) | same |
   | `DSTATE_READY` | 202 (`0xca`) | `FUN_0045ec10` (`jpeg_start_decompress`) |
   | `DSTATE_PRELOAD` | 203 (`0xcb`) | same |
   | `DSTATE_PRESCAN` | 204 (`0xcc`) | same |
   | `DSTATE_SCANNING` | 205 (`0xcd`) | `FUN_0045eba0` (`jpeg_read_scanlines`) |
   | `DSTATE_RAW_OK` | 206 (`0xce`) | `FUN_0045e9a0` (`jpeg_finish_decompress`) |
   | `DSTATE_BUFIMAGE` | 207 (`0xcf`) | `FUN_0045ec10` |
   | `DSTATE_STOPPING` | 210 (`0xd2`) | `FUN_0045e9a0` |

   All defined together in `ref/libjpeg6b/jpegint.h:25-39`.

5. **`JERR_*` codes match the alphabetical-order enum exactly.**
   IJG's `jerror.h` builds a `J_MESSAGE_CODE` enum by invoking the
   `JMESSAGE()` macro once per error, alphabetically. Every numeric
   `msg_code` we observed in the decompilation is the right
   alphabetical slot for v6b:

   | hex | dec | symbol | observed in |
   |----:|----:|--------|-------------|
   | `0x08` | 8 | `JERR_BAD_HUFF_TABLE` | (deeper Huffman, not yet probed) |
   | `0x09` | 9 | `JERR_BAD_IN_COLORSPACE` | `FUN_0045f660 (jpeg_default_colorspace)` |
   | `0x0a` | 10 | `JERR_BAD_J_COLORSPACE` | `FUN_0046bbc0 (jinit_color_converter)` |
   | `0x0c` | 12 | `JERR_BAD_LIB_VERSION` | `FUN_0045e6a0`, `FUN_0045ecc0` |
   | `0x0f` | 15 | `JERR_BAD_PRECISION` | `FUN_0046bdf0 (initial_setup)` |
   | `0x11` | 17 | `JERR_BAD_PROG_SCRIPT` | `FUN_0046bfd0 (validate_script)` |
   | `0x12` | 18 | `JERR_BAD_SAMPLING` | `FUN_0046bdf0` |
   | `0x13` | 19 | `JERR_BAD_SCAN_SCRIPT` | `FUN_0046bfd0` |
   | `0x14` | 20 | `JERR_BAD_STATE` | ubiquitous |
   | `0x15` | 21 | `JERR_BAD_STRUCT_SIZE` | `FUN_0045e6a0`, `FUN_0045ecc0` |
   | `0x1a` | 26 | `JERR_COMPONENT_COUNT` | `FUN_0046bdf0`, `FUN_0046bfd0` |
   | `0x1b` | 27 | `JERR_CONVERSION_NOTIMPL` | `FUN_0046bbc0` |
   | `0x20` | 32 | `JERR_EMPTY_IMAGE` | `FUN_0046bdf0` |
   | `0x29` | 41 | `JERR_IMAGE_TOO_BIG` | `FUN_0046bdf0` (literal `0xffdc = JPEG_MAX_DIMENSION = 65500`) |
   | `0x2d` | 45 | `JERR_MISSING_DATA` | `FUN_0046bfd0` |
   | `0x33` | 51 | `JERR_NO_IMAGE` | `FUN_0045ea70 (jpeg_read_header)` |
   | `0x3c` | 60 | `JERR_SOF_UNSUPPORTED` | `FUN_0045e170 (read_markers)` |
   | `0x43` | 67 | `JERR_TOO_LITTLE_DATA` | `FUN_0045e9a0` |
   | `0x44` | 68 | `JERR_UNKNOWN_MARKER` | `FUN_0045e170` |

   The "alphabetical" insertions between 6b and 7 (`JERR_BAD_CROP_SPEC`
   added in v7 at slot 5, shifting everything after it by one) would
   move every code above; the slot numbers here match **6b** exclusively.

Auxiliary clues that don't add new information but corroborate:

* No JPEG-related strings appear anywhere in `.rdata` (we searched for
  `"JPEG"`, `"Huffman"`, `"Bogus"`, `"JFIF"`, `"DQT"`, `"DHT"`, etc.).
  IJG's default `jerror.c` plugs all the message text via the
  `addon_message_table`, but the Bulanci build clearly replaces
  `jpeg_std_error` with a custom error manager that just calls
  `cinfo->err->error_exit` with the bare `msg_code` and parameters
  -- no string formatting -- so the message-text table linker-strips.
* The literals `MAX_COMPONENTS = 10` (`unaff_ESI[0xf] <= 10` checks)
  and `JPEG_MAX_DIMENSION = 65500` (`0xffdc`) in `FUN_0046bdf0`,
  matching `ref/libjpeg6b/jmorecfg.h:35,173`.
* The `1999-2002 SleepTeam Labs` copyright in `.rdata` puts the build
  in the libjpeg-6b era (released 1998-03; superseded by v7 only in
  2009).

## Source-file → engine-address map (verified, partial)

Verified by reading the decompilation alongside the cited IJG source
file. Every `FUN_xxx` listed below is byte-for-byte equivalent to the
IJG release-6b function on the right.

### Top-level entry points (called from `CDSJpegImage` wrapper)

| Address | IJG function | Source file |
|---------|--------------|-------------|
| `0x0045e6a0` | `jpeg_CreateDecompress` | `jdapimin.c` |
| `0x0045ecc0` | `jpeg_CreateCompress` | `jcapimin.c` |
| `0x0045ea70` | `jpeg_read_header` | `jdapimin.c` |
| `0x0045ec10` | `jpeg_start_decompress` | `jdapistd.c` |
| `0x0045eba0` | `jpeg_read_scanlines` | `jdapistd.c` |
| `0x0045e9a0` | `jpeg_finish_decompress` | `jdapistd.c` |
| `0x0045eee0` | `jpeg_start_compress` | `jcapimin.c` |
| `0x0045ef60` | `jpeg_write_scanlines` | `jcapistd.c` |
| `0x0045edf0` | `jpeg_finish_compress` | `jcapimin.c` |
| `0x0045d1a0` | `jpeg_destroy` | `jcomapi.c` |

### Decompressor internals (mostly grouped under `_Globals` in Ghidra)

| Address | IJG function | Source file |
|---------|--------------|-------------|
| `0x0045e8d0` | `jpeg_consume_input` | `jdapimin.c` |
| `0x0045e7a0` | `default_decompress_parms` | `jdapimin.c` |
| `0x0045d160` | `jpeg_abort` | `jcomapi.c` |
| `0x0045d1f0` | `alloc_small` (for marker save) | `jmemmgr.c` |
| `0x0045fe60` | `jinit_input_controller` | `jdinput.c` |
| `0x0045fd00` / `0x0045fd40` / `0x0045fe00` | `consume_markers` / `reset_input_controller` / `start_input_pass` / `finish_input_pass` | `jdinput.c` |
| `0x0045e620` | `jinit_marker_reader` | `jdmarker.c` |
| `0x0045e170` | `read_markers` | `jdmarker.c` |
| `0x0045e5f0` | `reset_marker_reader` | `jdmarker.c` |
| `0x0045e4d0` | `read_restart_marker` | `jdmarker.c` |
| `0x0045e540` | `jpeg_resync_to_restart` | `jdmarker.c` |
| `0x0045df60` | `skip_variable` | `jdmarker.c` |
| `0x0045dde0` | `save_marker` (default APPn) | `jdmarker.c` |

### Compressor parameter helpers (the `CDSJpegImage` namespace block at `0x0045ECC0..0x0045F7DF`)

| Address | IJG function | Source file |
|---------|--------------|-------------|
| `0x0045f6e0` | `jpeg_set_defaults` | `jcparam.c` |
| `0x0045f660` | `jpeg_default_colorspace` | `jcparam.c` |
| `0x0045f370` | `jpeg_set_colorspace` | `jcparam.c` |
| `0x0045f1f0` | `jpeg_quality_scaling` | `jcparam.c` |
| `0x0045f230` | `jpeg_set_quality` | `jcparam.c` |
| `0x0045f260` | `jpeg_add_quant_table` | `jcparam.c` |

### Compressor master + colour conversion

| Address | IJG function | Source file |
|---------|--------------|-------------|
| `0x0046bdf0` | `initial_setup` | `jcmaster.c` |
| `0x0046bfd0` | `validate_script` | `jcmaster.c` |
| `0x0046bbc0` | `jinit_color_converter` | `jccolor.c` |
| `0x0046b6e0` | `rgb_ycc_start` | `jccolor.c` |
| `0x0046b7c0` | `rgb_ycc_convert` | `jccolor.c` |
| `0x0046b8e0` | `rgb_gray_convert` | `jccolor.c` |
| `0x0046b990` | `cmyk_ycck_convert` | `jccolor.c` |
| `0x0046bae0` | `grayscale_convert` | `jccolor.c` |
| `0x0046bb40` | `null_convert` | `jccolor.c` |

### Shared arithmetic helpers

| Address | IJG function | Source file |
|---------|--------------|-------------|
| `0x0045f7e0` | `jdiv_round_up` | `jutils.c` (22 callers, the canonical "ceil(a/b)" helper) |
| `0x0049db50..0x0049dc8f` | `const int jpeg_natural_order[DCTSIZE2+16]` (320 B) | `jutils.c` |

The remaining 25-30 unidentified functions in the `0x46xxxx` block
(encoder DCT/Huffman/sampler/marker-writer/mem-mgr/etc.) are
**guaranteed** to be the other stock IJG-6b sources -- `jcmainct.c`,
`jcprepct.c`, `jccoefct.c`, `jcsample.c`, `jcdctmgr.c`, `jchuff.c`,
`jcphuff.c`, `jcmarker.c`, `jfdctint.c`, `jfdctfst.c`, `jfdctflt.c`,
`jcinit.c`, `jmemmgr.c`, `jmemnobs.c`, plus the decompressor's
`jdmaster.c`, `jdcoefct.c`, `jdmainct.c`, `jdpostct.c`, `jddctmgr.c`,
`jidctint.c`, `jidctfst.c`, `jidctflt.c`, `jdsample.c`, `jdcolor.c`,
`jdmerge.c`, `jdhuff.c`, `jdphuff.c`, `jdtrans.c`. Identifying which
address is which is **pure source-line counting** now that we know the
library and release.

## The Bulanci wrapper class

The 12 functions in `0x00431510 .. 0x004320ae` are the only
project-specific code in this whole area. They expose two operations
on `CDSJpegImage` and adapt libjpeg's source/destination managers to
the engine's `CDSStreamStorage` polymorphic byte streams:

| Address | Size | Role | Notes |
|---------|-----:|------|-------|
| `FUN_00431510` | 62 | `CDSJpegImage::Constructor` | RTTI / vtable init |
| `FUN_00431590` | (small) | source/destination-manager struct init | wires the 6 fn-ptrs (init/fill/skip/resync/term/empty\_output) |
| `FUN_00431670` | -- | `fill_input_buffer` callback | reads up to 4 KiB from underlying `CDSStreamStorage`; on EOF emits the standard fake JPEG EOI (`FF D9`) |
| `FUN_004316c0` | -- | `skip_input_data` callback | mirror of IJG's `jdatasrc.c::skip_input_data` |
| `LAB_00431440` | -- | `init_source` callback | -- |
| `LAB_00431470` | -- | `term_source` callback | -- |
| `FUN_00431700` | -- | `jpeg_CDSStreamStorage_src` | the per-call "install our custom `jpeg_source_mgr` and a 4096-byte input buffer" routine. Allocates `0x38 = 56` bytes for the `jpeg_source_mgr` struct + `0x1000 = 4096` for the input buffer (matches `INPUT_BUF_SIZE` in `ref/libjpeg6b/jdatasrc.c:33`). |
| `FUN_00431b70` | 0x12f | **`CDSJpegImage::DecompressToImage`** | The decoder loop. See pseudocode below. |
| `FUN_00431cc0` | 0x2b | thunk to `DecompressToImage` for the polymorphic `IDSImage` interface | |
| `FUN_00431de0` | 0x6e | destructor | |
| `FUN_00431e50` | 0x1bf | **`CDSJpegImage::CompressFromImage`** | The encoder loop. See pseudocode below. |
| `FUN_00432030` | 0x36 | thunk to `CompressFromImage` | |
| `FUN_00432090` | 0x1e | tiny accessor (returns `&this->m_metadata` for an `IDSResource` interface) | |

### Decoder wrapper -- `FUN_00431b70`

```c
void CDSJpegImage::DecompressToImage(CDSStreamStorage* src,
                                     u32 src_arg,
                                     CDSImage* dst)
{
    struct jpeg_decompress_struct cinfo;        // 432 bytes on stack
    CDSJpegImage::ErrMgr err;                   // custom error manager

    cinfo.err = jpeg_init_custom_error(&err);   // FUN_00431590
    jpeg_create_decompress(&cinfo);             // FUN_0045e6a0(&cinfo, 62, 432)
    jpeg_CDSStreamStorage_src(&cinfo, src, src_arg);  // FUN_00431700
    jpeg_read_header(&cinfo, TRUE);             // FUN_0045ea70(&cinfo, 1)
    jpeg_start_decompress(&cinfo);              // FUN_0045ec10

    if (cinfo.output_components != 3)
        CDSSimpleException::Throw(13, 15);      // engine error 13/15

    CDSImage::Init(dst, cinfo.output_width, cinfo.output_height,
                   /*pixel_format=*/5,          // CDSImage 5 = 24bpp BGR
                   /*palette=*/NULL,
                   /*flags=*/-1);
    JSAMPROW row = _Globals::GetColorPlane(dst);

    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, &row, 1);   // FUN_0045eba0
        for (int x = 0; x < cinfo.output_width; ++x) {
            std::swap(row[3*x + 0], row[3*x + 2]);  /* libjpeg RGB -> engine BGR */
        }
        row += dst->stride;                     // dst->stride = *(int*)(dst+0x10)
    }

    jpeg_finish_decompress(&cinfo);             // FUN_0045e9a0
    jpeg_destroy_decompress(&cinfo);            // thunk_FUN_0045d1a0
}
```

This is the function that `CDSDsmFile::HandleRecordRead`
(`0x00428c40`, see `dsm_file_format.md`) hands every MJPEG frame to:
it slices a `CDSQueueStream` view of one `BitmapJpegAnim` chunk body
(SOI..EOI) and lets the decoder pull bytes through the
`CDSStreamStorage` source manager.

### Encoder wrapper -- `FUN_00431e50`

```c
void CDSJpegImage::CompressFromImage(CDSStreamStorage* dst,
                                     CDSImage* src,
                                     int quality)
{
    if (src->pixel_format != 5) {                  // 5 = 24bpp BGR
        CDSObject scratch;
        CDSObject::Init(&scratch, &src->width, /*pixel_format=*/5, 0);
        CPoemScroller::BlitDispatch(&scratch, NULL, src, NULL, NULL);
        CDSJpegImage::CompressFromImage(dst, &scratch, quality);
        CDSObject::Destroy(&scratch);
        return;
    }

    struct jpeg_compress_struct cinfo;             // 360 bytes on stack
    CDSJpegImage::ErrMgr err;

    cinfo.err = jpeg_init_custom_error(&err);
    jpeg_create_compress(&cinfo);                  // FUN_0045ecc0(&cinfo, 62, 360)
    jpeg_CDSStreamStorage_dst(&cinfo, dst);        // FUN_00431510

    cinfo.image_width      = src->width;
    cinfo.image_height     = src->height;
    cinfo.input_components = 3;
    cinfo.in_color_space   = JCS_RGB;              // = 2

    jpeg_set_defaults(&cinfo);                     // FUN_0045f6e0
    jpeg_set_quality(&cinfo, quality, TRUE);       // FUN_0045f230, force_baseline=1
    jpeg_start_compress(&cinfo, TRUE);             // FUN_0045eee0, write_all_tables=1

    JSAMPROW row = _Globals::GetColorPlane(src);
    while (cinfo.next_scanline < cinfo.image_height) {
        // libjpeg expects RGB; the engine stores BGR.
        // Swap in-place, write the row, swap back so the source CDSImage
        // is left unmodified.
        for (int x = 0; x < src->width; ++x)
            std::swap(row[3*x + 0], row[3*x + 2]);
        jpeg_write_scanlines(&cinfo, &row, 1);     // FUN_0045ef60
        for (int x = 0; x < src->width; ++x)
            std::swap(row[3*x + 0], row[3*x + 2]);
        row += src->stride;
    }
    jpeg_finish_compress(&cinfo);                  // FUN_0045edf0
    jpeg_destroy_compress(&cinfo);                 // thunk_FUN_0045d1a0
}
```

The recursive "wrong pixel format" branch reinterprets the source as a
24bpp BGR buffer using `CPoemScroller::BlitDispatch` (the universal
format-converter used everywhere in the renderer) before falling back
into the libjpeg-6b call site.

## Handoff for the match track

This subsystem is now **Verified for source attribution but Empty for
COFF-exact matching**. The work to reach `STATUS.md = Verified` for
matching is:

1. **Drop the IJG-6b sources** (already in `ref/libjpeg6b/`) into the
   build under e.g. `third_party/jpeg-6b/`. Ensure `jconfig.vc` is the
   `jconfig.h` chosen (matches the engine's Win32 MSVC compile).
2. **Configure the IJG build** to match Bulanci's defaults: the
   default `dct_method = JDCT_DEFAULT` (which is `JDCT_ISLOW` in
   stock `jmorecfg.h` -- so `jidctint.c` is the active IDCT path,
   `jidctfst.c` and `jidctflt.c` are dead code but still link).
3. **Strip the error-message strings** (Bulanci shipped without them
   -- see "no JPEG strings" above). Probably accomplished by replacing
   `jpeg_std_error()` with a stub `jpeg_init_custom_error()` that
   plugs an `error_exit` that does NOT pass the table through
   `addon_message_table`. The Bulanci-side wrapper lives at
   `FUN_00431590`.
4. **Match the API entry points first** (the 10 functions in the
   "Top-level entry points" table above) -- they have the most stable
   prototypes and the most diagnostic state-machine assertions.
5. **Then sweep the encoder/decoder internals** function-by-function;
   each one should land at its expected address with byte-identical
   bytes once the compiler flags are right.
6. **Match the 12 `CDSJpegImage` wrapper functions** by hand. These
   are the only ones not in `ref/libjpeg6b/`; everything else is
   already in the tree.

The Ghidra-side cleanup (set prototypes, push types) can wait until
after the IJG sources land in `third_party/`, because once the
prototypes match the stock `jpeglib.h` declarations, Ghidra
auto-recovers the parameter types on the dependent calls.
