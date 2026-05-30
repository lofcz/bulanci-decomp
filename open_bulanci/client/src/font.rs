//! Text rendering — bitmap-font port of the original game's `CDSFont`.
//!
//! The original Bulánci engine ships three pre-rendered glyph atlases
//! (ClassID 54, resource IDs `0x100ae` / `0x100af` / `0x100b0`) and routes
//! every UI string through `CDSFont::DrawChar` (`@ 0x00437370`). Each atlas
//! is a single-row strip:
//!
//! | Resource | Atlas    | Line H | bpp | Role           |
//! |----------|----------|-------:|----:|----------------|
//! | 0x100ae  | 1713×16  |   16   |  2  | Large UI font  |
//! | 0x100af  | 1069×13  |   13   |  1  | Medium (poem)  |
//! | 0x100b0  |  879×11  |   11   |  1  | Small UI font  |
//!
//! Glyph metrics — extracted by `tools/bulanci_unpack/bulanci_unpack.py`
//! and shipped as `*.json` next to each atlas PNG — store a 256-entry
//! table indexed by **CP-1250** code byte. Each entry is
//! `{offsetX:u16, width:u8, offsetY:u8, height:u8}`, matching the on-disk
//! `CDSFont` layout (see `_save_font` in the unpacker).
//!
//! ## Render model (`CDSFont::DrawChar`)
//!
//! For each glyph the engine blits the atlas rect
//! `(offsetX, offsetY) .. (offsetX+width, height)` onto the target at
//! `(dest_x, dest_y + offsetY) .. (dest_x + width, dest_y + height)`. So
//! `dest_y` is the **top of the font cell**, and the metrics already
//! encode each glyph's vertical placement within that cell.
//!
//! ## Public abstraction
//!
//! All UI text in `open_bulanci` goes through the [`FontProvider`] trait
//! so callers never see the underlying renderer (bitmap atlas, TTF,
//! whatever). [`BitmapFontProvider`] is the default implementation; a
//! swap to TTF only requires another impl of the same trait. Callers
//! request a size in pixels and the provider picks the closest available
//! bitmap.

use raylib::prelude::*;
use serde::Deserialize;

// ============================================================================
// Glyph metrics + atlas
// ============================================================================

/// One entry in a `CDSFont` 256-glyph metrics table.
///
/// Field meaning is taken verbatim from the disassembly of
/// `CDSFont::DrawChar` (see `ghidra` decompile at `0x00437370`): the source
/// rect inside the atlas is `(offset_x, offset_y) .. (offset_x + width,
/// height)`, and the destination is offset by `offset_y` rows from the
/// caller's `dest_y` argument.
#[derive(Debug, Clone, Copy, Default, Deserialize)]
pub struct GlyphMetric {
    /// X position of this glyph's left edge inside the atlas, in pixels.
    #[serde(rename = "offsetX")]
    pub offset_x: u16,
    /// Width of this glyph in pixels. `0` means "this code is not mapped"
    /// (the original engine returns `default_width` as the advance and
    /// draws nothing).
    pub width: u8,
    /// Vertical offset of the glyph's first row inside the atlas, relative
    /// to the atlas top edge.
    #[serde(rename = "offsetY")]
    pub offset_y: u8,
    /// Bottom edge (exclusive) of the glyph rows inside the atlas. The
    /// actual rendered height is `height - offset_y`.
    pub height: u8,
}

/// Raw shape of a `*.font.json` file as emitted by
/// `tools/bulanci_unpack/bulanci_unpack.py::_save_font`.
#[derive(Debug, Deserialize)]
struct RawFontJson {
    #[serde(rename = "defaultWidth")]
    default_width: u32,
    #[serde(rename = "lineHeight")]
    line_height: u32,
    characters: Vec<RawCharEntry>,
}

#[derive(Debug, Deserialize)]
struct RawCharEntry {
    index: usize,
    #[serde(rename = "offsetX")]
    offset_x: u16,
    width: u8,
    #[serde(rename = "offsetY")]
    offset_y: u8,
    height: u8,
}

/// A fully-loaded bitmap font: GPU texture for the atlas + parsed glyph
/// table.
///
/// The atlas texture has already been **chroma-keyed**: every pixel that
/// is black in the source PNG has been set to alpha = 0, every pixel that
/// is white has been set to RGB = (255, 255, 255). That way callers can
/// just multiply the texture by their desired text colour at draw time
/// (matching the `primary_color` field of the engine's `font_info`
/// struct from `TextShaper_LayOutAndRender`).
pub struct BitmapFont {
    /// GPU atlas (one row of glyphs, `atlas_w × line_height` pixels).
    pub atlas: Texture2D,
    /// Cell height in pixels — `dest_y .. dest_y + line_height` is the
    /// vertical band one line of text occupies.
    pub line_height: u32,
    /// Fallback advance for glyphs whose `width == 0` (matches
    /// `CDSFont::GetCharWidth` falling back to the singleton at
    /// `this + 0x215`).
    pub default_width: u32,
    /// Indexed by **CP-1250 byte**. `glyphs[c as usize]` is the metric for
    /// the character whose CP-1250 encoding is `c`.
    pub glyphs: [GlyphMetric; 256],
    /// Approximate distance from the cell top to the ASCII baseline
    /// (`offset_y + glyph_height` for capital letters, derived once at
    /// load time). Used by the [`FontProvider`] adapter that takes a
    /// baseline-style `y` (so callers using macroquad-style baseline
    /// coordinates Just Work).
    pub baseline: f32,
}

impl BitmapFont {
    /// Pixel advance of a single CP-1250 code byte. Mirrors
    /// `CDSFont::GetCharWidth @ 0x00437330`:
    ///
    /// ```text
    /// if (glyph.width != 0) return glyph.width;
    /// return defaultWidth >> 2;        // arithmetic-shift, but
    ///                                  // defaultWidth is always positive
    /// ```
    ///
    /// The `>> 2` fallback is the original engine's space-advance trick:
    /// the metrics table stores `width = 0` for the literal space glyph
    /// (and every unmapped CP-1250 byte) and `GetCharWidth` recovers a
    /// quarter of `defaultWidth` as the pen advance for those. For our
    /// three atlases this works out to `2`/`3`/`4` pixels (small/medium/
    /// large) — the exact word-spacing the original game uses.
    fn char_advance(&self, b: u8) -> i32 {
        let w = self.glyphs[b as usize].width;
        if w != 0 {
            w as i32
        } else {
            (self.default_width as i32) >> 2
        }
    }

    /// The 256-entry CP-1250 advance table (`char_advance` for every
    /// code byte). Handed to the scene host so Luau's `bulanci.measure`
    /// can reproduce `measure` without borrowing the font itself.
    pub fn advances(&self) -> [i32; 256] {
        let mut t = [0i32; 256];
        for (b, slot) in t.iter_mut().enumerate() {
            *slot = self.char_advance(b as u8);
        }
        t
    }

    /// Pixel width of `text` if rendered with this font, with one
    /// `char_spacing` pixel of padding between glyphs (same convention as
    /// `CDSFont::GetCharWidth + font_info[+4]` in
    /// `TextShaper_LayOutAndRender`).
    pub fn measure(&self, text: &str, char_spacing: i32) -> f32 {
        let mut x: i32 = 0;
        for ch in text.chars() {
            let b = utf32_to_cp1250(ch as u32);
            x += self.char_advance(b) + char_spacing;
        }
        x.max(0) as f32
    }

    /// Draw `text` so that the top of the font cell lands at `(x, y)`.
    /// Glyph-level positioning matches `CDSFont::DrawChar` exactly: the
    /// atlas rect `(offset_x, offset_y) .. (offset_x + width, height)` is
    /// blitted to `(dest_x, dest_y + offset_y) .. (dest_x + width,
    /// dest_y + height)`.
    ///
    /// Pen X is snapped to an integer on every iteration so the texture
    /// quads land on whole-pixel boundaries — bitmap fonts look
    /// "shredded" / blurry the moment they sample at fractional coords,
    /// because adjacent atlas glyphs sit zero-pixels apart and any
    /// interpolation bleeds one glyph's edge into the next.
    pub fn draw_cell_top<D: RaylibDraw>(
        &self,
        d: &mut D,
        text: &str,
        x: f32,
        y: f32,
        color: Color,
        char_spacing: i32,
    ) {
        let mut pen_x = x.round();
        let y = y.round();
        for ch in text.chars() {
            let b = utf32_to_cp1250(ch as u32);
            let m = &self.glyphs[b as usize];
            if m.width != 0 && m.height as u16 > m.offset_y as u16 {
                let src_h = (m.height as u16 - m.offset_y as u16) as f32;
                crate::gfx::blit(
                    d,
                    &self.atlas,
                    pen_x,
                    y + m.offset_y as f32,
                    Some(Rectangle::new(
                        m.offset_x as f32,
                        m.offset_y as f32,
                        m.width as f32,
                        src_h,
                    )),
                    None,
                    false,
                    color,
                );
            }
            pen_x += (self.char_advance(b) + char_spacing) as f32;
        }
    }
}

// ============================================================================
// JSON + PNG loading
// ============================================================================

/// Parse a `*.font.json` blob into a 256-entry metrics table + line
/// metrics. Returns `Err` on malformed input.
fn parse_metrics(json: &str) -> anyhow::Result<(u32, u32, [GlyphMetric; 256])> {
    let raw: RawFontJson = serde_json::from_str(json)?;
    let mut glyphs = [GlyphMetric::default(); 256];
    for ch in raw.characters {
        if ch.index < 256 {
            glyphs[ch.index] = GlyphMetric {
                offset_x: ch.offset_x,
                width: ch.width,
                offset_y: ch.offset_y,
                height: ch.height,
            };
        }
    }
    Ok((raw.default_width, raw.line_height, glyphs))
}

/// Estimate the ASCII baseline of a font by inspecting capital `A` (or
/// falling back to `M`, `H`, `0`). For all three CDSFont atlases shipped
/// with Bulánci, `A`'s `offset_y + (height - offset_y) == offset_y +
/// glyph_height` is the bottom of the cap-height band — close enough to
/// the visual baseline for placement.
fn estimate_baseline(glyphs: &[GlyphMetric; 256]) -> f32 {
    for code in [b'A', b'M', b'H', b'0', b'I'] {
        let g = &glyphs[code as usize];
        if g.width != 0 && g.height > g.offset_y {
            return g.height as f32;
        }
    }
    // Last-ditch fallback for a font that somehow has no Latin glyphs.
    0.0
}

/// Take an atlas PNG with white-on-black glyphs (the format emitted by
/// the unpacker for both 1bpp and 2bpp `CDSFont` atlases) and convert it
/// into an RGBA texture where:
///   - black source pixels become fully transparent;
///   - non-black source pixels become opaque white, with `alpha =
///     max(R, G, B)` so 2bpp anti-aliased atlases keep their shading.
///
/// This way the caller can colour text at draw time by passing any tint
/// and the per-pixel alpha is preserved.
///
/// We decode the PNG to RGBA with the `image` crate (raylib's stb_image
/// can't expose a mutable pixel buffer ergonomically), run the chroma-key
/// on the raw bytes, then hand them to [`crate::gfx::texture_from_rgba8`] —
/// the shared, WebGL-safe upload path used by every texture in the client.
fn atlas_png_to_texture(rl: &mut RaylibHandle, thread: &RaylibThread, bytes: &[u8]) -> Texture2D {
    let decoded = image::load_from_memory_with_format(bytes, image::ImageFormat::Png)
        .expect("font atlas decode")
        .to_rgba8();
    let (w, h) = decoded.dimensions();
    let mut rgba = decoded.into_raw();
    for px in rgba.chunks_exact_mut(4) {
        let luma = px[0].max(px[1]).max(px[2]);
        if luma == 0 {
            px[3] = 0;
        } else {
            px[0] = 255;
            px[1] = 255;
            px[2] = 255;
            px[3] = luma;
        }
    }
    crate::gfx::texture_from_rgba8(rl, thread, w, h, &rgba, TextureFilter::TEXTURE_FILTER_POINT)
        .expect("font atlas upload")
}

/// Load one `BitmapFont` from already-resolved bytes.  No path
/// strings cross the API — call sites pass typed [`AssetHandle`]s
/// through [`crate::asset::AssetServer::bytes`] and feed the results
/// in here.  Used by [`BitmapFontProvider::from_handles`].
pub fn load_bitmap_font_from_bytes(
    rl: &mut RaylibHandle,
    thread: &RaylibThread,
    png_bytes: &[u8],
    json_bytes: &[u8],
) -> anyhow::Result<BitmapFont> {
    let json_str = std::str::from_utf8(json_bytes)
        .map_err(|e| anyhow::anyhow!("font metrics JSON is not UTF-8: {e}"))?;
    let (default_width, line_height, glyphs) = parse_metrics(json_str)?;
    let baseline = estimate_baseline(&glyphs);
    Ok(BitmapFont {
        atlas: atlas_png_to_texture(rl, thread, png_bytes),
        line_height,
        default_width,
        glyphs,
        baseline,
    })
}

// ============================================================================
// Provider abstraction — every UI text call goes through this trait so we
// can swap the underlying renderer (bitmap atlas / TTF / future GPU SDF)
// without touching any of the screen modules.
// ============================================================================

/// Provider-agnostic text renderer. The size parameter is a **pixel
/// height hint** (mirrors macroquad's `font_size`); a bitmap-font impl
/// picks the closest matching atlas, a TTF impl scales freely.
///
/// `char_spacing` is the extra pixel gap inserted between every pair of
/// glyphs, matching the original engine's `font_info[+4]` field that
/// `TextShaper_LayOutAndRender` adds to each char-advance step. The
/// original game wires this per-widget — `CStaticText` defaults to 0,
/// `CButton::BuildAt` sets `1`, `CRadio::BuildAt` sets `2`,
/// `CPoemScroller::ctor` sets `1`. Callers should pass the same value
/// the original widget did to keep the layout pixel-accurate.
#[allow(dead_code)] // `line_height` / `ascent` are part of the public
                    // contract so future providers can be plugged in;
                    // no current call-site uses them yet.
pub trait FontProvider {
    /// Pixel width of `text` at `size_px` with `char_spacing` extra
    /// pixels between glyphs. Mirrors macroquad's
    /// `measure_text(...).width`.
    fn measure(&self, text: &str, size_px: u16, char_spacing: i32) -> f32;
    /// Total height of one line of text at `size_px` (atlas line height
    /// for bitmap fonts, font size for TTF).
    fn line_height(&self, size_px: u16) -> f32;
    /// Distance from a baseline `y` up to the top of the font cell —
    /// equivalently, the offset to subtract from a baseline `y` to get
    /// the cell-top `y` the bitmap blitter wants.
    fn ascent(&self, size_px: u16) -> f32;
}

/// The "use the original game's bitmap fonts" implementation. Holds all
/// three atlases and dispatches to the closest size for each call.
pub struct BitmapFontProvider {
    pub small: BitmapFont,
    pub medium: BitmapFont,
    pub large: BitmapFont,
}

impl BitmapFontProvider {
    /// Load all three CDSFont atlases through the typed-handle asset
    /// server.  Every byte buffer comes back via
    /// [`crate::asset::AssetServer::bytes`], so missing files fall
    /// back to the class-typed placeholders (magenta 1×1 PNG + empty
    /// JSON) and the screen renders the placeholder text-shape
    /// instead of panicking on startup.  No path string ever
    /// crosses the API.
    pub fn from_handles(
        rl: &mut RaylibHandle,
        thread: &RaylibThread,
        assets: &crate::asset::AssetServer,
    ) -> anyhow::Result<Self> {
        use crate::generated::assets::engine::fonts as fh;
        let load = |rl: &mut RaylibHandle,
                        png: crate::asset::handle::AssetHandle<crate::asset::types::Font>,
                        json: crate::asset::handle::AssetHandle<crate::asset::types::FontJson>|
                   -> anyhow::Result<BitmapFont> {
            let png_bytes  = assets.bytes(png);
            let json_bytes = assets.bytes(json);
            load_bitmap_font_from_bytes(rl, thread, &png_bytes, &json_bytes)
        };
        Ok(Self {
            small:  load(rl, fh::FONT_SMALL,  fh::FONT_SMALL_JSON)?,
            medium: load(rl, fh::FONT_MEDIUM, fh::FONT_MEDIUM_JSON)?,
            large:  load(rl, fh::FONT_LARGE,  fh::FONT_LARGE_JSON)?,
        })
    }

    /// The six handle hashes the three bitmap fonts are built from
    /// (PNG + metrics JSON for small / medium / large).  Used by the
    /// hot-reload tick to detect when a font asset changed.
    pub fn source_hashes() -> [u64; 6] {
        use crate::generated::assets::engine::fonts as fh;
        [
            fh::FONT_SMALL.raw(),  fh::FONT_SMALL_JSON.raw(),
            fh::FONT_MEDIUM.raw(), fh::FONT_MEDIUM_JSON.raw(),
            fh::FONT_LARGE.raw(),  fh::FONT_LARGE_JSON.raw(),
        ]
    }

    /// `true` if a hot reload touched any of the three font atlases or
    /// their metrics sidecars.
    pub fn touched_by(&self, dirty: &std::collections::HashSet<u64>) -> bool {
        Self::source_hashes().iter().any(|h| dirty.contains(h))
    }

    /// Rebuild all three fonts from the asset server.  On failure the
    /// previous fonts are preserved (so a malformed mid-write metrics
    /// JSON keeps the old glyphs until the next valid save).
    pub fn reload(
        &mut self,
        rl: &mut RaylibHandle,
        thread: &RaylibThread,
        assets: &crate::asset::AssetServer,
    ) -> anyhow::Result<()> {
        *self = Self::from_handles(rl, thread, assets)?;
        Ok(())
    }

    /// Draw `text` with the ASCII baseline at `y_baseline` (matching the
    /// old macroquad `draw_text_ex` convention) through any active raylib
    /// draw handle (backbuffer, render-texture mode, shader mode, …).
    pub fn draw_text<D: RaylibDraw>(
        &self,
        d: &mut D,
        text: &str,
        x: f32,
        y_baseline: f32,
        size_px: u16,
        color: Color,
        char_spacing: i32,
    ) {
        let f = self.pick(size_px);
        let cell_top = y_baseline - f.baseline;
        f.draw_cell_top(d, text, x, cell_top, color, char_spacing);
    }

    /// Pick the closest matching atlas for a requested pixel size.
    ///
    /// Empirical mapping derived from how the original game uses each
    /// CDSFont: the three atlases (`0x100b0` small, `0x100af` medium,
    /// `0x100ae` large) all carry roles spanning a range of caller-
    /// requested sizes, so we pick by the role boundary rather than
    /// purely by line-height distance.
    ///
    /// | Requested | Atlas    | Original-game role                       |
    /// |-----------|----------|------------------------------------------|
    /// | `≤ 12`    | small    | `CStaticText` w/ `0x100b0`: version line,|
    /// |           |          | copyright, lobby "Press start" hint      |
    /// | `13..=15` | medium   | `CRadio` labels (`0x100af`, char-spacing |
    /// |           |          | 2), `CListBox` rows, `CPoemScroller`,    |
    /// |           |          | `CEdit` content                          |
    /// | `≥ 16`    | large    | `CButton` (`0x100ae`, fixed in           |
    /// |           |          | `CButton::BuildAt`), every dialog header |
    /// |           |          | / heading, gameplay HUD strings          |
    pub fn pick(&self, size_px: u16) -> &BitmapFont {
        match size_px {
            0..=12 => &self.small,
            13..=15 => &self.medium,
            _ => &self.large,
        }
    }
}

impl FontProvider for BitmapFontProvider {
    fn measure(&self, text: &str, size_px: u16, char_spacing: i32) -> f32 {
        self.pick(size_px).measure(text, char_spacing)
    }

    fn line_height(&self, size_px: u16) -> f32 {
        self.pick(size_px).line_height as f32
    }

    fn ascent(&self, size_px: u16) -> f32 {
        self.pick(size_px).baseline
    }
}

// ============================================================================
// CP-1250 mapping
// ============================================================================

/// Map a Unicode scalar value to its CP-1250 byte encoding (the codepage
/// the original game uses — see `TextShaper_LayOutAndRender`, which
/// calls `WideCharToMultiByte_Wrapper` with the current thread ACP
/// before indexing into the glyph table).
///
/// Returns `b'?'` for code points that have no representation in
/// CP-1250. ASCII (`0..=0x7F`) maps to itself.
pub fn utf32_to_cp1250(cp: u32) -> u8 {
    if cp < 0x80 {
        return cp as u8;
    }
    // CP-1250 high-half (0x80..=0xFF). Generated from
    // https://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP1250.TXT.
    match cp {
        0x20AC => 0x80, 0x201A => 0x82, 0x201E => 0x84, 0x2026 => 0x85,
        0x2020 => 0x86, 0x2021 => 0x87, 0x2030 => 0x89, 0x0160 => 0x8A,
        0x2039 => 0x8B, 0x015A => 0x8C, 0x0164 => 0x8D, 0x017D => 0x8E,
        0x0179 => 0x8F,
        0x2018 => 0x91, 0x2019 => 0x92, 0x201C => 0x93, 0x201D => 0x94,
        0x2022 => 0x95, 0x2013 => 0x96, 0x2014 => 0x97, 0x2122 => 0x99,
        0x0161 => 0x9A, 0x203A => 0x9B, 0x015B => 0x9C, 0x0165 => 0x9D,
        0x017E => 0x9E, 0x017A => 0x9F,
        0x00A0 => 0xA0, 0x02C7 => 0xA1, 0x02D8 => 0xA2, 0x0141 => 0xA3,
        0x00A4 => 0xA4, 0x0104 => 0xA5, 0x00A6 => 0xA6, 0x00A7 => 0xA7,
        0x00A8 => 0xA8, 0x00A9 => 0xA9, 0x015E => 0xAA, 0x00AB => 0xAB,
        0x00AC => 0xAC, 0x00AD => 0xAD, 0x00AE => 0xAE, 0x017B => 0xAF,
        0x00B0 => 0xB0, 0x00B1 => 0xB1, 0x02DB => 0xB2, 0x0142 => 0xB3,
        0x00B4 => 0xB4, 0x00B5 => 0xB5, 0x00B6 => 0xB6, 0x00B7 => 0xB7,
        0x00B8 => 0xB8, 0x0105 => 0xB9, 0x015F => 0xBA, 0x00BB => 0xBB,
        0x013D => 0xBC, 0x02DD => 0xBD, 0x013E => 0xBE, 0x017C => 0xBF,
        0x0154 => 0xC0, 0x00C1 => 0xC1, 0x00C2 => 0xC2, 0x0102 => 0xC3,
        0x00C4 => 0xC4, 0x0139 => 0xC5, 0x0106 => 0xC6, 0x00C7 => 0xC7,
        0x010C => 0xC8, 0x00C9 => 0xC9, 0x0118 => 0xCA, 0x00CB => 0xCB,
        0x011A => 0xCC, 0x00CD => 0xCD, 0x00CE => 0xCE, 0x010E => 0xCF,
        0x0110 => 0xD0, 0x0143 => 0xD1, 0x0147 => 0xD2, 0x00D3 => 0xD3,
        0x00D4 => 0xD4, 0x0150 => 0xD5, 0x00D6 => 0xD6, 0x00D7 => 0xD7,
        0x0158 => 0xD8, 0x016E => 0xD9, 0x00DA => 0xDA, 0x0170 => 0xDB,
        0x00DC => 0xDC, 0x00DD => 0xDD, 0x0162 => 0xDE, 0x00DF => 0xDF,
        0x0155 => 0xE0, 0x00E1 => 0xE1, 0x00E2 => 0xE2, 0x0103 => 0xE3,
        0x00E4 => 0xE4, 0x013A => 0xE5, 0x0107 => 0xE6, 0x00E7 => 0xE7,
        0x010D => 0xE8, 0x00E9 => 0xE9, 0x0119 => 0xEA, 0x00EB => 0xEB,
        0x011B => 0xEC, 0x00ED => 0xED, 0x00EE => 0xEE, 0x010F => 0xEF,
        0x0111 => 0xF0, 0x0144 => 0xF1, 0x0148 => 0xF2, 0x00F3 => 0xF3,
        0x00F4 => 0xF4, 0x0151 => 0xF5, 0x00F6 => 0xF6, 0x00F7 => 0xF7,
        0x0159 => 0xF8, 0x016F => 0xF9, 0x00FA => 0xFA, 0x0171 => 0xFB,
        0x00FC => 0xFC, 0x00FD => 0xFD, 0x0163 => 0xFE, 0x02D9 => 0xFF,
        _ => b'?',
    }
}

#[cfg(test)]
#[path = "font_tests.rs"]
mod tests;
