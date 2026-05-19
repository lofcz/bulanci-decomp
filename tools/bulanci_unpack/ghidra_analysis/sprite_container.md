# BitmapSprite (ClassID 52) container analysis

The "BitmapSprite" name is an unpacker convention — the editor source has
no entry for ClassID 52 (`Editor.ResourceItems.BitmapFormat` only knows 21,
22, 28), so the format is purely game-side.

The header layout is now fully recovered and double-confirmed (Ghidra
disassembly of the factory + statistical clustering across all 130
master-pack BitmapSprite resources). What remains TODO is the per-frame
RLE/LC payload after offset 0x2c, which is consumed by `CBulPicture` at
runtime, not by `CDSBitmap` itself.

## Native registration  (Ghidra-verified)

`bulanci.exe` registers ClassID 52 in its global resource factory table
at module-init time. The relevant stub lives at `0x0047cdd0`; disassembly
is:

```asm
0047cdd0  PUSH  0x00432a50           ; factory function for ClassID 52
0047cdd5  PUSH  0x004b7bfc           ; class-name slot (BSS; filled at runtime)
0047cdda  PUSH  0x34                 ; ClassID = 52
0047cddc  MOV   ECX, 0x004b7e08      ; class-registry context
0047cde1  CALL  0x0042e910           ; CDSResInfo::Register
0047cde6  PUSH  0x0047e8b0           ; atexit / static-dtor registration
0047cdeb  CALL  0x00447e72
```

The factory at `0x00432a50` (verified by direct disassembly) allocates
**0x50 (80) bytes** with `FUN_00447c42`, then fills:

| offset | value                | role                                              |
|-------:|----------------------|---------------------------------------------------|
| +0x00  | `0x004872d0`         | primary vtable (`CDSObject`-shape)                |
| +0x04  | `0x004872a4`         | secondary vtable (interface subobject)            |
| +0x08  | 0                    | source stream pointer (filled by Read)            |
| +0x0c  | 0                    | parent CBulanci pointer (filled by Attach)        |
| +0x1c  | `0x00487290`         | tertiary vtable                                   |
| +0x20  | 1                    | refcount                                          |
| +0x24  | `0x00487274`         | per-instance ID vtable                            |
| +0x28  | `0x0048725c`         | type-tag vtable                                   |
| +0x2c  | 0                    | (zeroed; first dword of cached payload header)    |
| +0x38, +0x3c | 0/0            | cached pixel pointers (filled lazily)             |

So this is a thin "lazy" wrapper — it doesn't itself hold pixel data.
The pixels live in a separately-allocated `CBulPicture` (1136 bytes,
factory `CBulPicture::FUN_0040eb30`, allocates `0x470` bytes) that is
bound to the ClassID-52 handle the first time the sprite is drawn. The
CBulPicture layout is also Ghidra-verified — the factory writes:

```
+0x00 / +0x04 / +0x10 / +0x18       four vtable pointers
+0x68                               uchar* packed pixel data (init 0)
+0x6c..+0x46b                       256-entry RGBA palette
                                    (1024 bytes, init = 0xFFFFFFFF)
+0x46c..+0x46f                      tail; +0x46d holds the transparent
                                    palette index (0xFF = none)
```

(See the `for iVar2 = 0x100` loop in `FUN_0040eb30` that fills 256 dwords
with `0xFFFFFFFF` starting at `puVar1[0x1b]` == `+0x6c`.)

`CBulPicture` registers itself separately as type ID `0x7e8` (2024) in
its own factory context (`0x004b358c`) via the registration stub at
`0x0047b6e0`. It is therefore **not** a `.eap` ClassID — it is reachable
only via the runtime binding path below.

The lazy-load chain (taken from `CDSMpx::FUN_00446b90`) is:

```
container.Read(...)            ;  reads the bytes below into a member
   -> FUN_00430e70(stream)     ;  wraps payload in a CDSEasyMemStream
       -> FUN_00430a70         ;  asserts the stream is non-empty
```

The actual RLE/delta decoder consumes the `CDSEasyMemStream` from inside
`CBulPicture`'s constructor (still to be identified — likely a member of
the `CBulPicture` vtable). The 80-byte handle itself never touches a
pixel buffer.

## Payload header — fully recovered

Every ClassID-52 entry in the master pack starts with the fixed
0x2c-byte (11-dword) descriptor below. Every field has been checked
against all 130 master-pack samples; values in the rightmost column
are the most common observation and the typical observed range.

```
+0x00 u32 totalSize       ; len(raw); equals .eap record size on every sample
+0x04 u32 encodedSize     ; bytes of "primary" encoded buffer
+0x08 u32 encodedSize2    ; usually = encodedSize; rare divergence indicates
                          ; a multi-stream encoded sprite (one sample, id 65839)
+0x0c u32 width           ; common 55 / 104 / 122 / 240; observed 0..276
+0x10 u32 height          ; common 43; observed 11..276
+0x14 u32 frameCount      ; common 43; observed 5..120
+0x18 u32 channels        ; **always 3** (BGR); 130/130 samples
+0x1c u32 inMemSize       ; runtime allocation hint; commonly 0x470 == sizeof
                          ; CBulPicture but ranges 264..4473
+0x20 u32 flags           ; small int codec selector (3, 5..20, 31..62);
                          ; bpp / compression mode — semantic TBD
+0x24 u32 encodedSize3    ; third copy of encodedSize; always equals
                          ; encodedSize2 (130/130 samples)
+0x28 u32 packed          ; 24-bit packed field; low byte = tag (0x01/0x02/0x03);
                          ; observed e.g. 0xDF03, 0x5A03, 0x5BF01 — palette
                          ; or sentinel descriptor; semantics TBD
+0x2c ...                 ; RLE/LC stream (decode TBD)
```

### Cross-sample confirmation

A survey of the 130 master-pack BitmapSprite resources
(`tools/bulanci_unpack/ghidra_analysis/inspect_sprite_headers.py`)
produced:

- `field[6]` (channels) takes **exactly one value** (`3`) across 130 of 130
  samples. This is the strongest "marker" signal in the header and the
  parser warns when violated.
- `field[0]` (totalSize) equals the on-disk record length on every
  sample. The parser warns when it doesn't.
- `field[9]` (encodedSize3) equals `field[2]` (encodedSize2) on every
  sample. The parser warns when it doesn't.
- `field[1]` vs `field[2]` (encodedSize vs encodedSize2) almost always
  match, but **diverge** on a handful of samples (most notably id 65839,
  the 91 KB compound atlas with `width == 0`). This is intentional and
  not flagged.

Empirical clustering: files `res_*65716..65720` share an identical
36-byte header but differ in `totalSize` (1612..2614). They are five
recolours of the same 43-frame 104x55 sprite, all starting with the
identical 32-byte byte sequence `00 00 15 68 0a 1d 23 68 07 1d 0a 82 ...`
at offset 0x2c. This confirms the post-header bytes carry the actual
animation payload (and that the encoder produces a stable prefix when
the base animation is unchanged).

## Decoder TODOs

To complete the decoder we still need to:

1. Identify the `CBulPicture` Read method that consumes the
   `CDSEasyMemStream`. It must live somewhere on the CBulPicture vtable
   (most likely the slot used by `FUN_0040b050`'s caller in
   `CBulanci::FUN_0042cbf0`).
2. Disassemble that method to recover the per-frame opcode set. Best
   guess (based on byte distribution) is an Autodesk Animator FLI-style
   stream: tag bytes 0x68 = "row repeat", 0x1d = "literal run", etc.
3. Reconstruct the palette source. The CBulPicture allocates a 256-entry
   RGBA palette and fills it with `0xFFFFFFFF`, but the master-pack
   header doesn't visibly contain 1024 bytes of palette data anywhere
   adjacent to the RLE bytes. Either the palette is inferred from the
   pixel stream itself, or the engine uses a shared system palette.
4. Pin down the semantics of `flags` (offset +0x20) and `packed`
   (offset +0x28). `flags` is a small enum (~10 distinct values) so it
   probably names the codec variant; `packed` looks like a packed
   palette/sentinel descriptor (low byte is a 1..3 tag).
5. Combine the decoded frames into a PNG or animated PNG/GIF for the
   manifest.

## Reproducing the empirical survey

Three small helper scripts live in this folder. They have no dependencies
beyond the Python standard library and they're cheap to run, so feel
free to use them as throwaway sanity tools whenever a new sample shows
up or the format is suspected to have shifted.

```bash
# 1. Dump the first 11 u32s of every BitmapSprite + per-field histograms.
python tools/bulanci_unpack/ghidra_analysis/inspect_sprite_headers.py \
       unpacked/overlay

# 2. Print one specific sprite's 44-byte header in both hex and decoded form.
python tools/bulanci_unpack/ghidra_analysis/dump_one_header.py \
       unpacked/overlay/res_0000065716_52_BitmapSprite.bin

# 3. After re-running the unpacker, verify the new sanity checks across
#    every BitmapSprite in the manifest (channels==3, totalSize==size,
#    encodedSize3==encodedSize2) and list any multi-stream variants.
python tools/bulanci_unpack/ghidra_analysis/check_sprite_manifest.py \
       unpacked/overlay
```

As of this writing the master pack produces:

* 130/130 samples with `channels == 3`
* 130/130 samples with `totalSize == .eap-record size`
* 130/130 samples with `encodedSize3 == encodedSize2`
* 0 sanity warnings emitted by the parser
* 44 multi-stream variants (`encodedSize != encodedSize2`)
* 8 compound-atlas sprites (`width == 0`)
* 2 fit-to-source sprites (`width == 0xFFFFFFFF`)
