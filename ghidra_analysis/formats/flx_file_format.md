# `CDSFlxFile` -- the FLX animation container

**TL;DR.** The on-disk format read by `CDSFlxFile` is **not stock Autodesk
FLI/FLC**. It is a **Bulanci-original variant** whose decoders are
clearly FLI-inspired (the BRUN run/literal opcode polarity is the same,
LC's "skip + RLE" delta is the same idea) but every container-level
detail differs: the file header is 36 bytes (FLI is 128, FLC is 128
with magic `0xAF11`/`0xAF12`), there is no frame magic word
(`0xF1FA`/`0xF1FB`), chunks are 5 bytes of header instead of 6, and the
chunk-tag namespace is the dense set `{0, 4, 8, 9, 10, 11, 12, 13, 14, 15}`.
Some of those tag *numbers* coincide with stock FLI/FLC chunk IDs --
e.g. 4, 11, 12, 13, 15 -- but the **semantics never match**: tag 4 is
a delta (not `COLOR_256`), tag 11 is a region list (not `COLOR_64`),
tag 15 is the *mask*-plane BRUN (not the color-plane BRUN id 15).
Only tag 9 (palette) actually replicates a stock FLI/FLC chunk by
shape (FLI `COLOR_256`, id 4) -- but renumbered to 9. Tags 12--15 and
the parallel mask plane have no FLI/FLC counterparts.

Reverse-engineered live in `bulanci.exe` against the master-pack
samples; addresses below are stable RVAs into the committed
`orig/bulanci.exe`. All 27 functions of `CDSFlxFile` are accounted for
and have been renamed in Ghidra (no remaining `FUN_xxxxxxxx` in the
class).

The companion BitmapSprite (ClassID 52) container that wraps a FLX
payload is documented separately in `sprite_container.md`; this file
covers only the FLX byte-stream and the engine class that parses it.

### How this lines up with `sprite_container.md`

`sprite_container.md` and this file describe the **same byte stream**
from two layers. The terminology mapping is:

| sprite_container.md term            | FLX-level term (this file)                          |
|---|---|
| 0x2c-byte BitmapSprite header        | 0x24-byte FLX header + 8 bytes of frame-0 head      |
| "chunk" (`{0,1,2,3,4,5}` outer tags) | **frame** (its outer tag is the FLX `chunkCount`)    |
| "inner chunk" (`proto_decoder.py`)   | **FLX chunk** (its tag is the dispatcher key)        |
| `flags = chunkCount - 1`             | total number of frames minus one                     |

In other words the sprite_container.md "tag distribution" table
(`{0:449, 1:1365, 2:624, 3:194, 4:49, 5:14}`) is really a **histogram
of frame chunk-counts**: 449 empty frames, 1365 single-chunk frames,
624 two-chunk frames, etc. Empty-body frames (chunkCount = 0) are
"reuse previous frame" hints; they are emitted by exporters when a
frame is byte-identical to the previous one.

## File layout

```
+0x00 .. +0x23   nine u32 header fields (read by BindStream at 0x432ac0)
+0x24 ........   frame stream: back-to-back frames until EOF
```

The header is read in a single `IStream::Read(buffer, 0x24)` call and
its dwords are then scattered into the outer `CDSFlxFile` object. We
have not yet reverse-engineered every field but the dispatcher tells
us:

| outer offset | role (proven `CDSFlxFile_BindStream` @ `0x00432ac0`) |
|---:|---|
|  +0x08 .. +0x18 | five geometry/resource dwords from the 36-byte read (`nChannels` .. `nAnimFrameCountMinusOne`) |
|  +0x30 / +0x34 | source-stream `Tell()` snapshot (u64) captured **after** the header read — byte offset of the first frame |
|  +0x38        | source `IDSStream*` pointer (`pSourceStream`)       |
|  +0x3c        | scratch decode-buffer pointer (`pDecodeBuffer`, alloc'd lazily) |
|  +0x40        | `bodyStartCursor` — dword 1 from the header read   |
|  +0x44        | `bodyEndCursor` = stream length minus `0x24`       |
|  +0x48        | `dwTotalSize` — dword 0 from the header read (`== len(raw)`) |
|  +0x4c        | `dwEncodedSize2` — dword 2 from the header read (usually equals `bodyStartCursor`) |

| file header offset | outer offset | field |
|---:|---:|---|
|  +0x00 | +0x48 | `dwTotalSize` |
|  +0x04 | +0x40 | `bodyStartCursor` |
|  +0x08 | +0x4c | `dwEncodedSize2` |
|  +0x10 | +0x10 | `nBitmapWidth` |
|  +0x14 | +0x0c | `nBitmapHeight` |
|  +0x18 | +0x08 | `nChannels` |
|  +0x1c | +0x14 | `nSeqTotalDurationMs` (file alias `inMemSize` / legacy `nInMemSizeHint`; often `0x470`) |
|  +0x20 | +0x18 | `nSeqFrameCountMinusOne` (file alias `flags`; `frameCount - 1`) |

(`+0x28` is the `vf_chain` vtable slot from the factory, not a header field.)

The stream cursor is **circular**: at the end of a frame, if the next
read would go past `outer+0x44` (`bodyEndCursor`), the cursor wraps to `outer+0x3c`
(`DecodeFrame` final block). This is what makes the format loopable
without a separate loop bit.

## Frame layout

Each frame in the body is

```
+0x00  u32 frameSize     ; total bytes incl. this header
+0x04  u8  chunkCount    ; number of chunks immediately following
+0x05  u8[frameSize - 5] ; chunkCount back-to-back chunk records
```

There is **no frame magic word** (compare FLI: `u32 size; u16 0xF1FA;
u16 chunks; u32 reserved` = 16 bytes). Compactness is deliberate; the
chunk count fits in one byte because Bulanci frames never exceed a few
chunks (mostly 1--3, max observed in the BitmapSprite master pack is 6).

## Chunk layout

```
+0x00  u32 chunkSize     ; total bytes incl. this header
+0x04  u8  tag           ; one of {0,4,8,9,10,11,12,13,14,15}
+0x05  u8[chunkSize - 5] ; body (decoder-specific; see table)
```

Compared to stock FLI/FLC's 6-byte chunk header (`u32 size; u16 type;`)
this saves one byte per chunk and limits the tag namespace to 256
values.

### Chunk dispatch table (CDSFlxFile::DecodeFrame @ `0x00432c60`)

| tag | name                | dst                          | decoder                  | rough FLI/FLC analogue |
|---:|---|---|---|---|
|  0  | RleColor            | color plane (`+0x1c`)        | `DecodeRleColor`         | FLI BRUN (id 15)        |
|  4  | DeltaColor          | color plane (`+0x1c`)        | `DecodeDeltaColor`       | FLI LC (id 12)          |
|  8  | CopyColor           | color plane (`+0x1c`)        | memcpy                   | FLI COPY (id 16)        |
|  9  | PaletteRgb          | consumer palette (`+0x1c`, 4B stride) | `DecodePaletteRgb` | **FLI COLOR_256 (id 4)** |
| 10  | Move                | (observer broadcast)         | inline                   | -- (sub-image position) |
| 11  | RegionList          | (observer broadcast)         | `DecodeRegionList`       | -- (dirty-rect hint)    |
| 12  | FrameDuration       | (observer broadcast)         | inline (u16 ms/ticks)    | -- (frame timing)       |
| 13  | TransKeyIndex       | consumer `+0x18` (u8)        | inline                   | -- (transparency idx)   |
| 14  | RleMask             | mask plane (`+0x20`, lazy)   | `DecodeRleColor`         | FLI BRUN (id 15)        |
| 15  | DeltaMask           | mask plane (`+0x20`)         | `DecodeDeltaColor`       | FLI LC (id 12)          |

After all chunks of the frame are processed, if at least one
pixel-writing tag fired (`0/4/8/9/14/15`), the consumer's
`NotifyRect(0,0,w,h)` is called -- so a redraw is triggered exactly
once per frame, regardless of how many sub-chunks contributed.

Notes about the analogy column:

* **The tag IDs themselves do not match FLI/FLC.** They are densely
  packed into 0..15, not the sparse `{4,7,11,12,13,15,16,18}` of stock
  FLI/FLC.
* **Tag 9 (palette) is the only chunk that mirrors stock FLI/FLC by
  shape.** It uses the same "skip + count + RGB triples" record format
  as FLI `COLOR_256` (id 4) -- but with the chunk tag renumbered to 9
  and writes RGB into a 32-bit BGRA palette slot (leaving the alpha
  byte untouched at the slot's prior value).
* **There is a separate mask plane** (tags 14, 15) -- stock FLI/FLC has
  no concept of a parallel alpha/mask channel. The mask plane is
  allocated on demand (`AllocMaskPlane @ 0x00436ff0`) and is
  `width * height` bytes (8bpp).
* **Frame timing rides inside the chunk stream** (tag 12, u16 in
  ms/ticks per frame). Stock FLI/FLC put the speed in the file header.

## Decoder algorithms

### `DecodeRleColor` @ `0x00432740` (tags 0, 14)

```text
while src < src + n:
    N = next u8 read as i8
    if N == 0     : 4-byte op: count = next u16; v = next u8; emit (count+1)*v
    if  1..127    : 2-byte op: v = next u8;                    emit (N+1)*v
    if -1..-127   : (256-N) literal bytes follow; copy verbatim
```

Stock FLI BRUN has no long-run escape; Bulanci added the `N==0` form
for runs longer than 128. This was already cross-verified against the
130-sample master pack via the regression script
`tools/bulanci_unpack/ghidra_analysis/proto_decoder.py`, which re-runs
the production `_flxrle_decode` / `_flxdelta_apply` / `_flxpalette_apply`
implementations from `bulanci_unpack.py` and confirms every keyframe
chunk decompresses to exactly `bitmapWidth * bitmapHeight` bytes
(538/538 chunks across the pack).

### `DecodeDeltaColor` @ `0x00432780` (tags 4, 15)

Skip-aware delta packets, repeated to end of body:

```text
while src < src + n:
    S = next u8
    if S == 0xff : 3-byte skip: skip = next u16; dst += skip
    else         : 1-byte skip: dst += S
    N = next u8 read as i8
    if N == 0    : 1+0 (no-op separator)
    if N == 0x80 : 3-byte op: c = next i16;
                    if c >= 0: emit (c+1) copies of next u8
                    if c <  0: copy (-c) literal bytes
    if  1..127   : 2-byte op: emit (N+1) copies of next u8
    if -1..-127  : (256-N) literal bytes follow
```

Same opcode polarity as `DecodeRleColor` but each packet is preceded
by a skip count and the long-run form uses a signed s16 (not u16) so
it can also encode "long literal run" via a negative count.

### `DecodeCopyColor` @ `0x004327e0` (tag 8)

Pure `msvcrt::memcpy(dst, src, n)`. Equivalent to FLI/FLC `FLI_COPY`.

### `DecodePaletteRgb` @ `0x00432800` (tag 9)

24-bit RGB palette update with stock FLI/FLC `COLOR_256` semantics; the
chunk tag is 9 (instead of FLI's 4). A single cursor walks the palette
buffer across the entire chunk — **the first byte of each packet is a
relative skip that ADVANCES the cursor, not an absolute start index**:

```text
cursor = 0
while src < src + n:
    u8 skip         ; cursor += skip   (relative; persists across packets)
    u8 countMinus1
    repeat (countMinus1 + 1) times:
        u8 R, u8 G, u8 B   ; written as `u16 then u8` into a 4B BGRA slot,
                            ; alpha byte left untouched
        cursor += 1
```

In Ghidra (`param_3` is a `u16 *`, so `+ skip * 2` advances by
`skip * 4` bytes = `skip` BGRA entries):

```c
param_3 = param_3 + (uint)*param_2 * 2;   // cursor += skip
param_2 = param_2 + 2;
iVar2  = *pbVar1 + 1;
do {
    *param_3 = *(undefined2 *)param_2;
    *(byte *)(param_3 + 1) = param_2[2];
    param_2 = param_2 + 3;
    param_3 = param_3 + 2;
} while (--iVar2 != 0);
```

An earlier draft of this decoder mistakenly treated the first byte as
an absolute start index, which left every palette entry between
adjacent packets at its init value (white). Sphere sprites such as
65748/65749 looked uniformly white as a result. The cursor-based
reading is FLI/FLC-faithful and produces correct grayscale gradients
on those samples.

Empirically: 89/130 BitmapSprite master-pack sprites carry an inline
palette via this tag; the remaining 41/130 inherit the palette from
the most-recently-loaded inline-palette sibling (the engine keeps the
palette buffer live across consecutive sprite loads in the same
render context). The unpacker mirrors that lookup by threading a
1024-byte ambient-palette state through the per-pack `save_resource`
loop, and stamps each inheriting sprite's atlas sidecar with
`paletteSource: "inherited"` plus `inheritedFrom: <head_id>`. Only
sprites with no inline palette **and** no sibling to inherit from
(e.g. the very first sprite in a pack or a hand-authored one-off)
fall back to a 256-step grayscale ramp; the master pack contains
none of those after the inheritance resolution.

### `DecodeRegionList` @ `0x00432850` (tag 11)

Buffers up to 32 dirty-region records on the stack as
`(u8 a; s16 x; s16 y)*` and then fans them out to all observers via
`NotifyRegionList @ 0x00436eb0`. The format does not carry a count
prefix -- the body is "read records until end of chunk, at most 32".

## Class shape

`CDSFlxFile` is a multiple-inheritance MSVC class with **five vtables**
embedded inline:

| outer offset | vtable RVA   | role / interface (best guess)                    |
|---:|---:|---|
| `+0x00` | `0x004872d0` | `CDSObject` (top of class -- 3 method slots + class-info pointer) |
| `+0x04` | `0x004872a4` | secondary subobject (10 slots; **`DecodeFrame` is slot 8**, `BindStream` is slot... wait -- `BindStream` is on vtable 4) |
| `+0x1c` | `0x00487290` | tertiary subobject (4 slots)                     |
| `+0x24` | `0x00487274` | `IDSResource`-style (7 slots; **`BindStream` is slot 4**) |
| `+0x28` | `0x0048725c` | type-tag subobject (5 slots)                     |

`+0x28` is also the start of an embedded `CDSChain` subobject (proven
by the `Sub20ChainOp` / `Sub24ChainOp` adjustor thunks that forward to
`CDSChain::FUN_0042ac90`).

### Per-vtable summary

* **`v0 = 0x004872d0`** -- `CDSObject` base.
  `[0]=GetClassRegistry, [1]=ScalarDeletingDtor, [2]=ReleaseRef`.

* **`v1 = 0x004872a4`** -- secondary interface. Slots include
  `Sub04Dtor`, `Sub04Release`, `CloseStream` (slot 4), `ResetCursor`
  (slot 6), **`DecodeFrame` (slot 8)**, `CreateBoundClone` (slot 9).

* **`v2 = 0x00487290`** -- tertiary interface (4 slots).

* **`v3 = 0x00487274`** -- `IDSResource`/storage-style. Slots include
  `Sub24Dtor`, `Sub24Release`, **`BindStream` (slot 4)**, plus the
  `Sub20ChainOp` forwarder (slot 1).

* **`v4 = 0x0048725c`** -- type-tag (5 slots).

### Outer field layout (Ghidra `CDSFlxFile`, 0x50 bytes)

| outer offset | name | role |
|---:|---|---|
| `+0x00` | `vf_IDSReferenced` | `CDSObject` vtable |
| `+0x04` | `vf_metaFace` | secondary (`DecodeFrame` / `CloseStream`) |
| `+0x08` | `nChannels` | from file `+0x18`; master pack always `3` |
| `+0x0c` | `nBitmapHeight` | from file `+0x14` |
| `+0x10` | `nBitmapWidth` | from file `+0x10` |
| `+0x14` | `nSeqTotalDurationMs` | from file `+0x1c` |
| `+0x18` | `nSeqFrameCountMinusOne` | from file `+0x20` |
| `+0x1c` | `vf_event` | tertiary vtable |
| `+0x20` | `refcount` | `ReleaseRef` |
| `+0x24` | `vf_IDSResource` | `BindStream` adjustor face |
| `+0x28` | `vf_chain` | type-tag + embedded `CDSChain` |
| `+0x2c` | `bodySeekBiasLo` | factory zero only; **no** non-zero writer in `.text` (R4 todo 33) |
| `+0x30` / `+0x34` | `dwStreamTellLo` / `dwStreamTellHi` | `Tell()` after header read; used in `DecodeFrame`/`CloseStream` Seek64 (meta-face `+0x2c`/`+0x30` operands) |
| `+0x38` | `pSourceStream` | `IDSStream*` |
| `+0x3c` | `pDecodeBuffer` | lazy scratch buffer |
| `+0x40` | `bodyStartCursor` | file dword @ `+0x04` |
| `+0x44` | `bodyEndCursor` | `streamLength - 0x24` |
| `+0x48` | `dwTotalSize` | file dword @ `+0x00`; copied to decode consumer `+0x14` in `CreateBoundClone` |
| `+0x4c` | `dwEncodedSize2` | file dword @ `+0x08` |

## Open questions

1. ~~**Header dwords.**~~ **Resolved (agent todo 33 / slice 30):** all nine
   header dwords mapped in `CDSFlxFile_BindStream` disasm — see table
   above and `struct_recovery/CDSFlxFile.md`. Bitmap geometry uses
   `nBitmapWidth` / `nBitmapHeight`; `nChannels` is always `3` in the
   master pack; `nSeqTotalDurationMs` (file `+0x1c`) is commonly `0x470` (`sizeof(CBulPicture)`).
2. **Where the FLX is consumed.** `param_2` of `DecodeFrame` is an
   external "consumer" with the observer slots `+0x4`
   (`NotifyMove`-target), `+0xc` (`NotifyRegionList`-target),
   `+0x10` (`NotifyTransKey`-target). For the BitmapSprite container
   (ClassID 52) the consumer is the `CBulPicture` instance that owns
   the 1024-byte palette at `+0x6c` and the `CDSBitmap*` at `+0x68`;
   the relevant Ghidra-renamed methods are `Sub04Dtor`, `Sub04Release`
   and the `NotifyMove` / `NotifyRegionList` callbacks above. Other
   resource types (`CDSMpx`-managed video surfaces) likely use the
   same observer contract but haven't been confirmed.
4. **CreateBoundClone.** Allocates `0x60` bytes, calls
   `CDSObject::FUN_00425620`, and stores `outer+0x48` into the new
   object's `+0x14`. Reads like a "spawn a satellite that references
   this FLX file" -- possibly a per-instance playhead. Needs a caller
   sweep to confirm.
5. **`Sub20ChainOp` / `Sub24ChainOp`.** Both forward to
   `CDSChain::FUN_0042ac90`. That CDSChain method itself is still
   `FUN_`-named -- worth a Ghidra pass to learn whether it's
   `AddRef`, `Release`, `Link`, or `Unlink`. Once known, the embedded
   CDSChain at outer `+0x28` is fully typed.

## Renamed entry points

`CDSFlxFile` (all 27 methods, decoder + plumbing):

| Address    | Name                                | Role                                           |
|-----------:|-------------------------------------|------------------------------------------------|
| `0x00432740` | `CDSFlxFile::DecodeRleColor`      | BRUN-style RLE → color or mask plane           |
| `0x00432780` | `CDSFlxFile::DecodeDeltaColor`    | skip + RLE delta → color or mask plane         |
| `0x004327e0` | `CDSFlxFile::DecodeCopyColor`     | `memcpy` → color plane                          |
| `0x00432800` | `CDSFlxFile::DecodePaletteRgb`    | 24-bit RGB palette update (FLI COLOR_256-shape)|
| `0x00432840` | `CDSFlxFile::ResetCursor`         | vtable slot: clear `*param`                     |
| `0x00432850` | `CDSFlxFile::DecodeRegionList`    | parse ≤32 `(u8,s16,s16)` + broadcast            |
| `0x004328e0` | `CDSFlxFile::GetClassRegistry`    | returns `&DAT_004b7e08` (CDSResInfo registry)   |
| `0x00432950` | `CDSFlxFile::ReleaseRef`          | refcount release with chained delete            |
| `0x004329c0` | `CDSFlxFile::DestructInPlace`     | dtor: free decode buf + release stream + chain  |
| `0x00432aa0` | `CDSFlxFile::ScalarDeletingDtor`  | `Destruct + maybe free(this)`                   |
| `0x00432ac0` | `CDSFlxFile::BindStream`          | reads the 36-byte file header                   |
| `0x00432b60` | `CDSFlxFile::CloseStream`         | release stream + free decode buffer             |
| `0x00432be0` | `CDSFlxFile::CreateBoundClone`    | allocate a satellite CDSObject                  |
| `0x00432c60` | `CDSFlxFile::DecodeFrame`         | **the chunk-stream dispatcher**                 |
| `0x00436e80` | `CDSFlxFile::NotifyMove`          | broadcast position update to observers          |
| `0x00436eb0` | `CDSFlxFile::NotifyRegionList`    | broadcast region-list update to observers       |
| `0x00436ff0` | `CDSFlxFile::AllocMaskPlane`      | (re)alloc the `width * height` mask plane       |

Render-side helpers (consumer object & blit dispatch):

| Address    | Name                | Role                                                       |
|-----------:|---------------------|------------------------------------------------------------|
| `0x004360d0` | `GetPaletteBuffer`| Returns consumer palette base when `+0x34 ∈ [1, 256]`, else 0 |
| `0x004360f0` | `GetColorPlane`   | Returns consumer color plane base (`+0x1c`) offset by `+0x34 * 4` bytes |
| `0x00436e40` | `NotifyDirtyRect` | Observer fan-out: vtbl[0] of every observer with rect       |
| `0x00436ef0` | `NotifyFrameTime` | Observer fan-out: vtbl[0x10] of every observer with `u16` ticks |
| `0x00437080` | `NotifyDirtyAll`  | Build `RECT(0,0,w,h)` and fan out via `NotifyDirtyRect`     |
| `0x004368d0` | `CPoemScroller::BlitDispatch` | Master blit dispatcher — picks 1 of 5 kernel tables |
| `0x0043f470` | `BlitChromaKey`   | Source-chroma-key blit kernel                              |
| `0x004414e0` | `BlitMasked`      | Mask-plane blit kernel                                     |
| `0x0040eb30` | `CBulPicture_Create` | Allocates 0x470-byte CBulPicture; fills palette with `0xFFFFFFFF` |
| `0x0040b050` | `CBulPicture::DrawSurface` | Reads `+0x46d` chroma index; calls `BlitDispatch`     |

Blit-kernel dispatch tables (data, indexed by
`(src_format << 3) | dst_format`):

| Address     | Name                  | Selected when                                                  |
|------------:|-----------------------|----------------------------------------------------------------|
| `0x004b08c8` | `BlitTable_Opaque`   | no chroma, no mask                                             |
| `0x004b09c8` | `BlitTable_DestKey`  | destination chroma override (`consumer+0x14 != 0xFFFFFFFF`)    |
| `0x004b0ac8` | `BlitTable_ChromaKey`| source chroma only (`consumer+0x18 != 0xFF`, no mask)          |
| `0x004b0bc8` | `BlitTable_Masked`   | mask plane only (`consumer+0x20 != NULL`, no chroma)           |
| `0x004b0cc8` | `BlitTable_KeyAndMask`| chroma + mask                                                  |

MSVC adjustor thunks (multi-inheritance plumbing, not engine-meaningful):
`Sub04Dtor`, `Sub04Release`, `Sub1cDtor`, `Sub1cRelease`,
`Sub20ChainOp`, `Sub24Dtor`, `Sub24Release`, `Sub24ChainOp`,
`Sub28Dtor`, `Sub28Release`.

Plate comments capturing the chunk parser, both RLE decoders, the
binding sequence, and the observer callbacks are written inline into
Ghidra (`set_plate_comment`), so the format is documented at each
function as well.
