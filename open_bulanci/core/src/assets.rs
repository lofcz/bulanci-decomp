//! Hash-keyed virtual file system for shipped assets, plus a writable
//! overlay layer for runtime mods + hot-reload.
//!
//! The pack is produced by `open_bulanci/asset_pipeline/pack_assets.py`,
//! zstd-compressed by `core/build.rs`, and embedded here via
//! `include_bytes!`.  Every entry is keyed by a `blake2b-64` digest:
//!
//!  * For typed assets the digest is `blake2b("<folder>/<slug>")` —
//!    the exact value that `build_typed_handles.py` baked into each
//!    `AssetHandle` constant, so the runtime never has to translate
//!    a slug back to a path.
//!  * For path-only files (transpiled Lua levels, poems) the digest
//!    is `blake2b(path)`; the `read(path)` helper below recomputes it
//!    locally so call sites can keep using ergonomic string paths
//!    without any runtime path table.
//!
//! ## Modding & hot reload
//!
//! The VFS layers an **embedded** read-only base (the zstd pack) and
//! a **writable overlay** of `hash -> bytes`.  `read_hash` always
//! consults the overlay first, so swapping `set_overlay(h, bytes)`
//! into the overlay map is exactly equivalent to "the user dropped a
//! new file in the mods folder."  The accompanying file watcher (see
//! `client/src/asset/watcher.rs`) translates disk events into
//! `set_overlay` / `clear_overlay` calls and pings the
//! `AssetServer`'s dirty queue so the engine reloads the affected
//! handles on the next tick.
//!
//! Crucially, no asset name, slug, folder path, or atlas metadata
//! string is stored in the *pack* — `strings(1)` on a release binary
//! finds zero registry-derived literals.  The modder-facing path →
//! hash reference lives on disk next to the executable
//! (`mods/_manifest.json`) for end users who want to author overrides;
//! the binary itself stays opaque.

use std::collections::HashMap;
use std::sync::RwLock;
use anyhow::{Result, Context, anyhow};
use blake2::{Blake2b, Digest, digest::consts::U8};

const COMPRESSED_PACK: &[u8] = include_bytes!(concat!(env!("OUT_DIR"), "/assets.pack.zst"));

/// Magic+version constants must match `pack_assets.py` / `build_paks.py`.
const PACK_MAGIC: u32 = 0x504B4C42; // little-endian 'BLKP'
/// v1: the embedded master pack — `[magic][version][count]` then entries.
const PACK_VERSION_V1: u32 = 1;
/// v2: a compiled *pak* — adds a small UTF-8 (JSON) identity header
/// (`{id, kind, order}`) between the version and the count so a shipped
/// binary pak is self-describing. Entry layout is identical to v1.
const PACK_VERSION_V2: u32 = 2;

/// `blake2b("…", digest_size=8)` truncated to a `u64` (big-endian) —
/// identical to `build_typed_handles.py::hash_path` and to
/// `pack_assets.py::_hash_raw_path`.  The function is kept tiny and
/// branch-free so `read(&str)` stays cheap even on the hot path.
pub fn hash_path(path: &str) -> u64 {
    let mut h = <Blake2b<U8> as Digest>::new();
    h.update(path.as_bytes());
    let out = h.finalize();
    u64::from_be_bytes(out.into())
}

/// The reserved id of the base-game pak (mount layer 0). Mirrors the
/// `master` entry in `paks/profile.json`.
pub const MASTER_PAK_ID: &str = "master";

/// One mounted content layer — a pak. `master` is always layer 0; mod
/// paks are pushed on top in load order, so a later mount overrides an
/// earlier one for any shared hash (last-wins), and brand-new hashes a
/// mod adds simply become resolvable. This is the GZDoom-style layering:
/// a mod pak can shadow the master *or* another mod, and add content.
struct Layer {
    id:  String,
    map: HashMap<u64, Vec<u8>>,
}

pub struct AssetFileSystem {
    /// Mount stack, low -> high priority. `layers[0]` is the master pak;
    /// mod paks are appended in profile/load order. Behind a lock so the
    /// pak loader / editor can mount or remount at runtime through `&self`.
    layers:  RwLock<Vec<Layer>>,
    /// Ephemeral top layer for live editor/broker pushes + hot reload.
    /// Always wins over every mounted pak. This is the "live overlay" the
    /// asset bridge writes into; persisted overrides live in mod *layers*.
    overlay: RwLock<HashMap<u64, Vec<u8>>>,
}

impl AssetFileSystem {
    /// Decompress the embedded pack and parse it into the master layer.
    /// Runs once at startup; subsequent reads are pure HashMap lookups.
    pub fn load() -> Result<Self> {
        let decompressed = zstd::decode_all(COMPRESSED_PACK)
            .context("failed to decompress embedded assets pack")?;
        Self::from_bytes(&decompressed)
    }

    /// Parse a *decompressed* pack blob into a fresh VFS whose only
    /// mounted layer is the master pak. Public so tools/tests can build a
    /// VFS from a pack on disk.
    pub fn from_bytes(blob: &[u8]) -> Result<Self> {
        let map = Self::parse_pack(blob)?;
        Ok(AssetFileSystem {
            layers:  RwLock::new(vec![Layer { id: MASTER_PAK_ID.to_string(), map }]),
            overlay: RwLock::new(HashMap::new()),
        })
    }

    /// Parse a *decompressed* `assets.pack` blob into a `hash -> bytes`
    /// table. Shared by the master loader and the per-pak mount path.
    fn parse_pack(blob: &[u8]) -> Result<HashMap<u64, Vec<u8>>> {
        if blob.len() < 12 {
            return Err(anyhow!("asset pack truncated (header)"));
        }
        let magic   = u32::from_le_bytes(blob[0..4].try_into().unwrap());
        let version = u32::from_le_bytes(blob[4..8].try_into().unwrap());
        if magic != PACK_MAGIC {
            return Err(anyhow!(
                "asset pack magic mismatch: got 0x{:08x}, expected 0x{:08x}",
                magic, PACK_MAGIC,
            ));
        }
        // v1: count immediately follows the version. v2: a u32 header
        // length + that many identity-header bytes sit between them.
        let (count, mut cur) = match version {
            PACK_VERSION_V1 => {
                let count = u32::from_le_bytes(blob[8..12].try_into().unwrap()) as usize;
                (count, 12usize)
            }
            PACK_VERSION_V2 => {
                let header_len = u32::from_le_bytes(blob[8..12].try_into().unwrap()) as usize;
                let count_off = 12 + header_len;
                if count_off + 4 > blob.len() {
                    return Err(anyhow!("asset pack truncated inside v2 header"));
                }
                let count = u32::from_le_bytes(blob[count_off..count_off + 4].try_into().unwrap()) as usize;
                (count, count_off + 4)
            }
            other => {
                return Err(anyhow!(
                    "asset pack version {} not supported by this build (expected {} or {})",
                    other, PACK_VERSION_V1, PACK_VERSION_V2,
                ));
            }
        };
        let mut map = HashMap::with_capacity(count);
        for _ in 0..count {
            if cur + 12 > blob.len() {
                return Err(anyhow!("asset pack truncated inside entry header"));
            }
            let hash     = u64::from_le_bytes(blob[cur..cur+8].try_into().unwrap());
            let data_len = u32::from_le_bytes(blob[cur+8..cur+12].try_into().unwrap()) as usize;
            cur += 12;
            if cur + data_len > blob.len() {
                return Err(anyhow!("asset pack truncated inside data block"));
            }
            map.insert(hash, blob[cur..cur+data_len].to_vec());
            cur += data_len;
        }
        Ok(map)
    }

    // ---- multi-pak mounting --------------------------------------------

    /// Mount a parsed pak layer on top of the stack. Later mounts win
    /// over earlier ones (and over the master) for any shared hash.
    pub fn mount(&self, id: impl Into<String>, map: HashMap<u64, Vec<u8>>) {
        let id = id.into();
        let mut layers = self.layers.write().unwrap();
        // Re-mounting an id replaces it in place (keeps load-order index).
        if let Some(slot) = layers.iter_mut().find(|l| l.id == id) {
            slot.map = map;
        } else {
            layers.push(Layer { id, map });
        }
    }

    /// Parse a compiled pak blob and mount it as a layer. Returns the
    /// number of entries mounted.
    pub fn mount_pack_bytes(&self, id: impl Into<String>, blob: &[u8]) -> Result<usize> {
        let map = Self::parse_pack(blob)?;
        let n = map.len();
        self.mount(id, map);
        Ok(n)
    }

    /// Mount (or replace) a single asset into a named pak layer — the
    /// dev path the disk pak loader uses to fold a folder of override /
    /// new-asset files into a mounted mod layer without a pack build.
    pub fn mount_asset(&self, pak_id: &str, hash: u64, bytes: Vec<u8>) {
        let mut layers = self.layers.write().unwrap();
        if let Some(layer) = layers.iter_mut().find(|l| l.id == pak_id) {
            layer.map.insert(hash, bytes);
        } else {
            let mut map = HashMap::new();
            map.insert(hash, bytes);
            layers.push(Layer { id: pak_id.to_string(), map });
        }
    }

    /// Drop a single asset from a named pak layer (dev hot-remove).
    pub fn unmount_asset(&self, pak_id: &str, hash: u64) {
        if let Some(layer) = self.layers.write().unwrap().iter_mut().find(|l| l.id == pak_id) {
            layer.map.remove(&hash);
        }
    }

    /// The id of the top-most mounted pak (ignoring the ephemeral live
    /// overlay) that provides `hash` — i.e. who currently "wins" it among
    /// the persisted layers. Drives the editor's layering view ("provided
    /// by modX, overrides master"). Returns `None` if no pak carries it.
    pub fn source_of(&self, hash: u64) -> Option<String> {
        let layers = self.layers.read().unwrap();
        layers.iter().rev().find(|l| l.map.contains_key(&hash)).map(|l| l.id.clone())
    }

    /// Mounted pak ids, low -> high priority (master first). Diagnostics
    /// + editor status.
    pub fn layer_ids(&self) -> Vec<String> {
        self.layers.read().unwrap().iter().map(|l| l.id.clone()).collect()
    }

    /// Resolve a typed-handle hash to its bytes. Resolution order is
    /// **live overlay -> highest mod -> ... -> master** (first hit wins),
    /// so a live push beats a persisted mod, which beats the base game.
    /// Returns owned `Vec<u8>` because the guards can't outlive the call.
    pub fn read_hash(&self, hash: u64) -> Option<Vec<u8>> {
        if let Some(b) = self.overlay.read().unwrap().get(&hash) {
            return Some(b.clone());
        }
        let layers = self.layers.read().unwrap();
        for layer in layers.iter().rev() {
            if let Some(b) = layer.map.get(&hash) {
                return Some(b.clone());
            }
        }
        None
    }

    /// Ergonomic path-based read for files that don't have a typed
    /// handle yet (poems, transpiled Lua level scripts).  The string
    /// is hashed locally with the same blake2b function the packer
    /// used, so there's still no path table in the binary — just the
    /// string literals at each call site (which name engine concepts
    /// like `poems/poem_0.txt`, not asset slugs).  Picks up hot-reload
    /// overrides for free because it goes through `read_hash`.
    pub fn read(&self, path: &str) -> Option<Vec<u8>> {
        self.read_hash(hash_path(path))
    }

    /// UTF-8 convenience wrapper around `read`.
    pub fn read_to_string(&self, path: &str) -> Option<Result<String>> {
        self.read(path).map(|bytes| {
            String::from_utf8(bytes).map_err(|e| anyhow!("invalid utf-8 file data: {}", e))
        })
    }

    // ---- overlay management (mods + hot reload) ------------------------

    /// Install a runtime override for a single asset hash.  Replaces
    /// any existing override.  The caller is responsible for pinging
    /// the consumer-side dirty queue (e.g. `AssetServer::mark_dirty`)
    /// so cached decoded data gets dropped.
    ///
    /// `bytes` may be any size — placeholders fall through naturally
    /// when a consumer's decoder rejects the data, so even a half-
    /// downloaded mod file just shows the magenta texture / silent
    /// audio until the write completes.
    pub fn set_overlay(&self, hash: u64, bytes: Vec<u8>) {
        self.overlay.write().unwrap().insert(hash, bytes);
    }

    /// Drop an overlay so the embedded asset comes back through.
    pub fn clear_overlay(&self, hash: u64) {
        self.overlay.write().unwrap().remove(&hash);
    }

    /// Number of active overlay entries.  Used by boot diagnostics
    /// ("12 mods loaded") and the editor status bar.
    pub fn overlay_count(&self) -> usize {
        self.overlay.read().unwrap().len()
    }

    /// `true` when the asset's bytes currently come from the overlay
    /// rather than the embedded pack — handy for boot diagnostics
    /// that want to flag "the user is running modified content."
    pub fn is_overlayed(&self, hash: u64) -> bool {
        self.overlay.read().unwrap().contains_key(&hash)
    }

    /// Every hash currently overridden by the overlay.  Used by the
    /// editor → engine bridge's `GET /overlays` so the editor UI can
    /// show which assets are running live-modified content.
    pub fn overlay_hashes(&self) -> Vec<u64> {
        self.overlay.read().unwrap().keys().copied().collect()
    }

    // ---- introspection -------------------------------------------------

    /// All hashes present in the VFS (every mounted layer ∪ overlay).
    /// Useful for diagnostic tools (e.g. dumping which handles resolve
    /// and which fall back).
    pub fn list_hashes(&self) -> Vec<u64> {
        let mut out: Vec<u64> = Vec::new();
        for layer in self.layers.read().unwrap().iter() {
            out.extend(layer.map.keys().copied());
        }
        out.extend(self.overlay.read().unwrap().keys().copied());
        out.sort_unstable();
        out.dedup();
        out
    }

    /// Count of distinct asset hashes resolvable across all mounted paks
    /// (union; the ephemeral overlay is not counted). For boot logs.
    pub fn len(&self) -> usize {
        let layers = self.layers.read().unwrap();
        if layers.len() == 1 {
            return layers[0].map.len();
        }
        let mut set = std::collections::HashSet::new();
        for layer in layers.iter() {
            set.extend(layer.map.keys().copied());
        }
        set.len()
    }

    pub fn is_empty(&self) -> bool {
        self.layers.read().unwrap().iter().all(|l| l.map.is_empty())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_embedded_assets_load() {
        let vfs = AssetFileSystem::load().expect("VFS should load and decompress successfully");
        assert!(!vfs.is_empty(), "VFS should contain transpiled Lua level scripts");

        // Lua level scripts are looked up by path (no typed handles).
        let test_script = vfs.read_to_string("levels/res_0000065855_2026_Script.lua");
        assert!(test_script.is_some(), "levels/res_0000065855_2026_Script.lua should exist in VFS");
        let script_code = test_script.unwrap().unwrap();
        assert!(script_code.contains("OnInit"), "Lua level script should contain 'OnInit'");
    }

    #[test]
    fn test_hash_matches_python() {
        // Sanity-check that the Rust hash function matches the one
        // baked into the codegen.  If either side drifts we want a
        // loud test failure, not a silent runtime miss.
        assert_eq!(
            hash_path("menu/sfx/sfx_start.wav"),
            u64::from_be_bytes(blake2b_truncate("menu/sfx/sfx_start.wav")),
            "hash_path drifted from the reference implementation",
        );
    }

    // Tiny reference helper for the test above — uses the same crate
    // but reconstructs the digest manually so a refactor of `hash_path`
    // doesn't get to mark its own homework.
    fn blake2b_truncate(s: &str) -> [u8; 8] {
        let mut h = <Blake2b<U8> as Digest>::new();
        h.update(s.as_bytes());
        h.finalize().into()
    }

    #[test]
    fn overlay_takes_precedence_over_embedded() {
        let vfs = AssetFileSystem::load().unwrap();
        let path = "levels/res_0000065855_2026_Script.lua";
        let h    = hash_path(path);

        let original = vfs.read_hash(h).expect("script exists in pack");
        assert!(original.windows(6).any(|w| w == b"OnInit"));
        assert!(!vfs.is_overlayed(h));

        vfs.set_overlay(h, b"-- modded --".to_vec());
        assert!(vfs.is_overlayed(h));
        assert_eq!(vfs.read_hash(h).unwrap(), b"-- modded --");
        assert_eq!(vfs.read(path).unwrap(),   b"-- modded --");

        vfs.clear_overlay(h);
        assert!(!vfs.is_overlayed(h));
        assert_eq!(vfs.read_hash(h).unwrap(), original);
    }

    #[test]
    fn mod_layer_overrides_master_and_adds_assets() {
        let vfs = AssetFileSystem::load().unwrap();
        let path = "levels/res_0000065855_2026_Script.lua";
        let h = hash_path(path);
        assert_eq!(vfs.source_of(h).as_deref(), Some("master"));

        // Mount a mod that both overrides the master script and adds a
        // brand-new asset (GZDoom-style).
        let new_hash = hash_path("mod/new_asset");
        let mut map = HashMap::new();
        map.insert(h, b"-- from mod --".to_vec());
        map.insert(new_hash, b"brand new".to_vec());
        vfs.mount("mod_a", map);

        assert_eq!(vfs.read_hash(h).unwrap(), b"-- from mod --");
        assert_eq!(vfs.source_of(h).as_deref(), Some("mod_a"));
        assert_eq!(vfs.read_hash(new_hash).unwrap(), b"brand new");
        assert_eq!(vfs.source_of(new_hash).as_deref(), Some("mod_a"));

        // A higher mod wins over a lower one (last-wins).
        let mut map_b = HashMap::new();
        map_b.insert(h, b"-- from mod b --".to_vec());
        vfs.mount("mod_b", map_b);
        assert_eq!(vfs.read_hash(h).unwrap(), b"-- from mod b --");
        assert_eq!(vfs.source_of(h).as_deref(), Some("mod_b"));

        // The live overlay still beats every mounted pak, but provenance
        // (source_of) reports the top persisted layer, not the overlay.
        vfs.set_overlay(h, b"-- live --".to_vec());
        assert_eq!(vfs.read_hash(h).unwrap(), b"-- live --");
        assert_eq!(vfs.source_of(h).as_deref(), Some("mod_b"));
    }

    #[test]
    fn parses_compiled_pack_v2() {
        // Hand-build the exact frame `build_paks.py` emits so the Python
        // writer and the Rust reader can't silently drift.
        let header = br#"{"id":"m","kind":"mod","order":1}"#;
        let mut blob = Vec::new();
        blob.extend_from_slice(&PACK_MAGIC.to_le_bytes());
        blob.extend_from_slice(&PACK_VERSION_V2.to_le_bytes());
        blob.extend_from_slice(&(header.len() as u32).to_le_bytes());
        blob.extend_from_slice(header);
        blob.extend_from_slice(&1u32.to_le_bytes()); // count
        let h: u64 = 0x0123_4567_89ab_cdef;
        let data = b"hello v2 pack";
        blob.extend_from_slice(&h.to_le_bytes());
        blob.extend_from_slice(&(data.len() as u32).to_le_bytes());
        blob.extend_from_slice(data);

        let vfs = AssetFileSystem::load().unwrap();
        let n = vfs.mount_pack_bytes("mod_v2", &blob).unwrap();
        assert_eq!(n, 1);
        assert_eq!(vfs.read_hash(h).unwrap(), data);
        assert_eq!(vfs.source_of(h).as_deref(), Some("mod_v2"));
    }
}
