# bulanci_unpack

Phase 1 of the assets-first remaster: decodes the custom container used by
`bulanci.exe` (PE overlay), `.eap` (compiled level bundle) and `.eapres`
(editor-level XML), and writes every embedded resource to disk.

The format was reverse-engineered from the decompiled `Editor.exe`:

- Container: [`editor_il_spy/Editor/GZipStream.cs`](../../editor_il_spy/Editor/GZipStream.cs)
- `.eap` resource layout: [`editor_il_spy/Editor/Level.cs`](../../editor_il_spy/Editor/Level.cs) (`Compile()`) and [`editor_il_spy/Editor/ResourceItem.cs`](../../editor_il_spy/Editor/ResourceItem.cs) (`WriteHeader()`)
- Resource-specific encoders: files under [`editor_il_spy/Editor.ResourceItems/`](../../editor_il_spy/Editor.ResourceItems)
- Script bytecode header: [`editor_il_spy/Editor.Scripts/Script.cs`](../../editor_il_spy/Editor.Scripts/Script.cs), opcodes in [`editor_il_spy/Editor.Scripts/Opcode.cs`](../../editor_il_spy/Editor.Scripts/Opcode.cs)

## Requirements

Python 3.10+ (uses only `zlib`, `struct`, `argparse`, `json`, `pathlib`,
`dataclasses`, `xml.etree` — all standard library).

## Usage

```text
python tools/bulanci_unpack/bulanci_unpack.py overlay orig/bulanci.exe   -o unpacked/overlay
python tools/bulanci_unpack/bulanci_unpack.py eap     tutorial.eap       -o unpacked/tutorial_eap
python tools/bulanci_unpack/bulanci_unpack.py eapres  tutorial.eapres    -o unpacked/tutorial_eapres
python tools/bulanci_unpack/bulanci_unpack.py auto    <file>             -o unpacked/<name>
```

The `auto` subcommand routes by extension (`.exe` -> overlay, `.eap` -> eap,
`.eapres` -> eapres).

## Output layout

```
<out-dir>/
  _raw.bin                                                  # full inflated payload (always)
  _manifest.json                                            # structured summary
  _payload.xml                                              # for .eapres
  res_<id>_<classId>_<ClassName>.bin                        # raw bytes of each resource (.eap only)
  res_<id>_<classId>_<ClassName>.jpg|.bmp|.mp3|.sign.json|.script.bin
                                                            # friendly companion for known ClassIDs
```

## Container format

```
+0    uint32  magic = 'GZIP' (0x50495A47)
+4    int64   dirOff  (offset relative to container start)
+12   ...     N back-to-back 32 KiB-uncompressed chunks; each chunk is
              stored as a zlib deflate stream (with 0x78 0xDA header) if it
              compresses smaller than 32 KiB, otherwise raw uncompressed bytes.

@dirOff
      int64   uncompressedSize    (total inflated payload bytes)
      int32   indexLen
      byte[]  zlib-compressed array of (N+1) int64 chunk offsets.
              chunk_offsets[0]   == 12      (= first chunk position)
              chunk_offsets[N]   == dirOff  (= one past the last chunk)
              chunk_size[n]      = chunk_offsets[n+1] - chunk_offsets[n]
```

For the `bulanci.exe` container, the start offset is the end of the last PE
section's raw data (the standard PE overlay convention).

## `.eap` bundle layout

```
int32 count
int32 format_tag                           // 8 for editor-compiled .eap (Level.Compile);
                                           // 32 for the native game's master pack inside
                                           // bulanci.exe's overlay. Treated as opaque.
repeat count times:
    int32 = 90                             // per-record marker (always 90)
    uint32 ID
    uint32 ClassID                         // see table below
    int64  Position                        // offset into the data section
    uint32 Size                            // payload size after the ClassID dword

data section:                              // back-to-back resource entries
    uint32 ClassID                         // duplicates the header ClassID
    byte[Size] payload                     // encoded per ClassID (see Editor.exe)
```

## Known `ClassID`s

The first half of the table is the editor-visible class set (defined in
[`editor_il_spy/Editor.ResourceItems/`](../../editor_il_spy/Editor.ResourceItems));
the second half is recovered from `bulanci.exe`'s master pack (format tag 32)
and was reverse-engineered from the binary plus its decompiled CDS\* classes.

| ClassID | Class             | Encoding                                                                                                   |
|---------|-------------------|------------------------------------------------------------------------------------------------------------|
| 21      | `BitmapJPEG`      | Raw JPEG bytes                                                                                             |
| 22      | `BitmapBMP`       | Raw BMP bytes                                                                                              |
| 28      | `BitmapSpecial`   | u32 w, u32 h, u32 marker, u32 stride, u32 field4, byte padA=0xFF, u32 paletteCount, byte hasUnpacked; then `paletteCount*4` palette entries (BGR + reserved) and `stride*height` packed pixels. When `hasUnpacked==1` an additional `width*height` "unpacked" buffer follows (the runtime cache; we ignore it). `marker` is a bpp tier (0=1bpp, 1=2bpp, 2=4bpp, 3=8bpp, 4=16bpp, 5=24bpp BGR, 6=32bpp); all of 0..5 decode to PNG today. `field4` is a transparent palette index for indexed variants or a transparent RGB sentinel for marker 5. |
| 43      | `AudioBank`       | u32 dataLen, u16 channels, u16 bits, u32 freq, then `dataLen` bytes of little-endian PCM. Companion `.wav` is written. |
| 48      | `Mp3`             | int32 dataLen, uint32 size, u16 channels, u16 bits, u32 freq, MP3 stream                                   |
| 52      | `BitmapSprite`    | Fixed 0x2c-byte header (11×u32: totalSize, encodedSize, encodedSize2, width, height, frameCount, channels=3, inMemSize, codec flags, encodedSize3, 24-bit packed tag) followed by an RLE/LC pixel stream that `CBulPicture` decodes at runtime. Header is fully recovered and sanity-checked; the per-frame RLE decoder is still TODO. See [`ghidra_analysis/sprite_container.md`](ghidra_analysis/sprite_container.md). |
| 54      | `MouseCursor`     | u32 totalSize, u32 frameCount, u32 …; small (8 KB) cursor frames + 24bpp BGR palette                       |
| 58      | `DsmInner`        | u32 nameLen, char[nameLen] filename, u64 FILETIME, u32 fileLen, byte[fileLen] payload. The master pack carries one `BULANCI.TMP` entry — an embedded sub-archive used by `CDSDsmFile`. |
| 67      | `AudioBankIndex`  | u32 bankResourceID (the ClassID-43 partner), u32 reserved, u32 sampleCount, u32[sampleCount] byte lengths. Sums match the partner's `dataLen` exactly. |
| 76      | `BitmapJpegAnim`  | u32 totalSize + 5×u32 descriptor + back-to-back JPEG frames. We auto-extract every embedded JPEG to `*.frameNNN.jpg`. |
| 94      | `Sign`            | uint32 packedDate, int32 len+UTF-16LE content, int32 len+UTF-16LE copyright, byte 0                        |
| 2026    | `Script`          | int32 codeLen, int32 nExports, int32 nVars, byte[codeLen] bytecode, int32[nExports] entries — see disassembler below. |
| 2043    | `TextBlock`       | u32 charCount, char[charCount] UTF-16LE. First codepoint usually `0x0001` (section/line marker). Decoded to `*.txt`. |
| 2050    | `TextTable`       | u32 totalSize, u32 entryCount, u32×2 reserved, then mixed fixed-size records + UTF-16LE strings. Phase-1 extractor dumps a JSON sidecar listing every printable string found. |

## Script bytecode disassembler

`Script` (ClassID 2026) bodies are disassembled to `*.script.asm` per the
opcode table in [`Opcode.cs`](../../editor_il_spy/Editor.Scripts/Opcode.cs).
Each function entry starts with `byte varCount` followed by commands. The
disassembler renders inline nested arguments compactly, e.g.

```
fn export#1 @ 0x00d8:
  ; varCount=0
    @0x00d9  SetGlobalVar(3, IntConst(0))
    @0x00e0  IfEqual(IsNet(), IntConst(0), 251)
    @0x00eb  SetGlobalVar(3, StrmCreateMem(IntConst(4096), IntConst(4096)))
    @0x00f8  SetCommStrm(GetGlobalVar(3))
    @0x00fb  LoadPreface(100001)
    @0x0100  SetMusic(100003, 0)
    @0x0109  SetInsertMode(IntConst(0))
    @0x010f  InsertView(CreateImage(IntConst(0), IntConst(0), 100002))
```

The native game's master-pack scripts use a *superset* of the editor's
opcode enum. All 103 opcodes (45 base `CDSScript` + 58 `CLevelScript`
extension entries) are documented in
[`ghidra_analysis/script_dispatch_table.md`](ghidra_analysis/script_dispatch_table.md);
every handler in `bulanci.exe` has been hand-disassembled to recover
both its argument shape and a human-readable name. Names come from the
editor's `Editor.Scripts.Opcode` enum where possible; for the 36 game-
only opcodes the editor never emits, the unpacker uses a reverse-
engineered name that describes the engine call the handler forwards to
(e.g. `BindToSlot`, `PlayAnim`, `TimerStart`, `CollResize`). The
mapping reads naturally even when handlers compose deeply, e.g.
`PlayAnim(GetSlot(IntConst(6)), IntConst(0))`.

The disassembler also recognises functions with branchy control flow
(`If*`, `Goto`, `Switch`, `Select`) and walks all reachable bytes inside
them. Functions without branches stop at the first `Return`, and any
trailing bytes are flagged as `; (N byte(s) of unreachable tail/inline
helper)` — typically these are subroutines that `Call` opcodes target
but the export table doesn't list as top-level entries. As a result no
`<UNKNOWN op=N>` markers should ever appear in a freshly unpacked
`.script.asm`.

## Name resolution from `.eapres`

`tools/bulanci_unpack/bulanci_unpack.py eap LEVEL.eap -o out/` automatically
pairs `LEVEL.eap` with a sibling `LEVEL.eapres` (the editor's XML side car).
The names typed in the editor flow into each manifest entry's `name` field.
Override the side-car path with `--names-from PATH.eapres`.

## What is still out of scope

- **`BitmapSprite` (ClassID 52) RLE pixels.** The 0x2c-byte header is now
  fully recovered and sanity-checked: totalSize, the three encodedSize
  mirrors, width/height/frameCount, channels (always 3), the
  `CBulPicture`-shaped inMemSize hint, the codec-flags byte, and the
  24-bit packed tag. The per-frame RLE/LC stream that follows is still
  TODO — it's decoded at runtime by `CBulPicture` (factory at
  `0x0040eb30`, palette+pixels layout documented in
  [`ghidra_analysis/sprite_container.md`](ghidra_analysis/sprite_container.md)),
  not by `CDSBitmap` itself, and tracking down the right vtable slot is
  the remaining piece of work. The format is purely native, with no
  editor source to cross-reference.
- **Exact authoritative names** for the 36 game-only opcodes in the
  `CLevelScript` extension table. Argument shapes and runtime
  behaviours are recovered, and each opcode now has a descriptive name,
  but those names reflect the engine call the handler forwards to (e.g.
  `RegisterTimer`, `PlayAnim`, `CollInsert`) rather than the original
  C++ identifier from the lost developer-side source. See the "What's
  still unknown" section of
  [`ghidra_analysis/script_dispatch_table.md`](ghidra_analysis/script_dispatch_table.md)
  for the handful of names that are best-effort guesses.
- A native C++ port of the unpacker. The Phase 1 unpacker is intentionally
  a Python throwaway tool; a parallel MSVC C++ implementation will land
  when we build the proper toolchain.

## Verification samples shipped with this repo

- `tutorial.eap` and `tutorial.eapres` round-trip through `decompress_container`
  with their recorded `uncompressedSize` matching the inflated payload byte for byte.
- The PE overlay of `orig/bulanci.exe` (after offset `0xB6000`) decompresses with
  the same code path.
