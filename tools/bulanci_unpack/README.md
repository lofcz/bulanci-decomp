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

| ClassID | Class                  | Encoding                                                           |
|---------|------------------------|--------------------------------------------------------------------|
| 21      | `BitmapJPEG`           | Raw JPEG bytes                                                     |
| 22      | `BitmapBMP`            | Raw BMP bytes                                                      |
| 28      | `BitmapSpecial`        | width, height, 5, stride, transparentRGB, padA, 0x01000000, padB, BGR24 pixels |
| 48      | `Mp3`                  | int32 dataLen, uint32 size, u16 channels, u16 bits, u32 freq, MP3 stream |
| 94      | `Sign`                 | uint32 packedDate, int32 len+UTF-16LE content, int32 len+UTF-16LE copyright, byte 0 |
| 2026    | `Script`               | int32 codeLen, int32 nExports, int32 nVars, byte[codeLen] bytecode, int32[nExports] entries |

## What is intentionally out of scope (Phase 1)

- Converting `BitmapSpecial` (24bpp BGR with sentinel-color transparency) to PNG.
  We capture the parsed header in `_manifest.json` and the raw pixel bytes in
  `res_*.bin`; the PNG converter is a Phase 2 task.
- Disassembling the `Script` bytecode (opcodes already mapped in
  `Editor.Scripts.Opcode` — see [`Opcode.cs`](../../editor_il_spy/Editor.Scripts/Opcode.cs)).
- Resolving resource names: the `.eap` bundle stores numeric IDs only; the
  human-readable names live in the matching `.eapres` XML.
- A native C++ port of the unpacker. The Phase 1 unpacker is intentionally a
  Python throwaway tool; a parallel MSVC C++ implementation will land when we
  build the proper toolchain.

## Verification samples shipped with this repo

- `tutorial.eap` and `tutorial.eapres` round-trip through `decompress_container`
  with their recorded `uncompressedSize` matching the inflated payload byte for byte.
- The PE overlay of `orig/bulanci.exe` (after offset `0xB6000`) decompresses with
  the same code path.
