//! Cursor composition: which generic [`Atlas`]es act as the menu
//! cursor's idle-twitch tracks, and in what order.
//!
//! The original engine's `CGunMouse_ctor @ 0x00426060` registers
//! three `BitmapSprite` resources with its track manager:
//!
//! ```text
//!   DAT_004af900 = { 0x1010b, 0x1010a, 0x10109 }
//!                = {  65803 ,  65802 ,  65801  }
//! ```
//!
//! Each sprite bundles two things in lock-step — a per-frame visual
//! (a strip of 14×13 frames) and a per-frame motion delta (the
//! `0x0A NotifyMove` opcodes the engine broadcasts to
//! `CGunMouse::OnMouseMove`). Splitting the two would visibly desync
//! the dot; here we keep them married by storing each track as one
//! [`Atlas`] whose `frames[i].motion` field carries the delta.
//!
//! On disk under `assets/`:
//!
//! ```text
//!   atlases/cursor_idle_a/atlas.{bin,json,png}   ← engine sprite 65803
//!   atlases/cursor_idle_b/atlas.{bin,json,png}   ← engine sprite 65802
//!   atlases/cursor_idle_c/atlas.{bin,json,png}   ← engine sprite 65801
//!   cursor/cursor.{bin,json}                      ← composition (this file's source)
//! ```
//!
//! The composition file lists atlases by their generic handle (the
//! folder name), so the cursor stays decoupled from any specific
//! engine ID. Engine provenance lives inside each atlas's
//! `meta.engineSpriteId` for traceability.

use anyhow::{anyhow, Context, Result};
use macroquad::prelude::Texture2D;
use serde::Deserialize;
use serde_json::Value;

use bulanci_core::assets::AssetFileSystem;

use crate::atlas::{Atlas, AtlasFrame};
#[cfg(test)]
use crate::atlas::parse_atlas_bytes;

/// VFS path of the production (MsgPack) composition blob.
pub const CURSOR_BIN_PATH: &str = "cursor/cursor.bin";
/// VFS path of the source-form composition blob.
pub const CURSOR_JSON_PATH: &str = "cursor/cursor.json";

/// Loaded cursor composition. Holds the three idle-track atlases in
/// `DAT_004af900` order: `idx 0` is the track `SetCurrentTrack(0, 1)`
/// plays at Activate, `idx 1` and `idx 2` are the alternatives the
/// `(rand() * 48) >> 19` re-roll selects between.
pub struct CursorAtlasSet {
    atlases: Vec<Atlas>,
    #[allow(dead_code)]
    meta: Value,
}

impl CursorAtlasSet {
    /// Load the composition + each named atlas from the VFS. Fails
    /// loudly if the composition is missing, any referenced atlas is
    /// missing, or invariants regress — every artefact is shipped in
    /// `assets.pack`, so a failure here means the build is broken.
    pub fn load(vfs: &AssetFileSystem) -> Result<Self> {
        let composition: CursorJson = if let Some(b) = vfs.read(CURSOR_BIN_PATH) {
            rmp_serde::from_slice(b)
                .with_context(|| format!("decoding msgpack {CURSOR_BIN_PATH}"))?
        } else {
            let j = vfs.read(CURSOR_JSON_PATH).ok_or_else(|| {
                anyhow!("missing both {CURSOR_BIN_PATH} and {CURSOR_JSON_PATH}")
            })?;
            serde_json::from_slice(j).with_context(|| format!("parsing {CURSOR_JSON_PATH}"))?
        };
        if composition.schema_version != 1 {
            return Err(anyhow!(
                "cursor composition: unsupported schemaVersion {}",
                composition.schema_version
            ));
        }
        if composition.idle_tracks.is_empty() {
            return Err(anyhow!("cursor composition: idleTracks is empty"));
        }

        let mut atlases = Vec::with_capacity(composition.idle_tracks.len());
        for entry in &composition.idle_tracks {
            let atlas = Atlas::load(vfs, &entry.atlas)
                .with_context(|| format!("loading cursor idle track '{}'", entry.atlas))?;
            atlases.push(atlas);
        }

        Ok(Self {
            atlases,
            meta: composition.meta,
        })
    }

    #[inline]
    pub fn len(&self) -> usize {
        self.atlases.len()
    }

    /// All loaded atlases as a slice, in composition order. The cursor
    /// passes this to [`crate::cursor::GunMouse::on_sprite_frame_tick`]
    /// so every tick reads the active track's motion from the same
    /// place its frame index is advanced from.
    pub fn atlases(&self) -> &[Atlas] {
        &self.atlases
    }

    /// Atlas at composition index `i` (wraps via `i % len()`).
    pub fn atlas(&self, i: usize) -> &Atlas {
        &self.atlases[i % self.atlases.len()]
    }

    /// Convenience: texture of the i-th track. Equivalent to
    /// `self.atlas(i).texture()`.
    pub fn texture(&self, i: usize) -> &Texture2D {
        self.atlas(i).texture()
    }

    /// Verbatim composition-level metadata (the
    /// `cursor.json`/`cursor.bin` top-level `meta` field).
    #[allow(dead_code)]
    pub fn meta(&self) -> &Value {
        &self.meta
    }

    /// Consume the set and return its owned atlases.
    #[allow(dead_code)]
    pub fn into_atlases(self) -> Vec<Atlas> {
        self.atlases
    }
}

// Convenience extractors the cursor's hot-path can call without
// re-pattern-matching each frame:

/// Per-frame motion delta, or `(0, 0)` if the frame has none. Used
/// by `on_sprite_frame_tick` to feed the dot accumulator.
#[inline]
pub fn frame_motion(frame: &AtlasFrame) -> (i8, i8) {
    if let Some(motion_val) = frame.meta.get("motion") {
        if let Some(arr) = motion_val.as_array() {
            if arr.len() == 2 {
                let dx = arr[0].as_i64().unwrap_or(0) as i8;
                let dy = arr[1].as_i64().unwrap_or(0) as i8;
                return (dx, dy);
            }
        }
    }
    (0, 0)
}

// ─────────────────────────── on-disk schema ────────────────────────────

#[derive(Deserialize)]
struct CursorJson {
    #[serde(rename = "schemaVersion")]
    schema_version: u32,
    #[serde(rename = "idleTracks")]
    idle_tracks: Vec<IdleTrackEntry>,
    #[serde(default)]
    meta: Value,
}

#[derive(Deserialize)]
struct IdleTrackEntry {
    atlas: String,
}

// ─────────────────────────── headless parsing ──────────────────────────

/// Build a `CursorAtlasSet` from in-memory composition + atlas blobs
/// (any mix of JSON and MsgPack — `parse_atlas_bytes` auto-detects).
/// Texture handles stay `None` so headless tests can drive the cursor
/// without macroquad's GL context.
#[cfg(test)]
pub(crate) fn parse_cursor_set_for_tests(
    composition_raw: &[u8],
    atlas_blobs: &[(&str, &[u8])],
) -> Result<CursorAtlasSet> {
    let composition: CursorJson = if composition_raw.first() == Some(&b'{') {
        serde_json::from_slice(composition_raw).context("parsing cursor composition (json)")?
    } else {
        rmp_serde::from_slice(composition_raw).context("decoding cursor composition (msgpack)")?
    };
    if composition.schema_version != 1 {
        return Err(anyhow!("cursor composition: unsupported schemaVersion"));
    }

    let mut atlases = Vec::with_capacity(composition.idle_tracks.len());
    for entry in &composition.idle_tracks {
        let bytes = atlas_blobs
            .iter()
            .find(|(n, _)| *n == entry.atlas)
            .map(|(_, b)| *b)
            .ok_or_else(|| anyhow!("test stub missing atlas blob '{}'", entry.atlas))?;
        atlases.push(parse_atlas_bytes(&entry.atlas, bytes)?);
    }

    Ok(CursorAtlasSet {
        atlases,
        meta: composition.meta,
    })
}

#[cfg(test)]
mod tests {
    //! Schema-level invariants for the cursor composition + each
    //! referenced atlas. Production paths go through
    //! [`CursorAtlasSet::load`]; these tests target the
    //! filesystem-backed source artefacts directly to catch
    //! `build_cursor_atlas.py` regressions before they reach the
    //! pack step.

    use super::*;

    fn shipped_cursor_set() -> CursorAtlasSet {
        let composition = std::fs::read("../assets/cursor/cursor.json")
            .expect("run scripts/build_cursor_atlas.py first (missing cursor.json)");
        let composition_parsed: CursorJson = serde_json::from_slice(&composition)
            .expect("cursor.json schema regression");

        let blobs: Vec<(String, Vec<u8>)> = composition_parsed
            .idle_tracks
            .iter()
            .map(|t| {
                let path = format!("../assets/atlases/{}/atlas.json", t.atlas);
                let bytes = std::fs::read(&path)
                    .unwrap_or_else(|_| panic!("missing atlas json: {path}"));
                (t.atlas.clone(), bytes)
            })
            .collect();
        let blob_refs: Vec<(&str, &[u8])> =
            blobs.iter().map(|(n, b)| (n.as_str(), b.as_slice())).collect();
        parse_cursor_set_for_tests(&composition, &blob_refs)
            .expect("composition fails its own invariants")
    }

    /// The shipped composition has three idle tracks, in
    /// `DAT_004af900` order, every track closes its motion loop at
    /// (0, 0), and frame dims stay at 14×13. If this regresses,
    /// regenerate via `scripts/build_cursor_atlas.py` and re-pack.
    #[test]
    fn shipped_composition_has_three_closed_idle_tracks() {
        let set = shipped_cursor_set();
        assert_eq!(set.len(), 3, "expected exactly 3 idle-twitch tracks");

        let names: Vec<&str> = set.atlases().iter().map(|a| a.name.as_str()).collect();
        assert_eq!(names, vec!["cursor_idle_a", "cursor_idle_b", "cursor_idle_c"]);

        let expected_engine_ids = [65803_i64, 65802, 65801];
        for (atlas, expected_id) in set.atlases().iter().zip(expected_engine_ids.iter()) {
            let engine_id = atlas
                .meta
                .get("engineSpriteId")
                .and_then(|v| v.as_i64())
                .unwrap_or_else(|| panic!("{}: missing meta.engineSpriteId", atlas.name));
            assert_eq!(
                engine_id, *expected_id,
                "{}: engineSpriteId mismatch",
                atlas.name
            );

            for f in &atlas.frames {
                assert_eq!(
                    f.rect.w, 14,
                    "{}: frame rect width regressed ({}×{})",
                    atlas.name, f.rect.w, f.rect.h
                );
                assert_eq!(
                    f.rect.h, 13,
                    "{}: frame rect height regressed ({}×{})",
                    atlas.name, f.rect.w, f.rect.h
                );
                assert_eq!(
                    f.origin,
                    (7, 6),
                    "{}: frame origin should center the 14×13 dot (got {:?})",
                    atlas.name,
                    f.origin
                );
            }

            let (sx, sy) = atlas
                .frames
                .iter()
                .map(|f| frame_motion(f))
                .fold((0_i32, 0_i32), |(ax, ay), (dx, dy)| {
                    (ax + dx as i32, ay + dy as i32)
                });
            assert_eq!(
                (sx, sy),
                (0, 0),
                "{}: motion deltas don't close to origin",
                atlas.name
            );
        }

        let lens: Vec<usize> = set.atlases().iter().map(|a| a.frames.len()).collect();
        assert_eq!(
            lens,
            vec![9, 22, 24],
            "track lengths regressed (hand-verified against engine sprites 65803/65802/65801)"
        );
    }
}
