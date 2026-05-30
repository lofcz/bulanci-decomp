//! Disk-backed pak loading + hot reload for the asset server.
//!
//! This is the native loader for the layered pak system (see
//! `open_bulanci/paks/` and [`bulanci_core::assets::AssetFileSystem`]).
//! It bridges four pieces:
//!
//!  * a **pak profile** (`paks/profile.json`) listing the active paks in
//!    load order (low -> high priority),
//!  * each **mod pak folder** (`paks/<id>/` with a `pak.json` manifest +
//!    content files), which is mounted as a *persisted layer* over the
//!    embedded master — a later pak overrides an earlier one and may add
//!    brand-new assets (GZDoom-style layering),
//!  * the **`AssetServer`** layer/overlay API, which the mounts and the
//!    file watcher drive, plus the dirty queue that reloads consumers,
//!  * a legacy **master dev-overlay watch** of `open_bulanci/assets/` so
//!    editing the master source tree still hot-reloads in a dev checkout
//!    (the master itself ships as the embedded pack).
//!
//! Web (wasm32) and platforms where `notify` can't initialise skip the
//! watchers; the embedded master still works and live overlays arrive
//! through the editor -> engine bridge instead.

#![cfg(not(target_arch = "wasm32"))]

use std::collections::HashMap;
use std::path::{Path, PathBuf};
use std::sync::Arc;

use anyhow::{Context, Result};
use notify::{
    event::{ModifyKind, RemoveKind},
    EventKind, RecommendedWatcher, RecursiveMode, Watcher,
};

use bulanci_core::assets::hash_path;

use super::handle::RawHandle;
use super::server::AssetServer;

/// Modder-facing reference file written by `build_typed_handles.py`.
/// Keys are pack-internal paths, values are 16-hex handle hashes. Used by
/// the legacy master dev-overlay watch to resolve dropped files.
const MODS_REFERENCE_NAME: &str = "mods.reference.json";

/// The pak profile file naming the active pak set + load order.
const PROFILE_NAME: &str = "profile.json";
/// Per-pak manifest filename (the project's "package.json").
const PAK_MANIFEST_NAME: &str = "pak.json";

// ---------------------------------------------------------------------
// profile + pak manifest schema (subset we read at runtime)
// ---------------------------------------------------------------------

#[derive(serde::Deserialize)]
struct Profile {
    #[serde(default)]
    paks: Vec<ProfileEntry>,
}

#[derive(serde::Deserialize)]
struct ProfileEntry {
    id: String,
    #[serde(default)]
    kind: String,
    /// Folder under the pak root (dev folder pak).
    #[serde(default)]
    path: Option<String>,
    /// Compiled binary pak relative to the pak root (dist pak v2). Takes
    /// precedence over `path` when present.
    #[serde(default)]
    file: Option<String>,
    #[serde(default = "default_true")]
    enabled: bool,
}

fn default_true() -> bool {
    true
}

#[derive(serde::Deserialize, Default)]
struct PakManifest {
    #[serde(default)]
    assets: HashMap<String, PakAsset>,
    #[serde(default)]
    scenes: HashMap<String, PakScene>,
    #[serde(default)]
    levels: HashMap<String, PakLevel>,
}

#[derive(serde::Deserialize)]
struct PakAsset {
    #[serde(default)]
    folder: Option<String>,
    #[serde(default)]
    slug: Option<String>,
    #[serde(default)]
    class: Option<String>,
    file: String,
    #[serde(default)]
    #[allow(dead_code)]
    overrides: bool,
}

#[derive(serde::Deserialize)]
struct PakScene {
    /// Legacy single-module scene (a Luau file built imperatively). Optional
    /// now that scenes can be declarative.
    #[serde(default)]
    file: Option<String>,
    /// Declarative scene document (`scenes/<name>.scene.json`).
    #[serde(default)]
    doc: Option<String>,
    /// Attached code-behind scripts for a declarative scene.
    #[serde(default)]
    scripts: Vec<String>,
    #[serde(default)]
    hash: Option<String>,
}

#[derive(serde::Deserialize)]
struct PakLevel {
    /// Declarative level document (`levels/<name>.level.json`).
    doc: String,
    /// Attached gamemode/code-behind scripts for this level.
    #[serde(default)]
    scripts: Vec<String>,
}

/// Dev-only merged catalog entry: who provides a hash and (if known) its
/// friendly name/class. Not shipped — dist resolves names -> hashes at
/// compile time so binaries stay slug-free.
#[derive(Clone, Debug)]
pub struct AssetMeta {
    pub folder: Option<String>,
    pub slug: Option<String>,
    pub class: Option<String>,
    pub pak: String,
}

// ---------------------------------------------------------------------
// per-pak file -> hash index
// ---------------------------------------------------------------------

/// Resolves a content file (relative, forward-slashed, to a pak folder)
/// to the asset hash it provides. Order: explicit manifest mapping ->
/// hash-named file (`<16hex>.ext`) -> `blake2b(path)` for path-keyed
/// content (scenes, levels, poems).
struct PakIndex {
    by_file: HashMap<String, u64>,
}

impl PakIndex {
    fn from_manifest(m: &PakManifest) -> Self {
        let mut by_file = HashMap::new();
        for (hash_hex, asset) in &m.assets {
            if let Ok(h) = u64::from_str_radix(hash_hex, 16) {
                by_file.insert(norm(&asset.file), h);
            }
        }
        // Scenes resolve to `blake2b("scenes/<name>")` (the same key the
        // engine looks up and `build_paks.py` compiles to), unless the
        // manifest pins an explicit hash. This keeps the dev folder mount,
        // the dist binary, and the runtime scene lookup all in agreement.
        // A declarative scene also pins its doc + scripts to the literal
        // path hashes the SceneManager reads them by.
        for (name, scene) in &m.scenes {
            if let Some(file) = &scene.file {
                let h = scene
                    .hash
                    .as_ref()
                    .and_then(|hh| u64::from_str_radix(hh, 16).ok())
                    .unwrap_or_else(|| hash_path(&format!("scenes/{name}")));
                by_file.insert(norm(file), h);
            }
            if let Some(doc) = &scene.doc {
                let nd = norm(doc);
                let h = hash_path(&nd);
                by_file.insert(nd, h);
            }
            for script in &scene.scripts {
                let ns = norm(script);
                let h = hash_path(&ns);
                by_file.insert(ns, h);
            }
        }
        // Levels mirror declarative scenes: the doc + any gamemode scripts are
        // pinned to the literal path hashes the engine reads them by
        // (`levels/<name>.level.json`), so the dev folder mount, the dist
        // binary, and the runtime `LevelSurface::load` lookup all agree.
        for level in m.levels.values() {
            let nd = norm(&level.doc);
            let h = hash_path(&nd);
            by_file.insert(nd, h);
            for script in &level.scripts {
                let ns = norm(script);
                let h = hash_path(&ns);
                by_file.insert(ns, h);
            }
        }
        Self { by_file }
    }

    fn resolve(&self, rel: &str) -> u64 {
        if let Some(&h) = self.by_file.get(rel) {
            return h;
        }
        if let Some(stem) = Path::new(rel).file_stem().and_then(|s| s.to_str()) {
            if stem.len() == 16 && stem.chars().all(|c| c.is_ascii_hexdigit()) {
                if let Ok(h) = u64::from_str_radix(stem, 16) {
                    return h;
                }
            }
        }
        hash_path(rel)
    }
}

fn norm(p: &str) -> String {
    p.replace('\\', "/")
}

// ---------------------------------------------------------------------
// Mods/paks bootstrap
// ---------------------------------------------------------------------

/// Result of setting up the disk loader. Owns the watchers (dropping
/// them stops events) and the dev name registry.
pub struct Mods {
    /// The legacy master dev-overlay root we're watching, if any.
    #[allow(dead_code)]
    pub root: Option<PathBuf>,
    /// The pak root (`paks/`) we mounted from, if found.
    #[allow(dead_code)]
    pub paks_root: Option<PathBuf>,
    /// Held alive for the process lifetime — dropping stops events.
    _watchers: Vec<RecommendedWatcher>,
    /// Override files installed across all mod paks + master overlay.
    pub initial_loaded: usize,
    /// Dev-only merged catalog (hash -> who/what). Empty in shipped
    /// builds where there's no pak folder on disk.
    pub name_registry: HashMap<u64, AssetMeta>,
}

impl Mods {
    /// Mount the active mod paks from `paks/profile.json` as persisted
    /// layers, arm a watcher per pak, and (in a dev checkout) keep the
    /// legacy master dev-overlay watch alive. Returns a `Mods` even when
    /// nothing's installed so callers keep a stable type.
    pub fn bootstrap(server: Arc<AssetServer>) -> Option<Self> {
        let mut watchers: Vec<RecommendedWatcher> = Vec::new();
        let mut initial_loaded = 0usize;
        let mut name_registry: HashMap<u64, AssetMeta> = HashMap::new();

        // ---- 1. Mounted mod paks (the layered pak system). ----
        let paks_root = pick_paks_root();
        if let Some(ref root) = paks_root {
            match load_profile(root) {
                Ok(profile) => {
                    for entry in &profile.paks {
                        if !entry.enabled || entry.kind == "master" {
                            continue; // master is the embedded base layer
                        }
                        // Dist: a compiled binary pak (pack v2) — mount it
                        // directly, no folder walk or watcher.
                        if let Some(file) = &entry.file {
                            let pak_path = root.join(file);
                            match std::fs::read(&pak_path) {
                                Ok(blob) => match server.vfs().mount_pack_bytes(&entry.id, &blob) {
                                    Ok(n) => {
                                        initial_loaded += n;
                                        eprintln!("[paks] mounted '{}' from {} ({} entries, binary)", entry.id, pak_path.display(), n);
                                    }
                                    Err(e) => eprintln!("[paks] mount binary '{}' failed: {e:#}", entry.id),
                                },
                                Err(e) => eprintln!("[paks] read '{}' failed: {e}", pak_path.display()),
                            }
                            continue;
                        }
                        // Dev: a folder pak — walk + mount + watch.
                        let rel = entry.path.clone().unwrap_or_else(|| entry.id.clone());
                        let pak_dir = root.join(&rel);
                        match mount_pak(&server, &entry.id, &pak_dir, &mut name_registry) {
                            Ok((loaded, watcher)) => {
                                initial_loaded += loaded;
                                eprintln!(
                                    "[paks] mounted '{}' from {} ({} entries)",
                                    entry.id, pak_dir.display(), loaded,
                                );
                                if let Some(w) = watcher {
                                    watchers.push(w);
                                } else {
                                    eprintln!("[paks] watcher disabled for '{}'", entry.id);
                                }
                            }
                            Err(e) => eprintln!("[paks] mount '{}' failed: {e:#}", entry.id),
                        }
                    }
                }
                Err(e) => eprintln!("[paks] profile load failed: {e:#}"),
            }
        }

        // ---- 2. Legacy master dev-overlay watch (live-edit the source). ----
        let root = pick_mods_root();
        if let Some(ref dev_root) = root {
            let refp = pick_reference_file(dev_root);
            let index = Arc::new(
                refp.as_ref()
                    .and_then(|p| ModsIndex::load(p).ok())
                    .unwrap_or_else(ModsIndex::empty),
            );
            if let Some(w) = spawn_master_watcher(dev_root.clone(), server.clone(), index) {
                watchers.push(w);
            }
        }

        eprintln!(
            "[paks] ready: {} mod entr(ies) installed, {} cataloged",
            initial_loaded,
            name_registry.len(),
        );

        Some(Mods {
            root,
            paks_root,
            _watchers: watchers,
            initial_loaded,
            name_registry,
        })
    }
}

/// Find the pak root containing `profile.json`. Precedence: env override
/// -> `paks/` next to the exe -> a dev checkout's `open_bulanci/paks/`.
fn pick_paks_root() -> Option<PathBuf> {
    if let Ok(p) = std::env::var("BULANCI_PAKS_DIR") {
        let p = PathBuf::from(p);
        if p.join(PROFILE_NAME).is_file() {
            return Some(p);
        }
    }
    if let Ok(exe) = std::env::current_exe() {
        if let Some(parent) = exe.parent() {
            let c = parent.join("paks");
            if c.join(PROFILE_NAME).is_file() {
                return Some(c);
            }
        }
    }
    for cand in [
        std::env::current_dir().ok().map(|c| c.join("open_bulanci").join("paks")),
        std::env::current_dir().ok().map(|c| c.join("paks")),
    ]
    .into_iter()
    .flatten()
    {
        if cand.join(PROFILE_NAME).is_file() {
            return Some(cand);
        }
    }
    None
}

fn load_profile(root: &Path) -> Result<Profile> {
    let raw = std::fs::read_to_string(root.join(PROFILE_NAME))
        .with_context(|| format!("reading {}", root.join(PROFILE_NAME).display()))?;
    serde_json::from_str(&raw).context("parsing profile.json")
}

fn load_pak_manifest(pak_dir: &Path) -> PakManifest {
    let p = pak_dir.join(PAK_MANIFEST_NAME);
    match std::fs::read_to_string(&p) {
        Ok(raw) => serde_json::from_str(&raw).unwrap_or_else(|e| {
            eprintln!("[paks] {}: manifest parse failed: {e}", p.display());
            PakManifest::default()
        }),
        Err(_) => PakManifest::default(),
    }
}

/// Walk a mod pak folder, mount every content file into the pak's layer,
/// populate the dev name registry, and arm a watcher for live edits.
fn mount_pak(
    server: &Arc<AssetServer>,
    pak_id: &str,
    pak_dir: &Path,
    name_registry: &mut HashMap<u64, AssetMeta>,
) -> Result<(usize, Option<RecommendedWatcher>)> {
    if !pak_dir.is_dir() {
        return Ok((0, None));
    }
    let manifest = load_pak_manifest(pak_dir);
    let index = Arc::new(PakIndex::from_manifest(&manifest));

    // Record friendly names for the dev catalog (override + new assets).
    for (hash_hex, asset) in &manifest.assets {
        if let Ok(h) = u64::from_str_radix(hash_hex, 16) {
            name_registry.insert(
                h,
                AssetMeta {
                    folder: asset.folder.clone(),
                    slug: asset.slug.clone(),
                    class: asset.class.clone(),
                    pak: pak_id.to_string(),
                },
            );
        }
    }

    let mut loaded = 0usize;
    for entry in walk_files(pak_dir) {
        if let Some(rel) = pretty_rel(&entry, pak_dir) {
            if rel == PAK_MANIFEST_NAME || rel.ends_with(".schema.json") {
                continue; // manifest/metadata, not content
            }
            match std::fs::read(&entry) {
                Ok(bytes) => {
                    let h = index.resolve(&rel);
                    server.mount_pak_asset(pak_id, RawHandle(h), bytes);
                    loaded += 1;
                }
                Err(e) => eprintln!("[paks] read {} failed: {e}", entry.display()),
            }
        }
    }

    let watcher = spawn_pak_watcher(pak_id.to_string(), pak_dir.to_path_buf(), server.clone(), index);
    Ok((loaded, watcher))
}

fn spawn_pak_watcher(
    pak_id: String,
    root: PathBuf,
    server: Arc<AssetServer>,
    index: Arc<PakIndex>,
) -> Option<RecommendedWatcher> {
    let root_cb = root.clone();
    let mut watcher = notify::recommended_watcher(move |res: notify::Result<notify::Event>| {
        let ev = match res {
            Ok(e) => e,
            Err(e) => {
                eprintln!("[paks] watcher error: {e}");
                return;
            }
        };
        if !is_content_event(&ev.kind) {
            return;
        }
        for path in ev.paths {
            let rel = match pretty_rel(&path, &root_cb) {
                Some(r) => r,
                None => continue,
            };
            if rel == PAK_MANIFEST_NAME {
                continue;
            }
            let h = index.resolve(&rel);
            match std::fs::metadata(&path) {
                Ok(md) if md.is_file() => match std::fs::read(&path) {
                    Ok(bytes) => {
                        eprintln!("[paks] {} reload {} -> 0x{h:016x} ({} bytes)", pak_id, rel, bytes.len());
                        server.mount_pak_asset(&pak_id, RawHandle(h), bytes);
                    }
                    Err(e) => eprintln!("[paks] read {} failed: {e}", path.display()),
                },
                Ok(_) => {}
                Err(_) => {
                    eprintln!("[paks] {} removed {} -> 0x{h:016x}", pak_id, rel);
                    server.unmount_pak_asset(&pak_id, RawHandle(h));
                }
            }
        }
    })
    .ok()?;
    watcher.watch(&root, RecursiveMode::Recursive).ok()?;
    Some(watcher)
}

fn is_content_event(kind: &EventKind) -> bool {
    matches!(
        kind,
        EventKind::Create(_)
            | EventKind::Modify(ModifyKind::Data(_))
            | EventKind::Modify(ModifyKind::Any)
            | EventKind::Modify(ModifyKind::Name(_))
    ) || matches!(kind, EventKind::Remove(RemoveKind::File) | EventKind::Remove(RemoveKind::Any))
}

// ---------------------------------------------------------------------
// Legacy master dev-overlay watch (live-edit the master source tree)
// ---------------------------------------------------------------------

#[derive(serde::Deserialize)]
struct ModsReferenceFile {
    paths: HashMap<String, String>,
}

struct ModsIndex {
    by_path: HashMap<String, u64>,
}

impl ModsIndex {
    fn load(reference_path: &Path) -> Result<Self> {
        let raw = std::fs::read_to_string(reference_path)
            .with_context(|| format!("reading {}", reference_path.display()))?;
        let parsed: ModsReferenceFile = serde_json::from_str(&raw)
            .with_context(|| format!("parsing {}", reference_path.display()))?;
        let mut by_path = HashMap::with_capacity(parsed.paths.len());
        for (path, hash_hex) in parsed.paths {
            if let Ok(h) = u64::from_str_radix(&hash_hex, 16) {
                by_path.insert(path, h);
            }
        }
        Ok(Self { by_path })
    }

    fn empty() -> Self {
        Self { by_path: HashMap::new() }
    }

    fn resolve(&self, rel_path: &str) -> u64 {
        if let Some(&h) = self.by_path.get(rel_path) {
            return h;
        }
        if let Some(stem) = Path::new(rel_path).file_stem().and_then(|s| s.to_str()) {
            if stem.len() == 16 && stem.chars().all(|c| c.is_ascii_hexdigit()) {
                if let Ok(h) = u64::from_str_radix(stem, 16) {
                    return h;
                }
            }
        }
        hash_path(rel_path)
    }
}

/// Pick the master dev-overlay root: `open_bulanci/assets/` in a dev
/// checkout (identified by `manifest.slim.json`). Returns `None` in
/// shipped layouts, where the master is purely the embedded pack.
fn pick_mods_root() -> Option<PathBuf> {
    let dev_assets = std::env::current_dir().ok().map(|cwd| cwd.join("open_bulanci").join("assets"));
    if let Some(p) = dev_assets {
        if p.is_dir() && p.join("manifest.slim.json").is_file() {
            return Some(p);
        }
    }
    let here = std::env::current_dir().ok().map(|cwd| cwd.join("assets"));
    if let Some(p) = here {
        if p.is_dir() && p.join("manifest.slim.json").is_file() {
            return Some(p);
        }
    }
    None
}

fn pick_reference_file(mods_root: &Path) -> Option<PathBuf> {
    let candidates = [
        mods_root.join(MODS_REFERENCE_NAME),
        mods_root
            .parent()
            .map(|p| p.join(MODS_REFERENCE_NAME))
            .unwrap_or_else(|| PathBuf::from(MODS_REFERENCE_NAME)),
    ];
    candidates.into_iter().find(|p| p.is_file())
}

/// Watch the master source tree and push edits into the *live overlay*
/// (not a pak layer) so editing source files hot-reloads in dev. These
/// are ephemeral previews of master edits; persisting them is the
/// editor's "repack master" path.
fn spawn_master_watcher(
    root: PathBuf,
    server: Arc<AssetServer>,
    index: Arc<ModsIndex>,
) -> Option<RecommendedWatcher> {
    let root_cb = root.clone();
    let mut watcher = notify::recommended_watcher(move |res: notify::Result<notify::Event>| {
        let ev = match res {
            Ok(e) => e,
            Err(e) => {
                eprintln!("[master-watch] error: {e}");
                return;
            }
        };
        if !is_content_event(&ev.kind) {
            return;
        }
        for path in ev.paths {
            let rel = match pretty_rel(&path, &root_cb) {
                Some(r) => r,
                None => continue,
            };
            if rel == MODS_REFERENCE_NAME {
                continue;
            }
            let h = index.resolve(&rel);
            match std::fs::metadata(&path) {
                Ok(md) if md.is_file() => {
                    if let Ok(bytes) = std::fs::read(&path) {
                        server.install_overlay(RawHandle(h), bytes);
                    }
                }
                Ok(_) => {}
                Err(_) => server.remove_overlay(RawHandle(h)),
            }
        }
    })
    .ok()?;
    watcher.watch(&root, RecursiveMode::Recursive).ok()?;
    Some(watcher)
}

// ---------------------------------------------------------------------
// shared fs helpers
// ---------------------------------------------------------------------

fn walk_files(root: &Path) -> Vec<PathBuf> {
    let mut out = Vec::new();
    fn go(dir: &Path, out: &mut Vec<PathBuf>) {
        let read = match std::fs::read_dir(dir) {
            Ok(r) => r,
            Err(_) => return,
        };
        for ent in read.flatten() {
            let p = ent.path();
            if p.is_dir() {
                go(&p, out);
            } else if p.is_file() {
                out.push(p);
            }
        }
    }
    go(root, &mut out);
    out
}

fn pretty_rel(abs: &Path, root: &Path) -> Option<String> {
    let rel = abs.strip_prefix(root).ok()?;
    Some(rel.to_string_lossy().replace('\\', "/"))
}
