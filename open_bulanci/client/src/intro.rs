//! Startup `CAdvertising` splash shown before `CMenu` is constructed.
//!
//! Original path:
//! `CBulanci::OnEvent(0xf7)` builds `CAdvertising`, loads resource
//! `0x1013a`, runs it modally, then enters the main menu via event `0xcc`.

use macroquad::prelude::*;

use crate::app::{register_menu_slots, ClientApp};
use crate::ruch::Ruch;
use crate::state::{AppPhase, RUCH_SLOTS};

const INTRO_DURATION_MS: u64 = 4_000;

impl ClientApp {
    /// Mirrors `CAdvertising`: dismiss after the 4000ms timer, left mouse
    /// button down, or any key not eaten by the base window handler.
    pub(crate) fn update_intro_input(&mut self, now_ms: u64) {
        let should_dismiss = now_ms.saturating_sub(self.intro_started_ms) >= INTRO_DURATION_MS
            || is_mouse_button_pressed(MouseButton::Left)
            || get_last_key_pressed().is_some();

        if should_dismiss {
            self.enter_menu_from_intro(now_ms);
        }
    }

    /// Deliver the post-splash menu event. This is where the original
    /// constructs `CMenu`, so all menu-owned timers and one-shot effects
    /// start here rather than at process boot.
    fn enter_menu_from_intro(&mut self, now_ms: u64) {
        if self.menu_started {
            return;
        }
        self.menu_started = true;
        self.phase = AppPhase::Menu;
        self.last_frame_ms = now_ms;

        register_menu_slots(&mut self.scheduler, now_ms);
        for &slot in &RUCH_SLOTS {
            self.scheduler.register(slot, Ruch::random_hidden_delay(), true, now_ms);
        }

        // CMenu ctor immediately clicks Start, playing slot 0x1b exactly
        // when the menu appears, not while the CAdvertising modal is live.
        self.play_sfx("audio/sfx_start.wav");
        self.on_day_night_tick();
    }

    /// CAdvertising ctor adds a full-window white CBlackView, then centers
    /// bitmap resource 0x1013a on it.
    pub(crate) fn draw_intro(&mut self) {
        clear_background(WHITE);
        let splash = self.get_texture("images/intro_splash.jpg");
        let x = ((800.0 - splash.width()) * 0.5).floor();
        let y = ((600.0 - splash.height()) * 0.5).floor();
        draw_texture(&splash, x, y, WHITE);
    }
}
