# Subsystem understanding tracker

Confidence levels:

* **Empty** — not yet investigated.
* **Sketched** — high-level shape only; details TBD.
* **Partial** — most of the surface mapped; specific corners uncertain.
* **Verified** — Ghidra-cross-checked: layouts, dispatch tables and
  callsites are tied to specific addresses in `bulanci.exe`.

A subsystem at **Verified** is ready to drive the match track — see
`README.md` ("Handoff to the match track").

Class → module breakdown lives in `docs/MODULES.md`; class sizes
below are sourced from it.

Entries are grouped by leverage for matching (biggest formats / state
machines first).

Each entry uses the same fields:

* **Classes** — engine classes covered.
* **Bytes** — total code size (from `docs/MODULES.md`).
* **Confidence** — see scale above.
* **What's known** — concrete findings; addresses where applicable.
* **Open questions** — what still blocks promotion to **Verified**.
* **Artefacts** — `.md` / `.py` files produced for this entry.

---

## Already analysed

### Script VM

* **Classes:** `CDSScript`, `CLevelScript`
* **Bytes:** ~0.6 KB
* **Confidence:** **Verified**
* **What's known:**
  * 103-opcode dispatch table at `this+0x2c` built from
    `0x004b0118` (base 45) + `0x004af018` (ext 58).
  * Dispatcher `FUN_004384c0`.
  * Interpreter loop `FUN_00438b30`.
  * Public entry `FUN_00438c40`.
  * All 13 export indices mapped to engine callsites with argc and
    semantic names.
* **Open questions:**
  * Per-opcode operand decoding for ops without a clean editor
    counterpart.
  * Full stack-effect table.
* **Artefacts:** `script_lifecycle.md`, `script_dispatch_table.md`

### Resource container (`.eap` / `.eapres` / overlay)

* **Classes:** (unpacker only)
* **Bytes:** —
* **Confidence:** **Verified**
* **What's known:** Format reverse-engineered from editor source
  (`GZipStream.cs`, `Level.cs`, `ResourceItem.cs`). Working unpacker.
* **Open questions:** None for the container itself; per-`ClassID`
  payloads are tracked under their own entries.
* **Artefacts:** `tools/bulanci_unpack/README.md`,
  `tools/bulanci_unpack/bulanci_unpack.py`

### Network (DirectPlay)

* **Classes:** `CDSDirectPlay`, `CDSDirectPlaySender`
* **Bytes:** 0.26 KB
* **Confidence:** **Verified**
* **What's known:**
  * DirectX 7 `IDirectPlay4A` + `IDirectPlayLobby3A` (CoCreated via
    `CLSID_DirectPlay` / `CLSID_DirectPlayLobby`).
  * Transports: TCP/IP (`DPSPGUID_TCPIP`) and IPX (`DPSPGUID_IPX`).
  * App GUID `{739ECD1D-7DDC-4116-8444-38F677CB329F}` @ `0x00481a50`.
  * Object layout (`CDSDirectPlay` @ `0x8c` bytes) and embedded
    `CDSDirectPlaySender` (queue + worker thread draining via
    `IDirectPlay4::Send` vtbl[26] @ +0x68 with `DPSEND_GUARANTEED`)
    mapped end-to-end.
  * Lifecycle: `CoCreateInstance` → `EnumConnections` →
    `InitializeConnection` → `Open(CREATE|JOIN)` → `CreatePlayer` →
    `Sender::Bind`.
  * Host migration flag set on host
    (`DPSESSION_MIGRATEHOST|KEEPALIVE|DIRECTPLAYPROTOCOL = 0x2044`).
* **Open questions:** Engine-level host migration is not wired (the
  `DPSYS_HOST = 0x000D` system msg is not handled in the dispatcher).
* **Artefacts:** `net_protocol.md`

### Net dispatcher

* **Classes:** `CGame::ProcessNetMessage` (`FUN_00415290`, **not**
  `FUN_004185a0`)
* **Bytes:** —
* **Confidence:** **Verified**
* **What's known:**
  * Full first-byte switch over msg types `0x00..0x19, 0x64` mapped to
    engine handlers and to each of the 27 `CDSDirectPlay::Send`
    call-sites (25 unique type bytes).
  * Recording playback (`CGaming::FUN_00415f80`) and live receive
    (`CGame::FUN_00416030` case 0 via `CDSDirectPlay::Receive`
    `FUN_0043ab70`, vtbl[25] @ +0x64) both funnel through this entry
    point.
  * `FUN_004185a0` is the case-`0x15` body that pushes the payload
    into `cgame+0x1ac` (`CChainedStrm`) and fires script export #9.
  * System-msg pseudo-player (`idFrom==0`) cases 5/0x31/0x101
    documented.
  * Replication model is event-only (no per-tick snapshots):
    * player state via msg `0x0d`
    * projectiles via `0x0f`
    * hits via `0x10`
    * damage via `0x0c`
    * death via `0x11`
    * item placement via `0x18`
    * round timing via `0x12` / `0x16` / `0x17`
* **Open questions:** None at this level. The bodies of
  `FUN_0041d280`, `FUN_0041f010`, `FUN_0041f210`, `FUN_004209f0`,
  `FUN_00420510`, `FUN_00420a70` (per-msg handlers inside `CGaming`)
  still need their field-layout passes if/when we match `CGaming`
  byte-exactly.
* **Artefacts:** `net_protocol.md`

### BitmapSprite (`ClassID 52`)

* **Classes:** `CDSBitmap`, `CBulPicture`, `CDSFlxFile`
* **Bytes:** 70 B (header) + payload
* **Confidence:** **Verified**
* **What's known:**
  * Full 0x2c-byte header + post-header animation-frame stream +
    per-opcode pixel decoders all recovered.
  * Frames carry inner chunks dispatched by opcode at
    `CDSFlxFile::DecodeFrame @ 0x00432c60`; the unpacker ports the
    RLE (0x00/0x0e), delta (0x04/0x0f), palette (0x09) and
    transparent-index (0x0d) decoders verbatim (sources:
    `FUN_00432740`, `FUN_00432780`, `FUN_00432800`).
  * Empirically `fields[4]` is the bitmap width and `fields[5]` is
    the bitmap height (verified by `bw*bh == |decoded RLE|` on all
    538 keyframe chunks).
  * 130/130 master-pack samples now emit a horizontal RGBA atlas PNG
    plus a JSON sidecar describing frame size, count, ring-frame
    index and per-frame metadata (engine opcodes applied,
    NotifyMove origin, duration).
* **Open questions:**
  * Palette inheritance for the ~41 recolour-variant sprites that
    don't carry an inline palette (currently falls back to a
    grayscale ramp).
  * Semantics of `fields[3]` (the "width" header field, which
    doesn't match the bitmap width).
* **Artefacts:** `sprite_container.md`, `flx_file_format.md`,
  `inspect_sprite_headers.py`, `check_sprite_manifest.py`,
  `dump_chunk_bodies.py`, `proto_decoder.py`

---

## File / payload formats (engine-side)

These unlock matching for whole class clusters once the on-disk
schema is known.

### MPx audio stream (not video — see write-up)

* **Classes:** `CDSMpx`, `CDSMpxStream`, `CDSMpxDecoder`,
  `CDSVideoPlayer`
* **Bytes:** 14.4 KB (+ 3.3 KB of per-layer decoders adjacent in
  `Engine.DS.CDSMpx`)
* **Confidence:** **Verified** — codec, library, frame layout, CRC,
  per-layer decoder, and player wiring all locked. Library source
  identified bit-for-bit as **libmad 0.15.1b** (`ref/libmad-0.15.1b/`,
  Underbit Technologies, Jan 2004). Only an actual `.mpx` byte-stream
  from a retail install is still missing for end-to-end on-disk
  validation, but the codec is fully understood.
* **What's known:**
  * `CDSMpx` is **libmad 0.15.1b** wrapped in a thin `CDSMpx::` class
    shell: a full **MPEG-1 / MPEG-2 / MPEG-2.5 Layer I/II/III audio
    decoder** (i.e. MP1+MP2+MP3). Same situation as
    `CDSJpegImage.cpp` re-using verbatim libjpeg-6b. Library identity
    established by seven independent cross-checks (see
    `mpx_audio_format.md` §9.2):
      1. **Q3.28 fixed-point format**: libmad's `MAD_F_FRACBITS = 28`,
         `MAD_F_ONE = 0x10000000`. Matches every scalefactor and
         cosine constant in the bulanci binary.
      2. **CRC-16 lookup table bit-identical** to libmad
         `bit.c::crc_table[256]` at `g_awMadCrcTable` (`0x0048b938`).
         First 8 entries `{0x0000, 0x8005, 0x800f, 0x000a, 0x801b,
         0x001e, 0x0014, 0x8011}` -- bytes match verbatim.
      3. **CRC compute algorithm match**: `mad_bit_crc` (`0x004597b0`)
         has the same 32-bit-chunked + `switch(len/8)` 3/2/1
         fall-through + bit-tail structure as libmad's
         `mad_bit_crc()` in `bit.c:197`.
      4. **Scalefactor table bit-identical** to libmad
         `layer12.c::sf_table[64]` (from `sf_table.dat`) at
         `g_anMadSfTable` (`0x0049c210`). 8/64 entries verified
         byte-for-byte starting with `0x20000000, 0x1965fea5,
         0x1428a2fa, 0x10000000, 0x0cb2ff53, ...`.
      5. **Layer I linear table bit-identical** to libmad
         `layer12.c::linear_table[14]` at `g_anMadLinearTable`
         (`0x0049c310`). All 14 entries verified
         (`0x15555555 … 0x10002000`).
      6. **Layer I sample requantize**: `mad_layer_I_sample`
         (`0x0045c480`) is a byte-for-byte semantic match to libmad's
         `I_sample()` in `layer12.c:75` including the
         `&linear_table - 2` indexing trick that makes `[nb]` reach
         `linear_table[nb-2]`.
      7. **DCT32 cosine constants** in `g_anMadDct32Costab`
         (`0x0048af70`) are libmad's `synth.c::costab1..31` baked at
         compile time (e.g. `0x0b504f33 ≈ cos(π/4)`, `0x0ec835e8 ≈
         cos(π/8)`, `0x0fb14be8 ≈ cos(π/16)`).
  * 32-point polyphase synthesis at `mad_synth_frame_dct32_full`
    (`0x00456910`, libmad `synth.c::dct32`); stereo / mono outer
    loops `DecodeGranuleStereo` (`0x00457aa0`) / `DecodeGranuleMono`
    (`0x00458450`); final `Q3.28 → s16` clamp at `QuantizeQ31ToS16`
    (`0x00446670`) implementing libmad's SSO-path output scaling
    (`>> 13` ≈ libmad's `SHIFT(x) = ((x + (1<<11)) >> 12)` followed
    by the final `>> 1` round-to-even). L2 requantisation is libmad
    `II_samples()` at `mad_layer_II_samples` (`0x0045c800`), with
    the bulanci shift `0x1d - nb = 29 - nb` matching libmad's
    `MAD_F_FRACBITS - (nb - 1)` exactly. No precomputed L2 lookup
    exists -- the 256 KiB table at `0x004b84f8` earlier misattributed
    as an L2 dequant table was actually the **2D alpha-blend LUT**
    initialised by `CBulanci::InitAlphaBlendLut` (corrected).
  * **Library bisect:** LAME 3.92 `mpglib` (`ref/lame392/mpglib/`)
    **falsified** (float vs Q3.28, no CRC verification, different L2
    strategy, runtime cos() instead of pre-baked tables). libmad
    0.15.1b (`ref/libmad-0.15.1b/`) **confirmed** with seven
    bit-identical fingerprints (see above and
    `mpx_audio_format.md` §9.2). Likely build flags:
    `FPM_DEFAULT|FPM_64BIT + OPT_SSO + !OPT_DCTO`.
  * On-disk `.mpx` framing: **16-byte stripped `WAVEFORMAT`** then a
    raw MPEG audio bitstream
    (`{u32 dataSize; u16 ch; u16 bits; u32 rate; u32 avgBytesPerSec;}`),
    derived from `CDSMpxStream::{SaveMpxFile, LoadMpxFile}`
    (`0x00432eb0` / `0x00433180`) and the duration calc at
    `ComputeDurationMs` (`0x0043a000`). Struct pushed into Ghidra as
    `MpxFileHeader`.
  * Player chain: `CDSMpxStream → CDSMpx → CDSMpxDecoder (IDSAudioSource)
    → CDSAudioPlayer → DirectSound`. Consumed by `CDSAudioVideoPlayer`
    (alongside a separate video resource) for `CMovieView`.
  * `CDSVideoPlayer` is **misnamed** — it is a generic sorted-array
    multi-track media-source manager (4 funcs / 84 B), reused by the
    audio side of `CDSAudioVideoPlayer` at offset `+0x08`. Renamed
    funcs: `ConstructTrackManager`, `InsertOrFindTrack`,
    `AddTrackSource`, `SetCurrentTrack`, `BeginCurrentTrackPlayback`.
  * No `BitmapJpegAnim`/`MpxAudio`-class resources ship in the master
    pack overlay; `ResolveResource` (`0x00446b90`) gates re-loading on
    `(ClassID != 0x27) && (g_pDirectSoundSingleton->vt != null)` —
    i.e. script-VM stream sources skip the resource re-lookup, and
    DirectSound-less hosts skip MPx-resource resolution entirely.
* **Open questions:**
  * Locate an actual `.mpx` file (likely loose next to `bulanci.exe` in
    a full retail install) and round-trip the 16-byte header through
    `_scratch_mpx_header.py` (now committed in
    `tools/bulanci_unpack/ghidra_analysis/`). No sample available in
    this tree.
  * Confirm `wBitsPerSample` as a 16-bit field vs. a packed codec hint.
  * Pin down the libmad build flags via `objdiff` once libmad is wired
    into the project build (current best guess:
    `FPM_DEFAULT|FPM_64BIT + OPT_SSO + !OPT_DCTO`).
  * `flags & 0x02` in `DispatchLayerDecoder` is now traced to a sticky
    config bit copied from `bitstreamCtx[0xe]` (byte 56) into
    `frameInfo[0xb]` (byte 44) at the top of `ReadOneFrame` — i.e. it
    is **not** a per-frame header bit, it is one-shot decoder
    configuration. Combined with the halved sample-rate / halved frame
    size and the `DecodeGranuleMono` switch, this strongly indicates a
    "force-mono / downmix" path (probably hooked into the
    `IDSAudioSource::SetChannels(1)` analogue). Still wants the
    bitstream-side setter located for final confirmation.
* **Artefacts:** `mpx_audio_format.md`, `_scratch_mpx_header.py`

### DSM multi-stream container

* **Classes:** `CDSDsmFile`
* **Bytes:** 2.1 KB
* **Confidence:** **Verified**
* **What's known:**
  * `CDSDsmFile` is the engine-side reader for **ClassID 76 =
    `BitmapJpegAnim`** — the master pack's synchronized **MJPEG
    video + 16-bit PCM audio** movies.
  * 36-byte `CDsmHeader` (struct defined in Ghidra via
    `create_struct`) fully decoded:
    * `dwPayloadEndOffset` (file size + loop sentinel)
    * `dwCanvasWidth` / `dwCanvasHeight` / `dwPixelFormat`
      (CDSImage 5 = 24bpp BGR, 6 = 32bpp BGRA)
    * `dwDurationMs`
    * `dwFrameCount`
    * `dwAudioByteCount`
    * `dwAudioFormatPacked` (`(bits<<16)|channels` — `0x00100001` =
      16-bit mono)
    * `dwAudioSampleRate` (22050 / 44100 Hz)
  * Followed by `2 * dwFrameCount` interleaved `{u32 len, byte[len]}`
    chunks: chunk `2k` is a complete JPEG
    (`FF D8 FF DB ... FF D9`), chunk `2k+1` is the matching frame's
    raw little-endian PCM.
  * Cross-verified on all four shipping resources in
    `unpacked/overlay/` — the computed audio bytes from
    `dwDurationMs * rate * channels * bits / 8 / 1000` matches the
    on-disk `dwAudioByteCount` within ≤ 2 bytes (sample alignment)
    for every file, and `dwPayloadEndOffset == sizeof(file)` exactly.
  * Runtime side: `N` concurrent reader/writer handles, each with
    its own 1 MiB `CDSMemQueue` ring buffer, tracked by the embedded
    handle bank at `obj+0x54`.
  * JPEG decode happens in `HandleRecordRead` (`0x00428c40`,
    vtable2 slot 7) via the engine's reusable `FUN_00431b70`
    (libjpeg-style with per-row R↔B swap).
  * 31/31 funcs accounted for; 23 renamed in Ghidra. Plate comments
    and inline disassembly comments now annotate the three `Read`
    calls and the `Tell64` call inside `HandleOpenStream`.
  * The unrelated `BULANCI.TMP` (ClassID 58 `DsmInner`) is just a
    name collision — it's a `MZ`-prefixed PE file.
* **Open questions:**
  * Whether the writer-side handle path (vtable2 slot 5 +
    `HandleQueueWrite`) is ever exercised at runtime, or if it
    exists only for tooling.
  * Mapping the four shipping animations
    (resources 77825 / 77827 / 77829 / 77831) to the gameplay events
    that trigger them (search scripts for those IDs).
* **Artefacts:** `dsm_file_format.md`, `_scratch_dsm_header.py`,
  `_scratch_dsm_payload.py`

### FLX animation file

* **Classes:** `CDSFlxFile`
* **Bytes:** 1.7 KB
* **Confidence:** **Verified**
* **What's known:**
  * **Bulanci-original variant** (not stock FLI/FLC).
  * 36-byte file header, 5-byte frame header
    (`u32 size; u8 chunkCount;`), 5-byte inner-chunk header
    (`u32 size; u8 opcode;`).
  * Inner-chunk dispatcher `DecodeFrame` @ `0x00432c60` switches on
    the dense opcode set `{0,4,8,9,10,11,12,13,14,15}` — no overlap
    with stock FLI/FLC IDs.
  * Per-opcode decoders (`FUN_00432740` RLE, `FUN_00432780` delta,
    `FUN_004327e0` memcpy, `FUN_00432800` palette) all ported into
    `bulanci_unpack.py` and round-tripping cleanly on 130/130
    BitmapSprite payloads in the master pack.
  * BRUN/LC decoders are FLI-inspired (same opcode polarity) but
    with a long-run u16 escape on BRUN.
  * Parallel mask plane (opcodes 0x0e/0x0f) is supported by the
    runtime; the master pack uses 0x0e but never 0x0f.
  * The container that wraps a FLX payload is `BitmapSprite`
    (ClassID 52); see `sprite_container.md`.
* **Open questions:** None at the FLX layer. Higher-layer questions
  (recolour-variant palette inheritance, `fields[3]` "width"
  semantics) tracked under the BitmapSprite entry.
* **Artefacts:** `flx_file_format.md`, `proto_decoder.py`

### JPEG codec

* **Classes:** `CDSJpegImage`
* **Bytes:** 7.4 KB (`CDSJpegImage` namespace) + ~several KB more in
  `_Globals` (decompressor internals Ghidra hasn't grouped yet)
* **Confidence:** **Partial** (source-attribution complete; per-func
  COFF-exact matching not yet done)
* **What's known:**
  * **Verbatim libjpeg-6b** (Independent JPEG Group reference,
    `JVERSION = "6b  27-Mar-1998"`). Reference sources committed at
    `ref/libjpeg6b/`.
  * Five mutually-independent fingerprints pin the release:
    * `JPEG_LIB_VERSION = 62` (`0x3e`) in the create-API guards at
      `FUN_0045e6a0` / `FUN_0045ecc0`.
    * `sizeof(jpeg_decompress_struct) = 432` and
      `sizeof(jpeg_compress_struct) = 360` from the same guards.
    * `const int jpeg_natural_order[DCTSIZE2+16]` at `0x0049db50`
      byte-for-byte (320 B, including the v6 "extra 16 × 63" safety
      pad).
    * `DSTATE_START..DSTATE_STOPPING` (200..210) and
      `CSTATE_START..CSTATE_WRCOEFS` (100..103) verbatim across the
      API entries.
    * Every observed `JERR_*` numeric code matches the v6b
      alphabetical enum slot (`0xc = JERR_BAD_LIB_VERSION`, `0x15 =
      JERR_BAD_STRUCT_SIZE`, `0x33 = JERR_NO_IMAGE`, etc.) — see the
      full table in `jpeg_decoder.md`.
  * The class ships **both** the decoder (used by
    `CDSJpegImage::DecompressToImage @ 0x00431b70` and by
    `CDSDsmFile::HandleRecordRead` for MJPEG frames) and the encoder
    (used by `CDSJpegImage::CompressFromImage @ 0x00431e50` and
    presumably by the `BitmapJpegAnim` writer in tooling).
  * Project-specific code is just the 12-function `CDSJpegImage`
    wrapper class at `0x00431510..0x004320ae` (787 B), which adapts a
    `CDSStreamStorage` to libjpeg's `jpeg_source_mgr` /
    `jpeg_destination_mgr` (`INPUT_BUF_SIZE = 4096` matches
    `jdatasrc.c`) and per-row swaps R↔B so the engine's 24bpp BGR
    `CDSImage` (pixel-format 5) lines up with libjpeg's RGB output.
  * IJG message strings (`"JPEG ..."`, `"Bogus ..."`, etc.) are
    absent — the engine plugs a custom error manager that calls
    `error_exit` with the bare `msg_code`, so the message-table
    linker-strips. This is the only deliberate modification.
  * 10 top-level API entry points already mapped to their IJG source
    file (`jdapimin.c`, `jcapimin.c`, `jdapistd.c`, `jcapistd.c`,
    `jcomapi.c`) plus 25+ internals (`jdmarker.c`, `jdinput.c`,
    `jcparam.c`, `jcmaster.c`, `jccolor.c`, `jutils.c`) — see the
    address tables in `jpeg_decoder.md`.
* **Open questions:**
  * Which `dct_method` the default config selects
    (`JDCT_ISLOW`/`JDCT_IFAST`/`JDCT_FLOAT`). Stock `JDCT_DEFAULT` is
    `JDCT_ISLOW`, so `jidctint.c` is the live path; the other two are
    dead-but-linked code. Verifying this lets us drop the dead IDCT
    sources from the `third_party` build to avoid bloat.
  * Per-function COFF-exact addresses for the 50+ unmapped IJG
    internals in the `0x46xxxx` block (encoder DCT / Huffman /
    sampler / marker writer) and the `0x45D000..0x45EBFF` block
    (decoder coef-ct / main-ct / post-ct / IDCT-mgr / Huffman /
    upsampler / colour deconverter). Pure source-line accounting,
    not investigation.
* **Artefacts:** `jpeg_decoder.md`, `ref/libjpeg6b/`

### BMP decoder

* **Classes:** `CDSBmpImage`
* **Bytes:** 1.5 KB
* **Confidence:** **Verified**
* **What's known:**
  * **Bespoke code, not a library port.** No libbmp / FreeImage /
    DevIL / stb_image fingerprints; only BMP-related string in the
    binary is the RTTI type-descriptor `.?AVCDSBmpImage@@` at
    `0x004afde0`, shared by all five sub-object `RTTICompleteObject-
    Locator`s. I/O is engine-native through `CDSStreamStorage`
    virtual slots (read `+0x10`, write `+0x14`, tell `+0x20`,
    seek `+0x28`); errors are routed through the engine-wide
    `CDSSimpleException(9, 0xB)` (`FUN_00434c20`).
  * **Format accepted/emitted is bare-bones Microsoft DIB v3:** 14-byte
    `BITMAPFILEHEADER` + 40-byte `BITMAPINFOHEADER`, bottom-up rows.
    `bfOffBits` is honoured on read; V4/V5 headers are silently
    truncated to 40 bytes (any trailing bytes are skipped by the
    `bfOffBits` seek).
  * `Load` `FUN_004320c0` — magic `0x4D42` check, 40-byte info-header
    read, **rejects any `biCompression != 0`** (so RLE4/RLE8/BITFIELDS
    are *not* supported — *answers the open question*). Reads palette
    for `biBitCount ≤ 8`, then bottom-up rows. 16 bpp is taken as DIB
    5-5-5 and in-place expanded to engine 5-6-5 (mask `0xfc1f`, green
    shifted up by 1; new LSB zeroed).
  * `Save` `FUN_00432440` — two adaptive guards then a single write
    path. Outer guard: if image stride is not DWORD-aligned, build a
    fresh `CDSBmpImage` at the *same* format, copy-blit, recurse.
    Inner guard: if format-index is 6 (32 bpp), build a fresh
    `CDSBmpImage` at format-index 5 (24 bpp), copy-blit, recurse.
    Main path writes 14+40 bytes of header, optional palette, then
    rows; for format-index 4 (engine 5-6-5) it packs each row to DIB
    5-5-5 (mask `0xf81f`, green shifted down by 1) through a malloc'd
    scratch buffer (`FUN_00425130`).
  * `FUN_00435c70` is the `biBitCount → engine format index` switch
    (`1→0, 2→1, 4→2, 8→3, 16→4, 24→5, 32→6, else→7`); BPP and
    default-palette-entries tables live at `DAT_004b0050` /
    `DAT_004b0030`. Format-index 4 is **the only place** where the
    engine's 5-6-5 16 bpp layout differs from DIB 5-5-5; the two
    `Load`/`Save` conversions are exact symmetric inverses.
  * 12 functions accounted for, all renamed in Ghidra:
    `Load` (`004320c0`), `Save` (`00432440`), ctor (`00432330`),
    scalar-deleting dtor (`00432700`) + four base-adjuster thunks
    (`004322a0`/`b0`/`c0`/`e0`), `__getClassData` (`00432290`),
    row-scratch realloc (`00425130`), `ValidateStride` (`00436020`)
    and `FillBitmapInfoHeader` (`00436060`) — the last two are BMP-
    only helpers called from `Save`.
  * Five sub-object vtables in `.rdata` at `0x004871f0..0x00487254`;
    primary vtable @ `0x0048724c`, `Load`/`Save` slots @
    `0x00487218`/`0x0048721c` (the `+0x54` sub-object vtable). The
    JPEG decoder next door uses the same vtable shape, so the
    `Load`/`Save` slot offsets carry over.
* **Open questions:** None at this layer.
* **Match track:** 3 / 12 functions matched in `src/bulanci/CDSBmpImage.cpp`
  (`GetClassData` 100 % / `ValidateStride` 100 % / `FillBitmapInfoHeader`
  ~56 %), section score on `bulanci/CDSBmpImage` at ~10.8 %. The four
  scalar-deleting dtor thunks are compiler-generated MI displacement
  adjusters and need the class layout declared before they fall out
  for free; the remaining helpers (`AllocRowScratch`,
  `scalar_deleting_dtor`) and the big three (`ctor`, `LoadDibStream`,
  `SaveDibStream`) are blocked on project-wide convention fixes
  (`static __stdcall` / `static __fastcall` markers on the `_Globals`
  and `CDSObject` helpers they call, plus the five sub-object vftable
  symbols for the ctor). See `bmp_decoder.md` for the reconstructed C.
* **Artefacts:** `bmp_decoder.md`

### Stream hierarchy

* **Classes:** `IDSStream` (interface) + `CDSFilterStream`,
  `CDSFileStream`, `CDSEasyMemStream`, `CDSQueueStream`,
  `CDSSafeStream`, `CDSGZipStream`. (`CDSGZipStreamData`,
  `CDSSafeStreamInfo` and `CDSMemQueue` are companion data/state
  classes held *by* a stream — they don't share the vtable contract.)
* **Bytes:** 7.6 KB
* **Confidence:** **Verified**
* **What's known:**
  * `IDSStream`'s vtable owns **15 virtual slots**, not "~6". The
    byte-IO contract is slots 4..12, with lifetime housekeeping at
    0..3 and identity/teardown at 13..14:
    `Read / Write / Flush / GetSize / Tell / SetSize / Seek / Lock /
    Unlock / GetName / Close` (slot 10 `Seek` takes `(off_lo, off_hi,
    origin)` with origin `0=BEGIN / 1=CURRENT / 2=END`).
  * Slot ordering pinned by RTTI walk on **five** concrete classes
    (`CDSFileStream` @ vtbl `0x0047f72c`, `CDSEasyMemStream` @
    `0x004803dc`, `CDSGZipStream` @ `0x00480594`, `CDSFilterStream` @
    `0x004870fc`, `CDSQueueStream` @ `0x00486dac`) and double-checked
    against the raw vtable offsets used by `CDSFlxFile::BindStream`
    (`0x00432ac0`) and `CDSFlxFile::CloseStream` (`0x00432b60`).
  * Engine-side errno table (`CDSStreamException`,
    `.?AVCDSStreamException@@` @ `0x004afd6c`): `1=read 2=write
    3=seek 4=setsize 5=lock 6=unlock 7=flush 8=uninitialised`.
  * **Naming correction.** The original entry treated
    `CDSStreamStorage` as the byte-stream base. It is not.
    `CDSStreamStorage` inherits from `IDSStorage` and is a *container
    of streams* (its 9-slot IDSStorage vtable is array-indexed
    getters/setters, not byte-IO); the actual byte-stream interface is
    `IDSStream` (`.?AVIDSStream@@` @ `0x004ae2a4`).
* **Open questions:** None at this level. The 4-slot `IDSReferenced`
  subobject vtable (lifetime / dtor / scalar-deleting-dtor) hasn't
  been split open; not on the critical path.
* **Artefacts:** `stream_hierarchy.md`

### GZip compression

* **Classes:** `CDSGZipStream`, `CDSGZipStreamData`
* **Bytes:** 1.3 KB
* **Confidence:** **Verified**
* **What's known:**
  * Container is a custom seekable wrapper, **not** RFC 1952 gzip.
    The 4-byte file magic `1346984519u = 0x50495A47` spells `"GZIP"`
    in ASCII; per-block payload is the **zlib (RFC 1950) wrapper** --
    `windowBits = +15`, `memLevel = 8`, `strategy = Z_DEFAULT_STRATEGY`,
    `level = 9 (best)`, single-shot `Z_FINISH`. Not raw deflate, not
    RFC 1952 gzip.
  * **zlib 1.1.3** is statically linked. Version string `"1.1.3"` at
    `0x00487594` (`g_szZlibVersion_1_1_3`); full zlib 1.1.x `z_errmsg`
    table at `0x0049e000..0x0049ed00`; `sizeof(z_stream) = 0x38` (the
    value passed to `inflateInit_`/`deflateInit_`) pins the layout to
    the 1.1.x series.
  * `inflateInit_` (`0x0046ef60`) and `deflateInit_` (`0x004704c0`)
    are stock zlib trampolines that lock the call to
    `inflateInit2_(..., 15, ...)` and
    `deflateInit2_(..., 9, 8, 15, 8, 0, ...)` -- i.e. the zlib
    wrapper with the default window/memLevel/strategy and `level = 9`.
    The body of `deflateInit2_` at `0x004702c0` enforces exactly the
    1.1.x argument-validation rules
    (method == `Z_DEFLATED`, windowBits in `[8,15]`, memLevel in
    `[1,9]`, strategy in `[0,2]`, stream_size == `0x38`).
  * Block size **`0x8000` = 32 KB**; decompress scratch
    **`0x8040`** (block + 64 B slack); compress scratch
    **`0xA00C`** (`>= deflateBound(0x8000)`); both allocated by
    `CDSGZipStream::AllocateBuffers @ 0x00435670`.
    "Store raw if compressed >= 32 KB" rule enforced on both sides
    (boundary tested as `size == 32768`).
  * Engine class layout filed in `gzip_stream.md` (every named field
    pinned to a numeric offset by reading `Open`, `CloseInt`,
    `ReadBufferAt`, `WriteBuffer`, `AllocateBuffers`, and the inherited
    `Read` thunk at `0x00435220`). `CDSGZipStream` instance is
    >=`0x48` bytes; `CDSGZipStreamData` is exactly `0x20` bytes,
    refcounted, holds `m_lSize` / `m_nChunkCount` / `m_chunks[]`.
  * Method addresses (all renamed in Ghidra):
    `Open` (`0x004356e0`), `CloseInt` (`0x004354a0`),
    `ReadBufferAt` (`0x00435050`), `WriteBuffer` (`0x00435140`),
    `AllocateBuffers` (`0x00435670`),
    static `Compress` (`0x00434ee0`), static `Decompress`
    (`0x00434e30`).
* **Matching:** First matches landed in `src/bulanci/_Globals.cpp`:
  `inflateInit_` (`0x0046ef60`) and `deflateInit_` (`0x004704c0`) are
  **byte-exact** (26/26 and 37/37). `CDSGZipStream::Decompress`
  (`0x00434e30`) and `Compress` (`0x00434ee0`) match in length
  (170/170 and 172/172) with the same instructions and operand
  layout; the 33-byte diff is purely MSVC 8 register-allocator
  preference (`EBX` vs `EDI` for the `destLen` pointer) and a single
  block-layout swap.  Resolves to 0 once real zlib 1.1.3 bodies
  replace the six core stubs.
  Six zlib core helpers (`inflate`, `deflate`, `inflateInit2_`,
  `deflateInit2_`, `inflateEnd`, `deflateEnd`) carry layered opacity
  barriers so the matched callers compile correctly -- see the
  "Stub opacity barriers" section in `gzip_stream.md` before touching
  them.  Refresh status with
  `python scripts/internal/zlib_status.py`; per-function hex dump via
  `python scripts/internal/zlib_dump_pair.py <symbol>`.
* **Open questions:** Remaining `CDSGZipStream` public surface (`Read`,
  `Write`, `Seek`, `Flush`, `SetLength`, `Close`, ctors/dtor) is still
  under `FUN_xxxx` names -- not blocking matching, since the field
  layout is fixed and the per-method behaviour is the C# mirror.
* **Artefacts:** `gzip_stream.md`, `scripts/internal/zlib_status.py`,
  `scripts/internal/zlib_dump_pair.py`

### Animation runtime

* **Classes:** `CDSAnim`, `CAnim`, `CBulAnim` (+ pure `IDSAnim`
  interface @ RTTI `0x004ae8ac`)
* **Bytes:** 0.7 KB (engine classes only — most of the actual playback
  code lives in a shared 7-vtable shell + ~28 functions in the
  `0x004386f0..0x00439eb0` cluster that Ghidra splits across
  `CDSVideoPlayer` / `CDSAudioPlayer` / `CDSObject` namespaces)
* **Confidence:** **Partial**
* **What's known:**
  * **Inheritance.** RTTI confirms the ladder is
    `IDSInterface → IDSAnim → CDSAnim → CBulAnim`, with `CAnim` as a
    *sibling* concrete class (separate hierarchy, also implementing
    `IDSAnim` but at object offset `+0x98` instead of `+0x78`). Sizes:
    `CDSAnim` = 0xd0, `CBulAnim` = 0xd4 (extra team-index byte at
    `+0xd0`), `CAnim` = 0xf0.
  * **Seven-vftable layout.** Both `CDSAnim` and `CBulAnim` carry
    vftables at offsets `0x00 / 0x04 / 0x10 / 0x18 / 0x68 / 0x6c / 0x78`
    — the IDSImage / IDSChained / IDSEventHandler / IDSReferenced /
    IDSUpdated / **IDSAnim** / chain-thunk faces. `CAnim`'s 0x20-byte
    widget chrome shifts the last three slots to `0x88 / 0x8c / 0x98`.
    All COL records and per-slot dispatch tables are mapped in
    `anim_runtime.md`.
  * **Track manager** is an embedded `CDSObject` at `outer + 0x88`
    (init `CDSObject::ConstructTrackManager @ 0x00439c70`,
    teardown `0x00439d30`). It owns:
    * a tracks array (`+0x1c`, 8-byte entries
      `{u32 flags, CDSAnimSequence*}`),
    * the current track index (`+0x2c`),
    * current frame index (`+0x38`) that wraps at `seq[0x14]`,
    * a paused flag (`+0x35`), and
    * a `frameDelayOverrideMs` field (`+0x44`, `-1` = use sequence
      duration).
  * **Frame timing model — unit pinned to milliseconds.** The engine
    clock is `g_dwElapsedMs = timeGetTime() - g_dwStartMs` updated once
    per frame in `CDSApp_UpdateClock @ 0x0042e790`. Per-track timers live
    on the CDSObject scheduler (`Scheduler_DispatchDueEvents @ 0x0042eb30`,
    arm with `Scheduler_SetEventDelayMs @ 0x0042f2d0`, anchor with
    `Scheduler_SetEventLastFireMs @ 0x0042f290`): each slot fires when
    `g_dwElapsedMs >= lastFire + delay`, so **every `delay` value flowing
    through this system is real wall-clock milliseconds**. On each tick
    `TM_AdvanceFrame @ 0x004399b0` arms the next slot from the active
    sequence's declared duration via Bresenham-style integer division
    `delay = ((f+1)*seq[0x10])/seq[0x14] - (f*seq[0x10])/seq[0x14]` ms,
    so `seq[0x10]` is total duration in ms and `seq[0x14]` is frame
    count. A non-negative `+0x44` overrides this with a fixed per-frame
    delay (also ms).
  * **`frameDelayOverrideMs` (`+0x44`) is set ONLY by the construction-
    time speed formula** — there is exactly one writer in the whole
    binary (`TM_SetFrameDelayOverrideMs @ 0x00439720`, called from
    `CBulanek::FUN_0041e4b0`). Given a speed parameter (40..200, default
    100 == no override), it computes `delay_ms = round(47.25 / (speed /
    100.0)) = round(4725 / speed)` (constants pinned at
    `g_kSpeedFormulaNumerator_47p25 @ 0x00482908` and
    `g_kSpeedFormulaNeutral_100p0 @ 0x00482910`). That gives the
    sensible-game range: 24 ms / 41.7 fps at speed=200, 47 ms / 21 fps
    at speed=100 (formula skipped — keeps -1 so sequence default
    applies), 118 ms / 8.5 fps at speed=40.
  * **FLX opcode 0x0C is a side-channel notification, NOT a timing
    override.** The opcode-0x0C handler in `CDSFlxFile::DecodeFrame`
    calls `BroadcastFrameTimeHint @ 0x00436ef0` (renamed; previously
    `_Globals::NotifyFrameTime`), which walks the **per-consumer**
    subscriber list at `consumer+0x38..consumer+0x40` and dispatches to
    each subscriber's vftable slot 4. Across the entire binary there is
    no path from this fan-out that writes the track manager's
    `frameDelayOverrideMs` — confirmed by an exhaustive instruction
    sweep for direct stores to `[*+0x44]` and a callgraph walk from
    `BroadcastFrameTimeHint`. The earlier inference that 0x0C drove
    playback cadence was wrong; in the 130-sprite master pack the
    actual values (mostly 0/1) corroborate this — they would be
    sub-perceptible as ms delays.
  * **Connection to `CBulPicture` / FLX.** Resources expose their
    animation face via `CheckedVirtualBaseCast(res, DAT_004b8370)`
    (distinct from the static-bitmap face at `DAT_004b826c`). On every
    tick the runtime calls `seq->vfn[7]((trackEntry, renderTarget))` —
    for a `CBulPicture` sequence this is the engine wrapper around
    `CDSFlxFile::DecodeFrame @ 0x00432c60` (RLE/delta/palette opcodes
    already documented under BitmapSprite). The reverse direction
    (sequence → runtime) is the 5-slot `IDSAnim` vtable at `obj+0x6c`,
    with slot 0 (`FUN_00438f20`) being the bulk-event dispatcher that
    forwards each 16-byte event record to `vbase->vfn[9]` (the script-
    facing `OnBitmapEvt` path).
  * **Per-tick blit** goes through `CPoemScroller::BlitDispatch`
    (`TM_TickBlit @ 0x00439080`), called against the engine singleton at
    `DAT_004b3b88 + 0x80` — same singleton used by `CDSDirectPlay` and
    the script VM.
  * **Construction templates** for both `CBulAnim` (`CMenu::FUN_004104f0`)
    and `CAnim` (`CBulanci::FUN_00411010`) recovered: alloc → base ctor
    `CDSAnim::FUN_00439560(this, x, y, NULL, 0)` → patch all 7 vftables
    → 4× `BindSequence(this+0x78, ...)` for the four facing/anim
    variants packaged in one BitmapSprite → `SetTrack` with a
    random starting frame and the team-tinted 256-entry palette
    remap.
* **Open questions:**
  * On-disk source of `seq[0x10]` (sequence-default duration in ms).
    The sequence pointer added to the track is `resource_handle + 4`,
    but a CBulPicture's `+0x14` field (the offset that becomes
    `seq[0x10]`) is uninitialized by `CBulPicture_Create` — it must be
    written later by the resource-pool wrapper that wraps a
    BitmapSprite blob into the 2-vftable sequence object returned by
    `(*g_pApp[0x70])->vfn[4](resId, 0)`. Not on disk in any natural
    alignment of the 0x2c-byte BitmapSprite header. **Likely
    candidates:** a hardcoded default (matches the 47.25 ms / 21 fps
    cadence the speed formula degenerates to at speed=100), or the
    resource-pool's per-resource type table.
  * The real purpose of FLX opcode 0x0C (broadcasts a u16 via
    `BroadcastFrameTimeHint @ 0x00436ef0` to per-consumer subscribers).
    Plausible roles: profiling/debug timing hints, script-VM hooks for
    cadence-aware behavior, or a deprecated codepath. The subscriber
    contract (vftable slot 4 with `(consumer, u16 ticks)` signature)
    needs a real listener mapped before its semantics can be confirmed.
  * Exact slot order of the 24-entry primary vftable (the IDSImage
    face) — slot 14 = Tick, slot 15 = Render are nailed; the rest
    falls out once the shared `CDSView`/IDSImage layout is solved.
  * Semantics of `+0x70` and `+0x74` in the IDSAnim subobject (used as
    a subscriber-identity cookie in `SetCurrentSequence`).
  * Layout of the 16-byte "user event" record that IDSAnim slot 0
    forwards to `vbase->vfn[9]` (probably `{kind, x, y, frame}` to
    match the script-side `OnBitmapEvt` signature).
* **Artefacts:** `anim_runtime.md`

### Editor colophon record (was: "Resource signing")

* **Classes:** `CDSResourceSign` (engine ClassID `0x5E` = 94 = `Sign`)
* **Bytes:** 0.4 KB
* **Confidence:** **Decoded**
* **What's known:** A serialisable record carrying
  `{ CDate publishDate, wstring content, wstring copyright, u8 flag }`,
  total `sizeof = 0x28`. Constructor at `0x00434540`, factory at
  `0x00434930`, read/write at `0x004343A0` / `0x00434310`. The 14
  vtable-reachable methods are 2 stream helpers + read + write + ctor
  + class-meta getter + dtor body + scalar-deleting dtor + 3+3
  adjustor thunks for the dtor / Release on the four sub-objects. The
  4 vtables match the RTTI Class Hierarchy Descriptor at `0x004A4518`
  (8 base classes, sub-object offsets `0 / 4 / 8 / 16`). The class is
  registered at static-init by `0x0047D1D0..0x0047D260` and is only
  reachable through `InitializeByClassId(94, stream)`. **Despite the
  name, there is no cryptography here**: no hash, no key, no cipher,
  no MAC. It's a "signed by the author" colophon, not a crypto
  signature.
* **Open questions:** Whether Editor.exe ever sets `flagByte` to
  anything other than 0 (the one shipping instance has `0x00`); what
  triggers the post-v1 conditional read branch in `ReadFromStream`.
* **Artefacts:** `sign_record.md`; the single shipping instance
  `unpacked/tutorial_eap/res_0000100000_94_Sign.bin`
  (`2006-08-26 / "Tutorial 1" / "SleepTeam Labs"`).
* **Source-side matching status (`src/bulanci/CDSResourceSign.cpp`)**:
  * **Byte-exact:** `FUN_0042e680` (ReadDateField, 19 B),
    `FUN_0042e6a0` (WriteDateField, 19 B),
    `FUN_00434310` (WriteToStream, 55 B),
    `FUN_004345c0` (GetClassRegistry, 6 B). The two date helpers use
    `__declspec(noinline)` to inhibit MSVC8's tail-call inlining and
    keep the `ecx` / `edi` register allocation aligned with the
    target.
  * **Within 2 bytes of exact:** `FUN_004343a0` (ReadFromStream,
    85 B → 97.1 %). The single irreducible mismatch is the
    `xor al, al` epilogue that MSVC8 emits for `return 0;` in a
    `uchar`-returning function; removing it would require widening
    the mapping to `void`-returning, which falls outside the byte
    match contract.
  * **Cannot be matched at the source level** (documented in
    `STUB_BODY()` form):
    * `FUN_004345d0..f0` and `FUN_00434640..60` (six 8-byte
      adjustor thunks of the form `sub ecx, N; jmp <target>`).
      These are linker/compiler-generated stubs for the multiple-
      inheritance `Release` / `scalar deleting dtor` chains; they
      cannot be expressed in C++ source without inline assembly.
    * `FUN_00434540` (constructor, 126 B) and `FUN_00434670`
      (destructor body, 124 B) both carry MSVC8 `/GS` security
      cookie + SEH frames and reference four vtable symbols
      (`vftable_00487510 / 4f4 / 4dc / vftable`). Matching them
      requires both the `/GS` build flag and real vtable
      definitions which the project does not yet emit.
    * `FUN_00434610` (refcount/release helper, 41 B) and
      `FUN_004349a0` (scalar deleting dtor, 30 B): mapping declares
      them `__fastcall` but the synced header emits them as
      `__thiscall` members, so the call sites cannot be written in
      portable C++ without changing the mapping convention.
  * **Per-unit aggregate:** `bulanci/CDSResourceSign` advanced from
    a uniformly stubbed baseline to **38.87 %** byte match (per
    `objdiff-cli`), with 4 functions at 100 % and 1 at 97 %.

### Localised text

* **Classes:** `CDSStaticTexts`, script export `GetInfo(language)`
* **Bytes:** <100 B (16-byte singleton + 508-byte pool table)
* **Confidence:** **Verified**
* **What's known:** `CDSStaticTexts` is a 16-byte holder with two
  vftables (3+4 stub slots, no real "GetText" method) plus a
  `wchar_t** pool, int32 count` pair. The shipped binary has exactly
  **one** populated instance: a compile-time singleton in `.data` at
  `0x004afbb4` whose pool slot (`0x004afbbc`) points at a 127-entry
  `wchar_t*[]` table at `0x004af9b8`. Pool indices 0..111 hold the
  Czech UI (error messages, dialogs, key names, credits); 112..126
  hold a small English "iwannaplay.com" overlay used by the
  trial/registration dialogs (which carry direct Czech↔English pairs
  10 indices apart for the game-setup widget). UI widgets read the
  pool by hard-coded compile-time index, e.g.
  `*(wchar_t**)(PTR_PTR_004afbbc + 0xc8)` for `"Typ hry:"`; ~20
  dialog functions reference `0x004af9b8` directly. The class is
  also registered with the global class registry as classId **8**
  via `HandleClassRegister(0x0042e910)` so it can implement
  `IDSTexts` in the type system, but no resource in the master pack
  uses that classId, so the factory at `0x0042eeb0` is never invoked
  at runtime. The script-side `language = 1 = Czech` selector
  documented in `script_lifecycle.md` is **orthogonal** to this pool
  — it is hard-coded at all three `CallExport(slot=0)` call-sites
  and only affects per-script bytecode `StrConst` choices, not the
  static text.
* **Open questions:** None for the shipped Czech build. (Localising
  to another language requires relinking the .exe; the binary has no
  runtime pool-swap path.)
* **Artefacts:** `static_texts.md`, `static_texts.py`,
  `static_texts.json`, `script_lifecycle.md`

---

## Subsystems / state machines (engine-side)

### App shell

* **Classes:** `CDSApp`
* **Bytes:** 1.6 KB
* **Confidence:** **Verified**
* **What's known:**
  * CRT entry chain `_mainCRTStartup` (`0x00448e40`) →
    `___tmainCRTStartup` (`0x00448c60`) → `WinMain` (`0x00402680`) →
    `CDSApp_AppMain` (`0x0042aa60`).
  * `CDSApp_AppMain` calls `(*g_AppDescriptor.factory)()` =
    `CBulanci_CreateObject` (`0x00402a90`), then dispatches through
    `g_pApp`'s vftable in fixed order: `SetCmdLine` (vtbl[31],
    purecall) → `CDSApp_OnCreate` (vtbl[28] = `0x0042a210`) →
    `CDSApp_Run` (vtbl[29] = `0x00429d60`) → `CDSApp_dtor` (vtbl[2] =
    `0x0042b560`).
  * **Primary vftable `g_pCDSApp_vftable @ 0x0048700c`** — 34 slots
    enumerated end-to-end and renamed in Ghidra. Includes the
    entry-point quartet, the WndProc dispatcher, the activate hook,
    the modal entry/exit hooks, the dirty-rect callback, view-tree
    serialization slots 3/4/5 (broadcast pattern), `CDSView_HitTest`,
    `CDSView_SetRect`/`GetParentBounds`/`ComputeAnchoredRect`,
    `CDSView_OnFocus`/`OnKeyDown`/`OnKeyUp`/`OnChar`, and the
    purecall stubs at 15..18 and 31. Full slot table in
    `app_shell.md`.
  * **WndProc** `CDSApp_WndProc` (`0x00429c00`) is a trivial relay
    into vtbl[32] (`CDSApp_WndProcDispatch` = `0x0042a660`), the WM_*
    switch. Full message map documented (DESTROY, PAINT, CLOSE,
    ACTIVATEAPP, SETCURSOR, KEY{DOWN,UP,CHAR}, SYSKEY{DOWN,UP},
    MOUSE{MOVE,L*,R*}). ALT+ENTER toggles fullscreen via
    `CDSApp_SetWindowed`.
  * **Message pump**: `CDSApp_Run` → `CDSView_DoModal(this, NULL)` —
    the generic modal pump also reused by every dialog. Inner loop is
    `while (this->exitCode == 0) CDSApp_PumpTick(g_pApp)`. Tick body
    is "drain frames while engine event queue is empty, else dispatch
    one engine event". One frame = `CDSApp_UpdateClock` +
    `CDSApp_PulseTasks` + Win32 PeekMessageW / TranslateMessage /
    DispatchMessageW drain, then `CDSApp_RenderFrame` via the
    embedded `CDSBackBuffer` at `+0x7c/+0x80`.
  * **Tick frequency**: free-running (no `Sleep` / no `WaitMessage`).
    The pump only stops on the per-frame `PeekMessage`. Master clock
    is `g_dwElapsedMs = timeGetTime() - g_dwStartMs`
    (`@ 0x004b3bd8` / `0x004b3bdc`), refreshed at the top of every
    frame.
  * **Globals labelled & typed in Ghidra**: `g_pHInstance`,
    `g_pHwnd`, `g_pApp`, `g_pModalFocus`, `g_pInputChainHead`,
    `g_dwElapsedMs`, `g_dwStartMs`, `g_pTaskList`, `g_pEventQueue`,
    `g_AppDescriptor`, `g_AppClassTable`, `g_pCDSApp_vftable`.
  * **Object layout** (CDSApp portion): 4-vptr MI at
    `+0/+4/+0x10/+0x18`, logical app rect at `+0x20..+0x2c`
    (default 800×600), `flags1` `+0x44` and `flags2` `+0x46`,
    modal-loop exit code at `+0x4a`, parent-frame ptr at `+0x4c`,
    child-views list head at `+0x54`, embedded `CDSBackBuffer` at
    `+0x7c/+0x80`, `windowed` registry cache at `+0xe4`,
    dirty-rect array `+0x254` / count `+0x25c`, drawable flag
    `+0x274`. Shipping derived class is `CBulanci` (`0x4cc` bytes
    total, ctor `CBulanci_ctor`).
  * **App descriptor** at `g_AppDescriptor @ 0x004b3300` is a CDS
    class-registration record filled at C++ static-init time by
    `CBulanci_RegisterAppDescriptor` calling
    `HandleClassRegister(_, 2000, &g_AppClassTable, &CBulanci_CreateObject)`.
    The `+0xc` slot holds the factory pointer the engine top-level
    invokes.
  * Functions named in Ghidra (this round): `WinMain`,
    `CDSApp_AppMain`, `CDSApp_OnCreate`, `CDSApp_Run`,
    `CDSApp_WndProc`, `CDSApp_WndProcDispatch`,
    `CDSApp_OnActivateApp`, `CDSApp_PreCreateHook`,
    `CDSApp_OnDestroy`, `CDSApp_SetWindowed`, `CDSApp_ctor`,
    `CDSApp_dtor`, `CDSApp_DtorScalar`, `CDSApp_GetClassTable`,
    `CDSApp_DispatchInputEvent`, `CDSApp_KeybQueue`,
    `CDSApp_MouseQueue`, `CDSApp_PumpTick`, `CDSApp_FrameBody`,
    `CDSApp_RenderFrame`, `CDSApp_InitClock`,
    `CDSApp_UpdateClock`, `CDSApp_PulseTasks`,
    `CDSApp_PollEventQueue`, `CDSApp_DispatchOneEvent`,
    `CDSApp_AdaptDisplaySize`, `CDSView_DoModal`,
    `CDSView_SetModalEligible`, `CDSView_SetActive`,
    `CDSView_Invalidate`, `CDSView_IsModalDoneRecursive`,
    `CDSView_AcquireKeyboardFocus`, `CDSView_HitTest`,
    `CDSView_SetRect`, `CDSView_GetParentBounds`,
    `CDSView_ComputeAnchoredRect`, `CDSView_OnFocus`,
    `CDSView_OnKeyDown`, `CDSView_OnKeyUp`, `CDSView_OnChar`,
    `CDSView_EmptyHook27`, `CBulanci_CreateObject`, `CBulanci_ctor`,
    `CBulanci_RegisterAppDescriptor`, `CBulanci_GetAppDescriptor`,
    `CDSDirectSound_InitPrimary`.
* **Open questions:** Semantics of base-class vtable slots 3/4/5
  (broadcast templates whose per-class meaning depends on the
  derived override — needs a concrete override read on e.g. `CMenu`);
  layout of `g_AppDescriptor` past `+0x0c`; the wakeup contract
  between `CDSApp_KeybQueue` / `CDSApp_MouseQueue` enqueues at
  `this+0x10` and the pump's `CDSApp_PollEventQueue` /
  `CDSApp_DispatchOneEvent` dequeue side. None of these block
  byte-exact matching of `CDSApp` itself.
* **Artefacts:** `app_shell.md`

### DirectSound wrapper

* **Classes:** `CDSDirectSound`, `CDSAudioPlayer`,
  `CDSAudioVideoPlayer`
* **Bytes:** 1.2 KB
* **Confidence:** **Empty**
* **What's known:** DSound primary/secondary buffer management.
* **Open questions:** Mixer slot count, audio/video sync model.
* **Artefacts:** —

### Audio bank

* **Classes:** `CDSAudioBank`, `CDSAudioBankSample`, `CDSWav`,
  `CDSWavStream`
* **Bytes:** 2.0 KB
* **Confidence:** **Empty**
* **What's known:** Streamed sample playback layered on top of the
  stream hierarchy.
* **Open questions:** Sample-table layout in `.dsm`, RAM-cached vs
  streamed decision.
* **Artefacts:** —

### Threading

* **Classes:** `CDSWorkingThread`
* **Bytes:** 0.14 KB
* **Confidence:** **Empty**
* **What's known:** 5 funcs / 2 vtables; small but central — wraps
  `CreateThread` for audio/video streaming.
* **Open questions:** API surface (start/stop/wait), used by which
  clients.
* **Artefacts:** —

### Input

* **Classes:** `CDirectKeyb`, `CDSMouse`, `CDSImageMouse`,
  `CGunMouse`
* **Bytes:** 2.8 KB
* **Confidence:** **Empty**
* **What's known:** DirectInput-backed keyboard wrapper + three
  mouse variants (raw / image-cursor / weapon-aim).
* **Open questions:** Key-binding storage (probably profile-side),
  how `CGunMouse` couples with `CBulanci` aim.
* **Artefacts:** —

### Exception hierarchy

* **Classes:** `CDSException` + 7 subclasses (`CDSStreamException`,
  `CDSDirectXException`, `CDSRegKeyException`, `CDSApiException`,
  `CDSResourceException`, `CDSMemoryException`, `CDSSimpleException`)
* **Bytes:** 1.6 KB
* **Confidence:** **Empty**
* **What's known:** One vtable each; the base owns `What()`.
* **Open questions:** Which throw sites are wired to which subclass
  — knowing this attaches every `Unwind@xxxx` to its semantic
  parent.
* **Artefacts:** —

---

## Game logic

### Player entity (Bulánci)

* **Classes:** `CBulanci`, `CBulanek`
* **Bytes:** 37.2 KB
* **Confidence:** **Empty**
* **What's known:** `CBulanci` alone is 30.7 KB / 140 funcs — the
  heaviest game class. `CBulanek` (6.5 KB / 38) is the per-team-slot
  peer.
* **Open questions:**
  * State machine: spawn → move → shoot → die.
  * Inventory/weapon switching.
  * Network replication.
  * Where script callbacks (`OnTimer`, `OnEnter`/`OnLeave`) feed
    into the entity.
* **Artefacts:** —

### Combat / projectiles

* **Classes:** `CWeapon`, `CShot`, `CMina`, `CSpells`, `CExplosion`,
  `CTeleportPoint`
* **Bytes:** 5.1 KB
* **Confidence:** **Empty**
* **What's known:** Combat objects branched off `CIcon`/`CObstacle`.
  `CShot` is the projectile, `CMina` a placed mine, `CSpells` magic,
  `CExplosion` the FX.
* **Open questions:** Damage model, hit resolution, weapon →
  projectile mapping.
* **Artefacts:** —

### Match orchestration

* **Classes:** `CGame`, `CGaming`, `CLevelList`, `CLoadingLevel`,
  `CLevelScore`
* **Bytes:** 5.8 KB
* **Confidence:** **Sketched**
* **What's known:** `CGaming` is the active-level driver (35 funcs);
  its dtor `FUN_0041b850` fires script export 2 (`OnDeinit`) — proves
  the level → script lifecycle hookup. `CGame::FUN_004185a0` is the
  net-message dispatcher mentioned above.
* **Open questions:** Match-state machine (lobby → load → play →
  score), per-frame tick order.
* **Artefacts:** `script_lifecycle.md`

### Menu / start state machine

* **Classes:** `CMenu`, `CStartGame1`, `CStartGame2`
* **Bytes:** 14.2 KB
* **Confidence:** **Empty**
* **What's known:** `CMenu` (8.3 KB) drives the main menu tree;
  `CStartGame1` / `CStartGame2` (~6 KB) are the new-game flows.
* **Open questions:** Screen-graph transitions, settings
  persistence, hand-off into `CGaming`.
* **Artefacts:** —

### Multiplayer lobby UI

* **Classes:** `CChatList`, `CChatEdit`, `CSessionList`,
  `CSessionItem`, `CTcpIpConfig`
* **Bytes:** 1.6 KB
* **Confidence:** **Empty**
* **What's known:** Chat + session-browser widgets sitting on top
  of `CDSDirectPlay*`.
* **Open questions:** Wire-format of lobby chat vs in-game
  `OnNetCustom`.
* **Artefacts:** —

### Scoring / HUD counters

* **Classes:** `CScore`, `CScoreItem`, `CGameCounter`, `CNumCounter`,
  `CShotCounter`, `CPoem`, `CPoemScroller`
* **Bytes:** 4.0 KB
* **Confidence:** **Empty**
* **What's known:** Six counter variants + a scrolling poem widget
  for between-round screens.
* **Open questions:** Counter render contract; `CPoem*` for which
  scenes.
* **Artefacts:** —

### Movie / cinema views

* **Classes:** `CMovieView`, `CBlackView`
* **Bytes:** 0.8 KB
* **Confidence:** **Empty**
* **What's known:** `CMovieView` (14 funcs) wraps `CDSMpx` for the
  intro/outro movies; `CBlackView` is the fade-to-black transition.
* **Open questions:** —
* **Artefacts:** —

### Save / profile format

* **Classes:** (no obvious class; check `CBulanci` / `CGame`
  write-paths and `CDSRegKeyException` callers)
* **Bytes:** —
* **Confidence:** **Empty**
* **What's known:** Likely a mix of registry (HKCU) values and a
  file blob. The presence of `CDSRegKeyException` is a clear hint
  that profile state lives in the registry.
* **Open questions:** Registry layout (key path, value names),
  on-disk binary blob format if any.
* **Artefacts:** —

### Advertising

* **Classes:** `CAdvertising`
* **Bytes:** 0.21 KB
* **Confidence:** **Empty**
* **What's known:** 10 funcs / 5 vtables; tiny widget.
* **Open questions:** What it advertises (in-game banner? launcher
  splash?) and where the asset lives.
* **Artefacts:** —

### Help / History scripts

* **Classes:** `CHelpScript`, `CHistoryScript`, `CHelpDlg`,
  `CHistoryDlg`, `CHelpView`, `CHistoryView`
* **Bytes:** 1.1 KB
* **Confidence:** **Sketched**
* **What's known:** Same `CLevelScript` substrate; invoked from
  `CHelpDlg::FUN_00421c10` and `CHistoryDlg::FUN_00422f70` with
  argc=1 (language selector). Only the `GetInfo` export is
  documented.
* **Open questions:** Are extra export indices used by these
  subclasses? Do they override any dispatch slots?
* **Artefacts:** `script_lifecycle.md`

---

## Game UI widget kit

UI widgets (`CButton`, `CRadio`, `CSwitch`, `CScrollBar`,
`CScroller`, `CListBox`, `CListBoxItem`, `CListViewer`, `CEdit`,
`CNumEdit`, `CWindow`, `CPanel`, `CStaticText`, `CVolume`,
`CColorSet`, `CColorSwitch`, `CKeybShow`, `CProgressBar`, `CSwitch`,
`CBitmap`) account for ~30 KB but follow a common widget contract
(same ctor/dtor/Draw/Tick vtable shape). They're better matched
mechanically than reverse-engineered as a "subsystem" — the leverage
is finding the shared base-vtable virtual-slot order **once**, after
which every widget's vtable members type-resolve for free.

### Widget base contract

* **Classes:** (all widgets listed above)
* **Bytes:** ~30 KB
* **Confidence:** **Empty**
* **What's known:** Every widget has a 4- or 5-entry vtable with
  what looks like ctor/dtor/Draw/Tick/HandleInput.
* **Open questions:** Exact virtual-slot order on the shared base —
  unlocks ~270 widget functions for typed matching.
* **Artefacts:** —

---

## Updating this file

When you produce new findings:

1. Move the entry up (or into the `## Already analysed` group) if
   confidence rose (`Empty` → `Sketched` → `Partial` → `Verified`).
2. Trim "Open questions" as you resolve them; add specific Ghidra
   addresses so the next agent can verify.
3. Add the artefact path under "Artefacts" — every `.md` and
   committed `.py` should be reachable from here.
4. If a subsystem you discover doesn't fit any group above, add a
   new `###` entry under the most appropriate `##` group, or open a
   new `##` group rather than stuffing it into an existing entry.
