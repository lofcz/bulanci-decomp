# `CDSMpx` -- the MPx audio container (MPEG-1/2/2.5 Layer I/II/III)

**TL;DR.** Despite the `STATUS.md` grouping ("MPX video stream") and the
neighbouring `CDSVideoPlayer` / `CMovieView` classes, `CDSMpx` is **not a
video codec**. It is a full **MPEG-1, MPEG-2 (LSF) and MPEG-2.5 audio
decoder covering Layers I, II and III** -- specifically, a re-namespaced
build of **libmad 0.15.1b** (Underbit Technologies, Jan 2004) -- wrapped
in a thin Bulanci-specific `.mpx` container that prepends a 16-byte
WAVEFORMAT-style block in front of an otherwise stock MPEG audio
bitstream. The library identification is bit-identical across the CRC
table, scalefactor table, Layer I linear table, cosine constants, and
core algorithm shapes (see §9.2). For the byte-for-byte decomp this is
the same situation as `CDSJpegImage.cpp` vs `ref/libjpeg6b/`: matching
code wraps verbatim upstream sources, no original audio code needs to
be written. The four classes group together as one subsystem because
they all live in the player chain that `CMovieView` drives, but their
roles split cleanly:

| class           | role                                                                         |
|-----------------|------------------------------------------------------------------------------|
| `CDSMpx`        | MPEG audio decoder + per-handle MPx reader (the 13.8 KB / 39-func core)      |
| `CDSMpxStream`  | save/load wrapper that emits the 4 + 12 byte MPx header around a `CDSMpx`    |
| `CDSMpxDecoder` | tiny (3 funcs) `IDSAudioSource`-style frontend exposed to `CDSAudioPlayer`   |
| `CDSVideoPlayer`| **misnamed**: a generic multi-source media-track manager (4 funcs / 84 B)    |

The "video" in `CDSVideoPlayer` / `CDSAudioVideoPlayer` / `CMovieView` is
real -- `CMovieView` does play movies -- but the *video* frames come from
a **separate resource** (loaded via the same overlay/EAP path as
`BitmapJpegAnim` etc.) and `CDSMpx` only provides the *audio* track.

All findings below are cross-checked against decompiled bodies in
`bulanci.exe.c` and constants read from `bulanci.exe` via Ghidra MCP
(image base `0x00400000`). No `.mpx` resource ships in the
`unpacked/overlay/` master pack, so on-disk verification is pending
(see "Open questions").

## 1. Class map

Sizes from `docs/MODULES.md`; address spans from the Ghidra `Engine.DS.*`
namespaces.

| class           | funcs | bytes  | vftables | first - last        |
|-----------------|------:|-------:|---------:|---------------------|
| `CDSMpx`        |    39 | 13 832 |        3 | `00432f10 - 0045d166` |
| `CDSMpxStream`  |    11 |    341 |        5 | `00432eb0 - 004331d2` |
| `CDSMpxDecoder` |     3 |    151 |        1 | `004468b0 - 00446afe` |
| `CDSVideoPlayer`|     4 |     84 |        2 | `00439b90 - 00439fde` |

RTTI confirms these are the actual class names (strings at
`004afe20 .AVCDSMpx`, `004afe38 .AVCDSMpxStream`,
`004b0dd0 .AVCDSMpxDecoder`, `004b01d4 .AVCDSVideoPlayer`).

`CDSMpx` has three vtables because it's multiply-inherited; the
destructor `FUN_00432f40` patches them at offsets `+0x00`, `+0x04` and
`+0x18` before chaining to `CDSObject::vftable`.

## 2. The `CDSMpx` decoder object (`0x98c8` bytes)

Allocation size proven by the factory `FUN_00446b00`:

```c
this = (CDSMpx *)FUN_00447c42(0x98c8);   // 39 112 bytes
FUN_00446a00(this, *(int**)(p + 0x1c), *(undefined4*)(p + 0x2c));
```

Decoder-state offsets recovered from `FUN_004465e0` (open-substream
helper), `FUN_004466c0` (input-buffer refill), `FUN_00446770` (per-frame
decode loop) and `FUN_00446a00` (constructor with bitstream attach):

| offset    | size   | field (recovered name)                                |
|-----------|-------:|--------------------------------------------------------|
| `+0x0000` |  4     | `CDSMpx::vftable`                                     |
| `+0x0004` |  ~0x40 | bitstream reader state (`getbits` window, etc.)       |
| `+0x0044` |  ~0x24 | frame-header struct (MPEG header decoded into ints)   |
| `+0x2478` |  ~0x10 | per-channel subband-coeff window + ring index         |
| `+0x3480` |  2     | `numChannels` (1 mono / 2 stereo)                     |
| `+0x3482` |  2     | `samplesPerFrameMinusOne` or `numGranuleSamples`      |
| `+0x3484` |  ~0x1200| `int channelL[1152]` -- left-channel PCM scratch     |
| `+0x4684` |  ~0x1200| `int channelR[1152]` -- right-channel PCM scratch    |
| `+0x5888` |  8     | `u64 streamBaseOffset` -- file-pos of payload start   |
| `+0x5890` |  4     | `u32 inputCursor` -- bytes consumed so far            |
| `+0x5894` |  4     | `u32 inputLimit` -- total payload size                |
| `+0x5898` |  4     | `CDSStreamStorage* inputStream`                       |
| `+0x589c` |  ~0x10 | `CDSMemQueue` output PCM ring (init to `0x8014` cap)  |
| `+0x58ac` |  4     | `u32 maxSamplePacketBytes`                            |
| `+0x58bc` |  0x4000| `byte inputBuffer[16 KiB]` -- raw MPEG bitstream      |
| `+0x98bc` |  4     | `u32 initFlag` (1 or 2 after first frame)             |
| `+0x98c0` |  4     | `u32 sampleFormatPacked` (mirrors `this+0x58`)        |

The `1152` here is **deliberate** -- it is the MPEG-1 Layer II/III frame
size (`36 subbands × 32 samples = 1152` samples per channel per frame).
The two `int[1152]` buffers at `+0x3484` and `+0x4684` are the L/R
per-channel granule scratch.

## 3. Bitstream proof: this is an MPEG audio decoder

Three independent constants nail down the codec.

### 3.1 Frame-sync search (`FUN_00456850`)

```c
pcVar1 = (char*)FUN_00459690(&this->bitstreamCursor);
for (; (pcVar1 < (char*)(this->bufferEnd - 1) &&
        ((*pcVar1 != -1 || ((pcVar1[1] & 0xe0U) != 0xe0))));
     pcVar1 = pcVar1 + 1) { }
```

Scans byte-by-byte for `0xFF` followed by a byte whose top **three bits**
are set. That is the canonical 11-bit MPEG audio sync word, accepted in
the inclusive form that allows MPEG-2.5 (`0xFFE_`) as well as
MPEG-1/MPEG-2 (`0xFFF_`).

### 3.2 Frame-header parser (`FUN_00458f50`)

The function reads exactly the 32-bit MPEG audio frame header, in order:

| bit position | width | meaning                                | stored at                |
|--------------|------:|----------------------------------------|--------------------------|
| 0..10        | 11    | sync word `11111111111`                | (skipped)                |
| 11           | 1     | MPEG-2.5 flag (0 = MPEG-2.5)           | flags1 \| `0x4000` if 0  |
| 12           | 1     | MPEG-1 flag (0 = MPEG-2/LSF)           | flags1 \| `0x1000` if 0  |
| 13..14       | 2     | layer (`4 - raw`: 11=L1, 10=L2, 01=L3) | header[0] = `4 - raw`    |
| 15           | 1     | protection (CRC present if 0)          | flags1 \| `0x10` if 0    |
| 16..19       | 4     | bitrate index (0..14)                  | header[4] = table lookup |
| 20..21       | 2     | sample-rate index (0..2)               | header[5] = table lookup |
| 22           | 1     | padding                                | flags1 \| `0x80` if 1    |
| 23           | 1     | private                                | flags2 \| `0x100` if 1   |
| 24..25       | 2     | channel mode (`3 - raw`: 0..3)         | header[1] = `3 - raw`    |
| 26..27       | 2     | mode extension                         | header[2]                |
| 28           | 1     | copyright                              | flags1 \| `0x20` if 1    |
| 29           | 1     | original                               | flags1 \| `0x40` if 1    |
| 30..31       | 2     | emphasis                               | header[3]                |
| (+16 bits)   |    16 | CRC (only if protection bit was 0)     | header[6.5]              |

This is the textbook ISO/IEC 11172-3 / 13818-3 frame header. Three
invalid combinations are rejected:

* `layer == 0b00` (raw `4 - 0`)  → error `0x102`
* `bitrate == 0b1111`            → error `0x103`
* `samplerate == 0b11`           → error `0x104`

### 3.3 Bitrate + samplerate tables

Read directly from the binary at `0x0048b7b4` and `0x0048b91c`. The
bitrate table is `4 * 15` u32 entries (= 240 bytes) indexed
`[layerIdx * 15 + bitrateIdx]`. Decoded values:

**Row 1 (Layer I):** `0, 32 000, 64 000, 96 000, 128 000, 160 000,
192 000, 224 000, 256 000, 288 000, 320 000, 352 000, 384 000, 416 000,
448 000` -- exact MPEG-1 Layer I bitrate table (kbps × 1000).

**Row 2 (Layer II):** `0, 32 000, 48 000, 56 000, 64 000, 80 000, 96 000,
112 000, 128 000, 160 000, 192 000, 224 000, 256 000, 320 000, 384 000`
-- exact MPEG-1 Layer II table.

**Row 3 (Layer III):** `0, 32 000, 40 000, 48 000, 56 000, 64 000, 80 000,
96 000, 112 000, 128 000, 160 000, 192 000, 224 000, 256 000, 320 000`
-- exact MPEG-1 Layer III table.

**Sample-rate table at `0x0048b91c`:** `44 100, 48 000, 32 000`. The
parser shifts those right by 1 in MPEG-2 mode and by 2 in MPEG-2.5 mode,
yielding the standard `22 050 / 24 000 / 16 000` and
`11 025 / 12 000 / 8 000` sets.

### 3.4 Frame-size dispatcher (`FUN_00458e10`)

Picks the granule size from the layer field:

```c
if (header[0] == 1)              iVar4 = 0xC;   // Layer I  -> 12 samples/subband -> 384/frame
else if (header[0] != 3 ||
         header[7] & 0x1000 == 0) iVar4 = 0x24;  // Layer II -> 36 -> 1152/frame
else                             iVar4 = 0x12;   // Layer III LSF -> 18 -> 576/granule
```

i.e. Layer I = 384 PCM samples per frame, Layer II = 1152, Layer III =
1152 (MPEG-1) or 576 per granule (MPEG-2 LSF). All three branches
exist; this decoder is **a full MP1 + MP2 + MP3 decoder**, not just MP2.

### 3.5 Subband synthesis (`FUN_00456910`) and per-frame loop (`FUN_00457aa0` / `FUN_00458450`)

`FUN_00456910` is the classic 32-point cosine-matrix polyphase synthesis
shared by all three MPEG audio layers (libmad `synth.c::dct32`).
Its coefficients are 32-bit Q3.28 fixed-point cosines stored in
`g_anMadDct32Costab` at `0x0048af70..0x0048afb0` (`cos(kπ/64)`),
matching libmad's `costab1..31` compile-time bake-in. Sample
constants (in their stored uint form):

* `0xb504f33a` ≈ `cos(π/4)`  (libmad `MAD_F(0x0b504f33)`, scaled)
* `0xec835e80` ≈ `cos(π/8)`  (libmad `MAD_F(0x0ec835e8)`, scaled)
* `0x61f78aa0` ≈ `sin(π/8)`
* `0xfb14be80` ≈ `cos(π/16)` (libmad `MAD_F(0x0fb14be8)`, scaled)
* `0xd4db315x` ≈ `cos(3π/16)` (libmad `MAD_F(0x0d4db315)`)

inlined as a fully-unrolled radix-2 butterfly that fans out 32 subband
inputs into 32 PCM samples per call. The final 16-bit clamp/quantize
(`FUN_00446670`) is

```c
int v = sample + 0x1000;
if (v > 0x0fffffff) return  0x7fff;
if (v < -0x10000000) v =   -0x10000000;
return v >> 13;   // Q1.31 → s15  with +0.5 LSB rounding bias
```

which is the standard Q1.31 → s16 quantizer used by every MPEG audio
decoder, including libmad (`audio_linear_dither`).

`FUN_00457aa0` is the **stereo** per-frame outer loop (writes L/R into
two `int[1152]` granule buffers via a stride of `0x200` between the two
halves); `FUN_00458450` is the **mono / downmix** variant selected by
the `flags & 0x02` bit set in `FUN_00458e10`. Both call
`FUN_00456910` once per subband group.

### 3.6 ~~The "magic" quantization table (`InitMpegDequantizerTable` @ `0x00446550`)~~ &mdash; **retracted**

**Correction.** An earlier draft of this write-up claimed that the
256 KiB lookup at `0x004b84f8` (allocated by `0x00446550`) was the MPEG
Layer II requantisation matrix `c·(sample-d)`. **That was wrong.**
Tracing the readers (`FUN_0043f490` / `FUN_0043f630` / `FUN_0043f7c0`
and friends) shows it is the **2D sprite alpha-composite LUT** for
RGB565 blitting, indexed `table[(srcByte - dstByte + 255)*256 + alpha]`
and equal to `(srcByte - dstByte) * alpha / 255`. The init function
lives in `CBulanci::`, not `CDSMpx::`, and is called once at app
startup. It is now renamed `CBulanci::InitAlphaBlendLut` and the
global is `g_pAlphaBlendLut`. **No 256 KiB precomputed MP2 dequant
table exists.** The actual L2 dequantisation in `DecodeLayer2Frame` is
done **on the fly** via the scalefactor table at `0x0049c210` and the
bit-math in `DequantizeLayer2Subband` (see §3.8 below).

The MPEG-audio identification still holds (six independent fingerprints
remain &mdash; sync word, frame header, granule-size dispatch, bitrate /
samplerate tables, CRC-16-IBM table, layered decoder dispatch); only
the "MP2 needs a 256 KiB table" rationale was a misread.

### 3.7 CRC-16 over the frame side-info (`CalculateCrc16Mpeg` @ `0x004597b0`)

A fourth independent fingerprint: `0x004597b0` is a hand-rolled
**CRC-16-IBM** (poly `0x8005`) over a variable-length bitstream slice,
backed by a 256-entry byte-at-a-time lookup table at
`g_wCrc16Poly8005Table` (`0x0048b938`). The core loop is the textbook

```c
for (uint32 word; nBits >= 32; nBits -= 32) {
    word = BitstreamReadBits(&bs, 32);
    crc = (uint16)(crc << 8) ^ table[((word >> 16) ^ (crc >> 8)) & 0xff];
    crc = (uint16)(crc << 8) ^ table[((word >>  8) ^ (crc >> 8)) & 0xff];
    crc = (uint16)(crc << 8) ^ table[((word      ) ^ (crc >> 8)) & 0xff];
    crc = (uint16)(crc << 8) ^ table[((word >>  8) ^ (crc >> 8)) & 0xff];
}
// 8/16/24-bit byte tail, then a bit-feed remainder
while (nBits--) {
    bit = BitstreamReadBits(&bs, 1) ^ (crc >> 15);
    crc <<= 1;
    if (bit & 1) crc ^= 0x8005;
}
```

That's exactly the CRC the MPEG-1/2 audio frame header optionally
appends when `flags1 & 0x10` is *clear* in the parsed frame info (the
"protection bit" in ISO/IEC 11172-3 §2.4.1.2). No video codec in this
era used CRC-16 over its compressed payload -- this is yet another
shape that only an MPEG audio decoder grows.

### 3.8 The per-layer dispatch table at `g_pfnMpegLayerDecoder`

Right after `g_dwMpegSampleRateTable` (`0x0048b91c`) sits a three-entry
function-pointer table at `0x0048b928`:

| index    | address       | now named               | body |
|----------|---------------|-------------------------|-----:|
| `[0]` L1 | `0x0045c4e0`  | `DecodeLayer1Frame`     | 783 B |
| `[1]` L2 | `0x0045c8c0`  | `DecodeLayer2Frame`     | 1642 B |
| `[2]` L3 | `0x0045c100`  | `DecodeLayer3Frame`     | 895 B |

`ReadOneFrame` (`0x00459500`) dispatches via
`(*pfn[layer])(frameInfo, bitstreamCtx)` -- the engine indexes the
table as `&g_dwMpegSampleRateTable[2] + layer*4` so layer = 0 (= reserved)
would land on the harmless `32000` constant. The defensive pre-check
in `ParseMpegFrameHeader` ensures layer is in `{1, 2, 3}` before any
dispatch.

Decompilation of `DecodeLayer2Frame` is the cleanest single-shot
confirmation of "this is MPEG audio". It:

1. Picks one of five allocation tables (ISO/IEC 11172-3 Tables B.2a..B.2e)
   based on `(channelMode == mono?, sampleRate, free-format flag,
   bitrate)`, mirroring the standard's exact selection logic
   (`<= 80 kbps mono @ 32 kHz -> Table B.2c` etc).
2. Reads per-subband bit-allocation codewords (variable-width `nbal`
   from `g_wMpegL2NbalTable @ 0x0049c3fc`).
3. Reads 2-bit SCFSI per used subband, then 6-bit scalefactors per
   SCFSI pattern (the same `0..3` switch that ISO 11172-3 §2.4.2.4
   defines).
4. **Verifies the CRC**: computes `CalculateCrc16Mpeg` over the
   protected bits, compares to the 16-bit CRC stored in the frame at
   `frameInfo + 0x1a`, and errors out with code `0x201` on mismatch
   (unless a "tolerate CRC errors" sticky bit is set at
   `frameInfo + 0x2c & 0x01`).
5. Walks 12 granules (`for (i = 0; i < 12; ++i)` via the
   `local_174 < 0x480` outer loop and the `0x60` stride) × 3 samples
   per granule × 32 subbands -- exactly the L2 frame structure of
   1152 samples.
6. Per-sample: calls `mad_layer_II_samples` (`0x0045c800`, libmad
   `layer12.c::II_samples`) to recover 3 samples at a time from the
   bit-allocation step + read codewords, then multiplies by the
   scalefactor table at `g_anMadSfTable @ 0x0049c210` (libmad's
   `sf_table[64]`, Q3.28 representing `2^(-scf/3)`).

DecodeLayer1Frame and DecodeLayer3Frame share the same skeleton -- in
particular DecodeLayer3Frame additionally has a Huffman + IMDCT path
that feeds the same per-channel `int[1152]` scratch as L1/L2 before
the final `PolyphaseSynthDct32` stage.

That makes **five independent fingerprints of a stock ISO MPEG-1/2/2.5
audio decoder** (frame sync, header layout, bitrate / samplerate
tables, CRC-16-IBM, layered dispatch + Layer II bit-allocation /
scalefactor /SCFSI structure), all matching textbook ISO/IEC 11172-3
/ 13818-3 -- enough to nail the codec identification without ever
running a `.mpx` file.

## 4. The `.mpx` file framing

`CDSMpxStream` is a tiny class (11 funcs) whose only job is to save /
load a `CDSMpx`'s persistent state around the raw MPEG audio bitstream.
The save (`FUN_00432eb0`) and load (`FUN_00433180`) paths are perfectly
symmetric:

```c
// save (FUN_00432eb0)
stream->vt[5_write](this - 0x08,  4);     // u32  dataSize
stream->vt[5_write](this - 0x30, 12);     // byte header[12]
queue->vt[10_seek](startOffsetLow, startOffsetHigh, 0);
FUN_0042fdf0(stream, queue, dataSize, 0, NULL, 0);

// load (FUN_00433180)
stream->vt[4_read]  (this - 0x08,  4);    // u32  dataSize
stream->vt[4_read]  (this - 0x30, 12);    // byte header[12]
*(u64*)(this - 0x10) = stream->vt[8_tell64]();
// stash stream pointer for later refills
```

Combined with the duration formula recovered from
`FUN_0043a000` -- `duration_ms = (dataSize * 1000) / ((bits>>3) * channels * sampleRate)`
-- and the field offsets used by `FUN_0043a000` and friends
(`+4 = dataSize, +8 = channels, +0xa = bitsPerSample, +0xc = sampleRate`),
the on-disk layout is:

```
+0x00   u32  dwDataSize           ; total bytes of MPEG audio bitstream that follow
+0x04   u16  wChannels            ; 1 (mono) or 2 (stereo)
+0x06   u16  wBitsPerSample       ; 16
+0x08   u32  dwSampleRate         ; e.g. 22050, 44100
+0x0c   u32  dwAvgBytesPerSec     ; precomputed for DirectSound
+0x10   byte[dwDataSize]  bitstream ; back-to-back MPEG-1/2/2.5 L1/L2/L3 frames
```

This is essentially a 16-byte **stripped-down `WAVEFORMAT`** prepended
to a raw MPEG audio stream (the same trick id3-less `.mp2` players use,
except the size and format are pre-baked so the engine can size the
DirectSound secondary buffer before parsing the first frame).

The MPEG bitstream itself is byte-identical to a standalone `.mp2`/`.mp3`
-- frames start with `0xFF Ex` (MPEG-2.5) or `0xFF Fx` (MPEG-1/2), each
frame carries its own bitrate / samplerate / channel mode, and the
decoder happily switches all three on a per-frame basis.

## 5. Player wiring

```
                 .mpx file on disk
                       │
                       ▼
   ┌──────────────────────────────────────────────────────┐
   │ CDSMpxStream                                          │
   │  • reads 4 + 12-byte header                           │
   │  • Tell64 stream cursor → payload start               │
   │  • holds CDSStreamStorage* to the source              │
   └───────────────┬──────────────────────────────────────┘
                   │
                   ▼   (FUN_00446b00 factory:  alloc 0x98c8 + FUN_00446a00 init)
   ┌──────────────────────────────────────────────────────┐
   │ CDSMpx (the 39-KiB decoder object)                    │
   │  +0x58bc 16 KiB input buffer  ──── refilled from      │
   │           CDSStreamStorage via FUN_004466c0           │
   │  +0x44   MPEG frame header parsed by FUN_00458f50     │
   │  +0x3484 / +0x4684  L/R synthesis scratch (int[1152]) │
   │  +0x589c CDSMemQueue ring buffer of decoded PCM       │
   └───────────────┬──────────────────────────────────────┘
                   │  (PCM samples in s16 little-endian)
                   ▼
   ┌──────────────────────────────────────────────────────┐
   │ CDSMpxDecoder  (3 funcs / 151 B)                      │
   │  • implements IDSAudioSource for CDSAudioPlayer       │
   │  • pulls PCM out of the ring on demand                │
   └───────────────┬──────────────────────────────────────┘
                   │
                   ▼
   ┌──────────────────────────────────────────────────────┐
   │ CDSAudioPlayer / CDSAudioVideoPlayer                  │
   │  • drives an IDirectSoundBuffer8 (vt[15] = SetVolume) │
   │  • per-tick top-up via FUN_0043a060 / FUN_0043a0f0    │
   └──────────────────────────────────────────────────────┘
```

`CMovieView` (`FUN_004236a0`, ctor signature
`(this, param_1, param_2, param_3, audioResId, videoResId)`) loads
**two separate resources**: one referenced via `this+0x7c` (audio,
likely the `.mpx`) and one via `this+0x78` (video). It instantiates a
`CDSAudioVideoPlayer` (80 bytes, alloc'd in `FUN_0043bca0`) that holds
the audio handle at `+0x04` and an embedded `CDSVideoPlayer` at `+0x08`.

So the original `STATUS.md` claim *"`.mpx` is a custom container; is the
codec borrowed (FLI/SMK derived) or original?"* is wrong on three
counts:

1. There is no video in a `.mpx`. Video for `CMovieView` comes from a
   separate resource and is *not* a `CDSMpx`.
2. The codec is neither original nor FLI/SMK-derived; it is the
   **standard ISO MPEG-1/2/2.5 Layer I/II/III audio codec**.
3. The lookalike classes (`CDSMpxStream`, `CDSMpxDecoder`,
   `CDSVideoPlayer`) are *plumbing*, not a multi-stream multiplex.

## 6. `CDSVideoPlayer` -- the misnamed generic track manager

For completeness: `FUN_00439c70` (ctor) sets up an `int[]` array at
`+0x1c` (capacity `+0x28 = 8`, count `+0x24 = 0`) of 8-byte entries
`{u32 id; IDSSource* src;}`. `FUN_00439bd0` does a sorted insert/find,
`FUN_00439eb0` makes the entry at index *i* current and (optionally)
starts playback, `FUN_00439b40` drives the per-tick pump. The dtor
`FUN_00439d30` calls `vt[8]` then `vt[2]` on every entry's `src`. Nothing
about it is specifically video; `CDSAudioVideoPlayer` reuses the very
same class at offset `+0x08` to manage its video-source list.

## 7. Renames / structs pushed to Ghidra

Applied via `rename_function_by_address` / `set_global` / `create_struct`
/ `set_plate_comment` against the running `bulanci.exe` Ghidra project.
Auto-applied `CDSMpx::` / `CDSMpxStream::` / `CDSMpxDecoder::` /
`CDSVideoPlayer::` namespace prefixes from RTTI are not repeated below.

| address      | new name                                       |
|--------------|-------------------------------------------------|
| `0x00432eb0` | `CDSMpxStream::SaveMpxFile`                     |
| `0x00433180` | `CDSMpxStream::LoadMpxFile`                     |
| `0x004465e0` | `CDSMpx::ResetDecoderState`                     |
| `0x00446550` | `CBulanci::InitAlphaBlendLut` (**not** MPEG-related; see §3.6) |
| `0x00446640` | `CDSMpx::ReinitDecoder`                         |
| `0x00446670` | `CDSMpx::QuantizeQ31ToS16`                      |
| `0x004466c0` | `CDSMpx::RefillInputBuffer`                     |
| `0x00446770` | `CDSMpx::DecodeFrame` (driver)                  |
| `0x00446940` | `CDSMpx::ReadPCM` (queue-pull API)              |
| `0x004469a0` | `CDSMpx::ForwardReadPcmToInstance`              |
| `0x00446a00` | `CDSMpx::AttachBitstream` (was "Open(stream)")  |
| `0x00446b00` | `CDSMpx::CreateFromHandle` (factory)            |
| `0x00446b90` | `CDSMpx::ResolveResource`                       |
| `0x00456850` | `mad_stream_sync` (was `CDSMpx::FindMpegFrameSync`) |
| `0x00456910` | `mad_synth_frame_dct32_full` (was `CDSMpx::PolyphaseSynthDct32`) |
| `0x00457aa0` | `CDSMpx::DecodeGranuleStereo`                   |
| `0x00458450` | `CDSMpx::DecodeGranuleMono`                     |
| `0x00458e10` | `CDSMpx::DispatchLayerDecoder` (per-frame)      |
| `0x00458eb0` | `CDSMpx::InitSynthState`                        |
| `0x00458ee0` | `CDSMpx::InitFrameInfo`                         |
| `0x00458f50` | `mad_header_decode` (was `ParseMpegFrameHeader`) |
| `0x00459130` | `CDSMpx::EstimateAvgBitrate`                    |
| `0x004592d0` | `CDSMpx::SeekToValidFrame`                      |
| `0x00459500` | `CDSMpx::ReadOneFrame`                          |
| `0x00459650` | `mad_bit_init` (was `SetBitstreamCursor`)       |
| `0x00459670` | `mad_bit_length`                                |
| `0x00459690` | `mad_bit_nextbyte` (was `GetBitstreamCursor`)   |
| `0x004596b0` | `mad_bit_skip` (was `BitstreamSkipBits`)        |
| `0x004596f0` | `mad_bit_read` (was `BitstreamReadBits`)        |
| `0x004597b0` | `mad_bit_crc` (poly 0x8005, was `CalculateCrc16Mpeg`) |
| `0x00439b40` | `CDSVideoPlayer::BeginCurrentTrackPlayback`     |
| `0x00439bd0` | `CDSVideoPlayer::InsertOrFindTrack`             |
| `0x00439c70` | `CDSVideoPlayer::ConstructTrackManager`         |
| `0x00439e00` | `CDSVideoPlayer::AddTrackSource`                |
| `0x00439eb0` | `CDSVideoPlayer::SetCurrentTrack`               |
| `0x0043a000` | `ComputeDurationMs` (player-side WAV duration)  |
| `0x0048af70` | `g_anMadDct32Costab` (libmad `synth.c::costab1..31`, `int[31]`) |
| `0x0048b7b4` | `g_dwMpegBitrateTable` (typed `uint[60]`)       |
| `0x0048b91c` | `g_dwMpegSampleRateTable` (typed `uint[3]`)     |
| `0x0045c100` | `mad_layer_III` (was `DecodeLayer3Frame`, 895 B) |
| `0x0045c480` | `mad_layer_I_sample` (was `DequantizeLayer1Sample`) |
| `0x0045c4e0` | `mad_layer_I` (was `DecodeLayer1Frame`, 783 B)  |
| `0x0045c800` | `mad_layer_II_samples` (was `DequantizeLayer2Subband`) |
| `0x0045c8c0` | `mad_layer_II` (was `DecodeLayer2Frame`, 1642 B) |
| `0x0048b928` | `g_pfnMpegLayerDecoder` (3 × `void *`)          |
| `0x0048b938` | `g_awMadCrcTable` (libmad `bit.c::crc_table`, `ushort[256]`) |
| `0x0049c210` | `g_anMadSfTable` (libmad `sf_table.dat`, `int[64]`) |
| `0x0049c308` | start of `&g_anMadLinearTable - 2` (the `nb*4` indexing offset; libmad's `&linear_table[-2]` trick) |
| `0x0049c310` | `g_anMadLinearTable` (libmad `layer12.c::linear_table`, `int[14]`) |
| `0x004b84bc` | `g_pDirectSoundSingleton` (`void *`)            |
| `0x004b84f8` | `g_pAlphaBlendLut` (`short *`; **not MPEG-related**, see §3.6) |

Plate comments anchored at:
`0x00432eb0`, `0x00433180`, `0x00446550`, `0x00446670`, `0x00446770`,
`0x00446a00`, `0x00446b90`, `0x00456850`, `0x00456910`, `0x00458e10`,
`0x00458f50`, plus the four data globals listed above.

Structs created in the Ghidra data-type manager:

```c
struct MpxFileHeader {            // 16 bytes; written/read by SaveMpxFile / LoadMpxFile
  uint32 dwDataSize;
  uint16 wChannels;
  uint16 wBitsPerSample;
  uint32 dwSampleRate;
  uint32 dwAvgBytesPerSec;
};

struct MpegAudioFrameInfo {       // 40 bytes; output of ParseMpegFrameHeader
  int    layer;                   // 1..3 (engine maps 4 - rawLayerField)
  int    channelMode;             // 0..3 (engine maps 3 - rawModeField)
  int    modeExtension;
  int    emphasis;
  uint   bitratePerSecond;
  uint   sampleRateHz;            // post-LSF/MPEG-2.5 shift
  short  crc16;
  short  reserved;
  uint   flags1;                  // 0x10 prot-absent, 0x20 copyright, 0x40 original,
                                  // 0x80 padding, 0x1000 MPEG-2 LSF, 0x4000 MPEG-2.5
  uint   flags2;                  // 0x100 private
  short  crc16Extra;
  short  reserved2;
};
```

Not yet pushed (open / mechanical / low-value): the bare scalar-dtors at
`0x00432fc0` / `0x00446ae0`, the small `CDSMpxStream::` virtual-slot
trampolines at `0x00432ff0..0x00433090`, and the long tail of
`FUN_004***` helpers that fall out of the matching pass once the
`CDSMpx` struct layout above is wired into Ghidra parameters.

## 8. Open questions / what would promote this to **Verified**

1. **An actual `.mpx` file.** No `BitmapJpegAnim`/`MpxAudio` resource of
   the matching ClassID ships in `unpacked/overlay/` -- the master pack
   contains only ClassIDs `{21, 28, 48, 52, 54, 58, 67, 76, 94, 2026,
   2043, 2050}`. Two plausible explanations:
   * The trial-version distribution simply does not contain intros /
     outros (`CMovieView` is referenced from `CMenu`'s "Movie" button
     and from `CHelpDlg`, but those code paths may early-out).
   * Movies ship as loose files (e.g. `intro.mpx`, `outro.mpx`) next to
     `bulanci.exe`, not as resources inside the EAP/overlay. Locating a
     full retail install would settle this. The ClassID check in
     `FUN_00446b90` against `0x27` (= 39, which `bulanci_unpack.py` lists
     as script-VM opcode `StrmWrite`) suggests an indirect-load path
     gated on a different IDSResource ClassID.
2. **Confirm the 12-byte header layout** on a real `.mpx`. The mapping
   here is consistent with both the save/load symmetry and the
   `FUN_0043a000` duration calc, but the `wBitsPerSample` field being a
   `u16` rather than packed in with channels is an inference -- could
   equally be `{ u16 channels; u16 codecHint; u32 sampleRate;
   u32 avgBytesPerSec; }`.
3. **Pin down the libmad build flags.** §9.2 narrows it to
   `FPM_DEFAULT|FPM_64BIT + OPT_SSO + !OPT_DCTO`, but verifying that
   against an `objdiff` of `mad_synth_frame_dct32_full` compiled under
   MSVC8 `/O2` (the project's chosen toolchain) is the cleanest way to
   pick the exact combination. The Layer III path (`mad_layer_III`) is
   also the most sensitive to `OPT_SPEED` / `OPT_ACCURACY` knobs --
   needs one inspection pass once libmad sources are on the project
   build path.
4. **Mono vs. joint-stereo dispatch.** The `flags & 0x02` test that
   selects `FUN_00458450` over `FUN_00457aa0` (the dispatch in
   `FUN_00458e10`) is described above as "mono / downmix", but it could
   equally be the M/S (intensity-stereo) flag for joint-stereo Layer II
   frames. Needs one more pass to lock down.
5. **Sync to `CDSAudioBank`.** Confirmed `CDSMpx` does **not** push PCM
   into the audio bank -- it owns its own output ring (`+0x589c`) and is
   pulled from by `CDSAudioPlayer` directly. No interaction with
   `CDSAudioBank` / `CDSAudioBankSample`.

A `_scratch_mpx_header.py` that parses the 16-byte header off any
candidate file and dumps the first few MPEG frame headers from the
bitstream is the quickest verification step once a `.mpx` sample is in
hand. It is committed alongside this write-up; usage:

```bash
python tools/bulanci_unpack/ghidra_analysis/_scratch_mpx_header.py \
    path/to/intro.mpx --frames 8
```

It prints the recovered `MpxFileHeader`, then walks the MPEG payload,
decoding the first N frame headers with the exact same bitrate /
sample-rate tables and `4 - layer` quirk used by
`ParseMpegFrameHeader`, and finishes with a payload-size cross-check
and a layer / version / sample-rate distribution. If the first 4 bytes
after the header don't look like an MPEG audio sync word the script
exits with a clear "either the header schema is wrong on this file or
the `.mpx` contains a different codec" warning -- i.e. the writeup's
schema falsifies cleanly the moment a real sample disagrees.

Additionally, the global at `g_pDirectSoundSingleton` (`0x004b84bc`) is
referenced by `ResolveResource` as a "DirectSound up?" gate, which
suggests `CMovieView` can short-circuit movie playback on systems with
no audio device -- worth tracing if the "no intro" symptom of the trial
build turns out to be that path rather than just missing assets.

## 9. Library bisect log

Source-library candidates compared against the decompiled `CDSMpx`
internals. Each row records the falsification verdict so future passes
don't repeat work.

### 9.1 LAME 3.92 `mpglib` (April 2002, `ref/lame392/mpglib/`) -- **FALSIFIED**

Six structural differences, any one sufficient on its own:

| feature                | LAME 3.92 `mpglib`                                           | bulanci binary                                            |
|------------------------|--------------------------------------------------------------|-----------------------------------------------------------|
| Numeric format         | `typedef float real` (default)                               | Q3.28 signed fixed-point throughout                       |
| Cosine constants       | computed at startup via `cos(k*pi/64)` in `tabinit.c`        | pre-baked Q-format ints in `g_anMadDct32Costab`            |
| Sample-rate table      | 9 explicit values `{44100,48000,32000,22050,...,8000}`       | 3 values `{44100,48000,32000}` + right-shift for LSF / 2.5 |
| Bitrate table          | `tabsel_123[2][3][16]`                                        | `g_dwMpegBitrateTable[4][15]` + separate LSF table at `0x0048b8a4` |
| L2 dequant lookup      | `real muls[27][64]` (≈ 7 KiB float)                          | on-the-fly bit math in `mad_layer_II_samples`              |
| L2 grouped codes       | `grp_3tab[96] / grp_5tab[384] / grp_9tab[3072]` lookups       | arithmetic ungrouping (`% n`, `/ n`)                       |
| Frame CRC              | **not computed, not verified** (only `error_protection` flag stored) | **actively computed and compared** via `mad_bit_crc`  |
| Synth split            | `dct64` + 32-tap windowing via `synth_1to1` (float MAC chain) | `mad_synth_frame_dct32_full` (Q3.28 butterfly) + separate windowing |

mpglib's missing CRC verification was the cleanest single discriminator.

### 9.2 libmad 0.15.1b (Underbit, Jan 2004, `ref/libmad-0.15.1b/`) -- **CONFIRMED**

**This is the source library.** The `CDSMpx` MPEG decoder is libmad 0.15.1b,
likely built straight from the public release tarball with the project's
MSVC8 toolchain and re-namespaced into a thin `CDSMpx::` wrapper.

Seven bit-identical fingerprints, any **one** sufficient to identify the
library; together they leave essentially zero residual probability for
anything else:

| # | what                              | libmad source                                                                | bulanci binary                                  | match                |
|---|-----------------------------------|------------------------------------------------------------------------------|-------------------------------------------------|----------------------|
| 1 | Fixed-point format                | `MAD_F_FRACBITS == 28`, `MAD_F_ONE == 0x10000000` (`fixed.h:72-93`)          | scalefactor `s[3]=1.0 == 0x10000000` everywhere | identical (Q3.28)    |
| 2 | CRC-16 lookup table               | `crc_table[256]` in `bit.c:44`, starts `{0x0000, 0x8005, 0x800f, 0x000a, ...}` | `g_awMadCrcTable @ 0x0048b938` byte-for-byte    | **bit-identical**    |
| 3 | CRC compute algorithm             | `mad_bit_crc()` -- 32-bit chunked + `switch(len/8)` 3/2/1 fall-through + bit-tail | `mad_bit_crc @ 0x004597b0` -- same structure with reverse-counter tail | **algorithmic match** |
| 4 | Layer I/II scalefactor table      | `sf_table[64]` from `sf_table.dat` -- `{0x20000000, 0x1965fea5, 0x1428a2fa, 0x10000000, 0x0cb2ff53, ...}` | `g_anMadSfTable @ 0x0049c210`                  | **bit-identical (8/64 verified, table size and offset match)** |
| 5 | Layer I linear scaling table      | `linear_table[14]` in `layer12.c:53` -- `{0x15555555, 0x12492492, 0x11111111, 0x10842108, ...}` | `g_anMadLinearTable @ 0x0049c310`                | **bit-identical (14/14 verified)** |
| 6 | Layer I sample requantization     | `I_sample()` in `layer12.c:75` -- MSB-invert, sign-extend, `<<(28-(nb-1))`, `+MAD_F_ONE>>(nb-1)`, `*linear_table[nb-2]` | `mad_layer_I_sample @ 0x0045c480` -- same expression shape with `<<(0x1d-nb)` and `*table[nb-2]` (indexed via `&table-2` trick) | **byte-for-byte semantic match incl. indexing trick** |
| 7 | DCT32 cosine constants            | `dct32()` in `synth.c:152` -- `costab1..31 = MAD_F(0x0fb14be8 ...)` baked at compile time | `g_anMadDct32Costab @ 0x0048af70` -- pre-baked cos(kπ/64) | **same constants** (possibly OPT_DCTO branch; see below) |

Together these establish provenance with the same level of evidence that
`CDSJpegImage.cpp` already cites for **libjpeg-6b** (`ref/libjpeg6b/`).
The `CDSMpx` source file can be reorganized along libmad module lines
(`bit.c` / `frame.c` / `layer12.c` / `layer3.c` / `stream.c` / `synth.c`)
once the Ghidra-side renames have all propagated.

#### Build configuration hints

libmad's behaviour depends on a couple of `OPT_*` toggles. The bulanci
binary appears to use:

* **`FPM_DEFAULT`** (or `FPM_64BIT`): the bulanci `mad_f_mul` retrieve is
  `(hi << 4) | (lo >> 28)` -- a 32x32→64 multiply followed by a 28-bit
  scale. That's the default 64-bit-intermediate path, not `FPM_INTEL`'s
  inline-asm shortcut.
* **`OPT_SSO` ON** (synth.c:97 `#define SHIFT(x) (((x) + (1L << 11)) >> 12)`):
  the granule-loop output shift `>> 13` (= `>> 12 + >> 1` rounding) in
  bulanci's `CDSMpx::QuantizeQ31ToS16` (`0x00446670`) matches the SSO
  optimisation path exactly. With SSO off the shift would be 28 not 13.
* **`OPT_DCTO` likely OFF**: the cosine constants in `g_anMadDct32Costab`
  match the *non-OPT_DCTO* `costab1..31` values shifted, not the OPT_DCTO
  ones; the corresponding `MUL(x,y) = mad_f_mul(x,y)` (default) is what
  the bulanci dct32 inlines.

These can be locked down precisely once an objdiff run with libmad
compiled under MSVC8 / `/O2` is in place.

#### What this unblocks

* **Matching strategy for `CDSMpx.cpp`**: follow the same approach as
  `CDSJpegImage.cpp` -- include verbatim libmad sources from
  `ref/libmad-0.15.1b/` and wrap them with a `CDSMpx::` class shell
  whose member functions thunk to `mad_*` API entry points. The
  `// !FUNC ADDR BEGIN/END` blocks in `src/bulanci/CDSMpx.cpp` will be
  satisfied by the libmad object files, not by hand-written code.
* **Renames now use libmad's upstream symbol names** (`mad_layer_I`,
  `mad_layer_II`, `mad_layer_III`, `mad_bit_crc`, `mad_bit_read`,
  `mad_header_decode`, `mad_synth_frame_dct32_full`, `mad_stream_sync`,
  `mad_layer_I_sample`, `mad_layer_II_samples`). The class-prefix
  `CDSMpx::` is still applied by Ghidra's RTTI namespace for the few
  driver functions (`CreateFromHandle`, `AttachBitstream`, `DecodeFrame`,
  `ReadPCM`, `RefillInputBuffer`, …) that are bulanci-specific glue.
* **No need to reverse the Huffman tables.** libmad's `huffman.c` is
  ~73 KB of pre-built code-tables; the decompiler views inside
  `mad_layer_III` (`0x0045c100`) almost certainly index those tables
  verbatim. Once the libmad include is wired in, `objdiff` will match
  those byte-for-byte without any further analysis.

### 9.3 Other candidates (not pursued)

* **mpg123 0.59x** (pre-mpglib split) -- moot. libmad is confirmed.
* **dist10 / ISO reference** -- moot.
* **maplay 1.2** -- moot (and would have been falsified by L3 coverage).
* **In-house Czech implementation** -- moot. The "leading hypothesis"
  in the previous draft was wrong; the unique fingerprint
  (Q3.28 + CRC verification + on-the-fly L2 dequant + 14-entry
  `linear_table`) is *exactly* libmad's signature, not an
  in-house re-implementation. Earlier puzzlement about the
  combination resolved once the actual upstream sources were on disk.

## 10. Porting progress (objdiff matches)

Hand-port targets live inside `src/bulanci/CDSMpx.cpp` as `CDSMpx::mad_*`
class methods (see `// !FUNC ADDR BEGIN/END` blocks).  Each method body
is transcribed verbatim from `ref/libmad-0.15.1b/<file>.c` with the
minimal shape adjustments needed for the project's COFF-friendly
prototypes (`int*` instead of `struct mad_bitptr*`, `uint*` instead of
`struct mad_stream*`, etc.).  A small `MadBitptr_layout` struct lives in
the `// !PROLOGUE BEGIN/END` block at the top of the file so the bit
helpers can address `byte / cache / left` by name without depending on
libmad's full headers.

Two MSVC8 quirks recur and have project-wide implications:

* **`__declspec(noinline)` on every libmad helper** that's referenced
  from another `mad_*` function.  Upstream libmad ships `bit.c`,
  `stream.c`, `frame.c`, etc. as separate translation units; concatenating
  them into one `CDSMpx.cpp` lets MSVC8 /O2 inline 17-byte leaf calls
  into their callers, which inflates the caller's size and destroys the
  byte-match.  The `noinline` attribute restores the orig's per-call
  CALL/RET shape.
* **`void` returns instead of `uchar` where libmad's source returns
  `void`.**  Ghidra's recovered prototype defaults to `uchar` because the
  `RET` instruction clobbers the `al` register; if we keep `uchar` the
  port's `return 0;` makes MSVC emit an extra `xor al, al` not present
  in the orig.  Prototypes are flipped to `void` in Ghidra (via
  `set_function_prototype` MCP) before re-running `export_mapping_via_mcp`
  and `sync_units`.  Affected so far: `mad_bit_init`, `mad_bit_skip`,
  `mad_header_init`, `mad_frame_mute`, `mad_frame_init`.

### 10.1 `bit.c` (six functions, complete)

| addr       | function           | size  | match% | residual diff                                  |
|------------|--------------------|-------|--------|------------------------------------------------|
| `0x459650` | `mad_bit_init`     | 23 B  | 100.0% | -                                              |
| `0x459670` | `mad_bit_length`   | 31 B  | 100.0% | -                                              |
| `0x459690` | `mad_bit_nextbyte` | 17 B  | 100.0% | -                                              |
| `0x4596b0` | `mad_bit_skip`     | 64 B  | 100.0% | -                                              |
| `0x4596f0` | `mad_bit_read`     | 177 B | 97.6%  | 3-byte `lea ecx, [ecx]` loop-alignment NOP     |
| `0x4597b0` | `mad_bit_crc`      | 335 B | 84.8%  | EBP-frame + `and esp, 0xfffffff8` (orig has    |
|            |                    |       |        | aligned stack; MSVC8 here picks ebp as the     |
|            |                    |       |        | loop counter vs orig's ebx).  Cannot reproduce |
|            |                    |       |        | the aligned prologue from portable C; neither  |
|            |                    |       |        | `#pragma optimize("y", off)` nor               |
|            |                    |       |        | `__declspec(align(8))` on the bitptr local     |
|            |                    |       |        | triggers it under MSVC8 /O2.                   |

Notable port details:

* `mad_bit_read` uses an explicit `static_cast<unsigned int>` on the
  fast-path AND so MSVC picks `shr` over `sar` for the unsigned shift
  (matches orig's `shr eax, cl` at `+0x38`).
* `mad_bit_crc` reconstructs the by-value `mad_bitptr` on the caller's
  frame so it can `lea ecx, [&bitptr]` for the inner `mad_bit_read`
  calls.  The orig stores its struct at `[ebp+0xc]` in the aligned
  region; ours sits at `[esp+0xc]` in the FPO frame.

### 10.2 `stream.c` (one function ported, others inlined upstream)

| addr       | function          | size | match% | residual diff                       |
|------------|-------------------|------|--------|--------------------------------------|
| `0x456850` | `mad_stream_sync` | 83 B | 94.2%  | orig uses esi for stream ptr (saved  |
|            |                   |      |        | via `push esi`); mine picks ecx and  |
|            |                   |      |        | skips the push/pop.  Functionally    |
|            |                   |      |        | identical; ~5 bytes of diff.         |

`mad_stream_init`, `mad_stream_buffer`, `mad_stream_skip`,
`mad_stream_finish`, `mad_stream_errorstr` all got inlined into their
callers in the orig (verified: no standalone symbols at those entries).

### 10.3 `synth.c` (two of two init-side functions matched; DCT32 pending)

| addr       | function          | size  | match%   | residual diff                            |
|------------|-------------------|-------|----------|------------------------------------------|
| `0x4568b0` | `mad_synth_mute`  |  84 B | 88.7%    | same MSVC8 loop-alignment NOP pattern as |
|            |                   |       |          | `mad_frame_mute`: 9 bytes of `lea`/`mov` |
|            |                   |       |          | filler at three loop heads, plus a `eax` |
|            |                   |       |          | vs `esi` register swap on the s-counter. |
| `0x458eb0` | `mad_synth_init`  |  44 B | **100%** | -                                        |
| `0x458f20` | `mad_frame_finish`|  36 B | 99.5%    | byte-identical; objdiff reports a       |
|            | (in `_Globals.cpp`)|      |          | mismatch on the `call _free` symbol      |
|            |                   |       |          | name (orig uses the address-suffixed     |
|            |                   |       |          | overload `_free_00447392`, mine resolves |
|            |                   |       |          | to the plain `_free`).  Relocation       |
|            |                   |       |          | metadata only, no code byte differs.     |

`mad_synth_init` matches byte-exact even though libmad's source emits
`synth->pcm.channels = 0; synth->pcm.length = 0;` as two ushort fields;
MSVC8 keeps them as two distinct 16-bit stores rather than folding into
one 32-bit store, matching the orig.

The big synth.c entry point -- `mad_synth_frame_dct32_full` (`0x456910`,
4481 B) -- is still on the queue.  It's libmad's polyphase synthesis
butterfly with 31 baked-in `MAD_F(...)` cosine constants
(`g_anMadDct32Costab` at `0x0048af70`); essentially a fully-unrolled DCT-IV
that should compile byte-exact from `synth.c::dct32()` once the
`OPT_DCTO` build flag is locked down (§9.2).

### 10.5 `frame.c` (three of three relevant functions, all matched)

| addr       | function          | size  | match%   | residual diff                            |
|------------|-------------------|-------|----------|------------------------------------------|
| `0x458ee0` | `mad_header_init` | 56 B  | **100%** | -                                        |
| `0x4595a0` | `mad_frame_mute`  | 106 B | 89.1%    | orig uses esi for outer-loop counter and |
|            |                   |       |          | edi for the zero; mine has them swapped. |
|            |                   |       |          | Plus two `lea esp, [esp]` 4-byte NOPs    |
|            |                   |       |          | that MSVC8 places at slightly different  |
|            |                   |       |          | offsets to align the inner loop head.    |
| `0x459620` | `mad_frame_init`  | 33 B  | **100%** | -                                        |

The `mad_header_init` port is byte-exact only because we load
`mad_timer_zero` (`DAT_0049c54c`/`DAT_0049c550`) via two `*reinterpret_cast`
reads instead of emitting two immediate-zero stores.  Without that the
diff is 6 bytes (`mov [eax+0x24], ecx; mov [eax+0x28], ecx` vs the orig's
`mov ecx, [DAT_0049c54c]; mov [eax+0x24], ecx; mov edx, [DAT_0049c550];
mov [eax+0x28], edx`).  libmad's source actually writes
`header->duration = mad_timer_zero;` so the orig is preserving the load
because `mad_timer_zero` is an `extern const` in another TU and MSVC8
can't const-propagate it without WPO.

### 10.6 Aggregate

```
CDSMpx .text section match: 6.36%  (17996 B total)
```

The numerator counts the twelve `CDSMpx::mad_*` ports above (six `bit.c`
+ one `stream.c` + two `synth.c` init helpers + three `frame.c` init
helpers).  `mad_frame_finish` lives in `_Globals.cpp` and is counted
separately.  The denominator includes ~12 KB of Bulanci-specific glue
(`AttachBitstream`, `RefillInputBuffer`, `DecodeFrame`,
`ResolveResource`, `CreateFromHandle`, etc.) and ~5 KB of the big libmad
work functions (`mad_layer_I`, `mad_layer_II`, `mad_layer_III`,
`mad_synth_frame_dct32_full`, `mad_header_decode`) which are next on the
porting queue.

### 10.7 Next targets (by ascending size)

| addr       | function                       | size   | notes                                |
|------------|--------------------------------|--------|--------------------------------------|
| `0x4567b0` | (unrenamed `FUN_004567b0`)     |  69 B  | small bit-twiddler near `mad_stream_sync`; possibly `mad_stream_buffer`/`mad_stream_skip` |
| `0x456820` | (unrenamed `FUN_00456820`)     |  46 B  | three-arg helper next to `mad_stream_sync` |
| `0x45c480` | `mad_layer_I_sample`           |  88 B  | static helper; Ghidra recovers args via EAX+EDI (custom MSVC8 reg convention); needs naked/asm or alternate strategy |
| `0x458f50` | `mad_header_decode`            | 472 B  | sysparam in ESI (header*); standard libmad logic, needs custom calling convention for byte match |
| `0x45c100` | `mad_layer_III`                | 895 B  | Layer III decoder body (Huffman side-info only; main tables in static helpers) |
| `0x45c4e0` | `mad_layer_I`                  | 786 B  | Layer I decoder; calls `mad_layer_I_sample` heavily |
| `0x45c800` | `mad_layer_II_samples`         | 188 B  | Layer II requantizer |
| `0x45c8c0` | `mad_layer_II`                 | 1675 B | Layer II decoder |
| `0x456910` | `mad_synth_frame_dct32_full`   | 4481 B | the polyphase DCT32 butterfly; 31 baked-in cosine constants from `g_anMadDct32Costab` |
