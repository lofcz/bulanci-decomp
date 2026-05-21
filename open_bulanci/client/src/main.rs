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

use std::sync::Arc;
use macroquad::prelude::*;

use bulanci_core::assets::AssetFileSystem;

mod app;
mod atlas;
mod audio;
mod cursor;
mod cursor_atlas;
mod exit_dialog;
mod font;
mod gameplay;
mod history_screen;
mod menu;
mod msvcrt_rand;
mod network;
mod poem;
mod render;
mod ruch;
mod startgame1;
mod state;

use app::ClientApp;
use audio::AudioManager;
use state::AppPhase;

fn window_conf() -> Conf {
    Conf {
        window_title: "OpenBulanci".to_string(),
        window_width: 800,
        window_height: 600,
        ..Default::default()
    }
}

#[macroquad::main(window_conf)]
async fn main() {
    let vfs = Arc::new(AssetFileSystem::load().expect("VFS failed to load"));
    let audio = AudioManager::new();
    let mut app = ClientApp::new("127.0.0.1:34568", vfs, audio);

    // CGunMouse is a software-rendered cursor — the OS cursor must be
    // hidden so the user only sees the red sniper-scope.
    show_mouse(false);
    let mut current_mouse_visible = false;

    loop {
        app.update();
        app.draw();
        // Toggle OS cursor visibility based on phase: hidden in menu
        // (where CGunMouse draws), visible in connecting/gameplay screens.
        // On Windows, the native cursor must also show when hovering over
        // the native topbar or borders (i.e. when not in client area).
        let target_mouse_visible = match app.phase {
            AppPhase::Menu => !audio::is_cursor_in_client_area(),
            _ => true,
        };
        if current_mouse_visible != target_mouse_visible {
            show_mouse(target_mouse_visible);
            current_mouse_visible = target_mouse_visible;
        }
        next_frame().await;
    }
}
