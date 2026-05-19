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
| 52      | `BitmapSprite`    | Fixed 0x2c-byte header (11×u32: totalSize, encodedSize, encodedSize2, width, height, frameCount, channels=3, inMemSize, `flags` = frameCount-1, encodedSize3 = frame-0 size, packed `numInner`) followed by a back-to-back **animation-frame stream** (each frame: `u32 frameSize, u8 numInner`, then `numInner` inner chunks each prefixed by `u32 chunkSize, u8 opcode`). The first frame's 5-byte header overlaps file offsets 0x24..0x28. Header, frame stream **and** the per-opcode pixel decoders (RLE 0x00/0x0e, delta 0x04/0x0f, palette 0x09 = FLI/FLC `COLOR_256` with a cursor-based relative-skip, region 0x0a/0x0b/0x0c, transparent-index 0x0d, mask plane 0x0e/0x0f) are fully recovered on 130/130 master-pack samples; the unpacker emits a horizontal atlas sprite-sheet (RGBA PNG, one stripe per frame), an `*.atlas.json` sidecar describing frame size, frame count, ring-frame index, a top-level `timing` block (raw u16 ticks from opcode 0x0C with carry-forward semantics matching the runtime's `frameDelayOverrideMs`, see [`ghidra_analysis/anim_runtime.md`](ghidra_analysis/anim_runtime.md)), and per-frame metadata (engine opcodes applied, NotifyMove origin, `durationTicks` + `effectiveDurationTicks`, parsed `events: [{kind, x, y}]` records from opcode 0x0B = per-frame anchor/attachment points, transparent-index hits), and an `*.atlas.gif` animated preview (per-frame local color tables for palette-cycle sprites, transparent slot marked via GCE, infinite loop via Netscape extension). Sprites with no inline palette (~41 recolour variants) get a grayscale fallback palette. **Transparency follows the engine's actual three-tier model** (see `CDSFlxFile::DecodeFrame @ 0x00432c60`): (1) per-pixel mask plane at `consumer+0x20` populated by opcodes 0x0e/0x0f — used by 39/130 sprites and gated by the masked-blit dispatch table at `DAT_004b0bc8`; (2) explicit color-key index from opcode 0x0d → `CBulPicture+0x46d` for 3/130 sprites; (3) implicit opaque for the remaining ~88 sprites whose engine-level BG is masked by a higher-layer compositor — for those the unpacker tallies the RGB at every bitmap-perimeter pixel across every frame and treats the dominant RGB (when it covers >50% of perimeter pixels) as a synthetic chroma key, exactly matching the engine's chroma-key blit kernels at `DAT_004b0ac8`. See [`ghidra_analysis/sprite_container.md`](ghidra_analysis/sprite_container.md) and [`ghidra_analysis/flx_file_format.md`](ghidra_analysis/flx_file_format.md). |
| 54      | `MouseCursor`     | u32 totalSize, u32 frameCount, u32 …; small (8 KB) cursor frames + 24bpp BGR palette                       |
| 58      | `DsmInner`        | u32 nameLen, char[nameLen] filename, u64 FILETIME, u32 fileLen, byte[fileLen] payload. The master pack carries one `BULANCI.TMP` entry — an MZ-prefixed PE blob unrelated to `CDSDsmFile` (see `ghidra_analysis/dsm_file_format.md`). |
| 67      | `AudioBankIndex`  | u32 bankResourceID (the ClassID-43 partner), u32 reserved, u32 sampleCount, u32[sampleCount] byte lengths. Sums match the partner's `dataLen` exactly. |
| 76      | `BitmapJpegAnim`  | `CDSDsmFile` synchronized MJPEG + 16-bit PCM movie: 36-byte `CDsmHeader` (`dwPayloadEndOffset`, `dwCanvasWidth/Height/PixelFormat`, `dwDurationMs`, `dwFrameCount`, `dwAudioByteCount`, `dwAudioFormatPacked`, `dwAudioSampleRate`) followed by `2 * dwFrameCount` interleaved `{u32 len, byte[len]}` chunks — one full JPEG (`FF D8 FF DB ...`) then one PCM-audio block per frame. We auto-extract every JPEG to `*.frameNNN.jpg` and concatenate every audio chunk into a single playable `*.audio.wav`. Full RE in `ghidra_analysis/dsm_file_format.md`. |
| 94      | `Sign`            | uint32 packedDate, int32 len+UTF-16LE content, int32 len+UTF-16LE copyright, byte 0                        |
| 2026    | `Script`          | `CLevelScript`. int32 codeLen, int32 nExports, int32 nVars, byte[codeLen] bytecode, int32[nExports] entries — see disassembler below. |
| 2043    | `Poem`            | `CPoem`. u32 charCount, char[charCount] UTF-16LE. First codepoint usually `0x0001` (section/line marker). Decoded to `*.txt`. |
| 2050    | `HistoryScript`   | `CHistoryScript` — a `CDSScript` subclass behind the history dialog. Wire format identical to ClassID 2026; only the runtime opcode-45..52 extension differs (8 entries at `0x004af7ac`, not yet named). Disassembled with the base 0..44 opcode set; ext opcodes render as `<UNKNOWN op=N>`. |
| 2076    | `HelpScript`      | `CHelpScript` — same as 2050 but for the help dialog (ext table at `0x004af2fc`). |

The per-language UI string pool used by every dialog in `bulanci.exe`
(`CDSStaticTexts`, 127 UTF-16LE entries) is **not** a `.eap` resource —
it's baked into the executable's `.data` section. Dump it with
[`ghidra_analysis/static_texts.py`](ghidra_analysis/static_texts.py); see
[`ghidra_analysis/static_texts.md`](ghidra_analysis/static_texts.md) for
the layout.

## Script bytecode disassembler

`Script` (ClassID 2026) bodies are disassembled to `*.script.asm` per the
opcode table in [`Opcode.cs`](../../editor_il_spy/Editor.Scripts/Opcode.cs).
Each function entry starts with `byte varCount` followed by commands. The
disassembler renders inline nested arguments compactly, e.g.

```
fn export#1 @ 0x0093  ; OnInit()  -- fires once during CBulanci construction; level scene setup happens here
  ; varCount=0
    @0x0094  RegisterTimer(IntConst(0), Add(Mul(Rand(IntConst(0), IntConst(10)), IntConst(1000)), IntConst(5000)), IntConst(6))
    @0x00b6  LoadPreface(65640)
    @0x00bb  SetMusic(65863, 0)
    @0x00c4  SetInsertMode(IntConst(0))
    @0x00ca  InsertView(CreateImage(IntConst(0), IntConst(0), 65643))
    @0x00da  SetInsertMode(IntConst(2))
    @0x00e0  InsertView(CreateObstacle(IntConst(170), IntConst(140), IntConst(265), IntConst(180)))
    ...
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

The script's **exports table is positional** — index N is wired to a
fixed lifecycle event by the engine. A `CLevelScript` always declares
exactly 11 exports (`exports[0..10]`); each is fired by a different
engine call-site, with a fixed argc and a fixed argv shape. The
disassembler renders every export as `fn export#N @ 0xNNNN  ; Name(args)
-- when it fires`, e.g.

```
fn export#3  @ 0x04bc  ; OnBitmapEvt(slot, evt)   -- fires when a CBitmap
                                                    sub-view emits an event …
fn export#6  @ 0x0935  ; OnTimer(slotId)          -- fires when a
                                                    RegisterTimer countdown
                                                    hit zero …
fn export#10 @ 0x049a  ; OnGameStart()            -- fires when the level
                                                    transitions from
                                                    loaded/paused to running …
```

Note the second example: `OnGameStart` lives at a *lower* bytecode
address (0x049a) than `OnBitmapEvt` (0x04bc) — the master pack scripts
do **not** put their exports in address order, so the disassembler is
careful to label by exports-table position and pick each function's end
from the sorted-address neighbour. The full per-export contract (which
engine function fires it, what each argument means, what triggers it in
the game world) is in
[`ghidra_analysis/script_lifecycle.md`](ghidra_analysis/script_lifecycle.md).

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

- **`BitmapSprite` (ClassID 52) recolour-variant palettes.** The
  container, frame stream and per-opcode pixel decoders are now fully
  recovered and the unpacker emits indexed PNGs plus a per-sprite
  atlas. 89/130 sprites carry their palette inline (FLX opcode 0x09)
  and decode to correct full-colour PNGs. The remaining 41/130 are
  recolour variants whose palette is inherited from a sibling resource
  via the engine's ambient render context; the unpacker currently
  falls back to a grayscale ramp for those. Picking the right sibling
  palette automatically would let us emit colour PNGs for them too;
  the manifest's `decoded.paletteSource` field distinguishes the two
  cases. See [`ghidra_analysis/sprite_container.md`](ghidra_analysis/sprite_container.md)
  and [`ghidra_analysis/flx_file_format.md`](ghidra_analysis/flx_file_format.md).
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
