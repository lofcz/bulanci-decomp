//! Rendering primitives + the per-phase render dispatcher.
//!
//! Holds the texture cache (`warm`/`texture`), the small set of
//! `ClientApp` text helpers every view uses (`draw_t`, `measure_t`), and
//! the top-level `draw()` orchestration. Drawing follows three idiomatic
//! raylib passes per frame:
//!
//!   * **Pass A** — rasterise the scrolling poem text into the 205×166
//!     `poem` render texture (menu phase only).
//!   * **Pass B** — render the whole frame into the 800×600 `logical`
//!     render texture, compositing the poem surface through the fade
//!     shader via a nested `begin_shader_mode`.
//!   * **Pass C** — blit `logical` (scaled, Y-flipped) to the backbuffer.
//!
//! Textures load with `&mut RaylibHandle` + `&RaylibThread`, which can't
//! happen inside a `begin_texture_mode` scope, so every texture is warmed
//! up-front (or on hot-reload) and the draw path only ever does immutable
//! cache lookups via [`ClientApp::texture`].

use raylib::prelude::*;

use crate::app::ClientApp;
use crate::asset::class::AssetClass;
use crate::asset::handle::{AssetHandle, RawHandle};
use crate::gfx::{blit, RenderResources};
use crate::state::AppPhase;

// ============================================================================
// Texture cache.
// ============================================================================

impl ClientApp {
    /// Decode + cache one texture identified by a typed [`AssetHandle`],
    /// unless it is already resident. The bytes go through
    /// [`crate::asset::AssetServer::bytes`], so a missing / hot-reloaded
    /// asset transparently falls back to the class-typed placeholder.
    ///
    /// `T: AssetClass` enforces that only image-bearing handles flow
    /// through here at compile time. Called outside the draw scope
    /// (startup warm + hot-reload), never mid-frame.
    pub(crate) fn warm<T: AssetClass>(
        &mut self,
        rl: &mut RaylibHandle,
        thread: &RaylibThread,
        handle: AssetHandle<T>,
    ) {
        let key = RawHandle::from(handle).0;
        if self.textures.contains_key(&key) {
            return;
        }
        let bytes = self.assets.bytes(handle);
        if let Some(tex) = crate::gfx::texture_from_encoded(
            rl,
            thread,
            &bytes,
            TextureFilter::TEXTURE_FILTER_POINT,
        ) {
            self.textures.insert(key, tex);
        }
    }

    /// Preload every texture the intro + menu + sub-screens draw, so the
    /// per-frame draw path is a pure immutable cache lookup. Re-run on
    /// hot-reload (skips already-resident entries).
    pub(crate) fn warm_textures(&mut self, rl: &mut RaylibHandle, thread: &RaylibThread) {
        use crate::generated::assets::menu;
        self.warm(rl, thread, menu::backgrounds::BG_MENU);
        self.warm(rl, thread, menu::backgrounds::BG_HERO);
        self.warm(rl, thread, menu::backgrounds::BG_START);
        self.warm(rl, thread, menu::backgrounds::BG_HISTORY);
        self.warm(rl, thread, menu::backgrounds::BG_QUIT);
        self.warm(rl, thread, menu::intro::INTRO_SPLASH);
        self.warm(rl, thread, menu::cursor::CURSOR_RETICLE);
        self.warm(rl, thread, menu::icons::ICON_START);
        self.warm(rl, thread, menu::icons::ICON_START_HI);
        self.warm(rl, thread, menu::icons::ICON_HISTORY);
        self.warm(rl, thread, menu::icons::ICON_HISTORY_HI);
        self.warm(rl, thread, menu::icons::ICON_QUIT);
        self.warm(rl, thread, menu::icons::ICON_QUIT_HI);
        self.warm(rl, thread, menu::widgets::radio::RADIO_SELECTED);
        self.warm(rl, thread, menu::widgets::radio::RADIO_UNSELECTED);
        self.warm(rl, thread, menu::widgets::radio::RADIO_ACTIVE);
        self.warm(rl, thread, menu::widgets::radio::RADIO_SELECTED_HOVER);
        self.warm(rl, thread, menu::widgets::radio::RADIO_UNSELECTED_HOVER);
        self.warm(rl, thread, menu::widgets::dialog_button::BTN_DIALOG_NORMAL);
        self.warm(rl, thread, menu::widgets::dialog_button::BTN_DIALOG_HOVER);
        self.warm(rl, thread, menu::widgets::dialog_button::BTN_DIALOG_PRESSED);
    }

    /// Immutable cache lookup, callable from any draw scope. Returns the
    /// magenta fallback texture for an un-warmed handle (which should not
    /// happen — every drawn handle is in [`Self::warm_textures`]).
    pub fn texture<T: AssetClass>(&self, handle: AssetHandle<T>) -> &Texture2D {
        let key = RawHandle::from(handle).0;
        self.textures.get(&key).unwrap_or(&self.fallback_texture)
    }
}

// ============================================================================
// Shared text helpers — used by every sub-screen module.
// ============================================================================

impl ClientApp {
    /// Draw a UTF-8 string through the active text provider, baseline at
    /// `y` (the old macroquad `draw_text_ex` convention). `char_spacing =
    /// 0` matches the default `CStaticText` widget.
    pub fn draw_t<D: RaylibDraw>(
        &self,
        d: &mut D,
        text: &str,
        x: f32,
        y: f32,
        font_size: u16,
        color: Color,
    ) {
        self.fonts.draw_text(d, text, x, y, font_size, color, 0);
    }

    /// `draw_t` with an explicit `char_spacing` (extra pixels between
    /// every pair of glyphs — `font_info[+4]` in the original shaper).
    pub fn draw_t_sp<D: RaylibDraw>(
        &self,
        d: &mut D,
        text: &str,
        x: f32,
        y: f32,
        font_size: u16,
        char_spacing: i32,
        color: Color,
    ) {
        self.fonts.draw_text(d, text, x, y, font_size, color, char_spacing);
    }

    #[allow(dead_code)] // shared text helper; kept for gameplay/HUD callers.
    pub fn measure_t(&self, text: &str, font_size: u16) -> f32 {
        self.fonts.pick(font_size).measure(text, 0)
    }
}

// ============================================================================
// Top-level render dispatcher.
// ============================================================================

impl ClientApp {
    pub fn draw(&self, rl: &mut RaylibHandle, thread: &RaylibThread, res: &mut RenderResources) {
        let mut d = rl.begin_drawing(thread);

        // -------- Pass B: render the whole logical frame. --------
        {
            let mut tm = d.begin_texture_mode(thread, &mut res.logical);
            tm.clear_background(Color::BLACK);
            match self.phase {
                // Intro / Menu / Connecting are all the surface stack now: the
                // menu base plus whatever overlay the phase pushed (the intro
                // splash or the connecting screen). Gameplay is still its own
                // immediate-mode pass (migrated in a later phase).
                AppPhase::Intro | AppPhase::Menu | AppPhase::Connecting => {
                    self.draw_surface_stack(&mut tm)
                }
                AppPhase::Playing => self.draw_gameplay(&mut tm),
            }
        }

        // -------- Pass C: blit the logical surface to the backbuffer,
        //          scaled to the window and Y-flipped (render textures are
        //          stored upside-down). --------
        let sw = d.get_screen_width() as f32;
        let sh = d.get_screen_height() as f32;
        d.clear_background(Color::BLACK);
        blit(
            &mut d,
            res.logical.texture(),
            0.0,
            0.0,
            None,
            Some((sw, sh)),
            true,
            Color::WHITE,
        );
    }

    /// Render the active surface stack: every screen (the menu's backgrounds,
    /// dials, poem, version line, ruch, sub-screens, cursor — plus the intro
    /// splash / connecting overlays) is a surface's composited draw list, which
    /// Rust walks through the shared font/texture helpers, bottom→top (base
    /// then overlays) for painter order. If the base-UI pak isn't mounted the
    /// stack is empty, so we fall back to the bare background rather than a
    /// black frame.
    fn draw_surface_stack<D: RaylibDraw>(&self, d: &mut D) {
        if self.surfaces.is_empty() {
            blit(
                d,
                self.texture(crate::generated::assets::menu::backgrounds::BG_MENU),
                0.0,
                0.0,
                None,
                Some((800.0, 600.0)),
                false,
                Color::WHITE,
            );
            return;
        }
        for surface in self.surfaces.iter() {
            crate::scene_runtime::draw_items(self, &surface.draw_list(), d);
        }
    }
}
