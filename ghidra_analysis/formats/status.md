# Formats Subsystem Status

Confidence levels:
* **Empty** — not yet investigated.
* **Sketched** — high-level shape only; details TBD.
* **Partial** — most of the surface mapped; specific corners uncertain.
* **Verified** — Ghidra-cross-checked: layouts, dispatch tables and callsites are tied to specific addresses in `bulanci.exe`.

A subsystem at **Verified** is ready to drive the match track — see the root `README.md` ("Handoff to the match track").

---

## File / Payload Formats & Asset Decoders

These unlock matching for whole class clusters once the on-disk schema is known.

### Resource container (`.eap` / `.eapres` / overlay)

* **Classes:** (unpacker only)
* **Bytes:** —
* **Confidence:** **Verified**
* **What's known:** Format reverse-engineered from editor source (`GZipStream.cs`, `Level.cs`, `ResourceItem.cs`). Working unpacker.
* **Open questions:** None for the container itself; per-`ClassID` payloads are tracked under their own entries.
* **Artefacts:** `tools/bulanci_unpack/README.md`, `tools/bulanci_unpack/bulanci_unpack.py`

### BitmapSprite (`ClassID 52`)

* **Classes:** `CDSBitmap`, `CBulPicture`, `CDSFlxFile`
* **Bytes:** 70 B (header) + payload
* **Confidence:** **Verified**
* **What's known:**
  * Full 0x2c-byte header + post-header animation-frame stream + per-opcode pixel decoders all recovered.
  * Frames carry inner chunks dispatched by opcode at `CDSFlxFile::DecodeFrame @ 0x00432c60`; the unpacker ports the RLE (0x00/0x0e), delta (0x04/0x0f), palette (0x09) and transparent-index (0x0d) decoders verbatim (sources: `FUN_00432740`, `FUN_00432780`, `FUN_00432800`).
  * Empirically `fields[4]` is the bitmap width and `fields[5]` is the bitmap height (verified by `bw*bh == |decoded RLE|` on all 538 keyframe chunks).
  * 130/130 master-pack samples now emit a horizontal RGBA atlas PNG plus a JSON sidecar describing frame size, count, ring-frame index and per-frame metadata (engine opcodes applied, NotifyMove origin, duration).
  * **Palette inheritance resolved.** All 41 master-pack recolour variants now render in their authentic colours. Empirical rule: a sprite with no inline FLX opcode-0x09 palette inherits from the **most-recently-loaded inline-palette sibling** (the engine keeps the palette buffer live across consecutive sprite loads in the same render context). The master pack splits into three inline → followers chains:
    * `65715` → `65716..65747` (32 followers — Bulánek character poses sharing one team-tinted palette)
    * `65753` → `65754` (1 follower)
    * `65830` → `65831..65838` (8 followers — level-decoration item set sharing one palette)
  * The unpacker carries a 1024-byte ambient-palette state across its per-pack `save_resource` loop; inheriting sprites surface the link in `paletteSource: "inherited"` / `inheritedFrom: <head_id>` (atlas sidecar + manifest decoded block). Per-pack aggregate now reads `inline: 89, inherited: 41, grayscale-fallback: 0` across all 16 unpacked packs.
* **Open questions:**
  * Semantics of `fields[3]` (the "width" header field, which doesn't match the bitmap width).
* **Artefacts:** `./sprite_container.md`, `./flx_file_format.md`, `./inspect_sprite_headers.py`, `./check_sprite_manifest.py`, `./dump_chunk_bodies.py`, `./proto_decoder.py`

### MPx audio stream (not video — see write-up)

* **Classes:** `CDSMpx`, `CDSMpxStream`, `CDSMpxDecoder`, `CDSVideoPlayer`
* **Bytes:** 14.4 KB (+ 3.3 KB of per-layer decoders adjacent in `Engine.DS.CDSMpx`)
* **Confidence:** **Verified** — codec, library, frame layout, CRC, per-layer decoder, and player wiring all locked. Library source identified bit-for-bit as **libmad 0.15.1b** (`ref/libmad-0.15.1b/`, Underbit Technologies, Jan 2004). Only an actual `.mpx` byte-stream from a retail install is still missing for end-to-end on-disk validation, but the codec is fully understood.
* **What's known:**
  * `CDSMpx` is **libmad 0.15.1b** wrapped in a thin `CDSMpx::` class shell: a full **MPEG-1 / MPEG-2 / MPEG-2.5 Layer I/II/III audio decoder** (i.e. MP1+MP2+MP3). Same situation as `CDSJpegImage.cpp` re-using verbatim libjpeg-6b. Library identity established by seven independent cross-checks:
    1. **Q3.28 fixed-point format**: libmad's `MAD_F_FRACBITS = 28`, `MAD_F_ONE = 0x10000000`. Matches every scalefactor and cosine constant in the bulanci binary.
    2. **CRC-16 lookup table bit-identical** to libmad `bit.c::crc_table[256]` at `g_awMadCrcTable` (`0x0048b938`). First 8 entries `{0x0000, 0x8005, 0x800f, 0x000a, 0x801b, 0x001e, 0x0014, 0x8011}` -- bytes match verbatim.
    3. **CRC compute algorithm match**: `mad_bit_crc` (`0x004597b0`) has the same 32-bit-chunked + `switch(len/8)` 3/2/1 fall-through + bit-tail structure as libmad's `mad_bit_crc()` in `bit.c:197`.
    4. **Scalefactor table bit-identical** to libmad `layer12.c::sf_table[64]` (from `sf_table.dat`) at `g_anMadSfTable` (`0x0049c210`). 8/64 entries verified byte-for-byte starting with `0x20000000, 0x1965fea5, 0x1428a2fa, 0x10000000, 0x0cb2ff53, ...`.
    5. **Layer I linear table bit-identical** to libmad `layer12.c::linear_table[14]` at `g_anMadLinearTable` (`0x0049c310`). All 14 entries verified (`0x15555555 … 0x10002000`).
    6. **Layer I sample requantize**: `mad_layer_I_sample` (`0x0045c480`) is a byte-for-byte semantic match to libmad's `I_sample()` in `layer12.c:75` including the `&linear_table - 2` indexing trick that makes `[nb]` reach `linear_table[nb-2]`.
    7. **DCT32 cosine constants** in `g_anMadDct32Costab` (`0x0048af70`) are libmad's `synth.c::costab1..31` baked at compile time (e.g. `0x0b504f33 ≈ cos(π/4)`, `0x0ec835e8 ≈ cos(π/8)`, `0x0fb14be8 ≈ cos(π/16)`).
  * 32-point polyphase synthesis at `mad_synth_frame_dct32_full` (`0x00456910`, libmad `synth.c::dct32`); stereo / mono outer loops `DecodeGranuleStereo` (`0x00457aa0`) / `DecodeGranuleMono` (`0x00458450`); final `Q3.28 → s16` clamp at `QuantizeQ31ToS16` (`0x00446670`) implementing libmad's SSO-path output scaling (`>> 13` ≈ libmad's `SHIFT(x) = ((x + (1<<11)) >> 12)` followed by the final `>> 1` round-to-even). L2 requantisation is libmad `II_samples()` at `mad_layer_II_samples` (`0x0045c800`), with the bulanci shift `0x1d - nb = 29 - nb` matching libmad's `MAD_F_FRACBITS - (nb - 1)` exactly. No precomputed L2 lookup exists -- the 256 KiB table at `0x004b84f8` earlier misattributed as an L2 dequant table was actually the **2D alpha-blend LUT** initialised by `CBulanci::InitAlphaBlendLut` (corrected).
  * **Library bisect:** LAME 3.92 `mpglib` (`ref/lame392/mpglib/`) **falsified** (float vs Q3.28, no CRC verification, different L2 strategy, runtime cos() instead of pre-baked tables). libmad 0.15.1b (`ref/libmad-0.15.1b/`) **confirmed** with seven bit-identical fingerprints (see above). Likely build flags: `FPM_DEFAULT|FPM_64BIT + OPT_SSO + !OPT_DCTO`.
  * On-disk `.mpx` framing: **16-byte stripped `WAVEFORMAT`** then a raw MPEG audio bitstream (`{u32 dataSize; u16 ch; u16 bits; u32 rate; u32 avgBytesPerSec;}`), derived from `CDSMpxStream::{SaveMpxFile, LoadMpxFile}` (`0x00432eb0` / `0x00433180`) and the duration calc at `ComputeDurationMs` (`0x0043a000`). Struct pushed into Ghidra as `MpxFileHeader`.
  * Player chain: `CDSMpxStream → CDSMpx → CDSMpxDecoder (IDSAudioSource) → CDSAudioPlayer → DirectSound`. Consumed by `CDSAudioVideoPlayer` (alongside a separate video resource) for `CMovieView`.
  * `CDSVideoPlayer` is **misnamed** — it is a generic sorted-array multi-track media-source manager (4 funcs / 84 B), reused by the audio side of `CDSAudioVideoPlayer` at offset `+0x08`. Renamed funcs: `ConstructTrackManager`, `InsertOrFindTrack`, `AddTrackSource`, `SetCurrentTrack`, `BeginCurrentTrackPlayback`.
  * No `BitmapJpegAnim`/`MpxAudio`-class resources ship in the master pack overlay; `ResolveResource` (`0x00446b90`) gates re-loading on `(ClassID != 0x27) && (g_pDirectSoundSingleton->vt != null)` — i.e. script-VM stream sources skip the resource re-lookup, and DirectSound-less hosts skip MPx-resource resolution entirely.
* **Open questions:**
  * Locate an actual `.mpx` file (likely loose next to `bulanci.exe` in a full retail install) and round-trip the 16-byte header through `_scratch_mpx_header.py` (now committed in `ghidra_analysis/formats/`). No sample available in this tree.
  * Confirm `wBitsPerSample` as a 16-bit field vs. a packed codec hint.
  * Pin down the libmad build flags via `objdiff` once libmad is wired into the project build (current best guess: `FPM_DEFAULT|FPM_64BIT + OPT_SSO + !OPT_DCTO`).
  * `flags & 0x02` in `DispatchLayerDecoder` is now traced to a sticky config bit copied from `bitstreamCtx[0xe]` (byte 56) into `frameInfo[0xb]` (byte 44) at the top of `ReadOneFrame` — i.e. it is **not** a per-frame header bit, it is one-shot decoder configuration. Combined with the halved sample-rate / halved frame size and the `DecodeGranuleMono` switch, this strongly indicates a "force-mono / downmix" path (probably hooked into the `IDSAudioSource::SetChannels(1)` analogue). Still wants the bitstream-side setter located for final confirmation.
* **Artefacts:** `./mpx_audio_format.md`, `./_scratch_mpx_header.py`, `./libmad_function_map.md`

### DSM multi-stream container

* **Classes:** `CDSDsmFile`
* **Bytes:** 2.1 KB
* **Confidence:** **Verified**
* **What's known:**
  * `CDSDsmFile` is the engine-side reader for **ClassID 76 = `BitmapJpegAnim`** — the master pack's synchronized **MJPEG video + 16-bit PCM audio** movies.
  * 36-byte `CDsmHeader` (struct defined in Ghidra via `create_struct`) fully decoded:
    * `dwPayloadEndOffset` (file size + loop sentinel)
    * `dwCanvasWidth` / `dwCanvasHeight` / `dwPixelFormat` (CDSImage 5 = 24bpp BGR, 6 = 32bpp BGRA)
    * `dwDurationMs`
    * `dwFrameCount`
    * `dwAudioByteCount`
    * `dwAudioFormatPacked` (`(bits<<16)|channels` — `0x00100001` = 16-bit mono)
    * `dwAudioSampleRate` (22050 / 44100 Hz)
  * Followed by `2 * dwFrameCount` interleaved `{u32 len, byte[len]}` chunks: chunk `2k` is a complete JPEG (`FF D8 FF DB ... FF D9`), chunk `2k+1` is the matching frame's raw little-endian PCM.
  * Cross-verified on all four shipping resources in `unpacked/overlay/` — the computed audio bytes from `dwDurationMs * rate * channels * bits / 8 / 1000` matches the on-disk `dwAudioByteCount` within ≤ 2 bytes (sample alignment) for every file, and `dwPayloadEndOffset == sizeof(file)` exactly.
  * Runtime side: `N` concurrent reader/writer handles, each with its own 1 MiB `CDSMemQueue` ring buffer, tracked by the embedded handle bank at `obj+0x54`.
  * JPEG decode happens in `HandleRecordRead` (`0x00428c40`, vtable2 slot 7) via the engine's reusable `FUN_00431b70` (libjpeg-style with per-row R↔B swap).
  * 31/31 funcs accounted for; 23 renamed in Ghidra. Plate comments and inline disassembly comments now annotate the three `Read` calls and the `Tell64` call inside `HandleOpenStream`.
  * The unrelated `BULANCI.TMP` (ClassID 58 `DsmInner`) is just a name collision — it's a `MZ`-prefixed PE file.
* **Open questions:**
  * Whether the writer-side handle path (vtable2 slot 5 + `HandleQueueWrite`) is ever exercised at runtime, or if it exists only for tooling.
  * Mapping the four shipping animations (resources 77825 / 77827 / 77829 / 77831) to the gameplay events that trigger them (search scripts for those IDs).
* **Artefacts:** `./dsm_file_format.md`, `./_scratch_dsm_header.py`, `./_scratch_dsm_payload.py`

### FLX animation file

* **Classes:** `CDSFlxFile`
* **Bytes:** 1.7 KB
* **Confidence:** **Verified**
* **What's known:**
  * **Bulanci-original variant** (not stock FLI/FLC).
  * 36-byte file header, 5-byte frame header (`u32 size; u8 chunkCount;`), 5-byte inner-chunk header (`u32 size; u8 opcode;`).
  * Inner-chunk dispatcher `DecodeFrame` @ `0x00432c60` switches on the dense opcode set `{0,4,8,9,10,11,12,13,14,15}` — no overlap with stock FLI/FLC IDs.
  * Per-opcode decoders (`FUN_00432740` RLE, `FUN_00432780` delta, `FUN_004327e0` memcpy, `FUN_00432800` palette) all ported into `bulanci_unpack.py` and round-tripping cleanly on 130/130 BitmapSprite payloads in the master pack.
  * BRUN/LC decoders are FLI-inspired (same opcode polarity) but with a long-run u16 escape on BRUN.
  * Parallel mask plane (opcodes 0x0e/0x0f) is supported by the runtime; the master pack uses 0x0e but never 0x0f.
  * The container that wraps a FLX payload is `BitmapSprite` (ClassID 52); see `sprite_container.md`.
* **Open questions:** None at the FLX layer. Higher-layer questions (recolour-variant palette inheritance, `fields[3]` "width" semantics) tracked under the BitmapSprite entry.
* **Artefacts:** `./flx_file_format.md`, `./proto_decoder.py`

### JPEG codec

* **Classes:** `CDSJpegImage`
* **Bytes:** 7.4 KB (`CDSJpegImage` namespace) + ~several KB more in `_Globals` (decompressor internals Ghidra hasn't grouped yet)
* **Confidence:** **Partial** (source-attribution complete; per-func COFF-exact matching not yet done)
* **What's known:**
  * **Verbatim libjpeg-6b** (Independent JPEG Group reference, `JVERSION = "6b  27-Mar-1998"`). Reference sources committed at `ref/libjpeg6b/`.
  * Five mutually-independent fingerprints pin the release:
    * `JPEG_LIB_VERSION = 62` (`0x3e`) in the create-API guards at `FUN_0045e6a0` / `FUN_0045ecc0`.
    * `sizeof(jpeg_decompress_struct) = 432` and `sizeof(jpeg_compress_struct) = 360` from the same guards.
    * `const int jpeg_natural_order[DCTSIZE2+16]` at `0x0049db50` byte-for-byte (320 B, including the v6 "extra 16 × 63" safety pad).
    * `DSTATE_START..DSTATE_STOPPING` (200..210) and `CSTATE_START..CSTATE_WRCOEFS` (100..103) verbatim across the API entries.
    * Every observed `JERR_*` numeric code matches the v6b alphabetical enum slot (`0xc = JERR_BAD_LIB_VERSION`, `0x15 = JERR_BAD_STRUCT_SIZE`, `0x33 = JERR_NO_IMAGE`, etc.) — see the full table in `jpeg_decoder.md`.
  * The class ships **both** the decoder (used by `CDSJpegImage::DecompressToImage @ 0x00431b70` and by `CDSDsmFile::HandleRecordRead` for MJPEG frames) and the encoder (used by `CDSJpegImage::CompressFromImage @ 0x00431e50` and presumably by the `BitmapJpegAnim` writer in tooling).
  * Project-specific code is just the 12-function `CDSJpegImage` wrapper class at `0x00431510..0x004320ae` (787 B), which adapts a `CDSStreamStorage` to libjpeg's `jpeg_source_mgr` / `jpeg_destination_mgr` (`INPUT_BUF_SIZE = 4096` matches `jdatasrc.c`) and per-row swaps R↔B so the engine's 24bpp BGR `CDSImage` (pixel-format 5) lines up with libjpeg's RGB output.
  * IJG message strings (`"JPEG ..."`, `"Bogus ... "`, etc.) are absent — the engine plugs a custom error manager that calls `error_exit` with the bare `msg_code`, so the message-table linker-strips. This is the only deliberate modification.
  * 10 top-level API entry points already mapped to their IJG source file (`jdapimin.c`, `jcapimin.c`, `jdapistd.c`, `jcapistd.c`, `jcomapi.c`) plus 25+ internals (`jdmarker.c`, `jdinput.c`, `jcparam.c`, `jcmaster.c`, `jccolor.c`, `jutils.c`) — see the address tables in `jpeg_decoder.md`.
* **Open questions:**
  * Which `dct_method` the default config selects (`JDCT_ISLOW`/`JDCT_IFAST`/`JDCT_FLOAT`). Stock `JDCT_DEFAULT` is `JDCT_ISLOW`, so `jidctint.c` is the live path; the other two are dead-but-linked code. Verifying this lets us drop the dead IDCT sources from the `third_party` build to avoid bloat.
  * Per-function COFF-exact addresses for the 50+ unmapped IJG internals in the `0x46xxxx` block (encoder DCT / Huffman / sampler / marker writer) and the `0x45D000..0x45EBFF` block (decoder coef-ct / main-ct / post-ct / IDCT-mgr / Huffman / upsampler / colour deconverter). Pure source-line accounting, not investigation.
* **Artefacts:** `./jpeg_decoder.md`

### BMP decoder

* **Classes:** `CDSBmpImage`
* **Bytes:** 1.5 KB
* **Confidence:** **Verified**
* **What's known:**
  * **Bespoke code, not a library port.** No libbmp / FreeImage / DevIL / stb_image fingerprints; only BMP-related string in the binary is the RTTI type-descriptor `.?AVCDSBmpImage@@` at `0x004afde0`, shared by all five sub-object `RTTICompleteObjectLocator`s. I/O is engine-native through `CDSStreamStorage` virtual slots (read `+0x10`, write `+0x14`, tell `+0x20`, seek `+0x28`); errors are routed through the engine-wide `CDSSimpleException(9, 0xB)` (`FUN_00434c20`).
  * **Format accepted/emitted is bare-bones Microsoft DIB v3:** 14-byte `BITMAPFILEHEADER` + 40-byte `BITMAPINFOHEADER`, bottom-up rows. `bfOffBits` is honoured on read; V4/V5 headers are silently truncated to 40 bytes (any trailing bytes are skipped by the `bfOffBits` seek).
  * `Load` `FUN_004320c0` — magic `0x4D42` check, 40-byte info-header read, **rejects any `biCompression != 0`** (so RLE4/RLE8/BITFIELDS are *not* supported). Reads palette for `biBitCount ≤ 8`, then bottom-up rows. 16 bpp is taken as DIB 5-5-5 and in-place expanded to engine 5-6-5 (mask `0xfc1f`, green shifted up by 1; new LSB zeroed).
  * `Save` `FUN_00432440` — two adaptive guards then a single write path. Outer guard: if image stride is not DWORD-aligned, build a fresh `CDSBmpImage` at the *same* format, copy-blit, recurse. Inner guard: if format-index is 6 (32 bpp), build a fresh `CDSBmpImage` at format-index 5 (24 bpp), copy-blit, recurse. Main path writes 14+40 bytes of header, optional palette, then rows; for format-index 4 (engine 5-6-5) it packs each row to DIB 5-5-5 (mask `0xf81f`, green shifted down by 1) through a malloc'd scratch buffer (`FUN_00425130`).
  * `FUN_00435c70` is the `biBitCount → engine format index` switch (`1→0, 2→1, 4→2, 8→3, 16→4, 24→5, 32→6, else→7`); BPP and default-palette-entries tables live at `DAT_004b0050` / `DAT_004b0030`. Format-index 4 is **the only place** where the engine's 5-6-5 16 bpp layout differs from DIB 5-5-5; the two `Load`/`Save` conversions are exact symmetric inverses.
  * 12 functions accounted for, all renamed in Ghidra: `Load` (`004320c0`), `Save` (`00432440`), ctor (`00432330`), scalar-deleting dtor (`00432700`) + four base-adjuster thunks (`004322a0`/`b0`/`c0`/`e0`), `__getClassData` (`00432290`), row-scratch realloc (`00425130`), `ValidateStride` (`00436020`) and `FillBitmapInfoHeader` (`00436060`) — the last two are BMP-only helpers called from `Save`.
  * Five sub-object vtables in `.rdata` at `0x004871f0..0x00487254`; primary vtable @ `0x0048724c`, `Load`/`Save` slots @ `0x00487218`/`0x0048721c` (the `+0x54` sub-object vtable). The JPEG decoder next door uses the same vtable shape, so the `Load`/`Save` slot offsets carry over.
* **Open questions:** None at this layer.
* **Match track:** **9 / 12** functions matched in `src/bulanci/CDSBmpImage.cpp`, section score `bulanci/CDSBmpImage` **16.87 %** fuzzy / 11.55 % matched-code (168 / 1454 bytes). The three remaining holdouts are the two big readers (`Load` ~370 B, `Save` ~660 B) and the SEH-laden `ctor` (~150 B).
  * **100 % byte-exact:** `GetClassData`, `ValidateStride`, `FillBitmapInfoHeader`, `AllocRowScratch`, `scalar_deleting_dtor`, and all four MI-thunks (`scalar_deleting_dtor_thunk_n0x4/0x4c/0x54/0x58`). The four thunks and the dtor are `__declspec(naked)` inline-asm pieces (`sub ecx, N; jmp scalar_deleting_dtor` and the standard push-esi / call-base-dtor / conditional-free / ret-4 sequence). `AllocRowScratch` uses the same naked shape to call `_Globals::Runtime_Free` / `_Globals::Runtime_MallocOrThrow` with `ecx = &DAT_004b7c94`.
  * **`FillBitmapInfoHeader` is at 56 %** (above): MSVC8 chose the `lea edx, [eax-1]; cmp edx, 0xff; ja` LEA range-check pattern for the `1 <= n && n <= 256` palette clamp, while the target uses two split signed compares (`jle/jg`). All struct-write bytes are already aligned to the target order; the remaining delta is just the compiler's range-check + register-allocation choice for `biClrUsed`'s `(self->paletteEntries != clamped) ? clamped : 0` peephole.
  * **Pending:** `ctor` (149 B): SEH prologue + security cookie + delegation to `CDSImage::ctor` on `this+4` + 5 vtable patches at `+0x00/+0x04/+0x4c/+0x54/+0x58` + `refcount=1` + `chain=0`; the full body needs the multiple-inheritance class layout declared so MSVC schedules `CDSImage::ctor` correctly. `LoadDibStream` (368 B) and `SaveDibStream` (663 B) are large and require the `CDSStreamStorage` virtual-table dispatch tables to be wired up first. Reconstructed C for all three lives in `bmp_decoder.md`.
* **Artefacts:** `./bmp_decoder.md`

### Stream hierarchy

* **Classes:** `IDSStream` (interface) + `CDSFilterStream`, `CDSFileStream`, `CDSEasyMemStream`, `CDSQueueStream`, `CDSSafeStream`, `CDSGZipStream`. (`CDSGZipStreamData`, `CDSSafeStreamInfo` and `CDSMemQueue` are companion data/state classes held *by* a stream — they don't share the vtable contract.)
* **Bytes:** 7.6 KB
* **Confidence:** **Verified**
* **What's known:**
  * `IDSStream`'s vtable owns **15 virtual slots**, not "~6". The byte-IO contract is slots 4..12, with lifetime housekeeping at 0..3 and identity/teardown at 13..14: `Read / Write / Flush / GetSize / Tell / SetSize / Seek / Lock / Unlock / GetName / Close` (slot 10 `Seek` takes `(off_lo, off_hi, origin)` with origin `0=BEGIN / 1=CURRENT / 2=END`).
  * Slot ordering pinned by RTTI walk on **five** concrete classes (`CDSFileStream` @ vtbl `0x0047f72c`, `CDSEasyMemStream` @ `0x004803dc`, `CDSGZipStream` @ `0x00480594`, `CDSFilterStream` @ `0x004870fc`, `CDSQueueStream` @ `0x00486dac`) and double-checked against the raw vtable offsets used by `CDSFlxFile::BindStream` (`0x00432ac0`) and `CDSFlxFile::CloseStream` (`0x00432b60`).
  * Engine-side errno table (`CDSStreamException`, `.?AVCDSStreamException@@` @ `0x004afd6c`): `1=read 2=write 3=seek 4=setsize 5=lock 6=unlock 7=flush 8=uninitialised`.
  * **Naming correction.** The original entry treated `CDSStreamStorage` as the byte-stream base. It is not. `CDSStreamStorage` inherits from `IDSStorage` and is a *container of streams* (its 9-slot IDSStorage vtable is array-indexed getters/setters, not byte-IO); the actual byte-stream interface is `IDSStream` (`.?AVIDSStream@@` @ `0x004ae2a4`).
* **Open questions:** None at this level. The 4-slot `IDSReferenced` subobject vtable (lifetime / dtor / scalar-deleting-dtor) hasn't been split open; not on the critical path.
* **Ghidra plate comments:** **Every IDSStream slot for the four remaining concrete streams now has a plate comment** describing what it does, which `this+offset` fields it touches, the errno it raises, and any quirks (shared no-op thunks, ring-buffer math, 64-bit cursor pairs, etc.). Addresses covered:
  * CDSEasyMemStream: `0x4307f0`, `0x4308c0`, `0x409220`, `0x409230`, `0x4309f0`, `0x430980`, `0x430a40`, `0x430db0`, `0x430dc0`, `0x409200`.
  * CDSGZipStream: `0x435220`, `0x4352e0`, `0x446c10`, `0x435360`, `0x435390`, `0x446c20`, `0x4353c0`, `0x4099b0`, `0x409030`.
  * CDSFilterStream: `0x430420`, `0x430490`, `0x42ff90`, `0x42ffa0`, `0x430090`, `0x430d00`, `0x430500`, `0x4305c0`, `0x430640`, `0x4300d0`, `0x430c10`.
  * CDSQueueStream: `0x43c000`, `0x43c0b0`, `0x43beb0`, `0x43bee0`, `0x43bf10`, `0x43bf80`, `0x43c1a0`, `0x428960`.
  * Shared thunks: `0x467430` (no-op Flush for memory/queue streams, Ghidra-attributed to `CDSApp::CDSApp_PreCreateHook`), `0x43bff0` (Lock-unsupported, shared GZip/Queue), `0x434f90` (Unlock-unsupported, shared GZip/Queue).
* **Source-side matches:** `CDSFileStream.cpp`'s Read/Write/Flush/GetSize/Tell/SetSize/Seek/Lock/Unlock bodies are matched (with a preserved `// !PROLOGUE` block holding Win32 forwards, `FileHandle`, `SelfOrNull`, and TU-local `RaiseStreamException` / `ThrowStreamErrorNoReturn` shims so the matched bodies read like the original C++). The four other stream classes have plate comments + clean stubs only — matching their bodies is blocked on a separate issue documented below.
* **Blocked on signature-rendering:** the header generator emits Ghidra's `__fastcall (int param_1)` (where `param_1` is really `this`) as a one-arg member function `uchar Method(int param_1)`. MSVC8 then inserts a stack push for the extra arg at every call site, which breaks byte-matching for the dozen-odd slots whose matched bodies want to use `this` directly. Two ways out:
  1. Patch the header generator to drop the leading `this`-flavoured arg when the renderer was rendering a `__fastcall` method (preferred — fixes the entire codebase, not just streams).
  2. Hand-fix the stream slots by re-declaring them with the right prototype (e.g. `void CloseStream()` in the .h) and re-writing the matched body — local, but it puts the headers out of sync with what the next `sync_units.py` run will overwrite.
* **Artefacts:** `./stream_hierarchy.md`

### GZip compression

* **Classes:** `CDSGZipStream`, `CDSGZipStreamData`
* **Bytes:** 1.3 KB
* **Confidence:** **Verified**
* **What's known:**
  * Container is a custom seekable wrapper, **not** RFC 1952 gzip. The 4-byte file magic `1346984519u = 0x50495A47` spells `"GZIP"` in ASCII; per-block payload is the **zlib (RFC 1950) wrapper** -- `windowBits = +15`, `memLevel = 8`, `strategy = Z_DEFAULT_STRATEGY`, `level = 9 (best)`, single-shot `Z_FINISH`. Not raw deflate, not RFC 1952 gzip.
  * **zlib 1.1.3** is statically linked. Version string `"1.1.3"` at `0x00487594` (`g_szZlibVersion_1_1_3`); full zlib 1.1.x `z_errmsg` table at `0x0049e000..0x0049ed00`; `sizeof(z_stream) = 0x38` (the value passed to `inflateInit_`/`deflateInit_`) pins the layout to the 1.1.x series.
  * `inflateInit_` (`0x0046ef60`) and `deflateInit_` (`0x004704c0`) are stock zlib trampolines that lock the call to `inflateInit2_(..., 15, ...)` and `deflateInit2_(..., 9, 8, 15, 8, 0, ...)` -- i.e. the zlib wrapper with the default window/memLevel/strategy and `level = 9`. The body of `deflateInit2_` at `0x004702c0` enforces exactly the 1.1.x argument-validation rules (method == `Z_DEFLATED`, windowBits in `[8,15]`, memLevel in `[1,9]`, strategy in `[0,2]`, stream_size == `0x38`).
  * Block size **`0x8000` = 32 KB**; decompress scratch **`0x8040`** (block + 64 B slack); compress scratch **`0xA00C`** (`>= deflateBound(0x8000)`); both allocated by `CDSGZipStream::AllocateBuffers @ 0x00435670`. "Store raw if compressed >= 32 KB" rule enforced on both sides (boundary tested as `size == 32768`).
  * Engine class layout filed in `gzip_stream.md` (every named field pinned to a numeric offset by reading `Open`, `CloseInt`, `ReadBufferAt`, `WriteBuffer`, `AllocateBuffers`, and the inherited `Read` thunk at `0x00435220`). `CDSGZipStream` instance is >=`0x48` bytes; `CDSGZipStreamData` is exactly `0x20` bytes, refcounted, holds `m_lSize` / `m_nChunkCount` / `m_chunks[]`.
  * Method addresses (all renamed in Ghidra): `Open` (`0x004356e0`), `CloseInt` (`0x004354a0`), `ReadBufferAt` (`0x00435050`), `WriteBuffer` (`0x00435140`), `AllocateBuffers` (`0x00435670`), static `Compress` (`0x00434ee0`), static `Decompress` (`0x00434e30`).
* **Matching:** The 16 identified zlib helpers were renamed in Ghidra under the `zlib::` namespace and migrated to a new `zlib` unit (own `src/bulanci/zlib.cpp` / `include/bulanci/zlib.h` / `build.ninja` rule + `config/bulanci/units_listing.csv` row). Trailing underscores in zlib's API names (`inflateInit_`, `deflateInit2_`, ...) are stripped on both sides by `_sanitize_symbol_name()` / `sanitizeSymbolNames()`, so COFF symbols pair under the stripped form.
  * Four byte-exact matches in `src/bulanci/zlib.cpp`:
    * `zlib::inflateInit` (`0x0046ef60`) -- 26/26 bytes
    * `zlib::deflateInit` (`0x004704c0`) -- 37/37 bytes
    * `zlib::zcalloc` (`0x00471260`) -- 19/19 bytes (real body: `return calloc(items, size)`)
    * `zlib::inflate_codes_free` (`0x004733d0`) -- 22/22 bytes (real body: `ZFREE(z, c)` via z->zfree fn-ptr at offset +0x24).
  * Engine-level matches in `src/bulanci/_Globals.cpp`:
    * `CDSGZipStream::Decompress` (`0x00434e30`) -- 170/170 len-match, 33 bytes differ.
    * `CDSGZipStream::Compress` (`0x00434ee0`) -- 172/172 len-match, 33 bytes differ.
  Both engine diffs are MSVC 8 register-allocator preference (`EBX` vs `EDI` for the `destLen` pointer) and a single block-layout swap -- pure compiler-internal choice, no semantic difference. Expected to resolve to 0 once real zlib 1.1.3 bodies replace the six core opacity-barrier stubs (`zlib::inflate`, `zlib::deflate`, `zlib::inflateInit2`, `zlib::deflateInit2`, `zlib::inflateEnd`, `zlib::deflateEnd`) -- the barriers are what currently force the allocator's hand. See the "Stub opacity barriers" section in `gzip_stream.md` before touching them.
  The remaining ~35 zlib helpers in `0x46d000-0x474000` (`huft_build`, `inflate_codes`, `inflate_fast`, `_tr_init`, `_tr_flush_block`, `compress_block`, `deflate_fast/slow`, `lm_init`, `longest_match`, `adler32`, `crc32`, `zcfree`, ...) still need identification -- see the helper inventory table in `gzip_stream.md`.
  Refresh status with `python scripts/internal/zlib_status.py`; per-function hex dump via `python scripts/internal/zlib_dump_pair.py <symbol> [src.obj] [tgt.obj]`. A `build/orig/bulanci/zlib.obj` baseline is required for the `zlib::*` column; regenerate it via `python scripts/export_ghidra_objs.py` (Ghidra GUI must be closed).
* **Open questions:** Remaining `CDSGZipStream` public surface (`Read`, `Write`, `Seek`, `Flush`, `SetLength`, `Close`, ctors/dtor) is still under `FUN_xxxx` names -- not blocking matching, since the field layout is fixed and the per-method behaviour is the C# mirror.
* **Artefacts:** `./gzip_stream.md`, `scripts/internal/zlib_status.py`, `scripts/internal/zlib_dump_pair.py`

### Editor colophon record (was: "Resource signing")

* **Classes:** `CDSResourceSign` (engine ClassID `0x5E` = 94 = `Sign`)
* **Bytes:** 0.4 KB
* **Confidence:** **Decoded**
* **What's known:** A serialisable record carrying `{ CDate publishDate, wstring content, wstring copyright, u8 flag }`, total `sizeof = 0x28`. Constructor at `0x00434540`, factory at `0x00434930`, read/write at `0x004343A0` / `0x00434310`. The 14 vtable-reachable methods are 2 stream helpers + read + write + ctor + class-meta getter + dtor body + scalar-deleting dtor + 3+3 adjustor thunks for the dtor / Release on the four sub-objects. The 4 vtables match the RTTI Class Hierarchy Descriptor at `0x004A4518` (8 base classes, sub-object offsets `0 / 4 / 8 / 16`). The class is registered at static-init by `0x0047D1D0..0x0047D260` and is only reachable through `InitializeByClassId(94, stream)`. **Despite the name, there is no cryptography here**: no hash, no key, no cipher, no MAC. It's a "signed by the author" colophon, not a crypto signature.
* **Open questions:** Whether Editor.exe ever sets `flagByte` to anything other than 0 (the one shipping instance has `0x00`); what triggers the post-v1 conditional read branch in `ReadFromStream`.
* **Artefacts:** `./sign_record.md`; the single shipping instance `unpacked/tutorial_eap/res_0000100000_94_Sign.bin` (`2006-08-26 / "Tutorial 1" / "SleepTeam Labs"`).
* **Source-side matching status (`src/bulanci/CDSResourceSign.cpp`)**:
  * **Byte-exact (100 %):** 13 of 15 functions, totalling 525 of the unit's 553 bytes.
    * **Trivial forwarders:** `FUN_0042e680` (ReadDateField, 19 B), `FUN_0042e6a0` (WriteDateField, 19 B), and `FUN_004345c0` (GetClassRegistry, 6 B).
    * **Plain matched body:** `FUN_00434310` (WriteToStream, 55 B).
    * **`/GS` + SEH ctor and dtor:** `FUN_00434540` (126 B) and `FUN_00434670` (124 B). Written as `__declspec(naked)` C++ methods with hand-rolled inline `__asm` that reproduces the MSVC8 EXCEPT_REGISTRATION frame instruction-by-instruction.
    * **Multiple-inheritance support symbols:** the four vftable statics declared inside the class via a new `// !MEMBERS BEGIN/END` block and defined in the unit's `// !PROLOGUE`.
    * **Refcount helper:** `FUN_00434610` (41 B) -- naked, ecx-only entry; emits `ret` (not `ret 4`) so the `__fastcall(int)` convention from `mapping.csv` is honoured.
    * **Six 8-byte adjustor thunks** (`FUN_004345d0..f0` and `FUN_00434640..60`): naked `sub ecx, N` + tail-`jmp` into the canonical dtor / release helper.
  * **Within 2 bytes of exact:** `FUN_004343a0` (ReadFromStream, 85 B -- **97.1 %**). The single irreducible mismatch is the `xor al, al` epilogue that MSVC8 emits for `return 0;` in a `uchar`-returning function.
  * **Within 1 byte of exact:** `FUN_004349a0` (scalar-deleting dtor, 30 B -- **99.5 %**). All bytes match except the REL32 target name of the `call free` site, which the target COFF writes as `Runtime::MSVCRT::_free_00447392` while our source references the bare `Runtime::MSVCRT::_free` per current `mapping.csv`.
  * **Infrastructure landed in service of this unit:** `scripts/sync_units.py` now preserves a `// !PROLOGUE` block in headers and a new `// !MEMBERS BEGIN/END` block inside each class body.
  * **Per-unit aggregate:** `bulanci/CDSResourceSign` advanced from a uniformly-stubbed baseline through 38.87 % to **99.52 %** byte match, with 13 / 15 functions at 100 %.
* **Artefacts:** `./sign_record.md`

### Localised text

* **Classes:** `CDSStaticTexts`, script export `GetInfo(language)`
* **Bytes:** <100 B (16-byte singleton + 508-byte pool table)
* **Confidence:** **Verified**
* **What's known:**
  * `CDSStaticTexts` is a 16-byte holder with two vftables (3+4 stub slots, no real "GetText" method) plus a `wchar_t** pool, int32 count` pair. The shipped binary has exactly **one** populated instance: a compile-time singleton in `.data` at `0x004afbb4` whose pool slot (`0x004afbbc`) points at a 127-entry `wchar_t*[]` table at `0x004af9b8`. Pool indices 0..111 hold the Czech UI (error messages, dialogs, key names, credits); 112..126 hold a small English "iwannaplay.com" overlay used by the trial/registration dialogs (which carry direct Czech↔English pairs 10 indices apart for the game-setup widget). UI widgets read the pool by hard-coded compile-time index, e.g. `*(wchar_t**)(PTR_PTR_004afbbc + 0xc8)` for `"Typ hry:"`; ~20 dialog functions reference `0x004af9b8` directly.
  * The class is also registered with the global class registry as classId **8** via `HandleClassRegister(0x0042e910)` so it can implement `IDSTexts` in the type system, but no resource in the master pack uses that classId, so the factory at `0x0042eeb0` is never invoked at runtime.
  * The script-side `language = 1 = Czech` selector documented in `script_lifecycle.md` is **orthogonal** to this pool — it is hard-coded at all three `CallExport(slot=0)` call-sites and only affects per-script bytecode `StrConst` choices, not the static text.
* **Open questions:** None for the shipped Czech build. (Localising to another language requires relinking the .exe; the binary has no runtime pool-swap path.)
* **Artefacts:** `./static_texts.md`, `./static_texts.py`, `./static_texts.json`
