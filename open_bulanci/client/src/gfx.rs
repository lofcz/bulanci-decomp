//! Idiomatic-raylib rendering scaffolding shared by every screen module.
//!
//! Holds the whole-frame 800×600 offscreen [`RenderTexture2D`] the game
//! renders through. It lives *outside* [`crate::app::ClientApp`] so the
//! per-frame `&self` draw path can borrow the app immutably while
//! `rl.begin_texture_mode(&mut res.logical)` holds a disjoint mutable
//! borrow of the render target.
//!
//! It also carries the small set of cross-cutting draw helpers:
//!   * [`blit`] — the `draw_texture_pro` analog of macroquad's
//!     `draw_texture_ex(DrawTextureParams { source, dest_size, flip_y })`.
//!   * [`logical_mouse_position`] / [`logical_scale`] — map the OS cursor
//!     into the fixed 800×600 logical space.

use raylib::prelude::*;

use crate::window_init::{CLIENT_HEIGHT, CLIENT_WIDTH};

/// Offscreen render target. Owned by the game loop, passed `&mut` into
/// [`crate::app::ClientApp::draw`].
pub struct RenderResources {
    /// Whole-frame 800×600 surface. Everything is rendered here at logical
    /// coordinates, then blitted (scaled) to the backbuffer once per frame.
    pub logical: RenderTexture2D,
}

/// Upload raw RGBA8 pixels (`width * height * 4` bytes) to a new GPU texture.
///
/// This is the one place that knows the cross-platform texture-upload quirk:
/// we allocate a blank texture and push pixels with
/// [`RaylibTexture2D::update_texture`] rather than
/// [`RaylibHandle::load_texture_from_image`]. On emscripten/WebGL the latter
/// fails for the game's NPOT sheets (the cursor atlas, font atlases) even
/// after `InitWindow()`; the blank-allocate-then-update path works on every
/// backend. Every texture in the client funnels through here.
///
/// Returns `None` if the pixel buffer is the wrong size or the GPU upload
/// fails.
pub fn texture_from_rgba8(
    rl: &mut RaylibHandle,
    thread: &RaylibThread,
    width: u32,
    height: u32,
    rgba: &[u8],
    filter: TextureFilter,
) -> Option<Texture2D> {
    if rgba.len() != (width as usize) * (height as usize) * 4 {
        return None;
    }
    let blank = Image::gen_image_color(width as i32, height as i32, Color::new(0, 0, 0, 0));
    let mut tex = rl.load_texture_from_image(thread, &blank).ok()?;
    tex.update_texture(rgba).ok()?;
    tex.set_texture_filter(thread, filter);
    Some(tex)
}

/// Decode PNG/JPEG (or any other `image`-supported) bytes to RGBA8 and upload
/// them to the GPU via [`texture_from_rgba8`].
pub fn texture_from_encoded(
    rl: &mut RaylibHandle,
    thread: &RaylibThread,
    bytes: &[u8],
    filter: TextureFilter,
) -> Option<Texture2D> {
    let decoded = image::load_from_memory(bytes).ok()?.to_rgba8();
    let (w, h) = decoded.dimensions();
    let rgba = decoded.into_raw();
    texture_from_rgba8(rl, thread, w, h, &rgba, filter)
}

impl RenderResources {
    pub fn new(rl: &mut RaylibHandle, thread: &RaylibThread) -> Self {
        let logical = rl
            .load_render_texture(thread, CLIENT_WIDTH, CLIENT_HEIGHT)
            .expect("logical render texture");
        // Linear so the whole-frame upscale to the window stays smooth.
        logical
            .texture()
            .set_texture_filter(thread, TextureFilter::TEXTURE_FILTER_BILINEAR);

        Self { logical }
    }
}

/// `draw_texture_ex`-style blit: draw `tex` at `(x, y)` with an optional
/// `source` sub-rect, an optional explicit destination size, and an
/// optional vertical flip (negative source height — used to undo raylib's
/// render-texture Y-inversion). Replaces macroquad's
/// `draw_texture_ex(tex, x, y, tint, DrawTextureParams { .. })`.
pub fn blit<D: RaylibDraw>(
    d: &mut D,
    tex: impl AsRef<ffi::Texture2D>,
    x: f32,
    y: f32,
    source: Option<Rectangle>,
    dest_size: Option<(f32, f32)>,
    flip_y: bool,
    tint: Color,
) {
    // `ffi::Texture2D` is `Copy`, so snapshot the dims and end the borrow
    // before handing `tex` to `draw_texture_pro` by value.
    let t = *tex.as_ref();
    let mut src = source.unwrap_or(Rectangle::new(0.0, 0.0, t.width as f32, t.height as f32));
    let (dw, dh) = dest_size.unwrap_or((src.width.abs(), src.height.abs()));
    if flip_y {
        src.height = -src.height;
    }
    d.draw_texture_pro(
        tex,
        src,
        Rectangle::new(x, y, dw, dh),
        Vector2::new(0.0, 0.0),
        0.0,
        tint,
    );
}

/// Map the OS cursor position into the fixed 800×600 logical space the UI
/// is authored in. Sampled once per frame in `ClientApp::update` and
/// cached, so the draw path never touches input.
pub fn logical_mouse_position(rl: &RaylibHandle) -> (f32, f32) {
    let m = rl.get_mouse_position();
    let (sx, sy) = logical_scale(rl);
    (m.x / sx, m.y / sy)
}

fn logical_scale(rl: &RaylibHandle) -> (f32, f32) {
    let sw = (rl.get_screen_width() as f32).max(1.0);
    let sh = (rl.get_screen_height() as f32).max(1.0);
    (sw / CLIENT_WIDTH as f32, sh / CLIENT_HEIGHT as f32)
}
