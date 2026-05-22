//! Window mode control and logical 800x600 scaling.
//!
//! Alt+Enter is global: it toggles between a normal resizable native window
//! and miniquad's desktop-sized borderless fullscreen (`WS_POPUP` on Windows).
//! This never changes display resolution.

use macroquad::prelude::*;
use macroquad::window::miniquad;

use crate::settings::{SettingsStore, KEY_WINDOWED};
use crate::window_init::{CLIENT_HEIGHT, CLIENT_WIDTH};

pub struct WindowMode {
    settings: Box<dyn SettingsStore>,
    is_windowed: bool,
    last_windowed_position: Option<(u32, u32)>,
    last_windowed_size: (f32, f32),
    pending_restore: bool,
}

impl WindowMode {
    pub fn new(settings: Box<dyn SettingsStore>, windowed: bool) -> Self {
        let mode = WindowMode {
            settings,
            is_windowed: windowed,
            last_windowed_position: current_window_position(),
            last_windowed_size: (CLIENT_WIDTH as f32, CLIENT_HEIGHT as f32),
            pending_restore: false,
        };
        if !windowed {
            set_fullscreen(true);
        }
        mode
    }

    pub fn update(&mut self) {
        let current_size = (screen_width().max(1.0), screen_height().max(1.0));

        if self.pending_restore {
            if let Some((x, y)) = self.last_windowed_position {
                miniquad::window::set_window_position(x, y);
            }
            request_new_screen_size(self.last_windowed_size.0, self.last_windowed_size.1);
            self.pending_restore = false;
        } else if self.is_windowed {
            self.last_windowed_position = current_window_position();
            self.last_windowed_size = current_size;
        }

        if alt_enter_pressed() {
            self.toggle();
        }
    }

    fn toggle(&mut self) {
        if self.is_windowed {
            self.last_windowed_position = current_window_position();
            self.last_windowed_size = (screen_width().max(1.0), screen_height().max(1.0));
            self.is_windowed = false;
            self.settings.set_u32(KEY_WINDOWED, 0);
            set_fullscreen(true);
        } else {
            self.is_windowed = true;
            self.settings.set_u32(KEY_WINDOWED, 1);
            set_fullscreen(false);
            self.pending_restore = true;
        }
    }
}

pub fn logical_mouse_position() -> (f32, f32) {
    let (mx, my) = mouse_position();
    let (sx, sy) = logical_scale();
    (mx / sx, my / sy)
}

pub fn begin_logical_frame(render_target: RenderTarget) {
    let mut camera = Camera2D::from_display_rect(Rect::new(
        0.0,
        0.0,
        CLIENT_WIDTH as f32,
        CLIENT_HEIGHT as f32,
    ));
    camera.render_target = Some(render_target);
    set_camera(&camera);
}

pub fn end_logical_frame(texture: &Texture2D) {
    set_default_camera();
    clear_background(BLACK);
    draw_texture_ex(
        texture,
        0.0,
        0.0,
        WHITE,
        DrawTextureParams {
            dest_size: Some(vec2(screen_width(), screen_height())),
            flip_y: true,
            ..Default::default()
        },
    );
}

fn logical_scale() -> (f32, f32) {
    (
        screen_width().max(1.0) / CLIENT_WIDTH as f32,
        screen_height().max(1.0) / CLIENT_HEIGHT as f32,
    )
}

fn alt_enter_pressed() -> bool {
    let alt_down = is_key_down(KeyCode::LeftAlt) || is_key_down(KeyCode::RightAlt);
    alt_down && (is_key_pressed(KeyCode::Enter) || is_key_pressed(KeyCode::KpEnter))
}

#[cfg(any(target_os = "windows", target_os = "linux"))]
fn current_window_position() -> Option<(u32, u32)> {
    Some(miniquad::window::get_window_position())
}

#[cfg(not(any(target_os = "windows", target_os = "linux")))]
fn current_window_position() -> Option<(u32, u32)> {
    None
}
