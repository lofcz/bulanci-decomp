//! Framing layer for the original game's `Config` REG_BINARY payload.
//!
//! In the original `bulanci.exe`, the player profiles + keyboard bindings
//! + audio volumes are persisted under a single registry value
//! (`HKLM\Software\SleepTeam\Bulanci\Config`, type `REG_BINARY`) through
//! this pipeline (see `CBulanci::SaveConfig @ 0x00409cd0` and
//! `CBulanci::LoadConfig @ 0x0040a440`):
//!
//! ```text
//! Writer:  serialized payload  ─► CDSGZipStream ─► CDSEasyMemStream ─► REG_BINARY
//! Reader:  REG_BINARY ─► CDSEasyMemStream ─► CDSGZipStream ─► serialized payload
//! ```
//!
//! `CDSEasyMemStream` is just a thin in-memory buffer adapter — it does
//! not add any framing of its own — so the bytes that actually land in
//! the registry are exactly **standard RFC 1952 gzip**. This module is
//! the wire-compatible Rust equivalent: it gzips a payload before
//! calling [`SettingsStore::set_bytes`] under [`KEY_CONFIG`], and
//! gunzips the bytes returned by [`SettingsStore::get_bytes`].
//!
//! ## Why this lives one layer above `settings.rs`
//!
//! The K/V trait in `settings.rs` is intentionally schema-agnostic — it
//! stores raw bytes regardless of what those bytes mean. Only one of
//! the two original registry values (`Config`) is gzipped; the other
//! (`Windowed`) is a plain `REG_DWORD`. Pushing gzip into `set_bytes`
//! would either conflate transport with payload or require a second
//! "raw bytes" parallel API. Instead, this small helper owns the
//! framing decision and lets every other byte-shaped slot stay raw.
//!
//! ## Schema-agnostic
//!
//! These functions deal in `&[u8]` and `Vec<u8>` — they do not know
//! what's inside. The eventual `OpenConfig` struct (player names, key
//! bindings, volumes) will be serialized via `postcard` or `bincode`
//! by a higher layer, then handed to [`save`] as bytes. Likewise on
//! read: [`load`] returns the decompressed bytes, and the schema-aware
//! caller deserializes. This preserves a clean "transport vs. content"
//! split and means a future replacement of the inner serializer (for
//! versioning, migration, etc.) is a one-line change at the call site.
//!
//! ## Unused for now
//!
//! OpenBulanci does not yet define the `OpenConfig` struct (player
//! profiles, audio volumes, keybinds) — that's a separate piece of
//! work tracked alongside the StartGame1 / options screens. The
//! module is wired in now to lock in the layering and give the future
//! call sites a stable target. `#![allow(dead_code)]` silences the
//! binary-crate dead-public-item lint until those callers exist.

#![allow(dead_code)]

use flate2::read::GzDecoder;
use flate2::write::GzEncoder;
use flate2::Compression;
use std::io::{Read, Write};

use crate::settings::{SettingsStore, KEY_CONFIG};

/// Gzips `payload` using the default compression level (level 6 — the
/// same default zlib's `gzopen` uses and what the original game's
/// `CDSGZipStream` almost certainly produces at runtime).
///
/// Returns a freshly-allocated `Vec<u8>` containing standard RFC 1952
/// gzip bytes. Never panics: `Vec<u8>` is an infallible writer.
pub fn compress(payload: &[u8]) -> Vec<u8> {
    let mut encoder = GzEncoder::new(
        Vec::with_capacity(payload.len() / 2 + 32),
        Compression::default(),
    );
    encoder
        .write_all(payload)
        .expect("Vec<u8>::write_all is infallible");
    encoder
        .finish()
        .expect("GzEncoder::finish on Vec<u8> is infallible")
}

/// Gunzips `compressed`. Returns `None` if the bytes aren't a valid
/// gzip stream — for example, if someone hand-edited the registry
/// blob, or if a future original-game version switched encodings.
/// Callers should treat `None` as "config absent" and fall back to
/// in-memory defaults, matching the original's posture of silently
/// swallowing `CDSRegKeyException` (see `ThrowRegKeyException @
/// 0x00437ed0`).
pub fn decompress(compressed: &[u8]) -> Option<Vec<u8>> {
    let mut decoder = GzDecoder::new(compressed);
    let mut out = Vec::new();
    decoder.read_to_end(&mut out).ok()?;
    Some(out)
}

/// Gzips `payload` and writes it to `store` under [`KEY_CONFIG`]. This
/// is the OpenBulanci equivalent of the call sequence at the bottom
/// of `CBulanci::SaveConfig`:
///
/// ```text
///    CDSGZipStream::Write(payload, len)
///    RegSetValueExW(.., L"Config", REG_BINARY, compressed, compressed_len)
/// ```
pub fn save(store: &dyn SettingsStore, payload: &[u8]) {
    let compressed = compress(payload);
    store.set_bytes(KEY_CONFIG, &compressed);
}

/// Reads the bytes stored under [`KEY_CONFIG`] and gunzips them.
/// Returns `None` if the key is absent OR the bytes are not a valid
/// gzip stream. Mirrors the read side of `CBulanci::LoadConfig`.
pub fn load(store: &dyn SettingsStore) -> Option<Vec<u8>> {
    let compressed = store.get_bytes(KEY_CONFIG)?;
    decompress(&compressed)
}

// ============================================================================
// Tests
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn round_trips_arbitrary_bytes() {
        // A mix of repeating and random bytes so we exercise both the
        // compressible and incompressible paths through deflate.
        let mut payload = Vec::with_capacity(2048);
        for i in 0..2048u32 {
            payload.push((i.wrapping_mul(2654435761) >> 24) as u8);
        }
        for _ in 0..256 {
            payload.push(b'A');
        }

        let compressed = compress(&payload);
        // Sanity: gzip magic.
        assert_eq!(&compressed[..2], &[0x1f, 0x8b]);
        // Empty/very small payloads can grow after gzip framing, but
        // 2 KiB with a 256-byte run definitely shrinks.
        assert!(
            compressed.len() < payload.len(),
            "gzip didn't shrink a redundant 2.3 KiB payload"
        );

        let recovered = decompress(&compressed).expect("valid gzip");
        assert_eq!(recovered, payload);
    }

    #[test]
    fn round_trips_empty_payload() {
        let compressed = compress(&[]);
        assert_eq!(&compressed[..2], &[0x1f, 0x8b]);
        assert_eq!(decompress(&compressed).as_deref(), Some(&[][..]));
    }

    #[test]
    fn rejects_invalid_gzip() {
        assert_eq!(decompress(b"not a gzip stream at all"), None);
        assert_eq!(decompress(&[0x1f, 0x8b, 0x00]), None); // truncated header
        assert_eq!(decompress(&[]), None);
    }

    #[test]
    fn save_then_load_via_inmem_store() {
        // Tiny in-memory `SettingsStore` so we can exercise the full
        // pipeline without touching the registry/file system.
        use std::collections::HashMap;
        use std::sync::Mutex;

        struct InMemStore(Mutex<HashMap<String, Vec<u8>>>);
        impl SettingsStore for InMemStore {
            fn get_u32(&self, _: &str) -> Option<u32> {
                None
            }
            fn set_u32(&self, _: &str, _: u32) {}
            fn get_bytes(&self, k: &str) -> Option<Vec<u8>> {
                self.0.lock().unwrap().get(k).cloned()
            }
            fn set_bytes(&self, k: &str, v: &[u8]) {
                self.0.lock().unwrap().insert(k.to_string(), v.to_vec());
            }
        }

        let store = InMemStore(Mutex::new(HashMap::new()));
        let original: &[u8] = b"player-bindings:\x00\x01\x02\x03 volumes: 70/50";
        save(&store, original);

        // What landed in storage must be gzip-framed, not the raw bytes.
        let stored = store.get_bytes(KEY_CONFIG).unwrap();
        assert_eq!(&stored[..2], &[0x1f, 0x8b]);
        assert_ne!(stored, original);

        // And the round-trip recovers the exact original bytes.
        let recovered = load(&store).expect("load");
        assert_eq!(recovered, original);
    }
}
