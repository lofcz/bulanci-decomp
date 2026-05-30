//! Window mode control.
//!
//! Alt+Enter is global: it toggles between a normal resizable native window
//! and raylib's desktop-sized borderless fullscreen. This never changes the
//! display resolution. The logical 800×600 → window upscale lives in
//! [`crate::gfx`] (render-target blit) and [`crate::gfx::logical_mouse_position`].

use raylib::prelude::*;

use crate::settings::{SettingsStore, KEY_WINDOWED};
use crate::window_init::{CLIENT_HEIGHT, CLIENT_WIDTH};

pub struct WindowMode {
    settings: Box<dyn SettingsStore>,
    is_windowed: bool,
    last_windowed_position: (i32, i32),
    last_windowed_size: (i32, i32),
}

impl WindowMode {
    pub fn new(rl: &mut RaylibHandle, settings: Box<dyn SettingsStore>, windowed: bool) -> Self {
        let pos = rl.get_window_position();
        let mode = WindowMode {
            settings,
            is_windowed: windowed,
            last_windowed_position: (pos.x as i32, pos.y as i32),
            last_windowed_size: (CLIENT_WIDTH as i32, CLIENT_HEIGHT as i32),
        };
        if !windowed {
            rl.toggle_borderless_windowed();
        }
        mode
    }

    pub fn update(&mut self, rl: &mut RaylibHandle) {
        // Track the live windowed geometry so a later Alt+Enter restore
        // lands the window back where the user left it.
        if self.is_windowed && !rl.is_window_fullscreen() {
            let pos = rl.get_window_position();
            self.last_windowed_position = (pos.x as i32, pos.y as i32);
            self.last_windowed_size = (
                rl.get_screen_width().max(1),
                rl.get_screen_height().max(1),
            );
        }
        if alt_enter_pressed(rl) {
            self.toggle(rl);
        }
    }

    fn toggle(&mut self, rl: &mut RaylibHandle) {
        if self.is_windowed {
            let pos = rl.get_window_position();
            self.last_windowed_position = (pos.x as i32, pos.y as i32);
            self.last_windowed_size = (
                rl.get_screen_width().max(1),
                rl.get_screen_height().max(1),
            );
            self.is_windowed = false;
            self.settings.set_u32(KEY_WINDOWED, 0);
            rl.toggle_borderless_windowed();
        } else {
            self.is_windowed = true;
            self.settings.set_u32(KEY_WINDOWED, 1);
            rl.toggle_borderless_windowed();
            rl.set_window_size(self.last_windowed_size.0, self.last_windowed_size.1);
            rl.set_window_position(
                self.last_windowed_position.0,
                self.last_windowed_position.1,
            );
        }
    }
}

fn alt_enter_pressed(rl: &RaylibHandle) -> bool {
    use raylib::consts::KeyboardKey::*;
    let alt = rl.is_key_down(KEY_LEFT_ALT) || rl.is_key_down(KEY_RIGHT_ALT);
    alt && (rl.is_key_pressed(KEY_ENTER) || rl.is_key_pressed(KEY_KP_ENTER))
}
