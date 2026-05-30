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

use std::collections::HashSet;

use anyhow::{anyhow, Context, Result};
use raylib::prelude::{RaylibHandle, RaylibThread};
use serde::Deserialize;
use serde_json::Value;

use crate::asset::handle::AssetHandle;
use crate::asset::types::{BitmapSprite, BitmapSpritePng, CursorComposition};
use crate::asset::AssetServer;
use crate::atlas::{Atlas, AtlasFrame};
#[cfg(test)]
use crate::atlas::parse_atlas_bytes;

/// Everything `CursorAtlasSet::load` needs to rebuild itself on a hot
/// reload: the composition handle plus the per-track JSON/PNG handle
/// pairs.  Kept on the set so the engine tick can reconstruct the
/// whole thing when any dependency's bytes change.  `None` for the
/// headless test constructor (which never hot-reloads).
struct CursorSource {
    composition: AssetHandle<CursorComposition>,
    idle_tracks: Vec<(AssetHandle<BitmapSprite>, AssetHandle<BitmapSpritePng>)>,
}

/// Loaded cursor composition. Holds the three idle-track atlases in
/// `DAT_004af900` order: `idx 0` is the track `SetCurrentTrack(0, 1)`
/// plays at Activate, `idx 1` and `idx 2` are the alternatives the
/// `(rand() * 48) >> 19` re-roll selects between.
pub struct CursorAtlasSet {
    atlases: Vec<Atlas>,
    #[allow(dead_code)]
    meta: Value,
    source: Option<CursorSource>,
}

impl CursorAtlasSet {
    /// Load the composition + every idle-track atlas through typed
    /// handles.  The composition handle gives us the JSON blob (or
    /// the [`crate::asset::fallback::EMPTY_CURSOR_JSON`] placeholder
    /// when missing); the per-track JSON / PNG handles are passed in
    /// explicitly because the composition file lists slugs and the
    /// typed-handle layer must resolve them at compile time, not at
    /// runtime.  We assert that the JSON's `idleTracks` count matches
    /// the supplied handle list so a registry edit that adds /
    /// removes a track surfaces as a startup error rather than a
    /// silent mis-mapping.
    pub fn load(
        rl: &mut RaylibHandle,
        thread: &RaylibThread,
        assets: &AssetServer,
        composition_handle: AssetHandle<CursorComposition>,
        idle_tracks: &[(AssetHandle<BitmapSprite>, AssetHandle<BitmapSpritePng>)],
    ) -> Result<Self> {
        let comp_bytes = assets.bytes(composition_handle);
        // The pack-time transcoder rewrites `cursor/cursor.json` to a
        // MsgPack blob (smaller, faster to decode), but the placeholder
        // and any dev override stay as raw JSON.  The leading byte
        // disambiguates: `{` is JSON, anything else is MsgPack.
        let composition: CursorJson = if comp_bytes.first() == Some(&b'{') {
            serde_json::from_slice(&comp_bytes)
                .context("decoding cursor composition JSON")?
        } else {
            rmp_serde::from_slice(&comp_bytes)
                .context("decoding cursor composition MsgPack")?
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
        if composition.idle_tracks.len() != idle_tracks.len() {
            return Err(anyhow!(
                "cursor composition lists {} tracks but the caller provided {} handle pairs",
                composition.idle_tracks.len(),
                idle_tracks.len(),
            ));
        }

        let mut atlases = Vec::with_capacity(idle_tracks.len());
        for (idx, &(json_h, png_h)) in idle_tracks.iter().enumerate() {
            let atlas = Atlas::load(rl, thread, assets, json_h, png_h)
                .with_context(|| format!("loading cursor idle track #{idx}"))?;
            atlases.push(atlas);
        }

        Ok(Self {
            atlases,
            meta: composition.meta,
            source: Some(CursorSource {
                composition: composition_handle,
                idle_tracks: idle_tracks.to_vec(),
            }),
        })
    }

    /// `true` if the composition or any idle-track atlas depends on a
    /// hash in `dirty` — i.e. the user edited `cursor.bin` or one of
    /// the track PNG/JSON files.  Drives the engine tick's decision to
    /// call [`CursorAtlasSet::reload`].
    pub fn touched_by(&self, dirty: &HashSet<u64>) -> bool {
        let Some(src) = &self.source else { return false; };
        if dirty.contains(&src.composition.raw()) {
            return true;
        }
        src.idle_tracks
            .iter()
            .any(|(j, p)| dirty.contains(&j.raw()) || dirty.contains(&p.raw()))
    }

    /// Rebuild the whole set from its stored handles.  We rebuild the
    /// entire composition (not just the dirty track) because a
    /// `cursor.bin` edit can change the track count or meta, and the
    /// extra two atlas decodes are cheap and only happen on an actual
    /// edit.  On failure the previous set is preserved.
    pub fn reload(
        &mut self,
        rl: &mut RaylibHandle,
        thread: &RaylibThread,
        assets: &AssetServer,
    ) -> Result<()> {
        let Some(src) = &self.source else { return Ok(()); };
        let composition = src.composition;
        let idle_tracks = src.idle_tracks.clone();
        *self = CursorAtlasSet::load(rl, thread, assets, composition, &idle_tracks)?;
        Ok(())
    }

    /// All loaded atlases as a slice, in composition order. Served to the
    /// Luau menu scene (via `scene_runtime::build_services`) as frame
    /// count + per-frame `(Δx, Δy)` motion so the cursor's idle-twitch dot
    /// reads its motion from the same place its frame index is advanced.
    pub fn atlases(&self) -> &[Atlas] {
        &self.atlases
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

#[derive(Deserialize, Default)]
struct IdleTrackEntry {
    // Compiled-out of release builds: in release, `pack_assets.py`
    // strips every per-track `atlas: "<slug>"` field from the cursor
    // composition msgpack precisely because the field name would
    // leak into `.rodata` via the serde derive table.  The headless
    // tests still read the on-disk JSON though, where the field is
    // present, so we keep it under `#[cfg(test)]` (with `default` so
    // production deserialisation succeeds against the stripped blob
    // — empty `idleTracks` entries are fine, the production loader
    // takes the atlas handles from `CursorAtlasSet::load`'s arg).
    #[cfg(test)]
    #[serde(default)]
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
        source: None,
    })
}

#[cfg(test)]
mod tests {
    //! Schema-level invariants for the cursor composition + each
    //! referenced atlas. Production paths go through
    //! [`CursorAtlasSet::load`]; these tests target the
    //! filesystem-backed source artefacts directly to catch
    //! `open_bulanci/asset_pipeline/build_assets.py` regressions
    //! before they reach the pack step.

    use super::*;

    fn shipped_cursor_set() -> CursorAtlasSet {
        let composition = std::fs::read("../assets/cursor/cursor.json")
            .expect("run open_bulanci/asset_pipeline/build_assets.py first (missing cursor.json)");
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
    /// regenerate via `open_bulanci/asset_pipeline/build_assets.py` and re-pack.
    #[test]
    fn shipped_composition_has_three_closed_idle_tracks() {
        let set = shipped_cursor_set();
        assert_eq!(set.atlases().len(), 3, "expected exactly 3 idle-twitch tracks");

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
