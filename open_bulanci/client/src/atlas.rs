//! Generic atlas resource — a frame-strip / sprite-sheet image plus
//! structured per-frame and per-atlas metadata.
//!
//! An [`Atlas`] is a reusable bundle any client subsystem (cursor,
//! units, projectiles, fx, fonts, …) can consume. It carries:
//!
//! - **A sheet image**. Frames are typically packed into it, but the
//!   atlas itself never assumes a fixed grid — each [`AtlasFrame`]
//!   stores its own [`AtlasRect`]. Atlases whose frames happen to be
//!   uniform-size can declare a top-level `frame: { width, height }`
//!   default to keep the JSON terse; the loader then synthesises
//!   per-frame rects packed left-to-right at `(i × frame_w, 0)`.
//! - **A 2D origin** ("anchor"). Drawing at `(x, y)` actually paints
//!   the frame so that its origin sits at `(x, y)`; a centred sprite
//!   declares `origin = (frame_w/2, frame_h/2)`. Atlas-level origin
//!   acts as the default; any frame may override.
//! - **Arbitrary `meta`** at both atlas-level and frame-level —
//!   any metadata the asset author wants to attach (such as cursor
//!   idle movement deltas). Carried through verbatim as
//!   [`serde_json::Value`] so subsystems can deserialise/extract
//!   into their own typed views on demand (see e.g. `frame_motion` in `cursor_atlas.rs`).
//!
//! ## Runtime layout & Compile-time Gating
//!
//! Each atlas lives in one folder under `atlases/`:
//!
//! ```text
//!   atlases/<name>/atlas.bin    ← production runtime form (MsgPack)
//!   atlases/<name>/atlas.json   ← source / dev form
//!   atlases/<name>/atlas.png    ← the sheet image (referenced by `image`)
//! ```
//!
//! Gated by compilation target:
//! - **Dev builds (`#[cfg(debug_assertions)]`)** can load both MsgPack and JSON,
//!   but they load `.json` by default to enable hot-editing.
//! - **Production builds (`#[cfg(not(debug_assertions))]`)** compile *only* the
//!   MsgPack loading path. No JSON decoding code or overhead is shipped.
//!
//! ## Schema (schemaVersion = 1)
//!
//! ```json
//! {
//!   "schemaVersion": 1,
//!   "name": "cursor_idle_a",
//!   "image": "atlas.png",
//!   "frame":  { "width": 14, "height": 13 },   // optional default
//!   "origin": { "x": 7, "y": 6 },               // optional default
//!   "frames": [
//!     {
//!       "rect":   { "x": 0, "y": 0, "w": 14, "h": 13 }, // optional override
//!       "polygon": [[0,0], [14,0], [14,13], [0,13]],    // optional polygon
//!       "origin": { "x": 7, "y": 6 },                    // optional override
//!       "meta": {
//!         "motion": { "dx": 1, "dy": 2 }                 // optional motion in meta
//!       }
//!     }
//!   ],
//!   "meta": {
//!     "engineSpriteId": 65803
//!   }
//! }
//! ```
//!
//! Resolution rules applied at load:
//!
//! - Every frame must end up with a non-zero [`AtlasRect`]. If a
//!   frame omits `rect`, the atlas must declare a top-level `frame`,
//!   in which case the synthesised rect is
//!   `(index × frame.width, 0, frame.width, frame.height)`.
//! - Every frame's origin = explicit per-frame `origin` if present,
//!   else atlas-level `origin`, else `(0, 0)`.

use anyhow::{anyhow, Context, Result};
use macroquad::prelude::{FilterMode, Texture2D};
use serde::Deserialize;
use serde_json::Value;

use bulanci_core::assets::AssetFileSystem;

/// Source rect inside the sheet image. All four values are unsigned
/// pixel counts — atlases never reference pixels outside the sheet.
#[derive(Clone, Copy, Debug, PartialEq, Eq, Deserialize)]
pub struct AtlasRect {
    pub x: u32,
    pub y: u32,
    pub w: u32,
    pub h: u32,
}

/// Per-frame entry. Always exists for every `i` in `0..frameCount`.
#[allow(dead_code)]
pub struct AtlasFrame {
    /// Source rect inside the sheet. Always resolved at load time.
    pub rect: AtlasRect,
    /// 2D outline/hitbox or other polygon vertices as `(x, y)` points.
    pub polygon: Option<Vec<(f32, f32)>>,
    /// 2D anchor relative to the frame's top-left corner. Drawing at
    /// `(x, y)` paints the frame so that this point sits at `(x, y)`.
    pub origin: (i32, i32),
    /// Verbatim per-frame metadata (`Value::Null` when absent).
    pub meta: Value,
}

/// A loaded atlas. Owns its GPU texture and its per-frame metadata.
pub struct Atlas {
    /// Canonical handle (matches the folder name under `atlases/`).
    #[allow(dead_code)]
    pub name: String,
    /// Frames in playback / sheet-index order.
    pub frames: Vec<AtlasFrame>,
    /// GPU texture. `None` only in headless test parses — see
    /// [`Atlas::texture`]; production [`Atlas::load`] always
    /// populates it.
    pub texture: Option<Texture2D>,
    /// Verbatim atlas-level metadata (`Value::Null` when absent).
    #[allow(dead_code)]
    pub meta: Value,
}

impl Atlas {
    /// VFS path of the binary (production) atlas blob for a handle.
    pub fn bin_path(name: &str) -> String {
        format!("atlases/{name}/atlas.bin")
    }

    /// VFS path of the JSON (source / dev) atlas blob for a handle.
    pub fn json_path(name: &str) -> String {
        format!("atlases/{name}/atlas.json")
    }

    /// Load atlas `<name>` from the VFS. Gated by build profile.
    /// Fails loudly on missing files, schema mismatch, or bad formats.
    #[cfg(debug_assertions)]
    pub fn load(vfs: &AssetFileSystem, name: &str) -> Result<Self> {
        let json_path = Self::json_path(name);
        let parsed: AtlasJson = if let Some(j) = vfs.read(&json_path) {
            serde_json::from_slice(j)
                .with_context(|| format!("parsing json {json_path}"))?
        } else {
            let bin_path = Self::bin_path(name);
            let b = vfs
                .read(&bin_path)
                .ok_or_else(|| anyhow!("atlas {name}: missing both {json_path} and {bin_path}"))?;
            rmp_serde::from_slice(b)
                .with_context(|| format!("decoding msgpack {bin_path}"))?
        };

        resolve_texture(name, parsed, vfs)
    }

    /// Load atlas `<name>` from the VFS. Production builds only ship
    /// and compile the binary MsgPack loading logic.
    #[cfg(not(debug_assertions))]
    pub fn load(vfs: &AssetFileSystem, name: &str) -> Result<Self> {
        let bin_path = Self::bin_path(name);
        let b = vfs
            .read(&bin_path)
            .ok_or_else(|| anyhow!("atlas {name}: missing {bin_path}"))?;
        let parsed: AtlasJson = rmp_serde::from_slice(b)
            .with_context(|| format!("decoding msgpack {bin_path}"))?;

        resolve_texture(name, parsed, vfs)
    }

    /// GPU texture for this atlas. Panics if called on a headless
    /// parse — production code paths always go through
    /// [`Atlas::load`] which populates the texture.
    #[inline]
    pub fn texture(&self) -> &Texture2D {
        self.texture
            .as_ref()
            .expect("atlas texture missing; only headless parses leave this None")
    }

    #[inline]
    pub fn frame_count(&self) -> usize {
        self.frames.len()
    }
}

fn resolve_texture(name: &str, raw: AtlasJson, vfs: &AssetFileSystem) -> Result<Atlas> {
    let mut atlas = resolve(name, raw)?;

    let image_path = format!(
        "atlases/{name}/{}",
        atlas
            .meta
            .get("__image")
            .and_then(|v| v.as_str())
            .map(str::to_owned)
            .unwrap_or_else(|| "atlas.png".to_owned())
    );

    // Strip the internal helper key before exposing metadata publicly.
    if let Value::Object(ref mut map) = atlas.meta {
        map.remove("__image");
    }

    let bytes = vfs
        .read(&image_path)
        .ok_or_else(|| anyhow!("atlas {name}: missing image {image_path}"))?;
    let texture = Texture2D::from_file_with_format(bytes, None);
    texture.set_filter(FilterMode::Nearest);
    atlas.texture = Some(texture);

    Ok(atlas)
}

fn resolve(name: &str, raw: AtlasJson) -> Result<Atlas> {
    if raw.schema_version != 1 {
        return Err(anyhow!(
            "atlas {name}: unsupported schemaVersion {} (expected 1)",
            raw.schema_version
        ));
    }
    if raw.name != name {
        return Err(anyhow!(
            "atlas {name}: name field is '{}', folder name disagrees",
            raw.name
        ));
    }
    if raw.frames.is_empty() {
        return Err(anyhow!("atlas {name}: must declare at least one frame"));
    }

    let default_origin = raw.origin.map(|o| (o.x, o.y)).unwrap_or((0, 0));
    let default_dims = raw.frame;

    let mut frames = Vec::with_capacity(raw.frames.len());
    for (i, f) in raw.frames.into_iter().enumerate() {
        let rect = match f.rect {
            Some(r) => AtlasRect {
                x: r.x,
                y: r.y,
                w: r.w,
                h: r.h,
            },
            None => {
                let d = default_dims.ok_or_else(|| {
                    anyhow!(
                        "atlas {name}: frame {i} omits rect but atlas declares no default frame size"
                    )
                })?;
                AtlasRect {
                    x: (i as u32) * d.width,
                    y: 0,
                    w: d.width,
                    h: d.height,
                }
            }
        };
        if rect.w == 0 || rect.h == 0 {
            return Err(anyhow!(
                "atlas {name}: frame {i} has degenerate rect {}×{}",
                rect.w,
                rect.h
            ));
        }
        let polygon = f.polygon.map(|p| p.into_iter().map(|[x, y]| (x, y)).collect());
        let origin = match f.origin {
            Some(o) => (o.x, o.y),
            None => default_origin,
        };

        frames.push(AtlasFrame {
            rect,
            polygon,
            origin,
            meta: f.meta,
        });
    }

    // Stash the image filename inside meta so the loader can
    // find it without re-decoding.
    let mut meta = raw.meta;
    match &mut meta {
        Value::Object(map) => {
            map.insert("__image".to_owned(), Value::String(raw.image));
        }
        _ => {
            meta = serde_json::json!({ "__image": raw.image });
        }
    }

    Ok(Atlas {
        name: raw.name,
        frames,
        texture: None,
        meta,
    })
}

// ─────────────────────────── on-disk schema ────────────────────────────

#[derive(Deserialize)]
struct AtlasJson {
    #[serde(rename = "schemaVersion")]
    schema_version: u32,
    name: String,
    image: String,
    #[serde(default)]
    frame: Option<FrameDims>,
    #[serde(default)]
    origin: Option<OriginJson>,
    frames: Vec<FrameJson>,
    #[serde(default)]
    meta: Value,
}

#[derive(Clone, Copy, Deserialize)]
struct FrameDims {
    width: u32,
    height: u32,
}

#[derive(Deserialize)]
struct OriginJson {
    x: i32,
    y: i32,
}

#[derive(Deserialize)]
struct FrameJson {
    #[serde(default)]
    rect: Option<RectJson>,
    #[serde(default)]
    polygon: Option<Vec<[f32; 2]>>,
    #[serde(default)]
    origin: Option<OriginJson>,
    #[serde(default)]
    meta: Value,
}

#[derive(Deserialize)]
struct RectJson {
    x: u32,
    y: u32,
    w: u32,
    h: u32,
}

// ─────────────────────────── headless parsing ──────────────────────────

/// Parse an atlas blob (JSON or MsgPack — auto-detected by leading
/// byte: `'{'` → JSON, anything else → MsgPack) without touching the
/// GPU. Returns an [`Atlas`] whose `texture` is `None`. Tests use
/// this to validate the manifest pipeline end-to-end without
/// initialising macroquad.
#[allow(dead_code)]
pub fn parse_atlas_bytes(name: &str, raw: &[u8]) -> Result<Atlas> {
    let parsed: AtlasJson = if raw.first() == Some(&b'{') {
        serde_json::from_slice(raw).with_context(|| format!("parsing atlas {name} (json)"))?
    } else {
        rmp_serde::from_slice(raw).with_context(|| format!("parsing atlas {name} (msgpack)"))?
    };
    let mut atlas = resolve(name, parsed)?;
    if let Value::Object(ref mut map) = atlas.meta {
        map.remove("__image");
    }
    Ok(atlas)
}
