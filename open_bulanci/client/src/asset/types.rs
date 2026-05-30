//! Zero-sized marker structs used as the type parameter on
//! [`super::AssetHandle`].
//!
//! Each catalog `className` in `registry.json` maps to exactly one
//! struct here, with the name fixed by `_CLASS_TO_RUST` in
//! `open_bulanci/asset_pipeline/build_typed_handles.py`.  Adding a
//! new asset class is "add an entry to that dict + add the matching
//! struct below" — both halves must move together or the generator's
//! `--check` step will refuse to write.
//!
//! These types deliberately carry no data.  Their only job is to
//! distinguish handle kinds at compile time so the loader's
//! `load::<T>` method can return the matching `T::Loaded` type
//! (texture / atlas / bytes / decoded font / string) without a cast.
//!
//! Every marker implements [`AssetClass`] (in `super::class`) so the
//! [`AssetServer`](super::server::AssetServer) can resolve a typed
//! handle to a class-appropriate **placeholder** when the underlying
//! asset is missing (deleted in the editor, in-flight reload, corrupt
//! bytes, …).  The placeholder is monomorphised at compile time —
//! there's no runtime class lookup on the hot path.

use super::class::AssetClass;
use super::fallback;

macro_rules! marker {
    ($($(#[$m:meta])* $name:ident => $placeholder:expr,)*) => {
        $(
            $(#[$m])*
            // `dead_code` is suppressed because these structs are never
            // constructed — they exist purely as phantom tags inside
            // `AssetHandle<T>`.  See the module-level doc-comment.
            #[allow(dead_code)]
            #[derive(Debug, Clone, Copy, Eq, PartialEq, Hash)]
            pub struct $name;

            impl AssetClass for $name {
                const NAME: &'static str = stringify!($name);
                #[inline] fn placeholder() -> &'static [u8] { $placeholder }
            }
        )*
    };
}

marker! {
    /// Full-screen JPEG (`CLASS_BITMAP_JPEG = 42`).  Decoded to a
    /// `macroquad::Texture2D`.  Falls back to a 1×1 magenta pixel.
    BitmapJpeg        => fallback::MAGENTA_1X1_PNG,
    /// Alpha-masked PNG sprite (`CLASS_BITMAP_SPECIAL = 28`).
    /// Decoded to a `macroquad::Texture2D`.
    BitmapSpecial     => fallback::MAGENTA_1X1_PNG,
    /// Animated sprite strip — the `atlas.json` sidecar
    /// (`CLASS_BITMAP_SPRITE = 52`).  Pairs at codegen time with a
    /// [`BitmapSpritePng`] sibling for the actual pixels.
    BitmapSprite      => fallback::EMPTY_ATLAS_JSON,
    /// PNG image sibling of a [`BitmapSprite`] atlas.  Emitted
    /// automatically by `build_typed_handles.py` so the atlas loader
    /// can fetch both halves of an atlas through typed handles only.
    BitmapSpritePng   => fallback::MAGENTA_1X1_PNG,
    /// JPEG + audio cinematic animation (`CLASS_BITMAP_JPEG_ANIM`).
    /// Loads as a sequence of frames + a soundtrack handle.
    BitmapJpegAnim    => fallback::EMPTY_ATLAS_JSON,
    /// Streamed MP3 track (`CLASS_MP3 = 56`).  Returned as raw bytes
    /// for `rodio::Decoder` to pull through.  Placeholder is a 1-frame
    /// silent WAV — rodio decodes both via the same `Decoder::new`.
    Mp3               => fallback::SILENT_WAV,
    /// Multi-sample audio bank (`CLASS_AUDIO_BANK = 43`).  Returned
    /// as a `BankBytes` wrapper that knows how to slice individual
    /// samples by index.
    AudioBank         => fallback::SILENT_WAV,
    /// A single WAV slice extracted from an [`AudioBank`].  Lives at
    /// `audio/<sample_slug>.wav` after the polyphase 22 050 → 48 000
    /// Hz resample.  Each entry in an asset's `samples` table
    /// produces one of these handles, keyed by
    /// `<bank_folder>/<sample_slug>` so it's addressable without
    /// going through the bank.  Placeholder is silent so missing
    /// SFX never crash, they just don't audibly play.
    AudioSample       => fallback::SILENT_WAV,
    /// Bitmap font glyph atlas — the PNG (`CLASS_FONT = 1`).  Pairs
    /// at codegen time with a [`FontJson`] sibling for the metrics.
    Font              => fallback::MAGENTA_1X1_PNG,
    /// Metrics JSON sibling of a [`Font`].  Same compound-handle
    /// pattern as [`BitmapSpritePng`]: emitted automatically so the
    /// font loader gets bytes for both halves via typed handles.
    FontJson          => fallback::EMPTY_ATLAS_JSON,
    /// Cursor composition JSON (the hand-baked
    /// `assets/cursor/cursor.json` that lists the three idle-twitch
    /// atlas slugs in `CGunMouse_ctor` order).  Manual asset — no
    /// catalog resource ID.
    CursorComposition => fallback::EMPTY_CURSOR_JSON,
    /// Czech poem (`CLASS_POEM = 1030`).  Returned as the raw UTF-8
    /// string with the per-line alignment markers preserved.
    Poem              => fallback::EMPTY_TEXT,
    /// Compiled engine script (`CLASS_SCRIPT = 2026`).  Returned as
    /// the disassembled opcode buffer for the VM to execute.
    Script            => fallback::EMPTY_BYTES,
    /// History-script bridge node (`CLASS_HISTORY_SCRIPT = 2027`).
    /// Returned as a script reference + audio + frame timing.
    HistoryScript     => fallback::EMPTY_BYTES,
    /// Nested DSM container (`CLASS_DSM_INNER`).  Used by the asset
    /// pipeline; the client rarely touches these directly.
    DsmInner          => fallback::EMPTY_BYTES,
}
