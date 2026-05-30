//! Strongly-typed asset addressing layer.
//!
//! The shipped binary references each asset only by an opaque 64-bit
//! hash; the actual folder / slug / Bulánci resource ID stays in the
//! project files (`registry.json`) and never makes it into the
//! executable.  See `open_bulanci/asset_pipeline/FOLDERS.md`.
//!
//! ## How to use this from the game code
//!
//! ```ignore
//! use crate::generated::assets;
//!
//! // The const carries the asset class in its type, so the server
//! // returns the matching placeholder when a load misses:
//! let bank: AssetHandle<AudioBank> = assets::menu::sfx::MENU_SFX_BANK;
//! let bytes: Arc<[u8]>             = self.assets.bytes(bank);
//! ```
//!
//! ## Module layout
//!
//! * [`handle`]   — phantom-typed `AssetHandle<T>` + its `const fn new`.
//! * [`types`]    — one zero-sized marker per catalog class
//!                  (`BitmapJpeg`, `AudioBank`, …).  Each marker
//!                  carries its class-typed placeholder via
//!                  [`class::AssetClass`].
//! * [`class`]    — the `AssetClass` trait the markers implement.
//! * [`fallback`] — embedded placeholder bytes (silent WAV, 1×1
//!                  magenta PNG, empty atlas JSON, …).
//! * [`server`]   — the [`AssetServer`] that funnels every typed load.
//!                  Caches bytes, falls back to placeholders, exposes
//!                  the hot-reload seam.
//!
//! There is no runtime manifest in this layer: the shipped pack is
//! keyed by the same `blake2b("<folder>/<slug>")` digest the codegen
//! baked into each `AssetHandle`, so the server resolves bytes with a
//! single VFS lookup — no `hash -> path` indirection, no slugs or
//! folders anywhere in `.rodata`.  The fat `manifest.json` next to
//! the pipeline output is dev-tooling only and never ships.

// The asset layer is migrated to gradually — many of the helpers below
// will see their first caller weeks or months after the foundation lands.
// Warnings are suppressed in here so they don't bury the few genuine
// ones a real bug would produce.
#![allow(dead_code, unused_imports)]

pub mod class;
pub mod fallback;
pub mod handle;
pub mod server;
pub mod types;

// Disk-backed mods + hot-reload watcher.  Native-only — wasm builds
// rely on the editor → engine bridge instead of disk IO.
#[cfg(not(target_arch = "wasm32"))]
pub mod mods;

// Editor → engine live-mod link.  Native-only, opt-in via
// `BULANCI_DEV_EDITOR`.  Connects *out* to the asset studio's Vite
// mod-broker over a WebSocket and applies pushed overlays / scene patches
// through the same `install_overlay` path as the disk watcher — so a
// (re)opened dev client catches up to the editor's live state.
#[cfg(not(target_arch = "wasm32"))]
pub mod ws_dev;

// Editor → engine live-overlay bridge for the web build.  The wasm tab
// can't listen, so it receives overlay pushes over a WebSocket from the
// asset studio's dev-server broker and applies them through the exported
// functions here (same `install_overlay` path as the native bridge).
#[cfg(target_arch = "wasm32")]
pub mod web_bridge;

pub use class::AssetClass;
pub use handle::AssetHandle;
pub use server::AssetServer;
