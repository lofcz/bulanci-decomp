//! Data-driven menu runtime — the only menu path.
//!
//! The whole `AppPhase::Menu` screen (backgrounds, the three dials + their
//! glow/dim state machine, the poem scroller, the ruch scanlines, the
//! software cursor, and the StartGame1 / History / ExitConfirm sub-screens)
//! lives in Luau under `paks/base_ui`. Rust keeps only:
//!   * rendering primitives — [`draw_scene`] walks the scene's draw list and
//!     blits real textures / atlas frames / bitmap text;
//!   * input plumbing — pointer + mouse + keyboard + typed chars fed in via
//!     [`SceneMenu::tick`];
//!   * client services — [`build_services`] injects font measuring, VFS text
//!     reads, image sizes, and atlas frame metadata so the script can lay
//!     itself out without the engine core ever touching raylib;
//!   * lifecycle hooks — `bulanci.net.connect` / `bulanci.app.exit` are
//!     drained as [`HostCommand`]s the client acts on.
//!
//! The scene is a pak asset addressed by `blake2b("scenes/main_menu")`, so an
//! editor push (or a `dev_overrides` file) hot-reloads it like any other
//! asset — [`crate::app::ClientApp::apply_hot_reload`] rebuilds the host when
//! the scene or any of its libraries goes dirty.

use std::collections::HashMap;

use raylib::prelude::*;

use bulanci_core::scene::{AtlasInfo, DrawItem, HostServices, NodeKind, SceneManager, TextAnchor};

use crate::app::ClientApp;
use crate::atlas::Atlas;
use crate::gfx::blit;

/// The scene the client boots into.
pub const MENU_SCENE_NAME: &str = "main_menu";
/// Canonical VFS path of the legacy menu module (still a hot-reload fallback
/// candidate when no scene is loaded yet).
pub const SCENE_MENU_PATH: &str = "scenes/main_menu";
/// Library modules pre-loaded so scenes can `require("@name")`. All are
/// optional — a self-contained (editor-emitted) scene can skip any of them.
pub const WIDGETS_LIB_PATH: &str = "lib/widgets.luau";
pub const FX_LIB_PATH: &str = "lib/fx.luau";
pub const CURSOR_LIB_PATH: &str = "lib/cursor.luau";
pub const POEM_LIB_PATH: &str = "lib/poem.luau";
pub const SG1_LIB_PATH: &str = "lib/startgame1.luau";

/// The library modules every scene may `require`, preloaded by the router.
pub const SCENE_LIBS: &[(&str, &str)] = &[
    ("widgets", WIDGETS_LIB_PATH),
    ("fx", FX_LIB_PATH),
    ("cursor", CURSOR_LIB_PATH),
    ("poem", POEM_LIB_PATH),
    ("startgame1", SG1_LIB_PATH),
];

/// Fallback hot-reload candidates used only before a scene has loaded (so a
/// freshly-mounted base-UI pak boots the menu). Once a scene is live the
/// client uses [`SceneManager::deps`] instead, which also covers its scripts.
pub const HOT_RELOAD_PATHS: &[&str] = &[
    SCENE_MENU_PATH,
    WIDGETS_LIB_PATH,
    FX_LIB_PATH,
    CURSOR_LIB_PATH,
    POEM_LIB_PATH,
    SG1_LIB_PATH,
];

/// Load any scene by name through the core [`SceneManager`], wiring the
/// client's VFS reader, library set, and freshly-captured services. `None`
/// if the scene isn't mounted / fails to build.
pub fn load_scene(app: &ClientApp, name: &str) -> Option<SceneManager> {
    let services = build_services(app);
    // The reader owns an Arc clone of the asset stack, so it stays valid for
    // the (short) duration of the load without borrowing `app`.
    let assets = app.assets.clone();
    let read = move |p: &str| assets.vfs().read(p);
    SceneManager::load(name, &read, SCENE_LIBS, services)
}

/// Boot the main menu scene.
pub fn load_menu(app: &ClientApp) -> Option<SceneManager> {
    load_scene(app, MENU_SCENE_NAME)
}

/// Build the host service closures from the client's already-loaded asset
/// stack. Each closure owns its data (cloned tables / Arc), so it stays
/// `'static` for the lifetime of the Luau VM.
fn build_services(app: &ClientApp) -> HostServices {
    // --- measure: reproduce ClientApp::measure_t_sp without the fonts. ---
    let small = app.fonts.small.advances();
    let medium = app.fonts.medium.advances();
    let large = app.fonts.large.advances();
    let measure = move |text: &str, size: f32, spacing: i32| -> f32 {
        let fs = size.round().max(1.0) as u16;
        let adv = match fs {
            0..=12 => &small,
            13..=15 => &medium,
            _ => &large,
        };
        let mut x = 0i32;
        for ch in text.chars() {
            let b = crate::font::utf32_to_cp1250(ch as u32);
            x += adv[b as usize] + spacing;
        }
        x.max(0) as f32
    };

    // --- read_text: VFS text assets (poems). ---
    let assets = app.assets.clone();
    let read_text = move |path: &str| -> Option<String> {
        assets.vfs().read_to_string(path).and_then(|r| r.ok())
    };

    // --- font_metrics: (line_height, baseline) of the size-picked font. ---
    let small_m = (app.fonts.small.line_height as f32, app.fonts.small.baseline as f32);
    let medium_m = (app.fonts.medium.line_height as f32, app.fonts.medium.baseline as f32);
    let large_m = (app.fonts.large.line_height as f32, app.fonts.large.baseline as f32);
    let font_metrics = move |size: f32| -> (f32, f32) {
        let fs = size.round().max(1.0) as u16;
        match fs {
            0..=12 => small_m,
            13..=15 => medium_m,
            _ => large_m,
        }
    };

    // --- image_size: dims of every warmed texture, keyed by raw handle. ---
    let mut sizes: HashMap<u64, (f32, f32)> = HashMap::new();
    for (k, tex) in app.textures.iter() {
        let t = tex.as_ref();
        sizes.insert(*k, (t.width as f32, t.height as f32));
    }
    let image_size = move |handle: u64| -> Option<(f32, f32)> { sizes.get(&handle).copied() };

    // --- atlas_info: cursor tracks + dial atlases (frame count/origin/motion). ---
    use crate::generated::assets::menu::{cursor as cur_h, widgets::menu_button as btn_h};
    let mut atlases: HashMap<u64, AtlasInfo> = HashMap::new();
    let cursor_handles = [cur_h::CURSOR_IDLE_A.raw(), cur_h::CURSOR_IDLE_B.raw(), cur_h::CURSOR_IDLE_C.raw()];
    for (i, &h) in cursor_handles.iter().enumerate() {
        if let Some(a) = app.cursor_atlas.atlases().get(i) {
            atlases.insert(h, atlas_to_info(a));
        }
    }
    atlases.insert(btn_h::MENU_BUTTON_OFF.raw(), atlas_to_info(&app.menu_btn_off));
    atlases.insert(btn_h::MENU_BUTTON_ON.raw(), atlas_to_info(&app.menu_btn_on));
    let atlas_info = move |handle: u64| -> Option<AtlasInfo> { atlases.get(&handle).cloned() };

    HostServices {
        measure: Some(std::rc::Rc::new(measure)),
        read_text: Some(std::rc::Rc::new(read_text)),
        font_metrics: Some(std::rc::Rc::new(font_metrics)),
        image_size: Some(std::rc::Rc::new(image_size)),
        atlas_info: Some(std::rc::Rc::new(atlas_info)),
    }
}

fn atlas_to_info(a: &Atlas) -> AtlasInfo {
    let (ox, oy) = a.frames.first().map(|f| (f.origin.0 as f32, f.origin.1 as f32)).unwrap_or((0.0, 0.0));
    let motion = a
        .frames
        .iter()
        .map(|f| {
            let (dx, dy) = crate::cursor_atlas::frame_motion(f);
            (dx as f32, dy as f32)
        })
        .collect();
    AtlasInfo { frame_count: a.frame_count() as u32, origin_x: ox, origin_y: oy, motion }
}

/// Unpack an `0xRRGGBBAA` scene color into a raylib [`Color`].
fn rgba(c: u32) -> Color {
    Color::new(((c >> 24) & 0xFF) as u8, ((c >> 16) & 0xFF) as u8, ((c >> 8) & 0xFF) as u8, (c & 0xFF) as u8)
}

/// Resolve a scene sprite handle to one of the client's loaded atlases (dial
/// frame strips, cursor idle tracks). `None` means it's a plain texture.
fn atlas_for(app: &ClientApp, handle: u64) -> Option<&Atlas> {
    use crate::generated::assets::menu::{cursor as cur_h, widgets::menu_button as btn_h};
    if handle == btn_h::MENU_BUTTON_OFF.raw() {
        return Some(&app.menu_btn_off);
    }
    if handle == btn_h::MENU_BUTTON_ON.raw() {
        return Some(&app.menu_btn_on);
    }
    let tracks = app.cursor_atlas.atlases();
    if handle == cur_h::CURSOR_IDLE_A.raw() {
        return tracks.first();
    }
    if handle == cur_h::CURSOR_IDLE_B.raw() {
        return tracks.get(1);
    }
    if handle == cur_h::CURSOR_IDLE_C.raw() {
        return tracks.get(2);
    }
    None
}

/// Render one surface's flattened draw list through the client's font/texture
/// helpers. Sprites resolve to real atlas frames or plain textures (honoring
/// world scale + tint); text routes through the bitmap-font provider at the
/// node's anchor; quads are solid rects. The caller composites the surface
/// stack by walking it bottom→top and calling this per surface.
pub fn draw_items<D: RaylibDraw>(app: &ClientApp, items: &[DrawItem], d: &mut D) {
    for item in items {
        let w = item.world;
        let tint = rgba(item.tint);
        match &item.kind {
            NodeKind::Quad { w: qw, h: qh, color } => {
                let mut col = rgba(*color);
                // Multiply the quad color by the node tint alpha so a behavior
                // can fade a quad via set_tint without rebuilding its color.
                if item.tint != 0xFFFF_FFFF {
                    col.a = ((col.a as u32 * tint.a as u32) / 255) as u8;
                }
                d.draw_rectangle(w.x as i32, w.y as i32, (qw * w.sx).round() as i32, (qh * w.sy).round() as i32, col);
            }
            NodeKind::Circle { radius, color } => {
                let mut col = rgba(*color);
                if item.tint != 0xFFFF_FFFF {
                    col.a = ((col.a as u32 * tint.a as u32) / 255) as u8;
                }
                d.draw_circle(w.x as i32, w.y as i32, radius * w.sx, col);
            }
            NodeKind::Line { dx, dy, thickness, color } => {
                let mut col = rgba(*color);
                if item.tint != 0xFFFF_FFFF {
                    col.a = ((col.a as u32 * tint.a as u32) / 255) as u8;
                }
                d.draw_line_ex(
                    Vector2::new(w.x, w.y),
                    Vector2::new(w.x + dx * w.sx, w.y + dy * w.sy),
                    *thickness,
                    col,
                );
            }
            NodeKind::Text { text, size, spacing, anchor } => {
                let fs = size.round().max(1.0) as u16;
                match anchor {
                    TextAnchor::Baseline => app.draw_t_sp(d, text, w.x, w.y, fs, *spacing, tint),
                    TextAnchor::Top => app.fonts.pick(fs).draw_cell_top(d, text, w.x, w.y, tint, *spacing),
                }
            }
            NodeKind::Sprite { handle, frame } => {
                if let Some(atlas) = atlas_for(app, *handle) {
                    let n = atlas.frame_count();
                    if n > 0 {
                        let fi = (*frame as usize).min(n - 1);
                        let r = atlas.frames[fi].rect;
                        let (fw, fh) = (r.w as f32, r.h as f32);
                        blit(
                            d,
                            atlas.texture(),
                            w.x,
                            w.y,
                            Some(Rectangle::new(r.x as f32, r.y as f32, fw, fh)),
                            Some((fw * w.sx, fh * w.sy)),
                            false,
                            tint,
                        );
                    }
                } else if let Some(tex) = app.textures.get(handle) {
                    let (tw, th) = (tex.width as f32, tex.height as f32);
                    blit(d, tex, w.x, w.y, None, Some((tw * w.sx, th * w.sy)), false, tint);
                } else {
                    // Un-warmed handle: a visible marker beats a silent gap.
                    d.draw_rectangle_lines(w.x as i32, w.y as i32, 32, 32, Color::new(180, 150, 220, 160));
                }
            }
            NodeKind::Group | NodeKind::InputRegion { .. } => {}
        }
    }
}
