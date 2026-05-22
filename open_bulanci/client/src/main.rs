//! OpenBulanci graphical client binary.
//!
//! `main.rs` is intentionally tiny — its only job is to wire macroquad's
//! window into [`app::ClientApp`] and pump the per-frame `update()` →
//! `draw()` cycle. Every other concern lives in a dedicated module:
//!
//! | Module             | Concern                                            |
//! |--------------------|----------------------------------------------------|
//! | `state`            | Enums (`AppPhase`, `SubScreen`, `MenuBtn`) + slot IDs |
//! | `app`              | `ClientApp` struct + `update`/`dispatch_slot`      |
//! | `render`           | Shared draw helpers + menu/connecting chrome       |
//! | `menu`             | Left-side dial buttons + keyboard shortcuts        |
//! | `startgame1`       | `CStartGame1` host/join form                       |
//! | `history_screen`   | `CHistoryDlg` paged poem viewer                    |
//! | `exit_dialog`      | `CExitDlg` Y/N confirmation                        |
//! | `gameplay`         | Live match tick + rendering                        |
//! | `network`          | UDP connect, packet drain, reconciliation          |
//! | `cursor`           | `CGunMouse` software cursor                        |
//! | `cursor_atlas`     | Idle-twitch motion-track atlases (VFS-loaded)      |
//! | `ruch`             | `CRuch` phosphor scanline actors                   |
//! | `poem`             | Poem loading / alignment parsing                   |
//! | `audio`            | rodio sinks for ambient + SFX                      |
//! | `font`             | Bitmap-font (CDSFont) renderer + provider trait    |
//! | `settings`         | Cross-platform K/V store (registry/file/localStorage) |
//! | `config_blob`      | Gzip framing for the original's `Config` REG_BINARY |
//! | `window_init`      | Initial window centering on the primary monitor    |
//! | `window_mode`      | Resize scaling + Alt+Enter borderless fullscreen   |

use std::sync::Arc;
use macroquad::prelude::*;

use bulanci_core::assets::AssetFileSystem;

mod app;
mod atlas;
mod audio;
mod config_blob;
mod cursor;
mod cursor_atlas;
mod exit_dialog;
mod font;
mod gameplay;
mod history_screen;
mod intro;
mod menu;
mod msvcrt_rand;
mod network;
mod poem;
mod render;
mod ruch;
mod settings;
mod startgame1;
mod state;
mod widget;
mod window_init;
mod window_mode;

use app::ClientApp;
use audio::AudioManager;
use state::AppPhase;

fn window_conf() -> Conf {
    Conf {
        window_title: "OpenBulanci".to_string(),
        window_width: window_init::CLIENT_WIDTH as i32,
        window_height: window_init::CLIENT_HEIGHT as i32,
        ..Default::default()
    }
}

#[macroquad::main(window_conf)]
async fn main() {
    let no_main_menu_bg = std::env::args().any(|arg| arg == "--no-main-menu-bg");

    // ---- Settings + window placement (mirrors CDSApp_ctor +
    //      CDSApp_InitDirectDraw from the original) -----------------
    //
    // The original game reads `HKLM\Software\SleepTeam\Bulanci\Windowed`
    // as a REG_DWORD during construction (`CDSApp_ctor @ 0x0042b170`).
    // We preserve the same value name: non-zero starts/restores the
    // native framed window, zero starts borderless desktop-sized
    // fullscreen. The latter never changes display resolution.
    //
    // The storage backend is selected at compile time: registry on
    // Windows, JSON on Linux/macOS, localStorage on WASM. See
    // `settings.rs` for the selection logic and key compatibility
    // notes with the original game's binary.
    let settings_store = settings::default_store();
    let stored_windowed = settings_store.get_u32(settings::KEY_WINDOWED);
    // A missing value implies "first run on this host" — OpenBulanci
    // defaults to a normal resizable window.
    let windowed = stored_windowed.map(|v| v != 0).unwrap_or(true);
    if windowed {
        window_init::center_window_on_primary_monitor();
    }
    // Persist a 1 if the value was missing so subsequent runs (and any
    // future Alt+Enter toggle) see a stable baseline. The write is skipped
    // when the value already exists, so we don't churn the registry / file
    // / localStorage on every launch.
    if stored_windowed.is_none() {
        settings_store.set_u32(settings::KEY_WINDOWED, 1);
    }
    let mut window_mode = window_mode::WindowMode::new(settings_store, windowed);

    let vfs = Arc::new(AssetFileSystem::load().expect("VFS failed to load"));
    let audio = AudioManager::new();
    let mut app = ClientApp::new("127.0.0.1:34568", vfs, audio, no_main_menu_bg);
    if no_main_menu_bg {
        eprintln!("[audio] main-menu background music disabled by --no-main-menu-bg");
    }

    // CGunMouse is a software-rendered cursor — the OS cursor must be
    // hidden so the user only sees the red sniper-scope.
    show_mouse(false);
    // Macroquad only cancels the native close if this is armed before the
    // close event arrives; `is_quit_requested()` is just the latched request.
    prevent_quit();
    let mut current_mouse_visible = false;

    loop {
        window_mode.update();
        if is_quit_requested() {
            app.begin_retail_exit();
        }
        app.update();
        app.draw();
        // Toggle OS cursor visibility based on phase: hidden in menu
        // (where CGunMouse draws), visible in connecting/gameplay screens.
        // On Windows, the native cursor must also show when hovering over
        // the native topbar or borders (i.e. when not in client area).
        let target_mouse_visible = match app.phase {
            AppPhase::Menu => !audio::is_cursor_in_client_area(),
            AppPhase::Intro => !audio::is_cursor_in_client_area(),
            _ => true,
        };
        if current_mouse_visible != target_mouse_visible {
            show_mouse(target_mouse_visible);
            current_mouse_visible = target_mouse_visible;
        }
        next_frame().await;
    }
}
