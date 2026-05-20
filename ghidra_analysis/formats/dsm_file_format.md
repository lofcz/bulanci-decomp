# `CDSDsmFile` -- synchronized MJPEG + PCM movie container

**TL;DR.** `CDSDsmFile` (engine class id `0x4c` = `76`) is the engine-side
reader for **ClassID 76 = `BitmapJpegAnim`** resources -- the master pack's
**Motion-JPEG + 16-bit PCM movies** used for character/cutscene animations.
The on-disk format is a fixed 36-byte header followed by `2 * frameCount`
interleaved `{u32 len, byte[len] body}` chunks: video chunk `2k` is one full
JPEG (`FF D8 FF DB ...` → `FF D9`) and audio chunk `2k+1` is one frame's
worth of raw little-endian 16-bit PCM samples. Audio mono/stereo and
sample rate are configurable per file (22050/44100 Hz, 1 or 2 channels
observed). Every byte of the header has been decoded against the four
shipping `BitmapJpegAnim` resources in `unpacked/overlay/` and the
calculated `audio_bytes` matches the on-disk `dwAudioByteCount` field
within sample-alignment rounding for all four.

The "multi-stream" half of the class name refers to the runtime side:
`CDSDsmFile` exposes *N* concurrent reader/writer handles (each backed by
its own `CDSMemQueue` ring buffer) so the audio mixer and the video
renderer can both pull from the same `.dsm` payload independently, with
the source stream cursor wrapping back to file offset `0x24` (== end of
header) when the payload boundary is reached.

The class lives in `bulanci.exe` at `Engine.DS.Resource` (31 funcs / 6
vftables / 2 061 bytes, span `0x004289a0 .. 0x0043c79a`). All on-disk
facts in this document are cross-verified against (a) the disassembly of
`HandleOpenStream` / `HandleResourceRead` / `HandleRecordRead` and (b)
the four `res_*_76_BitmapJpegAnim.bin` files in the unpacked master pack.

## `CDsmHeader` -- 36-byte fixed header

```
offset  size  field                  meaning
+0x00   u32   dwPayloadEndOffset     total file size; the cursor wraps to 0x24 when it hits this
+0x04   u32   dwCanvasWidth          video width  (pixels)
+0x08   u32   dwCanvasHeight         video height (pixels)
+0x0c   u32   dwPixelFormat          CDSImage format index (5 = 24bpp BGR, 6 = 32bpp BGRA)
+0x10   u32   dwDurationMs           total media duration in milliseconds
+0x14   u32   dwFrameCount           number of video frames (== number of audio chunks)
+0x18   u32   dwAudioByteCount       total PCM bytes across all audio chunks
+0x1c   u32   dwAudioFormatPacked    (bitsPerSample << 16) | channels  (0x00100001 = 16-bit mono)
+0x20   u32   dwAudioSampleRate      PCM rate in Hz (e.g. 22050 or 44100)
```

A Ghidra struct named `CDsmHeader` with exactly these fields/offsets has
been created via `create_struct` (see "Renames / structs applied" below).

### Empirical verification

`unpacked/overlay/` ships four `BitmapJpegAnim` resources. For each
file the audio-bytes prediction
`(dwDurationMs * dwAudioSampleRate * channels * bitsPerSample) / 8 / 1000`
matches `dwAudioByteCount` to within sample-alignment (≤ 2 bytes):

| file | size | W × H | format | dur_ms | frames | fps | audio bytes (calc) | ch / bits | rate |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| `res_0000077825_76_BitmapJpegAnim.bin` | 1 777 053 | 166 × 154 | 5 (24bpp) | 12 012 | 180 | 14.99 | 529 730 (529 729) | 1 / 16 | 22 050 |
| `res_0000077827_76_BitmapJpegAnim.bin` | 2 961 334 | 166 × 153 | 5 (24bpp) | 12 416 | 149 | 12.00 | 2 190 184 (2 190 182) | 2 / 16 | 44 100 |
| `res_0000077829_76_BitmapJpegAnim.bin` | 2 372 182 | 166 × 154 | 5 (24bpp) | 19 152 | 287 | 14.99 | 844 604 (844 603) | 1 / 16 | 22 050 |
| `res_0000077831_76_BitmapJpegAnim.bin` | 2 105 512 | 166 × 153 | 6 (32bpp) | 13 200 | 396 | 30.00 | 582 120 (582 120) | 1 / 16 | 22 050 |

`dwPayloadEndOffset == file size` is exact for all four. Walking the
chunk stream starting at offset `0x24` consumes the file with zero
leftover bytes and yields exactly `dwFrameCount` JPEG chunks plus
`dwFrameCount` non-JPEG (PCM) chunks; the non-JPEG bytes sum to
`dwAudioByteCount`. See
`tools/bulanci_unpack/ghidra_analysis/_scratch_dsm_header.py` and
`_scratch_dsm_payload.py` for the verification scripts.

## Chunk stream

```
+0x24 ........  while (cursor < dwPayloadEndOffset) {
                    u32 len;                          // little-endian
                    byte[len] body;                   // exactly len bytes
                }
```

Empirically each pair of chunks corresponds to one video frame:

```
chunk 2k     body[0..3] == FF D8 FF DB  -- a complete JPEG (SOI..EOI)
chunk 2k+1   body         == raw little-endian PCM for that frame's duration
                            (size ≈ rate * channels * 2 / fps; aligned up to 2)
```

The JPEGs are decoded by `HandleRecordRead` (vtable2 slot 7) which
spin-waits for a buffered chunk, peeks the 4-byte length prefix, builds a
sliced `CDSQueueStream` view over the body, then hands the view to the
generic JPEG decoder `FUN_00431b70` (libjpeg-style:
`jpeg_create_decompress → jpeg_read_header → jpeg_start_decompress →
jpeg_read_scanlines` with a per-row R↔B swap so the output is 24bpp BGR).
The decoder ignores `dwPixelFormat` / `dwCanvasWidth` / `dwCanvasHeight`
in favour of the JPEG's own SOF header values; the canvas fields exist so
the caller can pre-allocate a `CDSImage` of the correct shape via
`InitializeChildObject` (vtable2 slot 9) before the first frame arrives.

The audio chunks flow through the same per-handle ring buffer and are
consumed by a separate reader handle on the audio side; nothing inside
`CDSDsmFile` decodes them (they are already raw PCM).

## Class layout (object size = `0x68` bytes)

Allocation site: `InitializeAndAllocate` (`0x00429040`), which does
`malloc(0x68)` then jumps to `InitializeMembers` (`0x00428d40`). The 12
base classes in the RTTI chain collapse into **6 distinct vftables**:

| Sub-object | Offset | Slots | Interface (per `GetClassMeta` return) | Notable slots |
|---|---:|---:|---|---|
| vtable1 | `+0x00` | 3   | `CDSDsmFile` self | `0` = `GetClassRegistry` (`0x00428db0`) |
| vtable2 | `+0x04` | 10  | `IDSImageSource`-shaped | `5` = `HandleAcquireWriteThunk`, **`7` = `HandleRecordRead` (`0x00428c40`)**, `8` = `HandleReleaseWriteThunk`, **`9` = `InitializeChildObject` (`0x00439ac0`)** |
| vtable3 | `+0x1c` | 8   | `CDSWav`-shaped | `5` = `HandleAcquireReadThunk`, **`6` = `HandleResourceRead` (`0x00428ad0`)**, `7` = `HandleReleaseReadThunk` |
| vtable4 | `+0x30` | 4   | `CDSFileStream`-shaped | thunks + class-meta getter only |
| vtable5 | `+0x38` | 6   | `CDSChain`-shaped (`IDSStorage`) | **`4` = `HandleOpenStream` (`0x00428f80`)** |
| vtable6 | `+0x3c` | 5   | `CPoem`-shaped (`IDSReferenced`) | thunks + class-meta getter only |

Each vtable slot `0` returns the class-metadata pointer for the interface
it represents (`CDSWav` / `CDSChain` / `CPoem` etc.); the engine uses
these as runtime type tags for `CheckedVirtualBaseCast` (`0x0042e9f0`).

### Data fields

```
+0x00   void*           vtable1
+0x04   void*           vtable2
+0x08 . CDsmHeader      embedded header struct A (dwCanvasWidth / Height / PixelFormat / DurationMs / FrameCount)
+0x1c   void*           vtable3
+0x20 . CDsmHeader      embedded header struct B (dwAudioByteCount / dwAudioFormatPacked / dwAudioSampleRate)
+0x2c   void*           pBackref            // runtime-only; cleared by FUN_00434250 on dtor
+0x30   void*           vtable4
+0x34   u32 = 1         stride field of the embedded handle-collection at +0x54
+0x38   void*           vtable5
+0x3c   void*           vtable6
+0x40   void*           pSubObjStash        // cleared by FUN_00434250 on dtor
+0x44   IDSStream*      src                 // backing stream (AddRef'd at Open)
+0x48   u64             srcBaseOffset       // = src->Tell64() captured at Open
+0x50   u32             dwPayloadEndOffset  // FIRST 4 BYTES of the file (== sizeof(file))
+0x54 . CDSCollection<Handle*>  embedded handle bank (12 bytes: ptr, ?, count)
+0x60   u32             handleBank.elemStride = 8
+0x64   u32             (tail / padding)
```

Note that the file-header fields straddle the vtable3 pointer at
`+0x1c`. This is by design: when `HandleOpenStream` is dispatched via
vtable5 the function's `this` register holds `obj+0x38`, so the reads
into "struct A" are `[esi-0x30]` (= `obj+0x08`) and "struct B" are
`[esi-0x18]` (= `obj+0x20`). The reads stop exactly at the boundaries
that would touch the vtable3 / vtable5 / vtable6 pointers, so the vtable
layout never gets corrupted.

The whole object is `0x68` bytes -- matches the `operator new(0x68)`
call in `InitializeAndAllocate`.

## On-disk header (40 bytes, but only 36 read into the object)

`HandleOpenStream` issues **three** `IStream::Read` calls in sequence
(see disassembly at `0x00428fbd / 0x00428fcd / 0x00428fdd` -- each call
site now carries an inline disassembly comment in Ghidra):

```
+0x00 .. +0x03   u32  dwPayloadEndOffset    -> obj+0x50
+0x04 .. +0x17   byte[20]  CDsmHeader struct A  -> obj+0x08
+0x18 .. +0x23   byte[12]  CDsmHeader struct B  -> obj+0x20
+0x24 ........   chunk stream
```

Total header = 36 bytes = `0x24`. The chunk stream begins at the same
`0x24` offset that `HandleResourceRead` (`0x00428ad0`) wraps the cursor
back to on every full pass through the payload.

## Multi-stream layout (the part the name actually means)

A `CDSDsmFile` instance exposes **two virtual interfaces** for clients:

* **vtable3 (IDSReadStream)** -- read side. Slot 5 acquires a read
  handle, slot 6 (`HandleResourceRead`, `0x00428ad0`) reads bytes from
  it, slot 7 releases the handle.
* **vtable2 (IDSImageSource-ish)** -- write side. Slot 5 acquires a write
  handle, slot 8 releases it. Slot 7 (`HandleRecordRead`) decodes the
  next buffered JPEG chunk into a destination `CDSImage`; slot 9
  (`InitializeChildObject`) hands the caller a fresh empty `CDSImage`
  sized for the header. The legacy "persistence" slot at vtable2 slot 4
  is a no-op stub (`CDSApp::FUN_00467430`), so the shipping game treats
  the format as read-only at the storage level.

Each Acquire call returns a **handle** -- a `0x30`-byte struct allocated
inside `HandleAcquireResource` (`0x004290c0`) -- which is registered on
the handle bank at `obj+0x54`. Layout:

```
struct CDSDsmFile_Handle {  // 0x30 bytes
    +0x00  byte  flagByte;        // bit 0 = reader, bit 1 = writer
    +0x01  byte  pad[3];
    +0x04  u32   cursor;          // current offset in the resource (init = 0x24)
    +0x08  u32   chunkBytesLeft;  // bytes still readable from current chunk
    +0x0c  CDSMemQueue queue;     // 0x20 bytes; ring buffer for the chunk bodies
    +0x2c  u32   chunkCounter;    // bumped on every payload wrap; spun-on by HandleRecordRead
};
```

`HandleAcquireResource` initialises the queue with
`CDSMpx::Configure(..., capacity=0x100000 /* 1 MiB */, step=0x80000
/* 0.5 MiB */, flag=1)`, so each handle's write-back buffer can grow
to a megabyte before re-allocation.

The Acquire-side has a small optimisation: if the **most recently
acquired** handle still has the requested flag bit clear, it OR's the
flag into that handle and returns it -- this is how a reader+writer
pair end up sharing one queue. See the inline comment on
`HandleAcquireResource`'s plate.

`HandleReleaseResource` (`0x00428ff0`) clears the requested flag from
`flagByte`; when both reader and writer bits are zero the handle is
unhooked from the bank and `free`d.

### Read path (per handle, `nBytes` requested)

```c
src->Seek(this->srcBaseOffset + handle->cursor);
while (nBytes != 0) {
    available = this->dwPayloadEndOffset - handle->cursor;
    n = min(nBytes, available);
    if (n != 0) {
        src->Read(scratch, n);
        CDSQueueStream::WriteFromStream(handle->queue, src, n);
        handle->cursor += n;
        nBytes        -= n;
    }
    if (handle->cursor == this->dwPayloadEndOffset) {
        handle->cursor = 0x24;
        handle->chunkCounter++;        // signal HandleRecordRead that the payload looped
        src->Seek(this->srcBaseOffset + 0x24);
    }
    if (n == 0 && nBytes > 0) break;   // source exhausted
}
```

The function does **not** parse chunk-length prefixes -- it just feeds
raw bytes from the source stream into the handle's queue. The chunk
boundaries are reconstructed downstream by `HandleRecordRead`
(`0x00428c40`), which peeks the next 4-byte length prefix off the queue
and slices the next `len` bytes out as a single record. This is the
hook the audio and video consumers attach to.

## Engine class-factory registration

`CDSDsmFile` is registered with engine class id `0x4c` (= 76 decimal,
matching the master pack's `ClassID 76 = BitmapJpegAnim`) at static
initialisation. Relevant addresses:

| Address | Role | Renamed to |
|---|---|---|
| `0x0047c30c` | static `CClassRegistration` block for CDSDsmFile (`push factory; push namePtr; push id=0x4c; mov ecx, registry; call register`) | --- |
| `0x00429040` | factory thunk: `malloc(0x68); InitializeMembers(obj)` | `InitializeAndAllocate` |
| `0x00428d40` | most-derived ctor body | `InitializeMembers` |
| `0x0042e910` | linked-list push of a registration node | `HandleClassRegister` |
| `0x0042ea20` | flat-table install (`g_apClassByIdTable[id] = node`) | `InitializeClassIdLookup` |
| `0x0042ef00` | class-id lookup + factory invocation | `InitializeByClassId` |
| `g_pClassRegistryDsm` (`0x004b3a64`) | per-class registry head used by `CDSDsmFile::GetClassRegistry` (`0x00428db0`) | --- |
| `g_apClassByIdTable` (`0x004b3be0`) | engine-wide `void*[0x1000]` lookup table | --- |
| `g_pClassRegHead` (`0x004b7be4`) | linked-list head of every registration | --- |
| `g_pCDSDsmFileClassName` (`0x004b7bfc`) | BSS slot for the runtime class name string | --- |
| `g_pCDSDsmFileMetaA` (`0x004b836c`) | misc class-metadata block | --- |

Typical caller: `CDSStreamStorage::FUN_00434760` (`0x00434760`) hands a
windowed stream to a just-built `CDSDsmFile` via `IDSStorage::Open`
(vtable5 slot 4). The master-pack reader builds a `CDSFilterStream(off,
len) + CDSSafeStream(...)` around the raw `BitmapJpegAnim` payload bytes
and lets `HandleOpenStream` parse the 36-byte header from there.

### Why `BULANCI.TMP` is **not** a DSM file

The master pack carries one `DsmInner` (ClassID 58) entry whose inner
filename is `BULANCI.TMP` (610 304 bytes; manifest at
`unpacked/overlay/res_0000065554_58_DsmInner.bin`). Parsing the first 36
bytes through the `CDsmHeader` schema yields `dwPayloadEndOffset = 0`
and a literal `MZ` PE signature inside struct A -- it's actually an
**8-byte-prefixed PE executable** (legacy installer/launcher stub), not a
movie. ClassID 58 `DsmInner` is a generic named-file wrapper unrelated to
`CDSDsmFile`; the name collision is coincidental.

## Renames / structs applied via Ghidra MCP

### Struct

* `CDsmHeader` -- 36-byte struct with the 9 fields listed above, created
  via `create_struct`. Apply to `obj+0x08` (struct A) and `obj+0x20`
  (struct B) when you want the decompiler to print field names instead
  of `[esi - 0x30]` / `[esi - 0x18]`.

### Function renames

| Address | Old | New |
|---|---|---|
| `0x004289a0` | `FUN_004289a0` | `InitializeQueueStreamView` |
| `0x00428ad0` | `FUN_00428ad0` | `HandleResourceRead` |
| `0x00428c40` | `FUN_00428c40` | `HandleRecordRead` |
| `0x00428d40` | `FUN_00428d40` (new fn) | `InitializeMembers` |
| `0x00428da0` | `FUN_00428da0` | `GetClassMetaA` |
| `0x00428db0` | `FUN_00428db0` | `GetClassRegistry` |
| `0x00428e40` | `FUN_00428e40` | `HandleRefcountRelease` |
| `0x00428ec0` | `FUN_00428ec0` | `HandleDestructInstance` |
| `0x00428f80` | `FUN_00428f80` | `HandleOpenStream` |
| `0x00428ff0` | `FUN_00428ff0` | `HandleReleaseResource` |
| `0x00429040` | `FUN_00429040` (new fn) | `InitializeAndAllocate` |
| `0x00429060` | `FUN_00429060` | `HandleScalarDelete` |
| `0x00429080` | `FUN_00429080` | `HandleReleaseReadThunk` |
| `0x004290a0` | `FUN_004290a0` | `HandleReleaseWriteThunk` |
| `0x004290c0` | `FUN_004290c0` | `HandleAcquireResource` |
| `0x00429190` | `FUN_00429190` | `HandleAcquireReadThunk` |
| `0x004291b0` | `FUN_004291b0` | `HandleAcquireWriteThunk` |
| `0x00439ac0` | `FUN_00439ac0` | `InitializeChildObject` |
| `0x0043bde0` | `FUN_0043bde0` | `InitializeSourceRegion` |
| `0x0043c2e0` | `FUN_0043c2e0` | `HandleQueueSkip` |
| `0x0043c450` | `FUN_0043c450` | `HandleQueueResize` |
| `0x0043c620` | `FUN_0043c620` | `HandleQueueWrite` |
| `0x0043c6f0` | `FUN_0043c6f0` | `HandleQueueAppendStream` |
| `0x0042e910` | `FUN_0042e910` | `HandleClassRegister` |
| `0x0042e9a0` | `FUN_0042e9a0` | `HandleVirtualBaseCast` |
| `0x0042e9f0` | `FUN_0042e9f0` | `CheckedVirtualBaseCast` |
| `0x0042ea20` | `FUN_0042ea20` | `InitializeClassIdLookup` |
| `0x0042ef00` | `FUN_0042ef00` | `InitializeByClassId` |
| `0x004b3a64` | `DAT_004b3a64` | `g_pClassRegistryDsm` |
| `0x004b3be0` | `DAT_004b3be0` | `g_apClassByIdTable` |
| `0x004b7be4` | `DAT_004b7be4` | `g_pClassRegHead` |
| `0x004b7bfc` | `DAT_004b7bfc` | `g_pCDSDsmFileClassName` |
| `0x004b836c` | `DAT_004b836c` | `g_pCDSDsmFileMetaA` |

### Plate / disassembly comments

Plate comments were added on `InitializeAndAllocate`, `InitializeMembers`,
`HandleOpenStream`, `HandleResourceRead`, `HandleRecordRead`,
`HandleAcquireResource`, `HandleReleaseResource`,
`HandleDestructInstance`, and `InitializeChildObject`. Inline
disassembly comments now annotate the three `Read` calls and the
`Tell64` call inside `HandleOpenStream` (at `0x00428fa7`, `0x00428fbd`,
`0x00428fcd`, `0x00428fdd`).

## What is still unknown

| Question | Best lever |
|---|---|
| Does the engine ever issue a `WriteRecord` on a writer handle in the shipping game (i.e. is the writer path actually used at runtime)? | Trace `HandleAcquireWriteThunk` (`0x004291b0`) callers and check whether `HandleQueueWrite` / `HandleQueueAppendStream` are reached from gameplay code or only from internal helpers. |
| Whether `dwPixelFormat = 6` (32bpp) files contain 32bpp JPEGs or whether the JPEG decoder still produces 24bpp and the engine pads to 32bpp at consume time. The lone format-6 file (`res_0000077831`) parses fine with the 24bpp decoder. | Decode the first frame of `res_0000077831` and compare with the engine's `CDSImage::FUN_004362f0` stride math. |
| Which gameplay event triggers each of the four shipping animations. | Search the scripts (ClassID 2026) for the resource IDs `77825 / 77827 / 77829 / 77831`. |
