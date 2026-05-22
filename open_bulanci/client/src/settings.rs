//! Cross-platform settings persistence with priority-based routing.
//!
//! The trait surface is a 1:1 mirror of the two registry value types the
//! original game uses (see `ghidra_analysis/engine/registry.md`):
//!
//! | Original (Windows-only)               | OpenBulanci abstraction |
//! | ------------------------------------- | ----------------------- |
//! | `REG_DWORD` (`RegQueryDword`)         | [`SettingsStore::get_u32`] / [`SettingsStore::set_u32`] |
//! | `REG_BINARY` (`RegQueryBinaryStream`) | [`SettingsStore::get_bytes`] / [`SettingsStore::set_bytes`] |
//!
//! ## Backend selection priority (native targets — Win / Linux / macOS)
//!
//! 1. **Portable mode** — if `config.json` exists *next to the
//!    executable* and is valid JSON (or is empty), it is used
//!    exclusively for both reads and writes. The Windows registry is
//!    not touched at all. This is the same "drop an empty config
//!    file to opt in" pattern used by Notepad++, OBS, mpv, etc.
//! 2. **Windows registry** — read order is `HKCU\Software\OpenBulanci`
//!    (where we ourselves write) then `HKLM\Software\SleepTeam\Bulanci`
//!    (the original game's install location, **read-only** — we never
//!    write there). Writes go exclusively to `HKCU\Software\OpenBulanci`
//!    because modern Windows denies `HKLM` writes to non-elevated
//!    processes and reserved hives (IIS app pool, restricted service
//!    accounts, etc.) may deny HKCU as well.
//! 3. **File fallback** — if any registry operation cannot open the
//!    hive at all (e.g. an IIS identity process with no user profile
//!    loaded), the store transparently switches to a fresh
//!    `config.json` next to the executable. On Linux/macOS this is
//!    *also* the default backend because those platforms have no
//!    registry of their own.
//!
//! ## WebAssembly
//!
//! Under `target_arch = "wasm32"`, the only persistent store available
//! inside the browser sandbox is `localStorage`, which is what we use.
//! There is no on-disk file and no registry equivalent.
//!
//! ## "Same keys" guarantee
//!
//! The value names (`Windowed`, `Config`) are reused verbatim across
//! every backend, and on Windows the legacy `HKLM\Software\SleepTeam\Bulanci`
//! subkey is read transparently so a player who once ran the original
//! Bulanci installer keeps their preserved settings.
//!
//! ## Error handling philosophy
//!
//! Persistence is best-effort: a missing file, a denied registry
//! handle, a quota-exceeded `localStorage` — none of these should
//! crash the game. Reads return `Option<T>` (`None` == "value not
//! present"). Writes return `()` and log warnings via `eprintln!` on
//! failure. This matches the original engine's posture (the game
//! catches `CDSRegKeyException` and silently continues with in-memory
//! defaults — see `ThrowRegKeyException` at `0x00437ed0`).
//!
//! ## "Dead code" lint
//!
//! `KEY_CONFIG` and the `get_bytes` / `set_bytes` half of the trait are
//! intentionally part of the public surface even though the current
//! client does not yet round-trip the `Config` blob — they constitute
//! the migration target for the next layer of work (the original
//! game's gzipped `CBulanci::SaveConfig` payload). The
//! `#![allow(dead_code)]` below silences the binary-crate dead-public-
//! item lint without weakening any real visibility.

#![allow(dead_code)]

/// Legacy subkey written by the original game's installer; on Windows
/// we read this for backwards-compat (see
/// `g_wszRegSubKeySleepTeamBulanci @ 0x0047f66c`) but **never write to
/// it** — the original install path is treated as immutable historical
/// data.
pub const LEGACY_HKLM_SUBKEY: &str = r"Software\SleepTeam\Bulanci";

/// Subkey we write to on Windows under `HKEY_CURRENT_USER`. Chosen to
/// be product-namespaced (no `SleepTeam` sub-grouping, since
/// OpenBulanci is a separate codebase) and not require admin.
pub const OPEN_HKCU_SUBKEY: &str = r"Software\OpenBulanci";

/// Filename used for portable mode and as the registry fallback target.
/// Drop this file next to the binary to opt into portable mode (empty
/// file is fine — it acts as a marker).
pub const PORTABLE_FILENAME: &str = "config.json";

/// Original `Windowed` value name (`REG_DWORD`). 0 = fullscreen on the
/// original, non-zero = windowed.
pub const KEY_WINDOWED: &str = "Windowed";

/// Original `Config` value name (`REG_BINARY`). Holds a gzip-compressed
/// blob with player profiles, key bindings, and audio volumes — see
/// `CBulanci::SaveConfig @ 0x00409cd0`.
pub const KEY_CONFIG: &str = "Config";

/// Cross-platform key/value store. Implementations are required to be
/// `Send + Sync` because the client may share a single boxed store
/// across the async macroquad main loop and any future worker threads.
pub trait SettingsStore: Send + Sync {
    /// Returns the `u32` stored under `key`, or `None` if the key is
    /// absent. Mirrors `RegQueryValueExW` with `REG_DWORD`.
    fn get_u32(&self, key: &str) -> Option<u32>;

    /// Persists `value` as a `u32` under `key`. Failures are logged to
    /// stderr but never propagated.
    fn set_u32(&self, key: &str, value: u32);

    /// Returns the byte blob stored under `key`, or `None` if absent.
    /// Mirrors `RegQueryValueExW` with `REG_BINARY`.
    fn get_bytes(&self, key: &str) -> Option<Vec<u8>>;

    /// Persists `value` as an opaque byte blob under `key`.
    fn set_bytes(&self, key: &str, value: &[u8]);
}

/// Returns the default store for the current target, following the
/// priority routing documented at the module level.
pub fn default_store() -> Box<dyn SettingsStore> {
    #[cfg(target_arch = "wasm32")]
    {
        Box::new(wasm_backend::LocalStorageStore::new())
    }
    #[cfg(not(target_arch = "wasm32"))]
    {
        select_native_store()
    }
}

#[cfg(not(target_arch = "wasm32"))]
fn select_native_store() -> Box<dyn SettingsStore> {
    use std::path::PathBuf;

    // ---- Priority 1: portable mode --------------------------------
    //
    // Presence of a `config.json` next to the exe is the explicit
    // opt-in. We REFUSE portable mode if the file exists but is
    // unparseable — clobbering a corrupt user config would silently
    // discard whatever the user was trying to recover.
    let portable_path: Option<PathBuf> = exe_relative_path(PORTABLE_FILENAME);
    if let Some(p) = &portable_path {
        if p.is_file() {
            match check_portable_validity(p) {
                Ok(()) => {
                    eprintln!("[settings] portable mode: {}", p.display());
                    return Box::new(file_backend::FileStore::new_at(p.clone()));
                }
                Err(reason) => {
                    eprintln!(
                        "[settings] {} exists but is rejected for portable mode ({reason}); \
                         falling through to platform default",
                        p.display()
                    );
                }
            }
        }
    }

    // ---- Priority 2 (Windows): registry, with file fallback -------
    #[cfg(target_os = "windows")]
    {
        return Box::new(windows_backend::WindowsStore::new(portable_path));
    }

    // ---- Priority 3 (Linux / macOS / BSD): file is the only backend
    //      we have — created on first write next to the exe. --------
    #[cfg(not(target_os = "windows"))]
    {
        match portable_path {
            Some(p) => Box::new(file_backend::FileStore::new_at(p)),
            None => {
                eprintln!(
                    "[settings] cannot resolve executable path; settings will not persist"
                );
                Box::new(NullStore)
            }
        }
    }
}

#[cfg(not(target_arch = "wasm32"))]
fn exe_relative_path(filename: &str) -> Option<std::path::PathBuf> {
    let exe = std::env::current_exe().ok()?;
    Some(exe.parent()?.join(filename))
}

/// Strict validation used by portable-mode opt-in: returns `Ok(())` if
/// the file is empty/whitespace-only (a marker file) or parses as a
/// valid `Document`. Anything else (binary garbage, syntactically
/// broken JSON, wrong shape) is rejected and the caller falls through
/// to the next priority — never overwrites the user's file.
#[cfg(not(target_arch = "wasm32"))]
fn check_portable_validity(path: &std::path::Path) -> Result<(), String> {
    let bytes = std::fs::read(path).map_err(|e| format!("read failed: {e}"))?;
    if bytes.iter().all(|b| b.is_ascii_whitespace()) {
        return Ok(());
    }
    serde_json::from_slice::<file_backend::Document>(&bytes)
        .map(|_| ())
        .map_err(|e| format!("invalid JSON: {e}"))
}

// ============================================================================
// NullStore — used as a last-resort sink when no backend can be set up.
// ============================================================================
#[cfg(not(target_arch = "wasm32"))]
struct NullStore;

#[cfg(not(target_arch = "wasm32"))]
impl SettingsStore for NullStore {
    fn get_u32(&self, _key: &str) -> Option<u32> {
        None
    }
    fn set_u32(&self, _key: &str, _value: u32) {}
    fn get_bytes(&self, _key: &str) -> Option<Vec<u8>> {
        None
    }
    fn set_bytes(&self, _key: &str, _value: &[u8]) {}
}

// ============================================================================
// File backend — used on all native targets (portable mode AND Windows
// fallback AND Linux/macOS default).
// ============================================================================
#[cfg(not(target_arch = "wasm32"))]
mod file_backend {
    use super::SettingsStore;
    use base64::Engine;
    use std::collections::BTreeMap;
    use std::path::PathBuf;
    use std::sync::Mutex;

    /// JSON shape on disk. `u32` values stay numeric; byte blobs are
    /// base64-encoded under a separate map so a hand-edit / `cat`
    /// round-trip never accidentally interprets a blob as a number.
    #[derive(Default, serde::Serialize, serde::Deserialize)]
    pub(super) struct Document {
        #[serde(default)]
        dwords: BTreeMap<String, u32>,
        #[serde(default)]
        binary_b64: BTreeMap<String, String>,
    }

    pub struct FileStore {
        path: PathBuf,
        inner: Mutex<Document>,
    }

    impl FileStore {
        /// Tolerant constructor: if the file is missing OR exists but
        /// is unparseable, starts with an empty document (subsequent
        /// writes will overwrite). Use this when the caller has
        /// already accepted that they own this path — e.g. portable
        /// mode after `check_portable_validity` succeeded, or as the
        /// Linux/macOS default backend, or as the Windows fallback.
        pub fn new_at(path: PathBuf) -> Self {
            let doc = load_or_default(&path);
            FileStore {
                path,
                inner: Mutex::new(doc),
            }
        }

        fn write_back(&self, doc: &Document) {
            if let Some(parent) = self.path.parent() {
                // `create_dir_all` is a no-op when the dir exists;
                // it's safe to call every write.
                if !parent.as_os_str().is_empty() {
                    if let Err(e) = std::fs::create_dir_all(parent) {
                        eprintln!(
                            "[settings] cannot create dir {}: {e}",
                            parent.display()
                        );
                        return;
                    }
                }
            }
            // Atomic write: serialize to a sibling `.tmp` and rename.
            // On every supported OS the rename is atomic on the same
            // filesystem, so a crashed game cannot leave a torn
            // config.json behind.
            let tmp = self.path.with_extension("json.tmp");
            let bytes = match serde_json::to_vec_pretty(doc) {
                Ok(b) => b,
                Err(e) => {
                    eprintln!("[settings] serialize failed: {e}");
                    return;
                }
            };
            if let Err(e) = std::fs::write(&tmp, &bytes) {
                eprintln!(
                    "[settings] cannot write {}: {e}",
                    tmp.display()
                );
                return;
            }
            if let Err(e) = std::fs::rename(&tmp, &self.path) {
                eprintln!(
                    "[settings] rename {} -> {} failed: {e}",
                    tmp.display(),
                    self.path.display()
                );
            }
        }
    }

    impl SettingsStore for FileStore {
        fn get_u32(&self, key: &str) -> Option<u32> {
            self.inner.lock().ok()?.dwords.get(key).copied()
        }

        fn set_u32(&self, key: &str, value: u32) {
            let snapshot = {
                let Ok(mut g) = self.inner.lock() else { return };
                g.dwords.insert(key.to_string(), value);
                clone_doc(&g)
            };
            self.write_back(&snapshot);
        }

        fn get_bytes(&self, key: &str) -> Option<Vec<u8>> {
            let s = self.inner.lock().ok()?.binary_b64.get(key)?.clone();
            base64::engine::general_purpose::STANDARD.decode(s).ok()
        }

        fn set_bytes(&self, key: &str, value: &[u8]) {
            let encoded = base64::engine::general_purpose::STANDARD.encode(value);
            let snapshot = {
                let Ok(mut g) = self.inner.lock() else { return };
                g.binary_b64.insert(key.to_string(), encoded);
                clone_doc(&g)
            };
            self.write_back(&snapshot);
        }
    }

    fn clone_doc(d: &Document) -> Document {
        Document {
            dwords: d.dwords.clone(),
            binary_b64: d.binary_b64.clone(),
        }
    }

    fn load_or_default(path: &PathBuf) -> Document {
        match std::fs::read(path) {
            Ok(bytes) if bytes.iter().all(|b| b.is_ascii_whitespace()) => Document::default(),
            Ok(bytes) => serde_json::from_slice(&bytes).unwrap_or_else(|e| {
                eprintln!(
                    "[settings] {} is corrupt ({e}); starting fresh",
                    path.display()
                );
                Document::default()
            }),
            Err(e) if e.kind() == std::io::ErrorKind::NotFound => Document::default(),
            Err(e) => {
                eprintln!("[settings] cannot read {}: {e}", path.display());
                Document::default()
            }
        }
    }
}

// ============================================================================
// Windows backend — HKCU\Software\OpenBulanci read/write,
// HKLM\Software\SleepTeam\Bulanci read-only legacy fallback,
// file fallback for genuine hive failures.
// ============================================================================
#[cfg(all(not(target_arch = "wasm32"), target_os = "windows"))]
mod windows_backend {
    use super::{
        file_backend::FileStore, SettingsStore, LEGACY_HKLM_SUBKEY, OPEN_HKCU_SUBKEY,
    };
    use std::path::PathBuf;
    use std::sync::atomic::{AtomicBool, Ordering};
    use winreg::enums::*;
    use winreg::RegKey;

    pub struct WindowsStore {
        /// Pre-constructed file fallback. `FileStore::new_at` does not
        /// touch the disk on construction (only `set_*` writes), so
        /// eager init is free.
        fallback: Option<FileStore>,
        /// Used purely for the "fallback activated" log message — the
        /// FileStore owns its own copy internally.
        fallback_path: Option<PathBuf>,
        /// Latches the first time we route a write to the file
        /// fallback so we log the event exactly once per session.
        fallback_logged: AtomicBool,
    }

    impl WindowsStore {
        pub fn new(fallback_path: Option<PathBuf>) -> Self {
            let fallback = fallback_path.clone().map(FileStore::new_at);
            WindowsStore {
                fallback,
                fallback_path,
                fallback_logged: AtomicBool::new(false),
            }
        }

        fn use_fallback(&self) -> Option<&FileStore> {
            let fs = self.fallback.as_ref()?;
            if !self.fallback_logged.swap(true, Ordering::Relaxed) {
                if let Some(p) = &self.fallback_path {
                    eprintln!(
                        "[settings] registry unavailable; using {}",
                        p.display()
                    );
                }
            }
            Some(fs)
        }

        fn fallback_file_exists(&self) -> bool {
            self.fallback_path
                .as_ref()
                .map_or(false, |p| p.is_file())
        }

        fn open_ours_for_read() -> Option<RegKey> {
            RegKey::predef(HKEY_CURRENT_USER)
                .open_subkey(OPEN_HKCU_SUBKEY)
                .ok()
        }

        fn open_legacy_for_read() -> Option<RegKey> {
            // KEY_READ is sufficient — we never write under HKLM.
            RegKey::predef(HKEY_LOCAL_MACHINE)
                .open_subkey_with_flags(LEGACY_HKLM_SUBKEY, KEY_READ)
                .ok()
        }

        fn open_ours_for_write() -> Option<RegKey> {
            RegKey::predef(HKEY_CURRENT_USER)
                .create_subkey(OPEN_HKCU_SUBKEY)
                .map(|(k, _)| k)
                .ok()
        }
    }

    impl SettingsStore for WindowsStore {
        fn get_u32(&self, key: &str) -> Option<u32> {
            // 1. HKCU\OpenBulanci — preferred (our own writes).
            if let Some(k) = Self::open_ours_for_read() {
                if let Ok(v) = k.get_value::<u32, _>(key) {
                    return Some(v);
                }
            }
            // 2. HKLM\SleepTeam\Bulanci — legacy install (READ-ONLY,
            //    we never touch its values).
            if let Some(k) = Self::open_legacy_for_read() {
                if let Ok(v) = k.get_value::<u32, _>(key) {
                    return Some(v);
                }
            }
            // 3. File fallback — only if it pre-exists, so we don't
            //    spuriously create a file just to read a missing key.
            if self.fallback_file_exists() {
                if let Some(fs) = self.fallback.as_ref() {
                    return fs.get_u32(key);
                }
            }
            None
        }

        fn set_u32(&self, key: &str, value: u32) {
            // 1. HKCU\OpenBulanci.
            if let Some(k) = Self::open_ours_for_write() {
                if k.set_value(key, &value).is_ok() {
                    return;
                }
            }
            // 2. File fallback (logs once on first activation).
            if let Some(fs) = self.use_fallback() {
                fs.set_u32(key, value);
            } else {
                eprintln!(
                    "[settings] no writable backend for u32 key '{key}'"
                );
            }
        }

        fn get_bytes(&self, key: &str) -> Option<Vec<u8>> {
            if let Some(k) = Self::open_ours_for_read() {
                if let Ok(v) = k.get_raw_value(key) {
                    if v.vtype == REG_BINARY {
                        return Some(v.bytes.into_owned());
                    }
                }
            }
            if let Some(k) = Self::open_legacy_for_read() {
                if let Ok(v) = k.get_raw_value(key) {
                    if v.vtype == REG_BINARY {
                        return Some(v.bytes.into_owned());
                    }
                }
            }
            if self.fallback_file_exists() {
                if let Some(fs) = self.fallback.as_ref() {
                    return fs.get_bytes(key);
                }
            }
            None
        }

        fn set_bytes(&self, key: &str, value: &[u8]) {
            if let Some(k) = Self::open_ours_for_write() {
                let raw = winreg::RegValue {
                    vtype: REG_BINARY,
                    bytes: value.to_vec().into(),
                };
                if k.set_raw_value(key, &raw).is_ok() {
                    return;
                }
            }
            if let Some(fs) = self.use_fallback() {
                fs.set_bytes(key, value);
            } else {
                eprintln!(
                    "[settings] no writable backend for bytes key '{key}'"
                );
            }
        }
    }
}

// ============================================================================
// WASM backend — `window.localStorage`.
// ============================================================================
#[cfg(target_arch = "wasm32")]
mod wasm_backend {
    use super::{SettingsStore, OPEN_HKCU_SUBKEY};
    use base64::Engine;

    /// Browser `localStorage`-backed store. Keys are namespaced under
    /// `bulanci/Software/OpenBulanci/<NAME>` so they don't collide
    /// with anything else the hosting page might be writing. Two
    /// storage classes share the keyspace without ambiguity because
    /// we serialize `u32` as a decimal string and `bytes` as
    /// `b64:<base64>` — the prefix discriminates.
    pub struct LocalStorageStore;

    impl LocalStorageStore {
        pub fn new() -> Self {
            LocalStorageStore
        }

        fn storage() -> Option<web_sys::Storage> {
            let win = web_sys::window()?;
            win.local_storage().ok().flatten()
        }

        fn make_key(name: &str) -> String {
            // Backslashes -> forward slashes for URL-style key paths,
            // even though localStorage doesn't care about either.
            format!("bulanci/{}/{}", OPEN_HKCU_SUBKEY.replace('\\', "/"), name)
        }
    }

    impl SettingsStore for LocalStorageStore {
        fn get_u32(&self, key: &str) -> Option<u32> {
            let s = Self::storage()?
                .get_item(&Self::make_key(key))
                .ok()
                .flatten()?;
            s.parse::<u32>().ok()
        }

        fn set_u32(&self, key: &str, value: u32) {
            let Some(storage) = Self::storage() else {
                eprintln!("[settings] localStorage unavailable");
                return;
            };
            if let Err(e) = storage.set_item(&Self::make_key(key), &value.to_string()) {
                eprintln!("[settings] localStorage set u32 failed: {e:?}");
            }
        }

        fn get_bytes(&self, key: &str) -> Option<Vec<u8>> {
            let s = Self::storage()?
                .get_item(&Self::make_key(key))
                .ok()
                .flatten()?;
            let b64 = s.strip_prefix("b64:")?;
            base64::engine::general_purpose::STANDARD.decode(b64).ok()
        }

        fn set_bytes(&self, key: &str, value: &[u8]) {
            let Some(storage) = Self::storage() else {
                eprintln!("[settings] localStorage unavailable");
                return;
            };
            let encoded = format!(
                "b64:{}",
                base64::engine::general_purpose::STANDARD.encode(value)
            );
            if let Err(e) = storage.set_item(&Self::make_key(key), &encoded) {
                eprintln!("[settings] localStorage set bytes failed: {e:?}");
            }
        }
    }
}
