//! Rendering primitives + the per-phase render dispatcher.
//!
//! Holds the small set of `ClientApp` helpers every other view uses
//! (`draw_t`, `measure_t`, `get_texture`) plus the top-level `draw()`
//! switch and the main-menu chrome layers (background, dial buttons,
//! poem scroller).

use macroquad::prelude::*;

use crate::app::ClientApp;
use crate::font::FontProvider;
use crate::poem::Align;
use crate::state::{AppPhase, MenuBtn, SubScreen};

// ============================================================================
// Shared draw helpers — used by every sub-screen module.
// ============================================================================

impl ClientApp {
    /// Get-or-load a texture from the VFS. Subsequent calls for the same
    /// path return the cached copy.
    ///
    /// The cursor dot atlases (`cursor/track_65801..65803.png`,
    /// BitmapSprite resources 65801..65803) ship as horizontal frame
    /// strips with pre-baked alpha = 0 on the background; see
    /// `tools/bulanci_unpack/bulanci_unpack.py` `_resolve_bg_key_indices`
    /// for the chroma-key resolution path that produces them (opcode
    /// 0x0D's transparent index resolves to the uninitialised palette
    /// slot 100, so the unpacker falls back to a perimeter-vote chroma
    /// key on palette[0] = black). These are loaded via
    /// [`crate::cursor_atlas::CursorAtlasSet`] at boot and consumed
    /// directly by `cursor::GunMouse::draw`, not through `get_texture`.
    pub fn get_texture(&mut self, path: &str) -> Texture2D {
        if let Some(t) = self.textures.get(path) {
            return t.clone();
        }
        let bytes = self.vfs.read(path).unwrap_or_else(|| panic!("missing asset: {}", path));
        let tex = Texture2D::from_file_with_format(bytes, None);
        // Crisp pixel art — no blurring on the dial sprites.
        tex.set_filter(FilterMode::Nearest);
        self.textures.insert(path.to_string(), tex.clone());
        tex
    }

    /// Draw a UTF-8 string with the active text provider (see
    /// [`crate::font::FontProvider`]). `y` is the baseline, matching
    /// macroquad's `draw_text_ex` convention so existing call-sites
    /// don't need to change.
    ///
    /// Uses `char_spacing = 0` to match the original game's default
    /// `CStaticText` widget. Callers that need a tighter or looser
    /// inter-glyph gap (CButton = 1, CRadio = 2, CPoemScroller = 1)
    /// should use [`ClientApp::draw_t_sp`] with the same value the
    /// matching original widget sets in `font_info[+4]`.
    pub fn draw_t(&self, text: &str, x: f32, y: f32, font_size: u16, color: Color) {
        self.fonts.draw(text, x, y, font_size, color, 0);
    }

    /// `draw_t` with an explicit `char_spacing` (extra pixels between
    /// every pair of glyphs). Mirrors the `font_info[+4]` field the
    /// original `TextShaper_LayOutAndRender` consults.
    pub fn draw_t_sp(
        &self,
        text: &str,
        x: f32,
        y: f32,
        font_size: u16,
        char_spacing: i32,
        color: Color,
    ) {
        self.fonts.draw(text, x, y, font_size, color, char_spacing);
    }

    pub fn measure_t(&self, text: &str, font_size: u16) -> f32 {
        self.fonts.measure(text, font_size, 0)
    }

    /// `measure_t` with the same `char_spacing` value passed to
    /// [`ClientApp::draw_t_sp`] — required so per-line centring math
    /// agrees with what `draw_t_sp` will eventually emit.
    pub fn measure_t_sp(&self, text: &str, font_size: u16, char_spacing: i32) -> f32 {
        self.fonts.measure(text, font_size, char_spacing)
    }
}

/// Light grey from the original engine's default text style block
/// `DAT_004ae418`: `[0x000000, 0xCECECE, 0xADADAD, 0x949494]`. Index 1
/// (`0xCECECE`) is the body / primary text colour — widgets like
/// `CRadio` and `CButton` that don't override the tint in their
/// `Render` route their text through this colour. We pull it out as a
/// constant so future widget-port code can reference the same source
/// of truth.
pub const TEXT_COLOR_PRIMARY: Color = Color {
    r: 0xCE as f32 / 255.0,
    g: 0xCE as f32 / 255.0,
    b: 0xCE as f32 / 255.0,
    a: 1.0,
};

/// Darker grey forced by `CStaticText::Render @ 0x004054d0`: every
/// `CStaticText` whose `+0x94` "force-default-colour" flag is `0` (the
/// default) has its `font_info[+0x10]` (outline-colour slot) overwritten
/// with `0x909090` on every frame, before the call to
/// `TextShaper_LayOutAndRender`. The shaper then drives `CDSFont::DrawChar`
/// in "with-outline" mode (`uStack_10438 = 0xd`) and the visible result
/// is the dimmer grey the original game uses for every menu / dialog
/// caption (`Hru:`, `Počet hráčů celkem:`, the version line, etc.).
pub const TEXT_COLOR_STATIC: Color = Color {
    r: 0x90 as f32 / 255.0,
    g: 0x90 as f32 / 255.0,
    b: 0x90 as f32 / 255.0,
    a: 1.0,
};

// ============================================================================
// Top-level render dispatcher.
// ============================================================================

impl ClientApp {
    pub fn draw(&mut self) {
        crate::window_mode::begin_logical_frame(self.logical_render_target.clone());
        clear_background(BLACK);
        match self.phase {
            AppPhase::Intro => self.draw_intro(),
            AppPhase::Menu => self.draw_menu(),
            AppPhase::Connecting => self.draw_connecting(),
            AppPhase::Playing => self.draw_gameplay(),
        }
        crate::window_mode::end_logical_frame(&self.logical_render_target.texture);
    }

    fn draw_menu(&mut self) {
        // ---- Layer 1: full-screen menu background (resource 0x1013b). ----
        let bg_menu = self.get_texture("images/bg_menu.jpg");
        draw_texture_ex(
            &bg_menu,
            0.0,
            0.0,
            WHITE,
            DrawTextureParams {
                dest_size: Some(vec2(800.0, 600.0)),
                ..Default::default()
            },
        );

        // ---- Layer 2: right-side hero art (resource 0x10139) at (610, 0). ----
        // We always show the right-side hero art / Sleep Team logo so it is drawn
        // on top of the rounded white frame border of bg_menu.jpg.
        let bg_hero = self.get_texture("images/bg_hero.jpg");
        draw_texture(&bg_hero, 610.0, 0.0, WHITE);

        // ---- Layer 3: three main-menu buttons (dial + label per row). ----
        self.draw_menu_button(MenuBtn::Start);
        self.draw_menu_button(MenuBtn::History);
        self.draw_menu_button(MenuBtn::Quit);

        // ---- Layer 4: CPoemScroller in the lower-left (10, 384, 215, 550). ----
        self.draw_poem_scroller();

        // ---- Layer 5: bottom version line. ----
        // From the actual decomp of `CMenu_ctor_with_ui @ 0x004265e0`:
        //   uVar11 = 0x100b0;   // font_id        → param_5
        //   uVar10 = 1;         // char_spacing   → param_4
        //   uVar9  = 0;         // smartptr stash → param_3
        //   CStaticText_BuildAtAuto(this_00, 0xe6, 0x23f,
        //                           uVar9, uVar10, uVar11);
        //
        // → bounding-box top-left = (0xe6, 0x23f) = (230, 575),
        //   font = `0x100b0` (small, 11 px), `char_spacing = 1`.
        //
        // Because this is a `CStaticText`, its `Render` overrides
        // `font_info[+0x10]` (outline colour) to `0x909090` every
        // frame, which `TextShaper_LayOutAndRender` then drives through
        // `CDSFont::DrawChar` in with-outline mode (`uStack_10438 = 0xd`).
        // We approximate that by tinting through `TEXT_COLOR_STATIC`
        // instead of the brighter `TEXT_COLOR_PRIMARY`. `draw_t_sp`
        // takes the ASCII baseline, so y = 575 + small-font ascent
        // (= 9 for capital A) = 584.
        self.draw_t_sp("Verze: 1.80", 230.0, 584.0, 11, 1, TEXT_COLOR_STATIC);

        // ---- Layer 6: sub-screen overlay (NOT modal — main buttons stay live). ----
        // The original sub-screens are dialog `CWindow` widgets whose
        // backgrounds (`bg_start.jpg = 0x1013c`, `bg_history.jpg = 0x1013e`,
        // `bg_quit.jpg = 0x1013d`) are swapped *into* the menu's existing
        // title bitmap slot (see main_menu.md §4.2 "load bg 0x1013c"),
        // NOT layered on top as fresh top-Z children. So in the engine's
        // child-render tree these backgrounds sit at the *same* Z as
        // `bg_menu` itself — well underneath the ruch.
        match self.sub_screen {
            SubScreen::None => {}
            SubScreen::StartGame1 => self.draw_startgame1(),
            SubScreen::History => self.draw_history(),
            SubScreen::ExitConfirm => self.draw_exit_confirm(),
        }

        // ---- Layer 7: CRuch phosphor scanlines (4 instances). ----
        // Per the `CMenu` ctor (main_menu.md §1 lines 273-277), the four
        // `CRuch` actors are the **last** children added to the menu, so
        // in the original engine they paint over every menu element —
        // including the sub-screen background swaps above. Keep this draw
        // call right before the cursor so the scanlines correctly flash
        // across the bottom-right `bg_start` / `bg_history` / `bg_quit`
        // panels too.
        //
        // Vertical span: `CRuch_ctor @ 0x00423c20` stores
        // `rect.bottom = g_pApp[0x2c] - g_pApp[0x24]` (= screen height),
        // and `CRuch_Render @ 0x00423e60` passes `rect.top..rect.bottom`
        // directly to `CDSImage::DrawVerticalLine`. So every ruch column
        // spans the full `[0, SCREEN_H)` range; we mirror that by
        // passing `SCREEN_H` through to `ruch.draw`.
        for ruch in &self.ruch {
            ruch.draw(crate::cursor::SCREEN_H as f32);
        }

        // ---- Layer 8 (topmost): CGunMouse software cursor. ----
        // The dot's per-blit alpha is read by the engine from the
        // sprite resource's `+0x20` field (see the docstring on
        // `cursor::GunMouse::draw` for the Ghidra trail) and ends up
        // **at most equal to** the 50 %-α crosshair-line alpha — so
        // we pass `LINE_ALPHA` to give the whole red overlay (arms
        // + ring + dot) a single consistent translucency. The dot's
        // sprite strip + per-tick (Δx, Δy) deltas come from
        // `self.cursor_atlas`, which `gun_mouse` reads to keep the
        // visual frame and motion offset in lock-step.
        let reticle = self.get_texture("images/cursor_reticle.png");
        self.gun_mouse
            .draw(&reticle, &self.cursor_atlas, crate::cursor::LINE_ALPHA);
    }

    fn draw_menu_button(&mut self, btn: MenuBtn) {
        crate::widget::MenuButtonWidget::draw(self, btn);
    }

    fn draw_poem_scroller(&self) {
        // Faithful port of `CPoemScroller::Render` (FUN_004240f0, §9.3 of
        // `main_menu.md`). The original engine:
        //   1. Rasterises the whole poem string into a CDSBmpImage
        //      surface using the medium CDSFont (resource `0x100af`,
        //      confirmed in `CPoemScroller::Constructor @ 0x004262c0`
        //      via the `LoadResource(0x100af, 0)` call that fills
        //      `this[+0x2a]`).
        //   2. Splits the visible (10, 384, 215, 550) rect into top-fade /
        //      middle / bottom-fade bands and blits each band onto the
        //      backbuffer, multiplying the top and bottom bands by the
        //      pre-baked 40-row alpha gradients at `this[+0x118]` /
        //      `this[+0x11c]`.
        //
        // We do exactly the same with: (a) a 205×166 `RenderTarget` that
        // mirrors the original's CDSBmpImage; (b) a custom fragment shader
        // that applies the 40-row top/bottom alpha ramps per-pixel as
        // the surface is composited onto the screen. With the bitmap
        // CDSFont in place the text is now genuinely 1:1 with the original.
        const RECT_X: f32 = 10.0;
        const RECT_Y: f32 = 384.0;
        const RECT_W: f32 = 205.0;
        const RECT_H: f32 = 166.0;

        if self.poems.is_empty() {
            return;
        }
        let lines = &self.poems[self.current_poem_idx];

        // The poem scroller is hard-coded to the medium CDSFont
        // (`0x100af`, 13 px line height) by the original `CPoemScroller`
        // constructor. We mirror that and grab the same atlas directly,
        // bypassing the provider's size dispatch.
        let font = &self.fonts.medium;
        let line_h = font.line_height as f32;

        // -------- Pass 1: rasterise the visible poem lines into the
        // offscreen surface. We bind a `Camera2D` whose visible rect
        // maps 1:1 to the render target's logical pixels, so subsequent
        // bitmap-blit calls land at the same coordinates the original
        // engine's blitter uses.
        let mut camera = Camera2D::from_display_rect(Rect::new(0.0, 0.0, RECT_W, RECT_H));
        camera.render_target = Some(self.poem_render_target.clone());
        set_camera(&camera);
        clear_background(Color::new(0.0, 0.0, 0.0, 0.0));

        // Per `CPoemScroller::Constructor @ 0x004262c0`:
        //   param_1[+0x26] = 1 → char_spacing = 1
        //   param_1[+0x49] = 0xB0B0B0 → primary_color = light grey
        // We pass the same values through the bitmap renderer so the
        // measure-step's centring math agrees with the draw-step's pen.
        const POEM_CHAR_SPACING: i32 = 1;
        let text_color = Color::new(0xB0 as f32 / 255.0, 0xB0 as f32 / 255.0, 0xB0 as f32 / 255.0, 1.0);
        let mut y_rt = self.poem_scroll_y - RECT_Y;
        for line in lines {
            if y_rt + line_h >= 0.0 && y_rt <= RECT_H {
                let text_w = font.measure(&line.text, POEM_CHAR_SPACING);
                let x = match line.align {
                    Align::Left => 2.0,
                    Align::Center => ((RECT_W - text_w) * 0.5).floor(),
                    Align::Right => (RECT_W - text_w - 2.0).floor(),
                };
                font.draw_cell_top(&line.text, x, y_rt.floor(), text_color, POEM_CHAR_SPACING);
            }
            y_rt += line_h;
        }
        crate::window_mode::begin_logical_frame(self.logical_render_target.clone());

        // -------- Pass 2: composite the offscreen surface onto the
        // backbuffer at (10, 384) through the fade-mask material. The
        // material's fragment shader multiplies α by the original
        // engine's 40-row top/bottom gradient (linear 0→1 across
        // uv.y ∈ [0, 40/166], 1→0 across uv.y ∈ [126/166, 1]).
        // `flip_y` undoes macroquad's render-target Y-inversion.
        gl_use_material(&self.poem_material);
        draw_texture_ex(
            &self.poem_render_target.texture,
            RECT_X,
            RECT_Y,
            WHITE,
            DrawTextureParams {
                dest_size: Some(vec2(RECT_W, RECT_H)),
                flip_y: true,
                ..Default::default()
            },
        );
        gl_use_default_material();
    }

    fn draw_connecting(&mut self) {
        let bg_menu = self.get_texture("images/bg_menu.jpg");
        draw_texture_ex(&bg_menu, 0.0, 0.0, WHITE, DrawTextureParams {
            dest_size: Some(vec2(800.0, 600.0)),
            ..Default::default()
        });
        let title = "Připojování k serveru…";
        let tw = self.measure_t(title, 22);
        self.draw_t(title, (800.0 - tw) * 0.5, 260.0, 22, Color::new(1.0, 0.85, 0.4, 1.0));
        let sub = "Esc pro zrušení";
        let sw = self.measure_t(sub, 14);
        self.draw_t(sub, (800.0 - sw) * 0.5, 300.0, 14, Color::new(0.7, 0.7, 0.7, 1.0));
    }
}
