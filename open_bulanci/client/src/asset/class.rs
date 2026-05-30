//! The [`AssetClass`] trait — bridge between the phantom-typed
//! [`AssetHandle<T>`](super::handle::AssetHandle) world and the
//! byte-level loader / fallback machinery.
//!
//! Every marker in [`super::types`] implements `AssetClass`.  The
//! trait gives the [`AssetServer`](super::server::AssetServer) two
//! things, both resolved at compile time:
//!
//! * `NAME`        — a short, allocation-free label used by debug
//!                   logs.  Kept on the *type*, not on the manifest,
//!                   so release builds can omit the manifest's
//!                   `class:` field entirely without losing the
//!                   diagnostic.
//! * `placeholder` — the bytes returned when the underlying asset is
//!                   missing.  Each marker picks a class-appropriate
//!                   placeholder (silent WAV / 1×1 magenta PNG /
//!                   empty atlas JSON / …) so downstream decoders
//!                   never see a "real-file-but-empty" surprise.
//!
//! There's no `class_id: u8` or dynamic dispatch: the relevant
//! `placeholder()` call site monomorphises against the concrete
//! marker so the cost is "a static address" instead of "a hashmap
//! lookup + jumptable".

/// Bridge from a phantom marker type (e.g. `AudioSample`,
/// `BitmapJpeg`) to the bytes the runtime should yield when the real
/// asset can't be loaded.
///
/// Implementing this on a new marker is the second half of "add a
/// new asset class": after extending the macro in `types.rs` you
/// don't write any extra code — the macro generates the impl for
/// you.  The placeholder bytes themselves live in
/// [`super::fallback`].
pub trait AssetClass: 'static {
    /// Short human label.  `"AudioSample"`, `"BitmapJpeg"`, … —
    /// matches the marker's struct name by construction.  Only used
    /// for debug logging.
    const NAME: &'static str;

    /// Bytes to hand back to the consumer when the asset is missing
    /// or fails to load.  Must be decodable by whatever the consumer
    /// passes the bytes through (e.g. silent WAV for `rodio`,
    /// 1×1 PNG for `image::load_from_memory`).
    fn placeholder() -> &'static [u8];
}
