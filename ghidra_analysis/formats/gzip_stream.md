# `CDSGZipStream` -- on-disk "GZIP" container

**TL;DR.** Despite the class name, `CDSGZipStream` does **not** produce RFC
1952 *gzip* streams. It is a small **random-access seekable container**
whose payload is split into fixed 32 KB blocks, each independently
compressed with **stock zlib 1.1.3** using the **zlib (RFC 1950)
wrapper** -- `windowBits = 15`, `memLevel = 8`, `strategy =
Z_DEFAULT_STRATEGY`, `level = 9 (best)`. The compressed-block table at
the end of the file is itself zlib-compressed with the same parameters.
The outer 4-byte file magic happens to be the ASCII string `"GZIP"`
(`0x50495A47` LE, `1346984519u`) -- that is the entire "GZip" in the
name.

Reverse-engineered live in `bulanci.exe` against the editor C# mirror
(`editor_il_spy/Editor/GZipStream.cs`). All references below are stable
RVAs into the committed `orig/bulanci.exe`.

The companion C# class is `Editor.GZipStream` (Stream subclass) with the
constant `Magic = 1346984519u`. Field-for-field the engine `CDSGZipStream`
matches the C# class -- so the C# source is a true 1:1 reimplementation
and can be used as ground truth for the on-disk format.

### Why the name lies

The file's first four bytes spell `G Z I P` in ASCII. So the *container*
is named "GZIP". But inside each 32 KB chunk, the compressed bytes are
**zlib-wrapped deflate** (two-byte CMF/FLG header + DEFLATE body + 4-byte
Adler-32 trailer, RFC 1950), not the RFC 1952 gzip wrapper (`1F 8B` +
header + DEFLATE body + CRC-32 + ISIZE). Confirmed by reading the
`inflateInit2_` / `deflateInit2_` arguments at the only two call-sites in
the binary (see below).

### On-disk layout

All multi-byte integers are little-endian. `m_lStart` is the absolute
offset where the GZip header begins inside the host stream.

```
+-------------------------+ m_lStart + 0
| uint32  magic = "GZIP"  |    = 0x50495A47 = 1346984519u
+-------------------------+ m_lStart + 4
| int64   index_offset    |    relative to m_lStart; points at the trailer
+-------------------------+ m_lStart + 12 = chunks[0]
| zlib_chunk[0]           |    each chunk holds <= 32768 uncompressed bytes
| zlib_chunk[1]           |    if the compressed size >= 32768, the
| ...                     |    raw uncompressed bytes are stored instead
| zlib_chunk[N-1]         |    (see "store-if-larger" rule below)
+-------------------------+ m_lStart + chunks[N] = m_lStart + index_offset
| int64   total_size      |    decompressed payload length, in bytes
| int32   index_zlib_len  |    compressed size of the chunks[] table
| u8      index_zlib_data[index_zlib_len]
+-------------------------+
                                The decoded index is (N) * int64,
                                listing chunks[1..N] (chunks[0]=12 is
                                implicit). chunks[N] equals
                                index_offset.
```

`m_chunks[]` in both implementations stores **end offsets** for each
32 KB block, so `chunkSize(i) = chunks[i+1] - chunks[i]`. The first
entry is the fixed value `12` (right after the 4+8 header). The list is
written with a final entry equal to `chunks[N-1]` duplicated -- this is
the C# `m_chunks.Add(m_chunks[m_chunks.Count - 1]);` line; the engine
does the same to make `chunks[N] - chunks[N-1] = 0` for the last
sentinel slot.

The capacity hint `m_chunks.Capacity = num2` uses
`num2 = (m_lSize + 32767) / 32768 + 2`, i.e. ceil(total / 32 KB) + 2
(one for the implicit `[0]=12` entry, one for the sentinel duplicate).

### "Store-if-larger" rule

When writing each block, the engine compresses into a temporary buffer
sized at `0x9BAD = 39341` bytes -- this is the same magic
`32768 * 1.2 + 20` as the C# `int destLen = 39341;`. The engine then
checks whether the compressed size is *smaller* than the raw block
size. If it is not (i.e. compression made the block bigger), the raw
32 KB block is written verbatim instead.

Read-side decoding mirrors this: `ReadBufferAt(n)` reads
`chunks[n+1] - chunks[n]` bytes; if the size is **exactly** 32768 the
bytes are taken verbatim as the uncompressed block, otherwise they are
passed to `zlib_inflate`. The boundary is identical on both sides:
*"compressed size < 32768 -> zlib-wrapped; compressed size == 32768 ->
raw"*. The engine additionally rejects sizes outside `(0, 32768]` as a
`BadImageFormatException` equivalent.

### zlib statically linked: version 1.1.3

The binary contains the canonical zlib 1.1.3 fingerprints:

| RVA          | bytes                                             | meaning                                      |
|--------------|---------------------------------------------------|----------------------------------------------|
| `0x00487594` | `"1.1.3\0"`                                       | `ZLIB_VERSION` literal                       |
| `0x0049e06f` | `"?need dictionary\0"` (leading `?` is `z_errmsg[0]`'s `"?"` "stream end" placeholder per zlib 1.1.x) | `z_errmsg[1]`                                |
| `0x0049e080` | `"incorrect data check\0"`                        | inflate Z_DATA_ERROR variant                 |
| `0x0049e098` | `"incorrect header check\0"`                      | inflate header reject                        |
| `0x0049e0b0` | `"invalid window size\0"`                         | `inflate.c`                                  |
| `0x0049e0c4` | `"unknown compression method\0"`                  | `inflate.c`                                  |
| `0x0049e1a4` | `"invalid bit length repeat\0"`                   | `inftrees.c`                                 |
| `0x0049e1e4` | `"invalid stored block lengths\0"`                | `infblock.c`                                 |
| `0x0049e204` | `"invalid block type\0"`                          | `infblock.c`                                 |
| `0x0049ecc0` | `"invalid literal/length code\0"`                 | `infcodes.c`                                 |
| `0x0049ecdc` | `"invalid distance code\0"`                       | `infcodes.c`                                 |

The error-message set is exactly the zlib 1.1.x palette; the version
string `"1.1.3"` is xref'd as the version argument by both class-level
helpers (see below). The stream-size constant passed to `inflateInit_`
and `deflateInit_` is `0x38 = 56`, which is `sizeof(z_stream)` in zlib
1.1.x on 32-bit x86. zlib 1.2.x grew the struct, so a 56-byte stream
size additionally pins this to the 1.1.x series.

### Engine entry points

| RVA          | name                                  | notes                                       |
|--------------|---------------------------------------|---------------------------------------------|
| `0x00434e30` | `CDSGZipStream::Decompress` (static)  | mirror of C# `Decompress`; uses `Z_FINISH`  |
| `0x00434ee0` | `CDSGZipStream::Compress`   (static)  | mirror of C# `Compress`;   uses `Z_FINISH`  |
| `0x00435050` | `CDSGZipStream::ReadBufferAt`         | calls `Decompress` for sub-32 KB chunks      |
| `0x00435140` | `CDSGZipStream::WriteBuffer`          | calls `Compress`; applies "store-if-larger" |
| `0x004354a0` | `CDSGZipStream::CloseInt` writer-half | second `Compress` xref -- index encoder      |
| `0x004356e0` | `CDSGZipStream::Open` reader-half     | second `Decompress` xref -- index decoder    |

Both static helpers compile to a textbook `Z_FINISH` single-shot:

```c
// CDSGZipStream::Decompress @ 0x00434e30  (decompiled)
ret = inflateInit_(&strm, "1.1.3", 0x38);
if (ret == 0) {
    ret = inflate(&strm, /*Z_FINISH*/ 4);
    if (ret == 1 /*Z_STREAM_END*/) {
        *destLen = strm.total_out;
        ret = inflateEnd(&strm);
    } else {
        inflateEnd(&strm);
        if (ret == 0) ret = -5 /*Z_BUF_ERROR*/;
    }
}
return ret;

// CDSGZipStream::Compress   @ 0x00434ee0  (decompiled)
ret = deflateInit_(&strm, /*level*/9, "1.1.3", 0x38);
if (ret == 0) {
    ret = deflate(&strm, /*Z_FINISH*/ 4);
    if (ret == 1 /*Z_STREAM_END*/) {
        *destLen = strm.total_out;
        ret = deflateEnd(&strm);
    } else {
        deflateEnd(&strm);
        if (ret == 0) ret = -5 /*Z_BUF_ERROR*/;
    }
}
return ret;
```

These two functions are byte-for-byte equivalent to `Compress` and
`Decompress` in `editor_il_spy/Editor/GZipStream.cs` (after lowering
the C# `ZStream` wrapper to its zlib equivalents).

### zlib wrapper, not raw deflate, not gzip

The "init1" helpers are stock zlib trampolines that fix `windowBits`:

| RVA          | name                  | what it actually calls                                                                  |
|--------------|-----------------------|------------------------------------------------------------------------------------------|
| `0x0046ef60` | `inflateInit_`        | `inflateInit2_(strm, 15, version, 0x38)`                                                 |
| `0x004704c0` | `deflateInit_`        | `deflateInit2_(strm, 9, /*method*/8, /*windowBits*/15, /*memLevel*/8, /*strategy*/0, ...)`|

The reference `deflateInit2_` body at `0x004702c0` validates these
arguments by exactly the zlib 1.1.x rules:

* `param_3 == 8` (method must be `Z_DEFLATED`)
* `param_4 - 8U < 8` (after `windowBits = abs(windowBits)`, the only
  legal range is `[8, 15]`)
* `param_5 - 1U < 9` (memLevel in `[1, 9]`)
* `param_6 < 3` (strategy in `[0, 2]`)
* `param_8 == 0x38` (stream size must match `sizeof(z_stream)`)

`windowBits = +15` is the **zlib RFC 1950 wrapper**. To get a raw
deflate stream zlib expects a *negative* `windowBits` (`-15`); to get a
gzip RFC 1952 stream zlib 1.1.x cannot produce one at all (that
codepath only landed in 1.2.x via `windowBits + 16`). So `CDSGZipStream`
emits zlib-wrapped DEFLATE, period.

### Engine class layout (read out from `Open` / `ReadBufferAt` / `WriteBuffer` / `CloseInt` / the inherited-Read thunk at `0x00435220`)

```
CDSGZipStream         (instance size: at least 0x48 bytes -- ends with the i64 at +0x40)
+0x00   <CDSChained-style base, 0x10 bytes>
        +0x00 vftable_a        ; primary vtable
        +0x04 vftable_b        ; secondary vtable slot (same address as vftable_a)
        +0x08 <chain/refcount>
        +0x0c <error context>  ; "this+0xc" is passed as the second arg to the
                               ;  FUN_004302e0 throw helper at every bounds-check
+0x10   m_bWriting             ; uint, value 1 (read) or 2 (write)
+0x14   <padding>
+0x18   m_lStart               ; int64, captured from host stream's tell at Open
+0x20   m_stream               ; CDSStream * (host stream, retained via vtbl[2] AddRef-style)
+0x24   m_pData                ; CDSGZipStreamData *
+0x28   m_buffer               ; byte * to 0x8040-byte decompress buffer (0x8000 + 0x40 slack)
+0x2c   m_compress_scratch     ; byte * to 0xa00c-byte compress output buffer (>= deflateBound(0x8000))
+0x30   m_nBufLoaded           ; int32, -1 == none, else index of cached block
+0x34   m_nBufPos              ; uint32, position within the currently-cached block (replaces C# `num2`)
+0x38   m_nBufSize             ; int32, valid bytes in m_buffer (after Decompress or fresh write)
+0x40   m_lBlockStart          ; int64, == (m_lPos / 0x8000) * 0x8000 -- block-aligned absolute pos

CDSGZipStreamData     (instance size: 0x20 bytes; allocated via FUN_00447c42(0x20))
+0x00   vftable_a              ; primary vtable
+0x04   vftable_b              ; secondary vtable slot (same address as vftable_a)
+0x08   refcount               ; int32, set to 1 at construction
+0x0c   <reserved>
+0x10   m_lSize                ; int64, total decompressed payload length
+0x18   m_nChunkCount          ; int32, len(m_chunks)
+0x1c   m_chunks               ; int64 *, end-offset table (chunks[0]=12, chunks[N] sentinel)
```

Both vftable slots at `+0x00` and `+0x04` are written with the *same*
address at construction (see `CDSGZipStream::Open @ 0x004356e0`,
`*puVar3 = CDSGZipStreamData::vftable; puVar3[1] = CDSGZipStreamData::vftable;`).
This is the engine's house pattern for any `CDSChained`-derived class;
the second slot is not a separate multiple-inheritance vtable.

The "virtual" cursor `m_lPos` from the C# side is not stored as a single
field in the engine; it is decomposed into `(m_lBlockStart, m_nBufPos)`
so the per-byte `Read` inner loop can advance with a single 32-bit
increment instead of a 64-bit modulo.

Buffer sizes match the engine constants:

| RVA in `AllocateBuffers @ 0x00435670` | size  | purpose                                                          |
|---------------------------------------|-------|------------------------------------------------------------------|
| `0x8040 = 32832`                      | bytes | `m_buffer` (32 KB block + 64-byte slack for the verbatim case)   |
| `0xa00c = 40972`                      | bytes | compress scratch (`deflateBound(32768)` rounded up, exceeds C#'s 39341) |

The C# `int destLen = 39341;` is a tight upper bound on
`deflateBound(32768)` for level=9, windowBits=15, memLevel=8; the C++
side simply rounds up to `0xa00c`. Both buffers are sufficient.

### Layout cross-checks

* `CDSGZipStream::Open @ 0x004356e0`
  * reads magic into a stack slot, compares to `0x50495a47`, throws
    `FUN_004302e0(0, this+0xc, 0)` on mismatch
  * reads `index_offset` (i64), seeks `host_stream` to `m_lStart + index_offset`
  * reads `m_lSize` (i64) into `m_pData+0x10`
  * computes `m_nChunkCount = ceil(m_lSize / 0x8000) + 2` via `__alldiv`
    (`m_pData+0x18`)
  * reads the compressed index length (i32), allocates a buffer
    (`FUN_0042f6f0`), reads the compressed bytes, feeds them through
    `CDSGZipStream::Decompress` (`FUN_00434e30`)
  * loops `i = 1..N` storing `int64` end-offsets in `m_chunks`
  * appends the sentinel duplicate: copies `chunks[N-1]` into `chunks[N]`
  * primes the reader by calling
    `CDSGZipStream::ReadBufferAt(this, 0, 0)`
* `CDSGZipStream::CloseInt @ 0x004354a0` (writer half)
  * flushes the last partial block via
    `CDSGZipStream::WriteBuffer(this)`
  * seeks `host_stream` to `m_lStart + 4` and writes the i64 index
    offset (the final value of `chunks[N-1]`)
  * seeks to `m_lStart + index_offset` and writes `m_lSize` (i64)
  * compresses `m_chunks[]` via `CDSGZipStream::Compress`
    (`FUN_00434ee0`) into a heap buffer sized
    `0x14 + chunks_byte_count + chunks_byte_count / 8 + ...`
  * writes the compressed index length (i32) and the compressed bytes
* `CDSGZipStream::ReadBufferAt @ 0x00435050`
  * `__alldiv(pos, 0x8000)` -> chunk index, sets
    `m_lBlockStart = block_idx * 0x8000`
  * `m_nBufPos = pos - m_lBlockStart`
  * if cached chunk is not the requested one, computes its compressed
    size as `chunks[i+1] - chunks[i]`, seeks the host stream, reads it
    in, and either copies raw (size == 0x8000) or feeds it through
    `CDSGZipStream::Decompress`
* `CDSGZipStream::WriteBuffer @ 0x00435140`
  * seeks the host stream to `m_lStart + chunks[N-1]`
  * compresses `m_buffer[0..m_nBufSize]` via
    `CDSGZipStream::Compress` -> compressed size in `m_nBufSize`
  * writes either the compressed bytes (compressed size < raw size) or
    the raw bytes (otherwise) -- matches the C# rule exactly

### Symbols set in Ghidra

The following functions and data items have been renamed in the live
Ghidra database (`bulanci.exe`) to make future cross-referencing
self-documenting:

| RVA          | label                                  |
|--------------|----------------------------------------|
| `0x00434e30` | `CDSGZipStream__Decompress_static`     |
| `0x00434ee0` | `CDSGZipStream__Compress_static`       |
| `0x00435050` | `CDSGZipStream__ReadBufferAt`          |
| `0x00435140` | `CDSGZipStream__WriteBuffer`           |
| `0x00435670` | `CDSGZipStream__AllocateBuffers`       |
| `0x004354a0` | `CDSGZipStream__CloseInt`              |
| `0x004356e0` | `CDSGZipStream__Open`                  |
| `0x0046ef60` | `zlib__inflateInit_`                   |
| `0x0046ee60` | `zlib__inflateInit2_`                  |
| `0x0046ef80` | `zlib__inflate`                        |
| `0x0046ee10` | `zlib__inflateEnd`                     |
| `0x004704c0` | `zlib__deflateInit_`                   |
| `0x004702c0` | `zlib__deflateInit2_`                  |
| `0x0046f3e0` | `zlib__deflate`                        |
| `0x0046f660` | `zlib__deflateEnd`                     |
| `0x00487594` | `g_szZlibVersion_1_1_3` (the `"1.1.3"` literal) |

The remaining `CDSGZipStream` public methods (`Read`, `Write`, `Seek`,
`Flush`, `SetLength`, `Close`, two ctors, dtor, plus property
getters / setters) are still under `FUN_xxxx` names; they share the
same field layout above and can be renamed in a follow-up pass without
changing any of the conclusions in this file.

### Re-implementing the format

A faithful reproducer needs:

1. zlib 1.1.3 (or any version that accepts being called with
   `version[0] == '1'`; modern zlib refuses 1.x version strings only on
   `inflateInit_` if you literally pass `"1.x.y"`, but every 1.2.x build
   still accepts it because the major matches);
2. `deflateInit2(level=9, method=Z_DEFLATED, windowBits=15, memLevel=8,
   strategy=Z_DEFAULT_STRATEGY)` per block, then `deflate(Z_FINISH)`,
   then `deflateEnd`;
3. The "compressed >= raw -> store raw" fallback, with a single 32 KB
   scratch input and a `0x9BAD`-byte output buffer;
4. Magic `0x50495A47`, index trailer layout above, `chunks[0] = 12`,
   sentinel duplicate.

Byte-for-byte parity with the engine is achievable: zlib's deflate is
deterministic for a given (level, windowBits, memLevel, strategy)
across versions 1.1.x and 1.2.x for the same input window, modulo a few
known bit-exact changes in 1.2.x optimisations. Round-tripping the same
blocks through zlib 1.1.3 will reproduce the original file exactly.

### Match status

The 16 identified zlib helpers were renamed in Ghidra under the
`zlib::` namespace and migrated to a new `zlib` unit (own
`src/bulanci/zlib.cpp` / `include/bulanci/zlib.h` / `build.ninja` rule
+ `config/bulanci/units_listing.csv` row).  After the rename
`scripts/generate_sources.py:_sanitize_symbol_name()` and Ghidra's
`ExportDelinker.java:sanitizeSymbolNames()` both strip trailing
underscores, so COFF symbols on each side pair under the stripped
form (`zlib::inflateInit`, `zlib::deflateInit2`, ...).

The engine-level `CDSGZipStream::Decompress` and `::Compress` stay in
`_Globals.cpp` (still under `_Globals::FUN_00434e30` /
`_Globals::FUN_00434ee0`) and call into `zlib::*` via the new header.

Run `python scripts/internal/zlib_status.py` to refresh.  A
`build/orig/bulanci/zlib.obj` baseline is required for the right
column; regenerate it via `python scripts/export_ghidra_objs.py` (close
the Ghidra GUI first) after any rename / mapping change.

| RVA          | C source-level name              | COFF symbol                       | Status                                       |
|--------------|----------------------------------|-----------------------------------|----------------------------------------------|
| `0x0046ef60` | `inflateInit_`                   | `zlib::inflateInit`               | **EXACT** (26 / 26 bytes) -- real zlib body  |
| `0x004704c0` | `deflateInit_`                   | `zlib::deflateInit`               | **EXACT** (37 / 37 bytes) -- real zlib body  |
| `0x00471260` | `zcalloc`                        | `zlib::zcalloc`                   | **EXACT** (19 / 19 bytes) -- real zlib body  |
| `0x004733d0` | `inflate_codes_free`             | `zlib::inflate_codes_free`        | **EXACT** (22 / 22 bytes) -- real zlib body  |
| `0x00434e30` | `CDSGZipStream::Decompress`      | `_Globals::FUN_00434e30`          | length match (170 / 170); 33 bytes differ    |
| `0x00434ee0` | `CDSGZipStream::Compress`        | `_Globals::FUN_00434ee0`          | length match (172 / 172); 33 bytes differ    |
| `0x0046ee10` | `inflateEnd`                     | `zlib::inflateEnd`                | opacity-barrier stub (real body needed)      |
| `0x0046ee60` | `inflateInit2_`                  | `zlib::inflateInit2`              | opacity-barrier stub                         |
| `0x0046ef80` | `inflate`                        | `zlib::inflate`                   | opacity-barrier stub                         |
| `0x0046f3e0` | `deflate`                        | `zlib::deflate`                   | opacity-barrier stub                         |
| `0x0046f660` | `deflateEnd`                     | `zlib::deflateEnd`                | opacity-barrier stub                         |
| `0x004702c0` | `deflateInit2_`                  | `zlib::deflateInit2`              | opacity-barrier stub                         |
| `0x0046edc0` | `inflateReset`                   | `zlib::inflateReset`              | `STUB_BODY()` (3 bytes)                      |
| `0x004704f0` | `inflate_blocks_reset`           | `zlib::inflate_blocks_reset`      | `STUB_BODY()` (3 bytes)                      |
| `0x00470570` | `inflate_blocks_new`             | `zlib::inflate_blocks_new`        | `STUB_BODY()` (3 bytes)                      |
| `0x00470620` | `inflate_blocks`                 | `zlib::inflate_blocks`            | `STUB_BODY()` (3 bytes)                      |
| `0x004710e0` | `inflate_blocks_free`            | `zlib::inflate_blocks_free`       | `STUB_BODY()` (3 bytes)                      |
| `0x00471290` | `init_block` (trees.c)           | `zlib::init_block`                | `STUB_BODY()` (5 bytes, __fastcall)          |

#### Zlib helper inventory (0x46d000-0x474000)

The address range `0x46d000-0x474000` contains ~50 statically-linked zlib
1.1.3 helpers.  These are the ones identified so far via Ghidra call-graph
+ string xrefs + body shape; the rest still need identification.

| RVA          | Likely zlib name             | Confidence  | How identified                              |
|--------------|------------------------------|-------------|---------------------------------------------|
| `0x0046edc0` | `inflateReset`               | high        | caller of `inflate_blocks_reset`            |
| `0x0046ee10` | `inflateEnd`                 | confirmed   | callee of `Decompress` cleanup              |
| `0x0046ee60` | `inflateInit2_`              | confirmed   | called by `inflateInit_` wrapper            |
| `0x0046ef60` | `inflateInit_`               | confirmed   | DEF_WBITS=15 wrapper                        |
| `0x0046ef80` | `inflate`                    | confirmed   | xrefs zlib state-machine error strings      |
| `0x0046f3e0` | `deflate`                    | confirmed   | called by `Compress` engine                 |
| `0x0046f660` | `deflateEnd`                 | confirmed   | callee of `Compress` cleanup                |
| `0x004702c0` | `deflateInit2_`              | confirmed   | called by `deflateInit_` wrapper            |
| `0x004704c0` | `deflateInit_`               | confirmed   | level-9 hardcoded wrapper                   |
| `0x004704f0` | `inflate_blocks_reset`       | high        | called from `inflate` BLOCKS state          |
| `0x00470570` | `inflate_blocks_new`         | high        | called from `inflateInit2_`                 |
| `0x00470620` | `inflate_blocks`             | high        | 2683-byte state machine + zlib err strings  |
| `0x004710e0` | `inflate_blocks_free`        | high        | called from `inflateEnd`                    |
| `0x00471260` | `zcalloc`                    | confirmed   | matched: `calloc(items, size)` byte-exact   |
| `0x00471290` | `init_block` (trees.c)       | high        | 286/30/19 loop counts = L/D/BL_CODES        |
| `0x004733d0` | `inflate_codes_free`         | confirmed   | matched: `ZFREE(z, c)` byte-exact           |

Remaining helpers (`huft_build`, `inflate_codes` state machine,
`inflate_fast`, `inflate_trees_*`, `_tr_init`, `_tr_flush_block`,
`compress_block`, `send_tree`, `build_tree`, `deflate_fast`,
`deflate_slow`, `lm_init`, `longest_match`, `adler32`, `crc32`, `zcfree`,
`zmemcpy/set/cmp`, `zError`) still need identification.  See
`scripts/internal/zlib_status.py` and `zlib_dump_pair.py` for byte-diff
tooling.

#### Why the engine helpers don't byte-match

Same length, same instructions, same operand layout -- only register
allocation differs.  Original uses `EDI` for the `destLen` (`uint*`)
pointer; MSVC 8 on our source picks `EBX` instead.  Both are
callee-saved and encode the same number of bytes for every `[reg]`
deref the function performs, so the choice is arbitrary from the
ABI's point of view -- it's a deterministic but undocumented preference
inside the VS2005 register allocator.  The pop/push pairs and a couple
of jump displacements pick up the resulting offset shifts.  Once the
stubbed zlib bodies are replaced with the real 1.1.3 source (no
inline-asm clobber barriers), the same callsites should pick `EDI` and
the diff collapses to 0.

#### Stub opacity barriers (read me before touching the stubs)

The six zlib-core stubs in `_Globals.cpp` are written as plain
matched-style bodies (no `STUB_BODY()`) with four opacity tricks
stacked on top of each other so the *matched callers* compile to the
exact byte sequence the original emits:

1. `__declspec(noinline)` -- keep the `CALL` instruction at the
   callsite (without it /O2 inlines the trivial body and the
   call/cleanup pair disappears).
2. `static volatile uint __r = 0; return __r + ...;` -- block return-
   value constant propagation across the call.  A pure `return 0;` lets
   /O2 prove `err == 0` after the call and fold the
   `if (err == 0)` / `if (err == 1)` branches that the original keeps
   live.
3. `*(volatile uint*)param_1` -- defeat escape analysis.  Stubs are
   visible in the same TU, so /O2 can see they never read the
   `z_streamp` argument; without this hint the optimiser eliminates the
   `strm.zalloc / .zfree / .opaque` stores in the caller, shrinking
   `Decompress` from 170 to 104 bytes.
4. `__asm { mov ecx, ecx ; mov edx, edx }` -- defeat interprocedural
   register-usage analysis.  With the body visible, /O2 sees the stub
   leaves `ECX` and `EDX` alone and lets the caller keep `err` in `ECX`
   across the call.  Inline asm forces MSVC to model both caller-saved
   registers as clobbered, so the caller spills `err` into the
   callee-saved `ESI` -- matching the original codegen against the
   real zlib symbol.

These barriers cost a few bytes inside each stub, so the stubs
themselves will never be size-matched on their own (`base = 17 bytes`
vs the real zlib bodies of 73-932 bytes).  That is fine: the stubs are
infrastructure for matching their callers, and disappear when the real
zlib 1.1.3 source is dropped in.  The recipe for that next step is in
the "Re-implementing the format" list above; the zlib 1.1.3 sources
are available from <http://www.zlib.net/zlib-1.1.3.tar.gz> (also
mirrored on GitHub's `madler/zlib` archive branch).

Anyone replacing a stub with a real zlib body should drop all four
opacity tricks at the same time: they exist solely to compensate for
the trivial bodies and would corrupt real zlib code if left in place.

