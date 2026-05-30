//! The runtime asset server.
//!
//! `AssetServer` owns the link between a typed
//! [`AssetHandle<T>`](super::handle::AssetHandle) and the bytes the
//! consumer ultimately decodes.  It's the **single funnel** every
//! asset load goes through — call sites never touch the VFS directly,
//! never touch a manifest, and never see a `&str` path.
//!
//! That funnel is what gets the rest of the design features for free:
//!
//! * **Caching**: an `Arc<[u8]>` per hash, behind a `RwLock`.  After
//!   warmup `bytes()` is a hash lookup + `Arc::clone` — no allocs.
//! * **Fallbacks**: when the VFS doesn't carry a given hash (the asset
//!   was deleted in the editor, the codegen wasn't re-run after a
//!   registry edit, the bytes are corrupt) we hand back the
//!   class-typed placeholder from [`AssetClass::placeholder`].
//!   Loads never panic, decoders never see empty bytes.
//! * **Hot-reload seam** (phase 2): a `dirty: HashSet<u64>` queue
//!   accepts invalidations from the file watcher.  The engine tick
//!   drains it; consumers re-fetch via `bytes()` and see fresh data.
//!   In release builds the queue stays compiled in (it's free when
//!   empty) but nobody writes to it.
//! * **No name leak**: the shipped pack is keyed by the same 64-bit
//!   hash the codegen baked into every `AssetHandle`, so the server
//!   resolves bytes with a single VFS lookup — no path table, no
//!   manifest, no slug or folder string anywhere in `.rodata`.
//!   Logging uses `AssetClass::NAME` (a `'static str` tied to the
//!   *type*, not to the asset).

use std::sync::{Arc, RwLock};
use std::collections::{HashMap, HashSet};

use bulanci_core::assets::AssetFileSystem;

use super::class::AssetClass;
use super::handle::{AssetHandle, RawHandle};

/// Runtime asset cache.  Wrap in `Arc` and clone it freely — every
/// piece of state is interior-mutable behind `RwLock`s, and the
/// `cache` lookups are read-mostly so contention is negligible.
pub struct AssetServer {
    vfs:   Arc<AssetFileSystem>,
    cache: RwLock<HashMap<u64, Arc<[u8]>>>,
    /// Hashes the (future) hot-reload watcher has marked dirty.  The
    /// engine tick drains this set and invalidates the matching
    /// cache entries; consumers re-fetch on the next `bytes()` call.
    /// In current builds (no watcher) this set is always empty —
    /// the cost is a single uncontended `read()` per drain.
    dirty: RwLock<HashSet<u64>>,
    /// Raw JSON scene-HMR patch payloads queued by the editor link
    /// ([`super::ws_dev`]'s op=3 frame, or the web `ob_scene_patch`
    /// export). Parsed + applied to the live `SceneManager` on the main
    /// thread each frame (see `ClientApp::update`). Mirrors `dirty`: pushed
    /// from any thread, drained once per frame. Always empty unless a Live
    /// editor session is streaming.
    scene_patches: RwLock<Vec<String>>,
}

impl AssetServer {
    /// Build a server from the already-loaded VFS.  Infallible — the
    /// VFS itself validates the pack header at startup, so by the
    /// time we get here there's nothing left for the server to fail
    /// on.
    pub fn new(vfs: Arc<AssetFileSystem>) -> Self {
        Self {
            vfs,
            cache: RwLock::new(HashMap::new()),
            dirty: RwLock::new(HashSet::new()),
            scene_patches: RwLock::new(Vec::new()),
        }
    }

    /// Number of asset entries the VFS holds (boot diagnostic).
    pub fn handle_count(&self) -> usize { self.vfs.len() }

    /// The underlying VFS — for the legacy `&str`-keyed paths that
    /// haven't been migrated to typed handles yet (poems, transpiled
    /// Lua level scripts).  Don't reach for this for new code; use
    /// `bytes()` instead.
    pub fn vfs(&self) -> &Arc<AssetFileSystem> { &self.vfs }

    // ---- the hot path ----------------------------------------------------

    /// Return the asset's bytes — or a class-typed placeholder if
    /// the asset is missing.
    ///
    /// `T: AssetClass` is monomorphised so the placeholder dispatch
    /// boils down to a static address.  On a cache hit the entire
    /// call is `read-lock → hashmap lookup → Arc::clone`; no
    /// allocations.  Cache misses copy the VFS slice into a fresh
    /// `Arc<[u8]>` exactly once and store it under a write lock.
    pub fn bytes<T: AssetClass>(&self, handle: AssetHandle<T>) -> Arc<[u8]> {
        let raw = handle.raw();

        // --- fast path: cache hit -----------------------------------
        if let Some(bytes) = self.cache.read().unwrap().get(&raw).cloned() {
            return bytes;
        }

        // --- slow path: load from VFS, populate cache --------------
        let loaded = self.load_bytes::<T>(raw);
        self.cache.write().unwrap().insert(raw, loaded.clone());
        loaded
    }

    /// `true` when the bytes returned by `bytes()` for this handle
    /// are the real asset (not the placeholder).  Useful for boot
    /// diagnostics that want to flag missing assets without paying
    /// the "load full bytes" cost twice.
    #[inline]
    pub fn is_real<T: AssetClass>(&self, handle: AssetHandle<T>) -> bool {
        self.vfs.read_hash(handle.raw()).is_some()
    }

    // ---- hot-reload seam ------------------------------------------------

    /// Mark a hash as needing reload.  Called by the file watcher or
    /// by the editor → engine bridge.  Safe to call from any thread.
    pub fn mark_dirty(&self, raw: RawHandle) {
        self.dirty.write().unwrap().insert(raw.0);
    }

    /// Drop a single cache entry so the next `bytes()` call re-reads
    /// from the VFS.  Available for editor flows that already know
    /// what changed and don't want to go through the dirty queue.
    pub fn invalidate(&self, raw: RawHandle) {
        self.cache.write().unwrap().remove(&raw.0);
    }

    /// Install a runtime override for one hash.  Pushes the bytes
    /// into the VFS overlay, drops the cached `Arc<[u8]>`, and queues
    /// the handle for the next tick's `drain_dirty` so consumers
    /// reload it (and the texture cache drops its matching GPU
    /// texture).  Used by both the file watcher and the editor →
    /// engine bridge — they're effectively the same operation, one
    /// driven by disk events and the other by IPC.
    pub fn install_overlay(&self, raw: RawHandle, bytes: Vec<u8>) {
        self.vfs.set_overlay(raw.0, bytes);
        self.invalidate(raw);
        self.mark_dirty(raw);
    }

    /// Drop a runtime override and re-queue the handle so the
    /// embedded version takes over on the next tick.
    pub fn remove_overlay(&self, raw: RawHandle) {
        self.vfs.clear_overlay(raw.0);
        self.invalidate(raw);
        self.mark_dirty(raw);
    }

    // ---- persisted pak layers (mods) -----------------------------------

    /// Install (or replace) an asset in a *persisted* pak layer (a mod
    /// pak), as opposed to the ephemeral live overlay used by
    /// [`install_overlay`](Self::install_overlay). Used by the disk pak
    /// loader/watcher to fold a folder of override/new-asset files into a
    /// mounted mod layer. Drops the cached bytes and queues the handle so
    /// the next tick reloads it.
    pub fn mount_pak_asset(&self, pak_id: &str, raw: RawHandle, bytes: Vec<u8>) {
        self.vfs.mount_asset(pak_id, raw.0, bytes);
        self.invalidate(raw);
        self.mark_dirty(raw);
    }

    /// Remove an asset from a persisted pak layer and re-queue it so a
    /// lower pak (or the master) takes over on the next tick.
    pub fn unmount_pak_asset(&self, pak_id: &str, raw: RawHandle) {
        self.vfs.unmount_asset(pak_id, raw.0);
        self.invalidate(raw);
        self.mark_dirty(raw);
    }

    /// Which pak currently provides `raw` (the top persisted layer),
    /// ignoring the live overlay. Drives the editor's layering view.
    pub fn source_of(&self, raw: RawHandle) -> Option<String> {
        self.vfs.source_of(raw.0)
    }

    /// Drain the dirty queue and clear matching cache entries.
    /// Returns the set of hashes that were dropped so the caller
    /// (`ClientApp::update`) can invalidate downstream caches —
    /// most importantly the GPU texture cache, which holds decoded
    /// `Texture2D`s and won't pick up new bytes on its own.
    ///
    /// Designed to be called once per frame (the engine tick already
    /// does that for the scheduler).  Cheap when nothing's dirty:
    /// one uncontended `read()` followed by an early-return on an
    /// empty `HashSet`.
    pub fn drain_dirty(&self) -> HashSet<u64> {
        if self.dirty.read().unwrap().is_empty() { return HashSet::new(); }
        let drained: HashSet<u64> = std::mem::take(&mut *self.dirty.write().unwrap());
        let mut cache = self.cache.write().unwrap();
        for h in &drained { cache.remove(h); }
        drained
    }

    // ---- scene HMR patch queue -----------------------------------------

    /// Queue a raw JSON scene-HMR patch from the editor bridge. Thread-safe;
    /// parsed + applied on the main thread by [`drain_scene_patches`].
    pub fn push_scene_patch(&self, json: String) {
        self.scene_patches.write().unwrap().push(json);
    }

    /// Drain all queued scene patches (FIFO). Cheap when empty: one
    /// uncontended `read()` then an early return.
    pub fn drain_scene_patches(&self) -> Vec<String> {
        if self.scene_patches.read().unwrap().is_empty() {
            return Vec::new();
        }
        std::mem::take(&mut *self.scene_patches.write().unwrap())
    }

    /// Number of active overlay entries (mods + live editor pushes).
    /// For boot logs / status bar.
    pub fn overlay_count(&self) -> usize { self.vfs.overlay_count() }

    /// Every hash currently served from the overlay (mods + live
    /// editor pushes).  Exposed for the editor → engine bridge's
    /// `GET /overlays` introspection endpoint.
    pub fn overlay_hashes(&self) -> Vec<u64> { self.vfs.overlay_hashes() }

    // ---- internals ------------------------------------------------------

    fn load_bytes<T: AssetClass>(&self, raw: u64) -> Arc<[u8]> {
        match self.vfs.read_hash(raw) {
            Some(b) => Arc::from(b),
            None => {
                // No slug / no folder available here — that's exactly
                // the point.  The class name is a `'static` tied to
                // the type, so it stays a single literal regardless
                // of how many handles share the class.
                eprintln!(
                    "[asset] {}: handle 0x{:016x} missing in VFS — placeholder",
                    T::NAME, raw,
                );
                Arc::from(T::placeholder())
            }
        }
    }
}
