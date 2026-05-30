//! Tiny embedded byte blobs used as class-typed placeholders.
//!
//! Every marker in [`super::types`] picks one of these via its
//! [`AssetClass::placeholder`](super::class::AssetClass::placeholder)
//! impl.  The bytes are static so the [`AssetServer`]
//! (super::server::AssetServer) hands them out behind a `Arc::from`
//! without a heap allocation on the hot path.
//!
//! Why placeholders matter: in dev/mod scenarios an asset can be
//! deleted from disk while the engine is still running.  Without a
//! fallback the next `vfs.read(path)` returns `None`, the consumer
//! either panics (font init, cursor atlas) or silently malfunctions
//! (audio plays nothing-but-doesn't-crash).  With a placeholder the
//! downstream decoder still produces a valid `Texture2D` /
//! `Decoder` / `BitmapFont`, the screen renders a magenta square
//! where the texture used to be, and the user gets an immediate
//! visual signal that *that* asset is gone — no scrolling logs, no
//! incidental crashes.
//!
//! The bytes are intentionally TINY (the PNG is 68 B, the WAV is
//! 46 B) so we don't bloat the binary with chunky fallbacks.  All
//! the actual visible/audible-ness is "minimal but valid"; replace
//! a per-class placeholder with something fancier (a checkerboard,
//! a "missing" voice line) by editing this file.

// ---------------------------------------------------------------------
// images — every visual class falls back to the magenta pixel
// ---------------------------------------------------------------------

/// 1×1 RGB magenta PNG (R=0xFF, G=0x00, B=0xFF).  Decodes via the
/// `image` crate inside macroquad — same code path `BitmapJpeg` /
/// `BitmapSpecial` / `Font` use for the real files.
///
/// We use PNG (not JPEG) because PNG's encoder is deterministic and
/// produces a smaller blob at 1×1.  macroquad's JPEG support also
/// can't decode arbitrarily small JPEGs — they require >= 8×8 due
/// to the DCT block — so a tiny JPEG fallback would itself need a
/// fallback.  PNG works at any size.
pub const MAGENTA_1X1_PNG: &[u8] = &[
    137, 80, 78, 71, 13, 10, 26, 10,
    0, 0, 0, 13, 73, 72, 68, 82,
    0, 0, 0, 1, 0, 0, 0, 1,
    8, 2, 0, 0, 0, 144, 119, 83, 222,
    0, 0, 0, 12, 73, 68, 65, 84,
    120, 156, 99, 248, 207, 240, 31, 0, 4, 0, 1, 255,
    34, 10, 58, 240,
    0, 0, 0, 0, 73, 69, 78, 68, 174, 66, 96, 130,
];

// ---------------------------------------------------------------------
// audio — one mono PCM sample, mathematically silent
// ---------------------------------------------------------------------

/// 22 050 Hz mono 16-bit PCM WAV containing exactly one zero sample.
///
/// Both `rodio`'s WAV decoder and its MP3 decoder accept a `&[u8]`
/// of WAV here (we tested in the audio module — the type isn't part
/// of the public API, the decoder sniffs from bytes).  An MP3 that
/// happens to decode to a single silent frame would be larger; a WAV
/// fits the same job in 46 bytes.
pub const SILENT_WAV: &[u8] = &[
    82, 73, 70, 70,                   // "RIFF"
    38, 0, 0, 0,                      // file size - 8
    87, 65, 86, 69,                   // "WAVE"
    102, 109, 116, 32,                // "fmt "
    16, 0, 0, 0,                      // fmt chunk size
    1, 0,                             // PCM
    1, 0,                             // mono
    34, 86, 0, 0,                     // 22 050 Hz
    68, 172, 0, 0,                    // byte rate
    2, 0,                             // block align
    16, 0,                            // bits per sample
    100, 97, 116, 97,                 // "data"
    2, 0, 0, 0,                       // data size
    0, 0,                             // 1 silent mono i16 sample
];

// ---------------------------------------------------------------------
// JSON-shaped fallbacks
// ---------------------------------------------------------------------

/// Empty-but-structurally-valid atlas sidecar.  Atlas / animation
/// loaders that expect a JSON descriptor land here when their asset
/// is missing.  The trailing newline keeps `serde_json` happy with
/// some strict modes.
pub const EMPTY_ATLAS_JSON: &[u8] = br#"{"frames":[],"sourceSize":{"w":1,"h":1}}"#;

/// Empty UTF-8 string — used for `Poem` and any text-y class.
pub const EMPTY_TEXT: &[u8]  = b"";

/// Zero-length blob — for opcode buffers and the like.  Distinct
/// constant from `EMPTY_TEXT` purely so a future placeholder can
/// change one without touching the other.
pub const EMPTY_BYTES: &[u8] = b"";

/// Structurally-valid empty cursor composition.  When the real
/// `cursor/cursor.json` is missing the deserializer still gets a
/// `schemaVersion: 1` document with an empty `idleTracks` list,
/// which `CursorAtlasSet::from_bytes` translates to "no tracks".
/// `CGunMouse::draw` then no-ops cleanly instead of panicking.
pub const EMPTY_CURSOR_JSON: &[u8] =
    br#"{"schemaVersion":1,"idleTracks":[]}"#;
