//! OpenBulanci graphical client binary.
//!
//! `main.rs` is intentionally tiny — its only job is to bring up the
//! raylib window + GL context, build the [`gfx::RenderResources`] and
//! [`app::ClientApp`], and pump the per-frame `update()` → `draw()` cycle
//! through [`raylib::core::game_loop::run`] (which uses
//! `emscripten_set_main_loop` on web and a plain `while
//! !window_should_close` on native). Every other concern lives in a
//! dedicated module:
//!
//! | Module             | Concern                                            |
//! |--------------------|----------------------------------------------------|
//! | `state`            | `AppPhase` + scheduler slot IDs                     |
//! | `app`              | `ClientApp` struct + `update`/`dispatch_slot`      |
//! | `gfx`              | Render targets, blit helper, logical scaling       |
//! | `render`           | Shared draw helpers + phase render dispatch         |
//! | `gameplay`         | Live match tick + rendering                        |
//! | `scene_runtime`    | Data-driven menu scene (Luau via SceneHost) — the only menu path |
//! | `network`          | UDP connect, packet drain, reconciliation          |
//! | `cursor_atlas`     | Idle-twitch motion-track atlases (VFS-loaded, served to the scene) |
//! | `audio`            | rodio sinks for ambient + SFX                      |
//! | `font`             | Bitmap-font (CDSFont) renderer + provider trait    |
//! | `settings`         | Cross-platform K/V store (registry/file/localStorage) |
//! | `config_blob`      | Gzip framing for the original's `Config` REG_BINARY |
//! | `window_init`      | Initial window centering on the primary monitor    |
//! | `window_mode`      | Resize scaling + Alt+Enter borderless fullscreen   |
//!
//! The entire main menu (dials, poem, ruch, cursor, sub-screens) lives in
//! Luau under `paks/base_ui` and is driven through `scene_runtime`; Rust
//! keeps only rendering primitives + lifecycle hooks.

use std::sync::Arc;
use std::time::{SystemTime, UNIX_EPOCH};

use bulanci_core::assets::AssetFileSystem;

mod app;
mod asset;
mod atlas;
mod audio;
mod config_blob;
mod cursor_atlas;
mod font;
mod gameplay;
mod generated;
mod gfx;
mod intro;
mod network;
mod render;
mod scene_runtime;
mod settings;
mod state;
mod surface;
mod window_init;
mod window_mode;

use app::ClientApp;
use audio::AudioManager;
use gfx::RenderResources;
use state::AppPhase;

fn startup_random_seed() -> u64 {
    SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .map(|duration| duration.as_millis() as u64)
        .unwrap_or(1)
}

fn main() {
    let no_main_menu_bg = std::env::args().any(|arg| arg == "--no-main-menu-bg");
    fastrand::seed(startup_random_seed());

    // ---- raylib window + GL context. -------------------------------
    // Fixed 800×600 client area — the logical resolution everything is
    // authored against. On native, `gfx`/`window_mode` upscale to whatever
    // the OS window ends up being.
    let mut builder = raylib::init();
    builder
        .size(
            window_init::CLIENT_WIDTH as i32,
            window_init::CLIENT_HEIGHT as i32,
        )
        .title("OpenBulanci");
    // Native only:
    //  * RESIZABLE so Alt+Enter borderless and manual resizes feed the
    //    logical-scale math.
    //  * V-sync for normal frames; GLFW temporarily disables it during modal
    //    window resize (see vendored win32_window.c) so presents don't block
    //    ~500 ms in the Intel ICD while DWM throttles compositing.
    //
    // On web we deliberately keep the canvas drawing buffer fixed at 800×600
    // (the desktop default resolution) and let CSS scale it to fit the page;
    // FLAG_WINDOW_RESIZABLE would make emscripten stretch the framebuffer to
    // the full browser window instead. See `web/index.html`.
    #[cfg(not(target_arch = "wasm32"))]
    {
        builder.resizable().vsync();
    }
    let (mut rl, thread) = builder.build();
    rl.set_target_fps(60);
    // Esc is a menu/dialog navigation key in this game, never an app-quit
    // shortcut — disable raylib's default "Esc closes the window".
    rl.set_exit_key(None);

    // ---- Settings + window placement (mirrors CDSApp_ctor +
    //      CDSApp_InitDirectDraw from the original) -----------------
    //
    // The original game reads `HKLM\Software\SleepTeam\Bulanci\Windowed`
    // as a REG_DWORD during construction (`CDSApp_ctor @ 0x0042b170`).
    // We preserve the same value name: non-zero starts/restores the
    // native framed window, zero starts borderless desktop-sized
    // fullscreen. The latter never changes display resolution.
    let settings_store = settings::default_store();
    let stored_windowed = settings_store.get_u32(settings::KEY_WINDOWED);
    let windowed = stored_windowed.map(|v| v != 0).unwrap_or(true);
    if windowed {
        window_init::center_window_on_primary_monitor(&mut rl);
    }
    if stored_windowed.is_none() {
        settings_store.set_u32(settings::KEY_WINDOWED, 1);
    }
    let mut window_mode = window_mode::WindowMode::new(&mut rl, settings_store, windowed);

    // ---- Offscreen render targets + poem shader. -------------------
    let mut render_res = RenderResources::new(&mut rl, &thread);

    // ---- Asset VFS, audio, and the app state container. ------------
    let vfs = Arc::new(AssetFileSystem::load().expect("VFS failed to load"));
    let audio = AudioManager::new();
    let mut app = ClientApp::new(&mut rl, &thread, "127.0.0.1:34568", vfs, audio, no_main_menu_bg);
    if no_main_menu_bg {
        eprintln!("[audio] main-menu background music disabled by --no-main-menu-bg");
    }

    // CGunMouse is a software-rendered cursor — hide the OS cursor so the
    // user only sees the red sniper-scope across the menu/intro phases.
    rl.hide_cursor();
    let mut current_mouse_visible = false;

    raylib::core::game_loop::run(rl, thread, 60, move |rl, thread| {
        window_mode.update(rl);
        app.update(rl, thread);
        app.draw(rl, thread, &mut render_res);

        // Toggle OS cursor visibility based on phase: hidden in menu /
        // intro (where CGunMouse draws), visible in connecting / gameplay.
        // On Windows we also reveal it when the pointer leaves the client
        // area so the native title bar / borders stay usable.
        let target_mouse_visible = match app.phase {
            AppPhase::Menu | AppPhase::Intro => !audio::is_cursor_in_client_area(),
            _ => true,
        };
        if current_mouse_visible != target_mouse_visible {
            if target_mouse_visible {
                rl.show_cursor();
            } else {
                rl.hide_cursor();
            }
            current_mouse_visible = target_mouse_visible;
        }
    });
}
